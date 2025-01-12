#define _CRT_SECURE_NO_WARNINGS
#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
0   sim.exe
1   imemin.txt          Assembler
2   dmemin.txt          Assembler
3   diskin.txt          
4   irq2in.txt          
5   dmemout.txt         V 
6   regout.txt          V
7   trace.txt           V
8   hwregtrace.txt      V     
9   cycles.txt          V
10  leds.txt            V
11  display7seg.txt     V
12  diskout.txt         V
13  monitor.txt         V
14  monitor.yuv         V
*/

int init_logfiles() {
    return 0;
}



int regout_txt(int* R, char* filePATH) {
    // Try to open the file in append mode
    FILE* file = fopen(filePATH, "w");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Write all Register values from 3->15 in the PATH
    for (int i = 3; i < 16; i++){
        fprintf(file, "%08X\n", R[i]);
    }

    fclose(file);
    return 0; 
}

int trace_txt(int PC,char* instructionHEX,int* R, char* filePATH) {
    // Try to open the file in append mode
    FILE* file = fopen(filePATH, "a");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    
    // Write the line to the file
    fprintf(file,"%03X %s ", 
        PC, instructionHEX);
    for (int i = 0; i <16; i++){
        fprintf(file,"%08x ", R[i]);
    }
	
    fprintf(file, "\n");

    fclose(file);
    return 0;


}

int cycles_txt(unsigned int clk, char* filePATH) {
    // Try to open the file in append mode
    FILE* file = fopen(filePATH, "w");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
   
    fprintf(file, "%u\n", clk);
   
    fclose(file);
    return 0;
}


int leds_txt_display7seg_txt(unsigned int clk, unsigned int value, char* filePATH) {
    // Try to open the file in append mode
    FILE* file = fopen(filePATH, "a");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    fprintf(file, "%u %08X\n", clk, value);

    fclose(file);
    return 0;
}


int hwregtrace_txt(unsigned int clk, char* Read_Write, int register_number, unsigned int data, char* filePATH) {
    char device_name[16] = "";
    //printf( "%u %s %d %08X %s", clk, Read_Write, register_number, data, filePATH);

    switch (register_number) {
    case 0: strcpy(device_name, "irq0enable"); break;
    case 1: strcpy(device_name, "irq1enable"); break;
    case 2: strcpy(device_name, "irq2enable"); break;
    case 3: strcpy(device_name, "irq0status"); break;
    case 4: strcpy(device_name, "irq1status"); break;
    case 5: strcpy(device_name, "irq2status"); break;
    case 6: strcpy(device_name, "irqhandler"); break;
    case 7: strcpy(device_name, "irqreturn"); break;
    case 8: strcpy(device_name, "clks"); break;
    case 9: strcpy(device_name, "leds"); break;
    case 10: strcpy(device_name, "display7seg"); break;
    case 11: strcpy(device_name, "timerenable"); break;
    case 12: strcpy(device_name, "timercurrent"); break;
    case 13: strcpy(device_name, "timermax"); break;
    case 14: strcpy(device_name, "diskcmd"); break;
    case 15: strcpy(device_name, "disksector"); break;
    case 16: strcpy(device_name, "diskbuffer"); break;
    case 17: strcpy(device_name, "diskstatus"); break;
    case 18: strcpy(device_name, "reserved"); break;
    case 19: strcpy(device_name, "reserved"); break;
    case 20: strcpy(device_name, "monitoraddr"); break;
    case 21: strcpy(device_name, "monitordata"); break;
    case 22: strcpy(device_name, "monitorcmd"); break;

    default:
        break;
    }

    //printf("\n%s", filePATH);

    
    // Try to open the file in append mode
    FILE* file = fopen(filePATH, "a");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    fprintf(file, "%u %s %s %08X\n", clk, Read_Write, device_name, data);
    fclose(file);
    return 0;

}


int monitor_txt(int monitor[256][256], const char* filePATH) {
    // Open the file for writing
    FILE* file = fopen(filePATH, "w");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Write each pixel value in hexadecimal format
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            // Write each pixel value in 2 hex digits format
            fprintf(file, "%02X\n", monitor[i][j] & 0xFF);
        }
    }

    // Close the file
    fclose(file);
    return 0;
}

int monitor_yuv(int monitor[256][256], const char* filePATH) {
    // Open the file in binary write mode
    FILE* file = fopen(filePATH, "wb");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Write the monitor data in binary format
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            // Write only the least significant byte of the pixel value
            unsigned char pixel = monitor[i][j] & 0xFF;
            fwrite(&pixel, sizeof(unsigned char), 1, file);
        }
    }

    // Close the file
    fclose(file);
    return 0;
}

#endif