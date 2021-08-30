#ifndef MOD_API_H
#define MOD_API_H

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
    bool disableFocusPause;
    bool active;
};

extern std::vector<ModInfo> modList;
extern int activeMod;

extern char modsPath[0x100];

extern char modTypeNames[OBJECT_COUNT][0x40];
extern char modScriptPaths[OBJECT_COUNT][0x40];
extern byte modScriptFlags[OBJECT_COUNT];
extern byte modObjCount;

inline void SetActiveMod(int id) { activeMod = id; }

void initMods();
bool loadMod(ModInfo *info, std::string modsPath, std::string folder, bool active);
void saveMods();

int OpenModMenu();

void RefreshEngine();

int GetSceneID(byte listID, const char *sceneName);

#endif

#endif