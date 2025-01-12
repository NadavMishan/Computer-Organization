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
	char line[MAX_LINE_LENGTH]; // Buffer to hold each line

	// Read the file line by line
	while (fgets(line, sizeof(line), file)) {
		// Check if the line contains only whitespace or is empty
		int only_whitespace = 1;
		for (char* ptr = line; *ptr != '\0'; ptr++) {
			if (!isspace((unsigned char)*ptr)) {
				only_whitespace = 0;
				break;
			}
		}

		// If the line contains non-whitespace characters, count it
		if (!only_whitespace) {
			line_count++;
		}
	}

	fclose(file);

	return line_count;
}

int copyFile(const char* sourceFilePath, const char* destinationFilePath) {
	FILE* sourceFile = fopen(sourceFilePath, "r");
	if (sourceFile == NULL) {
		fprintf(stderr, "Error: Unable to open source file %s\n", sourceFilePath);
		return -1;
	}

	FILE* destinationFile = fopen(destinationFilePath, "w");
	if (destinationFile == NULL) {
		fprintf(stderr, "Error: Unable to open destination file %s\n", destinationFilePath);
		fclose(sourceFile);
		return -1;
	}

	char buffer[1024];
	size_t bytesRead;

	while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0) {
		fwrite(buffer, 1, bytesRead, destinationFile);
	}

	fclose(sourceFile);
	fclose(destinationFile);

	printf("File copied successfully from %s to %s\n", sourceFilePath, destinationFilePath);
	return 0;
}


int debug_deleteOutputFiles(char* file_paths[]) {
	for (int i = 5; i < 15; i++) {
		if (file_paths[i] == NULL) {
			fprintf(stderr, "Error: Null file path at index %d.\n", i);
			continue;
		}

		if (remove(file_paths[i]) == 0) {
			printf("Successfully deleted: %s\n", file_paths[i]);
		}
		else {
			perror(file_paths[i]);
		}
	}

	return 0;
}
#endif
