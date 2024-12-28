#ifndef FILE_ACTIONS_H
#define FILE_ACTIONS_H
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 256

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
char** parseFile(const char* file_path) {
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
        lines[line_count] = _strdup(buffer);
        if (!lines[line_count]) {
            perror("Error duplicating string");
            break;
        }

        (line_count)++;
    }

    fclose(file);
    return lines;
}

// Function to read a specific line from a file
char* readSpecificLine(const char* filename, int lineNumber) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    static char line[MAX_LINE_LENGTH];
    int currentLine = 0;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        currentLine++;
        if (currentLine == lineNumber) {
            fclose(file);
            remove_whitespaces(line);
            return line;
        }
    }

    fclose(file);
    return NULL;
}



#endif