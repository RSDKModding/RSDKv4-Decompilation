#ifndef USERDATA_H
#define USERDATA_H

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

extern int(__cdecl *nativeFunction[16])(int, void *);

extern char gamePath[0x100];
extern int saveRAM[SAVEDATA_MAX];
extern Achievement achievements[ACHIEVEMENT_MAX];
extern LeaderboardEntry leaderboard[LEADERBOARD_MAX];


inline bool ReadSaveRAMData()
{
    char buffer[0x100];
    sprintf(buffer, "%ssavedata.sav", gamePath);
    FILE *saveFile = fopen(buffer, "rb");
    if (!saveFile)
        return false;
    fread(saveRAM, 4u, SAVEDATA_MAX, saveFile);
    fclose(saveFile);
    return true;
}

inline bool WriteSaveRAMData()
{
    char buffer[0x100];
    sprintf(buffer, "%ssavedata.sav", gamePath);
    FILE *saveFile = fopen(buffer, "wb");
    if (!saveFile)
        return false;
    fwrite(saveRAM, 4u, SAVEDATA_MAX, saveFile);
    fclose(saveFile);
    return true;
}

void InitUserdata();
void writeSettings();
void ReadUserdata();
void WriteUserdata();

#endif //!USERDATA_H