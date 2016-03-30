/*
 * MIT License
 *
 * LIBWAVE Copyright (c) 2016 Sebastien Serre <ssbx@sysmo.io>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file wave.h
 */

#ifndef WAVE_H
#define WAVE_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Information on a wave file
 */
typedef struct {

    // format
    uint16_t nChannels;
    uint32_t nSamplesPerSec;
    uint32_t nAvgBytesPerSec;
    uint16_t nBlockAlign;
    uint16_t wBitsPerSample;

    // extention
    uint16_t wValidBitsPerSample;
    uint32_t dwChannelMask;

    int      dataSize;

} WAVE_INFO;


/**
 * @brief Load a wave file in memory
 * @param fileName The wave file name
 * @param info Pointer to a WAVE_INFO variable
 * @return Pointer to the wave data
 */
void* waveLoad(char* fileName, WAVE_INFO* info);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif

