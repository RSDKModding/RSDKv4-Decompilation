#include "RetroEngine.hpp"

int globalVariablesCount;
int globalVariables[GLOBALVAR_COUNT];
char globalVariableNames[GLOBALVAR_COUNT][0x20];

void *nativeFunction[NATIIVEFUNCTION_COUNT];
int nativeFunctionCount = 0;

char gamePath[0x100];
int saveRAM[SAVEDATA_SIZE];
Achievement achievements[ACHIEVEMENT_COUNT];
int achievementCount = 0;

LeaderboardEntry leaderboards[LEADERBOARD_COUNT];

MultiplayerData multiplayerDataIN  = MultiplayerData();
MultiplayerData multiplayerDataOUT = MultiplayerData();
int matchValueData[0x100];
byte matchValueReadPos  = 0;
byte matchValueWritePos = 0;

int vsGameLength = 4;
int vsItemMode   = 1;
int vsPlayerID   = 0;
bool vsPlaying   = false;

int sendCounter = 0;

#if RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_LINUX
#include <sys/stat.h>
#include <sys/types.h>
#endif

#if !RETRO_USE_ORIGINAL_CODE
bool forceUseScripts         = false;
bool forceUseScripts_Config  = false;
bool skipStartMenu           = false;
bool skipStartMenu_Config    = false;
int disableFocusPause        = 0;
int disableFocusPause_Config = 0;

bool useSGame = false;

bool ReadSaveRAMData()
{
    useSGame = false;
    char buffer[0x180];
#if RETRO_USE_MOD_LOADER
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/%sSData.bin", redirectSave ? modsPath : getResourcesPath(), savePath);
    else
        sprintf(buffer, "%s%sSData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/%sSData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/%sSData.bin", redirectSave ? modsPath : getDocumentsPath(), savePath);
#else
    sprintf(buffer, "%s%sSData.bin", redirectSave ? modsPath : gamePath, savePath);
#endif
#else
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/%sSData.bin", getResourcesPath(), savePath);
    else
        sprintf(buffer, "%s%sSData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/%sSData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/%sSData.bin", getDocumentsPath(), savePath);
#else
    sprintf(buffer, "%s%sSData.bin", gamePath, savePath);
#endif
#endif

    FileIO *saveFile = fOpen(buffer, "rb");
    if (!saveFile) {
#if RETRO_USE_MOD_LOADER
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/%sSGame.bin", redirectSave ? modsPath : getResourcesPath(), savePath);
        else
            sprintf(buffer, "%s%sSGame.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/%sSGame.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
        sprintf(buffer, "%s/%sSGame.bin", redirectSave ? modsPath : getDocumentsPath(), savePath);
#else
        sprintf(buffer, "%s%sSGame.bin", redirectSave ? modsPath : gamePath, savePath);
#endif
#else
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/%sSGame.bin", getResourcesPath(), savePath);
        else
            sprintf(buffer, "%s%sSGame.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/%sSGame.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
        sprintf(buffer, "%s/%sSGame.bin", getDocumentsPath(), savePath);
#else
        sprintf(buffer, "%s%sSGame.bin", gamePath, savePath);
#endif
#endif

        saveFile = fOpen(buffer, "rb");
        if (!saveFile)
            return false;
        useSGame = true;
    }
    fRead(saveRAM, sizeof(int), SAVEDATA_SIZE, saveFile);
    fClose(saveFile);
    return true;
}

bool WriteSaveRAMData()
{
    char buffer[0x180];

    if (!useSGame) {
#if RETRO_USE_MOD_LOADER
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/%sSData.bin", redirectSave ? modsPath : getResourcesPath(), savePath);
        else
            sprintf(buffer, "%s%sSData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/%sSData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
        sprintf(buffer, "%s/%sSData.bin", redirectSave ? modsPath : getDocumentsPath(), savePath);
#else
        sprintf(buffer, "%s%sSData.bin", redirectSave ? modsPath : gamePath, savePath);
#endif
#else
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/%sSData.bin", getResourcesPath(), savePath);
        else
            sprintf(buffer, "%s%sSData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/%sSData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
        sprintf(buffer, "%s/%sSData.bin", getDocumentsPath(), savePath);
#else
        sprintf(buffer, "%s%sSData.bin", gamePath, savePath);
#endif
#endif
    }
    else {
#if RETRO_USE_MOD_LOADER
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/%sSGame.bin", redirectSave ? modsPath : getResourcesPath(), savePath);
        else
            sprintf(buffer, "%s%sSGame.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/%sSGame.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
        sprintf(buffer, "%s/%sSGame.bin", redirectSave ? modsPath : getDocumentsPath(), savePath);
#else
        sprintf(buffer, "%s%sSGame.bin", redirectSave ? modsPath : gamePath, savePath);
#endif
#else
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/%sSGame.bin", getResourcesPath(), savePath);
        else
            sprintf(buffer, "%s%sSGame.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/%sSGame.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
        sprintf(buffer, "%s/%sSGame.bin", getDocumentsPath(), savePath);
#else
        sprintf(buffer, "%s%sSGame.bin", gamePath, savePath);
#endif
#endif
    }

    FileIO *saveFile = fOpen(buffer, "wb");
    if (!saveFile)
        return false;
    fWrite(saveRAM, sizeof(int), SAVEDATA_SIZE, saveFile);
    fClose(saveFile);
    return true;
}

void InitUserdata()
{
    // userdata files are loaded from this directory
    sprintf(gamePath, "%s", BASE_PATH);
#if RETRO_USE_MOD_LOADER
    sprintf(modsPath, "%s", BASE_PATH);
#endif

#if RETRO_PLATFORM == RETRO_OSX
    sprintf(gamePath, "%s/RSDKv4", getResourcesPath());
    sprintf(modsPath, "%s/RSDKv4/", getResourcesPath());

    mkdir(gamePath, 0777);
#elif RETRO_PLATFORM == RETRO_ANDROID
    {
        char buffer[0x200];

        JNIEnv *env      = (JNIEnv *)SDL_AndroidGetJNIEnv();
        jobject activity = (jobject)SDL_AndroidGetActivity();
        jclass cls(env->GetObjectClass(activity));
        jmethodID method = env->GetMethodID(cls, "getBasePath", "()Ljava/lang/String;");
        auto ret         = env->CallObjectMethod(activity, method);

        strcpy(buffer, env->GetStringUTFChars((jstring)ret, NULL));

        sprintf(gamePath, "%s", buffer);
#if RETRO_USE_MOD_LOADER
        sprintf(modsPath, "%s", buffer);
#endif

        env->DeleteLocalRef(activity);
        env->DeleteLocalRef(cls);
    }
#endif

    char buffer[0x100];
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/settings.ini", getResourcesPath());
    else
        sprintf(buffer, "%ssettings.ini", gamePath);
#elif RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_ANDROID
    sprintf(buffer, "%s/settings.ini", gamePath);
#else
    sprintf(buffer, BASE_PATH "settings.ini");
#endif
    FileIO *file = fOpen(buffer, "rb");
    if (!file) {
        IniParser ini;

        ini.SetBool("Dev", "DevMenu", Engine.devMenu = false);
        ini.SetBool("Dev", "EngineDebugMode", engineDebugMode = false);
        ini.SetBool("Dev", "TxtScripts", forceUseScripts = false);
        forceUseScripts_Config = forceUseScripts;
        ini.SetInteger("Dev", "StartingCategory", Engine.startList = 255);
        ini.SetInteger("Dev", "StartingScene", Engine.startStage = 255);
        ini.SetInteger("Dev", "StartingPlayer", Engine.startPlayer = 255);
        ini.SetInteger("Dev", "StartingSaveFile", Engine.startSave = 255);
        ini.SetInteger("Dev", "FastForwardSpeed", Engine.fastForwardSpeed = 8);
        Engine.startList_Game  = Engine.startList;
        Engine.startStage_Game = Engine.startStage;

        ini.SetBool("Dev", "UseHQModes", Engine.useHQModes = true);
        ini.SetString("Dev", "DataFile", (char *)"Data.rsdk");
        StrCopy(Engine.dataFile[0], "Data.rsdk");
        if (!StrComp(Engine.dataFile[1], "")) {
            ini.SetString("Dev", "DataFile2", (char *)"Data2.rsdk");
            StrCopy(Engine.dataFile[1], "Data2.rsdk");
        }
        if (!StrComp(Engine.dataFile[2], "")) {
            ini.SetString("Dev", "DataFile3", (char *)"Data3.rsdk");
            StrCopy(Engine.dataFile[2], "Data3.rsdk");
        }
        if (!StrComp(Engine.dataFile[3], "")) {
            ini.SetString("Dev", "DataFile4", (char *)"Data4.rsdk");
            StrCopy(Engine.dataFile[3], "Data4.rsdk");
        }

        ini.SetInteger("Game", "Language", Engine.language = RETRO_EN);
        ini.SetInteger("Game", "GameType", Engine.gameTypeID = 0);
        ini.SetBool("Game", "SkipStartMenu", skipStartMenu = false);
        skipStartMenu_Config = skipStartMenu;
        ini.SetInteger("Game", "DisableFocusPause", disableFocusPause = 0);
        disableFocusPause_Config = disableFocusPause;

#if RETRO_USE_NETWORKING
        ini.SetString("Network", "Host", (char *)"127.0.0.1");
        StrCopy(networkHost, "127.0.0.1");
        ini.SetInteger("Network", "Port", networkPort = 50);
#endif

        ini.SetBool("Window", "FullScreen", Engine.startFullScreen = DEFAULT_FULLSCREEN);
        ini.SetBool("Window", "Borderless", Engine.borderless = false);
        ini.SetBool("Window", "VSync", Engine.vsync = false);
        ini.SetInteger("Window", "ScalingMode", Engine.scalingMode = 0);
        ini.SetInteger("Window", "WindowScale", Engine.windowScale = 2);
        ini.SetInteger("Window", "ScreenWidth", SCREEN_XSIZE_CONFIG = DEFAULT_SCREEN_XSIZE);
        SCREEN_XSIZE = SCREEN_XSIZE_CONFIG;
        ini.SetInteger("Window", "RefreshRate", Engine.refreshRate = 60);
        ini.SetInteger("Window", "DimLimit", Engine.dimLimit = 300);
        Engine.dimLimit *= Engine.refreshRate;

        ini.SetFloat("Audio", "BGMVolume", bgmVolume / (float)MAX_VOLUME);
        ini.SetFloat("Audio", "SFXVolume", sfxVolume / (float)MAX_VOLUME);

#if RETRO_USING_SDL2
        ini.SetInteger("Keyboard 1", "Up", inputDevice[INPUT_UP].keyMappings = SDL_SCANCODE_UP);
        ini.SetInteger("Keyboard 1", "Down", inputDevice[INPUT_DOWN].keyMappings = SDL_SCANCODE_DOWN);
        ini.SetInteger("Keyboard 1", "Left", inputDevice[INPUT_LEFT].keyMappings = SDL_SCANCODE_LEFT);
        ini.SetInteger("Keyboard 1", "Right", inputDevice[INPUT_RIGHT].keyMappings = SDL_SCANCODE_RIGHT);
        ini.SetInteger("Keyboard 1", "A", inputDevice[INPUT_BUTTONA].keyMappings = SDL_SCANCODE_Z);
        ini.SetInteger("Keyboard 1", "B", inputDevice[INPUT_BUTTONB].keyMappings = SDL_SCANCODE_X);
        ini.SetInteger("Keyboard 1", "C", inputDevice[INPUT_BUTTONC].keyMappings = SDL_SCANCODE_C);
        ini.SetInteger("Keyboard 1", "X", inputDevice[INPUT_BUTTONX].keyMappings = SDL_SCANCODE_A);
        ini.SetInteger("Keyboard 1", "Y", inputDevice[INPUT_BUTTONY].keyMappings = SDL_SCANCODE_S);
        ini.SetInteger("Keyboard 1", "Z", inputDevice[INPUT_BUTTONZ].keyMappings = SDL_SCANCODE_D);
        ini.SetInteger("Keyboard 1", "L", inputDevice[INPUT_BUTTONL].keyMappings = SDL_SCANCODE_Q);
        ini.SetInteger("Keyboard 1", "R", inputDevice[INPUT_BUTTONR].keyMappings = SDL_SCANCODE_E);
        ini.SetInteger("Keyboard 1", "Start", inputDevice[INPUT_START].keyMappings = SDL_SCANCODE_RETURN);
        ini.SetInteger("Keyboard 1", "Select", inputDevice[INPUT_SELECT].keyMappings = SDL_SCANCODE_TAB);

        ini.SetInteger("Controller 1", "Up", inputDevice[INPUT_UP].contMappings = SDL_CONTROLLER_BUTTON_DPAD_UP);
        ini.SetInteger("Controller 1", "Down", inputDevice[INPUT_DOWN].contMappings = SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        ini.SetInteger("Controller 1", "Left", inputDevice[INPUT_LEFT].contMappings = SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        ini.SetInteger("Controller 1", "Right", inputDevice[INPUT_RIGHT].contMappings = SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        ini.SetInteger("Controller 1", "A", inputDevice[INPUT_BUTTONA].contMappings = SDL_CONTROLLER_BUTTON_A);
        ini.SetInteger("Controller 1", "B", inputDevice[INPUT_BUTTONB].contMappings = SDL_CONTROLLER_BUTTON_B);
        ini.SetInteger("Controller 1", "C", inputDevice[INPUT_BUTTONC].contMappings = SDL_CONTROLLER_BUTTON_X);
        ini.SetInteger("Controller 1", "X", inputDevice[INPUT_BUTTONX].contMappings = SDL_CONTROLLER_BUTTON_Y);
        ini.SetInteger("Controller 1", "Y", inputDevice[INPUT_BUTTONY].contMappings = SDL_CONTROLLER_BUTTON_ZL);
        ini.SetInteger("Controller 1", "Z", inputDevice[INPUT_BUTTONZ].contMappings = SDL_CONTROLLER_BUTTON_ZR);
        ini.SetInteger("Controller 1", "L", inputDevice[INPUT_BUTTONL].contMappings = SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
        ini.SetInteger("Controller 1", "R", inputDevice[INPUT_BUTTONR].contMappings = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
        ini.SetInteger("Controller 1", "Start", inputDevice[INPUT_START].contMappings = SDL_CONTROLLER_BUTTON_START);
        ini.SetInteger("Controller 1", "Select", inputDevice[INPUT_SELECT].contMappings = SDL_CONTROLLER_BUTTON_GUIDE);

        ini.SetFloat("Controller 1", "LStickDeadzone", LSTICK_DEADZONE = 0.3);
        ini.SetFloat("Controller 1", "RStickDeadzone", RSTICK_DEADZONE = 0.3);
        ini.SetFloat("Controller 1", "LTriggerDeadzone", LTRIGGER_DEADZONE = 0.3);
        ini.SetFloat("Controller 1", "RTriggerDeadzone", RTRIGGER_DEADZONE = 0.3);
#endif

#if RETRO_USING_SDL1
        ini.SetInteger("Keyboard 1", "Up", inputDevice[INPUT_UP].keyMappings = SDLK_UP);
        ini.SetInteger("Keyboard 1", "Down", inputDevice[INPUT_DOWN].keyMappings = SDLK_DOWN);
        ini.SetInteger("Keyboard 1", "Left", inputDevice[INPUT_LEFT].keyMappings = SDLK_LEFT);
        ini.SetInteger("Keyboard 1", "Right", inputDevice[INPUT_RIGHT].keyMappings = SDLK_RIGHT);
        ini.SetInteger("Keyboard 1", "A", inputDevice[INPUT_BUTTONA].keyMappings = SDLK_z);
        ini.SetInteger("Keyboard 1", "B", inputDevice[INPUT_BUTTONB].keyMappings = SDLK_x);
        ini.SetInteger("Keyboard 1", "C", inputDevice[INPUT_BUTTONC].keyMappings = SDLK_c);
        ini.SetInteger("Keyboard 1", "X", inputDevice[INPUT_BUTTONX].keyMappings = SDLK_a);
        ini.SetInteger("Keyboard 1", "Y", inputDevice[INPUT_BUTTONY].keyMappings = SDLK_s);
        ini.SetInteger("Keyboard 1", "Z", inputDevice[INPUT_BUTTONZ].keyMappings = SDLK_d);
        ini.SetInteger("Keyboard 1", "L", inputDevice[INPUT_BUTTONL].keyMappings = SDLK_q);
        ini.SetInteger("Keyboard 1", "R", inputDevice[INPUT_BUTTONR].keyMappings = SDLK_e);
        ini.SetInteger("Keyboard 1", "Start", inputDevice[INPUT_START].keyMappings = SDLK_RETURN);
        ini.SetInteger("Keyboard 1", "Select", inputDevice[INPUT_SELECT].keyMappings = SDLK_TAB);

        ini.SetInteger("Controller 1", "Up", inputDevice[INPUT_UP].contMappings = 1);
        ini.SetInteger("Controller 1", "Down", inputDevice[INPUT_DOWN].contMappings = 2);
        ini.SetInteger("Controller 1", "Left", inputDevice[INPUT_LEFT].contMappings = 3);
        ini.SetInteger("Controller 1", "Right", inputDevice[INPUT_RIGHT].contMappings = 4);
        ini.SetInteger("Controller 1", "A", inputDevice[INPUT_BUTTONA].contMappings = 5);
        ini.SetInteger("Controller 1", "B", inputDevice[INPUT_BUTTONB].contMappings = 6);
        ini.SetInteger("Controller 1", "C", inputDevice[INPUT_BUTTONC].contMappings = 7);
        ini.SetInteger("Controller 1", "X", inputDevice[INPUT_BUTTONX].contMappings = 9);
        ini.SetInteger("Controller 1", "Y", inputDevice[INPUT_BUTTONY].contMappings = 10);
        ini.SetInteger("Controller 1", "Z", inputDevice[INPUT_BUTTONZ].contMappings = 11);
        ini.SetInteger("Controller 1", "L", inputDevice[INPUT_BUTTONL].contMappings = 12);
        ini.SetInteger("Controller 1", "R", inputDevice[INPUT_BUTTONR].contMappings = 13);
        ini.SetInteger("Controller 1", "Start", inputDevice[INPUT_START].contMappings = 8);
        ini.SetInteger("Controller 1", "Select", inputDevice[INPUT_SELECT].contMappings = 14);

        ini.SetFloat("Controller 1", "LStickDeadzone", LSTICK_DEADZONE = 0.3);
        ini.SetFloat("Controller 1", "RStickDeadzone", RSTICK_DEADZONE = 0.3);
        ini.SetFloat("Controller 1", "LTriggerDeadzone", LTRIGGER_DEADZONE = 0.3);
        ini.SetFloat("Controller 1", "RTriggerDeadzone", RTRIGGER_DEADZONE = 0.3);
#endif

        ini.Write(buffer);
    }
    else {
        fClose(file);
        IniParser ini(buffer, false);

        if (!ini.GetBool("Dev", "DevMenu", &Engine.devMenu))
            Engine.devMenu = false;
        if (!ini.GetBool("Dev", "EngineDebugMode", &engineDebugMode))
            engineDebugMode = false;
        if (!ini.GetBool("Dev", "TxtScripts", &forceUseScripts))
            forceUseScripts = false;
        forceUseScripts_Config = forceUseScripts;
        if (!ini.GetInteger("Dev", "StartingCategory", &Engine.startList))
            Engine.startList = 255;
        if (!ini.GetInteger("Dev", "StartingScene", &Engine.startStage))
            Engine.startStage = 255;
        if (!ini.GetInteger("Dev", "StartingPlayer", &Engine.startPlayer))
            Engine.startPlayer = 255;
        if (!ini.GetInteger("Dev", "StartingSaveFile", &Engine.startSave))
            Engine.startSave = 255;
        if (!ini.GetInteger("Dev", "FastForwardSpeed", &Engine.fastForwardSpeed))
            Engine.fastForwardSpeed = 8;
        if (!ini.GetBool("Dev", "UseHQModes", &Engine.useHQModes))
            Engine.useHQModes = true;

        Engine.startList_Game  = Engine.startList;
        Engine.startStage_Game = Engine.startStage;

        if (!ini.GetString("Dev", "DataFile", Engine.dataFile[0]))
            StrCopy(Engine.dataFile[0], "Data.rsdk");
        if (!StrComp(Engine.dataFile[1], "")) {
            if (!ini.GetString("Dev", "DataFile2", Engine.dataFile[1]))
                StrCopy(Engine.dataFile[1], "");
        }
        if (!StrComp(Engine.dataFile[2], "")) {
            if (!ini.GetString("Dev", "DataFile3", Engine.dataFile[2]))
                StrCopy(Engine.dataFile[2], "");
        }
        if (!StrComp(Engine.dataFile[3], "")) {
            if (!ini.GetString("Dev", "DataFile4", Engine.dataFile[3]))
                StrCopy(Engine.dataFile[3], "");
        }

        if (!ini.GetInteger("Game", "Language", &Engine.language))
            Engine.language = RETRO_EN;
        if (!ini.GetInteger("Game", "GameType", &Engine.gameTypeID))
            Engine.gameTypeID = 0;
        Engine.releaseType = Engine.gameTypeID ? "USE_ORIGINS" : "USE_STANDALONE";

        if (!ini.GetBool("Game", "SkipStartMenu", &skipStartMenu))
            skipStartMenu = false;
        skipStartMenu_Config = skipStartMenu;
        if (!ini.GetInteger("Game", "DisableFocusPause", &disableFocusPause))
            disableFocusPause = false;
        disableFocusPause_Config = disableFocusPause;

#if RETRO_USE_NETWORKING
        if (!ini.GetString("Network", "Host", networkHost))
            StrCopy(networkHost, "127.0.0.1");
        if (!ini.GetInteger("Network", "Port", &networkPort))
            networkPort = 50;
#endif

        if (!ini.GetBool("Window", "FullScreen", &Engine.startFullScreen))
            Engine.startFullScreen = DEFAULT_FULLSCREEN;
        if (!ini.GetBool("Window", "Borderless", &Engine.borderless))
            Engine.borderless = false;
        if (!ini.GetBool("Window", "VSync", &Engine.vsync))
            Engine.vsync = false;
        if (!ini.GetInteger("Window", "ScalingMode", &Engine.scalingMode))
            Engine.scalingMode = 0;
        if (!ini.GetInteger("Window", "WindowScale", &Engine.windowScale))
            Engine.windowScale = 2;
        if (!ini.GetInteger("Window", "ScreenWidth", &SCREEN_XSIZE_CONFIG))
            SCREEN_XSIZE_CONFIG = DEFAULT_SCREEN_XSIZE;
        SCREEN_XSIZE = SCREEN_XSIZE_CONFIG;
        if (!ini.GetInteger("Window", "RefreshRate", &Engine.refreshRate))
            Engine.refreshRate = 60;
        if (!ini.GetInteger("Window", "DimLimit", &Engine.dimLimit))
            Engine.dimLimit = 300; // 5 mins
        if (Engine.dimLimit >= 0)
            Engine.dimLimit *= Engine.refreshRate;

        float bv = 0, sv = 0;
        if (!ini.GetFloat("Audio", "BGMVolume", &bv))
            bv = 1.0f;
        if (!ini.GetFloat("Audio", "SFXVolume", &sv))
            sv = 1.0f;

        bgmVolume = bv * MAX_VOLUME;
        sfxVolume = sv * MAX_VOLUME;

        if (bgmVolume > MAX_VOLUME)
            bgmVolume = MAX_VOLUME;
        if (bgmVolume < 0)
            bgmVolume = 0;

        if (sfxVolume > MAX_VOLUME)
            sfxVolume = MAX_VOLUME;
        if (sfxVolume < 0)
            sfxVolume = 0;

#if RETRO_USING_SDL2
        if (!ini.GetInteger("Keyboard 1", "Up", &inputDevice[INPUT_UP].keyMappings))
            inputDevice[INPUT_UP].keyMappings = SDL_SCANCODE_UP;
        if (!ini.GetInteger("Keyboard 1", "Down", &inputDevice[INPUT_DOWN].keyMappings))
            inputDevice[INPUT_DOWN].keyMappings = SDL_SCANCODE_DOWN;
        if (!ini.GetInteger("Keyboard 1", "Left", &inputDevice[INPUT_LEFT].keyMappings))
            inputDevice[INPUT_LEFT].keyMappings = SDL_SCANCODE_LEFT;
        if (!ini.GetInteger("Keyboard 1", "Right", &inputDevice[INPUT_RIGHT].keyMappings))
            inputDevice[INPUT_RIGHT].keyMappings = SDL_SCANCODE_RIGHT;
        if (!ini.GetInteger("Keyboard 1", "A", &inputDevice[INPUT_BUTTONA].keyMappings))
            inputDevice[INPUT_BUTTONA].keyMappings = SDL_SCANCODE_Z;
        if (!ini.GetInteger("Keyboard 1", "B", &inputDevice[INPUT_BUTTONB].keyMappings))
            inputDevice[INPUT_BUTTONB].keyMappings = SDL_SCANCODE_X;
        if (!ini.GetInteger("Keyboard 1", "C", &inputDevice[INPUT_BUTTONC].keyMappings))
            inputDevice[INPUT_BUTTONC].keyMappings = SDL_SCANCODE_C;
        if (!ini.GetInteger("Keyboard 1", "X", &inputDevice[INPUT_BUTTONX].keyMappings))
            inputDevice[INPUT_BUTTONX].keyMappings = SDL_SCANCODE_A;
        if (!ini.GetInteger("Keyboard 1", "Y", &inputDevice[INPUT_BUTTONY].keyMappings))
            inputDevice[INPUT_BUTTONY].keyMappings = SDL_SCANCODE_S;
        if (!ini.GetInteger("Keyboard 1", "Z", &inputDevice[INPUT_BUTTONZ].keyMappings))
            inputDevice[INPUT_BUTTONZ].keyMappings = SDL_SCANCODE_D;
        if (!ini.GetInteger("Keyboard 1", "L", &inputDevice[INPUT_BUTTONL].keyMappings))
            inputDevice[INPUT_BUTTONL].keyMappings = SDL_SCANCODE_Q;
        if (!ini.GetInteger("Keyboard 1", "R", &inputDevice[INPUT_BUTTONR].keyMappings))
            inputDevice[INPUT_BUTTONR].keyMappings = SDL_SCANCODE_E;
        if (!ini.GetInteger("Keyboard 1", "Start", &inputDevice[INPUT_START].keyMappings))
            inputDevice[INPUT_START].keyMappings = SDL_SCANCODE_RETURN;
        if (!ini.GetInteger("Keyboard 1", "Select", &inputDevice[INPUT_SELECT].keyMappings))
            inputDevice[INPUT_SELECT].keyMappings = SDL_SCANCODE_TAB;

        if (!ini.GetInteger("Controller 1", "Up", &inputDevice[INPUT_UP].contMappings))
            inputDevice[INPUT_UP].contMappings = SDL_CONTROLLER_BUTTON_DPAD_UP;
        if (!ini.GetInteger("Controller 1", "Down", &inputDevice[INPUT_DOWN].contMappings))
            inputDevice[INPUT_DOWN].contMappings = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
        if (!ini.GetInteger("Controller 1", "Left", &inputDevice[INPUT_LEFT].contMappings))
            inputDevice[INPUT_LEFT].contMappings = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
        if (!ini.GetInteger("Controller 1", "Right", &inputDevice[INPUT_RIGHT].contMappings))
            inputDevice[INPUT_RIGHT].contMappings = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
        if (!ini.GetInteger("Controller 1", "A", &inputDevice[INPUT_BUTTONA].contMappings))
            inputDevice[INPUT_BUTTONA].contMappings = SDL_CONTROLLER_BUTTON_A;
        if (!ini.GetInteger("Controller 1", "B", &inputDevice[INPUT_BUTTONB].contMappings))
            inputDevice[INPUT_BUTTONB].contMappings = SDL_CONTROLLER_BUTTON_B;
        if (!ini.GetInteger("Controller 1", "C", &inputDevice[INPUT_BUTTONC].contMappings))
            inputDevice[INPUT_BUTTONC].contMappings = SDL_CONTROLLER_BUTTON_X;
        if (!ini.GetInteger("Controller 1", "X", &inputDevice[INPUT_BUTTONX].contMappings))
            inputDevice[INPUT_BUTTONX].contMappings = SDL_CONTROLLER_BUTTON_Y;
        if (!ini.GetInteger("Controller 1", "Y", &inputDevice[INPUT_BUTTONY].contMappings))
            inputDevice[INPUT_BUTTONY].contMappings = SDL_CONTROLLER_BUTTON_ZL;
        if (!ini.GetInteger("Controller 1", "Z", &inputDevice[INPUT_BUTTONZ].contMappings))
            inputDevice[INPUT_BUTTONZ].contMappings = SDL_CONTROLLER_BUTTON_ZR;
        if (!ini.GetInteger("Controller 1", "L", &inputDevice[INPUT_BUTTONL].contMappings))
            inputDevice[INPUT_BUTTONL].contMappings = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
        if (!ini.GetInteger("Controller 1", "R", &inputDevice[INPUT_BUTTONR].contMappings))
            inputDevice[INPUT_BUTTONR].contMappings = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
        if (!ini.GetInteger("Controller 1", "Start", &inputDevice[INPUT_START].contMappings))
            inputDevice[INPUT_START].contMappings = SDL_CONTROLLER_BUTTON_START;
        if (!ini.GetInteger("Controller 1", "Select", &inputDevice[INPUT_SELECT].contMappings))
            inputDevice[INPUT_SELECT].contMappings = SDL_CONTROLLER_BUTTON_GUIDE;

        if (!ini.GetFloat("Controller 1", "LStickDeadzone", &LSTICK_DEADZONE))
            LSTICK_DEADZONE = 0.3;
        if (!ini.GetFloat("Controller 1", "RStickDeadzone", &RSTICK_DEADZONE))
            RSTICK_DEADZONE = 0.3;
        if (!ini.GetFloat("Controller 1", "LTriggerDeadzone", &LTRIGGER_DEADZONE))
            LTRIGGER_DEADZONE = 0.3;
        if (!ini.GetFloat("Controller 1", "RTriggerDeadzone", &RTRIGGER_DEADZONE))
            RTRIGGER_DEADZONE = 0.3;
#endif

#if RETRO_USING_SDL1
        if (!ini.GetInteger("Keyboard 1", "Up", &inputDevice[INPUT_UP].keyMappings))
            inputDevice[INPUT_UP].keyMappings = SDLK_UP;
        if (!ini.GetInteger("Keyboard 1", "Down", &inputDevice[INPUT_DOWN].keyMappings))
            inputDevice[INPUT_DOWN].keyMappings = SDLK_DOWN;
        if (!ini.GetInteger("Keyboard 1", "Left", &inputDevice[INPUT_LEFT].keyMappings))
            inputDevice[INPUT_LEFT].keyMappings = SDLK_LEFT;
        if (!ini.GetInteger("Keyboard 1", "Right", &inputDevice[INPUT_RIGHT].keyMappings))
            inputDevice[INPUT_RIGHT].keyMappings = SDLK_RIGHT;
        if (!ini.GetInteger("Keyboard 1", "A", &inputDevice[INPUT_BUTTONA].keyMappings))
            inputDevice[INPUT_BUTTONA].keyMappings = SDLK_z;
        if (!ini.GetInteger("Keyboard 1", "B", &inputDevice[INPUT_BUTTONB].keyMappings))
            inputDevice[INPUT_BUTTONB].keyMappings = SDLK_x;
        if (!ini.GetInteger("Keyboard 1", "C", &inputDevice[INPUT_BUTTONC].keyMappings))
            inputDevice[INPUT_BUTTONC].keyMappings = SDLK_c;
        if (!ini.GetInteger("Controller 1", "X", &inputDevice[INPUT_BUTTONX].contMappings))
            inputDevice[INPUT_BUTTONX].contMappings = SDLK_a;
        if (!ini.GetInteger("Controller 1", "Y", &inputDevice[INPUT_BUTTONY].contMappings))
            inputDevice[INPUT_BUTTONY].contMappings = SDLK_s;
        if (!ini.GetInteger("Controller 1", "Z", &inputDevice[INPUT_BUTTONZ].contMappings))
            inputDevice[INPUT_BUTTONZ].contMappings = SDLK_d;
        if (!ini.GetInteger("Controller 1", "L", &inputDevice[INPUT_BUTTONL].contMappings))
            inputDevice[INPUT_BUTTONL].contMappings = SDLK_q;
        if (!ini.GetInteger("Controller 1", "R", &inputDevice[INPUT_BUTTONR].contMappings))
            inputDevice[INPUT_BUTTONR].contMappings = SDLK_e;
        if (!ini.GetInteger("Keyboard 1", "Start", &inputDevice[INPUT_START].keyMappings))
            inputDevice[INPUT_START].keyMappings = SDLK_RETURN;
        if (!ini.GetInteger("Keyboard 1", "Select", &inputDevice[INPUT_SELECT].keyMappings))
            inputDevice[INPUT_SELECT].keyMappings = SDLK_TAB;

        if (!ini.GetInteger("Controller 1", "Up", &inputDevice[INPUT_UP].contMappings))
            inputDevice[INPUT_UP].contMappings = 1;
        if (!ini.GetInteger("Controller 1", "Down", &inputDevice[INPUT_DOWN].contMappings))
            inputDevice[INPUT_DOWN].contMappings = 2;
        if (!ini.GetInteger("Controller 1", "Left", &inputDevice[INPUT_LEFT].contMappings))
            inputDevice[INPUT_LEFT].contMappings = 3;
        if (!ini.GetInteger("Controller 1", "Right", &inputDevice[INPUT_RIGHT].contMappings))
            inputDevice[INPUT_RIGHT].contMappings = 4;
        if (!ini.GetInteger("Controller 1", "A", &inputDevice[INPUT_BUTTONA].contMappings))
            inputDevice[INPUT_BUTTONA].contMappings = 5;
        if (!ini.GetInteger("Controller 1", "B", &inputDevice[INPUT_BUTTONB].contMappings))
            inputDevice[INPUT_BUTTONB].contMappings = 6;
        if (!ini.GetInteger("Controller 1", "C", &inputDevice[INPUT_BUTTONC].contMappings))
            inputDevice[INPUT_BUTTONC].contMappings = 7;
        if (!ini.GetInteger("Controller 1", "X", &inputDevice[INPUT_BUTTONX].contMappings))
            inputDevice[INPUT_BUTTONX].contMappings = 8;
        if (!ini.GetInteger("Controller 1", "Y", &inputDevice[INPUT_BUTTONY].contMappings))
            inputDevice[INPUT_BUTTONY].contMappings = 9;
        if (!ini.GetInteger("Controller 1", "Z", &inputDevice[INPUT_BUTTONZ].contMappings))
            inputDevice[INPUT_BUTTONZ].contMappings = 10;
        if (!ini.GetInteger("Controller 1", "L", &inputDevice[INPUT_BUTTONL].contMappings))
            inputDevice[INPUT_BUTTONL].contMappings = 11;
        if (!ini.GetInteger("Controller 1", "R", &inputDevice[INPUT_BUTTONR].contMappings))
            inputDevice[INPUT_BUTTONR].contMappings = 12;
        if (!ini.GetInteger("Controller 1", "Start", &inputDevice[INPUT_START].contMappings))
            inputDevice[INPUT_START].contMappings = 13;
        if (!ini.GetInteger("Controller 1", "Select", &inputDevice[INPUT_SELECT].contMappings))
            inputDevice[INPUT_SELECT].contMappings = 14;

        if (!ini.GetFloat("Controller 1", "LStickDeadzone", &LSTICK_DEADZONE))
            LSTICK_DEADZONE = 0.3;
        if (!ini.GetFloat("Controller 1", "RStickDeadzone", &RSTICK_DEADZONE))
            RSTICK_DEADZONE = 0.3;
        if (!ini.GetFloat("Controller 1", "LTriggerDeadzone", &LTRIGGER_DEADZONE))
            LTRIGGER_DEADZONE = 0.3;
        if (!ini.GetFloat("Controller 1", "RTriggerDeadzone", &RTRIGGER_DEADZONE))
            RTRIGGER_DEADZONE = 0.3;
#endif
    }

#if RETRO_USING_SDL2
    // Support for extra controller types SDL doesn't recognise
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/controllerdb.txt", getResourcesPath());
    else
        sprintf(buffer, "%scontrollerdb.txt", gamePath);
#elif RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_ANDROID
    sprintf(buffer, "%s/controllerdb.txt", gamePath);
#else
    sprintf(buffer, BASE_PATH "controllerdb.txt");
#endif
    file = fOpen(buffer, "rb");
    if (file) {
        fClose(file);

        int nummaps = SDL_GameControllerAddMappingsFromFile(buffer);
        if (nummaps >= 0)
            PrintLog("loaded %d controller mappings from '%s'\n", buffer, nummaps);
    }
#endif

#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/UData.bin", getResourcesPath());
    else
        sprintf(buffer, "%sUData.bin", gamePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/UData.bin", gamePath);
#else
    sprintf(buffer, "%sUData.bin", gamePath);
#endif
    file = fOpen(buffer, "rb");
    if (file) {
        fClose(file);
        ReadUserdata();
    }
    else {
        WriteUserdata();
    }
}

void WriteSettings()
{
    IniParser ini;

    ini.SetComment("Dev", "DevMenuComment", "Enable this flag to activate dev menu via the ESC key");
    ini.SetBool("Dev", "DevMenu", Engine.devMenu);
    ini.SetComment("Dev", "DebugModeComment",
                   "Enable this flag to activate features used for debugging the engine (may result in slightly slower game speed)");
    ini.SetBool("Dev", "EngineDebugMode", engineDebugMode);
    ini.SetComment("Dev", "ScriptsComment", "Enable this flag to force the engine to load from the scripts folder instead of from bytecode");
    ini.SetBool("Dev", "TxtScripts", forceUseScripts_Config);
    ini.SetComment("Dev", "SCComment", "Sets the starting category ID");
    ini.SetInteger("Dev", "StartingCategory", Engine.startList);
    ini.SetComment("Dev", "SSComment", "Sets the starting scene ID");
    ini.SetInteger("Dev", "StartingScene", Engine.startStage);
    ini.SetComment("Dev", "SPComment", "Sets the starting player ID");
    ini.SetInteger("Dev", "StartingPlayer", Engine.startPlayer);
    ini.SetComment("Dev", "SSaveComment", "Sets the starting save file ID");
    ini.SetInteger("Dev", "StartingSaveFile", Engine.startSave);
    ini.SetComment("Dev", "FFComment", "Determines how fast the game will be when fastforwarding is active");
    ini.SetInteger("Dev", "FastForwardSpeed", Engine.fastForwardSpeed);
    ini.SetComment(
        "Dev", "UseHQComment",
        "Determines if applicable rendering modes (such as 3D floor from special stages) will render in \"High Quality\" mode or standard mode");
    ini.SetBool("Dev", "UseHQModes", Engine.useHQModes);

    ini.SetComment("Dev", "DataFileComment", "Determines where the first RSDK file will be loaded from");
    ini.SetString("Dev", "DataFile", Engine.dataFile[0]);
    if (!StrComp(Engine.dataFile[1], "")) {
        ini.SetComment("Dev", "DataFileComment2", "Determines where the second RSDK file will be loaded from");
        ini.SetString("Dev", "DataFile2", Engine.dataFile[1]);
    }
    if (!StrComp(Engine.dataFile[2], "")) {
        ini.SetComment("Dev", "DataFileComment3", "Determines where the third RSDK file will be loaded from (normally unused)");
        ini.SetString("Dev", "DataFile3", Engine.dataFile[2]);
    }
    if (!StrComp(Engine.dataFile[3], "")) {
        ini.SetComment("Dev", "DataFileComment4", "Determines where the fourth RSDK file will be loaded from (normally unused)");
        ini.SetString("Dev", "DataFile4", Engine.dataFile[3]);
    }

    ini.SetComment("Game", "LangComment",
                   "Sets the game language (0 = EN, 1 = FR, 2 = IT, 3 = DE, 4 = ES, 5 = JP, 6 = PT, 7 = RU, 8 = KO, 9 = ZH, 10 = ZS)");
    ini.SetInteger("Game", "Language", Engine.language);
    ini.SetComment("Game", "GameTypeComment", "Determines game type in scripts (0 = Standalone/Original releases, 1 = Origins release)");
    ini.SetInteger("Game", "GameType", Engine.gameTypeID);
    ini.SetComment("Game", "SSMenuComment", "If set to true, disables the start menu");
    ini.SetBool("Game", "SkipStartMenu", skipStartMenu_Config);
    ini.SetComment("Game", "DFPMenuComment",
                   "Handles pausing behaviour when focus is lost\n; 0 = Game focus enabled, engine focus enabled\n; 1 = Game focus disabled, "
                   "engine focus enabled\n; 2 = Game focus enabled, engine focus disabled\n; 3 = Game focus disabled, engine focus disabled");
    ini.SetInteger("Game", "DisableFocusPause", disableFocusPause_Config);

#if RETRO_USE_NETWORKING
    ini.SetComment("Network", "HostComment", "The host (IP address or \"URL\") that the game will try to connect to.");
    ini.SetString("Network", "Host", networkHost);
    ini.SetComment("Network", "PortComment", "The port the game will try to connect to.");
    ini.SetInteger("Network", "Port", networkPort);
#endif

    ini.SetComment("Window", "FSComment", "Determines if the window will be fullscreen or not");
    ini.SetBool("Window", "FullScreen", Engine.startFullScreen);
    ini.SetComment("Window", "BLComment", "Determines if the window will be borderless or not");
    ini.SetBool("Window", "Borderless", Engine.borderless);
    ini.SetComment("Window", "VSComment",
                   "Determines if VSync will be active or not (not recommended as the engine is built around running at 60 FPS)");
    ini.SetBool("Window", "VSync", Engine.vsync);
    ini.SetComment("Window", "SMComment", "Determines what scaling is used. 0 is nearest neighbour, 1 is linear.");
    ini.SetInteger("Window", "ScalingMode", Engine.scalingMode);
    ini.SetComment("Window", "WSComment", "How big the window will be");
    ini.SetInteger("Window", "WindowScale", Engine.windowScale);
    ini.SetComment("Window", "SWComment", "How wide the base screen will be in pixels");
    ini.SetInteger("Window", "ScreenWidth", SCREEN_XSIZE_CONFIG);
    ini.SetComment("Window", "RRComment", "Determines the target FPS");
    ini.SetInteger("Window", "RefreshRate", Engine.refreshRate);
    ini.SetComment("Window", "DLComment", "Determines the dim timer in seconds, set to -1 to disable dimming");
    ini.SetInteger("Window", "DimLimit", Engine.dimLimit >= 0 ? Engine.dimLimit / Engine.refreshRate : -1);

    ini.SetFloat("Audio", "BGMVolume", bgmVolume / (float)MAX_VOLUME);
    ini.SetFloat("Audio", "SFXVolume", sfxVolume / (float)MAX_VOLUME);

#if RETRO_USING_SDL2
    ini.SetComment("Keyboard 1", "IK1Comment",
                   "Keyboard Mappings for P1 (Based on: https://github.com/libsdl-org/sdlwiki/blob/main/SDL2/SDLScancodeLookup.mediawiki)");
#endif
#if RETRO_USING_SDL1
    ini.SetComment("Keyboard 1", "IK1Comment", "Keyboard Mappings for P1 (Based on: https://wiki.libsdl.org/SDLKeycodeLookup)");
#endif
    ini.SetInteger("Keyboard 1", "Up", inputDevice[INPUT_UP].keyMappings);
    ini.SetInteger("Keyboard 1", "Down", inputDevice[INPUT_DOWN].keyMappings);
    ini.SetInteger("Keyboard 1", "Left", inputDevice[INPUT_LEFT].keyMappings);
    ini.SetInteger("Keyboard 1", "Right", inputDevice[INPUT_RIGHT].keyMappings);
    ini.SetInteger("Keyboard 1", "A", inputDevice[INPUT_BUTTONA].keyMappings);
    ini.SetInteger("Keyboard 1", "B", inputDevice[INPUT_BUTTONB].keyMappings);
    ini.SetInteger("Keyboard 1", "C", inputDevice[INPUT_BUTTONC].keyMappings);
    ini.SetInteger("Keyboard 1", "X", inputDevice[INPUT_BUTTONX].keyMappings);
    ini.SetInteger("Keyboard 1", "Y", inputDevice[INPUT_BUTTONY].keyMappings);
    ini.SetInteger("Keyboard 1", "Z", inputDevice[INPUT_BUTTONZ].keyMappings);
    ini.SetInteger("Keyboard 1", "L", inputDevice[INPUT_BUTTONL].keyMappings);
    ini.SetInteger("Keyboard 1", "R", inputDevice[INPUT_BUTTONR].keyMappings);
    ini.SetInteger("Keyboard 1", "Start", inputDevice[INPUT_START].keyMappings);
    ini.SetInteger("Keyboard 1", "Select", inputDevice[INPUT_SELECT].keyMappings);

#if RETRO_USING_SDL2
    ini.SetComment("Controller 1", "IC1Comment",
                   "Controller Mappings for P1 (Based on: https://github.com/libsdl-org/sdlwiki/blob/main/SDL2/SDL_GameControllerButton.mediawiki)");
    ini.SetComment("Controller 1", "IC1Comment2", "Extra buttons can be mapped with the following IDs:");
    ini.SetComment("Controller 1", "IC1Comment3", "CONTROLLER_BUTTON_ZL             = 16");
    ini.SetComment("Controller 1", "IC1Comment4", "CONTROLLER_BUTTON_ZR             = 17");
    ini.SetComment("Controller 1", "IC1Comment5", "CONTROLLER_BUTTON_LSTICK_UP      = 18");
    ini.SetComment("Controller 1", "IC1Comment6", "CONTROLLER_BUTTON_LSTICK_DOWN    = 19");
    ini.SetComment("Controller 1", "IC1Comment7", "CONTROLLER_BUTTON_LSTICK_LEFT    = 20");
    ini.SetComment("Controller 1", "IC1Comment8", "CONTROLLER_BUTTON_LSTICK_RIGHT   = 21");
    ini.SetComment("Controller 1", "IC1Comment9", "CONTROLLER_BUTTON_RSTICK_UP      = 22");
    ini.SetComment("Controller 1", "IC1Comment10", "CONTROLLER_BUTTON_RSTICK_DOWN    = 23");
    ini.SetComment("Controller 1", "IC1Comment11", "CONTROLLER_BUTTON_RSTICK_LEFT    = 24");
    ini.SetComment("Controller 1", "IC1Comment12", "CONTROLLER_BUTTON_RSTICK_RIGHT   = 25");
#endif
    ini.SetInteger("Controller 1", "Up", inputDevice[INPUT_UP].contMappings);
    ini.SetInteger("Controller 1", "Down", inputDevice[INPUT_DOWN].contMappings);
    ini.SetInteger("Controller 1", "Left", inputDevice[INPUT_LEFT].contMappings);
    ini.SetInteger("Controller 1", "Right", inputDevice[INPUT_RIGHT].contMappings);
    ini.SetInteger("Controller 1", "A", inputDevice[INPUT_BUTTONA].contMappings);
    ini.SetInteger("Controller 1", "B", inputDevice[INPUT_BUTTONB].contMappings);
    ini.SetInteger("Controller 1", "C", inputDevice[INPUT_BUTTONC].contMappings);
    ini.SetInteger("Controller 1", "X", inputDevice[INPUT_BUTTONX].contMappings);
    ini.SetInteger("Controller 1", "Y", inputDevice[INPUT_BUTTONY].contMappings);
    ini.SetInteger("Controller 1", "Z", inputDevice[INPUT_BUTTONZ].contMappings);
    ini.SetInteger("Controller 1", "L", inputDevice[INPUT_BUTTONL].contMappings);
    ini.SetInteger("Controller 1", "R", inputDevice[INPUT_BUTTONR].contMappings);
    ini.SetInteger("Controller 1", "Start", inputDevice[INPUT_START].contMappings);
    ini.SetInteger("Controller 1", "Select", inputDevice[INPUT_SELECT].contMappings);

    ini.SetComment("Controller 1", "DeadZoneComment", "Deadzones, 0.0-1.0");
    ini.SetFloat("Controller 1", "LStickDeadzone", LSTICK_DEADZONE);
    ini.SetFloat("Controller 1", "RStickDeadzone", RSTICK_DEADZONE);
    ini.SetFloat("Controller 1", "LTriggerDeadzone", LTRIGGER_DEADZONE);
    ini.SetFloat("Controller 1", "RTriggerDeadzone", RTRIGGER_DEADZONE);

    char buffer[0x100];

#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/settings.ini", getResourcesPath());
    else
        sprintf(buffer, "%ssettings.ini", gamePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/settings.ini", gamePath);
#else
    sprintf(buffer, "%ssettings.ini", gamePath);
#endif

    ini.Write(buffer, false);
}

void ReadUserdata()
{
    char buffer[0x100];
#if RETRO_USE_MOD_LOADER
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/%sUData.bin", redirectSave ? modsPath : getResourcesPath(), savePath);
    else
        sprintf(buffer, "%s%sUData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/%sUData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/%sUData.bin", redirectSave ? modsPath : getDocumentsPath(), savePath);
#else
    sprintf(buffer, "%s%sUData.bin", redirectSave ? modsPath : gamePath, savePath);
#endif
#else
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/%sUData.bin", getResourcesPath(), savePath);
    else
        sprintf(buffer, "%s%sUData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/%sUData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/%sUData.bin", getDocumentsPath(), savePath);
#else
    sprintf(buffer, "%s%sUData.bin", gamePath, savePath);
#endif
#endif
    FileIO *userFile = fOpen(buffer, "rb");
    if (!userFile)
        return;

    int buf = 0;
    for (int a = 0; a < ACHIEVEMENT_COUNT; ++a) {
        fRead(&buf, 4, 1, userFile);
        achievements[a].status = buf;
    }
    for (int l = 0; l < LEADERBOARD_COUNT; ++l) {
        fRead(&buf, 4, 1, userFile);
        leaderboards[l].score = buf;
        if (!leaderboards[l].score)
            leaderboards[l].score = 0x7FFFFFF;
    }

    fClose(userFile);

    if (Engine.onlineActive) {
        // Load from online
    }
}

void WriteUserdata()
{
    char buffer[0x100];
#if RETRO_USE_MOD_LOADER
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/%sUData.bin", redirectSave ? modsPath : getResourcesPath(), savePath);
    else
        sprintf(buffer, "%s%sUData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/%sUData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/%sUData.bin", redirectSave ? modsPath : getDocumentsPath(), savePath);
#else
    sprintf(buffer, "%s%sUData.bin", redirectSave ? modsPath : gamePath, savePath);
#endif
#else
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/%sUData.bin", getResourcesPath(), savePath);
    else
        sprintf(buffer, "%s%sUData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/%sUData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/%sUData.bin", getDocumentsPath(), savePath);
#else
    sprintf(buffer, "%s%sUData.bin", gamePath, savePath);
#endif
#endif

    FileIO *userFile = fOpen(buffer, "wb");
    if (!userFile)
        return;

    for (int a = 0; a < ACHIEVEMENT_COUNT; ++a) fWrite(&achievements[a].status, 4, 1, userFile);
    for (int l = 0; l < LEADERBOARD_COUNT; ++l) fWrite(&leaderboards[l].score, 4, 1, userFile);

    fClose(userFile);

    if (Engine.onlineActive) {
        // Load from online
    }
}
#endif

void AwardAchievement(int id, int status)
{
    if (id < 0 || id >= ACHIEVEMENT_COUNT)
        return;

    if (status == 100 && status != achievements[id].status)
        PrintLog("Achieved achievement: %s (%d)!", achievements[id].name, status);

    achievements[id].status = status;

    if (Engine.onlineActive) {
        // Set Achievement online
    }
#if !RETRO_USE_ORIGINAL_CODE
    WriteUserdata();
#endif
}

void SetAchievement(int *achievementID, int *status)
{
    if (!Engine.trialMode && !debugMode) {
        AwardAchievement(*achievementID, *status);
    }
}
#if RETRO_USE_MOD_LOADER
void AddGameAchievement(int *unused, const char *name) { StrCopy(achievements[achievementCount++].name, name); }
void SetAchievementDescription(uint *id, const char *desc)
{
    if (*id >= achievementCount)
        return;

    StrCopy(achievements[*id].desc, desc);
}
void ClearAchievements() { achievementCount = 0; }
void GetAchievementCount() { scriptEng.checkResult = achievementCount; }
void GetAchievementName(uint *id, int *textMenu)
{
    if (*id >= achievementCount)
        return;

    TextMenu *menu                       = &gameMenu[*textMenu];
    menu->entryHighlight[menu->rowCount] = false;
    AddTextMenuEntry(menu, achievements[*id].name);
}
void GetAchievementDescription(uint *id, int *textMenu)
{
    if (*id >= achievementCount)
        return;

    TextMenu *menu                       = &gameMenu[*textMenu];
    menu->entryHighlight[menu->rowCount] = false;
    AddTextMenuEntry(menu, achievements[*id].desc);
}
void GetAchievement(uint *id, void *unused)
{
    if (*id >= achievementCount)
        return;
    scriptEng.checkResult = achievements[*id].status;
}
#endif
void ShowAchievementsScreen()
{
#if !RETRO_USE_ORIGINAL_CODE
    CREATE_ENTITY(AchievementsMenu);
#endif
}

int SetLeaderboard(int *leaderboardID, int *score)
{
    if (!Engine.trialMode && !debugMode) {
        // 0  = GHZ1/EHZ1
        // 1  = GHZ2/EHZ1
        // 2  = GHZ3/CPZ1
        // 3  = MZ1/CPZ1
        // 4  = MZ2/ARZ1
        // 5  = MZ3/ARZ1
        // 6  = SYZ1/CNZ1
        // 7  = SYZ2/CNZ1
        // 8  = SYZ3/HTZ1
        // 9  = LZ1/HTZ1
        // 10 = LZ2/MCZ1
        // 11 = LZ3/MCZ1
        // 12 = SLZ1/OOZ1
        // 13 = SLZ2/OOZ1
        // 14 = SLZ3/MPZ1
        // 15 = SBZ1/MPZ2
        // 15 = SBZ2/MPZ3
        // 16 = SBZ3/SCZ
        // 17 = ???/WFZ
        // 18 = ???/DEZ
        // 19 = TotalScore (S1)/???
        // 20 = ???
        // 21 = HPZ
        // 22 = TotalScore (S2)
#if !RETRO_USE_ORIGINAL_CODE
        if (*score < leaderboards[*leaderboardID].score) {
            PrintLog("Set leaderboard (%d) value to %d", *leaderboardID, score);
            leaderboards[*leaderboardID].score = *score;
            WriteUserdata();
        }
        else {
            PrintLog("Attempted to set leaderboard (%d) value to %d... but score was already %d!", *leaderboardID, *score,
                     leaderboards[*leaderboardID].score);
        }
#endif
        return 1;
    }
    return 0;
}
void ShowLeaderboardsScreen()
{
    /*TODO*/
    PrintLog("we're showing the leaderboards screen");
}

bool disableFocusPause_Store = false;
void Connect2PVS(int *gameLength, int *itemMode)
{
    PrintLog("Attempting to connect to 2P game (%d) (%d)", *gameLength, *itemMode);

    multiplayerDataIN.type = 0;
    matchValueData[0]      = 0;
    matchValueData[1]      = 0;
    matchValueReadPos      = 0;
    matchValueWritePos     = 0;
#if RETRO_USE_NETWORKING
    Engine.gameMode = ENGINE_CONNECT2PVS;
#endif
    // PauseSound();
    // actual connection code
    vsGameLength = *gameLength;
    vsItemMode   = *itemMode;
    if (Engine.onlineActive) {
#if RETRO_USE_NETWORKING
        disableFocusPause_Store = disableFocusPause;
        disableFocusPause       = 3;
        RunNetwork();
#endif
    }
}
void Disconnect2PVS()
{
    PrintLog("Attempting to disconnect from 2P game");

    if (Engine.onlineActive) {
#if RETRO_USE_NETWORKING
        disableFocusPause = disableFocusPause_Store;
        // Engine.devMenu    = vsPlayerID;
        vsPlaying = false;
        DisconnectNetwork();
        InitNetwork();
#endif
    }
}
void SendEntity(int *entityID, int *verify)
{
    if (!sendCounter) {
        multiplayerDataOUT.type = 1;
        memcpy(multiplayerDataOUT.data, &objectEntityList[*entityID], sizeof(Entity));
        if (Engine.onlineActive) {
#if RETRO_USE_NETWORKING
            SendData(*verify);
#endif
        }
    }
    sendCounter = (sendCounter + 1) % 2;
}
void SendValue(int *value, int *verify)
{
    // PrintLog("Attempting to send value (%d) (%d)", *dataSlot, *value);

    multiplayerDataOUT.type    = 0;
    multiplayerDataOUT.data[0] = *value;
    if (Engine.onlineActive) {
#if RETRO_USE_NETWORKING
        SendData(*verify);
#endif
    }
}
bool receiveReady = false;
void ReceiveEntity(int *entityID, int *incrementPos)
{
    // PrintLog("Attempting to receive entity (%d) (%d)", *clearOnReceive, *entityID);

    if (Engine.onlineActive && receiveReady) {
        // receiveReady = false;
        if (*incrementPos == 1) {
            if (multiplayerDataIN.type == 1) {
                memcpy(&objectEntityList[*entityID], multiplayerDataIN.data, sizeof(Entity));
            }
            multiplayerDataIN.type = 0;
        }
        else {
            memcpy(&objectEntityList[*entityID], multiplayerDataIN.data, sizeof(Entity));
        }
    }
}
void ReceiveValue(int *value, int *incrementPos)
{
    // PrintLog("Attempting to receive value (%d) (%d)", *incrementPos, *value);

    if (Engine.onlineActive && receiveReady) {
        // receiveReady = false;
        if (*incrementPos == 1) {
            if (matchValueReadPos != matchValueWritePos) {
                *value = matchValueData[matchValueReadPos];
                matchValueReadPos++;
            }
        }
        else {
            *value = matchValueData[matchValueReadPos];
        }
    }
}
void TransmitGlobal(int *globalValue, const char *globalName)
{
    PrintLog("Attempting to transmit global (%s) (%d)", globalName, *globalValue);

    multiplayerDataOUT.type    = 2;
    multiplayerDataOUT.data[0] = GetGlobalVariableID(globalName);
    multiplayerDataOUT.data[1] = *globalValue;
    if (Engine.onlineActive) {
#if RETRO_USE_NETWORKING
        SendData();
#endif
    }
}

void Receive2PVSData(MultiplayerData *data)
{
    receiveReady = true;
    switch (data->type) {
        case 0: matchValueData[matchValueWritePos++] = data->data[0]; break;
        case 1:
            multiplayerDataIN.type = 1;
            memcpy(multiplayerDataIN.data, data->data, sizeof(Entity));
            break;
        case 2: globalVariables[data->data[0]] = data->data[1]; break;
    }
}

void Receive2PVSMatchCode(int code)
{
    receiveReady = true;
    code &= 0x00000FF0;
    code |= 0x00001000 * vsPlayerID;
    matchValueData[matchValueWritePos++] = code;
    ResumeSound();
    vsPlayerID = Engine.devMenu;
    // Engine.devMenu  = false;
    Engine.gameMode = ENGINE_MAINGAME;
    vsPlaying       = true;
    ClearNativeObjects();
    CREATE_ENTITY(RetroGameLoop); // hack
    if (Engine.gameDeviceType == RETRO_MOBILE)
        CREATE_ENTITY(VirtualDPad);
#if RETRO_USE_NETWORKING
    CREATE_ENTITY(MultiplayerHandler);
#endif
}

void ShowPromoPopup(int *id, const char *popupName) { PrintLog("Attempting to show promo popup: \"%s\" (%d)", popupName, id ? *id : 0); }
void ShowSegaIDPopup()
{
    // nothing here, its just all to a java method of the same name
}
void ShowOnlineSignIn()
{
    // nothing here, its just all to a java method of the same name
}
void ShowWebsite(int websiteID)
{
    switch (websiteID) {
        default: PrintLog("Showing unknown website: (%d)", websiteID); break;
        case 0: PrintLog("Showing website: \"%s\" (%d)", "http://www.sega.com/mprivacy", websiteID); break;
        case 1: PrintLog("Showing website: \"%s\" (%d)", "http://www.sega.com/legal", websiteID); break;
    }
}

// In the Sega Forever versions of S1 & S2, there's a feature where you can choose to watch an ad to continue from a Game Over
// We obviously can't do that here, so let's just take the L
void NativePlayerWaitingAds() { SetGlobalVariableByName("waitingAds.result", 2); }
void NativeWaterPlayerWaitingAds() { SetGlobalVariableByName("waitingAds.water", 2); }

#if RETRO_REV03
enum NotifyCallbackIDs {
    NOTIFY_DEATH_EVENT        = 128,
    NOTIFY_TOUCH_SIGNPOST     = 129,
    NOTIFY_HUD_ENABLE         = 130,
    NOTIFY_ADD_COIN           = 131,
    NOTIFY_KILL_ENEMY         = 132,
    NOTIFY_SAVESLOT_SELECT    = 133,
    NOTIFY_FUTURE_PAST        = 134,
    NOTIFY_GOTO_FUTURE_PAST   = 135,
    NOTIFY_BOSS_END           = 136,
    NOTIFY_SPECIAL_END        = 137,
    NOTIFY_DEBUGPRINT         = 138,
    NOTIFY_KILL_BOSS          = 139,
    NOTIFY_TOUCH_EMERALD      = 140,
    NOTIFY_STATS_ENEMY        = 141,
    NOTIFY_STATS_CHARA_ACTION = 142,
    NOTIFY_STATS_RING         = 143,
    NOTIFY_STATS_MOVIE        = 144,
    NOTIFY_STATS_PARAM_1      = 145,
    NOTIFY_STATS_PARAM_2      = 146,
    NOTIFY_CHARACTER_SELECT   = 147,
    NOTIFY_SPECIAL_RETRY      = 148,
    NOTIFY_TOUCH_CHECKPOINT   = 149,
    NOTIFY_ACT_FINISH         = 150,
    NOTIFY_1P_VS_SELECT       = 151,
    NOTIFY_CONTROLLER_SUPPORT = 152,
    NOTIFY_STAGE_RETRY        = 153,
    NOTIFY_SOUND_TRACK        = 154,
    NOTIFY_GOOD_ENDING        = 155,
    NOTIFY_BACK_TO_MAINMENU   = 156,
    NOTIFY_LEVEL_SELECT_MENU  = 157,
    NOTIFY_PLAYER_SET         = 158,
    NOTIFY_EXTRAS_MODE        = 159,
    NOTIFY_SPIN_DASH_TYPE     = 160,
    NOTIFY_TIME_OVER          = 161,
};

void NotifyCallback(int *callback, int *param1, int *param2, int *param3)
{
    if (!callback || !param1)
        return;

    switch (*callback) {
        default: PrintLog("NOTIFY: Unknown Callback -> %d", *param1); break;
        case NOTIFY_DEATH_EVENT: PrintLog("NOTIFY: DeathEvent() -> %d", *param1); break;
        case NOTIFY_TOUCH_SIGNPOST: PrintLog("NOTIFY: TouchSignPost() -> %d", *param1); break;
        case NOTIFY_HUD_ENABLE: PrintLog("NOTIFY: HUDEnable() -> %d", *param1); break;
        case NOTIFY_ADD_COIN:
            PrintLog("NOTIFY: AddCoin() -> %d", *param1);
            SetGlobalVariableByName("game.coinCount", GetGlobalVariableByName("game.coinCount") + *param1);
            break;
        case NOTIFY_KILL_ENEMY: PrintLog("NOTIFY: KillEnemy() -> %d", *param1); break;
        case NOTIFY_SAVESLOT_SELECT: PrintLog("NOTIFY: SaveSlotSelect() -> %d", *param1); break;
        case NOTIFY_FUTURE_PAST: PrintLog("NOTIFY: FuturePast() -> %d", *param1); break;
        case NOTIFY_GOTO_FUTURE_PAST: PrintLog("NOTIFY: GotoFuturePast() -> %d", *param1); break;
        case NOTIFY_BOSS_END: PrintLog("NOTIFY: BossEnd() -> %d", *param1); break;
        case NOTIFY_SPECIAL_END: PrintLog("NOTIFY: SpecialEnd() -> %d", *param1); break;
        case NOTIFY_DEBUGPRINT:
            // This callback can be called with either CallNativeFunction2 or CallNativeFunction4
            // todo: find a better way to check for which one was used
            if (*param2 == 264865096)
                PrintLog("NOTIFY: DebugPrint() -> %d", *param1);
            else
                PrintLog("NOTIFY: DebugPrint() -> %d, %d, %d", *param1, *param2, *param3);
            break;
        case NOTIFY_KILL_BOSS: PrintLog("NOTIFY: KillBoss() -> %d", *param1); break;
        case NOTIFY_TOUCH_EMERALD: PrintLog("NOTIFY: TouchEmerald() -> %d", *param1); break;
        case NOTIFY_STATS_ENEMY: PrintLog("NOTIFY: StatsEnemy() -> %d, %d, %d", *param1, *param2, *param3); break;
        case NOTIFY_STATS_CHARA_ACTION: PrintLog("NOTIFY: StatsCharaAction() -> %d, %d, %d", *param1, *param2, *param3); break;
        case NOTIFY_STATS_RING: PrintLog("NOTIFY: StatsRing() -> %d", *param1); break;
        case NOTIFY_STATS_MOVIE: PrintLog("NOTIFY: StatsMovie() -> %d", *param1); break;
        case NOTIFY_STATS_PARAM_1: PrintLog("NOTIFY: StatsParam1() -> %d, %d, %d", *param1, *param2, *param3); break;
        case NOTIFY_STATS_PARAM_2: PrintLog("NOTIFY: StatsParam2() -> %d", *param1); break;
        case NOTIFY_CHARACTER_SELECT:
            PrintLog("NOTIFY: CharacterSelect() -> %d", *param1);
            SetGlobalVariableByName("game.callbackResult", 1);
            SetGlobalVariableByName("game.continueFlag", 0);
            break;
        case NOTIFY_SPECIAL_RETRY:
            PrintLog("NOTIFY: SpecialRetry() -> %d, %d, %d", *param1, *param2, *param3);
            SetGlobalVariableByName("game.callbackResult", 1);
            break;
        case NOTIFY_TOUCH_CHECKPOINT: PrintLog("NOTIFY: TouchCheckpoint() -> %d", *param1); break;
        case NOTIFY_ACT_FINISH: PrintLog("NOTIFY: ActFinish() -> %d", *param1); break;
        case NOTIFY_1P_VS_SELECT: PrintLog("NOTIFY: 1PVSSelect() -> %d", *param1); break;
        case NOTIFY_CONTROLLER_SUPPORT:
            PrintLog("NOTIFY: ControllerSupport() -> %d", *param1);
            SetGlobalVariableByName("game.callbackResult", 1);
            break;
        case NOTIFY_STAGE_RETRY: PrintLog("NOTIFY: StageRetry() -> %d", *param1); break;
        case NOTIFY_SOUND_TRACK: PrintLog("NOTIFY: SoundTrack() -> %d", *param1); break;
        case NOTIFY_GOOD_ENDING: PrintLog("NOTIFY: GoodEnding() -> %d", *param1); break;
        case NOTIFY_BACK_TO_MAINMENU: PrintLog("NOTIFY: BackToMainMenu() -> %d", *param1); break;
        case NOTIFY_LEVEL_SELECT_MENU: PrintLog("NOTIFY: LevelSelectMenu() -> %d", *param1); break;
        case NOTIFY_PLAYER_SET: PrintLog("NOTIFY: PlayerSet() -> %d", *param1); break;
        case NOTIFY_EXTRAS_MODE: PrintLog("NOTIFY: ExtrasMode() -> %d", *param1); break;
        case NOTIFY_SPIN_DASH_TYPE: PrintLog("NOTIFY: SpindashType() -> %d", *param1); break;
        case NOTIFY_TIME_OVER: PrintLog("NOTIFY: TimeOver() -> %d", *param1); break;
    }
}
#endif

void ExitGame() { Engine.running = false; }

void FileExists(int *unused, const char *filePath)
{
    FileInfo info;
    scriptEng.checkResult = false;
    if (LoadFile(filePath, &info)) {
        scriptEng.checkResult = true;
        CloseFile();
    }
}

#if RETRO_USE_MOD_LOADER
void GetScreenWidth() { scriptEng.checkResult = SCREEN_XSIZE_CONFIG; }
void GetWindowScale() { scriptEng.checkResult = Engine.windowScale; }
void GetWindowScaleMode() { scriptEng.checkResult = Engine.scalingMode; }
void GetWindowFullScreen() { scriptEng.checkResult = Engine.isFullScreen; }
void GetWindowBorderless() { scriptEng.checkResult = Engine.borderless; }
void GetWindowVSync() { scriptEng.checkResult = Engine.vsync; }

bool changedScreenWidth = false;
void SetScreenWidth(int *width, int *unused)
{
    if (!width)
        return;

    SCREEN_XSIZE_CONFIG = *width;
    changedScreenWidth  = SCREEN_XSIZE_CONFIG != SCREEN_XSIZE;
}

void SetWindowScale(int *scale, int *unused)
{
    if (!scale)
        return;

    Engine.windowScale = *scale;
}

void SetWindowScaleMode(int *mode, int *unused)
{
    if (!mode)
        return;

    Engine.scalingMode = *mode;
}

void SetWindowFullScreen(int *fullscreen, int *unused)
{
    if (!fullscreen)
        return;

    Engine.isFullScreen    = *fullscreen;
    Engine.startFullScreen = *fullscreen;
}

void SetWindowBorderless(int *borderless, int *unused)
{
    if (!borderless)
        return;

    Engine.borderless = *borderless;
}

void SetWindowVSync(int *enabled, int *unused)
{
    if (!enabled)
        return;

    Engine.vsync = *enabled;
}
void ApplyWindowChanges()
{
    for (int i = 0; i < TEXTURE_COUNT; ++i) {
        glDeleteTextures(1, &textureList[i].id);
    }

    for (int i = 0; i < MESH_COUNT; ++i) {
        MeshInfo *mesh = &meshList[i];
        if (StrLength(mesh->fileName)) {
            if (mesh->frameCount > 1)
                free(mesh->frames);
            if (mesh->indexCount)
                free(mesh->indices);
            if (mesh->vertexCount)
                free(mesh->vertices);

            mesh->frameCount  = 0;
            mesh->indexCount  = 0;
            mesh->vertexCount = 0;
        }
    }

    if (changedScreenWidth)
        SCREEN_XSIZE = SCREEN_XSIZE_CONFIG;
    changedScreenWidth = false;

    ReleaseRenderDevice(true);
    InitRenderDevice();

    for (int i = 1; i < TEXTURE_COUNT; ++i) {
        if (StrLength(textureList[i].fileName)) {
            char fileName[64];
            StrCopy(fileName, textureList[i].fileName);
            textureList[i].fileName[0] = 0;

            LoadTexture(fileName, textureList[i].format);
        }
    }

    for (int i = 0; i < MESH_COUNT; ++i) {
        if (StrLength(meshList[i].fileName)) {
            char fileName[64];
            StrCopy(fileName, meshList[i].fileName);
            meshList[i].fileName[0] = 0;

            LoadMesh(fileName, meshList[i].textureID);
        }
    }
}
#endif
