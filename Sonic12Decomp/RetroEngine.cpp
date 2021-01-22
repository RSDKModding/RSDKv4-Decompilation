#include "RetroEngine.hpp"

bool usingCWD = false;

RetroEngine Engine = RetroEngine();

inline int getLowerRate(int intendRate, int targetRate)
{
    int result   = 0;
    int valStore = 0;

    result = targetRate;
    if (intendRate) {
        do {
            valStore   = result % intendRate;
            result     = intendRate;
            intendRate = valStore;
        } while (valStore);
    }
    return result;
}


bool processEvents()
{
#if RETRO_USING_SDL
    while (SDL_PollEvent(&Engine.sdlEvents)) {
        // Main Events
        switch (Engine.sdlEvents.type) {
            case SDL_WINDOWEVENT:
                switch (Engine.sdlEvents.window.event) {
                    case SDL_WINDOWEVENT_MAXIMIZED: {
                        SDL_RestoreWindow(Engine.window);
                        SDL_SetWindowFullscreen(Engine.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        Engine.isFullScreen = true;
                        break;
                    }
                    case SDL_WINDOWEVENT_CLOSE:
                        return false;
                        break;
                }
                break;
            case SDL_CONTROLLERDEVICEADDED: controllerInit(SDL_NumJoysticks() - 1); break;
            case SDL_CONTROLLERDEVICEREMOVED: controllerClose(SDL_NumJoysticks() - 1); break;
            case SDL_WINDOWEVENT_CLOSE:
                if (Engine.window) {
                    SDL_DestroyWindow(Engine.window);
                    Engine.window = NULL;
                }
                return false;
            case SDL_APP_WILLENTERBACKGROUND: /*Engine.Callback(CALLBACK_ENTERBG);*/ break;
            case SDL_APP_WILLENTERFOREGROUND: /*Engine.Callback(CALLBACK_ENTERFG);*/ break;
            case SDL_APP_TERMINATING:
                break;
#if RETRO_PLATFORM != RETRO_VITA
            case SDL_MOUSEMOTION:
                if (SDL_GetNumTouchFingers(SDL_GetTouchDevice(1)) <= 0) { // Touch always takes priority over mouse
                    SDL_GetMouseState(&touchX[0], &touchY[0]);
                    touchX[0] /= Engine.windowScale;
                    touchY[0] /= Engine.windowScale;
                    touches = 1;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (SDL_GetNumTouchFingers(SDL_GetTouchDevice(1)) <= 0) { // Touch always takes priority over mouse
                    switch (Engine.sdlEvents.button.button) {
                        case SDL_BUTTON_LEFT: touchDown[0] = 1; break;
                    }
                    touches = 1;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (SDL_GetNumTouchFingers(SDL_GetTouchDevice(1)) <= 0) { // Touch always takes priority over mouse
                    switch (Engine.sdlEvents.button.button) {
                        case SDL_BUTTON_LEFT: touchDown[0] = 0; break;
                    }
                    touches = 1;
                }
                break;
            case SDL_FINGERMOTION:
                touches = SDL_GetNumTouchFingers(SDL_GetTouchDevice(1));
                for (int i = 0; i < touches; i++) {
                    touchDown[i]       = true;
                    SDL_Finger *finger = SDL_GetTouchFinger(SDL_GetTouchDevice(1), i);
                    touchX[i]          = (finger->x * SCREEN_XSIZE * Engine.windowScale) / Engine.windowScale;

                    touchY[i] = (finger->y * SCREEN_YSIZE * Engine.windowScale) / Engine.windowScale;
                }
                break;
            case SDL_FINGERDOWN:
                touches = SDL_GetNumTouchFingers(SDL_GetTouchDevice(1));
                for (int i = 0; i < touches; i++) {
                    touchDown[i]       = true;
                    SDL_Finger *finger = SDL_GetTouchFinger(SDL_GetTouchDevice(1), i);
                    touchX[i]          = (finger->x * SCREEN_XSIZE * Engine.windowScale) / Engine.windowScale;

                    touchY[i] = (finger->y * SCREEN_YSIZE * Engine.windowScale) / Engine.windowScale;
                }
                break;
            case SDL_FINGERUP: touches = SDL_GetNumTouchFingers(SDL_GetTouchDevice(1)); break;
#endif
            case SDL_KEYDOWN:
                switch (Engine.sdlEvents.key.keysym.sym) {
                    default: break;
                    case SDLK_ESCAPE:
                        if (Engine.devMenu)
                            Engine.gameMode = ENGINE_INITDEVMENU;
                        break;
                    case SDLK_F4:
                        Engine.isFullScreen ^= 1;
                        if (Engine.isFullScreen) {
                            SDL_RestoreWindow(Engine.window);
                            SDL_SetWindowFullscreen(Engine.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        }
                        else {
                            SDL_SetWindowFullscreen(Engine.window, false);
                            SDL_SetWindowSize(Engine.window, SCREEN_XSIZE * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale);
                            SDL_SetWindowPosition(Engine.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                            SDL_RestoreWindow(Engine.window);
                        }
                        break;
                    case SDLK_F1:
                        if (Engine.devMenu) {
                            activeStageList   = 0;
                            stageListPosition = 0;
                            stageMode         = STAGEMODE_LOAD;
                            Engine.gameMode   = ENGINE_MAINGAME;
                        }
                        break;
                    case SDLK_F2:
                        if (Engine.devMenu) {
                            stageListPosition--;
                            if (stageListPosition < 0) {
                                activeStageList--;

                                if (activeStageList < 0) {
                                    activeStageList = 3;
                                }
                                stageListPosition = stageListCount[activeStageList] - 1;
                            }
                            stageMode       = STAGEMODE_LOAD;
                            Engine.gameMode = ENGINE_MAINGAME;
                        }
                        break;
                    case SDLK_F3:
                        if (Engine.devMenu) {
                            stageListPosition++;
                            if (stageListPosition >= stageListCount[activeStageList]) {
                                activeStageList++;

                                stageListPosition = 0;

                                if (activeStageList >= 4) {
                                    activeStageList = 0;
                                }
                            }
                            stageMode       = STAGEMODE_LOAD;
                            Engine.gameMode = ENGINE_MAINGAME;
                        }
                        break;
                    case SDLK_F10:
                        if (Engine.devMenu)
                            Engine.showPaletteOverlay ^= 1;
                        break;
                    case SDLK_BACKSPACE:
                        if (Engine.devMenu)
                            Engine.gameSpeed = Engine.fastForwardSpeed;
                        break;
#if RETRO_PLATFORM == RETRO_OSX
                    case SDLK_F6:
                        if (Engine.masterPaused)
                            Engine.frameStep = true;
                        break;
                    case SDLK_F7:
                        if (Engine.devMenu)
                            Engine.masterPaused ^= 1;
                        break;
#else
                    case SDLK_F11:
                        if (Engine.masterPaused)
                            Engine.frameStep = true;
                        break;
                    case SDLK_F12:
                        if (Engine.devMenu)
                            Engine.masterPaused ^= 1;
                        break;
#endif
                }
                break;
            case SDL_KEYUP:
                switch (Engine.sdlEvents.key.keysym.sym) {
                    default:
                        break;
                    case SDLK_BACKSPACE: Engine.gameSpeed = 1; break;
                }
                break;
            case SDL_QUIT: return false;
        }
    }
#endif
    return true;
}

#if RETRO_USE_NETWORKING && RSDK_DEBUG
#include <string>
#endif
void RetroEngine::Init()
{
    CalculateTrigAngles();
    GenerateBlendLookupTable();

    CheckRSDKFile(BASE_PATH "Data.rsdk");

    InitUserdata();
    InitNativeObjectSystem();

#if RETRO_USE_NETWORKING
    buildNetworkIndex();
#if RSDK_DEBUG
//here lies the networking test.
//check Network.cpp for the network code i've written so far
//it should be commented enough

//TO TEST: build client in x86, server in x64
//please PLEASE, if you solve networking, make a PR and i (RMG) will gladly review it and
//put the server code in it's correct spot (2P versus menu)
#if WIN32
    {
        ushort port = 300;
        playerListPos = 2;
        std::string code = generateCode(port, 6, 2);
        CodeData c = parseCode("put the code generated by the server here");
        initClient(c);
    }
#else
    {
        ushort port      = 25535;
        playerListPos    = 0;
        std::string code = generateCode(port, 8, 1);
        initServer(port);
    }
#endif // WIN32
#endif // RSDK_DEBUG
#endif // RETRO_USE_NETWORKING

    gameMode = ENGINE_MAINGAME;
    running  = false;
    if (LoadGameConfig("Data/Game/GameConfig.bin")) {
        if (InitRenderDevice()) {
            if (InitAudioPlayback()) {
                InitFirstStage();
                ClearScriptData();
                initialised = true;
                running     = true;
            }
        }
    }

    // Calculate Skip frame
    int lower        = getLowerRate(targetRefreshRate, refreshRate);
    renderFrameIndex = targetRefreshRate / lower;
    skipFrameIndex   = refreshRate / lower;

    gameType = GAME_UNKNOWN;
    if (strstr(gameWindowText, "Sonic 1")) {
        gameType = GAME_SONIC1;
    }
    if (strstr(gameWindowText, "Sonic 2")) {
        gameType = GAME_SONIC2;
    }

    
    ReadSaveRAMData();
    if (saveRAM[0x100] != Engine.gameType) {
        saveRAM[0x100] = Engine.gameType;
    }
    else {
        if (Engine.gameType == GAME_SONIC1) {
            SetGlobalVariableByName("options.spindash", saveRAM[0x101]);
            SetGlobalVariableByName("options.speedCap", saveRAM[0x102]);
            SetGlobalVariableByName("options.airSpeedCap", saveRAM[0x103]);
            SetGlobalVariableByName("options.spikeBehavior", saveRAM[0x104]);
            SetGlobalVariableByName("options.shieldType", saveRAM[0x105]);
        }
        else {
            SetGlobalVariableByName("options.airSpeedCap", saveRAM[0x101]);
            SetGlobalVariableByName("options.tailsFlight", saveRAM[0x102]);
            SetGlobalVariableByName("options.superTails", saveRAM[0x103]);
            SetGlobalVariableByName("options.spikeBehavior", saveRAM[0x104]);
            SetGlobalVariableByName("options.shieldType", saveRAM[0x105]);
        }
    }

    if (Engine.gameType == GAME_SONIC1) {
        StrCopy(achievements[5].name, "Ring King");
        StrCopy(achievements[0].name, "Blast Processing");
        StrCopy(achievements[1].name, "Ramp Ring Acrobatics");
        StrCopy(achievements[2].name, "Secret of Marble Zone");
        StrCopy(achievements[3].name, "Block Buster");
        StrCopy(achievements[4].name, "Secret of Labyrinth Zone");
        StrCopy(achievements[6].name, "Flawless Pursuit");
        StrCopy(achievements[7].name, "Bombs Away");
        StrCopy(achievements[9].name, "Hidden Transporter");
        StrCopy(achievements[8].name, "Chaos Connoisseur");
        StrCopy(achievements[10].name, "One For the Road");
        StrCopy(achievements[11].name, "Beat The Clock");
    }
    else if (Engine.gameType == GAME_SONIC2) {
        StrCopy(achievements[0].name, "Quick Run");
        StrCopy(achievements[1].name, "100% Chemical Free");
        StrCopy(achievements[2].name, "Early Bird Special");
        StrCopy(achievements[3].name, "Superstar");
        StrCopy(achievements[4].name, "Hit it Big");
        StrCopy(achievements[5].name, "Bop Non-stop");
        StrCopy(achievements[6].name, "Perfectionist");
        StrCopy(achievements[7].name, "A Secret Revealed");
        StrCopy(achievements[8].name, "Head 2 Head");
        StrCopy(achievements[9].name, "Metropolis Master");
        StrCopy(achievements[10].name, "Scrambled Egg");
        StrCopy(achievements[11].name, "Beat the Clock");
    }
    initStartMenu(0);
}

void RetroEngine::Run()
{
    uint frameStart, frameEnd = SDL_GetTicks();
    float frameDelta = 0.0f;

    while (running) {
        frameStart = SDL_GetTicks();
        frameDelta = frameStart - frameEnd;

        if (frameDelta < 1000.0f / (float)refreshRate)
            SDL_Delay(1000.0f / (float)refreshRate - frameDelta);

        frameEnd = SDL_GetTicks();

        running = processEvents();
        for (int s = 0; s < gameSpeed; ++s) {
            ProcessInput();

            if (!masterPaused || frameStep) {
                ProcessNativeObjects();
                RenderRenderDevice();
                frameStep = false;
            }
        }
    }

    ReleaseAudioDevice();
    ReleaseRenderDevice();
    writeSettings();

#if RETRO_USING_SDL
    SDL_Quit();
#endif
}

bool RetroEngine::LoadGameConfig(const char *filePath)
{
    FileInfo info;
    int fileBuffer  = 0;
    int fileBuffer2 = 0;
    char strBuffer[0x40];

    bool loaded = LoadFile(filePath, &info);
    if (loaded) {
        FileRead(&fileBuffer, 1);
        FileRead(gameWindowText, fileBuffer);
        gameWindowText[fileBuffer] = 0;

        FileRead(&fileBuffer, 1);
        FileRead(gameDescriptionText, fileBuffer);
        gameDescriptionText[fileBuffer] = 0;

        byte buf[3];
        for (int c = 0; c < 0x60; ++c) {
            FileRead(buf, 3);
            SetPaletteEntry(-1, c, buf[0], buf[1], buf[2]);
        }

        // Read Obect Names
        int objectCount = 0;
        FileRead(&objectCount, 1);
        for (int o = 0; o < objectCount; ++o) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
        }

        // Read Script Paths
        for (int s = 0; s < objectCount; ++s) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
        }

        int varCount = 0;
        FileRead(&varCount, 1);
        globalVariablesCount = varCount;
        for (int v = 0; v < varCount; ++v) {
            // Read Variable Name
            FileRead(&fileBuffer, 1);
            FileRead(&globalVariableNames[v], fileBuffer);
            globalVariableNames[v][fileBuffer] = 0;

            // Read Variable Value
            FileRead(&fileBuffer2, 1);
            globalVariables[v] = fileBuffer2 << 0;
            FileRead(&fileBuffer2, 1);
            globalVariables[v] += fileBuffer2 << 8;
            FileRead(&fileBuffer2, 1);
            globalVariables[v] += fileBuffer2 << 16;
            FileRead(&fileBuffer2, 1);
            globalVariables[v] += fileBuffer2 << 24;
        }

        // Read SFX
        int globalSFXCount = 0;
        FileRead(&globalSFXCount, 1);
        for (int s = 0; s < globalSFXCount; ++s) { // SFX Names
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
            strBuffer[fileBuffer] = 0;
        }
        for (int s = 0; s < globalSFXCount; ++s) { // SFX Paths
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
            strBuffer[fileBuffer] = 0;
        }

        // Read Player Names
        int playerCount = 0;
        FileRead(&playerCount, 1);
        for (int p = 0; p < playerCount; ++p) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
        }

        for (int c = 0; c < 4; ++c) {
            // Special Stages are stored as cat 2 in file, but cat 3 in game :(
            int cat = c;
            if (c == 2)
                cat = 3;
            else if (c == 3)
                cat = 2;
            stageListCount[cat] = 0;
            FileRead(&stageListCount[cat], 1);
            for (int s = 0; s < stageListCount[cat]; ++s) {

                // Read Stage Folder
                FileRead(&fileBuffer, 1);
                FileRead(&stageList[cat][s].folder, fileBuffer);
                stageList[cat][s].folder[fileBuffer] = 0;

                // Read Stage ID
                FileRead(&fileBuffer, 1);
                FileRead(&stageList[cat][s].id, fileBuffer);
                stageList[cat][s].id[fileBuffer] = 0;

                // Read Stage Name
                FileRead(&fileBuffer, 1);
                FileRead(&stageList[cat][s].name, fileBuffer);
                stageList[cat][s].name[fileBuffer] = 0;

                // Read Stage Mode
                FileRead(&fileBuffer, 1);
                stageList[cat][s].highlighted = fileBuffer;
            }
        }

        CloseFile();
    }

    
    //These need to be set every time its reloaded
    nativeFunctionCount = 0;
    AddNativeFunction("SetAchievement", SetAchievement);
    AddNativeFunction("SetLeaderboard", SetLeaderboard);
    AddNativeFunction("Connect2PVS", Connect2PVS);
    AddNativeFunction("Disconnect2PVS", Disconnect2PVS);
    AddNativeFunction("SendEntity", SendEntity);
    AddNativeFunction("SendValue", SendValue);
    AddNativeFunction("ReceiveEntity", ReceiveEntity);
    AddNativeFunction("ReceiveValue", ReceiveValue);
    AddNativeFunction("TransmitGlobal", TransmitGlobal);
    AddNativeFunction("ShowPromoPopup", ShowPromoPopup);

    return loaded;
}

void RetroEngine::Callback(int callbackID)
{
    switch (callbackID) {
        default:
#if RSDK_DEBUG
            printLog("Callback: Unknown (%d)", callbackID);
#endif
            break;
    }
}
