#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fileActions.h"
#include "instructionActions.h"
#include "loggers.h"
#include "hardware.h"
//#include "LoadwordStoreword.h"
//#include "diskActions.h"

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


/*
cmd command : sim.exe imemin.txt dmemin.txt diskin.txt irq2in.txt dmemout.txt regout.txt trace.txt hwregtrace.txt cycles.txt leds.txt display7seg.txt diskout.txt monitor.txt monitor.yuv
*/

int main(int argc, char* argv[]) {
	char* default_args[15] = {
	   "sim.exe", "imemin.txt", "dmemin.txt", "diskin.txt", "irq2in.txt",
	   "dmemout.txt", "regout.txt", "trace.txt", "hwregtrace.txt", "cycles.txt",
	   "leds.txt", "display7seg.txt", "diskout.txt", "monitor.txt", "monitor.yuv"
	};

	if (argc == 1) {
		// Replace argc and argv contents with default values
		argc = 15;
		argv = default_args;  // Point argv to the new array
	}

	/*for (int i = 0; i < argc; i++) {
		printf("argv %d, %s\n",i, argv[i]);
	}*/

	debug_deleteOutputFiles(argv); //TODO remove this line


	// Initialize variables
	int registers[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	unsigned int IO_registers[] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0 };
	int PC = 0;
	int irq = 0;
	int (*monitor)[256] = calloc(256, sizeof(*monitor)); // Initialise monitor
	instructionType insturction = { 0 };
	int disk_clk = 0;


	// Some files are written in "append" mode, so they might never be written to in the run.
	for (int i = 0; i < 12; i++)
	{
		if (i == 3 || i == 4 || i == 8 || i == 10 || i == 11) {
			ensureFileExists(argv[i]);
		}
	}
	
	copyFile(argv[2], argv[5]); //Initialise dmmout
	copyFile(argv[3], argv[12]); //Initialise diskout


	// Read Input files
	
	while (1) {
		
		
		printf("PC: %03X CLK: %d\n", PC,IO_registers[8]);
		//Parse Instruction

		char* instructionHex = readSpecificLine(argv[1], PC);

		insturction = parseInstruction(instructionHex, registers);

		trace_txt(PC, instructionHex, registers, argv[7]);

		// Execute Instruction
		if (insturction.opcode <= 15) {
			executeInsturctionBasic(insturction, registers, &PC);

		}

		else if (insturction.opcode <= 17) {
			executeInsturctionLwSw(insturction, registers, &PC, argv[5]);
		}

		else if (insturction.opcode <= 20) {
			executeInsturctionIO(insturction, registers, IO_registers, &PC, &irq, argv);
		}

		else {
			IO_registers[8] += 1;
			printf("----------------		Program Halted		----------------\n");
			break;
		}



		/*printf("\nRegisters: ");
		for (int i = 0; i < 16; i++) printf("%d) %d\t", i, registers[i]);*/
		printf("\nIO registers: ");
		for (int i = 0; i < 23; i++) printf("%d) %d\t", i, IO_registers[i]);
		printf("\n ------------------------------------------------------------------------------------------------------------------------\n");

		interrupts(&PC, IO_registers, &irq, &disk_clk, argv);
		HardwareCycle(insturction, registers, IO_registers, argv, monitor, &disk_clk);


		//if (IO_registers[8] == 100) break;
	}

	// End run .txt files
	regout_txt(registers, argv[6]);
	cycles_txt(IO_registers[8], argv[9]);
	monitor_txt(monitor, argv[13]);
	monitor_yuv(monitor, argv[14]);

	//debug_deleteOutputFiles(argv);
	return 0;
};