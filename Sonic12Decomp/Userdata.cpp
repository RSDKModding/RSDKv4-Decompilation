#include "RetroEngine.hpp"

int globalVariablesCount;
int globalVariables[GLOBALVAR_COUNT];
char globalVariableNames[GLOBALVAR_COUNT][0x20];

int (*nativeFunction[16])(int, void *);
int nativeFunctionCount = 0;

char gamePath[0x100];
int saveRAM[SAVEDATA_MAX];
Achievement achievements[ACHIEVEMENT_MAX];
LeaderboardEntry leaderboard[LEADERBOARD_MAX];

void InitUserdata()
{
    // userdata files are loaded from this directory
    sprintf(gamePath, "%s", "");

    char buffer[0x100];
#if RETRO_PLATFORM == RETRO_OSX
    if (!usingCWD)
        sprintf(buffer, "%s/settings.ini", getResourcesPath());
    else
        sprintf(buffer, "%ssettings.ini", gamePath);
#else
    sprintf(buffer, "%ssettings.ini", gamePath);
#endif
    FileIO *file = fOpen(buffer, "rb");
    if (!file) {
        IniParser ini;

        ini.SetBool("Dev", "DevMenu", Engine.devMenu = false);
        ini.SetBool("Dev", "StartingCategory", Engine.startList = 0);
        ini.SetBool("Dev", "StartingScene", Engine.startStage = 0);
        ini.SetBool("Dev", "FastForwardSpeed", Engine.fastForwardSpeed = 8);

        ini.SetBool("Game", "Language", Engine.language = RETRO_EN);

        ini.SetBool("Window", "Fullscreen", Engine.fullScreen = false);
        ini.SetBool("Window", "Borderless", Engine.borderless = false);
        ini.SetBool("Window", "VSync", Engine.vsync = false);
        ini.SetInteger("Window", "WindowScale", Engine.windowScale = 2);
        ini.SetInteger("Window", "ScreenWidth", SCREEN_XSIZE = 424);
        ini.SetInteger("Window", "RefreshRate", Engine.refreshRate = 60);

        ini.SetFloat("Audio", "BGMVolume", bgmVolume / (float)MAX_VOLUME);
        ini.SetFloat("Audio", "SFXVolume", sfxVolume / (float)MAX_VOLUME);

        ini.SetInteger("Keyboard 1", "Up", inputDevice[0].keyMappings = SDL_SCANCODE_UP);
        ini.SetInteger("Keyboard 1", "Down", inputDevice[1].keyMappings = SDL_SCANCODE_DOWN);
        ini.SetInteger("Keyboard 1", "Left", inputDevice[2].keyMappings = SDL_SCANCODE_LEFT);
        ini.SetInteger("Keyboard 1", "Right", inputDevice[3].keyMappings = SDL_SCANCODE_RIGHT);
        ini.SetInteger("Keyboard 1", "A", inputDevice[4].keyMappings = SDL_SCANCODE_Z);
        ini.SetInteger("Keyboard 1", "B", inputDevice[5].keyMappings = SDL_SCANCODE_X);
        ini.SetInteger("Keyboard 1", "C", inputDevice[6].keyMappings = SDL_SCANCODE_C);
        ini.SetInteger("Keyboard 1", "Start", inputDevice[7].keyMappings = SDL_SCANCODE_RETURN);

        ini.SetInteger("Controller 1", "Up", inputDevice[0].contMappings = SDL_CONTROLLER_BUTTON_DPAD_UP);
        ini.SetInteger("Controller 1", "Down", inputDevice[1].contMappings = SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        ini.SetInteger("Controller 1", "Left", inputDevice[2].contMappings = SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        ini.SetInteger("Controller 1", "Right", inputDevice[3].contMappings = SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        ini.SetInteger("Controller 1", "A", inputDevice[4].contMappings = SDL_CONTROLLER_BUTTON_A);
        ini.SetInteger("Controller 1", "B", inputDevice[5].contMappings = SDL_CONTROLLER_BUTTON_B);
        ini.SetInteger("Controller 1", "C", inputDevice[6].contMappings = SDL_CONTROLLER_BUTTON_X);
        ini.SetInteger("Controller 1", "Start", inputDevice[7].contMappings = SDL_CONTROLLER_BUTTON_START);

        ini.Write("settings.ini");
    }
    else {
        fClose(file);
        IniParser ini("settings.ini");

        if (!ini.GetBool("Dev", "DevMenu", &Engine.devMenu))
            Engine.devMenu = false;
        if (!ini.GetInteger("Dev", "StartingCategory", &Engine.startList))
            Engine.startList = 0;
        if (!ini.GetInteger("Dev", "StartingScene", &Engine.startStage))
            Engine.startStage = 0;
        if (!ini.GetInteger("Dev", "FastForwardSpeed", &Engine.fastForwardSpeed))
            Engine.fastForwardSpeed = 8;

        if (!ini.GetInteger("Game", "Language", &Engine.language))
            Engine.language = RETRO_EN;

        if (!ini.GetBool("Window", "FullScreen", &Engine.fullScreen))
            Engine.fullScreen = false;
        if (!ini.GetBool("Window", "Borderless", &Engine.borderless))
            Engine.borderless = false;
        if (!ini.GetBool("Window", "VSync", &Engine.vsync))
            Engine.vsync = false;
        if (!ini.GetInteger("Window", "WindowScale", &Engine.windowScale))
            Engine.windowScale = 2;
        if (!ini.GetInteger("Window", "ScreenWidth", &SCREEN_XSIZE))
            SCREEN_XSIZE = 424;
        if (!ini.GetInteger("Window", "Refresh Rate", &Engine.refreshRate))
            Engine.refreshRate = 60;

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

        if (!ini.GetInteger("Keyboard 1", "Up", &inputDevice[0].keyMappings))
            inputDevice[0].keyMappings = SDL_SCANCODE_UP;
        if (!ini.GetInteger("Keyboard 1", "Down", &inputDevice[1].keyMappings))
            inputDevice[1].keyMappings = SDL_SCANCODE_DOWN;
        if (!ini.GetInteger("Keyboard 1", "Left", &inputDevice[2].keyMappings))
            inputDevice[2].keyMappings = SDL_SCANCODE_LEFT;
        if (!ini.GetInteger("Keyboard 1", "Right", &inputDevice[3].keyMappings))
            inputDevice[3].keyMappings = SDL_SCANCODE_RIGHT;
        if (!ini.GetInteger("Keyboard 1", "A", &inputDevice[4].keyMappings))
            inputDevice[4].keyMappings = SDL_SCANCODE_Z;
        if (!ini.GetInteger("Keyboard 1", "B", &inputDevice[5].keyMappings))
            inputDevice[5].keyMappings = SDL_SCANCODE_X;
        if (!ini.GetInteger("Keyboard 1", "C", &inputDevice[6].keyMappings))
            inputDevice[6].keyMappings = SDL_SCANCODE_C;
        if (!ini.GetInteger("Keyboard 1", "Start", &inputDevice[7].keyMappings))
            inputDevice[7].keyMappings = SDL_SCANCODE_RETURN;

        if (!ini.GetInteger("Controller 1", "Up", &inputDevice[0].contMappings))
            inputDevice[0].contMappings = SDL_CONTROLLER_BUTTON_DPAD_UP;
        if (!ini.GetInteger("Controller 1", "Down", &inputDevice[1].contMappings))
            inputDevice[1].contMappings = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
        if (!ini.GetInteger("Controller 1", "Left", &inputDevice[2].contMappings))
            inputDevice[2].contMappings = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
        if (!ini.GetInteger("Controller 1", "Right", &inputDevice[3].contMappings))
            inputDevice[3].contMappings = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
        if (!ini.GetInteger("Controller 1", "A", &inputDevice[4].contMappings))
            inputDevice[4].contMappings = SDL_CONTROLLER_BUTTON_A;
        if (!ini.GetInteger("Controller 1", "B", &inputDevice[5].contMappings))
            inputDevice[5].contMappings = SDL_CONTROLLER_BUTTON_B;
        if (!ini.GetInteger("Controller 1", "C", &inputDevice[6].contMappings))
            inputDevice[6].contMappings = SDL_CONTROLLER_BUTTON_X;
        if (!ini.GetInteger("Controller 1", "Start", &inputDevice[7].contMappings))
            inputDevice[7].contMappings = SDL_CONTROLLER_BUTTON_START;
    }
    SetScreenSize(SCREEN_XSIZE, SCREEN_YSIZE);

#if RETRO_PLATFORM == RETRO_OSX
    if (!usingCWD)
        sprintf(buffer, "%s/UData.bin", getResourcesPath());
    else
        sprintf(buffer, "%sUData.bin", gamePath);
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

void writeSettings() {
    IniParser ini;

    ini.SetComment("Dev", "DevMenuComment", "Enable this flag to activate dev menu via the ESC key");
    ini.SetBool("Dev", "DevMenu", Engine.devMenu);
    ini.SetComment("Dev", "SCComment", "Sets the starting category ID");
    ini.SetBool("Dev", "StartingCategory", Engine.startList);
    ini.SetComment("Dev", "SSComment", "Sets the starting scene ID");
    ini.SetBool("Dev", "StartingScene", Engine.startStage);
    ini.SetComment("Dev", "FFComment", "Determines how fast the game will be when fastforwarding is active");
    ini.SetInteger("Dev", "FastForwardSpeed", Engine.fastForwardSpeed);

    ini.SetComment("Game", "LangComment", "Sets the game language (0 = EN, 1 = FR, 2 = IT, 3 = DE, 4 = ES, 5 = JP, 6 = PT, 7 = RU, 8 = KO, 9 = ZH, 10 = ZS)");
    ini.SetInteger("Game", "Language", Engine.language);

    ini.SetComment("Window", "FSComment", "Determines if the window will be fullscreen or not");
    ini.SetBool("Window", "Fullscreen", Engine.fullScreen);
    ini.SetComment("Window", "BLComment", "Determines if the window will be borderless or not");
    ini.SetBool("Window", "Borderless", Engine.borderless);
    ini.SetComment("Window", "VSComment", "Determines if VSync will be active or not");
    ini.SetBool("Window", "VSync", Engine.vsync);
    ini.SetComment("Window", "WSComment", "How big the window will be");
    ini.SetInteger("Window", "WindowScale", Engine.windowScale);
    ini.SetComment("Window", "SWComment", "How wide the base screen will be in pixels");
    ini.SetInteger("Window", "ScreenWidth", SCREEN_XSIZE);
    ini.SetComment("Window", "RRComment", "Determines the target FPS");
    ini.SetInteger("Window", "RefreshRate", Engine.refreshRate);

    ini.SetFloat("Audio", "BGMVolume", bgmVolume / (float)MAX_VOLUME);
    ini.SetFloat("Audio", "SFXVolume", sfxVolume / (float)MAX_VOLUME);

    ini.SetComment("Keyboard 1", "IK1Comment", "Keyboard Mappings for P1 (Based on: https://wiki.libsdl.org/SDL_Scancode)");
    ini.SetInteger("Keyboard 1", "Up", inputDevice[0].keyMappings);
    ini.SetInteger("Keyboard 1", "Down", inputDevice[1].keyMappings);
    ini.SetInteger("Keyboard 1", "Left", inputDevice[2].keyMappings);
    ini.SetInteger("Keyboard 1", "Right", inputDevice[3].keyMappings);
    ini.SetInteger("Keyboard 1", "A", inputDevice[4].keyMappings);
    ini.SetInteger("Keyboard 1", "B", inputDevice[5].keyMappings);
    ini.SetInteger("Keyboard 1", "C", inputDevice[6].keyMappings);
    ini.SetInteger("Keyboard 1", "Start", inputDevice[7].keyMappings);

    ini.SetComment("Controller 1", "IC1Comment", "Controller Mappings for P1 (Based on: https://wiki.libsdl.org/SDL_GameControllerButton)");
    ini.SetInteger("Controller 1", "Up", inputDevice[0].contMappings);
    ini.SetInteger("Controller 1", "Down", inputDevice[1].contMappings);
    ini.SetInteger("Controller 1", "Left", inputDevice[2].contMappings);
    ini.SetInteger("Controller 1", "Right", inputDevice[3].contMappings);
    ini.SetInteger("Controller 1", "A", inputDevice[4].contMappings);
    ini.SetInteger("Controller 1", "B", inputDevice[5].contMappings);
    ini.SetInteger("Controller 1", "C", inputDevice[6].contMappings);
    ini.SetInteger("Controller 1", "Start", inputDevice[7].contMappings);

    ini.Write("settings.ini");
}

void ReadUserdata()
{
    char buffer[0x100];
#if RETRO_PLATFORM == RETRO_OSX
    if (!usingCWD)
        sprintf(buffer, "%s/UData.bin", getResourcesPath());
    else
        sprintf(buffer, "%sUData.bin", gamePath);
#else
    sprintf(buffer, "%sUData.bin", gamePath);
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
        leaderboard[l].status = buf;
    }

    fClose(userFile);

    if (Engine.onlineActive) {
        // Load from online
    }
}

void WriteUserdata()
{
    char buffer[0x100];
#if RETRO_PLATFORM == RETRO_OSX
    if (!usingCWD)
        sprintf(buffer, "%s/UData.bin", getResourcesPath());
    else
        sprintf(buffer, "%sUData.bin", gamePath);
#else
    sprintf(buffer, "%sUData.bin", gamePath);
#endif
    FileIO *userFile = fOpen(buffer, "wb");
    if (!userFile)
        return;

    for (int a = 0; a < ACHIEVEMENT_MAX; ++a) fWrite(&achievements[a].status, 4, 1, userFile);
    for (int l = 0; l < LEADERBOARD_MAX; ++l) fWrite(&leaderboard[l].status, 4, 1, userFile);

    fClose(userFile);

    if (Engine.onlineActive) {
        // Load from online
    }
}

void AwardAchievement(int id, int status)
{
    if (id < 0 || id >= ACHIEVEMENT_MAX)
        return;

#if RSDK_DEBUG
    if (status == 100 && status != achievements[id].status)
        printLog("Achieved achievement: %s (%d)!", achievements[id].name, status);
#endif

    achievements[id].status = status;

    if (Engine.onlineActive) {
        // Set Achievement online
    }
    WriteUserdata();
}

int SetAchievement(int achievementID, void* achDone)
{
    int achievementDone = static_cast<int>(reinterpret_cast<intptr_t>(achDone));
    if (!Engine.trialMode && !debugMode) {
        AwardAchievement(achievementID, achievementDone);
        return 1;
    }
    return 0;
}
int SetLeaderboard(int leaderboardID, void *res)
{
    int result = static_cast<int>(reinterpret_cast<intptr_t>(res));
    if (!Engine.trialMode && !debugMode) {
#if RSDK_DEBUG
        printLog("Set leaderboard (%d) value to %d", leaderboard, result);
#endif
        switch (leaderboardID) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
                leaderboard[leaderboardID].status = result;
                WriteUserdata();
                return 1;
        }
    }
    return 0;
}

int Connect2PVS(int a1, void *a2)
{
#if RSDK_DEBUG 
    printLog("Attempting to connect to 2P game (%d) (%p)", a1, a2);
#endif
    if (Engine.onlineActive) {
        // Do online code
        return 1;
    }
    return 0;
}
int Disconnect2PVS(int a1, void *a2)
{
#if RSDK_DEBUG 
    printLog("Attempting to disconnect from 2P game (%d) (%p)", a1, a2);
#endif
    if (Engine.onlineActive) {
        // Do online code
        return 1;
    }
    return 0;
}
int SendEntity(int a1, void *a2)
{
#if RSDK_DEBUG
    printLog("Attempting to send entity (%d) (%p)", a1, a2);
#endif
    if (Engine.onlineActive) {
        // Do online code
        return 1;
    }
    return 0;
}
int SendValue(int a1, void *a2)
{
#if RSDK_DEBUG
    printLog("Attempting to send value (%d) (%p)", a1, a2);
#endif
    if (Engine.onlineActive) {
        // Do online code
        return 1;
    }
    return 0;
}
int ReceiveEntity(int a1, void *a2)
{
#if RSDK_DEBUG
    printLog("Attempting to receive entity (%d) (%p)", a1, a2);
#endif
    if (Engine.onlineActive) {
        // Do online code
        return 1;
    }
    return 0;
}
int ReceiveValue(int a1, void *a2)
{
#if RSDK_DEBUG
    printLog("Attempting to receive value (%d) (%p)", a1, a2);
#endif
    if (Engine.onlineActive) {
        // Do online code
        return 1;
    }
    return 0;
}
int TransmitGlobal(int a1, void *a2)
{
#if RSDK_DEBUG
    printLog("Attempting to transmit global (%d) (%p)", a1, a2);
#endif
    if (Engine.onlineActive) {
        // Do online code
        return 1;
    }
    return 0;
}

int ShowPromoPopup(int a1, void *a2)
{
#if RSDK_DEBUG
    printLog("Attempting to show promo popup (%d) (%p)", a1, a2);
#endif
    if (Engine.onlineActive) {
        //Do online code
        return 1;
    }
    return 0;
}
