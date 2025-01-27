#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ����� ��������
typedef struct {
    int opcode;
    char name[10];
} Opcode;

typedef struct {
    int regNumber;
    char name[10];
} Register;

// ����� ����� Opcode
Opcode opcodes[] = {
    {0, "add"}, {1, "sub"}, {2, "mac"}, {3, "and"},
    {4, "or"}, {5, "xor"}, {6, "sll"}, {7, "sra"},
    {8, "srl"}, {9, "beq"}, {10, "bne"}, {11, "blt"},
    {12, "bgt"}, {13, "ble"}, {14, "bge"}, {15, "jal"},
    {16, "lw"},{17, "sw"},{18, "reti"},{19, "in"},
    {20, "out"},{21, "halt"},
};

// ����� ����� Register
Register registers[] = {
    {0, "$zero"}, {1, "$imm1"}, {2, "$imm2"}, {3, "$v0"},
    {4, "$a0"}, {5, "$a1"}, {6, "$a2"}, {7, "$t0"},
    {8, "$t1"}, {9, "$t2"}, {10, "$s0"}, {11, "$s1"},
    {12, "$s2"}, {13, "$gp"}, {14, "$sp"}, {15, "$ra"}
};

#define OPCODE_SIZE (sizeof(opcodes) / sizeof(opcodes[0]))
#define REGISTER_SIZE (sizeof(registers) / sizeof(registers[0]))

// storing headers+ adresses
typedef struct {
    char label[50];
    int line_index;
} LabelInfo;

// ������ ��������
int find_max_address(const char* input_filename);
void remove_comments(const char* input_filename, const char* output_filename);
void headers_handler(const char* filename, LabelInfo* labels, int* label_count);
void read_and_process_file(const char* input_filename, const char* output_filename_IMEMIN, const char* output_filename_DMEMIN);
void convert_asm_to_numbers(const char* line, FILE* outputFile_IMEMIN, int** memory, int* memory_size, int* last_written_index);
int get_register_value(const char* reg_name);
void delete_file(const char* filename);

/*
cmd: asm.exe program.asm imemin.txt dmemin.txt
*/

int main(int argc, char* argv[]) {
    //const char input1_filename[] = "C:\\Users\\Shachar\\Desktop\\C\\Comp.Org.Project\\fib.asm";
    const char temp_file[] = "temp.txt";
    //const char output_filename_IMEMIN[] = "C:\\Users\\Shachar\\Desktop\\C\\Comp.Org.Project\\imemin1.txt";
    //const char output_filename_DMEMIN[] = "C:\\Users\\Shachar\\Desktop\\C\\Comp.Org.Project\\dmemin1.txt";
    if (argc < 2) {
        argv[1]= "C:\\Users\\Shachar\\Desktop\\C\\Comp.Org.Project\\mulmat.txt";
        argv[2]= "C:\\Users\\Shachar\\Desktop\\C\\Comp.Org.Project\\imemin.txt";
        argv[3]= "C:\\Users\\Shachar\\Desktop\\C\\Comp.Org.Project\\dmemin.txt";
    }


    remove_comments(argv[1], temp_file); //copy and clean hashtags in a new file
    //Definitions for headers handler
    LabelInfo labels[100];
    int label_count = 0;
    headers_handler(temp_file, labels, &label_count);//clean_adresses_and_save_values
    read_and_process_file(temp_file, argv[2], argv[3]); //remove branches headers in file
    delete_file(temp_file);// DELETE COPYFILE
    //read_and_process_file(input1_filename, output_filename_IMEMIN, output_filename_DMEMIN);//�� �� ������ �� ����� �� �������� ��� ��� ���� ����� ����

    return 0;
}



// ������� ������ ������ ��������� �����
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

            // ����� ���������� �� ������ 0x/0X
            if (sscanf(line, ".word 0x%x", &address) == 1 || sscanf(line, ".word 0X%x", &address) == 1) {
                //printf("Hexadecimal address: 0x%x\n", address); // ���� ������ ����� ����� ���
                if ((int)address > max_address) {
                    max_address = (int)address;
                }
            }
            // ����� �������
            else if (sscanf(line, ".word %d", &address) == 1) {
                //printf("Decimal address: %d\n", address); // ���� ������ ����� ����� ���
                if ((int)address > max_address) {
                    max_address = (int)address;
                }
            }
        }
    }



    fclose(inputFile);
    return max_address + 1; // ����� 1 ��� ����� �� ������ ������ �����
}

//REMOVE_COMMENTS
void remove_comments(const char* input_filename, const char* output_filename) {
    FILE* inputFile = fopen(input_filename, "r");
    FILE* outputFile = fopen(output_filename, "w");

    if (inputFile == NULL || outputFile == NULL) {
        if (inputFile) fclose(inputFile);
        if (outputFile) fclose(outputFile);
        perror("Error opening files");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), inputFile)) {
        char* comment_start = strchr(line, '#');
        if (comment_start != NULL) {
            *comment_start = '\0'; // ���� ����� ���� �����
        }
        // ���� ������ ������� ���� �����
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == ' ' || line[len - 1] == '\t' || line[len - 1] == '\n')) {
            line[--len] = '\0';
        }
        fprintf(outputFile, "%s\n", line); // ����� ����� ���� �� ����� ����
    }

    fclose(inputFile);
    fclose(outputFile);
}
//HEADERS_HANDLER
void headers_handler(const char* filename, LabelInfo* labels, int* label_count) {
    FILE* inputFile = fopen(filename, "r+"), * tempFile;

    if (inputFile == NULL) {
        perror("Error opening file");
        return;
    }

    tempFile = tmpfile();
    if (tempFile == NULL) {
        fclose(inputFile);
        perror("Error creating temporary file");
        return;
    }

    char line[256];
    int current_line = 0;
    *label_count = 0;

    while (fgets(line, sizeof(line), inputFile)) {
        char* comment_start = strchr(line, '#');
        if (comment_start != NULL) {
            *comment_start = '\0'; // ���� ����� ���� �����
        }

        // ���� ������ ������� ���� �����
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == ' ' || line[len - 1] == '\t' || line[len - 1] == '\n')) {
            line[--len] = '\0';
        }

        // ����� �� ����� ��� �����
        if (strchr(line, ':') != NULL) {
            char* colon = strchr(line, ':');
            *colon = '\0';
            strcpy(labels[*label_count].label, line);
            labels[*label_count].line_index = current_line;
            (*label_count)++;
        }
        else if (strlen(line) > 0) {
            fprintf(tempFile, "%s\n", line);
            current_line++;
        }
    }

    // ����� ����� ���� ����� ������
    rewind(inputFile);
    rewind(tempFile);
    while (fgets(line, sizeof(line), tempFile)) {
        fputs(line, inputFile);
    }

    // ����� ����� ������ �� ����
    fflush(inputFile);
    _chsize(_fileno(inputFile), ftell(inputFile));

    fclose(tempFile);

    // ����� ������� ������ ����
    rewind(inputFile);
    tempFile = tmpfile();
    if (tempFile == NULL) {
        fclose(inputFile);
        perror("Error creating temporary file");
        return;
    }

    while (fgets(line, sizeof(line), inputFile)) {
        // ����� ����� �� ��� ���� ������ ������� ������ �����
        char* start = line;
        while (*start == ' ' || *start == '\t') {
            start++;
        }

        // ����� ����� ��� ������ ������� ���� ������ line
        if (start != line) {
            memmove(line, start, strlen(start) + 1);
        }

        for (int i = 0; i < *label_count; i++) {
            char* label_pos;
            while ((label_pos = strstr(line, labels[i].label)) != NULL) {
                char updated_line[256];
                snprintf(updated_line, sizeof(updated_line), "%.*s%d%s",
                    (int)(label_pos - line), line,
                    labels[i].line_index, label_pos + strlen(labels[i].label));
                strcpy(line, updated_line);
            }
        }
        fprintf(tempFile, "%s", line);
    }

    // ����� ����� ������� ���� ����� ������
    rewind(inputFile);
    rewind(tempFile);
    while (fgets(line, sizeof(line), tempFile)) {
        fputs(line, inputFile);
    }

    fflush(inputFile);
    _chsize(_fileno(inputFile), ftell(inputFile));

    fclose(inputFile);
    fclose(tempFile);
}
// ������� ������ ���� �������
void read_and_process_file(const char* input_filename, const char* output_filename_IMEMIN, const char* output_filename_DMEMIN) {
    FILE* inputFile, * outputFile_IMEMIN, * outputFile_DMEMIN;
    char line[256];

    // ����� ���� ������� ��� ������ ��������� �����
    int MEMORY_SIZE = find_max_address(input_filename);
    if (MEMORY_SIZE <= 0) {
        fprintf(stderr, "Error: Invalid memory size.\n");
    }

    // ����� ������ ������
    int* memory = calloc(MEMORY_SIZE, sizeof(int));
    if (memory == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory.\n");
        return;
    }

    int last_written_index = -1; // ����� ������ ������ ����� ������

    inputFile = fopen(input_filename, "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        free(memory);
        return;
    }

    outputFile_IMEMIN = fopen(output_filename_IMEMIN, "w");
    if (outputFile_IMEMIN == NULL) {
        printf("�� ���� ����� �� ���� ���� IMEMIN.\n");
        fclose(inputFile);
        free(memory);
        return;
    }

    outputFile_DMEMIN = fopen(output_filename_DMEMIN, "w");
    if (outputFile_DMEMIN == NULL) {
        printf("�� ���� ����� �� ���� ���� DMEMIN.\n");
        fclose(inputFile);
        fclose(outputFile_IMEMIN);
        free(memory);
        return;
    }

    // ����� ����� ���� ��� ���� �������
    while (fgets(line, sizeof(line), inputFile)) {
        line[strcspn(line, "\r\n")] = '\0'; // ���� ���� ��� ����
        convert_asm_to_numbers(line, outputFile_IMEMIN, &memory, &MEMORY_SIZE, &last_written_index);
    }

    // ����� ���� ������� ����� DMEMIN
    for (int i = 0; i < MEMORY_SIZE; i++) {
        fprintf(outputFile_DMEMIN, "%08X\n", memory[i]);
    }

    fclose(inputFile);
    fclose(outputFile_IMEMIN);
    fclose(outputFile_DMEMIN);

    // ����� �������
    free(memory);
    printf("File processing completed.\n");
}

// ������� ����� ����� ������ �������
void convert_asm_to_numbers(const char* line, FILE* outputFile_IMEMIN, int** memory, int* memory_size, int* last_written_index) {
    char command[16], rd[16], rs[16], rt[16], rm[16];
    int imm1, imm2, opcode_value = -1;

   // ������ ����� �� ���� �������
    if (strncmp(line, ".word", 5) == 0) {
        unsigned int address = 0, data = 0;

        // ����� �� ����� ��� ���� ��������
        if ((sscanf(line, ".word %u 0x%x", &address, &data) == 2) ||      // ����� �������, ������ �����������
            (sscanf(line, ".word 0x%x 0x%x", &address, &data) == 2) ||    // ����� ����������, ������ �����������
            (sscanf(line, ".word 0x%x %u", &address, &data) == 2) ||      // ����� ����������, ������ ��������
            (sscanf(line, ".word %u %u", &address, &data) == 2)) {        // ����� �������, ������ ��������

            //// ����� �����
            //printf("Parsed address: %u (hex: 0x%x), data: %u (hex: 0x%x)\n",
            //    address, address, data, data);

            // ����� �� ������ ����� �������
            if (address < *memory_size) {
                (*memory)[address] = data; // ����� �������
                //printf("Memory updated at [%u]: %u (hex: 0x%x)\n", address, data, data);

                // ����� ������� ������
                if ((int)address > *last_written_index) {
                    *last_written_index = address;
                }
            }
            else {
                //printf("Address %u is out of memory bounds.\n", address);
            }
        }
        else {
            //printf("Error: Invalid format in line: %s\n", line);
        }
    }




    // ������ ����� ���� ������
    if (sscanf(line, "%s %[^,], %[^,], %[^,], %[^,], %d, %d", command, rd, rs, rt, rm, &imm1, &imm2) >= 7) {
        for (int i = 0; i < OPCODE_SIZE; i++) {
            if (strcmp(opcodes[i].name, command) == 0) {
                opcode_value = opcodes[i].opcode;
                break;
            }
        }

        if (opcode_value != -1) {
            // ����� ������ �� ��������� ���� ������
            int rd_val = get_register_value(rd);
            int rs_val = get_register_value(rs);
            int rt_val = get_register_value(rt);
            int rm_val = get_register_value(rm);

            // ����� �� �� ��������� ������
            if (rd_val != -1 && rs_val != -1 && rt_val != -1 && rm_val != -1) {
                // ����� ����� ����������� ������
                imm1 &= 0xFFF; // ����� IMM1 �-12 �����
                imm2 &= 0xFFF; // ����� IMM2 �-12 �����
                // ����� ������ ���������
                fprintf(outputFile_IMEMIN, "%02X%01X%01X%01X%01X%03X%03X\n",
                    opcode_value,
                    rd_val, rs_val, rt_val, rm_val,
                    imm1, imm2
                );
            }
            else {
                fprintf(stderr, "Error: Invalid register in line: %s\n", line);
            }
        }
    }
}

// ������� ������ ��� ������
int get_register_value(const char* reg_name) {
    for (int i = 0; i < REGISTER_SIZE; i++) {
        if (strcmp(registers[i].name, reg_name) == 0) {
            return registers[i].regNumber;
        }
    }
    return -1; // ��� �� ���� �� ������� �� ����
}
//����� ���� �����
void delete_file(const char* filename) {
    // ������ ����� �� �����
    if (remove(filename) == 0) {
        printf("File '%s' deleted successfully.\n", filename);
    }
    else {
        perror("Error deleting file");
    }
}
