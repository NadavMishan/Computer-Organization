#ifndef HARDWARE_H
#define HARDWARE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* R_IO
--Interrupts--					--Devices--				--DiskDrive--			--Monitor--	
V	0) irq0enable				V	8)  clks				14) diskcmd			V	20) monitoraddr
V	1) irq1enable				V	9)  leds				15) disksector		V	21) monitordata
V	2) irq2enable				V	10) display7seg			16) diskbuffer		V	22) monitorcmd
V	3) irq0status: Timer									17) diskstatus
	4) irq1status: HardDisk		--Timer--		
V	5) irq2status: irq2in.txt	V	11) timerenable			18) DNE
V	6) irqhandler				V	12) timercurrent		19) DNE 
V	7) irqreturn				V	13) tiemrmax		
*/

int interrupts(int* PC_ptr, int* R_IO, int* irq_ptr, char* inargs[]) {

	unsigned int irq2_clk;
	int irq2_size = countLines(inargs[4])+1;

	for (int i = 0; i < irq2_size ; i++) {
		irq2_clk = strtol(readSpecificLine(inargs[4],i), NULL, 10);

		if (irq2_clk == R_IO[8]) {
			R_IO[5] = 1;
			break; 
		}
		else  {
			R_IO[5] = 0;
		}
	}
	

	if(!*irq_ptr){
		if ((R_IO[0] & R_IO[1]) | (R_IO[2] & R_IO[3]) | (R_IO[4] & R_IO[5])) {
			*irq_ptr = 1;
			R_IO[7] = *PC_ptr;		// Save return address
			*PC_ptr = R_IO[6];		// Next instruction should be the ISR  
		}
	}
	return 0;
}



int Clk(unsigned int* R_IO) {
	// Clk +=1
	R_IO[8] = R_IO[8] + 1; 

	// Reset Clk
	if (R_IO[8] == 0xFFFFFFFF) R_IO[8] = 0;

	return 0;
}

int Timer(unsigned int* R_IO) {
	// Add +1 to the timer 
	if (R_IO[11]) {			
		R_IO[12] = R_IO[12] + 1;
	}

	// Reset the timer and set irqstatus0 
	if (R_IO[12] == R_IO[13]) {		// timercurrent == timermax
		R_IO[12] = 0;				// reset timer 
		R_IO[3] = 1; 				// set irq0status
	}

	return 0;
}

int monitorWrite(unsigned int* R_IO, int monitor[256][256]) { // TODO : who returns monitorcmd back to zero?
	int row = floor(R_IO[20] / 256);
	int col = R_IO[20] % 256;
	if (R_IO[22]) {
		monitor[row][col] = R_IO[21];
	}
	return 0;
}

int HardwareCycle(instructionType I, int* R, unsigned int* R_IO, char* inargs, int monitor[256][256]) {

	Clk(R_IO);

	Timer(R_IO);

	monitorWrite(R_IO, monitor);

	return 0;
}





#endif