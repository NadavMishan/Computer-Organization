#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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
int hex_to_signedInt(char* hex) {
    // Convert the hexadecimal string to an unsigned integer
    uint32_t unsigned_value = (uint32_t)strtoul(hex, NULL, 16);
    // Cast the unsigned value to a signed 32-bit integer
    int32_t signed_value = (int32_t)unsigned_value;
    return signed_value;
}

#endif