#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include "fileActions.h"
#include "instructionActions.h"

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
	char* inargs[] = { "sim.exe ","imemin.txt ","dmemin.txt ","diskin.txt ","irq2in.txt ",
		"dmemout.txt ","regout.txt","trace.txt","hwregtrace.txt ","cycles.txt ","leds.txt ",
		"display7seg.txt ","diskout.txt ","monitor.txt","monitor.yuv" };

	// Initialize variables
	int registers[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	int PC = 0;
	instructionType insturction = { 0 };

	// Read Input files

	char** imemin = parseFile(inargs[1]);

	while (1) {
		printf("PC: %d\n", PC);
		//Parse Instruction
		insturction = parseInstruction(imemin[PC], registers);
		
		// Execute Instruction
		if (insturction.opcode <= 15) { 
			executeInsturctionBasic(insturction, registers, &PC); 
		}

		else if (insturction.opcode <= 17) { 
			executeInsturctionLwSw(insturction, registers, &PC, *inargs); 
			PC += 1; //remove
		}
		
		else if (insturction.opcode <= 20) { 
			executeInsturctionIO(insturction, registers, &PC, *inargs); 
			PC += 1; //remove
		}
		
		else {
			printf("----------------		Program Halted		----------------\n");
			break;
		}

		printf("Registers: ");
		for (int i = 0; i < 16; i++) printf("%d) %d\t", i, registers[i]);
		printf("\n ------------------------------------------------------------\n");

	}


	return 0;
}