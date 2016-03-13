#ifndef CWAVE_H
#define CWAVE_H

#include <inttypes.h>

typedef struct cwave_data_t {

    uint16_t nChannels;
    uint32_t nSamplesPerSec;
    uint32_t nAvgBytesPerSec;
    uint16_t nBlockAlign;
    uint16_t wBitsPerSample;

    // extentions
    uint16_t wValidBitsPerSample;
    uint32_t dwChannelMask;

    // samples
    char*    data;
    int      dataSize;

} CWAVE_DATA;


CWAVE_DATA* cwaveOpen(char* fileName);

void cwaveFree(CWAVE_DATA* data);

#endif
