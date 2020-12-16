#include "RetroEngine.hpp"

int currentVideoFrame = 0;
int videoFrameCount = 0;
int videoWidth  = 0;
int videoHeight       = 0;

THEORAPLAY_Decoder *videoDecoder;
const THEORAPLAY_VideoFrame *videoVidData;
const THEORAPLAY_AudioPacket *videoAudioData;
THEORAPLAY_Io callbacks;

byte videoData = 0;
int videoFilePos = 0;
bool videoPlaying = 0;
int vidFrameMS = 0;
int vidBaseticks = 0;


bool videoSkipped = false;

static long videoRead(THEORAPLAY_Io *io, void *buf, long buflen)
{
    FILE *file      = (FILE *)io->userdata;
    const size_t br = fread(buf, 1, buflen * sizeof(byte), file);
    if (br == 0)
        return -1;
    return (int)br;
} // IoFopenRead

static void videoClose(THEORAPLAY_Io *io)
{
    FILE *file = (FILE *)io->userdata;
    fclose(file);
}

void PlayVideoFile(char *filePath) { 
    char filepath[0x100];
    StrCopy(filepath, "videos/");
    StrAdd(filepath, filePath);
    StrAdd(filepath, ".ogv");

    FILE *file = fopen(filepath, "rb");
    if (file) {
#if RSDK_DEBUG 
        printf("Loaded File '%s'!\n", filepath);
#endif

        callbacks.read     = videoRead;
        callbacks.close    = videoClose;
        callbacks.userdata = (void *)file;
        videoDecoder       = THEORAPLAY_startDecode(&callbacks, /*FPS*/ 30, THEORAPLAY_VIDFMT_RGBA);

        if (!videoDecoder) {
#if RSDK_DEBUG
            printf("Video Decoder Error!\n");
#endif
            return;
        }
        while (!videoAudioData || !videoVidData) {
            if (!videoAudioData)
                videoAudioData = THEORAPLAY_getAudio(videoDecoder);
            if (!videoVidData)
                videoVidData = THEORAPLAY_getVideo(videoDecoder);
        }
        if (!videoAudioData || !videoVidData) {
#if RSDK_DEBUG
            printf("Video or Audio Error!\n");
#endif
            return;
        }

        //clear audio data, we dont use it
        while ((videoAudioData = THEORAPLAY_getAudio(videoDecoder)) != NULL) THEORAPLAY_freeAudio(videoAudioData);

        videoWidth  = videoVidData->width;
        videoHeight = videoVidData->height;
        SetupVideoBuffer(videoWidth, videoHeight);
        vidBaseticks = SDL_GetTicks();
        vidFrameMS     = (videoVidData->fps == 0.0) ? 0 : ((Uint32)(1000.0 / videoVidData->fps));
        videoPlaying = true;
        trackID        = TRACK_COUNT - 1;

        // "temp" but I really cannot be bothered to go through the nightmare that is streaming the audio data
        // (yes I tried, and probably cut years off my life)
        StrCopy(filepath, "videos/");
        StrAdd(filepath, filePath);
        StrAdd(filepath, ".ogg");

        TrackInfo *track = &musicTracks[trackID];
        StrCopy(track->fileName, filepath);
        track->trackLoop = false;
        track->loopPoint = 0;

        //Switch it off so the reader can access it
        bool df              = Engine.usingDataFile;
        Engine.usingDataFile = false;
        PlayMusic(trackID);
        Engine.usingDataFile = df;

        videoSkipped = false;

        Engine.gameMode = ENGINE_VIDEOWAIT;
    }
#if RSDK_DEBUG
    else {
        printf("Couldn't find file '%s'!\n", filepath);
    }
#endif
    
}

void UpdateVideoFrame()
{
    if (videoPlaying) {
        if (videoFrameCount > currentVideoFrame) {
            GFXSurface *surface = &gfxSurface[videoData];
            int fileBuffer               = 0;
            FileRead(&fileBuffer, 1);
            videoFilePos += fileBuffer;
            FileRead(&fileBuffer, 1);
            videoFilePos += fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            videoFilePos += fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            videoFilePos += fileBuffer << 24;

            byte clr[3];
            for (int i = 0; i < 0x80; ++i) {
                FileRead(&clr, 3);
                activePalette32[i].r = clr[0];
                activePalette32[i].g = clr[1];
                activePalette32[i].b = clr[2];
                activePalette[i]     = ((ushort)(clr[0] >> 3) << 11) | 32 * (clr[1] >> 2) | (clr[2] >> 3);
            }

            FileRead(&fileBuffer, 1);
            while (fileBuffer != ',') FileRead(&fileBuffer, 1); // gif image start identifier

            FileRead(&fileBuffer, 2); // IMAGE LEFT
            FileRead(&fileBuffer, 2); // IMAGE TOP
            FileRead(&fileBuffer, 2); // IMAGE WIDTH
            FileRead(&fileBuffer, 2); // IMAGE HEIGHT
            FileRead(&fileBuffer, 1); // PaletteType
            bool interlaced = (fileBuffer & 0x40) >> 6;
            if (fileBuffer >> 7 == 1) {
                int c = 0x80;
                do {
                    ++c;
                    FileRead(&fileBuffer, 3);
                } while (c != 0x100);
            }
            ReadGifPictureData(surface->width, surface->height, interlaced,
                                       graphicData, surface->dataPosition);

            SetFilePosition(videoFilePos);
            ++currentVideoFrame;
        }
        else {
            videoPlaying = 0;
            CloseFile();
        }
    }
}

int ProcessVideo()
{
    if (videoPlaying) {
        CheckKeyPress(&keyPress, 0x10);

        if (videoSkipped && fadeMode < 0xFF) {
            fadeMode += 8;
        }

        if (keyPress.A) {
            if (!videoSkipped) 
                fadeMode = 0;

            videoSkipped = true;
        }

        if (!THEORAPLAY_isDecoding(videoDecoder) || (videoSkipped && fadeMode >= 0xFF)) {
            if (videoSkipped && fadeMode >= 0xFF)
                fadeMode = 0;

            if (videoVidData)
                THEORAPLAY_freeVideo(videoVidData);
            if (videoAudioData)
                THEORAPLAY_freeAudio(videoAudioData);
            if (videoDecoder)
                THEORAPLAY_stopDecode(videoDecoder);

            CloseVideoBuffer();
            videoPlaying = false;

            return 1; // video finished
        }

        // Don't pause or it'll go wild
        if (videoPlaying) {
            const Uint32 now = (SDL_GetTicks() - vidBaseticks);

            if (!videoVidData)
                videoVidData = THEORAPLAY_getVideo(videoDecoder);

            // Play video frames when it's time.
            if (videoVidData && (videoVidData->playms <= now)) {
                if (vidFrameMS && ((now - videoVidData->playms) >= vidFrameMS)) {

                    // Skip frames to catch up, but keep track of the last one+
                    //  in case we catch up to a series of dupe frames, which
                    //  means we'd have to draw that final frame and then wait for
                    //  more.

                    const THEORAPLAY_VideoFrame *last = videoVidData;
                    while ((videoVidData = THEORAPLAY_getVideo(videoDecoder)) != NULL) {
                        THEORAPLAY_freeVideo(last);
                        last = videoVidData;
                        if ((now - videoVidData->playms) < vidFrameMS)
                            break;
                    }

                    if (!videoVidData)
                        videoVidData = last;
                }

                // do nothing; we're far behind and out of options.
                if (!videoVidData) {
                    // video lagging uh oh
                }

                memset(Engine.videoFrameBuffer, 0, (videoWidth * videoHeight) * sizeof(uint));
                uint px = 0;
                for (uint i = 0; i < (videoWidth * videoHeight) * sizeof(uint); i += sizeof(uint)) {
                    Engine.videoFrameBuffer[px++] = (videoVidData->pixels[i + 3] << 24 | videoVidData->pixels[i] << 16
                                                     | videoVidData->pixels[i + 1] << 8 | videoVidData->pixels[i + 2] << 0);
                }

                THEORAPLAY_freeVideo(videoVidData);
                videoVidData = NULL;
            }

            //Clear audio data
            while ((videoAudioData = THEORAPLAY_getAudio(videoDecoder)) != NULL) THEORAPLAY_freeAudio(videoAudioData);

            return 2; // its playing as expected
        }
    }

    return 0; // its not even initialised
}


void SetupVideoBuffer(int width, int height) {
    Engine.videoBuffer = SDL_CreateTexture(Engine.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);

#if RSDK_DEBUG
    if (!Engine.videoBuffer) 
        printf("Failed to create video buffer!\n");
#endif

    Engine.videoFrameBuffer = new uint[width * height];
}
void CloseVideoBuffer() {
    if (videoPlaying) {
        if (Engine.videoFrameBuffer) {
            delete[] Engine.videoFrameBuffer;
            Engine.videoFrameBuffer = nullptr;
        }

        SDL_DestroyTexture(Engine.videoBuffer);
        Engine.videoBuffer = nullptr;
    }
}