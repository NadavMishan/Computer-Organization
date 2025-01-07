#ifndef HARDWARE_H
#define HARDWARE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* R_IO
--Interrupts--					--Devices--				--DiskDrive--		--Monitor--	
	0) irq0enable				V	8)  clks				14) diskcmd			20) monitoraddr
	1) irq1enable				V	9)  leds				15) disksector		21) monitordata
	2) irq2enable				V	10) display7seg			16) diskbuffer		22) monitorcmd
V	3) irq0status: Timer									17) diskstatus
	4) irq1status: HardDisk		--Timer--		
	5) irq2status: irq2in.txt	V	11) timerenable			18) DNE
	6) irqhandler				V	12) timercurrent		19) DNE 
	7) irqreturn				V	13) tiemrmax		
*/


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


int HardwareCycle(instructionType I, int* R, unsigned int* R_IO, char* inargs) {

	Clk(R_IO);

	//Interrupts();
	Timer(R_IO);

	return 0;
}





#endif