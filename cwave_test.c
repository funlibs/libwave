#include "cwave.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>


int main(int argc, char* argv[])
{

    if (argc < 3) exit(1);

    int shouldFail = strncmp(argv[2], "fail", 4);

    CWAVE_DATA* data = cwaveOpen(argv[1]);

    if (data == NULL) {
        if (shouldFail != 0) {
            printf("Read wav file status: failure\n");
            return 1;
        } else {
            printf("Read wav file status: success\n");
            return 0;
        }
    }

    printf("Read wav file status: success\n");
    cwaveFree(data);
    return 0;

}

