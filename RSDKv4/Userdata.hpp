#ifndef USERDATA_H
#define USERDATA_H

#define GLOBALVAR_COUNT (0x100)

#define ACHIEVEMENT_MAX (0x40)
#define LEADERBOARD_MAX (0x80)

#define SAVEDATA_MAX (0x2000)

#define NATIIVEFUNCTION_MAX (0x10)

#if RETRO_USE_MOD_LOADER
#include <string>
#include <map>
#include <unordered_map>
#endif

#define intToVoid(x)   (void *)(size_t)(x)
#define voidToInt(x)   (int)(size_t)(x)

#define unused(x) (void)x

struct SaveFile {
    int characterID;
    int lives;
    int score;
    int scoreBonus;
    int zoneID;
    int emeralds;
    int specialZoneID;
    int unused;
};

struct SaveGame {
    SaveFile files[4];
    int saveInitialized;
    int musVolume;
    int sfxVolume;
    int spindashEnabled;
    int boxRegion;
    int vDPadSize;
    int vDPadOpacity;
    int vDPadX_Move;
    int vDPadY_Move;
    int vDPadX_Jump;
    int vDPadY_Jump;
    int tailsUnlocked;
    int knuxUnlocked;
    int totalScore;
    int unused[18];
    int records[64];
};

enum OnlineMenuTypes {
    ONLINEMENU_ACHIEVEMENTS = 0,
    ONLINEMENU_LEADERBOARDS = 1,
};

struct Achievement {
    char name[0x40];
    int status;
};

struct LeaderboardEntry {
    int score;
};

struct MultiplayerData {
    int type;
    int data[0x1FF];
};

#if RETRO_USE_MOD_LOADER
struct ModInfo {
    std::string name;
    std::string desc;
    std::string author;
    std::string version;
    std::map<std::string, std::string> fileMap;
    std::string folder;
    bool useScripts;
    bool skipStartMenu;
    bool disableFocusPause;
    bool active;
};
#endif

extern void *nativeFunction[NATIIVEFUNCTION_MAX];
extern int nativeFunctionCount;

extern int globalVariablesCount;
extern int globalVariables[GLOBALVAR_COUNT];
extern char globalVariableNames[GLOBALVAR_COUNT][0x20];

extern char gamePath[0x100];
extern int saveRAM[SAVEDATA_MAX];
extern Achievement achievements[ACHIEVEMENT_MAX];
extern LeaderboardEntry leaderboards[LEADERBOARD_MAX];

extern MultiplayerData multiplayerDataIN;
extern MultiplayerData multiplayerDataOUT;
extern int matchValueData[0x100];
extern int matchValueReadPos;
extern int matchValueWritePos;

extern int sendDataMethod;
extern int sendCounter;

#if RETRO_USE_MOD_LOADER
extern std::vector<ModInfo> modList;
#endif

#if !RETRO_USE_ORIGINAL_CODE
extern bool forceUseScripts;
extern bool skipStartMenu;
extern bool skipStartMenu_Config;
extern bool disableFocusPause;
extern bool disableFocusPause_Config;
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
    return 0;
}

#define AddNativeFunction(name, funcPtr)                                                                                                             \
    if (nativeFunctionCount < NATIIVEFUNCTION_MAX) {                                                                                                 \
        SetGlobalVariableByName(name, nativeFunctionCount);                                                                                          \
        nativeFunction[nativeFunctionCount++] = (void *)funcPtr;                                                                                     \
    }

inline bool ReadSaveRAMData()
{
    char buffer[0x100];
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/SData.bin", getResourcesPath());
    else
        sprintf(buffer, "%sSData.bin", gamePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/SData.bin", gamePath);
#else
    sprintf(buffer, "%sSData.bin", gamePath);
#endif
    FileIO *saveFile = fOpen(buffer, "rb");
    if (!saveFile)
        return false;
    fRead(saveRAM, 4u, SAVEDATA_MAX, saveFile);
    fClose(saveFile);
    return true;
}

inline bool WriteSaveRAMData()
{
    char buffer[0x100];
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/SData.bin", getResourcesPath());
    else
        sprintf(buffer, "%sSData.bin", gamePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/SData.bin", gamePath);
#else
    sprintf(buffer, "%sSData.bin", gamePath);
#endif

    FileIO *saveFile = fOpen(buffer, "wb");
    if (!saveFile)
        return false;
    fWrite(saveRAM, 4u, SAVEDATA_MAX, saveFile);
    fClose(saveFile);
    return true;
}

#if !RETRO_USE_ORIGINAL_CODE
void InitUserdata();
void writeSettings();
void ReadUserdata();
void WriteUserdata();
#endif

int SetAchievement(int *achievementID, int *status);
void AwardAchievement(int id, int status);
#if RETRO_USE_MOD_LOADER
int AddAchievement(int *id, const char *name);
int ClearAchievements();
int GetAchievement(int *id, void *a2);
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

int Connect2PVS(int *gameLength, int *itemMode);
int Disconnect2PVS(int *a1, int *a2);
int SendEntity(int *entityID, int *dataSlot);
int SendValue(int *value, int *dataSlot);
int ReceiveEntity(int *entityID, int *dataSlot);
int ReceiveValue(int *value, int *dataSlot);
int TransmitGlobal(int *globalValue, const char *globalName);

void receive2PVSData(MultiplayerData *data);
void receive2PVSMatchCode(int code);

int ShowPromoPopup(int *a1, const char *popupName);
void ShowWebsite(int websiteID);

int ExitGame();
int OpenModMenu();

#if RETRO_USE_MOD_LOADER
void initMods();
bool loadMod(ModInfo *info, std::string modsPath, std::string folder, bool active);
void saveMods();

void RefreshEngine();
#endif

#endif //! USERDATA_H
