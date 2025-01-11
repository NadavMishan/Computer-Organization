#ifndef LWSW_H
#define LWSW_H
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int memRead(int address, char* dmemout_txt) {
    FILE* file = fopen(dmemout_txt, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open file %s\n", dmemout_txt);
        return -1; // Error opening file
    }

    printf("File opened successfully: %s\n", dmemout_txt);

    char line[256]; // Buffer to read lines
    int current_line = 0;

    while (fgets(line, sizeof(line), file)) {
        if (current_line == address) {
            // Remove newline character at the end of the line, if present
            line[strcspn(line, "\n")] = '\0';
            printf("Line %d: %s\n", address, line);
            fclose(file);
            return 0; // Success
        }
        current_line++;
    }

    // If we reach here, the requested line doesn't exist
    fclose(file);
    printf("Line %d: 0000\n", address);
    return 0; // Success with default value
}



	


int memWrite(int address, int value, char* dmemout_txt) {
    // Open the file for reading
    FILE* file = fopen(dmemout_txt, "r");
    FILE* temp_file = fopen("temp.txt", "w");
    if (temp_file == NULL || file == NULL) {
        if (file != NULL) fclose(file);
        if (temp_file != NULL) fclose(temp_file);
        fprintf(stderr, "Error: Unable to open files for processing.\n");
        return -1;
    }

    char line[256];
    int current_line = 0;
    int written = 0;

    // Process the original file line by line
    while (fgets(line, sizeof(line), file)) {
        if (current_line == address) {
            // Write the new value in the target address
            fprintf(temp_file, "%04d\n", value);
            written = 1;
        }
        else {
            // Copy the existing line to the temp file
            fputs(line, temp_file);
        }
        current_line++;
    }

    // Fill missing lines with 0000 if address is beyond the last line
    while (current_line < address) {
        fprintf(temp_file, "0000\n");
        current_line++;
    }

    // Write the value at the target address if not already written
    if (!written) {
        fprintf(temp_file, "%04d\n", value);
    }

    fclose(file);
    fclose(temp_file);

    // Replace the original file with the updated file
    remove(dmemout_txt);
    rename("temp.txt", dmemout_txt);

    return 0; // Success
}
#endif