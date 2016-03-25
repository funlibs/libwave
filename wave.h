/*
 * LIBWAVE Copyright (c) 2016 Sebastien Serre <ssbx@sysmo.io>.
 *
 * This file is provided to you under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See
 * the License for the specific language governing permissions and limitations
 * under the License.
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

