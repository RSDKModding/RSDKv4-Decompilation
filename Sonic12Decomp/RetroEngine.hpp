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

// Platform types
#define RETRO_STANDARD      (0)
#define RETRO_MOBILE        (1)

#if defined _WIN32
#define RETRO_PLATFORM (RETRO_WIN)
#define RETRO_PLATTYPE (RETRO_STANDARD)
#elif defined __APPLE__
#if __IPHONEOS__
#define RETRO_PLATTYPE (RETRO_iOS)
#define RETRO_PLATTYPE (RETRO_MOBILE)
#else
#define RETRO_PLATFORM (RETRO_OSX)
#define RETRO_PLATTYPE (RETRO_STANDARD)
#endif
#else
#define RETRO_PLATFORM (RETRO_WIN)
#define RETRO_PLATTYPE (RETRO_STANDARD)
#endif

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
    RETRO_JP = 5,
    RETRO_PT = 6,
    RETRO_RU = 7,
    RETRO_KO = 8,
    RETRO_ZH = 9,
    RETRO_ZS = 10,
};

enum RetroStates {
    ENGINE_DEVMENU     = 0,
    ENGINE_MAINGAME    = 1,
    ENGINE_INITDEVMENU = 2,
    ENGINE_WAIT        = 3,
    ENGINE_SCRIPTERROR = 4,
    ENGINE_INITPAUSE   = 5,
    ENGINE_EXITPAUSE   = 6,
    ENGINE_7           = 7,
    ENGINE_8           = 8,
};

//enum RetroEngineCallbacks {
//
//};

enum RetroGameType {
    GAME_UNKNOWN = 0,
    GAME_SONIC1  = 1,
    GAME_SONIC2  = 2,
};

// General Defines
#define SCREEN_YSIZE (240)
#define SCREEN_CENTERY (SCREEN_YSIZE / 2)

#if RETRO_PLATFORM == RETRO_WIN
#include <SDL.h>
#include <vorbis/vorbisfile.h>
#elif RETRO_PLATFORM == RETRO_OSX
#include <SDL2/SDL.h>
#include <Vorbis/vorbisfile.h>

#include "cocoaHelpers.hpp"
#endif

extern bool usingCWD;

//Utils
#include "Ini.hpp"

#include "Math.hpp"
#include "Reader.hpp"
#include "String.hpp"
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
#include "Userdata.hpp"
#include "Debug.hpp"

//Native Entities
#include "RetroGameLoop.hpp"

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

    bool showPaletteOverlay = false;

    void Init();
    void Run();

    bool LoadGameConfig(const char *Filepath);

    int callbackMessage = 0;
    int prevMessage     = 0;
    int waitValue       = 0;
    void Callback(int callbackID);

    char gameWindowText[0x40];
    char gameDescriptionText[0x100];
    const char *gameVersion       = "1.0.0";
    const char *gamePlatform      = "Standard";
    const char *gameRenderType    = "SW_Rendering";
    const char *gameHapticSetting = "Use_Haptics";
    //Mobile settings
    //const char *GamePlatform = "Mobile";
    //const char *GameRenderType = "HW_Rendering";
    //const char *GameHapticSetting = "Use_Haptics";

    int gameType = GAME_UNKNOWN;

    ushort *frameBuffer = nullptr;

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

    SDL_Event sdlEvents;
#endif
};

extern RetroEngine Engine;
#endif // !RETROENGINE_H
