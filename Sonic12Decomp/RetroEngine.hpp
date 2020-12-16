#ifndef RETROENGINE_H
#define RETROENGINE_H

// Disables POSIX use c++ name blah blah stuff
#pragma warning(disable : 4996)

// ================
// STANDARD LIBS
// ================
#include <stdio.h>
#include <string.h>

// ================
// STANDARD TYPES
// ================
typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long long ulong;

#define RSDK_DEBUG (1)

// Platforms
#define RETRO_WIN      (0)
#define RETRO_OSX      (1)
#define RETRO_XBOX_360 (2)
#define RETRO_PS3      (3)
#define RETRO_iOS      (4)
#define RETRO_ANDROID  (5)
#define RETRO_WP7      (6)

#define RETRO_PLATFORM (RETRO_WIN)

#if RETRO_PLATFORM == RETRO_WINDOWS || RETRO_PLATFORM == RETRO_OSX
#define RETRO_USING_SDL (1)
#else //Since its an else & not an elif these platforms probably aren't supported yet
#define RETRO_USING_SDL (0)
#endif

enum RetroLanguages {
    RETRO_EN = 0,
    RETRO_FR = 1,
    RETRO_IT = 2,
    RETRO_DE = 3,
    RETRO_ES = 4,
    RETRO_JP = 5
};

enum RetroStates {
    ENGINE_DEVMENU         = 0,
    ENGINE_MAINGAME        = 1,
    ENGINE_INITDEVMENU     = 2,
    ENGINE_EXITGAME        = 3,
    ENGINE_SCRIPTERROR     = 4,
    ENGINE_ENTER_HIRESMODE = 5,
    ENGINE_EXIT_HIRESMODE  = 6,
    ENGINE_PAUSE           = 7,
    ENGINE_WAIT            = 8,
    ENGINE_VIDEOWAIT       = 9,
};

//enum RetroEngineCallbacks {
//
//};

enum RetroBytecodeFormat {
    BYTECODE_MOBILE = 0,
    BYTECODE_PC     = 1,
};

// General Defines
#define SCREEN_YSIZE (240)
#define SCREEN_CENTERY (SCREEN_YSIZE / 2)

#if RETRO_PLATFORM == RETRO_WIN
#include <SDL.h>
#include <vorbis/vorbisfile.h>
#include <theora/theora.h>
#include <theoraplay.h>
#endif

//Utils
#include "Ini.hpp"

#include "Math.hpp"
#include "String.hpp"
#include "Reader.hpp"
#include "Animation.hpp"
#include "Audio.hpp"
#include "Input.hpp"
#include "Object.hpp"
#include "Palette.hpp"
#include "Drawing.hpp"
#include "Scene3D.hpp"
#include "Collision.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Sprite.hpp"
#include "Text.hpp"
#include "Video.hpp"
#include "Userdata.hpp"
#include "Debug.hpp"

//Native Entities
#include "RetroGameLoop.hpp"

#define GLOBALVAR_COUNT (0x100)

class RetroEngine
{
public:
    bool usingDataFile = false;
    bool usingBytecode = false;

    bool initialised = false;
    bool running     = false;

    int gameMode     = 1;
    int language     = RETRO_EN;
    int message      = 0;
    bool highResMode = false;

    bool trialMode      = false;
    bool onlineActive   = true;
    bool hapticsEnabled = true;

    int frameSkipSetting = 0;
    int frameSkipTimer   = 0;

    
    // Ported from RSDKv5
    bool devMenu = false;
    int startList  = 0;
    int startStage = 0;
    int gameSpeed        = 1;
    int fastForwardSpeed = 8;
    bool masterPaused    = false;
    bool frameStep       = false;

    void Init();
    void Run();

    void LoadGameConfig(const char *Filepath);

    void AwardAchievement(int id, int status);

    void SetAchievement(int achievementID, int achievementDone);
    void SetLeaderboard(int leaderboardID, int result);
    void LoadAchievementsMenu();
    void LoadLeaderboardsMenu();

    int callbackMessage = 0;
    int prevMessage     = 0;
    int waitValue       = 0;
    void Callback(int callbackID);

    char gameWindowText[0x40];
    char gameDescriptionText[0x100];
    const char *gameVersion       = "1.0.0";
    const char *gamePlatform      = "Standard";
    const char *gameRenderType    = "SW_Rendering";
    const char *gameHapticSetting = "Use_Haptics"; //" No_Haptics "; //No Haptics is default for pc but people with controllers exist
    //Mobile settings
    //const char *GamePlatform = "Mobile";
    //const char *GameRenderType = "HW_Rendering";
    //const char *GameHapticSetting = "Use_Haptics";

    ushort *frameBuffer = nullptr;
    uint *videoFrameBuffer = nullptr;

    bool isFullScreen = false;

    bool fullScreen = false;
    bool borderless = false;
    bool vsync = false;
    int windowScale = 2;
    int refreshRate = 60;

#if RETRO_USING_SDL
    SDL_Window *window        = nullptr;
    SDL_Renderer *renderer    = nullptr;
    SDL_Texture *screenBuffer = nullptr;
    SDL_Texture *videoBuffer = nullptr;

    SDL_Event sdlEvents;
#endif
};

extern RetroEngine Engine;

extern int GlobalVariablesCount;
extern int GlobalVariables[GLOBALVAR_COUNT];
extern char GlobalVariableNames[GLOBALVAR_COUNT][0x20];
#endif // !RETROENGINE_H
