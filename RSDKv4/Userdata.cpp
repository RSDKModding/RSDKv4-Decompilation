#include "RetroEngine.hpp"

int globalVariablesCount;
int globalVariables[GLOBALVAR_COUNT];
char globalVariableNames[GLOBALVAR_COUNT][0x20];

void *nativeFunction[NATIIVEFUNCTION_MAX];
int nativeFunctionCount = 0;

char gamePath[0x100];
int saveRAM[SAVEDATA_MAX];
Achievement achievements[ACHIEVEMENT_MAX];
int achievementCount = 0;

LeaderboardEntry leaderboards[LEADERBOARD_MAX];

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

#if RETRO_PLATFORM == RETRO_OSX
#include <sys/stat.h>
#include <sys/types.h>
#endif

#if !RETRO_USE_ORIGINAL_CODE
bool forceUseScripts          = false;
bool forceUseScripts_Config   = false;
bool skipStartMenu            = false;
bool skipStartMenu_Config     = false;
bool disableFocusPause        = false;
bool disableFocusPause_Config = false;

bool useSGame = false;

bool ReadSaveRAMData()
{
    useSGame = false;
    char buffer[0x180];
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

    FileIO *saveFile = fOpen(buffer, "rb");
    if (!saveFile) {
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

        saveFile = fOpen(buffer, "rb");
        if (!saveFile)
            return false;
        useSGame = true;
    }
    fRead(saveRAM, sizeof(int), SAVEDATA_MAX, saveFile);
    fClose(saveFile);
    return true;
}

bool WriteSaveRAMData()
{
    char buffer[0x180];
    if (!useSGame) {
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
    }
    else {
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
    }

    FileIO *saveFile = fOpen(buffer, "wb");
    if (!saveFile)
        return false;
    fWrite(saveRAM, sizeof(int), SAVEDATA_MAX, saveFile);
    fClose(saveFile);
    return true;
}

void InitUserdata()
{
    // userdata files are loaded from this directory
    sprintf(gamePath, "%s", BASE_PATH);
    sprintf(modsPath, "%s", BASE_PATH);

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
        sprintf(modsPath, "%s", buffer);

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
        ini.SetBool("Dev", "UseHQModes", Engine.useHQModes = true);
        ini.SetString("Dev", "DataFile", (char *)"Data.rsdk");
        StrCopy(Engine.dataFile[0], "Data.rsdk");
        if (!StrComp(Engine.dataFile[1], "")) {
            ini.SetString("Dev", "DataFile2", (char *)"Data2.rsdk");
            StrCopy(Engine.dataFile[2], "Data2.rsdk");
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
        ini.SetBool("Game", "SkipStartMenu", skipStartMenu = false);
        skipStartMenu_Config = skipStartMenu;
        ini.SetBool("Game", "DisableFocusPause", disableFocusPause = false);
        disableFocusPause_Config = disableFocusPause;

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

        if (!ini.GetString("Dev", "DataFile", Engine.dataFile[0]))
            StrCopy(Engine.dataFile[0], "Data.rsdk");
        if (!StrComp(Engine.dataFile[1], "")) {
            ini.SetString("Dev", "DataFile2", (char *)"Data2.rsdk");
            StrCopy(Engine.dataFile[2], "Data2.rsdk");
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
        if (!ini.GetBool("Game", "SkipStartMenu", &skipStartMenu))
            skipStartMenu = false;
        skipStartMenu_Config = skipStartMenu;
        if (!ini.GetBool("Game", "DisableFocusPause", &disableFocusPause))
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
            printLog("loaded %d controller mappings from '%s'\n", buffer, nummaps);
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

void writeSettings()
{
    IniParser ini;

    ini.SetComment("Dev", "DevMenuComment", "Enable this flag to activate dev menu via the ESC key");
    ini.SetBool("Dev", "DevMenu", Engine.devMenu);
    ini.SetComment("Dev", "DebugModeComment",
                   "Enable this flag to activate features used for debugging the engine (may result in slightly slower game speed)");
    ini.SetBool("Dev", "EngineDebugMode", engineDebugMode);
    ini.SetComment("Dev", "ScriptsComment",
                   "Enable this flag to force the engine to load from the scripts folder instead of from bytecode");
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
    ini.SetComment("Game", "SSMenuComment", "if set to true, disables the start menu");
    ini.SetBool("Game", "SkipStartMenu", skipStartMenu_Config);
    ini.SetComment("Game", "DFPMenuComment", "if set to true, disables the game pausing when focus is lost");
    ini.SetBool("Game", "DisableFocusPause", disableFocusPause_Config);

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
    ini.SetComment("Window", "VSComment", "Determines if VSync will be active or not");
    ini.SetBool("Window", "VSync", Engine.vsync);
    ini.SetComment("Window", "SMComment",
                   "Determines what scaling is used. 0 is nearest neighbour, 1 is integer scale, 2 is sharp bilinear, and 3 is regular bilinear.");
    ini.SetComment("Window", "SMWarning",
                   "Note: Not all scaling options work correctly on certain platforms, as they don't support bilinear filtering.");
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
    ini.SetComment("Keyboard 1", "IK1Comment", "Keyboard Mappings for P1 (Based on: https://github.com/libsdl-org/sdlwiki/blob/main/SDLScancodeLookup.mediawiki)");
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
    ini.SetComment("Controller 1", "IC1Comment", "Controller Mappings for P1 (Based on: https://github.com/libsdl-org/sdlwiki/blob/main/SDLScancodeLookup.mediawiki)");
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
    FileIO *userFile = fOpen(buffer, "rb");
    if (!userFile)
        return;

    int buf = 0;
    for (int a = 0; a < ACHIEVEMENT_MAX; ++a) {
        fRead(&buf, 4, 1, userFile);
        achievements[a].status = buf;
    }
    for (int l = 0; l < LEADERBOARD_MAX; ++l) {
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
    FileIO *userFile = fOpen(buffer, "wb");
    if (!userFile)
        return;

    for (int a = 0; a < ACHIEVEMENT_MAX; ++a) fWrite(&achievements[a].status, 4, 1, userFile);
    for (int l = 0; l < LEADERBOARD_MAX; ++l) fWrite(&leaderboards[l].score, 4, 1, userFile);

    fClose(userFile);

    if (Engine.onlineActive) {
        // Load from online
    }
}
#endif

void AwardAchievement(int id, int status)
{
    if (id < 0 || id >= ACHIEVEMENT_MAX)
        return;

    if (status == 100 && status != achievements[id].status)
        printLog("Achieved achievement: %s (%d)!", achievements[id].name, status);

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
void SetAchievementDescription(int *id, const char *desc) { StrCopy(achievements[*id].desc, desc); }
void ClearAchievements() { achievementCount = 0; }
void GetAchievementCount() { scriptEng.checkResult = achievementCount; }
void GetAchievementName(uint *id, int* textMenu)
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
            printLog("Set leaderboard (%d) value to %d", *leaderboardID, score);
            leaderboards[*leaderboardID].score = *score;
            WriteUserdata();
        }
        else {
            printLog("Attempted to set leaderboard (%d) value to %d... but score was already %d!", *leaderboardID, *score,
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
    printLog("we're showing the leaderboards screen");
}

bool disableFocusPause_Store = false;
void Connect2PVS(int *gameLength, int *itemMode)
{
    printLog("Attempting to connect to 2P game (%d) (%d)", *gameLength, *itemMode);

    multiplayerDataIN.type = 0;
    matchValueData[0]      = 0;
    matchValueData[1]      = 0;
    matchValueReadPos      = 0;
    matchValueWritePos     = 0;
    Engine.gameMode        = ENGINE_CONNECT2PVS;
    // PauseSound();
    // actual connection code
    vsGameLength = *gameLength;
    vsItemMode   = *itemMode;
    if (Engine.onlineActive) {
#if RETRO_USE_NETWORKING
        disableFocusPause_Store = disableFocusPause;
        disableFocusPause       = true;
        runNetwork();
#endif
    }
}
void Disconnect2PVS()
{
    printLog("Attempting to disconnect from 2P game");

    if (Engine.onlineActive) {
#if RETRO_USE_NETWORKING
        disableFocusPause = disableFocusPause_Store;
        //Engine.devMenu    = vsPlayerID;
        vsPlaying         = false;
        disconnectNetwork();
        initNetwork();
#endif
    }
}
void SendEntity(int *entityID, void *unused)
{
    if (!sendCounter) {
        multiplayerDataOUT.type = 1;
        memcpy(multiplayerDataOUT.data, &objectEntityList[*entityID], sizeof(Entity));
        if (Engine.onlineActive) {
#if RETRO_USE_NETWORKING
            sendData();
#endif
        }
    }
    sendCounter = (sendCounter + 1) % 2;
}
void SendValue(int *value, void *unused)
{
    // printLog("Attempting to send value (%d) (%d)", *dataSlot, *value);

    multiplayerDataOUT.type    = 0;
    multiplayerDataOUT.data[0] = *value;
    if (Engine.onlineActive) {
#if RETRO_USE_NETWORKING
        sendData();
#endif
    }
}
bool recieveReady = false;
void ReceiveEntity(int *entityID, int *incrementPos)
{
    // printLog("Attempting to receive entity (%d) (%d)", *clearOnReceive, *entityID);

    if (Engine.onlineActive && recieveReady) {
        // recieveReady = false;
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
    // printLog("Attempting to receive value (%d) (%d)", *incrementPos, *value);

    if (Engine.onlineActive && recieveReady) {
        // recieveReady = false;
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
    printLog("Attempting to transmit global (%s) (%d)", globalName, *globalValue);

    multiplayerDataOUT.type    = 2;
    multiplayerDataOUT.data[0] = GetGlobalVariableID(globalName);
    multiplayerDataOUT.data[1] = *globalValue;
    if (Engine.onlineActive) {
#if RETRO_USE_NETWORKING
        sendData();
#endif
    }
}

void receive2PVSData(MultiplayerData *data)
{
    recieveReady = true;
    switch (data->type) {
        case 0: matchValueData[matchValueWritePos++] = data->data[0]; break;
        case 1:
            multiplayerDataIN.type = 1;
            memcpy(multiplayerDataIN.data, data->data, sizeof(Entity));
            break;
        case 2: globalVariables[data->data[0]] = data->data[1]; break;
    }
}

void receive2PVSMatchCode(int code)
{
    recieveReady = true;
    code &= 0x00000FF0;
    code |= 0x00001000 * vsPlayerID;
    matchValueData[matchValueWritePos++] = code;
    ResumeSound();
    vsPlayerID      = Engine.devMenu;
    //Engine.devMenu  = false;
    Engine.gameMode = ENGINE_MAINGAME;
    vsPlaying       = true;
    ClearNativeObjects();
    CREATE_ENTITY(RetroGameLoop); // hack
    if (Engine.gameDeviceType == RETRO_MOBILE)
        CREATE_ENTITY(VirtualDPad);
    CREATE_ENTITY(MultiplayerHandler);
}

void ShowPromoPopup(int *id, const char *popupName) { printLog("Attempting to show promo popup: \"%s\" (%d)", popupName, id ? *id : 0); }
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
        default: printLog("Showing unknown website: (%d)", websiteID); break;
        case 0: printLog("Showing website: \"%s\" (%d)", "http://www.sega.com/mprivacy", websiteID); break;
        case 1: printLog("Showing website: \"%s\" (%d)", "http://www.sega.com/legal", websiteID); break;
    }
}

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

void SetScreenWidth(int *width, int *unused)
{
    SCREEN_XSIZE = SCREEN_XSIZE_CONFIG = *width;
#if RETRO_PLATFORM != RETRO_ANDROID
    SetScreenDimensions(SCREEN_XSIZE_CONFIG * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale);

#if RETRO_SOFTWARE_RENDER
    if (Engine.frameBuffer)
        delete[] Engine.frameBuffer;
    if (Engine.frameBuffer2x)
        delete[] Engine.frameBuffer2x;

    Engine.frameBuffer   = new ushort[GFX_LINESIZE * SCREEN_YSIZE];
    Engine.frameBuffer2x = new ushort[GFX_LINESIZE_DOUBLE * (SCREEN_YSIZE * 2)];
    memset(Engine.frameBuffer, 0, (GFX_LINESIZE * SCREEN_YSIZE) * sizeof(ushort));
    memset(Engine.frameBuffer2x, 0, GFX_LINESIZE_DOUBLE * (SCREEN_YSIZE * 2) * sizeof(ushort));
#endif
    if (Engine.texBuffer)
        delete[] Engine.texBuffer;

    Engine.texBuffer = new uint[GFX_LINESIZE * SCREEN_YSIZE];
    memset(Engine.texBuffer, 0, (GFX_LINESIZE * SCREEN_YSIZE) * sizeof(uint));
#endif

#if RETRO_USING_SDL2
    SDL_SetWindowSize(Engine.window, SCREEN_XSIZE_CONFIG * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale);
#endif

#if RETRO_USING_OPENGL
    displaySettings.width   = SCREEN_XSIZE * Engine.windowScale;
    displaySettings.height  = SCREEN_YSIZE * Engine.windowScale;
    displaySettings.offsetX = 0;
    setupViewport();
#endif
}
void SetWindowScale(int *scale, int *unused)
{
    Engine.windowScale = *scale;
#if RETRO_USING_SDL2
    SDL_SetWindowSize(Engine.window, SCREEN_XSIZE_CONFIG * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale);
#endif

#if RETRO_USING_OPENGL
    displaySettings.width   = SCREEN_XSIZE * Engine.windowScale;
    displaySettings.height  = SCREEN_YSIZE * Engine.windowScale;
    displaySettings.offsetX = 0;
    setupViewport();
#endif
}
void SetWindowFullScreen(int *fullscreen, int *unused)
{
    Engine.isFullScreen = *fullscreen;
    setFullScreen(Engine.isFullScreen);
}
void SetWindowBorderless(int *borderless, int *unused)
{
    Engine.borderless = *borderless;
#if RETRO_USING_SDL2
    SDL_RestoreWindow(Engine.window);
    SDL_SetWindowBordered(Engine.window, Engine.borderless ? SDL_FALSE : SDL_TRUE);
#endif
}
