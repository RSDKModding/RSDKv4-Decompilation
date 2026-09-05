/**
 * TheoraPlay; multithreaded Ogg Theora/Ogg Vorbis decoding.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */

#ifndef _INCL_THEORAPLAY_H_
#define _INCL_THEORAPLAY_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct THEORAPLAY_Io THEORAPLAY_Io;
struct THEORAPLAY_Io
{
    long (*read)(THEORAPLAY_Io *io, void *buf, long buflen);
    void (*close)(THEORAPLAY_Io *io);
    void *userdata;
};

typedef struct THEORAPLAY_Decoder THEORAPLAY_Decoder;

/* YV12 is YCrCb, not YCbCr; that's what SDL uses for YV12 overlays. */
typedef enum THEORAPLAY_VideoFormat
{
    THEORAPLAY_VIDFMT_YV12,  /* NTSC colorspace, planar YCrCb 4:2:0 */
    THEORAPLAY_VIDFMT_IYUV,  /* NTSC colorspace, planar YCbCr 4:2:0 */
    THEORAPLAY_VIDFMT_RGB,   /* 24 bits packed pixel RGB */
    THEORAPLAY_VIDFMT_RGBA   /* 32 bits packed pixel RGBA (full alpha). */
} THEORAPLAY_VideoFormat;

typedef struct THEORAPLAY_VideoFrame
{
    unsigned int playms;
    double fps;
    unsigned int width;
    unsigned int height;
    THEORAPLAY_VideoFormat format;
    unsigned char *pixels;
    struct THEORAPLAY_VideoFrame *next;
} THEORAPLAY_VideoFrame;

typedef struct THEORAPLAY_AudioPacket
{
    unsigned int playms;  /* playback start time in milliseconds. */
    int channels;
    int freq;
    int frames;
    float *samples;  /* frames * channels float32 samples. */
    struct THEORAPLAY_AudioPacket *next;
} THEORAPLAY_AudioPacket;

THEORAPLAY_Decoder *THEORAPLAY_startDecodeFile(const char *fname,
                                               const unsigned int maxframes,
                                               THEORAPLAY_VideoFormat vidfmt,
                                               unsigned int audio_bitstream);
THEORAPLAY_Decoder *THEORAPLAY_startDecode(THEORAPLAY_Io *io,
                                           const unsigned int maxframes,
                                           THEORAPLAY_VideoFormat vidfmt,
                                           unsigned int audio_bitstream);
void THEORAPLAY_stopDecode(THEORAPLAY_Decoder *decoder);

int THEORAPLAY_isDecoding(THEORAPLAY_Decoder *decoder);
int THEORAPLAY_decodingError(THEORAPLAY_Decoder *decoder);
int THEORAPLAY_isInitialized(THEORAPLAY_Decoder *decoder);
int THEORAPLAY_hasVideoStream(THEORAPLAY_Decoder *decoder);
int THEORAPLAY_hasAudioStream(THEORAPLAY_Decoder *decoder);
unsigned int THEORAPLAY_availableVideo(THEORAPLAY_Decoder *decoder);
unsigned int THEORAPLAY_availableAudio(THEORAPLAY_Decoder *decoder);

const THEORAPLAY_AudioPacket *THEORAPLAY_getAudio(THEORAPLAY_Decoder *decoder);
void THEORAPLAY_freeAudio(const THEORAPLAY_AudioPacket *item);

const THEORAPLAY_VideoFrame *THEORAPLAY_getVideo(THEORAPLAY_Decoder *decoder);
void THEORAPLAY_freeVideo(const THEORAPLAY_VideoFrame *item);

#ifdef __cplusplus
}
#endif

#endif  /* include-once blocker. */

/* end of theoraplay.h ... */

