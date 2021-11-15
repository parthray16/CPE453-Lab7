#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define pageTableSize (1 << 8)
#define pageSize (1 << 8)
#define frameSize (1 << 8)
#define numFrames (1 << 8)
#define MAXTLBSIZE 16
#define phymemSize frameSize * numFrames

typedef struct TLBObj{
    unsigned int pageNumber;
    int frameNum;
} TLBObj;

int main(int argc, char* argv[]){
    TLBObj* TLB[MAXTLBSIZE];
    int pageTable[pageTableSize];
    char physicalMem[phymemSize];
    int frameNum = 0;
    unsigned int logicalAddy = 0;
    unsigned int physicalAddy = 0;
    char value = 0;
    unsigned int offsetMask = (1 << 8) - 1;
    unsigned int pageMask = (1 << 16) - 1;
    unsigned int pageNumber = 0;
    unsigned int offsetNumber = 0;
    FILE *addyFile = fopen(argv[1], "r");
    FILE *demandFile = fopen("BACKING_STORE.bin", "rb");
    int first = 0;
    int i = 0;
    int TLBSize = 0;
    TLBObj* newTLBObj = NULL;
    int inTLB = 0; /* in TLB flag*/
    
    if (argc != 2){
        fprintf(stderr, "usage: ./virtualmem addresses.txt\n");
        return 1;
    }

    memset(pageTable, -1, pageTableSize*sizeof(int));

    while(fscanf(addyFile, "%u", &logicalAddy) == 1){
        pageNumber = (logicalAddy & pageMask) >> 8;
        offsetNumber = logicalAddy & offsetMask;
        inTLB = 0;
        /* check if in TLB */
        for (i = 0; i < TLBSize; i++){
            if (TLB[i]->pageNumber == pageNumber){
                physicalAddy = TLB[i]->frameNum * pageSize + offsetNumber;
                value = physicalMem[physicalAddy];
                fprintf(stdout, "Virtual address: %u Physical address: %u Value: %d\n", logicalAddy, physicalAddy, value);
                inTLB = 1;
                break;
            }
        }
        if (inTLB){
            continue;
        }
        /* not in TLB */
        if (pageTable[pageNumber] == -1){
            /* page fault */

            /* read in frame from demandFile */
            fseek(demandFile, pageNumber * frameSize, SEEK_SET);
            fread(physicalMem + frameNum * frameSize, sizeof(char), frameSize, demandFile);
            pageTable[pageNumber] = frameNum;
            frameNum++;

        }
        /* in page table */
        physicalAddy = pageTable[pageNumber] * pageSize + offsetNumber;
        value = physicalMem[physicalAddy];
        fprintf(stdout, "Virtual address: %u Physical address: %u Value: %d\n", logicalAddy, physicalAddy, value);

        /* add to TLB */
        newTLBObj = (TLBObj *)malloc(sizeof(TLBObj));
        newTLBObj->frameNum = pageTable[pageNumber];
        newTLBObj->pageNumber = pageNumber;
        if (TLBSize < MAXTLBSIZE){
            /* tlb not full */
            TLB[TLBSize++] = newTLBObj;
        }
        else{
            /* FIFO Replacement */
            free(TLB[first]);
            TLB[first] = newTLBObj;
            first = (first + 1) % MAXTLBSIZE;
        }
    }

    for (i = 0; i < TLBSize; i++){
        free(TLB[i]);
    }
    fclose(addyFile);
    fclose(demandFile);

    return 0;
}