#include "cachelab.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

typedef struct cacheLine {
    // int64_t blockSize;
    int64_t tag;
    int64_t startAddress;
    int usage;  // For LRU conviction policy
} cacheLine;

void cacheChunkOperation(
    int64_t associativity, int64_t chunkStartAddress, //int16_t bytes,
    int64_t setBits, int64_t blockBits, cacheLine cache[][associativity] 
);

int main(int argc, char* argv[])
{
    // command line parsing
    bool helpFlag = false;
    bool verboseFlag = false;
    char traceFile[256];
    int64_t setBits = 0;
    int64_t associativity = 0;
    int64_t blockBits = 0;

    char opt;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch(opt) {
        case 'h':
            printf("Optional help flag activated!\n");
            helpFlag = true;
            break;
        case 'v':
            printf("Verbose flag activated!\n");
            verboseFlag = true;
            break;
        case 't':
            printf("Trace file requested: %s\n", optarg);
            int length = strlen(optarg);
            memcpy(traceFile, optarg, length);
            traceFile[length] = '\0';
            break;
        case 's':
            printf("Bits for set selection: %d\n", atoi(optarg));
            setBits = atoi(optarg);
            break;
        case 'E':
            printf("Associativity: %d\n", atoi(optarg));
            associativity = atoi(optarg);
            break;
        case 'b':
            printf("Bits for each block: %d\n", atoi(optarg));
            blockBits = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Usage: %s [-hv] [-s nsetbits] [-E associativity] [-b blockbits] [-t tracefile]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    /* 
        Cache data structures:
        For the whole cache, we have [set] sets;
        For each set, we have [associativity] cache lines;
        For each cache line, we have two parts:
            - tag
            - data block
    */
    int64_t blockSize = pow(2, blockBits);
    int64_t set = pow(2, setBits);
    cacheLine cache[set][associativity];

    // Initiate cache
    for (int i = 0; i < set; i++) {
        for (int j = 0; j < associativity; j++) {
            cache[i][j].startAddress = 0x0;
            cache[i][j].tag = 0x0;
            cache[i][j].usage = false;
        }
    }

    /*
        Receives the starting address and the bytes to read,
        calculates the starting address of each chunk,
        and call cacheChunkOperation() for each chunk.

        For example, if the blockSize is 256 bytes,
        starting address is 0x50 (80 in dec) and reading in 1000 bytes,
        this (from 80 to 1079) will range over 5 blocks:
            0~255, 256~511, 512~767, 768~1023 and 1024~1279.
        We need to run cacheChunkOperation five times.
    */
    /*
    // Step 1: Test cold miss
    int addressWalker = (int)(0x00602400 / blockSize) * blockSize;
    while (addressWalker <= 0x00602400 + 1000) {
        cacheChunkOperation(associativity, addressWalker, setBits, blockBits, cache);
        addressWalker += blockSize;
    }
    printf("Cold miss test completed!\n");

    // Step 2: Test hit
    addressWalker = (int)(0x00602400 / blockSize) * blockSize;
    while (addressWalker <= 0x00602400 + 1000) {
        cacheChunkOperation(associativity, addressWalker, setBits, blockBits, cache);
        addressWalker += blockSize;
    }
    printf("Hit test completed!\n");

    // Step 3: Conviction hit
    // Since E=2, needs to fill set first
    addressWalker = (int)(0x0060A400 / blockSize) * blockSize;
    while (addressWalker <= 0x0060A400 + 1000) {
        cacheChunkOperation(associativity, addressWalker, setBits, blockBits, cache);
        addressWalker += blockSize;
    }
    addressWalker = (int)(0x0060C400 / blockSize) * blockSize;
    while (addressWalker <= 0x0060C400 + 1000) {
        cacheChunkOperation(associativity, addressWalker, setBits, blockBits, cache);
        addressWalker += blockSize;
    }
    printf("Conviction test completed!\n");
    */
    
    // Read file by line
    FILE* fp;
    char line[16];

    fp = fopen("../traces/long.trace");
    if (fp == NULL) {
        printf("File read failed!\n");
        exit(EXIT_FAILURE);
    }
    // TODO: Implement read functionality
    while (())

    // Test
    /* cacheChunkOperation(
        associativity, 0x00602400, //int16_t bytes,
        setBits, blockBits, cache
    ); */

    // printSummary(0, 0, 0);
    return 0;
}

/*
    cacheChunkOperation() only checks if a BLOCK of data resides in the cache.
    For example if we request 16 bytes of data starting from address 0x0,
    while the blockSize is 2 bytes, we should write a function to break it
    down to 8 chunks, and cacheOperation only check one chunk each time.
*/
void cacheChunkOperation(
    int64_t associativity, int64_t chunkStartAddress, //int16_t bytes,
    int64_t setBits, int64_t blockBits, cacheLine cache[][associativity] 
) {
    /*
        Examples:
        s = 4, E = 1, b = 4 -> 16 sets, each has one line, each 16 bytes
        cache looks like this:
        cacheLine cache[16][1]

        There are 3 cases:
        - Hit (tag matches + content already in block)
        - Miss (data is simply not there and the same block is cold)
        - Miss eviction (data is not there but same block is hot)

        With multiple cache lines in one set, conviction policy is LRU.
        So for example let's say data is supposed to go into set 0x2A,
        but all 16 lines of set 0x2A is occupied, now we need to find
        the one line that is least used recently.
    */

    /*
        Calculation of set, line and block offset:
        Let's say s = 8, E = 4, b= 16 ->
        256 sets, each has 4 lines, each 512 bytes.

        Address 0x00602400 comes in as a LOAD operation.
        - The lowest 16 bits (4 bytes) calculates the block offset;
        - The following 8 bits calculates the set number;
        - The rest goes to tag.

        We can use 0111|0000|0000|0000|0000 & address to fetch set bits.
        Then loop through all lines to see if we have a tag match.
        It's a hit if we do, but if not then there are two possibilities:
        - Some lines are empty, so we simply set the tag and write block;
        - All lines are full, use LRU to envict and set tag and write.
    */
    int64_t mask = 0x7FFFFFFFFFFFFFFF;
    // int64_t setMask = (((mask << (63 - setBits)) & 0x7FFFFFFFFFFFFFFF) >> (63 - setBits)) << blockBits;
    int64_t setMask = (mask >> (63 - setBits)) << blockBits;
    int64_t blockMask = (mask >> (63 - blockBits));
    int64_t tagMask = ~(setMask | blockMask);

    int64_t blockSize = pow(2, blockBits);
    int64_t setSelected = (chunkStartAddress & setMask) >> blockBits;

    /*
        Three cases:
        - Tag matches, so it's a hit
        - Tag doesn't match, and at least one of the lines is empty, write
        - Tag doesn't match, none of the lines is empty, convict on LRU
    */
    for (int i = 0; i < associativity; i++) {
        if (cache[setSelected][i].usage >= 1) {
            if (cache[setSelected][i].tag == (chunkStartAddress & tagMask)) {
                /*
                    If the set and tag both match and the usage flag is true,
                    then we have a hit. Remember we only need to check one chunk
                    of the data, not the whole range.
                */
                printf(
                        "Hit! %X : %X is within %X and %X, set %d, line %d\n",
                        chunkStartAddress, chunkStartAddress + blockSize - 1, 
                        cache[setSelected][i].startAddress, cache[setSelected][i].startAddress + blockSize - 1,
                        setSelected, associativity
                );
                cache[setSelected][i].usage++;
                return;
            }
        }
    }

    /*
        If none of the tags match, we try to write
    */
    for (int j = 0; j < associativity; j++) {
        if (cache[setSelected][j].usage == 0) {
            printf(
                "Miss! %X : %X can be written into set %d, line %d as it's cold\n",
                chunkStartAddress, chunkStartAddress + blockSize - 1, 
                setSelected, j
            );
            // Write into said line
            cache[setSelected][j].usage = 1;
            cache[setSelected][j].tag = chunkStartAddress & tagMask;
            cache[setSelected][j].startAddress = chunkStartAddress;
            return;
        }
    }

    /*
        If all lines are occupied, convict on LRU
    */
    int lineToConvict = 0;
    for (int k = 1; k < associativity; k++) {
        if (cache[setSelected][k].usage < cache[setSelected][lineToConvict].usage) {
            lineToConvict = k;
        }
    }
    printf(
        "Miss-Convict! %X : %X can be written into set %d, line %d after conviction\n",
        chunkStartAddress, chunkStartAddress + blockSize - 1, 
        setSelected, lineToConvict
    );
    // Write into said line
    cache[setSelected][lineToConvict].usage = 1;
    cache[setSelected][lineToConvict].tag = chunkStartAddress & tagMask;
    cache[setSelected][lineToConvict].startAddress = chunkStartAddress;
    return;
}