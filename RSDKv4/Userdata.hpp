#ifndef USERDATA_H
#define USERDATA_H

#define GLOBALVAR_COUNT (0x100)

#define ACHIEVEMENT_COUNT (0x40)
#define LEADERBOARD_COUNT (0x80)

#define SAVEDATA_SIZE (0x2000)

#if RETRO_USE_MOD_LOADER
#define NATIIVEFUNCTION_COUNT (0x30)
#else
#define NATIIVEFUNCTION_COUNT (0x10)
#endif

#define intToVoid(x) (void *)(size_t)(x)
#define voidToInt(x) (int)(size_t)(x)

#define unused(x) (void)x

struct SaveFile {
    int characterID;    // Value 0/8/16/24
    int lives;          // Value 1/9/17/25
    int score;          // Value 2/10/18/26
    int scoreBonus;     // Value 3/11/19/27
    int stageID;        // Value 4/12/20/28
    int emeralds;       // Value 5/13/21/29
    int specialStageID; // Value 6/14/22/30
    int unused;         // Value 7/15/23/31
};

struct SaveGame {
    SaveFile files[4];   // Values 0-31
    int saveInitialized; // Value 32
    int musVolume;       // Value 33
    int sfxVolume;       // Value 34
    int spindashEnabled; // Value 35
    int boxRegion;       // Value 36
    int vDPadSize;       // Value 37
    int vDPadOpacity;    // Value 38
    int vDPadX_Move;     // Value 39
    int vDPadY_Move;     // Value 40
    int vDPadX_Jump;     // Value 41
    int vDPadY_Jump;     // Value 42
    int tailsUnlocked;   // Value 43
    int knuxUnlocked;    // Value 44
    int unlockedActs;    // Value 45
    int unlockedHPZ;     // Value 46
    int unused[17];      // Values 47-63
    int records[0x80];   // Values 64-192
    int padding[0x73F];  // Values 193-2047
    int customSS[0x400]; // Values 2048-3072
};

enum OnlineMenuTypes {
    ONLINEMENU_ACHIEVEMENTS = 0,
    ONLINEMENU_LEADERBOARDS = 1,
};

struct Achievement {
    char name[0x40];
    char desc[0x80];
    int status;
};

struct LeaderboardEntry {
    int score;
};

#ifndef NETWORKING_H
struct MultiplayerData {
    int type;
    int data[0x1FF];
};
#endif

extern void *nativeFunction[NATIIVEFUNCTION_COUNT];
extern int nativeFunctionCount;

extern int globalVariablesCount;
extern int globalVariables[GLOBALVAR_COUNT];
extern char globalVariableNames[GLOBALVAR_COUNT][0x20];

extern char gamePath[0x100];
extern int saveRAM[SAVEDATA_SIZE];
extern Achievement achievements[ACHIEVEMENT_COUNT];
extern int achievementCount;
extern LeaderboardEntry leaderboards[LEADERBOARD_COUNT];

extern MultiplayerData multiplayerDataIN;
extern MultiplayerData multiplayerDataOUT;

extern int matchValueData[0x100];
extern byte matchValueReadPos;
extern byte matchValueWritePos;

extern int vsGameLength;
extern int vsItemMode;
extern int vsPlayerID;
extern bool vsPlaying;

extern int sendCounter;

#if !RETRO_USE_ORIGINAL_CODE
extern bool forceUseScripts;
extern bool forceUseScripts_Config;
extern bool skipStartMenu;
extern bool skipStartMenu_Config;
extern int disableFocusPause;
extern int disableFocusPause_Config;
#endif

inline int GetGlobalVariableByName(const char *name)
{
    for (int v = 0; v < globalVariablesCount; ++v) {
        if (StrComp(name, globalVariableNames[v]))
            return globalVariables[v];
    }
    return 0;
}

inline void SetGlobalVariableByName(const char *name, int value)
{
    for (int v = 0; v < globalVariablesCount; ++v) {
        if (StrComp(name, globalVariableNames[v])) {
            globalVariables[v] = value;
            break;
        }
    }
}

inline int GetGlobalVariableID(const char *name)
{
    for (int v = 0; v < globalVariablesCount; ++v) {
        if (StrComp(name, globalVariableNames[v]))
            return v;
    }
    return 0xFF;
}

#define AddNativeFunction(name, funcPtr)                                                                                                             \
    if (nativeFunctionCount < NATIIVEFUNCTION_COUNT) {                                                                                               \
        SetGlobalVariableByName(name, nativeFunctionCount);                                                                                          \
        nativeFunction[nativeFunctionCount++] = (void *)funcPtr;                                                                                     \
    }

extern bool useSGame;
bool ReadSaveRAMData();
bool WriteSaveRAMData();

#if !RETRO_USE_ORIGINAL_CODE
void InitUserdata();
void WriteSettings();
void ReadUserdata();
void WriteUserdata();

inline void AddAchievement(const char *name, const char *description)
{
    if (achievementCount < ACHIEVEMENT_COUNT) {
        StrCopy(achievements[achievementCount].name, name);
        StrCopy(achievements[achievementCount].desc, description);
        achievementCount++;
    }
}
#endif
void SetAchievement(int *achievementID, int *status);
void AwardAchievement(int id, int status);
#if RETRO_USE_MOD_LOADER
void AddGameAchievement(int *unused, const char *name);
void SetAchievementDescription(uint *id, const char *desc);
void ClearAchievements();
void GetAchievementCount();
void GetAchievementName(uint *id, int *textMenu);
void GetAchievementDescription(uint *id, int *textMenu);
void GetAchievement(uint *id, void *unused);
#endif
inline void LoadAchievementsMenu()
{
#if !RETRO_USE_ORIGINAL_CODE
    ReadUserdata();
#endif
}

void ShowAchievementsScreen();

int SetLeaderboard(int *leaderboardID, int *result);
inline void LoadLeaderboardsMenu()
{
#if !RETRO_USE_ORIGINAL_CODE
    ReadUserdata();
#endif
}
void ShowLeaderboardsScreen();

void Connect2PVS(int *gameLength, int *itemMode);
void Disconnect2PVS();
void SendEntity(int *entityID, int *verify);
void SendValue(int *value, int *verify);
void ReceiveEntity(int *entityID, int *incrementPos);
void ReceiveValue(int *value, int *incrementPos);
void TransmitGlobal(int *globalValue, const char *globalName);

void Receive2PVSData(MultiplayerData *data);
void Receive2PVSMatchCode(int code);

void ShowPromoPopup(int *id, const char *popupName);
void ShowSegaIDPopup();
void ShowOnlineSignIn();
void ShowWebsite(int websiteID);

// In the Sega Forever versions of S1 & S2, there's a feature where you can choose to watch an ad to continue from a Game Over
// We obviously can't do that here, so let's just take the L
inline void NativePlayerWaitingAds() { SetGlobalVariableByName("waitingAds.result", 2); }
inline void NativeWaterPlayerWaitingAds() { SetGlobalVariableByName("waitingAds.water", 2); }

#if RETRO_REV03
void NotifyCallback(int *callback, int *param1, int *param2, int *param3);
#endif

void ExitGame();
void FileExists(int *unused, const char *filePath);

#if RETRO_USE_MOD_LOADER
void GetScreenWidth();
void SetScreenWidth(int *width, int *unused);
void GetWindowScale();
void SetWindowScale(int *scale, int *unused);
void GetWindowScaleMode();
void SetWindowScaleMode(int *mode, int *unused);
void GetWindowFullScreen();
void SetWindowFullScreen(int *fullscreen, int *unused);
void GetWindowBorderless();
void SetWindowBorderless(int *borderless, int *unused);
void GetWindowVSync();
void SetWindowVSync(int *enabled, int *unused);
void ApplyWindowChanges();

#endif

#endif //! USERDATA_H
