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

MultiplayerData multiplayerDataIN  = MultiplayerData();
MultiplayerData multiplayerDataOUT = MultiplayerData();
int matchValueData[0x100];
int matchValueReadPos  = 0;
int matchValueWritePos = 0;

int sendDataMethod = 0;
int sendCounter    = 0;

void InitUserdata()
{
    // userdata files are loaded from this directory
    sprintf(gamePath, "%s", BASE_PATH);

    char buffer[0x100];
#if RETRO_PLATFORM == RETRO_OSX
    if (!usingCWD)
        sprintf(buffer, "%s/settings.ini", getResourcesPath());
    else
        sprintf(buffer, "%ssettings.ini", gamePath);
#else
    sprintf(buffer, BASE_PATH "settings.ini");
#endif
    FileIO *file = fOpen(buffer, "rb");
    if (!file) {
        IniParser ini;

        ini.SetBool("Dev", "DevMenu", Engine.devMenu = false);
        ini.SetInteger("Dev", "StartingCategory", Engine.startList = 0);
        ini.SetInteger("Dev", "StartingScene", Engine.startStage = 0);
        ini.SetInteger("Dev", "FastForwardSpeed", Engine.fastForwardSpeed = 8);
        ini.SetBool("Dev", "UseHQModes", Engine.useHQModes = true);

        ini.SetBool("Game", "Language", Engine.language = RETRO_EN);

        ini.SetBool("Window", "FullScreen", Engine.startFullScreen = DEFAULT_FULLSCREEN);
        ini.SetBool("Window", "Borderless", Engine.borderless = false);
        ini.SetBool("Window", "VSync", Engine.vsync = false);
        ini.SetInteger("Window", "WindowScale", Engine.windowScale = 2);
        ini.SetInteger("Window", "ScreenWidth", SCREEN_XSIZE = DEFAULT_SCREEN_XSIZE);
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

        StrCopy(Engine.dataFile, "Data.rsdk");
        ini.SetString("Dev", "DataFile", Engine.dataFile);

        ini.Write(BASE_PATH "settings.ini");
    }
    else {
        fClose(file);
        IniParser ini(BASE_PATH "settings.ini");

        if (!ini.GetBool("Dev", "DevMenu", &Engine.devMenu))
            Engine.devMenu = false;
        if (!ini.GetInteger("Dev", "StartingCategory", &Engine.startList))
            Engine.startList = 0;
        if (!ini.GetInteger("Dev", "StartingScene", &Engine.startStage))
            Engine.startStage = 0;
        if (!ini.GetInteger("Dev", "FastForwardSpeed", &Engine.fastForwardSpeed))
            Engine.fastForwardSpeed = 8;
        if (!ini.GetBool("Dev", "UseHQModes", &Engine.useHQModes))
            Engine.useHQModes = true;

        if (!ini.GetString("Dev", "DataFile", Engine.dataFile))
            StrCopy(Engine.dataFile, "Data.rsdk");

        if (!ini.GetInteger("Game", "Language", &Engine.language))
            Engine.language = RETRO_EN;

        if (!ini.GetBool("Window", "FullScreen", &Engine.startFullScreen))
            Engine.startFullScreen = DEFAULT_FULLSCREEN;
        if (!ini.GetBool("Window", "Borderless", &Engine.borderless))
            Engine.borderless = false;
        if (!ini.GetBool("Window", "VSync", &Engine.vsync))
            Engine.vsync = false;
        if (!ini.GetInteger("Window", "WindowScale", &Engine.windowScale))
            Engine.windowScale = 2;
        if (!ini.GetInteger("Window", "ScreenWidth", &SCREEN_XSIZE))
            SCREEN_XSIZE = DEFAULT_SCREEN_XSIZE;
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

void writeSettings()
{
    IniParser ini;

    ini.SetComment("Dev", "DevMenuComment", "Enable this flag to activate dev menu via the ESC key");
    ini.SetBool("Dev", "DevMenu", Engine.devMenu);
    ini.SetComment("Dev", "SCComment", "Sets the starting category ID");
    ini.SetBool("Dev", "StartingCategory", Engine.startList);
    ini.SetComment("Dev", "SSComment", "Sets the starting scene ID");
    ini.SetBool("Dev", "StartingScene", Engine.startStage);
    ini.SetComment("Dev", "FFComment", "Determines how fast the game will be when fastforwarding is active");
    ini.SetInteger("Dev", "FastForwardSpeed", Engine.fastForwardSpeed);
    ini.SetComment(
        "Dev", "UseHQComment",
        "Determines if applicable rendering modes (such as 3D floor from special stages) will render in \"High Quality\" mode or standard mode");
    ini.SetBool("Dev", "UseHQModes", Engine.useHQModes);

    ini.SetComment("Game", "LangComment",
                   "Sets the game language (0 = EN, 1 = FR, 2 = IT, 3 = DE, 4 = ES, 5 = JP, 6 = PT, 7 = RU, 8 = KO, 9 = ZH, 10 = ZS)");
    ini.SetInteger("Game", "Language", Engine.language);

    ini.SetComment("Window", "FSComment", "Determines if the window will be fullscreen or not");
    ini.SetBool("Window", "FullScreen", Engine.startFullScreen);
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

int SetAchievement(int achievementID, void *achDone)
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

int Connect2PVS(int gameLength, void *itemMode)
{
#if RSDK_DEBUG
    printLog("Attempting to connect to 2P game (%d) (%p)", gameLength, itemMode);
#endif
    multiplayerDataIN.type = 0;
    matchValueData[0]      = 0;
    matchValueData[1]      = 0;
    matchValueReadPos      = 0;
    matchValueWritePos     = 0;
    Engine.gameMode        = ENGINE_CONNECT2PVS;
    PauseSound();

    // actual connection code
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
#if RETRO_USE_NETWORKING
        sendData(0, sizeof(multiplayerDataOUT), &multiplayerDataOUT);
#endif
        return 1;
    }
    return 0;
}
int SendEntity(int dataSlot, void *entityID)
{
#if RSDK_DEBUG
    printLog("Attempting to send entity (%d) (%p)", dataSlot, entityID);
#endif
    if (!sendCounter) {
        multiplayerDataOUT.type = 1;
        memcpy(multiplayerDataOUT.data, &objectEntityList[static_cast<int>(reinterpret_cast<intptr_t>(entityID))], sizeof(Entity));
        if (Engine.onlineActive) {
#if RETRO_USE_NETWORKING
            sendData(0, sizeof(multiplayerDataOUT), &multiplayerDataOUT);
#endif
            return 1;
        }
    }
    sendCounter += 1;
    sendCounter %= 2;
    return 0;
}
int SendValue(int a1, void *value)
{
#if RSDK_DEBUG
    printLog("Attempting to send value (%d) (%p)", a1, value);
#endif
    multiplayerDataOUT.type    = 0;
    multiplayerDataOUT.data[0] = static_cast<int>(reinterpret_cast<intptr_t>(value));
    if (Engine.onlineActive && sendDataMethod) {
#if RETRO_USE_NETWORKING
        sendData(0, sizeof(multiplayerDataOUT), &multiplayerDataOUT);
#endif
        return 1;
    }
    return 0;
}
int ReceiveEntity(int dataSlotID, void *entityID)
{
#if RSDK_DEBUG
    printLog("Attempting to receive entity (%d) (%p)", dataSlotID, entityID);
#endif
    if (Engine.onlineActive) {
        // Do online code
        int entitySlot = static_cast<int>(reinterpret_cast<intptr_t>(entityID));
        if (dataSlotID == 1) {
            if (multiplayerDataIN.type == 1) {
                memcpy(&objectEntityList[entitySlot], multiplayerDataIN.data, sizeof(Entity));
            }
            multiplayerDataIN.type = 0;
        }
        else {
            memcpy(&objectEntityList[entitySlot], multiplayerDataIN.data, sizeof(Entity));
        }
    }
    return 0;
}
int ReceiveValue(int dataSlot, void *value)
{
#if RSDK_DEBUG
    printLog("Attempting to receive value (%d) (%p)", dataSlot, value);
#endif
    if (Engine.onlineActive) {
        // Do online code
        int *val = (int *)value;

        if (dataSlot == 1) {
            if (matchValueReadPos != matchValueWritePos) {
                *val = matchValueData[matchValueReadPos];
                matchValueReadPos++;
            }
        }
        else {
            *val = matchValueData[matchValueReadPos];
        }
        return 1;
    }
    return 0;
}
int TransmitGlobal(int globalValue, void *globalName)
{
#if RSDK_DEBUG
    printLog("Attempting to transmit global (%s) (%d)", (char *)globalName, globalValue);
#endif
    multiplayerDataOUT.type    = 2;
    multiplayerDataOUT.data[0] = GetGlobalVariableID((char *)globalName);
    multiplayerDataOUT.data[1] = globalValue;
    if (Engine.onlineActive && sendDataMethod) {
#if RETRO_USE_NETWORKING
        sendData(0, sizeof(multiplayerDataOUT), &multiplayerDataOUT);
#endif
        return 1;
    }
    return 0;
}

void receive2PVSData(MultiplayerData *data)
{
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
    matchValueData[matchValueWritePos++] = code;
    ResumeSound();
    Engine.gameMode = ENGINE_MAINGAME;
}

int ShowPromoPopup(int a1, void *a2)
{
#if RSDK_DEBUG
    printLog("Attempting to show promo popup (%d) (%p)", a1, a2);
#endif
    if (Engine.onlineActive) {
        // Do online code
        return 1;
    }
    return 0;
}
