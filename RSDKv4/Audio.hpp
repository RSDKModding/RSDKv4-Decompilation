#ifndef AUDIO_H
#define AUDIO_H

#define TRACK_COUNT (0x10)
#define SFX_COUNT   (0x100)
#if !RETRO_USE_ORIGINAL_CODE
#define CHANNEL_COUNT (0x10) // 4 in the original, 16 for convenience
#else
#define CHANNEL_COUNT (0x4)
#endif

#define MAX_VOLUME (100)

struct TrackInfo {
    char fileName[0x40];
    bool trackLoop;
    uint loopPoint;
};

#if !RETRO_USE_ORIGINAL_CODE
struct MusicPlaybackInfo {
    OggVorbis_File vorbisFile;
    int vorbBitstream;
#if RETRO_USING_SDL1
    SDL_AudioSpec spec;
#endif
#if RETRO_USING_SDL2
    SDL_AudioStream *stream;
#endif
    Sint16 *buffer;
    FileInfo fileInfo;
    bool trackLoop;
    uint loopPoint;
    bool loaded;
};
#endif

struct SFXInfo {
    char name[0x40];
    Sint16 *buffer;
    size_t length;
    bool loaded;
};

struct ChannelInfo {
    size_t sampleLength;
    Sint16 *samplePtr;
    int sfxID;
    byte loopSFX;
    sbyte pan;
};

enum MusicStatuses {
    MUSIC_STOPPED = 0,
    MUSIC_PLAYING = 1,
    MUSIC_PAUSED  = 2,
    MUSIC_LOADING = 3,
    MUSIC_READY   = 4,
};

extern int globalSFXCount;
extern int stageSFXCount;

extern int masterVolume;
extern int trackID;
extern int sfxVolume;
extern int bgmVolume;
extern bool audioEnabled;

extern bool musicEnabled;
extern int musicStatus;
extern int musicStartPos;
extern int musicPosition;
extern int musicRatio;
extern TrackInfo musicTracks[TRACK_COUNT];

extern SFXInfo sfxList[SFX_COUNT];
extern char sfxNames[SFX_COUNT][0x40];

extern ChannelInfo sfxChannels[CHANNEL_COUNT];

#if !RETRO_USE_ORIGINAL_CODE
extern MusicPlaybackInfo musInfo;
#endif

#if RETRO_USING_SDL1 || RETRO_USING_SDL2
extern SDL_AudioSpec audioDeviceFormat;
#endif

int InitAudioPlayback();
void LoadGlobalSfx();

#if RETRO_USING_SDL1 || RETRO_USING_SDL2
#if !RETRO_USE_ORIGINAL_CODE
// These functions did exist, but with different signatures
void ProcessMusicStream(Sint32 *stream, size_t bytes_wanted);
void ProcessAudioPlayback(void *data, Uint8 *stream, int len);
void ProcessAudioMixing(Sint32 *dst, const Sint16 *src, int len, int volume, sbyte pan);
#endif

#if !RETRO_USE_ORIGINAL_CODE
inline void freeMusInfo()
{
    if (musInfo.loaded) {
        SDL_LockAudio();

        if (musInfo.buffer)
            delete[] musInfo.buffer;
#if RETRO_USING_SDL2
        if (musInfo.stream)
            SDL_FreeAudioStream(musInfo.stream);
#endif
        ov_clear(&musInfo.vorbisFile);
        musInfo.buffer = nullptr;
#if RETRO_USING_SDL2
        musInfo.stream = nullptr;
#endif
        musInfo.trackLoop = false;
        musInfo.loopPoint = 0;
        musInfo.loaded    = false;

        SDL_UnlockAudio();
    }
}
#endif
#else
void ProcessMusicStream() {}
void ProcessAudioPlayback() {}
void ProcessAudioMixing() {}

#if !RETRO_USE_ORIGINAL_CODE
inline void freeMusInfo()
{
    if (musInfo.loaded) {
        if (musInfo.musicFile)
            delete[] musInfo.musicFile;
        musInfo.musicFile    = nullptr;
        musInfo.buffer       = nullptr;
        musInfo.stream       = nullptr;
        musInfo.pos          = 0;
        musInfo.len          = 0;
        musInfo.currentTrack = nullptr;
        musInfo.loaded       = false;
    }
}
#endif
#endif

void LoadMusic(void *userdata);
void SetMusicTrack(const char *filePath, byte trackID, bool loop, uint loopPoint);
void SwapMusicTrack(const char *filePath, byte trackID, uint loopPoint, uint ratio);
bool PlayMusic(int track, int musStartPos);
inline void StopMusic(bool setStatus)
{
    if (setStatus)
        musicStatus = MUSIC_STOPPED;
#if !RETRO_USE_ORIGINAL_CODE
    SDL_LockAudio();
    freeMusInfo();
    SDL_UnlockAudio();
#endif
}

void LoadSfx(char *filePath, byte sfxID);
void PlaySfx(int sfx, bool loop);
inline void StopSfx(int sfx)
{
    for (int i = 0; i < CHANNEL_COUNT; ++i) {
        if (sfxChannels[i].sfxID == sfx) {
            MEM_ZERO(sfxChannels[i]);
            sfxChannels[i].sfxID = -1;
        }
    }
}
void SetSfxAttributes(int sfx, int loopCount, sbyte pan);

void SetSfxName(const char *sfxName, int sfxID);

#if !RETRO_USE_ORIGINAL_CODE
// Helper Funcs
inline bool PlaySfxByName(const char *sfx, sbyte loopCnt)
{
    char buffer[0x40];
    int pos = 0;
    while (*sfx) {
        if (*sfx != ' ')
            buffer[pos++] = *sfx;
        sfx++;
    }
    buffer[pos] = 0;

    for (int s = 0; s < globalSFXCount + stageSFXCount; ++s) {
        if (StrComp(sfxNames[s], buffer)) {
            PlaySfx(s, loopCnt);
            return true;
        }
    }
    return false;
}
inline bool StopSFXByName(const char *sfx)
{
    char buffer[0x40];
    int pos = 0;
    while (*sfx) {
        if (*sfx != ' ')
            buffer[pos++] = *sfx;
        sfx++;
    }
    buffer[pos] = 0;

    for (int s = 0; s < globalSFXCount + stageSFXCount; ++s) {
        if (StrComp(sfxNames[s], buffer)) {
            StopSfx(s);
            return true;
        }
    }
    return false;
}
#endif

inline void SetMusicVolume(int volume)
{
    if (volume < 0)
        volume = 0;
    if (volume > MAX_VOLUME)
        volume = MAX_VOLUME;
    masterVolume = volume;
}

inline void SetGameVolumes(int bgmVol, int sfxVol)
{
    bgmVolume = bgmVol;
    sfxVolume = sfxVol;

    if (bgmVolume < 0)
        bgmVolume = 0;
    if (bgmVolume > MAX_VOLUME)
        bgmVolume = MAX_VOLUME;

    if (sfxVolume < 0)
        sfxVolume = 0;
    if (sfxVolume > MAX_VOLUME)
        sfxVolume = MAX_VOLUME;
}

inline void PauseSound()
{
    if (musicStatus == MUSIC_PLAYING)
        musicStatus = MUSIC_PAUSED;
}

inline void ResumeSound()
{
    if (musicStatus == MUSIC_PAUSED)
        musicStatus = MUSIC_PLAYING;
}

inline void StopAllSfx()
{
#if !RETRO_USE_ORIGINAL_CODE
#if RETRO_USING_SDL1 || RETRO_USING_SDL2
    SDL_LockAudio();
#endif
#endif
    for (int i = 0; i < CHANNEL_COUNT; ++i) sfxChannels[i].sfxID = -1;
#if !RETRO_USE_ORIGINAL_CODE
#if RETRO_USING_SDL1 || RETRO_USING_SDL2
    SDL_UnlockAudio();
#endif
#endif
}
inline void ReleaseGlobalSfx()
{
    for (int i = globalSFXCount - 1; i >= 0; --i) {
        if (sfxList[i].loaded) {
            StrCopy(sfxList[i].name, "");
            StrCopy(sfxNames[i], "");
            if (sfxList[i].buffer)
                free(sfxList[i].buffer);
            sfxList[i].buffer = NULL;
            sfxList[i].length = 0;
            sfxList[i].loaded = false;
        }
    }
    globalSFXCount = 0;
}
inline void ReleaseStageSfx()
{
    for (int i = (stageSFXCount + globalSFXCount) - 1; i >= globalSFXCount; --i) {
        if (sfxList[i].loaded) {
            StrCopy(sfxList[i].name, "");
            StrCopy(sfxNames[i], "");
            if (sfxList[i].buffer)
                free(sfxList[i].buffer);
            sfxList[i].buffer = NULL;
            sfxList[i].length = 0;
            sfxList[i].loaded = false;
        }
    }
    stageSFXCount = 0;
}

inline void ReleaseAudioDevice()
{
    StopMusic(true);
    StopAllSfx();
    ReleaseStageSfx();
    ReleaseGlobalSfx();
}

#endif // !AUDIO_H
