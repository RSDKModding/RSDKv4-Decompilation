#include "RetroEngine.hpp"
#include <string>

long Video::CallbackRead(THEORAPLAY_Io *io, void *buffer, long bufferLength)
{
    FileIO *file    = (FileIO *)io->userdata;
    const size_t br = fRead(buffer, 1, bufferLength * sizeof(byte), file);
    if (br == 0)
        return -1;

    return (int)br;
}

void Video::CallbackClose(THEORAPLAY_Io *io)
{
    FileIO *file = (FileIO *)io->userdata;
    fClose(file);
}

void Video::NativePlayVideo(int *unused, char *name)
{
    (void)unused;
    Engine.Video.PlayFile(name);
}

void Video::InitializeDecoder()
{
#if RETRO_USING_OPENGL
    this->format = THEORAPLAY_VIDFMT_RGBA;
#elif RETRO_USING_SDL2
    this->format = THEORAPLAY_VIDFMT_IYUV;
#elif RETRO_USING_SDL1
    // TODO: does SDL 1.2 support YUV?
    this->format = THEORAPLAY_VIDFMT_RGBA;
#endif

    this->decoder = THEORAPLAY_startDecode(&callbacks, /*FPS*/ 30, this->format, GetGlobalVariableByName("options.soundtrack") ? 1 : 0);
}

void Video::PlayFile(char *filePath)
{
    char pathBuffer[0x100] = {};
    int len                = StrLength(filePath);

    if (StrComp(filePath + ((size_t)len - 2), "us")) {
        filePath[len - 2] = 0;
    }

    StrCopy(pathBuffer, "videos/");
    StrAdd(pathBuffer, filePath);
    StrAdd(pathBuffer, ".ogv");

    bool addPath = true;

    // Fixes ".ani" ".Ani" bug and any other case differences
    char pathLower[0x100] = {};
    for (int c = 0; c < strlen(pathBuffer); ++c) {
        pathLower[c] = tolower(pathBuffer[c]);
    }

#if RETRO_USE_MOD_LOADER
    for (int m = 0; m < modList.size(); ++m) {
        if (modList[m].active) {
            std::map<std::string, std::string>::const_iterator iter = modList[m].fileMap.find(pathLower);
            if (iter != modList[m].fileMap.cend()) {
                StrCopy(pathBuffer, iter->second.c_str());
                // Engine.forceFolder   = true;
                Engine.usingDataFile = false;
                addPath              = false;
                break;
            }
        }
    }
#endif

    char filepath[0x100] = {};
    if (addPath == true) {
#if RETRO_PLATFORM == RETRO_UWP
        static char resourcePath[256] = { 0 };

        if (strlen(resourcePath) == 0) {
            auto folder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
            auto path   = to_string(folder.Path());

            std::copy(path.begin(), path.end(), resourcePath);
        }

        sprintf(filepath, "%s/%s", resourcePath, pathBuffer);
#elif RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_ANDROID
        sprintf(filepath, "%s/%s", gamePath, pathBuffer);
#else
        sprintf(filepath, "%s%s", BASE_PATH, pathBuffer);
#endif
    }
    else {
        sprintf(filepath, "%s", pathBuffer);
    }

    FileIO *file = fOpen(filepath, "rb");
    if (file != nullptr) {
        PrintLog("Loaded File '%s'!", filepath);

        callbacks.read     = CallbackRead;
        callbacks.close    = CallbackClose;
        callbacks.userdata = (void *)file;

        this->InitializeDecoder();

        if (!this->decoder) {
            PrintLog("Video Decoder Error!");
            return;
        }

        while (!this->frameData) {
            if (!this->frameData)
                this->frameData = THEORAPLAY_getVideo(this->decoder);
        }

        if (!this->frameData) {
            PrintLog("Video Error!");
            return;
        }

        this->width  = this->frameData->width;
        this->height = this->frameData->height;
        this->aspect = static_cast<float>(this->width) / static_cast<float>(this->height);

        this->SetupBuffer(this->width, this->height);
        this->startTicks = SDL_GetTicks();
        this->frameMS    = (this->frameData->fps == 0.0) ? 0 : ((Uint32)(1000.0 / this->frameData->fps));
        this->state      = VIDEO_PLAYING_OGV;
        trackID          = TRACK_COUNT - 1;

        this->skipped   = false;
        Engine.gameMode = ENGINE_VIDEOWAIT;
    }
    else {
        PrintLog("Couldn't find file '%s'!", filepath);
    }
}

void Video::UpdateFrame()
{
    if (this->state == VIDEO_PLAYING_RSV) {
        if (this->frameID < this->frameCount) {
            GFXSurface *surface = &gfxSurface[this->surfaceID];

            byte fileBuffer    = 0;
            ushort fileBuffer2 = 0;

            FileRead(&fileBuffer, 1);
            this->fileReadPos += fileBuffer;

            FileRead(&fileBuffer, 1);
            this->fileReadPos += fileBuffer << 8;

            FileRead(&fileBuffer, 1);
            this->fileReadPos += fileBuffer << 16;

            FileRead(&fileBuffer, 1);
            this->fileReadPos += fileBuffer << 24;

            byte clr[3] = {};
            for (int i = 0; i < 0x80; ++i) {
                FileRead(&clr, 3);
                activePalette32[i].r = clr[0];
                activePalette32[i].g = clr[1];
                activePalette32[i].b = clr[2];
                activePalette[i]     = ((ushort)(clr[0] >> 3) << 11) | 32 * (clr[1] >> 2) | (clr[2] >> 3);
            }

            FileRead(&fileBuffer, 1);
            while (fileBuffer != ',') {
                FileRead(&fileBuffer, 1); // gif image start identifier
            }

            FileRead(&fileBuffer2, 2); // IMAGE LEFT
            FileRead(&fileBuffer2, 2); // IMAGE TOP
            FileRead(&fileBuffer2, 2); // IMAGE WIDTH
            FileRead(&fileBuffer2, 2); // IMAGE HEIGHT
            FileRead(&fileBuffer, 1);  // PaletteType

            bool interlaced = (fileBuffer & 0x40) >> 6;
            if (fileBuffer >> 7 == 1) {
                int c = 0x80;
                do {
                    ++c;
                    FileRead(&fileBuffer, 1);
                    FileRead(&fileBuffer, 1);
                    FileRead(&fileBuffer, 1);
                } while (c != 0x100);
            }

            ReadGifPictureData(surface->width, surface->height, interlaced, graphicData, surface->dataPosition);
            SetFilePosition(this->fileReadPos);

            ++this->frameID;
        }
        else {
            this->state = VIDEO_NOT_PLAYING;
            CloseFile();
        }
    }
}

int Video::Process()
{
    if (this->state == VIDEO_PLAYING_OGV) {
        CheckKeyPress(&keyPress);

        if (this->skipped && fadeMode < 0xFF)
            fadeMode += 8;

        if (inputDevice[INPUT_ANY].press || touches > 0) {
            if (!this->skipped)
                fadeMode = 0;

            this->skipped = true;
        }

        if (!THEORAPLAY_isDecoding(this->decoder) || (this->skipped && fadeMode >= 0xFF)) {
            this->StopPlayback();
            ResumeSound();
            return 1; // video finished
        }

        // Don't pause or it'll go wild
        if (this->state == VIDEO_PLAYING_OGV) {
            const Uint32 now = (SDL_GetTicks() - this->startTicks);

            if (!this->frameData)
                this->frameData = THEORAPLAY_getVideo(this->decoder);

            // Play video frames when it's time.
            if (this->frameData && (this->frameData->playms <= now)) {
                if (this->frameMS && ((now - this->frameData->playms) >= this->frameMS)) {

                    // Skip frames to catch up, but keep track of the last one+
                    //  in case we catch up to a series of dupe frames, which
                    //  means we'd have to draw that final frame and then wait for
                    //  more.

                    const THEORAPLAY_VideoFrame *last = this->frameData;
                    while ((this->frameData = THEORAPLAY_getVideo(this->decoder)) != nullptr) {
                        THEORAPLAY_freeVideo(last);
                        last = this->frameData;
                        if ((now - this->frameData->playms) < this->frameMS)
                            break;
                    }

                    if (!this->frameData)
                        this->frameData = last;
                }

                // do nothing; we're far behind and out of options.
                if (!this->frameData) {
                    // video lagging uh oh
                }

#if RETRO_USING_OPENGL
                glBindTexture(GL_TEXTURE_2D, videoBuffer);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->frameData->width, this->frameData->height, GL_RGBA, GL_UNSIGNED_BYTE,
                                this->frameData->pixels);
                glBindTexture(GL_TEXTURE_2D, 0);
#elif RETRO_USING_SDL2
                int half_w     = this->frameData->width / 2;
                const Uint8 *y = (const Uint8 *)this->frameData->pixels;
                const Uint8 *u = y + (this->frameData->width * this->frameData->height);
                const Uint8 *v = u + (half_w * (this->frameData->height / 2));

                SDL_UpdateYUVTexture(Engine.videoBuffer, nullptr, y, this->frameData->width, u, half_w, v, half_w);
#elif RETRO_USING_SDL1
                memcpy(Engine.videoBuffer->pixels, this->frameData->pixels, this->frameData->width * this->frameData->height * sizeof(uint));
#endif

                THEORAPLAY_freeVideo(this->frameData);
                this->frameData = nullptr;
            }
            else if (!this->frameData) {
                StopPlayback();
                ResumeSound();
                return 1;
            }

            return 2; // its playing as expected
        }
    }

    return 0; // its not even initialised
}

void Video::StopPlayback()
{
    if (this->state == VIDEO_PLAYING_OGV) {
        // this->state and this->decoder are read by
        // the audio thread, so lock it to prevent a race
        // condition that results in invalid memory accesses.
        SDL_LockAudio();

        if (this->skipped && fadeMode >= 0xFF)
            fadeMode = 0;

        if (this->frameData) {
            THEORAPLAY_freeVideo(this->frameData);
            this->frameData = nullptr;
        }

        if (this->decoder) {
            THEORAPLAY_stopDecode(this->decoder);
            this->decoder = nullptr;
        }

        this->CloseBuffer();
        this->state = VIDEO_NOT_PLAYING;

        SDL_UnlockAudio();
    }
}

void Video::SetupBuffer(int width, int height)
{
#if RETRO_USING_OPENGL
    if (videoBuffer > 0) {
        glDeleteTextures(1, &videoBuffer);
        videoBuffer = 0;
    }

    glGenTextures(1, &videoBuffer);
    glBindTexture(GL_TEXTURE_2D, videoBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->frameData->width, this->frameData->height, GL_RGBA, GL_UNSIGNED_BYTE, this->frameData->pixels);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
#elif RETRO_USING_SDL1
    Engine.videoBuffer = SDL_CreateRGBSurface(0, width, height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

    if (!Engine.videoBuffer)
        PrintLog("Failed to create video buffer!");

    if (Engine.videoBuffer)
        SDL_FillRect(Engine.videoBuffer, nullptr, SDL_MapRGBA(Engine.videoBuffer->format, 0, 0, 0, 255));
#elif RETRO_USING_SDL2
    Engine.videoBuffer = SDL_CreateTexture(Engine.renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, width, height);

    if (!Engine.videoBuffer)
        PrintLog("Failed to create video buffer!");

    if (Engine.videoBuffer) {
        int hw = width / 2;
        int hh = height / 2;

        auto y  = std::vector<byte>(width * height, 0x00);
        auto uv = std::vector<byte>(hw * hh, 0x80);

        SDL_UpdateYUVTexture(Engine.videoBuffer, nullptr, y.data(), width, uv.data(), hw, uv.data(), hw);
    }
#endif
}

void Video::CloseBuffer()
{
    if (this->state == VIDEO_PLAYING_OGV) {
#if RETRO_USING_OPENGL
        if (videoBuffer > 0) {
            glDeleteTextures(1, &videoBuffer);
            videoBuffer = 0;
        }
#elif RETRO_USING_SDL1
        SDL_FreeSurface(Engine.videoBuffer);
        Engine.videoBuffer = nullptr;
#elif RETRO_USING_SDL2
        SDL_DestroyTexture(Engine.videoBuffer);
        Engine.videoBuffer = nullptr;
#endif
    }
}