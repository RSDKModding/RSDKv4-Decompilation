#ifndef AUDIO_H
#define AUDIO_H

#define TRACK_COUNT (0x10)
#define SFX_COUNT (0x100)
#define CHANNEL_COUNT (0x8) //4 in the original, 8 for convenience

#define MAX_VOLUME (100)

struct TrackInfo {
    char fileName[0x40];
    bool trackLoop;
    uint loopPoint;
};

struct MusicPlaybackInfo {
#if RETRO_USING_SDL
    OggVorbis_File vorbisFile;
    unsigned long long audioLen;
    int vorbBitstream;
    SDL_AudioSpec spec;
    SDL_AudioStream *stream;
    byte *buffer;
    byte *extraBuffer;
#endif
    FileInfo fileInfo;
    TrackInfo *currentTrack;
    bool loaded;
};

struct SFXInfo {
    char name[0x40];
    byte *buffer;
    int length;
    bool loaded;
};

struct ChannelInfo {
    int sampleStart;
    int sampleLength;
    byte *samplePtr;
    int sfxID;
    byte loopSFX;
    char pan;
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

extern int nextChannelPos;
extern bool musicEnabled;
extern int musicStatus;
extern int musicStartPos;
extern int musicRatio;
extern TrackInfo musicTracks[TRACK_COUNT];

extern SFXInfo sfxList[SFX_COUNT];
extern char sfxNames[SFX_COUNT][0x40];

extern ChannelInfo sfxChannels[CHANNEL_COUNT];

extern MusicPlaybackInfo musInfo;

#if RETRO_USING_SDL
extern SDL_AudioSpec audioDeviceFormat;
#endif

int InitAudioPlayback();

#if RETRO_USING_SDL
void ProcessMusicStream(void *data, Uint8 *stream, int le);
void ProcessAudioPlayback(void *data, Uint8 *stream, int len);
void ProcessAudioMixing(void *sfx, Uint8 *dst, const byte *src, SDL_AudioFormat format, Uint32 len, int volume, bool music);


inline void freeMusInfo()
{
    if (musInfo.loaded) {
        SDL_LockAudio();

        if (musInfo.buffer)
            delete[] musInfo.buffer;
        if (musInfo.extraBuffer)
            delete[] musInfo.extraBuffer;
        if (musInfo.stream)
            SDL_FreeAudioStream(musInfo.stream);
        ov_clear(&musInfo.vorbisFile);
        musInfo.buffer       = nullptr;
        musInfo.stream       = nullptr;
        musInfo.audioLen     = 0;
        musInfo.currentTrack = nullptr;
        musInfo.loaded       = false;

        SDL_UnlockAudio();
    }
}
#else
void ProcessMusicStream() {}
void ProcessAudioPlayback() {}
void ProcessAudioMixing() {}

inline void freeMusInfo()
{
    if (musInfo.loaded) {
        if (musInfo.musicFile)
            delete[] musInfo.musicFile;
        musInfo.musicFile    = nullptr;
        musInfo.buffer       = nullptr;
        musInfo.stream       = nullptr;
        musInfo.audioLen     = 0;
        musInfo.pos          = 0;
        musInfo.len          = 0;
        musInfo.currentTrack = nullptr;
        musInfo.loaded       = false;
    }
}
#endif

void SetMusicTrack(const char *filePath, byte trackID, bool loop, uint loopPoint);
void SwapMusicTrack(const char *filePath, byte trackID, uint loopPoint, uint ratio);
bool PlayMusic(int track, int musStartPos);
inline void StopMusic()
{
    musicStatus = MUSIC_STOPPED;
    SDL_LockAudio();
    freeMusInfo();
    SDL_UnlockAudio();
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
void SetSfxAttributes(int sfx, int loopCount, char pan);

#if !RSDK_DEBUG
inline void SetSfxName(const char* sfxName, int sfxID) {
    int sfxNameID  = 0;
    int soundNameID = 0;
    while (sfxName[sfxNameID]) {
        if (sfxName[sfxNameID] != ' ')
            sfxNames[sfxID][soundNameID++] = sfxName[sfxNameID];
        ++sfxNameID;
    }
    sfxNames[sfxID][soundNameID] = 0;
}
#else
void SetSfxName(const char *sfxName, int sfxID);
#endif

inline void SetMusicVolume(int volume)
{
    if (volume < 0)
        volume = 0;
    if (volume > MAX_VOLUME)
        volume = MAX_VOLUME;
    masterVolume = volume;
}

inline void SetGameVolumes(int bgmVolume, int sfxVolume) {
    //musicVolumeSetting = bgmVolume;
    SetMusicVolume(masterVolume);
    //sfxVolumeSetting = ((sfxVolume << 7) / 100);
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
    for (int i = 0; i < CHANNEL_COUNT; ++i) sfxChannels[i].sfxID = -1;
}
inline void ReleaseGlobalSfx()
{
    for (int i = globalSFXCount; i >= 0; --i) {
        if (sfxList[i].loaded) {
            StrCopy(sfxList[i].name, "");
            StrCopy(sfxNames[i], "");
            free(sfxList[i].buffer);
            sfxList[i].length = 0;
            sfxList[i].loaded = false;
        }
    }
    globalSFXCount = 0;
}
inline void ReleaseStageSfx()
{
    for (int i = stageSFXCount + globalSFXCount; i >= globalSFXCount; --i) {
        if (sfxList[i].loaded) {
            StrCopy(sfxList[i].name, "");
            StrCopy(sfxNames[i], "");
            free(sfxList[i].buffer);
            sfxList[i].length = 0;
            sfxList[i].loaded = false;
        }
    }
    stageSFXCount = 0;
}

inline void ReleaseAudioDevice()
{
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    ReleaseGlobalSfx();
}

#endif // !AUDIO_H
