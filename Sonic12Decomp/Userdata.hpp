#ifndef USERDATA_H
#define USERDATA_H

#define GLOBALVAR_COUNT (0x100)

#define ACHIEVEMENT_MAX (0x40)
#define LEADERBOARD_MAX (0x80)

#define SAVEDATA_MAX (0x2000)

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

extern int(*nativeFunction[16])(int, void *);
extern int nativeFunctionCount;

extern int globalVariablesCount;
extern int globalVariables[GLOBALVAR_COUNT];
extern char globalVariableNames[GLOBALVAR_COUNT][0x20];

extern char gamePath[0x100];
extern int saveRAM[SAVEDATA_MAX];
extern Achievement achievements[ACHIEVEMENT_MAX];
extern LeaderboardEntry leaderboard[LEADERBOARD_MAX];

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

inline void AddNativeFunction(const char* name, int (*funcPtr)(int, void*)) {
    if (nativeFunctionCount > 0xF)
        return;
    SetGlobalVariableByName(name, nativeFunctionCount);
    nativeFunction[nativeFunctionCount++] = funcPtr;
}

inline bool ReadSaveRAMData()
{
    char buffer[0x100];
#if RETRO_PLATFORM == RETRO_OSX
    if (!usingCWD)
        sprintf(buffer, "%s/savedata.sav", getResourcesPath());
    else
        sprintf(buffer, "%ssavedata.sav", gamePath);
#else
    sprintf(buffer, "%ssavedata.sav", gamePath);
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
#if RETRO_PLATFORM == RETRO_OSX
    if (!usingCWD)
        sprintf(buffer, "%s/savedata.sav", getResourcesPath());
    else
        sprintf(buffer, "%ssavedata.sav", gamePath);
#else
    sprintf(buffer, "%ssavedata.sav", gamePath);
#endif

    FileIO *saveFile = fOpen(buffer, "wb");
    if (!saveFile)
        return false;
    fWrite(saveRAM, 4u, SAVEDATA_MAX, saveFile);
    fClose(saveFile);
    return true;
}

void InitUserdata();
void writeSettings();
void ReadUserdata();
void WriteUserdata();

void AwardAchievement(int id, int status);

int SetAchievement(int achievementID, void* achDone);
int SetLeaderboard(int leaderboardID, void *res);
inline void LoadAchievementsMenu() { ReadUserdata(); }
inline void LoadLeaderboardsMenu() { ReadUserdata(); }

int Connect2PVS(int a1, void *a2);
int Disconnect2PVS(int a1, void *a2);
int SendEntity(int a1, void *a2);
int SendValue(int a1, void *a2);
int ReceiveEntity(int a1, void *a2);
int ReceiveValue(int a1, void *a2);
int TransmitGlobal(int a1, void *a2);

int ShowPromoPopup(int a1, void *a2);

#endif //!USERDATA_H