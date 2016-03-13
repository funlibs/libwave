/*
 * CWAVE Copyright (c) 2016 Sebastien Serre <ssbx@sysmo.io>.
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
#ifndef CWAVE_H
#define CWAVE_H

#include <inttypes.h>

typedef struct cwave_info_t {

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

} CWAVE_INFO;


void* cwaveOpen(char* fileName, CWAVE_INFO* info);

#endif

