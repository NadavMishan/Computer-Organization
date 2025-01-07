#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>

int sign_extend_12bit(int num) {
    // Check if the 12th bit (sign bit) is set to 1 
    if (num & 0x800) {
        return num | 0xFFFFF000;  // Sign extend 1
    }
    // We don't need to sign extend
    else { 
        return num;
    }
}


#endif