#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
    unsigned int logicalAddy = 0;
    unsigned int phyicalAddy = 0;
    char value = 0;
    unsigned int offsetMask = (1 << 8) - 1;
    unsigned int pageMask = (1 << 16) - 1;
    unsigned int pageNumber = 0;
    unsigned int offsetNumber = 0;
    FILE *addyFile = fopen(argv[1], "r");
    
    if (argc != 2){
        fprintf(stderr, "usage: ./virtualmem addresses.txt\n");
        return 1;
    }

    while(fscanf(addyFile, "%u", &logicalAddy) == 1){
        pageNumber = (logicalAddy & pageMask) >> 8;
        offsetNumber = logicalAddy & offsetMask;


        fprintf(stdout, "Virtual address: %u Physical address: %u Value: %d", logicalAddy, phyicalAddy, value);
    }

    fclose(addyFile);


    return 0;
}