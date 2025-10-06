#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 10000

unsigned char memory[MEMORY_SIZE];

int hexToDecimal(char hex[]) {
    int val;
    sscanf(hex, "%x", &val);
    return val;
}

void printMemory(int start, int end) {
    for (int i = start; i < end;) {
        printf("%06X : ", i);
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

    char name[20], address[10], length[10];
    int startAddr = 0, execAddr = 0, endAddr = 0;

    while (fgets(line, sizeof(line), fob)) {
        if (line[0] == 'H') {
            sscanf(line, "H %s %s %s", name, address, length);
            startAddr = hexToDecimal(address);
            endAddr = startAddr + hexToDecimal(length);
            printf("Program Name: %s\n", name);
            printf("Start Address: %06X\n", startAddr);
            printf("Program Length: %s\n", length);
        } 
        else if (line[0] == 'T') {
            // Text record
            char t_addr[10], t_len[10];
            char objStr[70];
            
            // Read only address and length first
            sscanf(line, "T %s %s", t_addr, t_len);
            
            // Find start of object codes in line and skip past length
            char *obj_ptr = strchr(line + 1, ' '); // skip 'T'
            obj_ptr = strchr(obj_ptr + 1, ' ');    // skip address
            obj_ptr = strchr(obj_ptr + 1, ' ');    // skip length
            
            if (obj_ptr) {
                obj_ptr++; // move past the space
                strcpy(objStr, obj_ptr);
                
                // remove newline at end
                objStr[strcspn(objStr, "\n")] = '\0';
                objStr[strcspn(objStr, "\r")] = '\0';
            }

            int t_start = hexToDecimal(t_addr);
            int t_len_dec = hexToDecimal(t_len);

            int i = 0;
            for (int j = t_start; j < t_start + t_len_dec && objStr[i] != '\0'; j++) {
                char byteStr[3] = { objStr[i], objStr[i + 1], '\0' };
                memory[j] = (unsigned char)strtol(byteStr, NULL, 16);
                i += 2;
            }
        } 
        else if (line[0] == 'E') {
            char e_addr[10];
            sscanf(line, "E %s", e_addr);
            execAddr = hexToDecimal(e_addr);
            printf("Execution starts at: %06X\n\n", execAddr);
        }
    }

    fclose(fob);

    printf("Loaded Memory Contents:\n");
    printMemory(startAddr, endAddr);

    return 0;
}
