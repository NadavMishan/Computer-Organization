#ifndef INSTRUCTION_ACTIONS_H
#define INSTRUCTION_ACTIONS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "LoadwordStoreword.h"

typedef struct instructionType {
	int opcode;
	int rd;
	int rs;
	int rt;
	int rm;
	int immediate1;
	int immediate2;
} instructionType;


// inputs an instruction as a string of a hex number and execute the right insturction.
instructionType parseInstruction(char* instruction_imemin, int* R) {
	/*
	* Insturction Format:
	0000 0000|	0000|	0000|	0000|	0000|	0000 0000 0000|		1111 1111 1111|
	OP code		rd		rs		rt		rm		immediate1			immediate2
	*/

	instructionType instruction = { 0 };


	// Hex -> long long unsigned
	unsigned long long instructionVal = strtoull(instruction_imemin, NULL, 16);
	//printf("instruction: %s | %llu \n", instruction_imemin, instructionVal);

	// Parse specific values for instruction
	instruction.opcode = (instructionVal & 0xFF0000000000) >> 40; // We know that last 16 bits are 0 (Logical/ Arithmetic shift doesn't matter)
	instruction.rd = (instructionVal & 0x00F000000000) >> 36;
	instruction.rs = (instructionVal & 0x000F00000000) >> 32;
	instruction.rt = (instructionVal & 0x0000F0000000) >> 28;
	instruction.rm = (instructionVal & 0x00000F000000) >> 24;

	//The imm values need to be sign extended for negative values
	instruction.immediate1 = sign_extend_12bit((instructionVal & 0x000000FFF000) >> 12);
	instruction.immediate2 = sign_extend_12bit((instructionVal & 0x000000000FFF));

	//printf("opcode: %d\t rd: %d\t rs: %d\t rt: %d\t rm: %d\t immediate1: %d\t immediate2: %d\n",
		//instruction.opcode, instruction.rd, instruction.rs, instruction.rt,
		//instruction.rm, instruction.immediate1, instruction.immediate2);


	// Assign values for $0, $imm1, $imm2
	R[0] = 0;
	R[1] = instruction.immediate1;
	R[2] = instruction.immediate2;

	return instruction;
}


int executeInsturctionBasic(instructionType I, int* R, int* PCadr) {
	int R_rm_12bits = R[I.rm] & 0xFFF; // take first 12 bits of R[rm]
	int PC = *PCadr;
	PC += 1;

	//char line[256] = "";

	// Choose the operation and execute
	switch (I.opcode)
	{
	case 0: //add
		R[I.rd] = R[I.rs] + R[I.rt] + R[I.rm];
		break;

	case 1:	//sub
		R[I.rd] = R[I.rs] - R[I.rt] - R[I.rm];
		break;

	case 2:	//mac
		R[I.rd] = R[I.rs] * R[I.rt] + R[I.rm];
		break;

	case 3:	//and
		R[I.rd] = R[I.rs] & R[I.rt] & R[I.rm];
		break;

	case 4:	//or
		R[I.rd] = R[I.rs] | R[I.rt] | R[I.rm];
		break;

	case 5:	//xor
		R[I.rd] = R[I.rs] ^ R[I.rt] ^ R[I.rm];
		break;

	case 6: //sll
		R[I.rd] = R[I.rs] << R[I.rt];
		break;

	case 7: // sra - Arithmetic shift, C default
		R[I.rd] = R[I.rs] >> R[I.rt];
		break;

	case 8: // srl - Logical shift 
		R[I.rd] = (unsigned int)R[I.rs] >> R[I.rt];
		break;

	case 9: //beq
		if (R[I.rs] == R[I.rt]) PC = R_rm_12bits;
		break;

	case 10: //bne
		if (R[I.rs] != R[I.rt]) PC = R_rm_12bits;
		break;

	case 11: //blt
		if (R[I.rs] < R[I.rt]) PC = R_rm_12bits;
		break;

	case 12: //bgt
		if (R[I.rs] > R[I.rt]) PC = R_rm_12bits;
		break;

	case 13: //ble
		if (R[I.rs] <= R[I.rt]) PC = R_rm_12bits;
		break;

	case 14: //bge
		if (R[I.rs] >= R[I.rt]) PC = R_rm_12bits;
		break;

	case 15: //jal 

		R[I.rd] = PC; // PC = PC +1
		PC = R_rm_12bits;
		printf("jal: PC: %d, return addr: %d\n", PC, R[I.rd]);
		break;

	}

	// Reset values for $0, $imm1, $imm2 if you tried to write to them.
	R[0] = 0;
	R[1] = I.immediate1;
	R[2] = I.immediate2;

	*PCadr = PC; // Update PC


	return 0;
}


int executeInsturctionLwSw(instructionType I, int* R, int* PCadr, char* dmemout) {
	switch (I.opcode)
	{
	case 16: // Load word (read)
		R[I.rd] = memRead(R[I.rs] + R[I.rt], dmemout) + R[I.rm];
		break;

	case 17: // Store word (write)
		memWrite(R[I.rs] + R[I.rt], R[I.rm] + R[I.rd], dmemout);

		break;

	default:
		break;
	}
	*PCadr += 1; // Update PC
	return 0;
	
}



int executeInsturctionIO(instructionType I, int* R, unsigned int* R_IO, int* PCadr, int* irq_ptr, char* inargs[]) {
	int PC = *PCadr;
	PC += 1;
	int RrsRrt = R[I.rs] + R[I.rt];


	// Choose the operation and execute
	switch (I.opcode)
	{
	case 18: //Reti
		PC = R_IO[7]; // Return to irqreturn PC
		*irq_ptr = 0; // return irq to 0 
		break;

	case 19:	// I/O - IN  (READ)
		hwregtrace_txt(R_IO[8], "READ", RrsRrt, R_IO[RrsRrt], inargs[8]);  
		R[I.rd] = R_IO[RrsRrt];

		if (I.rd == 22) { // "A read from monitorcmd using the in instruction will return the value 0." TODO is it "return the value 0" or "return *to* 0" 
			R[I.rd] = 0;
		}

		break;

	case 20:	// I/O - OUT (WRITE)

		hwregtrace_txt(R_IO[8], "WRITE", RrsRrt, R[I.rm], inargs[8]);   

		unsigned int value = R[I.rm];

		switch (RrsRrt)
		{

		case 9:	// leds
			// Check if  value is changed before updating in the file
			if (R_IO[RrsRrt] != R[I.rm]) {
				leds_txt_display7seg_txt(R_IO[8], R[I.rm], inargs[10]);
			}
			break;
		case 10: // display7seg
			// Check if  value is changed before updating in the file
			if (R_IO[RrsRrt] != R[I.rm]) {
				leds_txt_display7seg_txt(R_IO[8], R[I.rm], inargs[11]);
			}
			break;
		case 14: // diskcmd
		case 15: // disksector
		case 16: // diskbuffer
		case 17: // diskstatus

			if (R_IO[17]) { // if the disk is busy, don't write to it
				value = R_IO[RrsRrt];
			}
		}

		R_IO[RrsRrt] = value; // Assign value to the registers 

		break;
	}

	// Reset values for $0, $imm1, $imm2 if you tried to write to them.
	R[0] = 0;
	R[1] = I.immediate1;
	R[2] = I.immediate2;

	*PCadr = PC; // Update PC

	return 0;


}

#endif