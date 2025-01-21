#ifndef DISK_ACTIONS_H
#define DISK_ACTIONS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LENGTH 10 // Length of each line including the null terminator
#define DEFAULT_LINE "00000000"

// Function to copy 128 lines from one file to another

int copy128lines(char* filePathRead, int lineRead, char* filePathWrite, int lineWrite) {
	printf("Copying 128 lines from %s to %s\n", filePathRead, filePathWrite);
	printf("Copying from line %d to line %d\n", lineRead, lineWrite);

	// Read 128 lines from the source file
    FILE* source = fopen(filePathRead, "r");
	int copy[128] = { 0 };
	
	// initialize copy to 0
	for (int i = 0; i < 128; i++) {
		copy[i] = 0;
	}

	char buffer[LINE_LENGTH];
	if (source == NULL) {
		perror("Error opening source file");
		return 1;
	}
	int j = 0;
	for (int i = 0; i < (128 + lineRead); i++) {
		if (fgets(buffer, sizeof(buffer), source)) {
			if (i >= lineRead) {
				copy[i - lineRead] = strtol(buffer, NULL, 16);
			}
			
		}
	}
	fclose(source);


	// Write the 128 lines to the destination file
    
    FILE* dest = fopen(filePathWrite, "r");
    if (!dest) {
        perror("Error opening file");
        return 1;
    }

    FILE* temp = fopen("temp.txt", "w");
    if (!temp) {
        perror("Error creating temporary file");
        fclose(dest);
        return 1;
    }
    
	int current_line = 0;
    while (fgets(buffer, sizeof(buffer), dest)) {
            if ((current_line >= lineWrite) && (current_line < lineWrite+128)) {
				fprintf(temp, "%08X\n", copy[current_line - lineWrite]);
            }

			else {
				fprintf(temp, "%s", buffer);
			}
			current_line++;
        }

	// 0 padding is needed
	if (current_line < lineWrite) {
		for (int i = current_line; i < lineWrite; i++) {
			fprintf(temp, "%s\n", DEFAULT_LINE);
			current_line++;
		}
	}

	// finish writing the data to the file
	if (current_line < lineWrite + 128) {
		for (int i = current_line; i < lineWrite + 128; i++) {
			fprintf(temp, "%08X\n", copy[i - lineWrite]);
		}
	}


    fclose(dest);
    fclose(temp);

    // Replace the original file with the temporary file
    if (remove(filePathWrite) != 0) {
        perror("Error deleting original file");
        return 1;
    }
    if (rename("temp.txt", filePathWrite) != 0) {
        perror("Error renaming temporary file");
    }
	return 0;
}


#endif