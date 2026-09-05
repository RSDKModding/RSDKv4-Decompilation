/**
 * TheoraPlay; multithreaded Ogg Theora/Ogg Vorbis decoding.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "theoraplay.h"

static void dofile(const char *fname, const THEORAPLAY_VideoFormat vidfmt)
{
    THEORAPLAY_Decoder *decoder = NULL;
    const THEORAPLAY_VideoFrame *video = NULL;
    const THEORAPLAY_AudioPacket *audio = NULL;

    printf("Trying file '%s' ...\n", fname);
    decoder = THEORAPLAY_startDecodeFile(fname, 20, vidfmt);
    while (THEORAPLAY_isDecoding(decoder))
    {
        video = THEORAPLAY_getVideo(decoder);
        if (video)
        {
            printf("Got video frame (%u ms)!\n", video->playms);
            THEORAPLAY_freeVideo(video);
        } // if

        audio = THEORAPLAY_getAudio(decoder);
        if (audio)
        {
            printf("Got %d frames of audio (%u ms)!\n", audio->frames, audio->playms);
            THEORAPLAY_freeAudio(audio);
        } // if

        if (!video && !audio)
            usleep(10000);
    } // while

    if (THEORAPLAY_decodingError(decoder))
        printf("There was an error decoding this file!\n");
    else
        printf("done with this file!\n");

    THEORAPLAY_stopDecode(decoder);
} // dofile

int main(int argc, char **argv)
{
    THEORAPLAY_VideoFormat vidfmt = THEORAPLAY_VIDFMT_YV12;
    int i;

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--rgb") == 0)
            vidfmt = THEORAPLAY_VIDFMT_RGB;
        else if (strcmp(argv[i], "--rgba") == 0)
            vidfmt = THEORAPLAY_VIDFMT_RGBA;
        else if (strcmp(argv[i], "--yv12") == 0)
            vidfmt = THEORAPLAY_VIDFMT_YV12;
        else
            dofile(argv[i], vidfmt);
    } // for

    printf("done all files!\n");
    return 0;
} // main

// end of testtheoraplay.c ...

