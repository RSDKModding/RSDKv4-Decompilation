/**
 * TheoraPlay; multithreaded Ogg Theora/Ogg Vorbis decoding.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */

/*
 * This is meant to be a big, robust test case that handles lots of strange
 *  variations. If you want a dirt simple version, try sdlsimple.c
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define SUPPORT_OPENGL 1

#if SUPPORT_OPENGL
#define GL_GLEXT_LEGACY 0
#define GL_GLEXT_PROTOTYPES 1
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#endif

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
            free((void *) item);
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
    AudioQueue *item = NULL;

    if (!audio)
        return;

    item = (AudioQueue *) malloc(sizeof (AudioQueue));
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


static Uint32 sdlyuvfmt(const THEORAPLAY_VideoFormat vidfmt)
{
    switch (vidfmt)
    {
        case THEORAPLAY_VIDFMT_YV12:
            return SDL_YV12_OVERLAY;
        case THEORAPLAY_VIDFMT_IYUV:
            return SDL_IYUV_OVERLAY;
        default: break;
    } // switch

    return 0;
} // need_overlay


static void setcaption(const char *fname, const int opengl,
                       const THEORAPLAY_VideoFormat vidfmt,
                       const THEORAPLAY_VideoFrame *video,
                       const THEORAPLAY_AudioPacket *audio)
{
    char buf[1024];
    const char *fmtstr = "???";
    const char *basefname = NULL;
    const char *renderer = opengl ? "OpenGL" : "Software";

    basefname = strrchr(fname, '/');
    if (!basefname)
        basefname = fname;
    else
        basefname++;

    switch (vidfmt)
    {
        case THEORAPLAY_VIDFMT_RGB:  fmtstr = "RGB";  break;
        case THEORAPLAY_VIDFMT_RGBA: fmtstr = "RGBA"; break;
        case THEORAPLAY_VIDFMT_YV12: fmtstr = "YV12"; break;
        case THEORAPLAY_VIDFMT_IYUV: fmtstr = "IYUV"; break;
        default: assert(0 && "Unexpected video format!"); break;
    } // switch

    if (!audio && !video)
        snprintf(buf, sizeof (buf), "%s (no video, no audio)", basefname);
    else if (audio && video)
    {
        snprintf(buf, sizeof (buf), "%s (%ux%u, %.2gfps, %s %s, %uch, %uHz)",
                 basefname, video->width, video->height, video->fps,
                 renderer, fmtstr, audio->channels, audio->freq);
    } // else if
    else if (!audio && video)
    {
        snprintf(buf, sizeof (buf), "%s (%ux%u, %ffps, %s %s, no audio)",
                 basefname, video->width, video->height, video->fps,
                 renderer, fmtstr);
    } // else if
    else if (audio && !video)
    {
        snprintf(buf, sizeof (buf), "%s (no video, %uch, %uHz)",
                 basefname, audio->channels, audio->freq);
    } // else if

    printf("%s\n", buf);

    if (video)
        SDL_WM_SetCaption(buf, basefname);
} // setcaption


#if SUPPORT_OPENGL
static const char *glsl_vertex =
    "#version 110\n"
    "attribute vec2 pos;\n"
    "attribute vec2 tex;\n"
    "void main() {\n"
        "gl_Position = vec4(pos.xy, 0.0, 1.0);\n"
        "gl_TexCoord[0].xy = tex;\n"
    "}\n";

static const char *glsl_rgba_fragment =
    "#version 110\n"
    "uniform sampler2D samp;\n"
    "void main() { gl_FragColor = texture2D(samp, gl_TexCoord[0].xy); }\n";

// This shader was originally from SDL 1.3.
static const char *glsl_yuv_fragment =
    "#version 110\n"
    "uniform sampler2D samp0;\n"
    "uniform sampler2D samp1;\n"
    "uniform sampler2D samp2;\n"
    "const vec3 offset = vec3(-0.0625, -0.5, -0.5);\n"
    "const vec3 Rcoeff = vec3(1.164,  0.000,  1.596);\n"
    "const vec3 Gcoeff = vec3(1.164, -0.391, -0.813);\n"
    "const vec3 Bcoeff = vec3(1.164,  2.018,  0.000);\n"
    "void main() {\n"
    "    vec2 tcoord;\n"
    "    vec3 yuv, rgb;\n"
    "    tcoord = gl_TexCoord[0].xy;\n"
    "    yuv.x = texture2D(samp0, tcoord).r;\n"
    "    yuv.y = texture2D(samp1, tcoord).r;\n"
    "    yuv.z = texture2D(samp2, tcoord).r;\n"
    "    yuv += offset;\n"
    "    rgb.r = dot(yuv, Rcoeff);\n"
    "    rgb.g = dot(yuv, Gcoeff);\n"
    "    rgb.b = dot(yuv, Bcoeff);\n"
    "    gl_FragColor = vec4(rgb, 1.0);\n"
    "}\n";

static int init_shaders(const THEORAPLAY_VideoFormat vidfmt)
{
    const char *vertexsrc = glsl_vertex;
    const char *fragmentsrc = NULL;
    GLuint vertex = 0;
    GLuint fragment = 0;
    GLuint program = 0;
    GLint ok = 0;
    GLint shaderlen = 0;

    switch (vidfmt)
    {
        case THEORAPLAY_VIDFMT_RGB:
        case THEORAPLAY_VIDFMT_RGBA:
            fragmentsrc = glsl_rgba_fragment;
            break;
        case THEORAPLAY_VIDFMT_YV12:
        case THEORAPLAY_VIDFMT_IYUV:
            fragmentsrc = glsl_yuv_fragment;
            break;
        default: return 0;
    } // switch

    ok = 0;
    shaderlen = (GLint) strlen(vertexsrc);
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, (const GLchar **) &vertexsrc, &shaderlen);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        glDeleteShader(vertex);
        return 0;
    } // if

    ok = 0;
    shaderlen = (GLint) strlen(fragmentsrc);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, (const GLchar **) &fragmentsrc, &shaderlen);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        glDeleteShader(fragment);
        return 0;
    } // if

    ok = 0;
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glBindAttribLocation(program, 0, "pos");
    glBindAttribLocation(program, 1, "tex");
    glLinkProgram(program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        glDeleteProgram(program);
        return 0;
    } // if

    glUseProgram(program);

    if (fragmentsrc == glsl_rgba_fragment)
        glUniform1i(glGetUniformLocation(program, "samp"), 0);
    else if (fragmentsrc == glsl_yuv_fragment)
    {
        glUniform1i(glGetUniformLocation(program, "samp0"), 0);
        glUniform1i(glGetUniformLocation(program, "samp1"), 1);
        glUniform1i(glGetUniformLocation(program, "samp2"), 2);
    } // else if

    return 1;
} // init_shaders

static void prep_texture(GLuint *texture, const int idx)
{
    glActiveTexture(GL_TEXTURE0 + idx);
    glBindTexture(GL_TEXTURE_2D, texture[idx]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
} // prep_texture


static void init_textures(const THEORAPLAY_VideoFrame *video,
                          const GLenum glfmt, const GLenum gltype,
                          GLuint *texture)
{
    const int planar = (sdlyuvfmt(video->format) != 0);

    glGenTextures(planar ? 3 : 1, texture);
    prep_texture(texture, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, glfmt, video->width, video->height, 0,
                 glfmt, gltype, NULL);

    if (planar)
    {
        prep_texture(texture, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, glfmt, video->width / 2,
                     video->height / 2, 0, glfmt, gltype, NULL);
        prep_texture(texture, 2);
        glTexImage2D(GL_TEXTURE_2D, 0, glfmt, video->width / 2,
                     video->height / 2, 0, glfmt, gltype, NULL);
    } // if
} // init_textures

static void openglfmt(const THEORAPLAY_VideoFrame *video,
                      GLenum *glfmt, GLenum *gltype)
{
    switch (video->format)
    {
        case THEORAPLAY_VIDFMT_RGB:
            *glfmt = GL_RGB;
            *gltype = GL_UNSIGNED_BYTE;
            break;
        case THEORAPLAY_VIDFMT_RGBA:
            *glfmt = GL_RGBA;
            *gltype = GL_UNSIGNED_INT_8_8_8_8_REV;
            break;
        case THEORAPLAY_VIDFMT_YV12:
        case THEORAPLAY_VIDFMT_IYUV:
            *glfmt = GL_LUMINANCE;
            *gltype = GL_UNSIGNED_BYTE;
            break;
    } // switch
} // openglfmt

#endif  // SUPPORT_OPENGL


static void queue_more_audio(THEORAPLAY_Decoder *decoder, const Uint32 now)
{
    const THEORAPLAY_AudioPacket *audio;
    while ((audio = THEORAPLAY_getAudio(decoder)) != NULL)
    {
        const unsigned int playms = audio->playms;
        //printf("Got %d frames of audio (%u ms)!\n", audio->frames, audio->playms);
        queue_audio(audio);
        if (playms >= now + 2000)  // don't let this get too far ahead.
            break;
    } // while
} // queue_more_audio


static void playfile(const char *fname, const THEORAPLAY_VideoFormat vidfmt,
                     const int fullscreen, const int opengl)
{
    const int MAX_FRAMES = 30;
    THEORAPLAY_Decoder *decoder = NULL;
    const THEORAPLAY_VideoFrame *video = NULL;
    const THEORAPLAY_AudioPacket *audio = NULL;
    Uint32 vidmodeflags = 0;
    SDL_Surface *screen = NULL;
    SDL_Surface *shadow = NULL;
    SDL_Overlay *overlay = NULL;
    int has_audio = 0;
    int has_video = 0;
    Uint32 sdlinitflags = 0;
    #if SUPPORT_OPENGL
    GLenum glfmt = GL_NONE;
    GLenum gltype = GL_NONE;
    GLuint texture[3] = { 0, 0, 0 };
    #endif
    SDL_Event event;
    Uint32 framems = 0;
    int opened_audio = 0;
    int initfailed = 0;
    int planar = 0;
    int quit = 0;

    printf("Trying file '%s' ...\n", fname);

    decoder = THEORAPLAY_startDecodeFile(fname, MAX_FRAMES, vidfmt);
    if (!decoder)
    {
        fprintf(stderr, "Failed to start decoding '%s'!\n", fname);
        return;
    } // if

    // Wait until the decoder has parsed out some basic truths from the
    //  file. In a video game, you could choose not to block on this, and
    //  instead do something else until the file is ready.
    while (!THEORAPLAY_isInitialized(decoder))
        SDL_Delay(10);

    // Once we're initialized, we can tell if this file has audio and/or video.
    has_audio = THEORAPLAY_hasAudioStream(decoder);
    has_video = THEORAPLAY_hasVideoStream(decoder);

    if (has_video)
        sdlinitflags |= SDL_INIT_VIDEO;
    if (has_audio)
        sdlinitflags |= SDL_INIT_AUDIO;

    if (SDL_Init(sdlinitflags) == -1)
    {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return;
    } // if

    // wait until we have video and/or audio data, so we can set up hardware.
    if (has_video)
    {
        while ((video = THEORAPLAY_getVideo(decoder)) == NULL)
            SDL_Delay(10);
    } // if

    if (has_audio)
    {
        while ((audio = THEORAPLAY_getAudio(decoder)) == NULL)
        {
            if ((has_video) && (THEORAPLAY_availableVideo(decoder) >= MAX_FRAMES))
                break;  // we'll never progress, there's no audio yet but we've prebuffered as much as we plan to.
            SDL_Delay(10);
        } // while
    } // if

    setcaption(fname, opengl, vidfmt, video, audio);

    if (has_video)
    {
        const Uint32 overlayfmt = sdlyuvfmt(vidfmt);
        planar = (overlayfmt != 0);
        framems = (video->fps == 0.0) ? 0 : ((Uint32) (1000.0 / video->fps));

        vidmodeflags = 0;

        if (fullscreen)
        {
            vidmodeflags |= SDL_FULLSCREEN;
            SDL_ShowCursor(0);
        } // if

        if (opengl)
        {
            vidmodeflags |= (SDL_OPENGL | SDL_RESIZABLE);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        } // if

        screen = SDL_SetVideoMode(video->width, video->height, 0, vidmodeflags);

        initfailed = quit = (initfailed || !screen);

        if (!screen)
            fprintf(stderr, "SDL_SetVideoMode() failed: %s\n", SDL_GetError());

        #if SUPPORT_OPENGL
        else if (opengl)
        {
            static struct { float pos[2]; float tex[2]; } verts[4] = {
                { { -1.0f,  1.0f }, { 0.0f, 0.0f } },
                { {  1.0f,  1.0f }, { 1.0f, 0.0f } },
                { { -1.0f, -1.0f }, { 0.0f, 1.0f } },
                { {  1.0f, -1.0f }, { 1.0f, 1.0f } }
            };

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            SDL_GL_SwapBuffers();
            glClear(GL_COLOR_BUFFER_BIT);
            SDL_GL_SwapBuffers();
            glClear(GL_COLOR_BUFFER_BIT);
            SDL_GL_SwapBuffers();

            openglfmt(video, &glfmt, &gltype);

            initfailed = quit = (initfailed || !init_shaders(vidfmt));
            if (!initfailed)
            {
                glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof (verts[0]), &verts[0].pos[0]);
                glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof (verts[0]), &verts[0].tex[0]);
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);

                glDepthMask(GL_FALSE);
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_ALPHA_TEST);
                glDisable(GL_BLEND);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

                init_textures(video, glfmt, gltype, texture);
            } // if
        } // else if
        #endif

        else  // software surface
        {
            // blank out the screen to start.
            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
            SDL_Flip(screen);

            if (planar)
            {
                overlay = SDL_CreateYUVOverlay(video->width, video->height,
                                               overlayfmt, screen);

                if (!overlay)
                    fprintf(stderr, "YUV Overlay failed: %s\n", SDL_GetError());
                initfailed = quit = (initfailed || !overlay);
            } // if
            else
            {
                const int alpha = (vidfmt == THEORAPLAY_VIDFMT_RGBA);
                const int bits = 24 + (alpha * 8);
                const Uint32 rmask = SDL_SwapLE32(0xFF0000FF);
                const Uint32 gmask = SDL_SwapLE32(0x0000FF00);
                const Uint32 bmask = SDL_SwapLE32(0x00FF0000);
                const Uint32 amask = 0x00000000;
                shadow = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                              video->width, video->height,
                                              bits, rmask, gmask, bmask, amask);
                if (!shadow)
                    fprintf(stderr, "Shadow surface failed: %s\n", SDL_GetError());

                assert(!shadow || !SDL_MUSTLOCK(shadow));
                assert(!shadow || (shadow->pitch == (video->width * (bits/8))));
                initfailed = quit = (initfailed || !shadow);
            } // else
        } // else
    } // if

    baseticks = SDL_GetTicks();

    while (!quit && THEORAPLAY_isDecoding(decoder))
    {
        const Uint32 now = SDL_GetTicks() - baseticks;

        // Open the audio device as soon as we know what it should be.
        if ((has_audio) && (!opened_audio))
        {
            if (!audio)
                audio = THEORAPLAY_getAudio(decoder);
            if (audio)
            {
                SDL_AudioSpec spec;
                memset(&spec, '\0', sizeof (SDL_AudioSpec));
                spec.freq = audio->freq;
                spec.format = AUDIO_S16SYS;
                spec.channels = audio->channels;
                spec.samples = 2048;
                spec.callback = audio_callback;
                initfailed = quit = (initfailed || (SDL_OpenAudio(&spec, NULL) != 0));
                if (!quit)
                {
                    // queue some audio to start.
                    opened_audio = 1;
                    queue_audio(audio);
                    audio = NULL;
                    queue_more_audio(decoder, 0);
                    SDL_PauseAudio(0);  // start audio playback!
                } // if
            } // if
        } // if

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

            #if SUPPORT_OPENGL
            else if (opengl)
            {
                GLint w = video->width;
                GLint h = video->height;
                const Uint8 *pix = (const Uint8 *) video->pixels;

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture[0]);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, glfmt, gltype, pix);
                if (planar)
                {
                    const int swapped = (vidfmt == THEORAPLAY_VIDFMT_YV12);
                    pix += w * h;
                    glActiveTexture(swapped ? GL_TEXTURE2 : GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, texture[1]);
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w / 2, h / 2, glfmt, gltype, pix);
                    pix += (w / 2) * (h / 2);
                    glActiveTexture(swapped ? GL_TEXTURE1 : GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, texture[2]);
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w / 2, h / 2, glfmt, gltype, pix);
                } // if

                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                SDL_GL_SwapBuffers();
            } // else if
            #endif

            else if (!overlay)  // not a YUV thing.
            {
                memcpy(shadow->pixels, video->pixels, shadow->h * shadow->pitch);
                // shadow is a software surface, and is thus never "lost".
                //  Keep trying if the screen surface is lost, though.
                while (SDL_BlitSurface(shadow, NULL, screen, NULL) == -2)
                    SDL_Delay(1000);
                SDL_Flip(screen);
            } // else if

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

        if (opened_audio)
            queue_more_audio(decoder, now);

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
                    #if SUPPORT_OPENGL
                    else if (opengl)
                    {
                        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                        SDL_GL_SwapBuffers();
                    } // else if
                    #endif
                    break;

                #if SUPPORT_OPENGL
                case SDL_VIDEORESIZE:
                    assert(opengl);
                    screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 0, vidmodeflags);
                    glViewport(0, 0, event.resize.w, event.resize.h);
                    glScissor(0, 0, event.resize.w, event.resize.h);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                    SDL_GL_SwapBuffers();
                    break;
                #endif

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

    if (opened_audio)
    {
        while (!quit)
        {
            SDL_LockAudio();
            quit = (audio_queue == NULL);
            SDL_UnlockAudio();
            if (!quit)
                SDL_Delay(100);  // wait for final audio packets to play out.
        } // while
    } // if

    if (initfailed)
        printf("Initialization failed!\n");
    else if (THEORAPLAY_decodingError(decoder))
        printf("There was an error decoding this file!\n");
    else
        printf("done with this file!\n");

    if (shadow) SDL_FreeSurface(shadow);
    if (overlay) SDL_FreeYUVOverlay(overlay);
    if (video) THEORAPLAY_freeVideo(video);
    if (audio) THEORAPLAY_freeAudio(audio);
    if (decoder) THEORAPLAY_stopDecode(decoder);
    if (opened_audio) SDL_CloseAudio();
    SDL_Quit();
} // playfile

int main(int argc, char **argv)
{
    THEORAPLAY_VideoFormat vidfmt = THEORAPLAY_VIDFMT_YV12;
    int fullscreen = 0;
    int opengl = 0;
    int i;

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--rgb") == 0)
            vidfmt = THEORAPLAY_VIDFMT_RGB;
        else if (strcmp(argv[i], "--rgba") == 0)
            vidfmt = THEORAPLAY_VIDFMT_RGBA;
        else if (strcmp(argv[i], "--yv12") == 0)
            vidfmt = THEORAPLAY_VIDFMT_YV12;
        else if (strcmp(argv[i], "--iyuv") == 0)
            vidfmt = THEORAPLAY_VIDFMT_IYUV;
        #if SUPPORT_OPENGL
        else if (strcmp(argv[i], "--opengl") == 0)
            opengl = 1;
        #endif
        else if (strcmp(argv[i], "--software") == 0)
            opengl = 0;
        else if (strcmp(argv[i], "--fullscreen") == 0)
            fullscreen = 1;
        else if (strcmp(argv[i], "--windowed") == 0)
            fullscreen = 0;
        else
            playfile(argv[i], vidfmt, fullscreen, opengl);
    } // for

    printf("done all files!\n");

    return 0;
} // main

// end of sdltheoraplay.c ...

