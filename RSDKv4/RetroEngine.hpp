#ifndef RETROENGINE_H
#define RETROENGINE_H

// Disables POSIX use c++ name blah blah stuff
#pragma warning(disable : 4996)

// Setting this to true removes (almost) ALL changes from the original code, the trade off is that a playable game cannot be built, it is advised to
// be set to true only for preservation purposes
#ifndef RETRO_USE_ORIGINAL_CODE
#define RETRO_USE_ORIGINAL_CODE (0)
#endif

#ifndef RETRO_USE_MOD_LOADER
#define RETRO_USE_MOD_LOADER (!RETRO_USE_ORIGINAL_CODE && 1)
#endif

#ifndef RETRO_USE_NETWORKING
#define RETRO_USE_NETWORKING (!RETRO_USE_ORIGINAL_CODE && 1)
#endif

// Forces all DLC flags to be disabled, this should be enabled in any public releases
#ifndef RSDK_AUTOBUILD
#define RSDK_AUTOBUILD (0)
#endif

// ================
// STANDARD LIBS
// ================
#include <stdio.h>
#include <string.h>
#include <cmath>
#if RETRO_USE_MOD_LOADER
#include <regex>
#endif

// ================
// STANDARD TYPES
// ================
typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short ushort;
typedef unsigned int uint;
// typedef unsigned long long ulong;

// Platforms (RSDKv4 only defines these 7 (I assume), but feel free to add your own custom platform define for easier platform code changes)
#define RETRO_WIN      (0)
#define RETRO_OSX      (1)
#define RETRO_XBOX_360 (2)
#define RETRO_PS3      (3)
#define RETRO_iOS      (4)
#define RETRO_ANDROID  (5)
#define RETRO_WP7      (6)
// Custom Platforms start here
#define RETRO_UWP   (7)
#define RETRO_LINUX (8)

// Platform types (Game manages platform-specific code such as HUD position using this rather than the above)
#define RETRO_STANDARD (0)
#define RETRO_MOBILE   (1)

#if defined _WIN32

#if defined WINAPI_FAMILY
#if WINAPI_FAMILY != WINAPI_FAMILY_APP
#define RETRO_PLATFORM   (RETRO_WIN)
#define RETRO_DEVICETYPE (RETRO_STANDARD)
#else
#include <WInRTIncludes.hpp>

#define RETRO_PLATFORM   (RETRO_UWP)
#define RETRO_DEVICETYPE (UAP_GetRetroGamePlatform())
#endif
#else
#define RETRO_PLATFORM   (RETRO_WIN)
#define RETRO_DEVICETYPE (RETRO_STANDARD)
#endif

#elif defined __APPLE__
#if __IPHONEOS__
#define RETRO_PLATFORM   (RETRO_iOS)
#define RETRO_DEVICETYPE (RETRO_MOBILE)
#else
#define RETRO_PLATFORM   (RETRO_OSX)
#define RETRO_DEVICETYPE (RETRO_STANDARD)
#endif
#elif defined __ANDROID__
#define RETRO_PLATFORM   (RETRO_ANDROID)
#define RETRO_DEVICETYPE (RETRO_MOBILE)
#include <jni.h>
#elif defined(__linux__)
#define RETRO_PLATFORM   (RETRO_LINUX)
#define RETRO_DEVICETYPE (RETRO_STANDARD)
#else
//#error "No Platform was defined"
#define RETRO_PLATFORM   (RETRO_LINUX)
#define RETRO_DEVICETYPE (RETRO_STANDARD)
#endif

#define DEFAULT_SCREEN_XSIZE 424
#define DEFAULT_FULLSCREEN   false
#define RETRO_USING_MOUSE
#define RETRO_USING_TOUCH

#ifndef BASE_PATH
#define BASE_PATH ""
#endif

#if !defined(RETRO_USE_SDL2) && !defined(RETRO_USE_SDL1)
#define RETRO_USE_SDL2 (1)
#endif

#if RETRO_PLATFORM == RETRO_WIN || RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_LINUX || RETRO_PLATFORM == RETRO_UWP                       \
    || RETRO_PLATFORM == RETRO_ANDROID
#ifdef RETRO_USE_SDL2
#define RETRO_USING_SDL1 (0)
#define RETRO_USING_SDL2 (1)
#elif defined(RETRO_USE_SDL1)
#define RETRO_USING_SDL1 (1)
#define RETRO_USING_SDL2 (0)
#endif
#else // Since its an else & not an elif these platforms probably aren't supported yet
#define RETRO_USING_SDL1 (0)
#define RETRO_USING_SDL2 (0)
#endif

#if RETRO_PLATFORM == RETRO_iOS || RETRO_PLATFORM == RETRO_ANDROID || RETRO_PLATFORM == RETRO_WP7
#define RETRO_GAMEPLATFORM (RETRO_MOBILE)
#elif RETRO_PLATFORM == RETRO_UWP
#define RETRO_GAMEPLATFORM (UAP_GetRetroGamePlatform())
#else
#define RETRO_GAMEPLATFORM (RETRO_STANDARD)
#endif

#define RETRO_SW_RENDER (0)
#define RETRO_HW_RENDER (1)

#ifdef USE_SW_REN
#define RETRO_RENDERTYPE (RETRO_SW_RENDER)
#elif defined(USE_HW_REN)
#define RETRO_RENDERTYPE (RETRO_HW_RENDER)
#elif !defined(RETRO_RENDERTYPE)
#define RETRO_RENDERTYPE (RETRO_SW_RENDER)
#endif

#ifndef RETRO_USING_OPENGL
#define RETRO_USING_OPENGL (1)
#endif

#define RETRO_SOFTWARE_RENDER (RETRO_RENDERTYPE == RETRO_SW_RENDER)
//#define RETRO_HARDWARE_RENDER (RETRO_RENDERTYPE == RETRO_HW_RENDER)

#if RETRO_USING_OPENGL
#if RETRO_PLATFORM == RETRO_ANDROID
#define GL_GLEXT_PROTOTYPES

#include <GLES/gl.h>
#include <GLES/glext.h>

#undef glGenFramebuffers
#undef glBindFramebuffer
#undef glFramebufferTexture2D
#undef glDeleteFramebuffers

#undef GL_FRAMEBUFFER
#undef GL_COLOR_ATTACHMENT0
#undef GL_FRAMEBUFFER_BINDING

#define glGenFramebuffers      glGenFramebuffersOES
#define glBindFramebuffer      glBindFramebufferOES
#define glFramebufferTexture2D glFramebufferTexture2DOES
#define glDeleteFramebuffers   glDeleteFramebuffersOES

#define GL_FRAMEBUFFER         GL_FRAMEBUFFER_OES
#define GL_COLOR_ATTACHMENT0   GL_COLOR_ATTACHMENT0_OES
#define GL_FRAMEBUFFER_BINDING GL_FRAMEBUFFER_BINDING_OES
#elif RETRO_PLATFORM == RETRO_OSX
#define GL_GLEXT_PROTOTYPES
#define GL_SILENCE_DEPRECATION

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

#undef glGenFramebuffers
#undef glBindFramebuffer
#undef glFramebufferTexture2D
#undef glDeleteFramebuffers

#undef GL_FRAMEBUFFER
#undef GL_COLOR_ATTACHMENT0
#undef GL_FRAMEBUFFER_BINDING

#define glGenFramebuffers      glGenFramebuffersEXT
#define glBindFramebuffer      glBindFramebufferEXT
#define glFramebufferTexture2D glFramebufferTexture2DEXT
#define glDeleteFramebuffers   glDeleteFramebuffersEXT

#define GL_FRAMEBUFFER         GL_FRAMEBUFFER_EXT
#define GL_COLOR_ATTACHMENT0   GL_COLOR_ATTACHMENT0_EXT
#define GL_FRAMEBUFFER_BINDING GL_FRAMEBUFFER_BINDING_EXT
#else
#include <GL/glew.h>
#endif
#endif

#define RETRO_USE_HAPTICS (1)

// NOTE: This is only used for rev00 stuff, it was removed in rev01 and later builds
#if RETRO_PLATFORM <= RETRO_WP7
#define RETRO_GAMEPLATFORMID (RETRO_PLATFORM)
#else

// use *this* macro to determine what platform the game thinks its running on (since only the first 7 platforms are supported natively by scripts)
#if RETRO_PLATFORM == RETRO_LINUX
#define RETRO_GAMEPLATFORMID (RETRO_WIN)
#elif RETRO_PLATFORM == RETRO_UWP
#define RETRO_GAMEPLATFORMID (UAP_GetRetroGamePlatformId())
#else
#error Unspecified RETRO_GAMEPLATFORMID
#endif

#endif

// Determines which revision to use (see defines below for specifics). Datafiles from REV00 and REV01 builds will not work on later revisions and vice versa.
#ifndef RSDK_REVISION
#define RSDK_REVISION (3)
#endif

// Revision from early versions of Sonic 1
#define RETRO_REV00 (RSDK_REVISION == 0)

// Revision from early versions of Sonic 2
#define RETRO_REV01 (RSDK_REVISION >= 1)

// Revision from the S3&K POC, this is also used in the Sega Forever versions of S1 & S2
#define RETRO_REV02 (RSDK_REVISION >= 2)

// Revision included as part of RSDKv5U (Sonic Origins)
#define RETRO_REV03 (RSDK_REVISION >= 3)

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

#if RETRO_REV00
enum RetroEngineMessages {
    MESSAGE_NONE      = 0,
    MESSAGE_MESSAGE_1 = 1,
    MESSAGE_LOSTFOCUS = 2,
    MESSAGE_MESSAGE_3 = 3,
    MESSAGE_MESSAGE_4 = 4,
};
#endif

enum RetroStates {
    ENGINE_DEVMENU     = 0,
    ENGINE_MAINGAME    = 1,
    ENGINE_INITDEVMENU = 2,
    ENGINE_WAIT        = 3,
    ENGINE_SCRIPTERROR = 4,
    ENGINE_INITPAUSE   = 5,
    ENGINE_EXITPAUSE   = 6,
    ENGINE_ENDGAME     = 7,
    ENGINE_RESETGAME   = 8,

#if !RETRO_USE_ORIGINAL_CODE && RETRO_USE_NETWORKING
    // Custom GameModes (required to make some features work)
    ENGINE_CONNECT2PVS = 0x80,
    ENGINE_WAIT2PVS    = 0x81,
#endif
#if RETRO_USE_MOD_LOADER
    ENGINE_INITMODMENU = 0x82,
#endif
};

enum RetroGameType {
    GAME_UNKNOWN = 0,
    GAME_SONIC1  = 1,
    GAME_SONIC2  = 2,
};

// General Defines
#define SCREEN_YSIZE   (240)
#define SCREEN_CENTERY (SCREEN_YSIZE / 2)

#if RETRO_PLATFORM == RETRO_WIN || RETRO_PLATFORM == RETRO_UWP || RETRO_PLATFORM == RETRO_ANDROID || RETRO_PLATFORM == RETRO_LINUX
#if RETRO_USING_SDL2
#include <SDL.h>
#elif RETRO_USING_SDL1
#include <SDL.h>
#endif
#include <vorbis/vorbisfile.h>
#elif RETRO_PLATFORM == RETRO_OSX
#include <SDL2/SDL.h>
#include <Vorbis/vorbisfile.h>

#include "cocoaHelpers.hpp"

#elif RETRO_USING_SDL2
#include <SDL2/SDL.h>
#include <vorbis/vorbisfile.h>
#else

#endif

#if !RETRO_USE_ORIGINAL_CODE
extern bool usingCWD;
extern bool engineDebugMode;
#endif

// Utils
#if !RETRO_USE_ORIGINAL_CODE
#include "Ini.hpp"
#endif

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
#include "Networking.hpp"
#include "Renderer.hpp"
#include "Userdata.hpp"
#include "Debug.hpp"
#include "ModAPI.hpp"

// Native Entities
#include "NativeObjects.hpp"

class RetroEngine
{
public:
    RetroEngine()
    {
        if (RETRO_GAMEPLATFORM == RETRO_STANDARD) {
            gamePlatform   = "STANDARD";
            gameDeviceType = RETRO_STANDARD;
        }
        else {
            gamePlatform   = "MOBILE";
            gameDeviceType = RETRO_MOBILE;
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    bool usingDataFile_Config = false;
#endif
    bool usingDataFile = false;
    bool usingBytecode = false;
#if RETRO_REV03 && !RETRO_USE_ORIGINAL_CODE
    bool usingOrigins = false;
#endif

    char dataFile[RETRO_PACKFILE_COUNT][0x80];

    bool initialised = false;
    bool running     = false;
    double deltaTime = 0;

    int gameMode = ENGINE_MAINGAME;
    int language = RETRO_EN;
#if RETRO_REV00
    int message = 0;
#endif
    int gameDeviceType    = RETRO_STANDARD;
    int globalBoxRegion   = REGION_JP;
    bool nativeMenuFadeIn = false;

    bool trialMode        = false;
    bool onlineActive     = true;
    bool useHighResAssets = false;
#if RETRO_USE_HAPTICS
    bool hapticsEnabled = true;
#endif

    int frameSkipSetting = 0;
    int frameSkipTimer   = 0;

#if !RETRO_USE_ORIGINAL_CODE
    // Ported from RSDKv5
    int startList_Game  = -1;
    int startStage_Game = -1;

    bool consoleEnabled  = false;
    bool devMenu         = false;
    int startList        = -1;
    int startStage       = -1;
    int startPlayer      = -1;
    int startSave        = -1;
    int gameSpeed        = 1;
    int fastForwardSpeed = 8;
    bool masterPaused    = false;
    bool frameStep       = false;
    int dimTimer         = 0;
    int dimLimit         = 0;
    float dimPercent     = 1.0;
    float dimMax         = 1.0;

    char startSceneFolder[0x10];
    char startSceneID[0x10];

    bool showPaletteOverlay = false;
    bool useHQModes         = true;

    bool hasFocus  = true;
    int focusState = 0;
#endif

    void Init();
    void Run();

    bool LoadGameConfig(const char *filepath);
#if RETRO_USE_MOD_LOADER
    void LoadXMLWindowText();
    void LoadXMLVariables();
    void LoadXMLPalettes();
    void LoadXMLObjects();
    void LoadXMLSoundFX();
    void LoadXMLPlayers(TextMenu *menu);
    void LoadXMLStages(TextMenu *menu, int listNo);
#endif

    char gameWindowText[0x40];
    char gameDescriptionText[0x100];
#ifdef DECOMP_VERSION
    const char *gameVersion = DECOMP_VERSION;
#else
    const char *gameVersion  = "1.3.2";
#endif
    const char *gamePlatform = nullptr;

    int gameTypeID       = 0;
    const char *releaseType = "USE_STANDALONE";

#if RETRO_RENDERTYPE == RETRO_SW_RENDER
    const char *gameRenderType = "SW_RENDERING";
#elif RETRO_RENDERTYPE == RETRO_HW_RENDER
    const char *gameRenderType = "HW_RENDERING";
#endif

#if RETRO_USE_HAPTICS
    const char *gameHapticSetting = "USE_F_FEEDBACK"; // None is default, but people with controllers exist
#else
    const char *gameHapticSetting = "NO_F_FEEDBACK";
#endif

#if !RETRO_USE_ORIGINAL_CODE
    byte gameType = GAME_UNKNOWN;
#if RETRO_USE_MOD_LOADER
    bool modMenuCalled = false;
    bool forceSonic1   = false;
#endif
#endif

#if RETRO_SOFTWARE_RENDER
    ushort *frameBuffer   = nullptr;
    ushort *frameBuffer2x = nullptr;
#endif
    uint *texBuffer = nullptr;

#if !RETRO_USE_ORIGINAL_CODE
    bool isFullScreen = false;

    bool startFullScreen  = false; // if should start as fullscreen
    bool borderless       = false;
    bool vsync            = false;
    int scalingMode       = 0;
    int windowScale       = 2;
    int refreshRate       = 60; // user-picked screen update rate
    int screenRefreshRate = 60; // hardware screen update rate
    int targetRefreshRate = 60; // game logic update rate

    int renderFrameIndex = 0;
    int skipFrameIndex   = 0;

    int windowXSize; // width of window/screen in the previous frame
    int windowYSize; // height of window/screen in the previous frame
#endif

#if !RETRO_USE_ORIGINAL_CODE
#if RETRO_USING_SDL2
    SDL_Window *window = nullptr;
#if !RETRO_USING_OPENGL
    SDL_Renderer *renderer = nullptr;
#if RETRO_SOFTWARE_RENDER
    SDL_Texture *screenBuffer   = nullptr;
    SDL_Texture *screenBuffer2x = nullptr;
#endif // RETRO_SOFTWARE_RENDERER
#endif

    SDL_Event sdlEvents;

#if RETRO_USING_OPENGL
    SDL_GLContext glContext; // OpenGL context
#endif // RETRO_USING_OPENGL
#endif // RETRO_USING_SDL2

#if RETRO_USING_SDL1
    SDL_Surface *windowSurface = nullptr;

    SDL_Surface *screenBuffer   = nullptr;
    SDL_Surface *screenBuffer2x = nullptr;

    SDL_Event sdlEvents;
#endif // RETRO_USING_SDL1
#endif //! RETRO_USE_ORIGINAL_CODE
};

extern RetroEngine Engine;
#endif // !RETROENGINE_H
