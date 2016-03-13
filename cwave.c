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

#include "cwave.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

/*
 * From: http://www-mmsp.ece.mcgill.ca/documents/audioformats/wave/wave.html
 *
 * See doc_waveSpecs.pdf for a local version.
 */


/*
 * See doc_waveMultiChan.pdf for more on this
 */
#define SPEAKER_FRONT_LEFT             0x1
#define SPEAKER_FRONT_RIGHT            0x2
#define SPEAKER_FRONT_CENTER           0x4
#define SPEAKER_LOW_FREQUENCY          0x8
#define SPEAKER_BACK_LEFT              0x10
#define SPEAKER_BACK_RIGHT             0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER   0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER  0x80
#define SPEAKER_BACK_CENTER            0x100
#define SPEAKER_SIDE_LEFT              0x200
#define SPEAKER_SIDE_RIGHT             0x400
#define SPEAKER_TOP_CENTER             0x800
#define SPEAKER_TOP_FRONT_LEFT         0x1000
#define SPEAKER_TOP_FRONT_CENTER       0x2000
#define SPEAKER_TOP_FRONT_RIGHT        0x4000
#define SPEAKER_TOP_BACK_LEFT          0x8000
#define SPEAKER_TOP_BACK_CENTER        0x10000
#define SPEAKER_TOP_BACK_RIGHT         0x20000
#define SPEAKER_RESERVED               0x8000000

/*
 * The standard format codes for waveform data
 */
const uint16_t WAVE_FORMAT_PCM        = 0x0001; // PCM
const uint16_t WAVE_FORMAT_IEEE_FLOAT = 0x0003; // IEEE float
const uint16_t WAVE_FORMAT_ALAW       = 0x0006; // 8-bit ITU-T G.711 A-law
const uint16_t WAVE_FORMAT_MULAW      = 0x0007; // 8-bit ITU-T G.711 µ-law
const uint16_t WAVE_FORMAT_EXTENSIBLE = 0xFFFE; // Determined by SubFormat



/*
 * Wave files have a master RIFF chunk which includes a WAVE identifier
 * followed by sub-chunks. The data is stored in little-endian byte order.
 */
typedef struct master_wave_chunk_t
{

    char    ckID[4];        // Chunk ID: "RIFF"
    uint32_t cksize;        // Chunk size: 4 + n

    char        WAVEID[4];  // WAVE ID: "WAVE"
    //FMT_CHUNK* c[n]; Wave chunks containing format information and sampled data

} MASTER_WAVE_CHUNK;


/*
 * GUID type util
 */
typedef struct guid_t {
    uint16_t formatCode;
    char     fixedString[14];
} GUID;


/*
 * The fmt specifies the format of the data. There are 3 variants of the
 * Format chunk for sampled data. These differ in the extensions to the basic
 * fmt chunk.
 */
typedef struct riff_head_t
{

    char     ckID[4];                // Chunk ID: "fmt "
    uint32_t cksize;                 // Chunk size: 40(cbSize=22), 18(cbSize=0), 16(no cbSize)

} RIFF_HEAD;
typedef struct fmt_chunk_t { // max size 40

    uint16_t    wFormatTag;         // Format code (WAVE_FORMAT_*)
    uint16_t    nChannels;          // Number of interleaved channels
    uint32_t    nSamplesPerSec;     // Sampling rate (blocks per second)
    uint32_t    nAvgBytesPerSec;    // Data rate
    uint16_t    nBlockAlign;        // Data block size (bytes)
    uint16_t    wBitsPerSample;     // Bits per sample
    uint16_t    cbSize;             // Size of the extension (0 or 22)
    uint16_t    wValidBitsPerSample;// Number of valid bits
    uint32_t    dwChannelMask;      // Speaker position mask
    GUID        SubFormat;          // GUID, including the data format code

} FMT_CHUNK;


/*
 * PCM Format
 *
 * The first part of the Format chunk is used to describe PCM data
 *
 * - For PCM data, the Format chunk in the header declares the number of
 *   bits/sample in each sample (wBitsPerSample). The original documentation
 *   (Revision 1) specified that the number of bits per sample is to be rounded
 *   up to the next multiple of 8 bits. This rounded-up value is the container
 *   size. This information is redundant in that the container size (in bytes)
 *   for each sample can also be determined from the block size divided by the
 *   number of channels (nBlockAlign / nChannels).
 *    - This redundancy has been appropriated to define new formats. For instance,
 *      Cool Edit uses a format which declares a sample size of 24 bits together
 *      with a container size of 4 bytes (32 bits) determined from the block size
 *      and number of channels. With this combination, the data is actually stored
 *      as 32-bit IEEE floats. The normalization (full scale 223) is however
 *      different from the standard float format.
 * - PCM data is two's-complement except for resolutions of 1-8 bits, which
 *   are represented as offset binary.
 */

/*
 * Extensible Format
 *
 * The WAVE_FORMAT_EXTENSIBLE format code indicates that there is an extension
 * to the Format chunk. The extension has one field which declares the number
 * of "valid" bits/sample (wValidBitsPerSample). Another field (dwChannelMask)
 * contains bits which indicate the mapping from channels to loudspeaker
 * positions. The last field (SubFormat) is a 16-byte globally
 * unique identifier (GUID).
 *
 * - With the WAVE_FORMAT_EXTENSIBLE format, the original bits/sample field
 *   (wBitsPerSample) must match the container size (8 * nBlockAlign / nChannels).
 *   This means that wBitsPerSample must be a multiple of 8. Reduced precision
 *   within the container size is now specified by wValidBitsPerSample.
 * - The number of valid bits (wValidBitsPerSample) is informational only. The
 *   data is correctly represented in the precision of the container size. The
 *   number of valid bits can be any value from 1 to the container size in bits.
 * - The loudspeaker position mask uses 18 bits, each bit corresponding to a
 *   speaker position (e.g. Front Left or Top Back Right), to indicate the
 *   channel to speaker mapping. More details are in the document cited above.
 *   This field is informational. An all-zero field indicates that channels are
 *   mapped to outputs in order: first channel to first output, second channel
 *   to second output, etc.
 * - The first two bytes of the GUID form the sub-code specifying the data
 *   format code, e.g. WAVE_FORMAT_PCM. The remaining 14 bytes contain a fixed
 *   string, "\x00\x00\x00\x00\x10\x00\x80\x00\x00\xAA\x00\x38\x9B\x71".
 *
 * The WAVE_FORMAT_EXTENSIBLE format should be used whenever:
 *
 * - PCM data has more than 16 bits/sample.
 * - The number of channels is more than 2.
 * - The actual number of bits/sample is not equal to the container size.
 * - The mapping from channels to speakers needs to be specified.
 */


/*
 * EXAMPLE for PCM data
 *
 * Consider sampled data with the following parameters,
 *
 *  - Nc channels
 *  - The total number of blocks is Ns. Each block consists of Nc samples.
 *  - Sampling rate F (blocks per second)
 *  - Each sample is M bytes long
 *
 *  FIELD               LENGTH          CONTENTS
 *  ckID                4               Chunk ID: "RIFF"
 *  cksize              4               Chunk size: 4 + 24 + (8 + M*Nc*Ns + (0 or 1))
 *    WAVEID            4               WAVE ID: "WAVE"
 *    ckID              4               Chunk ID: "fmt "
 *    cksize            4               Chunk size: 16
 *      wFormatTag      2               WAVE_FORMAT_PCM
 *      nChannels       2               Nc
 *      nSamplesPerSec 	4               F
 *      nAvgBytesPerSec 4               F*M*Nc
 *      nBlockAlign 	2               M*Nc
 *      wBitsPerSample 	2               rounds up to 8*M
 *    ckID              4               Chunk ID: "data"
 *    cksize            4               Chunk size: M*Nc*Ns
 *      sampled data    M*Nc*Ns         Nc*Ns channel-interleaved M-byte samples
 *      pad byte        0 or 1 	        Padding byte if M*Nc*Ns is odd
 *
 *
 * NOTES: Wave files often have information chunks that precede or follow the sound
 * data (data chunk). Some programs (naively) assume that for PCM data, the
 * preamble in the file header is exactly 44 bytes long (as in the table
 * above) and that the rest of the file contains sound data.
 * This is not a safe assumption.
 */


void cwaveDebugFmt(FMT_CHUNK chunk);

CWAVE_DATA* cwaveOpen(char* fileName)
{

    // try to open the file
    FILE *wavFile = fopen(fileName, "r");

    if (!wavFile) {
        printf("Open file failed\n");
        return NULL;
    }


    // try to read the master header
    MASTER_WAVE_CHUNK wave_chunk;
    if (fread(&wave_chunk, sizeof(MASTER_WAVE_CHUNK), 1, wavFile) < 1) {
        printf("Can not read wave chunk\n");
        return NULL;
    }

    // check indianness consistancy
    volatile uint32_t i = 0x01234567;
    int isLittleIndian = (*((uint8_t*)(&i))) == 0x67;
    if (isLittleIndian) {
        if (
            memcmp(wave_chunk.ckID, "XFIR", 4) == 0 ||
            memcmp(wave_chunk.ckID, "FFIR", 4) == 0)
        {
            printf("Indianness of the file does not match the system one");
            return NULL;
        }
    } else {
        if (
            memcmp(wave_chunk.ckID, "RIFX", 4) == 0 ||
            memcmp(wave_chunk.ckID, "RIFF", 4) == 0)
        {
            printf("Indianness of the file does not match the system one");
            return NULL;
        }
    }

    // read the format chunk and get content lenght
    RIFF_HEAD fmt_chunk_head;
    if (fread(&fmt_chunk_head, sizeof(RIFF_HEAD), 1, wavFile) < 1) {
        printf("Can not read format header chunk\n");
        return NULL;
    }
    if (memcmp(fmt_chunk_head.ckID, "fmt ", 4) != 0) {
        printf("Not an format header\n");
        return NULL;
    }

    int fmtSize = fmt_chunk_head.cksize;

    // max size of FMT_CHUNK
    if (fmtSize > 40) return NULL;

    // read format options
    FMT_CHUNK fmt_chunk = {0,0,0,0,0,0,0,0,0,{0,""}};
    if (fread(&fmt_chunk, fmtSize, 1, wavFile) < 1) {
        printf("Can not read format chunk\n");
        return NULL;
    }

    if (fmt_chunk.wFormatTag != WAVE_FORMAT_PCM) {
        if (fmt_chunk.cbSize != 0) {
            if (fmt_chunk.SubFormat.formatCode != WAVE_FORMAT_PCM) {
                printf ("It is not PCM data\n");
                return NULL;
            }
        }
    }

    /*
     * TODO read dwChannelMask to know wich channel go with wich speaker.
     */
    uint16_t expectedConfigForSurround =
        SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER |
        SPEAKER_BACK_CENTER;

    uint16_t expectedConfigForStereo = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;

    cwaveDebugFmt(fmt_chunk);

    RIFF_HEAD data_chunk = {"", 0};
    if (fread(&data_chunk, sizeof(RIFF_HEAD), 1, wavFile) < 1) {
        printf ("Can not read the data chunk: %s\n", data_chunk.ckID);
        return NULL;
    }

    if (memcmp(data_chunk.ckID, "data", 4) != 0) {
        printf ("It is not a data chunk: %s\n", data_chunk.ckID);
        return NULL;
    }


    // I should be able to fill channel buffers with this:
    int dataSize     = data_chunk.cksize;
    int sampleLen       = fmt_chunk.wBitsPerSample / 8;
    int channelNum      = fmt_chunk.nChannels;
    printf("%d octets divided in %d octets sample lenght for %d channel\n",
           dataSize, sampleLen, channelNum);

    char *wavData = calloc(dataSize, sizeof(char));
    int haveRead = fread(wavData, 1, (size_t) dataSize, wavFile);
    if (haveRead < dataSize) {
        printf("warning unexpected end of file %d %s\n", haveRead, wavData);
    } else {
        printf("Read file success\n");
    }

    fclose(wavFile);

    CWAVE_DATA *cwaveData = calloc(1, sizeof(CWAVE_DATA));
    cwaveData->nChannels            = fmt_chunk.nChannels;
    cwaveData->nSamplesPerSec       = fmt_chunk.nSamplesPerSec;
    cwaveData->nAvgBytesPerSec      = fmt_chunk.nAvgBytesPerSec;
    cwaveData->nBlockAlign          = fmt_chunk.nBlockAlign;
    cwaveData->wBitsPerSample       = fmt_chunk.wBitsPerSample;
    cwaveData->wValidBitsPerSample  = fmt_chunk.wValidBitsPerSample;
    cwaveData->dwChannelMask        = fmt_chunk.dwChannelMask;
    cwaveData->dataSize             = dataSize;
    cwaveData->data                 = wavData;

    return cwaveData;

}

void cwaveFree(CWAVE_DATA* cwaveData)
{

    if (cwaveData == NULL) return;
    if (cwaveData->data != NULL) free(cwaveData->data);
    free(cwaveData);

}

void cwaveDebugFmt(FMT_CHUNK fmt_chunk)
{

    // print format options
    printf("\n\n");
    printf("wFormatTag is:          %10x\n", fmt_chunk.wFormatTag);
    printf("nChannels is:           %10d\n", fmt_chunk.nChannels);
    printf("nSamplesPerSec is:      %10d\n", fmt_chunk.nSamplesPerSec);

    printf("nAvgBytesPerSec is:     %10d\n", fmt_chunk.nAvgBytesPerSec);
    printf("nBlockAlign is:         %10d\n", fmt_chunk.nBlockAlign);
    printf("wBitsPerSample is:      %10d\n", fmt_chunk.wBitsPerSample);
    if (fmt_chunk.cbSize != 0) {
        printf("\nHave extention!\n");
        printf("wValidBitsPerSample is: %10d\n", fmt_chunk.wValidBitsPerSample);
        printf("dwChannelMask is:       %10d\n", fmt_chunk.dwChannelMask);
        printf("SubFormat GUID is:      %10x %s \n",
               fmt_chunk.SubFormat.formatCode,
               fmt_chunk.SubFormat.fixedString);
    }

    printf("\n\n");
}

