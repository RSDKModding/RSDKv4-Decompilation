#include "RetroEngine.hpp"
#include <cmath>

int globalSFXCount = 0;
int stageSFXCount  = 0;

int masterVolume  = MAX_VOLUME;
int trackID       = -1;
int sfxVolume     = MAX_VOLUME;
int bgmVolume     = MAX_VOLUME;
bool audioEnabled = false;

bool musicEnabled = 0;
int musicStatus   = MUSIC_STOPPED;
int musicStartPos = 0;
int musicPosition = 0;
int musicRatio    = 0;
TrackInfo musicTracks[TRACK_COUNT];
SFXInfo sfxList[SFX_COUNT];
char sfxNames[SFX_COUNT][0x40];

ChannelInfo sfxChannels[CHANNEL_COUNT];

#if !RETRO_USE_ORIGINAL_CODE
MusicPlaybackInfo musInfo;
#endif

int trackBuffer = -1;

#define ADJUST_VOLUME(s, v) (s = (s * v) / MAX_VOLUME)

#if RETRO_USING_SDL1 || RETRO_USING_SDL2

#if RETRO_USING_SDL2
SDL_AudioDeviceID audioDevice;
#endif
SDL_AudioSpec audioDeviceFormat;

#define LockAudioDevice()   SDL_LockAudio()
#define UnlockAudioDevice() SDL_UnlockAudio()

#define AUDIO_FREQUENCY (44100)
#define AUDIO_FORMAT    (AUDIO_S16SYS) /**< Signed 16-bit samples */
#define AUDIO_SAMPLES   (0x800)
#define AUDIO_CHANNELS  (2)

#else
#define LockAudioDevice()   ;
#define UnlockAudioDevice() ;
#endif

#define MIX_BUFFER_SAMPLES (256)

int InitAudioPlayback()
{
    StopAllSfx(); //"init"
#if !RETRO_USE_ORIGINAL_CODE
#if RETRO_USING_SDL1 || RETRO_USING_SDL2
    SDL_AudioSpec want;
    want.freq     = AUDIO_FREQUENCY;
    want.format   = AUDIO_FORMAT;
    want.samples  = AUDIO_SAMPLES;
    want.channels = AUDIO_CHANNELS;
    want.callback = ProcessAudioPlayback;

#if RETRO_USING_SDL2
    if ((audioDevice = SDL_OpenAudioDevice(nullptr, 0, &want, &audioDeviceFormat, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE)) > 0) {
        audioEnabled = true;
        SDL_PauseAudioDevice(audioDevice, 0);
    }
    else {
        printLog("Unable to open audio device: %s", SDL_GetError());
        audioEnabled = false;
        return true; // no audio but game wont crash now
    }
#elif RETRO_USING_SDL1
    if (SDL_OpenAudio(&want, &audioDeviceFormat) == 0) {
        audioEnabled = true;
        SDL_PauseAudio(0);
    }
    else {
        printLog("Unable to open audio device: %s", SDL_GetError());
        audioEnabled = false;
        return true; // no audio but game wont crash now
    }
#endif // !RETRO_USING_SDL1
#endif

#if RETRO_PLATFORM == RETRO_3DS
    if (aud_Init(ProcessAudioPlayback) == 0) {
        audioEnabled = true;
    }
    else {
        audioEnabled = false;
    }
#endif
#endif

    LoadGlobalSfx();

    return true;
}

void LoadGlobalSfx()
{
    FileInfo info;
    FileInfo infoStore;
    char strBuffer[0x100];
    byte fileBuffer = 0;
    int fileBuffer2 = 0;

    if (LoadFile("Data/Game/GameConfig.bin", &info)) {
        infoStore = info;

        FileRead(&fileBuffer, 1);
        FileRead(strBuffer, fileBuffer);

        FileRead(&fileBuffer, 1);
        FileRead(strBuffer, fileBuffer);

        byte buf[3];
        for (int c = 0; c < 0x60; ++c) FileRead(buf, 3);

        // Read Obect Names
        byte objectCount = 0;
        FileRead(&objectCount, 1);
        for (byte o = 0; o < objectCount; ++o) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
        }

        // Read Script Paths
        for (byte s = 0; s < objectCount; ++s) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
        }

        byte varCount = 0;
        FileRead(&varCount, 1);
        globalVariablesCount = varCount;
        for (byte v = 0; v < varCount; ++v) {
            // Read Variable Name
            FileRead(&fileBuffer, 1);
            FileRead(&globalVariableNames[v], fileBuffer);
            globalVariableNames[v][fileBuffer] = 0;

            // Read Variable Value
            FileRead(&fileBuffer2, 4);
        }

        // Read SFX
        globalSFXCount = 0;
        FileRead(&fileBuffer, 1);
        globalSFXCount = fileBuffer;
        for (byte s = 0; s < globalSFXCount; ++s) { // SFX Names
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
            strBuffer[fileBuffer] = 0;

            SetSfxName(strBuffer, s);
        }
        for (byte s = 0; s < globalSFXCount; ++s) { // SFX Paths
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
            strBuffer[fileBuffer] = 0;

            GetFileInfo(&infoStore);
            LoadSfx(strBuffer, s);
            SetFileInfo(&infoStore);
        }

        CloseFile();
    }

    for (int i = 0; i < CHANNEL_COUNT; ++i) sfxChannels[i].sfxID = -1;
}

size_t readVorbis(void *mem, size_t size, size_t nmemb, void *ptr)
{
#if !RETRO_USE_ORIGINAL_CODE
    MusicPlaybackInfo *info = (MusicPlaybackInfo *)ptr;
    return FileRead2(&info->fileInfo, mem, (int)(size * nmemb));
#endif
    return 0;
}
int seekVorbis(void *ptr, ogg_int64_t offset, int whence)
{
#if !RETRO_USE_ORIGINAL_CODE
    MusicPlaybackInfo *info = (MusicPlaybackInfo *)ptr;
    switch (whence) {
        case SEEK_SET: whence = 0; break;
        case SEEK_CUR: whence = (int)GetFilePosition2(&info->fileInfo); break;
        case SEEK_END: whence = info->fileInfo.vfileSize; break;
        default: break;
    }
    SetFilePosition2(&info->fileInfo, (int)(whence + offset));
    return (int)GetFilePosition2(&info->fileInfo) <= info->fileInfo.vfileSize;
#endif
    return 0;
}
long tellVorbis(void *ptr)
{
#if !RETRO_USE_ORIGINAL_CODE
    MusicPlaybackInfo *info = (MusicPlaybackInfo *)ptr;
    return GetFilePosition2(&info->fileInfo);
#endif
    return 0;
}
int closeVorbis(void *ptr)
{
#if !RETRO_USE_ORIGINAL_CODE
    return CloseFile2((FileInfo *)ptr);
#endif
    return 1;
}

size_t readVorbis_Sfx(void *mem, size_t size, size_t nmemb, void *ptr)
{
#if !RETRO_USE_ORIGINAL_CODE
    FileInfo *info = (FileInfo *)ptr;
    return FileRead2(info, mem, (int)(size * nmemb));
#endif
    return 0;
}
int seekVorbis_Sfx(void *ptr, ogg_int64_t offset, int whence)
{
#if !RETRO_USE_ORIGINAL_CODE
    FileInfo *info = (FileInfo *)ptr;
    switch (whence) {
        case SEEK_SET: whence = 0; break;
        case SEEK_CUR: whence = (int)GetFilePosition2(info); break;
        case SEEK_END: whence = info->vfileSize; break;
        default: break;
    }
    SetFilePosition2(info, (int)(whence + offset));
    return (int)GetFilePosition2(info) <= info->vfileSize;
#endif
    return 0;
}
long tellVorbis_Sfx(void *ptr)
{
#if !RETRO_USE_ORIGINAL_CODE
    FileInfo *info = (FileInfo *)ptr;
    return GetFilePosition2(info);
#endif
    return 0;
}
int closeVorbis_Sfx(void *ptr)
{
#if !RETRO_USE_ORIGINAL_CODE
    return CloseFile2((FileInfo *)ptr);
#endif
    return 0;
}

#if !RETRO_USE_ORIGINAL_CODE
void ProcessMusicStream(Sint32 *stream, size_t bytes_wanted)
{
    if (!musInfo.loaded)
        return;
    switch (musicStatus) {
        case MUSIC_READY:
        case MUSIC_PLAYING: {
#if RETRO_USING_SDL2
            while (SDL_AudioStreamAvailable(musInfo.stream) < bytes_wanted) {
                // We need more samples: get some
                long bytes_read = ov_read(&musInfo.vorbisFile, (char *)musInfo.buffer, sizeof(musInfo.buffer), 0, 2, 1, &musInfo.vorbBitstream);

                if (bytes_read == 0) {
                    // We've reached the end of the file
                    if (musInfo.trackLoop) {
                        ov_pcm_seek(&musInfo.vorbisFile, musInfo.loopPoint);
                        continue;
                    }
                    else {
                        musicStatus = MUSIC_STOPPED;
                        break;
                    }
                }

                if (SDL_AudioStreamPut(musInfo.stream, musInfo.buffer, (int)bytes_read) == -1)
                    return;
            }

            // Now that we know there are enough samples, read them and mix them
            int bytes_done = SDL_AudioStreamGet(musInfo.stream, musInfo.buffer, (int)bytes_wanted);
            if (bytes_done == -1) {
                return;
            }
            if (bytes_done != 0)
                ProcessAudioMixing(stream, musInfo.buffer, bytes_done / sizeof(Sint16), (bgmVolume * masterVolume) / MAX_VOLUME, 0);
#endif

#if RETRO_USING_SDL1
            size_t bytes_gotten = 0;
            byte *buffer        = (byte *)malloc(bytes_wanted);
            memset(buffer, 0, bytes_wanted);
            while (bytes_gotten < bytes_wanted) {
                // We need more samples: get some
                long bytes_read =
                    ov_read(&musInfo.vorbisFile, (char *)musInfo.buffer,
                            sizeof(musInfo.buffer) > (bytes_wanted - bytes_gotten) ? (bytes_wanted - bytes_gotten) : sizeof(musInfo.buffer), 0, 2, 1,
                            &musInfo.vorbBitstream);

                if (bytes_read == 0) {
                    // We've reached the end of the file
                    if (musInfo.trackLoop) {
                        ov_pcm_seek(&musInfo.vorbisFile, musInfo.loopPoint);
                        continue;
                    }
                    else {
                        musicStatus = MUSIC_STOPPED;
                        break;
                    }
                }

                if (bytes_read > 0) {
                    memcpy(buffer + bytes_gotten, musInfo.buffer, bytes_read);
                    bytes_gotten += bytes_read;
                }
                else {
                    printLog("Music read error: vorbis error: %d", bytes_read);
                }
            }

            if (bytes_gotten > 0) {
                SDL_AudioCVT convert;
                MEM_ZERO(convert);
                int cvtResult = SDL_BuildAudioCVT(&convert, musInfo.spec.format, musInfo.spec.channels, musInfo.spec.freq, audioDeviceFormat.format,
                                                  audioDeviceFormat.channels, audioDeviceFormat.freq);
                if (cvtResult == 0) {
                    if (convert.len_mult > 0) {
                        convert.buf = (byte *)malloc(bytes_gotten * convert.len_mult);
                        convert.len = bytes_gotten;
                        memcpy(convert.buf, buffer, bytes_gotten);
                        SDL_ConvertAudio(&convert);
                    }
                }

                // Now that we know there are enough samples, read them and mix them
                // int bytes_done = SDL_AudioStreamGet(musInfo.stream, musInfo.buffer, bytes_wanted);
                // if (bytes_done == -1) {
                //    return;
                //}

                if (cvtResult == 0)
                    ProcessAudioMixing(stream, (const Sint16 *)convert.buf, bytes_gotten / sizeof(Sint16), (bgmVolume * masterVolume) / MAX_VOLUME,
                                       0);

                if (convert.len > 0 && convert.buf)
                    free(convert.buf);
            }
            if (bytes_wanted > 0)
                free(buffer);

#elif RETRO_PLATFORM == RETRO_3DS
            size_t bytes_gotten = 0;
            byte *buffer        = (byte *)malloc(bytes_wanted);
            if (!buffer) {
                printf("ProcessMusicStream: malloc failed\n");
                return;
            }
            memset(buffer, 0, bytes_wanted);
            while (bytes_gotten < bytes_wanted) {
                // We need more samples: get some
                long bytes_read =
                    ov_read(&musInfo.vorbisFile, (char *)musInfo.buffer,
                            sizeof(musInfo.buffer) > (bytes_wanted - bytes_gotten) ? (bytes_wanted - bytes_gotten) : sizeof(musInfo.buffer),// 0, 2, 1,
                            &musInfo.vorbBitstream);

                if (bytes_read == 0) {
                    // We've reached the end of the file
                    if (musInfo.trackLoop) {
                        ov_pcm_seek(&musInfo.vorbisFile, musInfo.loopPoint);
                        continue;
                    }
                    else {
                        musicStatus = MUSIC_STOPPED;
                        break;
                    }
                }

                if (bytes_read > 0) {
                    memcpy(buffer + bytes_gotten, musInfo.buffer, bytes_read);
                    bytes_gotten += bytes_read;
                }
                else {
                    printLog("Music read error: vorbis error: %d", bytes_read);
                }
            }

            if (bytes_gotten > 0) {
                /*SDL_AudioCVT convert;
                MEM_ZERO(convert);
                int cvtResult = SDL_BuildAudioCVT(&convert, musInfo.spec.format, musInfo.spec.channels, musInfo.spec.freq, audioDeviceFormat.format,
                                                  audioDeviceFormat.channels, audioDeviceFormat.freq);
                if (cvtResult == 0) {
                    if (convert.len_mult > 0) {
                        convert.buf = (byte *)malloc(bytes_gotten * convert.len_mult);
                        convert.len = bytes_gotten;
                        memcpy(convert.buf, buffer, bytes_gotten);
                        SDL_ConvertAudio(&convert);
                    }
                }

                // Now that we know there are enough samples, read them and mix them
                // int bytes_done = SDL_AudioStreamGet(musInfo.stream, musInfo.buffer, bytes_wanted);
                // if (bytes_done == -1) {
                //    return;
                //}

                if (cvtResult == 0)*/
                    ProcessAudioMixing(stream, (const Sint16 *)buffer, bytes_gotten / sizeof(Sint16), (bgmVolume * masterVolume) / MAX_VOLUME,
                                       0);

                //if (convert.len > 0 && convert.buf)
                //    free(convert.buf);
            }
            if (bytes_wanted > 0)
                free(buffer);
#endif
            break;
        }
        case MUSIC_STOPPED:
        case MUSIC_PAUSED:
        case MUSIC_LOADING:
            // dont play
            break;
    }
}

void ProcessAudioPlayback(void *userdata, Uint8 *stream, int len)
{
    (void)userdata; // Unused

    if (!audioEnabled)
        return;

    Sint16 *output_buffer = (Sint16 *)stream;

    size_t samples_remaining = (size_t)len / sizeof(Sint16);
    while (samples_remaining != 0) {
        Sint32 mix_buffer[MIX_BUFFER_SAMPLES];
        memset(mix_buffer, 0, sizeof(mix_buffer));

        const size_t samples_to_do = (samples_remaining < MIX_BUFFER_SAMPLES) ? samples_remaining : MIX_BUFFER_SAMPLES;

        // Mix music
        ProcessMusicStream(mix_buffer, samples_to_do * sizeof(Sint16));

        // Mix SFX
        for (byte i = 0; i < CHANNEL_COUNT; ++i) {
            ChannelInfo *sfx = &sfxChannels[i];
            if (sfx == NULL)
                continue;

            if (sfx->sfxID < 0)
                continue;

            if (sfx->samplePtr) {
                Sint16 buffer[MIX_BUFFER_SAMPLES];

                size_t samples_done = 0;
                while (samples_done != samples_to_do) {
                    size_t sampleLen = (sfx->sampleLength * 2 < samples_to_do - samples_done) ? sfx->sampleLength * 2 : samples_to_do - samples_done;
                    //memcpy(&buffer[samples_done], sfx->samplePtr, sampleLen * sizeof(Sint16));
                    Uint16* in  = (Uint16*)sfx->samplePtr;
                    Uint32* out = (Uint32*)&buffer[samples_done];
                    for (size_t i = 0; i < sampleLen / 2; i++) {
                        *out++ = (*in << 16) | *in++;
                    }

                    samples_done += sampleLen;
                    sfx->samplePtr += sampleLen / 2;
                    sfx->sampleLength -= sampleLen / 2;

                    if (sfx->sampleLength == 0) {
                        if (sfx->loopSFX) {
                            sfx->samplePtr    = sfxList[sfx->sfxID].buffer;
                            sfx->sampleLength = sfxList[sfx->sfxID].length;
                        }
                        else {
                            StopSfx(sfx->sfxID);
                            break;
                        }
                    }
                }

//#if RETRO_USING_SDL1 || RETRO_USING_SDL2
                ProcessAudioMixing(mix_buffer, buffer, (int)samples_done, sfxVolume, sfx->pan);
//#endif
            }
        }

        // Clamp mixed samples back to 16-bit and write them to the output buffer
        for (size_t i = 0; i < sizeof(mix_buffer) / sizeof(*mix_buffer); ++i) {
            const Sint16 max_audioval = ((1 << (16 - 1)) - 1);
            const Sint16 min_audioval = -(1 << (16 - 1));

            const Sint32 sample = mix_buffer[i];

            if (sample > max_audioval)
                *output_buffer++ = max_audioval;
            else if (sample < min_audioval)
                *output_buffer++ = min_audioval;
            else
                *output_buffer++ = sample;
        }

        samples_remaining -= samples_to_do;
    }
}

//#if RETRO_USING_SDL1 || RETRO_USING_SDL2
void ProcessAudioMixing(Sint32 *dst, const Sint16 *src, int len, int volume, sbyte pan)
{
    if (volume == 0)
        return;

    if (volume > MAX_VOLUME)
        volume = MAX_VOLUME;

    float panL = 0.0;
    float panR = 0.0;
    int i      = 0;

    if (pan < 0) {
        panR = 1.0f - abs(pan / 100.0f);
        panL = 1.0f;
    }
    else if (pan > 0) {
        panL = 1.0f - abs(pan / 100.0f);
        panR = 1.0f;
    }

    while (len--) {
        Sint32 sample = *src++;
        ADJUST_VOLUME(sample, volume);

        if (pan != 0) {
            if ((i % 2) != 0) {
                sample *= panR;
            }
            else {
                sample *= panL;
            }
        }

        *dst++ += sample;

        i++;
    }
}
#endif
//#endif

void LoadMusic(void *userdata)
{
    if (trackBuffer < 0 || trackBuffer >= TRACK_COUNT) {
        LockAudioDevice();
        StopMusic(true);
        UnlockAudioDevice();
        return;
    }

    TrackInfo *trackPtr = &musicTracks[trackBuffer];

    if (!trackPtr->fileName[0]) {
        LockAudioDevice();
        StopMusic(true);
        UnlockAudioDevice();
        return;
    }

    LockAudioDevice();
    uint oldPos   = 0;
    //uint oldTotal = 0;
    if (musInfo.loaded) {
        oldPos   = (uint)ov_pcm_tell(&musInfo.vorbisFile);
        //oldTotal = (uint)ov_pcm_total(&musInfo.vorbisFile, -1);
        StopMusic(false);
    }

    if (LoadFile2(trackPtr->fileName, &musInfo.fileInfo)) {
        musInfo.trackLoop = trackPtr->trackLoop;
        musInfo.loopPoint = trackPtr->loopPoint;
        musInfo.loaded    = true;

        unsigned long long samples = 0;
        ov_callbacks callbacks;

        callbacks.read_func  = readVorbis;
        callbacks.seek_func  = seekVorbis;
        callbacks.tell_func  = tellVorbis;
        callbacks.close_func = closeVorbis;

        int error = ov_open_callbacks(&musInfo, &musInfo.vorbisFile, NULL, 0, callbacks);
        if (error == 0) {
            musInfo.vorbBitstream = -1;
            musInfo.vorbisFile.vi = ov_info(&musInfo.vorbisFile, -1);

            samples = (unsigned long long)ov_pcm_total(&musInfo.vorbisFile, -1);

#if RETRO_USING_SDL2
            musInfo.stream = SDL_NewAudioStream(AUDIO_S16, musInfo.vorbisFile.vi->channels, (int)musInfo.vorbisFile.vi->rate,
                                                audioDeviceFormat.format, audioDeviceFormat.channels, audioDeviceFormat.freq);
            if (!musInfo.stream)
                printLog("Failed to create stream: %s", SDL_GetError());
#endif

#if RETRO_USING_SDL1
            musInfo.spec.format   = AUDIO_S16;
            musInfo.spec.channels = musInfo.vorbisFile.vi->channels;
            musInfo.spec.freq     = (int)musInfo.vorbisFile.vi->rate;
#endif

            musInfo.buffer = new Sint16[MIX_BUFFER_SAMPLES];

            if (musicStartPos) {
                float newPos  = oldPos * ((float)musicRatio * 0.0001); // 8000 == 0.8 (ratio / 10,000)
                musicStartPos = fmod(newPos, samples);

                ov_pcm_seek(&musInfo.vorbisFile, musicStartPos);
            }
            musicStartPos = 0;

            musicStatus  = MUSIC_PLAYING;
            masterVolume = MAX_VOLUME;
            trackID      = trackBuffer;
            trackBuffer  = -1;
        }
        else {
            musicStatus = MUSIC_STOPPED;
            CloseFile2(&musInfo.fileInfo);
            printLog("Failed to load vorbis! error: %d", error);
            switch (error) {
                default: printLog("Vorbis open error: Unknown (%d)", error); break;
                case OV_EREAD: printLog("Vorbis open error: A read from media returned an error"); break;
                case OV_ENOTVORBIS: printLog("Vorbis open error: Bitstream does not contain any Vorbis data"); break;
                case OV_EVERSION: printLog("Vorbis open error: Vorbis version mismatch"); break;
                case OV_EBADHEADER: printLog("Vorbis open error: Invalid Vorbis bitstream header"); break;
                case OV_EFAULT: printLog("Vorbis open error: Internal logic fault; indicates a bug or heap / stack corruption"); break;
            }
        }
    }
    UnlockAudioDevice();
}

void SetMusicTrack(const char *filePath, byte trackID, bool loop, uint loopPoint)
{
    LockAudioDevice();
    TrackInfo *track = &musicTracks[trackID];
    StrCopy(track->fileName, "Data/Music/");
    StrAdd(track->fileName, filePath);
    track->trackLoop = loop;
    track->loopPoint = loopPoint;
    UnlockAudioDevice();
}

void SwapMusicTrack(const char *filePath, byte trackID, uint loopPoint, uint ratio)
{
    if (StrLength(filePath) <= 0) {
        StopMusic(true);
    }
    else {
        LockAudioDevice();
        TrackInfo *track = &musicTracks[trackID];
        StrCopy(track->fileName, "Data/Music/");
        StrAdd(track->fileName, filePath);
        track->trackLoop = true;
        track->loopPoint = loopPoint;
        musicRatio       = ratio;
        UnlockAudioDevice();
        PlayMusic(trackID, 1);
    }
}

bool PlayMusic(int track, int musStartPos)
{
    if (!audioEnabled)
        return false;

    if (musicStatus != MUSIC_LOADING) {
        LockAudioDevice();
        musicStartPos = musStartPos;
        if (track < 0 || track >= TRACK_COUNT) {
            StopMusic(true);
            trackBuffer = -1;
            return false;
        }
        trackBuffer = track;
        musicStatus = MUSIC_LOADING;
        //SDL_CreateThread((SDL_ThreadFunction)LoadMusic, "LoadMusic", NULL);
        sys_CreateThread(LoadMusic, NULL);
        UnlockAudioDevice();
        return true;
    }
    else {
        printLog("WARNING music tried to play while music was loading!");
    }
    return false;
}

void SetSfxName(const char *sfxName, int sfxID)
{
    int sfxNameID   = 0;
    int soundNameID = 0;
    while (sfxName[sfxNameID]) {
        if (sfxName[sfxNameID] != ' ')
            sfxNames[sfxID][soundNameID++] = sfxName[sfxNameID];
        ++sfxNameID;
    }
    sfxNames[sfxID][soundNameID] = 0;
    printLog("Set SFX (%d) name to: %s", sfxID, sfxName);
}

void LoadSfx(char *filePath, byte sfxID)
{
    if (!audioEnabled)
        return;

    FileInfo info;
    char fullPath[0x80];

    StrCopy(fullPath, "Data/SoundFX/");
    StrAdd(fullPath, filePath);
    printf("%s\n", fullPath);
    if (LoadFile(fullPath, &info)) {
#if !RETRO_USE_ORIGINAL_CODE
        byte type = fullPath[StrLength(fullPath) - 3];
        if (type == 'w') {
            byte *sfx = new byte[info.vfileSize];
            FileRead(sfx, info.vfileSize);
            CloseFile();

    #if RETRO_USING_SDL
            SDL_RWops *src = SDL_RWFromMem(sfx, info.vfileSize);
            if (src == NULL) {
                printLog("Unable to open sfx: %s", info.fileName);
            }
            else {
                SDL_AudioSpec wav_spec;
                uint wav_length;
                byte *wav_buffer;
                SDL_AudioSpec *wav = SDL_LoadWAV_RW(src, 0, &wav_spec, &wav_buffer, &wav_length);

                SDL_RWclose(src);
                delete[] sfx;
                if (wav == NULL) {
                    printLog("Unable to read sfx: %s", info.fileName);
                }
                else {
                    SDL_AudioCVT convert;
                    if (SDL_BuildAudioCVT(&convert, wav->format, wav->channels, wav->freq, audioDeviceFormat.format, audioDeviceFormat.channels,
                                          audioDeviceFormat.freq)
                        > 0) {
                        convert.buf = (byte *)malloc(wav_length * convert.len_mult);
                        convert.len = wav_length;
                        memcpy(convert.buf, wav_buffer, wav_length);
                        SDL_ConvertAudio(&convert);

                        LockAudioDevice();
                        StrCopy(sfxList[sfxID].name, filePath);
                        sfxList[sfxID].buffer = (Sint16 *)convert.buf;
                        sfxList[sfxID].length = convert.len_cvt / sizeof(Sint16);
                        sfxList[sfxID].loaded = true;
                        UnlockAudioDevice();
                        SDL_FreeWAV(wav_buffer);
                    }
                    else { //this causes errors, actually
                        printLog("Unable to read sfx: %s (error: %s)", info.fileName, SDL_GetError());
                        sfxList[sfxID].loaded = false;
                        SDL_FreeWAV(wav_buffer);
                        //LockAudioDevice()
                        //StrCopy(sfxList[sfxID].name, filePath);
                        //sfxList[sfxID].buffer = (Sint16 *)wav_buffer;
                        //sfxList[sfxID].length = wav_length / sizeof(Sint16);
                        //sfxList[sfxID].loaded = false;
                        //UnlockAudioDevice()
                    }
                }
            }
    #elif RETRO_PLATFORM == RETRO_3DS
            FILE *src = fmemopen(sfx, info.vfileSize, "rb");
            if (src == NULL) {
                printLog("Unable to open sfx: %s", info.fileName);
            }
            else {
                AUD_WavData* wav = aud_LoadWAV(src);
                fclose(src);
    
                if (wav == NULL) {
                    printLog("Unable to read sfx: %s", info.fileName);
                }
                else {
                    if (sfxList[sfxID].buffer)
                        sys_LinearFree(sfxList[sfxID].buffer);
                    sfxList[sfxID].buffer = (Sint16 *)wav->data;
                    //memcpy(sfxList[sfxID].buffer, wav->data, wav->size);
                    sfxList[sfxID].length = wav->size / sizeof(Sint16);
                    sfxList[sfxID].loaded = true;
                    aud_FreeWAV(wav);
                }
            }
            delete[] sfx;
    #endif
        }
        else if (type == 'o') {
            // ogg sfx :(
            OggVorbis_File vf;
            ov_callbacks callbacks;
    #if RETRO_PLATFORM != RETRO_3DS
            callbacks = OV_CALLBACKS_NOCLOSE;
    #endif
            vorbis_info *vinfo;
            byte *buf;            
            int bitstream = -1;
            long samples;
            int read, toRead;

            callbacks.read_func  = readVorbis_Sfx;
            callbacks.seek_func  = seekVorbis_Sfx;
            callbacks.tell_func  = tellVorbis_Sfx;
            callbacks.close_func = closeVorbis_Sfx;

            info.cFileHandle = cFileHandle;
            cFileHandle      = nullptr;

            // GetFileInfo(&info);
            int error = ov_open_callbacks(&info, &vf, NULL, 0, callbacks);
            if (error != 0) {
                ov_clear(&vf);
                printLog("failed to load ogg sfx!");
                return;
            }

            vinfo = ov_info(&vf, -1);

            byte *audioBuf = NULL;
            uint audioLen  = 0;
    #if RETRO_USING_SDL
            long samplesize;
            SDL_AudioSpec spec;
            memset(&spec, 0, sizeof(SDL_AudioSpec));

            spec.format   = AUDIO_S16;
            spec.channels = vinfo->channels;
            spec.freq     = (int)vinfo->rate;
            spec.samples  = 4096; /* buffer size */

            samples = (long)ov_pcm_total(&vf, -1);

            audioLen = spec.size = (Uint32)(samples * spec.channels * 2);
            audioBuf             = (byte *)malloc(audioLen);
            buf                  = audioBuf;
            toRead               = audioLen;

            for (read = (int)ov_read(&vf, (char *)buf, toRead, 0, 2, 1, &bitstream); read > 0;
                 read = (int)ov_read(&vf, (char *)buf, toRead, 0, 2, 1, &bitstream)) {
                if (read < 0) {
                    free(audioBuf);
                    ov_clear(&vf);
                    printLog("failed to read ogg sfx!");
                    return;
                }
                toRead -= read;
                buf += read;
            }

            ov_clear(&vf);

            /* Don't return a buffer that isn't a multiple of samplesize */
            samplesize = ((spec.format & 0xFF) / 8) * spec.channels;
            audioLen &= ~(samplesize - 1);

            SDL_AudioCVT convert;
            if (SDL_BuildAudioCVT(&convert, spec.format, spec.channels, spec.freq, audioDeviceFormat.format, audioDeviceFormat.channels,
                                  audioDeviceFormat.freq)
                > 0) {
                convert.buf = (byte *)malloc(audioLen * convert.len_mult);
                convert.len = audioLen;
                memcpy(convert.buf, audioBuf, audioLen);
                SDL_ConvertAudio(&convert);

                LockAudioDevice();
                StrCopy(sfxList[sfxID].name, filePath);
                sfxList[sfxID].buffer = (Sint16 *)convert.buf;
                sfxList[sfxID].length = convert.len_cvt / sizeof(Sint16);
                sfxList[sfxID].loaded = true;
                UnlockAudioDevice();
                free(audioBuf);
            }
            else {
                LockAudioDevice();
                StrCopy(sfxList[sfxID].name, filePath);
                sfxList[sfxID].buffer = (Sint16 *)audioBuf;
                sfxList[sfxID].length = audioLen / sizeof(Sint16);
                sfxList[sfxID].loaded = true;
                UnlockAudioDevice();
            }
    #elif RETRO_PLATFORM == RETRO_3DS
            //int format = sizeof(Sint16);
            int size;
            int channels = vinfo->channels;
            samples = (long)ov_pcm_total(&vf, -1);

            audioLen = size = (Uint32)(samples * channels * 2);
            audioBuf        = (byte *)sys_LinearAlloc(audioLen);
            buf             = audioBuf;
            toRead          = audioLen;

            for (read = (int)ov_read(&vf, (char *)buf, toRead, &bitstream); read > 0;
                 read = (int)ov_read(&vf, (char *)buf, toRead, &bitstream)) {
                if (read < 0) {
                    sys_LinearFree(audioBuf);
                    ov_clear(&vf);
                    printLog("failed to read ogg sfx!");
                    return;
                }
                toRead -= read;
                buf += read;
            }

            ov_clear(&vf);

            // Don't return a buffer that isn't a multiple of samplesize
            //samplesize = ((spec.format & 0xFF) / 8) * spec.channels;
            //audioLen &= ~(samplesize - 1);

            StrCopy(sfxList[sfxID].name, filePath);
            sfxList[sfxID].buffer = (Sint16 *)audioBuf;
            sfxList[sfxID].length = audioLen / sizeof(Sint16);
            sfxList[sfxID].loaded = true;
    #endif
        }
        else {
            // wtf lol
            CloseFile();
            printLog("Sfx format not supported!");
        }
#endif
    }
}
void PlaySfx(int sfx, bool loop)
{
    LockAudioDevice();
    int sfxChannelID = -1;
    for (int c = 0; c < CHANNEL_COUNT; ++c) {
        if (sfxChannels[c].sfxID == sfx || sfxChannels[c].sfxID == -1) {
            sfxChannelID = c;
            break;
        }
    }

    ChannelInfo *sfxInfo  = &sfxChannels[sfxChannelID];
    sfxInfo->sfxID        = sfx;
    sfxInfo->samplePtr    = sfxList[sfx].buffer;
    sfxInfo->sampleLength = sfxList[sfx].length;
    sfxInfo->loopSFX      = loop;
    sfxInfo->pan          = 0;
    UnlockAudioDevice();
}
void SetSfxAttributes(int sfx, int loopCount, sbyte pan)
{
    LockAudioDevice();
    int sfxChannel = -1;
    for (int i = 0; i < CHANNEL_COUNT; ++i) {
        if (sfxChannels[i].sfxID == sfx) {
            sfxChannel = i;
            break;
        }
    }
    if (sfxChannel == -1) {
        UnlockAudioDevice();
        return; // wasn't found
    }

    ChannelInfo *sfxInfo = &sfxChannels[sfxChannel];
    sfxInfo->loopSFX     = loopCount == -1 ? sfxInfo->loopSFX : loopCount;
    sfxInfo->pan         = pan;
    sfxInfo->sfxID       = sfx;
    UnlockAudioDevice();
}
