#ifndef FILE_ACTIONS_H
#define FILE_ACTIONS_H
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 4096
#define MAX_LINE_LENGTH 128

// Function to remove white spaces from a string
void remove_whitespaces(char* str) {
    char* read = str, * write = str;
    while (*read) {
        if (!isspace((unsigned char)*read)) {
            *write++ = *read;
        }
        read++;
    }
    *write = '\0';
}

// Function to read lines from a file and process them
char** parseFile(const char* file_path, int* size_ref) {
    int line_count = 0;
    FILE* file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    char** lines = (char**)malloc(MAX_LINES * sizeof(char*));
    if (!lines) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    char buffer[MAX_LINE_LENGTH];
    line_count = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (line_count >= MAX_LINES) {
            fprintf(stderr, "Exceeded maximum number of lines (%d).\n", MAX_LINES);
            break;
        }

        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = '\0';
        remove_whitespaces(buffer);

        // Allocate memory for the line and store it
        lines[line_count] = _strdup(buffer);  // Replace _strdup with strdup for portability
        if (!lines[line_count]) {
            perror("Error duplicating string");

            // Free previously allocated lines in case of failure
            for (int i = 0; i < line_count; i++) {
                free(lines[i]);
            }
            free(lines);
            fclose(file);
            return NULL;
        }

        line_count++;
    }

    fclose(file);

    *size_ref = line_count;
    return lines;
}


// Function to read a specific line from a file
char* readSpecificLine(char* filename, int lineNumber) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    static char line[MAX_LINE_LENGTH];
    int currentLine = 0;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (currentLine == lineNumber) {
            fclose(file);
            remove_whitespaces(line);
            return line;
        }
        currentLine++;
    } 

    fclose(file);


    return NULL;
}


int countLines(const char* file_path) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening file");
        return -1;  // Return -1 to indicate an error
    }

    int line_count = 0;
    char ch;

    // Read the file character by character
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            line_count++;
        }
    }

    fclose(file);

    return line_count;
}


#endif
