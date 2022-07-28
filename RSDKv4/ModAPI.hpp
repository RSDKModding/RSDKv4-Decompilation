#ifndef MOD_API_H
#define MOD_API_H

#define PLAYER_COUNT (0x10)

extern char playerNames[PLAYER_COUNT][0x20];
extern byte playerCount;

#if RETRO_USE_MOD_LOADER
#include <string>
#include <map>
#include <unordered_map>
#include <tinyxml2.h>

struct ModInfo {
    std::string name;
    std::string desc;
    std::string author;
    std::string version;
    std::map<std::string, std::string> fileMap;
    std::string folder;
    bool useScripts;
    bool skipStartMenu;
    int disableFocusPause;
    bool redirectSave;
    std::string savePath;
    bool forceSonic1;
    bool active;
};

extern std::vector<ModInfo> modList;
extern int activeMod;

extern char modsPath[0x100];

extern bool redirectSave;

extern char modTypeNames[OBJECT_COUNT][0x40];
extern char modScriptPaths[OBJECT_COUNT][0x40];
extern byte modScriptFlags[OBJECT_COUNT];
extern byte modObjCount;

inline void SetActiveMod(int id) { activeMod = id; }

void InitMods();
bool LoadMod(ModInfo *info, std::string modsPath, std::string folder, bool active);
void ScanModFolder(ModInfo *info);
void SaveMods();

int OpenModMenu();

void RefreshEngine();
void GetModCount();
void GetModName(int *textMenu, int *highlight, uint *id, int *unused);
void GetModDescription(int *textMenu, int *highlight, uint *id, int *unused);
void GetModAuthor(int *textMenu, int *highlight, uint *id, int *unused);
void GetModVersion(int *textMenu, int *highlight, uint *id, int *unused);
void GetModActive(uint *id, int *unused);
void SetModActive(uint *id, int *active);
void MoveMod(uint *id, int *up);

#endif

#if RETRO_USE_MOD_LOADER || !RETRO_USE_ORIGINAL_CODE
extern char savePath[0x100];
int GetSceneID(byte listID, const char *sceneName);
#endif

#endif