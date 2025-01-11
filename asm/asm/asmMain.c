#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// הגדרת המילונים
typedef struct {
    int opcode;
    char name[10];
} Opcode;

typedef struct {
    int regNumber;
    char name[10];
} Register;

// הגדרת מילון Opcode
Opcode opcodes[] = {
    {0, "add"}, {1, "sub"}, {2, "mac"}, {3, "and"},
    {4, "or"}, {5, "xor"}, {6, "sll"}, {7, "sra"},
    {8, "srl"}, {9, "beq"}, {10, "bne"}, {11, "blt"},
    {12, "bgt"}, {13, "ble"}, {14, "bge"}, {15, "jal"}
};

// הגדרת מילון Register
Register registers[] = {
    {0, "$zero"}, {1, "$imm1"}, {2, "$imm2"}, {3, "$v0"},
    {4, "$a0"}, {5, "$a1"}, {6, "$a2"}, {7, "$t0"},
    {8, "$t1"}, {9, "$t2"}, {10, "$s0"}, {11, "$s1"},
    {12, "$s2"}, {13, "$gp"}, {14, "$sp"}, {15, "$ra"}
};

#define OPCODE_SIZE (sizeof(opcodes) / sizeof(opcodes[0]))
#define REGISTER_SIZE (sizeof(registers) / sizeof(registers[0]))

// הכרזות פונקציות
int find_max_address(const char* input_filename);
void read_and_process_file(const char* input_filename, const char* output_filename_IMEMIN, const char* output_filename_DMEMIN);
void convert_asm_to_numbers(const char* line, FILE* outputFile_IMEMIN, int** memory, int* memory_size, int* last_written_index);
int get_register_value(const char* reg_name);

int main() {
    char* inargs[] = { "program.asm ","imemin.txt ","dmemin.txt " };
    const char input_filename[] = inargs[0];
    const char output_filename_IMEMIN[] = inargs[1];
    const char output_filename_DMEMIN[] = inargs[2];

    read_and_process_file(input_filename, output_filename_IMEMIN, output_filename_DMEMIN);

    return 0;
}

// פונקציה למציאת הכתובת המקסימלית בקובץ
int find_max_address(const char* input_filename) {
    FILE* inputFile = fopen(input_filename, "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return -1;
    }

    char line[256];
    int max_address = -1;

    while (fgets(line, sizeof(line), inputFile)) {
        if (strncmp(line, ".word", 5) == 0) {
            unsigned int address;
            if (sscanf(line, ".word %x", &address) == 1 || sscanf(line, ".word %d", &address) == 1) {
                if ((int)address > max_address) {
                    max_address = (int)address;
                }
            }
        }
    }

    fclose(inputFile);
    return max_address + 1; // הוספת 1 כדי לכלול את הכתובת הגבוהה ביותר
}

// פונקציה לקריאת קובץ ועיבודו
void read_and_process_file(const char* input_filename, const char* output_filename_IMEMIN, const char* output_filename_DMEMIN) {
    FILE* inputFile, * outputFile_IMEMIN, * outputFile_DMEMIN;
    char line[256];

    // מציאת גודל הזיכרון לפי הכתובת המקסימלית בקובץ
    int MEMORY_SIZE = find_max_address(input_filename);
    if (MEMORY_SIZE <= 0) {
        fprintf(stderr, "Error: Invalid memory size.\n");
        return;
    }

    // הקצאת זיכרון דינמית
    int* memory = calloc(MEMORY_SIZE, sizeof(int));
    if (memory == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory.\n");
        return;
    }

    int last_written_index = -1; // שמירת הכתובת הגבוהה ביותר שנכתבה

    inputFile = fopen(input_filename, "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        free(memory);
        return;
    }

    outputFile_IMEMIN = fopen(output_filename_IMEMIN, "w");
    if (outputFile_IMEMIN == NULL) {
        printf("לא ניתן לפתוח את קובץ הפלט IMEMIN.\n");
        fclose(inputFile);
        free(memory);
        return;
    }

    outputFile_DMEMIN = fopen(output_filename_DMEMIN, "w");
    if (outputFile_DMEMIN == NULL) {
        printf("לא ניתן לפתוח את קובץ הפלט DMEMIN.\n");
        fclose(inputFile);
        fclose(outputFile_IMEMIN);
        free(memory);
        return;
    }

    // קריאת הקובץ שורה אחר שורה ועיבודו
    while (fgets(line, sizeof(line), inputFile)) {
        line[strcspn(line, "\r\n")] = '\0'; // הסרת תווי סוף שורה
        convert_asm_to_numbers(line, outputFile_IMEMIN, &memory, &MEMORY_SIZE, &last_written_index);
    }

    // כתיבת תוכן הזיכרון לקובץ DMEMIN
    for (int i = 0; i < MEMORY_SIZE; i++) {
        fprintf(outputFile_DMEMIN, "%08X\n", memory[i]);
    }

    fclose(inputFile);
    fclose(outputFile_IMEMIN);
    fclose(outputFile_DMEMIN);

    // שחרור הזיכרון
    free(memory);
    printf("File processing completed.\n");
}

// פונקציה להמרת שורות אסמבלי לזיכרון
void convert_asm_to_numbers(const char* line, FILE* outputFile_IMEMIN, int** memory, int* memory_size, int* last_written_index) {
    char command[16], rd[16], rs[16], rt[16], rm[16];
    int imm1, imm2, opcode_value = -1;

    // ניסיון לפענח את שורת הזיכרון
    if (strncmp(line, ".word", 5) == 0) {
        unsigned int address, data;
        if (sscanf(line, ".word %x %x", &address, &data) == 2 || sscanf(line, ".word %d %d", &address, &data) == 2) {
            if (address < *memory_size) {
                (*memory)[address] = data;
                if ((int)address > *last_written_index) {
                    *last_written_index = address;
                }
            }
        }
        return;
    }

    // ניסיון לפענח שורת אסמבלי
    if (sscanf(line, "%s %[^,], %[^,], %[^,], %[^,], %x, %x", command, rd, rs, rt, rm, &imm1, &imm2) >= 7) {
        for (int i = 0; i < OPCODE_SIZE; i++) {
            if (strcmp(opcodes[i].name, command) == 0) {
                opcode_value = opcodes[i].opcode;
                break;
            }
        }

        if (opcode_value != -1) {
            // מציאת הערכים של הרגיסטרים מתוך המילון
            int rd_val = get_register_value(rd);
            int rs_val = get_register_value(rs);
            int rt_val = get_register_value(rt);
            int rm_val = get_register_value(rm);

            // בדיקה אם כל הרגיסטרים תקינים
            if (rd_val != -1 && rs_val != -1 && rt_val != -1 && rm_val != -1) {
                fprintf(outputFile_IMEMIN, "%02X%01X%01X%01X%01X%03X%03X\n",
                    opcode_value,
                    rd_val, rs_val, rt_val, rm_val,
                    imm1 & 0xFFF, // הגבלת IMM1 ל-12 ביטים
                    imm2 & 0xFFF  // הגבלת IMM2 ל-12 ביטים
                );
            }
            else {
                fprintf(stderr, "Error: Invalid register in line: %s\n", line);
            }
        }
    }
}

// פונקציה למציאת ערך רגיסטר
int get_register_value(const char* reg_name) {
    for (int i = 0; i < REGISTER_SIZE; i++) {
        if (strcmp(registers[i].name, reg_name) == 0) {
            return registers[i].regNumber;
        }
    }
    return -1; // ערך לא חוקי אם הרגיסטר לא נמצא
}
