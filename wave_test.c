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

