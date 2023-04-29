#ifndef VIDEO_H
#define VIDEO_H

#include "theoraplay.h"

extern int currentVideoFrame;
extern int videoFrameCount;
extern int videoWidth;
extern int videoHeight;
extern float videoAR;

extern THEORAPLAY_Decoder *videoDecoder;
extern const THEORAPLAY_VideoFrame *videoVidData;
extern const THEORAPLAY_AudioPacket *videoAudioData;
extern THEORAPLAY_Io callbacks;

extern byte videoSurface;
extern int videoFilePos;
extern int videoPlaying; // 0 = not playing, 1 = playing OGV, 2 = playing RSV
extern int vidFrameMS;
extern int vidBaseticks;
extern float videoAR;

void PlayVideoFile(char *filepath);
void UpdateVideoFrame();
int ProcessVideo();
void StopVideoPlayback();

void SetupVideoBuffer(int width, int height);
void CloseVideoBuffer();

#endif // !VIDEO_H
