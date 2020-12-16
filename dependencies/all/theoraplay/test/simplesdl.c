/**
 * TheoraPlay; multithreaded Ogg Theora/Ogg Vorbis decoding.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */

/*
 * This is meant to be a dirt simple test case. If you want a big, robust
 *  version that handles lots of strange variations, try sdltheoraplay.c
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "theoraplay.h"
#include "SDL.h"

static Uint32 baseticks = 0;

typedef struct AudioQueue
{
    const THEORAPLAY_AudioPacket *audio;
    int offset;
    struct AudioQueue *next;
} AudioQueue;

static volatile AudioQueue *audio_queue = NULL;
static volatile AudioQueue *audio_queue_tail = NULL;

static void SDLCALL audio_callback(void *userdata, Uint8 *stream, int len)
{
    // !!! FIXME: this should refuse to play if item->playms is in the future.
    //const Uint32 now = SDL_GetTicks() - baseticks;
    Sint16 *dst = (Sint16 *) stream;

    while (audio_queue && (len > 0))
    {
        volatile AudioQueue *item = audio_queue;
        AudioQueue *next = item->next;
        const int channels = item->audio->channels;

        const float *src = item->audio->samples + (item->offset * channels);
        int cpy = (item->audio->frames - item->offset) * channels;
        int i;

        if (cpy > (len / sizeof (Sint16)))
            cpy = len / sizeof (Sint16);

        for (i = 0; i < cpy; i++)
        {
            const float val = *(src++);
            if (val < -1.0f)
                *(dst++) = -32768;
            else if (val > 1.0f)
                *(dst++) = 32767;
            else
                *(dst++) = (Sint16) (val * 32767.0f);
        } // for

        item->offset += (cpy / channels);
        len -= cpy * sizeof (Sint16);

        if (item->offset >= item->audio->frames)
        {
            THEORAPLAY_freeAudio(item->audio);
            SDL_free((void *) item);
            audio_queue = next;
        } // if
    } // while

    if (!audio_queue)
        audio_queue_tail = NULL;

    if (len > 0)
        memset(dst, '\0', len);
} // audio_callback


static void queue_audio(const THEORAPLAY_AudioPacket *audio)
{
    AudioQueue *item = (AudioQueue *) SDL_malloc(sizeof (AudioQueue));
    if (!item)
    {
        THEORAPLAY_freeAudio(audio);
        return;  // oh well.
    } // if

    item->audio = audio;
    item->offset = 0;
    item->next = NULL;

    SDL_LockAudio();
    if (audio_queue_tail)
        audio_queue_tail->next = item;
    else
        audio_queue = item;
    audio_queue_tail = item;
    SDL_UnlockAudio();
} // queue_audio


static void playfile(const char *fname)
{
    THEORAPLAY_Decoder *decoder = NULL;
    const THEORAPLAY_VideoFrame *video = NULL;
    const THEORAPLAY_AudioPacket *audio = NULL;
    SDL_Surface *screen = NULL;
    SDL_Overlay *overlay = NULL;
    SDL_AudioSpec spec;
    SDL_Event event;
    Uint32 framems = 0;
    int initfailed = 0;
    int quit = 0;

    printf("Trying file '%s' ...\n", fname);

    decoder = THEORAPLAY_startDecodeFile(fname, 30, THEORAPLAY_VIDFMT_IYUV);
    if (!decoder)
    {
        fprintf(stderr, "Failed to start decoding '%s'!\n", fname);
        return;
    } // if

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1)
    {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return;
    } // if

    // wait until we have video and/or audio data, so we can set up hardware.
    while (!audio || !video)
    {
        if (!audio) audio = THEORAPLAY_getAudio(decoder);
        if (!video) video = THEORAPLAY_getVideo(decoder);
        SDL_Delay(10);
    } // if

    SDL_WM_SetCaption(fname, "TheoraPlay");

    framems = (video->fps == 0.0) ? 0 : ((Uint32) (1000.0 / video->fps));
    screen = SDL_SetVideoMode(video->width, video->height, 0, 0);
    if (!screen)
        fprintf(stderr, "SDL_SetVideoMode() failed: %s\n", SDL_GetError());
    else  // software surface
    {
        // blank out the screen to start.
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        SDL_Flip(screen);

        overlay = SDL_CreateYUVOverlay(video->width, video->height,
                                       SDL_IYUV_OVERLAY, screen);
        if (!overlay)
            fprintf(stderr, "YUV Overlay failed: %s\n", SDL_GetError());
    } // else

    initfailed = quit = (!screen || !overlay);

    memset(&spec, '\0', sizeof (SDL_AudioSpec));
    spec.freq = audio->freq;
    spec.format = AUDIO_S16SYS;
    spec.channels = audio->channels;
    spec.samples = 2048;
    spec.callback = audio_callback;
    initfailed = quit = (initfailed || (SDL_OpenAudio(&spec, NULL) != 0));

    while (audio)
    {
        queue_audio(audio);
        audio = THEORAPLAY_getAudio(decoder);
    } // while

    baseticks = SDL_GetTicks();

    if (!quit)
        SDL_PauseAudio(0);

    while (!quit && THEORAPLAY_isDecoding(decoder))
    {
        const Uint32 now = SDL_GetTicks() - baseticks;

        if (!video)
            video = THEORAPLAY_getVideo(decoder);

        // Play video frames when it's time.
        if (video && (video->playms <= now))
        {
            //printf("Play video frame (%u ms)!\n", video->playms);
            if ( framems && ((now - video->playms) >= framems) )
            {
                // Skip frames to catch up, but keep track of the last one
                //  in case we catch up to a series of dupe frames, which
                //  means we'd have to draw that final frame and then wait for
                //  more.
                const THEORAPLAY_VideoFrame *last = video;
                while ((video = THEORAPLAY_getVideo(decoder)) != NULL)
                {
                    THEORAPLAY_freeVideo(last);
                    last = video;
                    if ((now - video->playms) < framems)
                        break;
                } // while

                if (!video)
                    video = last;
            } // if

            if (!video)  // do nothing; we're far behind and out of options.
            {
                static int warned = 0;
                if (!warned)
                {
                    warned = 1;
                    fprintf(stderr, "WARNING: Playback can't keep up!\n");
                } // if
            } // if

            else if (SDL_LockYUVOverlay(overlay) == -1)
            {
                static int warned = 0;
                if (!warned)
                {
                    warned = 1;
                    fprintf(stderr, "Couldn't lock YUV overlay: %s\n", SDL_GetError());
                } // if
            } // else if

            else
            {
                SDL_Rect dstrect = { 0, 0, video->width, video->height };
                const int w = video->width;
                const int h = video->height;
                const Uint8 *y = (const Uint8 *) video->pixels;
                const Uint8 *u = y + (w * h);
                const Uint8 *v = u + ((w/2) * (h/2));
                Uint8 *dst;
                int i;

                dst = overlay->pixels[0];
                for (i = 0; i < h; i++, y += w, dst += overlay->pitches[0])
                    memcpy(dst, y, w);

                dst = overlay->pixels[1];
                for (i = 0; i < h/2; i++, u += w/2, dst += overlay->pitches[1])
                    memcpy(dst, u, w/2);

                dst = overlay->pixels[2];
                for (i = 0; i < h/2; i++, v += w/2, dst += overlay->pitches[1])
                    memcpy(dst, v, w/2);

                SDL_UnlockYUVOverlay(overlay);

                if (SDL_DisplayYUVOverlay(overlay, &dstrect) != 0)
                {
                    static int warned = 0;
                    if (!warned)
                    {
                        warned = 1;
                        fprintf(stderr, "Couldn't display YUV overlay: %s\n", SDL_GetError());
                    } // if
                } // if
            } // else

            THEORAPLAY_freeVideo(video);
            video = NULL;
        } // if
        else  // no new video frame? Give up some CPU.
        {
            SDL_Delay(10);
        } // else

        while ((audio = THEORAPLAY_getAudio(decoder)) != NULL)
            queue_audio(audio);

        // Pump the event loop here.
        while (screen && SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_VIDEOEXPOSE:
                    if (overlay)
                    {
                        SDL_Rect dstrect = { 0, 0, screen->w, screen->h };
                        SDL_DisplayYUVOverlay(overlay, &dstrect);
                    } // if
                    break;

                case SDL_QUIT:
                    quit = 1;
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        quit = 1;
                    break;
            } // switch
        } // while
    } // while

    // Drain out the audio queue.
    while (!quit)
    {
        SDL_LockAudio();
        quit = (audio_queue == NULL);
        SDL_UnlockAudio();
        if (!quit)
            SDL_Delay(100);  // wait for final audio packets to play out.
    } // while

    if (initfailed)
        printf("Initialization failed!\n");
    else if (THEORAPLAY_decodingError(decoder))
        printf("There was an error decoding this file!\n");
    else
        printf("done with this file!\n");

    if (overlay) SDL_FreeYUVOverlay(overlay);
    if (video) THEORAPLAY_freeVideo(video);
    if (audio) THEORAPLAY_freeAudio(audio);
    if (decoder) THEORAPLAY_stopDecode(decoder);
    SDL_CloseAudio();
    SDL_Quit();
} // playfile

int main(int argc, char **argv)
{
    int i;
    for (i = 1; i < argc; i++)
        playfile(argv[i]);

    printf("done all files!\n");

    return 0;
} // main

// end of sdltheoraplay.c ...

