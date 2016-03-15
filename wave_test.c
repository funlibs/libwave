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
#include "wave.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>


int main(int argc, char* argv[])
{

    if (argc < 3) exit(1);


    WAVE_INFO info;
    void* data = waveLoad(argv[1], &info);


    int return_status;

    // if test must fail
    if (strncmp(argv[2], "fail", 4) == 0) {

        // test must fail
        if (data == NULL) { // NULL is ok

            return_status = 0;

        } else {

            free(data);
            return_status = 1;

        }

    } else {

        // test must succeed
        if (data == NULL) { // NULL is not ok

            return_status = 1;

        } else {

            free(data);
            return_status = 0;

        }

    }


    printf("Read wav file status: success %d\n", info.nChannels);

    return return_status;

}

