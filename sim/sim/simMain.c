#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include "fileActions.h"
#include "instructionActions.h"
#include "loggers.h"
#include "hardware.h"


/*
0   sim.exe
1   imemin.txt
2   dmemin.txt
3   diskin.txt
4   irq2in.txt
5   dmemout.txt
6   regout.txt
7   trace.txt
8   hwregtrace.txt
9   cycles.txt
10  leds.txt
11  display7seg.txt
12  diskout.txt
13  monitor.txt
14  monitor.yuv
*/



//int main(int argc, char* argv[]) {
int main() {
	char* inargs[] = { "sim.exe","imemin.txt","dmemin.txt","diskin.txt","irq2in.txt",
		"dmemout.txt","regout.txt","trace.txt","hwregtrace.txt","cycles.txt","leds.txt",
		"display7seg.txt","diskout.txt","monitor.txt","monitor.yuv" };

	// Initialize variables
	int registers[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	unsigned int IO_registers[] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0 };
	int PC = 0;
	int irq = 0;
	int (*monitor)[256] = calloc(256, sizeof(*monitor));

	instructionType insturction = { 0 };
	init_dmemout(inargs[2], inargs[5]); //Initialise dmmout

	// Read Input files

	while (1) {
		
		interrupts(&PC, IO_registers, &irq, inargs);

		printf("PC: %d\n", PC);
		//Parse Instruction
		
		char* instructionHex = readSpecificLine(inargs[1], PC);

		insturction = parseInstruction(instructionHex, registers);
		
		trace_txt(PC, instructionHex, registers, inargs[7]);

		// Execute Instruction
		if (insturction.opcode <= 15) {
			executeInsturctionBasic(insturction, registers, &PC);
		
		}

		else if (insturction.opcode <= 17) {
			executeInsturctionLwSw(insturction, registers, &PC, *inargs[5]);
			PC += 1; //remove
		}

		else if (insturction.opcode <= 20) {
			executeInsturctionIO(insturction, registers, IO_registers, &PC,&irq, inargs);
		}

		else {
			printf("----------------		Program Halted		----------------\n");
			break;
		}

		printf("Registers: ");
		for (int i = 0; i < 16; i++) printf("%d) %d\t", i, registers[i]);
		printf("\n ------------------------------------------------------------\n");

		HardwareCycle(insturction, registers, IO_registers, inargs, monitor);
	}
	
	// End run .txt files

	regout_txt(registers, inargs[6]);
	cycles_txt(IO_registers[8], inargs[9]);
	monitor_txt(monitor, inargs[13]);
	monitor_yuv(monitor, inargs[14]);

	return 0;
};


//dmemout = Copy of dmemin
int initialize_dmemout(const char* sourceFilePath, const char* destinationFilePath) {
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