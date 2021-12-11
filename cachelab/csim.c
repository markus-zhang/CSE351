#include "cachelab.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char* argv[])
{
    // command line parsing
    bool helpFlag = false;
    bool verboseFlag = false;
    char traceFile[256];
    int8_t setBits = 0;
    int8_t lineBits = 0;
    int8_t blockBits = 0;

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
            break;
        case 'E':
            printf("Bits for lines per set: %d\n", atoi(optarg));
            break;
        case 'b':
            printf("Bits for each block: %d\n", atoi(optarg));
            break;
        default:
            fprintf(stderr, "Usage: %s [-hv] [-s nsetbits] [-E associativity] [-b blockbits] [-t tracefile]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    // printSummary(0, 0, 0);
    return 0;
}
