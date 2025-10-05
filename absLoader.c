#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 10000  // simulated memory

unsigned char memory[MEMORY_SIZE];  // simulated memory array

int hexToDecimal(char hex[]) {
    int val;
    sscanf(hex, "%x", &val);
    return val;
}

// Function to print memory contents in hex
void printMemory(int start, int end) {
    for (int i = start; i < end;) {
        printf("%06X : ", i);  // print address in hex
        for (int j = 0; j < 6 && i < end; j++, i++) {
            printf("%02X ", memory[i]);
        }
        printf("\n");
    }
}

int main() {
    FILE *fob;
    char line[100];

    fob = fopen("objectFile.txt", "r");
    if (fob == NULL) {
        printf("Error: File not found.\n");
        return 1;
    }

    char label[2], name[20], address[10], length[10], recordType;
    int startAddr = 0, execAddr = 0, endAddr = 0;

    while (fgets(line, sizeof(line), fob)) {
        if (line[0] == 'H') {
            // Header Record
            sscanf(line, "H %s %s %s", name, address, length);
            startAddr = hexToDecimal(address);
            endAddr = startAddr + hexToDecimal(length);
            printf("Program Name: %s\n", name);
            printf("Start Address: %06X\n", startAddr);
            printf("Program Length: %s\n", length);
        } else if (line[0] == 'T') {
            // Text Record
            char t_addr[10], t_len[10], objcode[70];
            sscanf(line, "T %s %s %s", t_addr, t_len, objcode);
            int t_start = hexToDecimal(t_addr);
            int t_len_dec = hexToDecimal(t_len);

            int i = 0;
            for (int j = t_start; j < t_start + t_len_dec; j++) {
                char byteStr[3];
                byteStr[0] = objcode[i++];
                byteStr[1] = objcode[i++];
                byteStr[2] = '\0';

                memory[j] = (unsigned char)strtol(byteStr, NULL, 16);
            }
        } else if (line[0] == 'E') {
            // End Record
            char e_addr[10];
            sscanf(line, "E %s", e_addr);
            execAddr = hexToDecimal(e_addr);
            printf("Execution starts at: %06X\n\n", execAddr);
        }
    }

    fclose(fob);

    // Print loaded memory region
    printf("Loaded Memory Contents:\n");
    printMemory(startAddr, endAddr);

    return 0;
}

/*
output 
Start Address: 001000
Program Length: 0012
Execution starts at: 001000

Loaded Memory Contents:
001000 : 01 10 00 00 00 00
001006 : 00 00 00 00 00 00
00100C : 00 00 00 00 00 00
*/