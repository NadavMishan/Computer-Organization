#ifndef HARDWARE_H
#define HARDWARE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include "diskActions.h"

/* R_IO
--Interrupts--					--Devices--				--DiskDrive--			--Monitor--
V	0) irq0enable				V	8)  clks			V	14) diskcmd			V	20) monitoraddr
V	1) irq1enable				V	9)  leds			V	15) disksector		V	21) monitordata
V	2) irq2enable				V	10) display7seg		V	16) diskbuffer		V	22) monitorcmd
V	3) irq0status: Timer								V	17) diskstatus
V	4) irq1status: HardDisk		--Timer--
V	5) irq2status: irq2in.txt	V	11) timerenable			18) DNE
V	6) irqhandler				V	12) timercurrent		19) DNE
V	7) irqreturn				V	13) tiemrmax
*/




int interrupts(int* PC_ptr, int* R_IO, int* irq_ptr, int* disk_clk, char* inargs[]) {

	// ----- IRQ0 -----
	// Reset the timer and set irqstatus0 
	if (R_IO[13] > 0) {
		if (R_IO[12] == R_IO[13]) {		// timercurrent == timermax
			R_IO[12] = 0;				// reset timer 
			R_IO[3] = 1; 				// set irq0status
		}
	}

	// ----- IRQ1 -----
	if (*disk_clk == 1024) {
		*disk_clk = 0;
		R_IO[4] = 1;				// set irq1status
		R_IO[14] = 0;				// reset diskcmd
		R_IO[17] = 0;				// reset diskstatus
	
	}


	// ----- IRQ2 -----

	if (findValueInFile(inargs[4], R_IO[8])) {
		R_IO[5] = 1;
	}
	else {
		R_IO[5] = 0;
	}

	// Call ISR
	if (!*irq_ptr) { // Don't call ISR if already in ISR
		if ((R_IO[0] & R_IO[3]) | (R_IO[1] & R_IO[4]) | (R_IO[2] & R_IO[5])) {
			printf("\n --------------	irq triggered	---------------\n");
			printf("enable: 0) %d		1) %d		2) %d	\nstatus: 0) %d		1) %d		2) %d\n", R_IO[0], R_IO[1], R_IO[2], R_IO[3], R_IO[4], R_IO[5]);
			*irq_ptr = 1;
			R_IO[7] = *PC_ptr;		// Save return address
			*PC_ptr = R_IO[6];		// Next instruction should be the ISR  
		}
	}
	return 0;
}



int Clk(unsigned int* R_IO) {
	// Clk +=1
	R_IO[8] += 1;

	// Reset Clk
	if (R_IO[8] == 0xFFFFFFFF) R_IO[8] = 0;

	return 0;
}

int Timer(unsigned int* R_IO) {
	// Add +1 to the timer 
	if (R_IO[11]) {
		R_IO[12] = R_IO[12] + 1;
	}

	return 0;
}

int monitorWrite(unsigned int* R_IO, int monitor[256][256]) { 
	int row = floor(R_IO[20] / 256);
	int col = R_IO[20] % 256;
	if (R_IO[22]) {
		monitor[row][col] = R_IO[21];
	}
	return 0;
}

int disk(unsigned int* R_IO, char* dmemoutFilePath, char* diskoutFilePath, unsigned int* disk_clk) {
	int sector = 128 * R_IO[15];
	if (!R_IO[17]) {
		switch (R_IO[14]) // diskcmd
		{
		case 0:
			// Do nothing
			break;
		case 1: // Read
			printf("--------- Disk Read ---------\n");
			printf("in: %s, sector %d, out: %s, line: %d",diskoutFilePath, sector, dmemoutFilePath, R_IO[16]);
			diskReadWrite(diskoutFilePath, sector, dmemoutFilePath, R_IO[16]);
			R_IO[17] = 1; // diskstatus
			break;
		case 2: // Write
			printf("--------- Disk Write ---------\n");
			diskReadWrite(dmemoutFilePath, R_IO[16], diskoutFilePath, sector);
			R_IO[17] = 1; // diskstatus
			break;
			;
		}
	}

	if (R_IO[17]) { // raise the timer if the disk is busy
		*disk_clk += 1;

		return 0;
	}
}

int HardwareCycle(unsigned int* R_IO, char* inargs[], int monitor[256][256], unsigned int* disk_clk) {
	//printf("%s, %s", inargs[5], inargs[12]);
	disk(R_IO, inargs[5], inargs[12], disk_clk);

	Clk(R_IO);

	Timer(R_IO);

	monitorWrite(R_IO, monitor);

	return 0;
}





#endif