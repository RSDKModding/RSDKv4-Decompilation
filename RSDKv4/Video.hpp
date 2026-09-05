#ifndef VIDEO_H
#define VIDEO_H

#include "theoraplay.h"

class Video
{
public:
    enum PlayState {
        VIDEO_NOT_PLAYING,
        VIDEO_PLAYING_OGV,
        VIDEO_PLAYING_RSV,
    };

    static long CallbackRead(THEORAPLAY_Io *io, void *buffer, long bufferLength);
    static void CallbackClose(THEORAPLAY_Io *io);

    static void NativePlayVideo(int *unused, char *name);

    void PlayFile(char *filepath);
    void UpdateFrame();
    int Process();
    void StopPlayback();
    void InitializeDecoder();
    void SetupBuffer(int width, int height);
    void CloseBuffer();

    int state;
    int frameID;
    int frameMS;
    int frameCount;
    byte surfaceID;
    int fileReadPos;
    int startTicks;
    int width;
    int height;
    float aspect;
    bool skipped;

    THEORAPLAY_Decoder *decoder;
    const THEORAPLAY_VideoFrame *frameData;
    const THEORAPLAY_AudioPacket *audioData;
    THEORAPLAY_VideoFormat format;
    THEORAPLAY_Io callbacks;
};

#endif //! VIDEO_H