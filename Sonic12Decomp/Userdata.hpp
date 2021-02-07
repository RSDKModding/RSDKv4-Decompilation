#ifndef USERDATA_H
#define USERDATA_H

#define GLOBALVAR_COUNT (0x100)

#define ACHIEVEMENT_MAX (0x40)
#define LEADERBOARD_MAX (0x80)

#if !RETRO_USE_ORIGINAL_CODE
#define MOD_MAX (0x100)
#endif

#define SAVEDATA_MAX (0x2000)

#if !RETRO_USE_ORIGINAL_CODE
#include <string>
#include <map>
#include <unordered_map>
#endif

enum OnlineMenuTypes {
    ONLINEMENU_ACHIEVEMENTS = 0,
    ONLINEMENU_LEADERBOARDS = 1,
};

struct Achievement {
    char name[0x40];
    int status;
};

struct LeaderboardEntry {
    int status;
};

struct MultiplayerData {
    int type;
    int data[0x1FF];
};

#if !RETRO_USE_ORIGINAL_CODE
struct ModInfo {
    std::string name;
    std::string desc;
    std::string author;
    std::string version;
    std::map<std::string, std::string> fileMap;
    std::string folder;
    int iconTexture;
    bool useScripts;
    bool skipStartMenu;
    bool active;
};
#endif

extern int (*nativeFunction[16])(int, void *);
extern int nativeFunctionCount;

extern int globalVariablesCount;
extern int globalVariables[GLOBALVAR_COUNT];
extern char globalVariableNames[GLOBALVAR_COUNT][0x20];

extern char gamePath[0x100];
extern int saveRAM[SAVEDATA_MAX];
extern Achievement achievements[ACHIEVEMENT_MAX];
extern LeaderboardEntry leaderboard[LEADERBOARD_MAX];

extern MultiplayerData multiplayerDataIN;
extern MultiplayerData multiplayerDataOUT;
extern int matchValueData[0x100];
extern int matchValueReadPos;
extern int matchValueWritePos;

extern int sendDataMethod;
extern int sendCounter;

#if !RETRO_USE_ORIGINAL_CODE
extern ModInfo modList[MOD_MAX];
extern int modCount;
extern bool forceUseScripts;
extern bool skipStartMenu;
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

inline void AddNativeFunction(const char *name, int (*funcPtr)(int, void *))
{
    if (nativeFunctionCount > 0xF)
        return;
    SetGlobalVariableByName(name, nativeFunctionCount);
    nativeFunction[nativeFunctionCount++] = funcPtr;
}

inline bool ReadSaveRAMData()
{
    char buffer[0x100];
#if RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/SData.bin", getResourcesPath());
    else
        sprintf(buffer, "%sSData.bin", gamePath);
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
#if RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/SData.bin", getResourcesPath());
    else
        sprintf(buffer, "%sSData.bin", gamePath);
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

void AwardAchievement(int id, int status);

int SetAchievement(int achievementID, void *achDone);
int SetLeaderboard(int leaderboardID, void *res);
inline void LoadAchievementsMenu()
{
#if !RETRO_USE_ORIGINAL_CODE
    ReadUserdata();
#endif
}
inline void LoadLeaderboardsMenu()
{
#if !RETRO_USE_ORIGINAL_CODE
    ReadUserdata();
#endif
}

int Connect2PVS(int a1, void *a2);
int Disconnect2PVS(int a1, void *a2);
int SendEntity(int a1, void *a2);
int SendValue(int a1, void *a2);
int ReceiveEntity(int a1, void *a2);
int ReceiveValue(int a1, void *a2);
int TransmitGlobal(int a1, void *a2);

void receive2PVSData(MultiplayerData *data);
void receive2PVSMatchCode(int code);

int ShowPromoPopup(int a1, void *a2);

#if !RETRO_USE_ORIGINAL_CODE
void initMods();
void saveMods();
#endif

#endif //! USERDATA_H
