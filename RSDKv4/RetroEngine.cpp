#include "RetroEngine.hpp"

#if !RETRO_USE_ORIGINAL_CODE
bool usingCWD        = false;
bool engineDebugMode = false;
#endif

#if RETRO_PLATFORM == RETRO_ANDROID
#include <unistd.h>
#endif

RetroEngine Engine = RetroEngine();

#if !RETRO_USE_ORIGINAL_CODE
inline int GetLowerRate(int intendRate, int targetRate)
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
#endif

bool ProcessEvents()
{
#if !RETRO_USE_ORIGINAL_CODE
#if RETRO_USING_SDL1 || RETRO_USING_SDL2
    while (SDL_PollEvent(&Engine.sdlEvents)) {
        // Main Events
        switch (Engine.sdlEvents.type) {
#if RETRO_USING_SDL2
            case SDL_WINDOWEVENT:
                switch (Engine.sdlEvents.window.event) {
                    case SDL_WINDOWEVENT_MAXIMIZED: {
                        SDL_RestoreWindow(Engine.window);
                        SDL_SetWindowFullscreen(Engine.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        SDL_ShowCursor(SDL_FALSE);
                        Engine.isFullScreen = true;
                        break;
                    }
                    case SDL_WINDOWEVENT_CLOSE: return false;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        if (Engine.gameMode == ENGINE_MAINGAME && !(disableFocusPause & 1))
                            Engine.gameMode = ENGINE_INITPAUSE;
#if RETRO_REV00
                        if (!(disableFocusPause & 1))
                            Engine.message = MESSAGE_LOSTFOCUS;
#endif
                        Engine.hasFocus = false;
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED: Engine.hasFocus = true; break;
                }
                break;
            case SDL_CONTROLLERDEVICEADDED: controllerInit(Engine.sdlEvents.cdevice.which); break;
            case SDL_CONTROLLERDEVICEREMOVED: controllerClose(Engine.sdlEvents.cdevice.which); break;
            case SDL_APP_WILLENTERBACKGROUND:
                if (Engine.gameMode == ENGINE_MAINGAME && !(disableFocusPause & 1))
                    Engine.gameMode = ENGINE_INITPAUSE;
#if RETRO_REV00
                if (!(disableFocusPause & 1))
                    Engine.message = MESSAGE_LOSTFOCUS;
#endif
                Engine.hasFocus = false;
                break;
            case SDL_APP_WILLENTERFOREGROUND: Engine.hasFocus = true; break;
            case SDL_APP_TERMINATING: return false;
#endif

#if RETRO_USING_SDL2 && defined(RETRO_USING_MOUSE)
            case SDL_MOUSEMOTION:
                if (touches <= 1) { // Touch always takes priority over mouse
                    uint state = SDL_GetMouseState(&touchX[0], &touchY[0]);

                    touchXF[0] = (((touchX[0] - displaySettings.offsetX) / (float)displaySettings.width) * SCREEN_XSIZE_F) - SCREEN_CENTERX_F;
                    touchYF[0] = -(((touchY[0] / (float)displaySettings.height) * SCREEN_YSIZE_F) - SCREEN_CENTERY_F);
                    touchX[0]  = ((touchX[0] - displaySettings.offsetX) / (float)displaySettings.width) * SCREEN_XSIZE;
                    touchY[0]  = (touchY[0] / (float)displaySettings.height) * SCREEN_YSIZE;

                    touchDown[0] = state & SDL_BUTTON_LMASK;
                    if (touchDown[0])
                        touches = 1;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (touches <= 1) { // Touch always takes priority over mouse
                    switch (Engine.sdlEvents.button.button) {
                        case SDL_BUTTON_LEFT: touchDown[0] = true; break;
                    }
                    touches = 1;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (touches <= 1) { // Touch always takes priority over mouse
                    switch (Engine.sdlEvents.button.button) {
                        case SDL_BUTTON_LEFT: touchDown[0] = false; break;
                    }
                    touches = 0;
                }
                break;
#endif

#if defined(RETRO_USING_TOUCH) && RETRO_USING_SDL2
            case SDL_FINGERMOTION:
            case SDL_FINGERDOWN:
            case SDL_FINGERUP: {
                int count = SDL_GetNumTouchFingers(Engine.sdlEvents.tfinger.touchId);
                touches   = 0;
                for (int i = 0; i < count; i++) {
                    SDL_Finger *finger = SDL_GetTouchFinger(Engine.sdlEvents.tfinger.touchId, i);
                    if (finger) {
                        touchDown[touches] = true;
                        touchX[touches]    = finger->x * SCREEN_XSIZE;
                        touchY[touches]    = finger->y * SCREEN_YSIZE;
                        touchXF[touches]   = (finger->x * SCREEN_XSIZE_F) - SCREEN_CENTERX_F;
                        touchYF[touches]   = -((finger->y * SCREEN_YSIZE_F) - SCREEN_CENTERY_F);

                        touchX[touches] -= displaySettings.offsetX;
                        touchXF[touches] -= displaySettings.offsetX;

                        touches++;
                    }
                }
                break;
            }
#endif //! RETRO_USING_SDL2

            case SDL_KEYDOWN:
                switch (Engine.sdlEvents.key.keysym.sym) {
                    default: break;
                    case SDLK_ESCAPE:
                        if (Engine.devMenu) {
#if RETRO_USE_MOD_LOADER
                            // hacky patch because people can escape
                            if (Engine.gameMode == ENGINE_DEVMENU && stageMode == DEVMENU_MODMENU)
                                RefreshEngine();
#endif
                            ClearNativeObjects();
                            CREATE_ENTITY(RetroGameLoop);
                            if (Engine.gameDeviceType == RETRO_MOBILE)
                                CREATE_ENTITY(VirtualDPad);
                            Engine.gameMode = ENGINE_INITDEVMENU;
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
                            SetGlobalVariableByName("lampPostID", 0); // For S1
                            SetGlobalVariableByName("starPostID", 0); // For S2
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
                            SetGlobalVariableByName("lampPostID", 0); // For S1
                            SetGlobalVariableByName("starPostID", 0); // For S2
                        }
                        break;

                    case SDLK_F4:
                        Engine.isFullScreen ^= 1;
                        SetFullScreen(Engine.isFullScreen);
                        break;

                    case SDLK_F5:
                        if (Engine.devMenu) {
                            currentStageFolder[0] = 0; // reload all assets & scripts
                            stageMode             = STAGEMODE_LOAD;
                        }
                        break;

                    case SDLK_F8:
                        if (Engine.devMenu)
                            showHitboxes ^= 2;
                        break;

                    case SDLK_F9:
                        if (Engine.devMenu)
                            showHitboxes ^= 1;
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
                    case SDLK_INSERT:
                        if (Engine.masterPaused)
                            Engine.frameStep = true;
                        break;

                    case SDLK_F12:
                    case SDLK_PAUSE:
                        if (Engine.devMenu)
                            Engine.masterPaused ^= 1;
                        break;
#endif
                }

#if RETRO_USING_SDL1
                keyState[Engine.sdlEvents.key.keysym.sym] = 1;
#endif
                break;
            case SDL_KEYUP:
                switch (Engine.sdlEvents.key.keysym.sym) {
                    default: break;
                    case SDLK_BACKSPACE: Engine.gameSpeed = 1; break;
                }
#if RETRO_USING_SDL1
                keyState[Engine.sdlEvents.key.keysym.sym] = 0;
#endif
                break;
            case SDL_QUIT: return false;
        }
    }
#endif
#endif
    return true;
}

void RetroEngine::Init()
{
    CalculateTrigAngles();
    GenerateBlendLookupTable();

    CloseRSDKContainers(); // Clears files

    Engine.usingDataFile = false;
    Engine.usingBytecode = false;

#if !RETRO_USE_ORIGINAL_CODE
    InitUserdata();
#if RETRO_USE_MOD_LOADER
    InitMods();
#endif
#if RETRO_USE_NETWORKING
    InitNetwork();
#endif

    char dest[0x200];
#if RETRO_PLATFORM == RETRO_UWP
    static char resourcePath[256] = { 0 };

    if (strlen(resourcePath) == 0) {
        auto folder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
        auto path   = to_string(folder.Path());

        std::copy(path.begin(), path.end(), resourcePath);
    }

    strcpy(dest, resourcePath);
    strcat(dest, "\\");
    strcat(dest, Engine.dataFile);
#elif RETRO_PLATFORM == RETRO_ANDROID
    StrCopy(dest, gamePath);
    StrAdd(dest, Engine.dataFile[0]);
    disableFocusPause = 0; // focus pause is ALWAYS enabled.
#else

    StrCopy(dest, BASE_PATH);
    StrAdd(dest, Engine.dataFile[0]);
#endif
    CheckRSDKFile(dest);
#else
    CheckRSDKFile("Data.rsdk");
#endif

#if !RETRO_USE_ORIGINAL_CODE
    for (int i = 1; i < RETRO_PACK_COUNT; ++i) {
        if (!StrComp(Engine.dataFile[i], "")) {
            StrCopy(dest, BASE_PATH);
            StrAdd(dest, Engine.dataFile[i]);
            CheckRSDKFile(dest);
        }
    }
#endif

    gameMode = ENGINE_MAINGAME;
    running  = false;
#if !RETRO_USE_ORIGINAL_CODE
    bool skipStart = skipStartMenu;
#endif
    SaveGame *saveGame = (SaveGame *)saveRAM;

    if (LoadGameConfig("Data/Game/GameConfig.bin")) {
        if (InitRenderDevice()) {
            if (InitAudioPlayback()) {
                InitFirstStage();
                ClearScriptData();
                initialised = true;
                running     = true;

#if !RETRO_USE_ORIGINAL_CODE
                if ((startList_Game != 0xFF && startList_Game) || (startStage_Game != 0xFF && startStage_Game) || startPlayer != 0xFF) {
                    skipStart = true;
                    InitStartingStage(startList_Game == 0xFF ? STAGELIST_PRESENTATION : startList_Game, startStage_Game == 0xFF ? 0 : startStage_Game,
                                      startPlayer == 0xFF ? 0 : startPlayer);
                }
                else if (startSave != 0xFF && startSave < 4) {
                    if (startSave == 0) {
                        SetGlobalVariableByName("options.saveSlot", 0);
                        SetGlobalVariableByName("options.gameMode", 0);

                        SetGlobalVariableByName("options.stageSelectFlag", 0);
                        SetGlobalVariableByName("player.lives", 3);
                        SetGlobalVariableByName("player.score", 0);
                        SetGlobalVariableByName("player.scoreBonus", 50000);
                        SetGlobalVariableByName("specialStage.emeralds", 0);
                        SetGlobalVariableByName("specialStage.listPos", 0);
                        SetGlobalVariableByName("stage.player2Enabled", 0);
                        SetGlobalVariableByName("lampPostID", 0); // For S1
                        SetGlobalVariableByName("starPostID", 0); // For S2
                        SetGlobalVariableByName("options.vsMode", 0);

                        SetGlobalVariableByName("specialStage.nextZone", 0);
                        InitStartingStage(STAGELIST_REGULAR, 0, 0);
                    }
                    else {
                        SetGlobalVariableByName("options.saveSlot", startSave);
                        SetGlobalVariableByName("options.gameMode", 1);
                        int slot = (startSave - 1) << 3;

                        SetGlobalVariableByName("options.stageSelectFlag", false);
                        SetGlobalVariableByName("player.lives", saveGame->files[slot].lives);
                        SetGlobalVariableByName("player.score", saveGame->files[slot].score);
                        SetGlobalVariableByName("player.scoreBonus", saveGame->files[slot].scoreBonus);
                        SetGlobalVariableByName("specialStage.emeralds", saveGame->files[slot].emeralds);
                        SetGlobalVariableByName("specialStage.listPos", saveGame->files[slot].specialStageID);
                        SetGlobalVariableByName("stage.player2Enabled", saveGame->files[slot].characterID == 3);
                        SetGlobalVariableByName("lampPostID", 0); // For S1
                        SetGlobalVariableByName("starPostID", 0); // For S2
                        SetGlobalVariableByName("options.vsMode", 0);

                        int nextStage = saveGame->files[slot].stageID;
                        if (nextStage >= 0x80) {
                            SetGlobalVariableByName("specialStage.nextZone", nextStage - 0x81);
                            InitStartingStage(STAGELIST_SPECIAL, saveGame->files[slot].specialStageID, saveGame->files[slot].characterID);
                        }
                        else if (nextStage >= 1) {
                            SetGlobalVariableByName("specialStage.nextZone", nextStage - 1);
                            InitStartingStage(STAGELIST_REGULAR, nextStage - 1, saveGame->files[slot].characterID);
                        }
                        else {
                            saveGame->files[slot].characterID    = 0;
                            saveGame->files[slot].lives          = 3;
                            saveGame->files[slot].score          = 0;
                            saveGame->files[slot].scoreBonus     = 50000;
                            saveGame->files[slot].stageID        = 0;
                            saveGame->files[slot].emeralds       = 0;
                            saveGame->files[slot].specialStageID = 0;
                            saveGame->files[slot].unused         = 0;

                            SetGlobalVariableByName("specialStage.nextZone", 0);
                            InitStartingStage(STAGELIST_REGULAR, 0, 0);
                        }
                    }
                    skipStart = true;
                }
#endif
            }
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    gameType = GAME_SONIC2;
#if RETRO_USE_MOD_LOADER
    if (strstr(gameWindowText, "Sonic 1") || forceSonic1) {
#else
    if (strstr(gameWindowText, "Sonic 1")) {
#endif
        gameType = GAME_SONIC1;
    }
#endif

#if !RETRO_USE_ORIGINAL_CODE
    bool skipStore = skipStartMenu;
    skipStartMenu  = skipStart;
    InitNativeObjectSystem();
    skipStartMenu = skipStore;
#else
    InitNativeObjectSystem();
#endif

#if !RETRO_USE_ORIGINAL_CODE
    // Calculate Skip frame
    int lower        = GetLowerRate(targetRefreshRate, refreshRate);
    renderFrameIndex = targetRefreshRate / lower;
    skipFrameIndex   = refreshRate / lower;

    ReadSaveRAMData();

    if (Engine.gameType == GAME_SONIC1) {
        AddAchievement("Ramp Ring Acrobatics",
                       "Without touching the ground,\rcollect all the rings in a\rtrapezoid formation in Green\rHill Zone Act 1");
        AddAchievement("Blast Processing", "Clear Green Hill Zone Act 1\rin under 30 seconds");
        AddAchievement("Secret of Marble Zone", "Travel though a secret\rroom in Marbale Zone Act 3");
        AddAchievement("Block Buster", "Break 16 blocks in a row\rwithout stopping");
        AddAchievement("Ring King", "Collect 200 Rings");
        AddAchievement("Secret of Labyrinth Zone", "Activate and ride the\rhidden platform in\rLabyrinth Zone Act 1");
        AddAchievement("Flawless Pursuit", "Clear the boss in Labyrinth\rZone without getting hurt");
        AddAchievement("Bombs Away", "Defeat the boss in Starlight Zone\rusing only the see-saw bombs");
        AddAchievement("Hidden Transporter", "Collect 50 Rings and take the hidden transporter path\rin Scrap Brain Act 2");
        AddAchievement("Chaos Connoisseur", "Collect all the chaos\remeralds");
        AddAchievement("One For the Road", "As a parting gift, land a\rfinal hit on Dr. Eggman's\rescaping Egg Mobile");
        AddAchievement("Beat The Clock", "Clear the Time Attack\rmode in less than 45\rminutes");
    }
    else if (Engine.gameType == GAME_SONIC2) {
        AddAchievement("Quick Run", "Complete Emerald Hill\rZone Act 1 in under 35\rseconds");
        AddAchievement("100% Chemical Free", "Complete Chemical Plant\rwithout going underwater");
        AddAchievement("Early Bird Special", "Collect all the Chaos\rEmeralds before Chemical\rPlant");
        AddAchievement("Superstar", "Complete any Act as\rSuper Sonic");
        AddAchievement("Hit it Big", "Get a jackpot on the Casino Night slot machines");
        AddAchievement("Bop Non-stop", "Defeat any boss in 8\rconsecutive hits without\rtouching he ground");
        AddAchievement("Perfectionist", "Get a Perfect Bonus by\rcollecting every Ring in an\rAct");
        AddAchievement("A Secret Revealed", "Find and complete\rHidden Palace Zone");
        AddAchievement("Head 2 Head", "Win a 2P Versus race\ragainst a friend");
        AddAchievement("Metropolis Master", "Complete Any Metropolis\rZone Act without getting\rhurt");
        AddAchievement("Scrambled Egg", "Defeat Dr. Eggman's Boss\rAttack mode in under 7\rminutes");
        AddAchievement("Beat the Clock", "Complete the Time Attack\rmode in less than 45\rminutes");
    }

    if (skipStart)
        Engine.gameMode = ENGINE_MAINGAME;
    else
        Engine.gameMode = ENGINE_WAIT;

    // "error message"
    if (!running) {
        char rootDir[0x80];
        char pathBuffer[0x80];

#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(rootDir, "%s/", getResourcesPath());
        else
            sprintf(rootDir, "%s", "");
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(rootDir, "%s/", gamePath);
#else
        sprintf(rootDir, "%s", "");
#endif
        sprintf(pathBuffer, "%s%s", rootDir, "usage.txt");

        FileIO *f;
        if ((f = fOpen(pathBuffer, "w")) == NULL) {
            PrintLog("ERROR: Couldn't open file '%s' for writing!", "usage.txt");
            return;
        }

        char textBuf[0x100];
        sprintf(textBuf, "RETRO ENGINE v4 USAGE:\n");
        fWrite(textBuf, 1, strlen(textBuf), f);

        sprintf(textBuf, "- Open the asset directory '%s' in a file browser\n", !rootDir[0] ? "./" : rootDir);
        fWrite(textBuf, 1, strlen(textBuf), f);

        sprintf(textBuf, "- Place a data pack named '%s' in the asset directory\n", Engine.dataFile[0]);
        fWrite(textBuf, 1, strlen(textBuf), f);

        sprintf(textBuf, "- OR extract a data pack and place the \"Data\" & \"Bytecode\" folders in the asset directory\n");
        fWrite(textBuf, 1, strlen(textBuf), f);

        fClose(f);
    }

#endif
}

void RetroEngine::Run()
{
    Engine.deltaTime = 0.0f;

    unsigned long long targetFreq = SDL_GetPerformanceFrequency() / Engine.refreshRate;
    unsigned long long curTicks   = 0;
    unsigned long long prevTicks  = 0;

    while (running) {
#if !RETRO_USE_ORIGINAL_CODE
        if (!vsync) {
            curTicks = SDL_GetPerformanceCounter();
            if (curTicks < prevTicks + targetFreq)
                continue;
            prevTicks = curTicks;
        }

        Engine.deltaTime = 1.0 / 60;
#endif
        running = ProcessEvents();

        // Focus Checks
        if (!(disableFocusPause & 2)) {
            if (!Engine.hasFocus) {
                if (!(Engine.focusState & 1))
                    Engine.focusState = PauseSound() ? 3 : 1;
            }
            else if (Engine.focusState) {
                if ((Engine.focusState & 2))
                    ResumeSound();
                Engine.focusState = 0;
            }
        }

        if (!(Engine.focusState & 1) || vsPlaying) {
#if !RETRO_USE_ORIGINAL_CODE
            for (int s = 0; s < gameSpeed; ++s) {
                ProcessInput();
#endif

#if !RETRO_USE_ORIGINAL_CODE
                if (!masterPaused || frameStep) {
#endif
                    ProcessNativeObjects();
#if !RETRO_USE_ORIGINAL_CODE
                }
#endif
            }

#if !RETRO_USE_ORIGINAL_CODE
            if (!masterPaused || frameStep) {
#endif
                FlipScreen();

#if !RETRO_USE_ORIGINAL_CODE
#if RETRO_USING_OPENGL && RETRO_USING_SDL2
                SDL_GL_SwapWindow(Engine.window);
#endif
                frameStep = false;
            }
#endif

#if RETRO_REV00
            Engine.message = MESSAGE_NONE;
#endif

#if RETRO_USE_HAPTICS
            int hapticID = GetHapticEffectNum();
            if (hapticID >= 0) {
                // playHaptics(hapticID);
            }
            else if (hapticID == HAPTIC_STOP) {
                // stopHaptics();
            }
#endif
        }
    }

    ReleaseAudioDevice();
    ReleaseRenderDevice();
#if !RETRO_USE_ORIGINAL_CODE
    ReleaseInputDevices();
#if RETRO_USE_NETWORKING
    DisconnectNetwork(true);
#endif
    WriteSettings();
#if RETRO_USE_MOD_LOADER
    SaveMods();
#endif
#endif

#if RETRO_USING_SDL1 || RETRO_USING_SDL2
    SDL_Quit();
#endif
}

#if RETRO_USE_MOD_LOADER
const tinyxml2::XMLElement *FirstXMLChildElement(tinyxml2::XMLDocument *doc, const tinyxml2::XMLElement *elementPtr, const char *name)
{
    if (doc) {
        if (!elementPtr)
            return doc->FirstChildElement(name);
        else
            return elementPtr->FirstChildElement(name);
    }
    return NULL;
}

const tinyxml2::XMLElement *NextXMLSiblingElement(tinyxml2::XMLDocument *doc, const tinyxml2::XMLElement *elementPtr, const char *name)
{
    if (doc) {
        if (!elementPtr)
            return doc->NextSiblingElement(name);
        else
            return elementPtr->NextSiblingElement(name);
    }
    return NULL;
}

const tinyxml2::XMLAttribute *FindXMLAttribute(const tinyxml2::XMLElement *elementPtr, const char *name) { return elementPtr->FindAttribute(name); }
const char *GetXMLAttributeName(const tinyxml2::XMLAttribute *attributePtr) { return attributePtr->Name(); }
int GetXMLAttributeValueInt(const tinyxml2::XMLAttribute *attributePtr) { return attributePtr->IntValue(); }
bool GetXMLAttributeValueBool(const tinyxml2::XMLAttribute *attributePtr) { return attributePtr->BoolValue(); }
const char *GetXMLAttributeValueString(const tinyxml2::XMLAttribute *attributePtr) { return attributePtr->Value(); }

void RetroEngine::LoadXMLWindowText()
{
    FileInfo info;
    for (int m = 0; m < (int)modList.size(); ++m) {
        if (!modList[m].active)
            continue;

        SetActiveMod(m);
        if (LoadFile("Data/Game/Game.xml", &info)) {
            tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;

            char *xmlData = new char[info.fileSize + 1];
            FileRead(xmlData, info.fileSize);
            xmlData[info.fileSize] = 0;

            bool success = doc->Parse(xmlData) == tinyxml2::XML_SUCCESS;

            if (success) {
                const tinyxml2::XMLElement *gameElement  = FirstXMLChildElement(doc, nullptr, "game");
                const tinyxml2::XMLElement *titleElement = FirstXMLChildElement(doc, gameElement, "title");
                if (titleElement) {
                    const tinyxml2::XMLAttribute *nameAttr = FindXMLAttribute(titleElement, "name");
                    if (nameAttr)
                        StrCopy(gameWindowText, GetXMLAttributeValueString(nameAttr));
                }
            }

            delete[] xmlData;
            delete doc;

            CloseFile();
        }
    }
    SetActiveMod(-1);
}
void RetroEngine::LoadXMLVariables()
{
    FileInfo info;
    for (int m = 0; m < (int)modList.size(); ++m) {
        if (!modList[m].active)
            continue;

        SetActiveMod(m);
        if (LoadFile("Data/Game/Game.xml", &info)) {
            tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;

            char *xmlData = new char[info.fileSize + 1];
            FileRead(xmlData, info.fileSize);
            xmlData[info.fileSize] = 0;

            bool success = doc->Parse(xmlData) == tinyxml2::XML_SUCCESS;

            if (success) {
                const tinyxml2::XMLElement *gameElement      = FirstXMLChildElement(doc, nullptr, "game");
                const tinyxml2::XMLElement *variablesElement = FirstXMLChildElement(doc, gameElement, "variables");
                if (variablesElement) {
                    const tinyxml2::XMLElement *varElement = FirstXMLChildElement(doc, variablesElement, "variable");
                    if (varElement) {
                        do {
                            const tinyxml2::XMLAttribute *nameAttr = FindXMLAttribute(varElement, "name");
                            const char *varName                    = "unknownVariable";
                            if (nameAttr)
                                varName = GetXMLAttributeValueString(nameAttr);

                            const tinyxml2::XMLAttribute *valAttr = FindXMLAttribute(varElement, "value");
                            int varValue                          = 0;
                            if (valAttr)
                                varValue = GetXMLAttributeValueInt(valAttr);

                            StrCopy(globalVariableNames[globalVariablesCount], varName);
                            globalVariables[globalVariablesCount] = varValue;
                            globalVariablesCount++;

                        } while ((varElement = NextXMLSiblingElement(doc, varElement, "variable")));
                    }
                }
            }

            delete[] xmlData;
            delete doc;

            CloseFile();
        }
    }
    SetActiveMod(-1);
}
void RetroEngine::LoadXMLPalettes()
{
    FileInfo info;
    for (int m = 0; m < (int)modList.size(); ++m) {
        if (!modList[m].active)
            continue;

        SetActiveMod(m);
        if (LoadFile("Data/Game/Game.xml", &info)) {
            tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;

            char *xmlData = new char[info.fileSize + 1];
            FileRead(xmlData, info.fileSize);
            xmlData[info.fileSize] = 0;

            bool success = doc->Parse(xmlData) == tinyxml2::XML_SUCCESS;

            if (success) {
                const tinyxml2::XMLElement *gameElement    = FirstXMLChildElement(doc, nullptr, "game");
                const tinyxml2::XMLElement *paletteElement = FirstXMLChildElement(doc, gameElement, "palette");
                if (paletteElement) {
                    for (const tinyxml2::XMLElement *clrElement = paletteElement->FirstChildElement("color"); clrElement;
                         clrElement = clrElement->NextSiblingElement("color")) {
                        const tinyxml2::XMLAttribute *bankAttr = clrElement->FindAttribute("bank");
                        int clrBank = 0;
                        if (bankAttr)
                            clrBank = bankAttr->IntValue();

                        const tinyxml2::XMLAttribute *indAttr = clrElement->FindAttribute("index");
                        int clrInd = 0;
                        if (indAttr)
                            clrInd = indAttr->IntValue();

                        const tinyxml2::XMLAttribute *rAttr = clrElement->FindAttribute("r");
                        int clrR = 0;
                        if (rAttr)
                            clrR = rAttr->IntValue();

                        const tinyxml2::XMLAttribute *gAttr = clrElement->FindAttribute("g");
                        int clrG = 0;
                        if (gAttr)
                            clrG = gAttr->IntValue();

                        const tinyxml2::XMLAttribute *bAttr = clrElement->FindAttribute("b");
                        int clrB = 0;
                        if (bAttr)
                            clrB = bAttr->IntValue();

                        SetPaletteEntry(clrBank, clrInd, clrR, clrG, clrB);
                    }

                    for (const tinyxml2::XMLElement *clrsElement = paletteElement->FirstChildElement("colors"); clrsElement;
                         clrsElement = clrsElement->NextSiblingElement("colors")) {
                        const tinyxml2::XMLAttribute *bankAttr = clrsElement->FindAttribute("bank");
                        int bank = 0;
                        if (bankAttr)
                            bank = bankAttr->IntValue();

                        const tinyxml2::XMLAttribute *indAttr = clrsElement->FindAttribute("start");
                        int index = 0;
                        if (indAttr)
                            index = indAttr->IntValue();

                        std::string text = clrsElement->GetText();
                        // working: AABBFF #FFaaFF (12, 32, 34) (145 53 234)
                        std::regex search(R"((?:#?([0-9A-F]{2})([0-9A-F]{2})([0-9A-F]{2}))|(?:\((\d+),?\s*(\d+),?\s*(\d+)\)))",
                                          std::regex_constants::icase | std::regex_constants::ECMAScript);
                        std::smatch match;
                        while (std::regex_search(text, match, search)) {
                            int r, g, b;
                            int base, start;
                            if (match[1].matched) {
                                // we have hex
                                base  = 16;
                                start = 1;
                            }
                            else {
                                // triplet
                                base  = 10;
                                start = 4;
                            }

                            r = std::stoi(match[start + 0].str(), nullptr, base);
                            g = std::stoi(match[start + 1].str(), nullptr, base);
                            b = std::stoi(match[start + 2].str(), nullptr, base);

                            SetPaletteEntry(bank, index++, r, g, b);
                            text = match.suffix();
                        }
                    }
                }
            }

            delete[] xmlData;
            delete doc;

            CloseFile();
        }
    }
    SetActiveMod(-1);
}
void RetroEngine::LoadXMLObjects()
{
    FileInfo info;
    modObjCount = 0;

    for (int m = 0; m < (int)modList.size(); ++m) {
        if (!modList[m].active)
            continue;

        SetActiveMod(m);
        if (LoadFile("Data/Game/Game.xml", &info)) {
            tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;

            char *xmlData = new char[info.fileSize + 1];
            FileRead(xmlData, info.fileSize);
            xmlData[info.fileSize] = 0;

            bool success = doc->Parse(xmlData) == tinyxml2::XML_SUCCESS;

            if (success) {
                const tinyxml2::XMLElement *gameElement    = FirstXMLChildElement(doc, nullptr, "game");
                const tinyxml2::XMLElement *objectsElement = FirstXMLChildElement(doc, gameElement, "objects");
                if (objectsElement) {
                    const tinyxml2::XMLElement *objElement = FirstXMLChildElement(doc, objectsElement, "object");
                    if (objElement) {
                        do {
                            const tinyxml2::XMLAttribute *nameAttr = FindXMLAttribute(objElement, "name");
                            const char *objName                    = "unknownObject";
                            if (nameAttr)
                                objName = GetXMLAttributeValueString(nameAttr);

                            const tinyxml2::XMLAttribute *scrAttr = FindXMLAttribute(objElement, "script");
                            const char *objScript                 = "unknownObject.txt";
                            if (scrAttr)
                                objScript = GetXMLAttributeValueString(scrAttr);

                            byte flags = 0;

                            // forces the object to be loaded, this means the object doesn't have to be and *SHOULD NOT* be in the stage object list
                            // if it is, it'll cause issues!!!!
                            const tinyxml2::XMLAttribute *loadAttr = FindXMLAttribute(objElement, "forceLoad");
                            int objForceLoad                       = false;
                            if (loadAttr)
                                objForceLoad = GetXMLAttributeValueBool(loadAttr);

                            flags |= (objForceLoad & 1);

                            StrCopy(modTypeNames[modObjCount], objName);
                            StrCopy(modScriptPaths[modObjCount], objScript);
                            modScriptFlags[modObjCount] = flags;
                            modObjCount++;

                        } while ((objElement = NextXMLSiblingElement(doc, objElement, "object")));
                    }
                }
            }
            else {
                PrintLog("Failed to parse game.xml File!");
            }

            delete[] xmlData;
            delete doc;

            CloseFile();
        }
    }
    SetActiveMod(-1);
}
void RetroEngine::LoadXMLSoundFX()
{
    FileInfo info;
    FileInfo infoStore;
    for (int m = 0; m < (int)modList.size(); ++m) {
        if (!modList[m].active)
            continue;

        SetActiveMod(m);
        if (LoadFile("Data/Game/Game.xml", &info)) {
            tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;

            char *xmlData = new char[info.fileSize + 1];
            FileRead(xmlData, info.fileSize);
            xmlData[info.fileSize] = 0;

            bool success = doc->Parse(xmlData) == tinyxml2::XML_SUCCESS;

            if (success) {
                const tinyxml2::XMLElement *gameElement   = FirstXMLChildElement(doc, nullptr, "game");
                const tinyxml2::XMLElement *soundsElement = FirstXMLChildElement(doc, gameElement, "sounds");
                if (soundsElement) {
                    const tinyxml2::XMLElement *sfxElement = FirstXMLChildElement(doc, soundsElement, "soundfx");
                    if (sfxElement) {
                        do {
                            const tinyxml2::XMLAttribute *nameAttr = FindXMLAttribute(sfxElement, "name");
                            const char *sfxName                    = "unknownSFX";
                            if (nameAttr)
                                sfxName = GetXMLAttributeValueString(nameAttr);

                            const tinyxml2::XMLAttribute *valAttr = FindXMLAttribute(sfxElement, "path");
                            const char *sfxPath                   = "unknownSFX.wav";
                            if (valAttr)
                                sfxPath = GetXMLAttributeValueString(valAttr);

                            SetSfxName(sfxName, globalSFXCount);

                            GetFileInfo(&infoStore);
                            CloseFile();
                            LoadSfx((char *)sfxPath, globalSFXCount);
                            SetFileInfo(&infoStore);
                            globalSFXCount++;

                        } while ((sfxElement = NextXMLSiblingElement(doc, sfxElement, "soundfx")));
                    }
                }
            }
            else {
                PrintLog("Failed to parse game.xml File!");
            }

            delete[] xmlData;
            delete doc;

            CloseFile();
        }
    }
    SetActiveMod(-1);
}
void RetroEngine::LoadXMLPlayers(TextMenu *menu)
{
    FileInfo info;

    for (int m = 0; m < (int)modList.size(); ++m) {
        if (!modList[m].active)
            continue;

        SetActiveMod(m);
        if (LoadFile("Data/Game/Game.xml", &info)) {
            tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;

            char *xmlData = new char[info.fileSize + 1];
            FileRead(xmlData, info.fileSize);
            xmlData[info.fileSize] = 0;

            bool success = doc->Parse(xmlData) == tinyxml2::XML_SUCCESS;

            if (success) {
                const tinyxml2::XMLElement *gameElement    = FirstXMLChildElement(doc, nullptr, "game");
                const tinyxml2::XMLElement *playersElement = FirstXMLChildElement(doc, gameElement, "players");
                if (playersElement) {
                    const tinyxml2::XMLElement *plrElement = FirstXMLChildElement(doc, playersElement, "player");
                    if (plrElement) {
                        do {
                            const tinyxml2::XMLAttribute *nameAttr = FindXMLAttribute(plrElement, "name");
                            const char *plrName                    = "unknownPlayer";
                            if (nameAttr)
                                plrName = GetXMLAttributeValueString(nameAttr);

                            if (menu)
                                AddTextMenuEntry(menu, plrName);
                            else
                                StrCopy(playerNames[playerCount++], plrName);

                        } while ((plrElement = NextXMLSiblingElement(doc, plrElement, "player")));
                    }
                }
            }
            else {
                PrintLog("Failed to parse game.xml File!");
            }

            delete[] xmlData;
            delete doc;

            CloseFile();
        }
    }
    SetActiveMod(-1);
}
void RetroEngine::LoadXMLStages(TextMenu *menu, int listNo)
{
    FileInfo info;
    for (int m = 0; m < (int)modList.size(); ++m) {
        if (!modList[m].active)
            continue;

        SetActiveMod(m);
        if (LoadFile("Data/Game/Game.xml", &info)) {
            tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;

            char *xmlData = new char[info.fileSize + 1];
            FileRead(xmlData, info.fileSize);
            xmlData[info.fileSize] = 0;

            bool success = doc->Parse(xmlData) == tinyxml2::XML_SUCCESS;

            if (success) {
                const tinyxml2::XMLElement *gameElement = FirstXMLChildElement(doc, nullptr, "game");
                const char *elementNames[]              = { "presentationStages", "regularStages", "bonusStages", "specialStages" };

                for (int l = 0; l < STAGELIST_MAX; ++l) {
                    const tinyxml2::XMLElement *listElement = FirstXMLChildElement(doc, gameElement, elementNames[l]);
                    if (listElement) {
                        const tinyxml2::XMLElement *stgElement = FirstXMLChildElement(doc, listElement, "stage");
                        if (stgElement) {
                            do {
                                const tinyxml2::XMLAttribute *nameAttr = FindXMLAttribute(stgElement, "name");
                                const char *stgName                    = "unknownStage";
                                if (nameAttr)
                                    stgName = GetXMLAttributeValueString(nameAttr);

                                const tinyxml2::XMLAttribute *folderAttr = FindXMLAttribute(stgElement, "folder");
                                const char *stgFolder                    = "unknownStageFolder";
                                if (folderAttr)
                                    stgFolder = GetXMLAttributeValueString(folderAttr);

                                const tinyxml2::XMLAttribute *idAttr = FindXMLAttribute(stgElement, "id");
                                const char *stgID                    = "unknownStageID";
                                if (idAttr)
                                    stgID = GetXMLAttributeValueString(idAttr);

                                const tinyxml2::XMLAttribute *highlightAttr = FindXMLAttribute(stgElement, "highlight");
                                bool stgHighlighted                         = false;
                                if (highlightAttr)
                                    stgHighlighted = GetXMLAttributeValueBool(highlightAttr);

                                if (menu) {
                                    if (listNo == 3 || listNo == 4) {
                                        if ((listNo == 4 && l == 2) || (listNo == 3 && l == 3)) {
                                            AddTextMenuEntry(menu, stgName);
                                            menu->entryHighlight[menu->rowCount - 1] = stgHighlighted;
                                        }
                                    }
                                    else if (listNo == l + 1) {
                                        AddTextMenuEntry(menu, stgName);
                                        menu->entryHighlight[menu->rowCount - 1] = stgHighlighted;
                                    }
                                }
                                else {
                                    StrCopy(stageList[l][stageListCount[l]].name, stgName);
                                    StrCopy(stageList[l][stageListCount[l]].folder, stgFolder);
                                    StrCopy(stageList[l][stageListCount[l]].id, stgID);
                                    stageList[l][stageListCount[l]].highlighted = stgHighlighted;

                                    stageListCount[l]++;
                                }

                            } while ((stgElement = NextXMLSiblingElement(doc, stgElement, "stage")));
                        }
                    }
                }
            }
            else {
                PrintLog("Failed to parse game.xml File!");
            }

            delete[] xmlData;
            delete doc;

            CloseFile();
        }
    }
    SetActiveMod(-1);
}
#endif

bool RetroEngine::LoadGameConfig(const char *filePath)
{
    FileInfo info;
    byte fileBuffer  = 0;
    byte fileBuffer2 = 0;
    char strBuffer[0x40];
    StrCopy(gameWindowText, "Retro-Engine"); // this is the default window name

    globalVariablesCount = 0;
#if RETRO_USE_MOD_LOADER
    playerCount = 0;
#endif

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
        byte objectCount = 0;
        FileRead(&objectCount, 1);
        for (byte o = 0; o < objectCount; ++o) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
        }

        // Read Script Paths
        for (byte s = 0; s < objectCount; ++s) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
        }

        byte varCount = 0;
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
        byte globalSFXCount = 0;
        FileRead(&globalSFXCount, 1);
        for (int s = 0; s < globalSFXCount; ++s) { // SFX Names
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
            strBuffer[fileBuffer] = 0;
        }
        for (byte s = 0; s < globalSFXCount; ++s) { // SFX Paths
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
            strBuffer[fileBuffer] = 0;
        }

        // Read Player Names
        byte plrCount = 0;
        FileRead(&plrCount, 1);
        for (byte p = 0; p < plrCount; ++p) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);

            // needed for PlayerName[] stuff in scripts
#if !RETRO_USE_ORIGINAL_CODE
            strBuffer[fileBuffer] = 0;
            StrCopy(playerNames[p], strBuffer);
            playerCount++;
#endif
        }

        for (byte c = 0; c < 4; ++c) {
            // Special Stages are stored as cat 2 in file, but cat 3 in game :(
            int cat = c;
            if (c == 2)
                cat = 3;
            else if (c == 3)
                cat = 2;
            stageListCount[cat] = 0;
            FileRead(&fileBuffer, 1);
            stageListCount[cat] = fileBuffer;
            for (byte s = 0; s < stageListCount[cat]; ++s) {

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

#if RETRO_USE_MOD_LOADER
        LoadXMLWindowText();
        LoadXMLVariables();
        LoadXMLPalettes();
        LoadXMLObjects();
        LoadXMLPlayers(NULL);
        LoadXMLStages(NULL, 0);

        SetGlobalVariableByName("options.devMenuFlag", devMenu ? 1 : 0);
        SetGlobalVariableByName("engine.standalone", 1);
#endif
    }

#if RETRO_REV03
    SetGlobalVariableByName("game.hasPlusDLC", !RSDK_AUTOBUILD);
#endif

    // These need to be set every time its reloaded
    nativeFunctionCount = 0;
    AddNativeFunction("SetAchievement", SetAchievement);
    AddNativeFunction("SetLeaderboard", SetLeaderboard);
#if RETRO_USE_HAPTICS
    AddNativeFunction("HapticEffect", HapticEffect);
#endif
    AddNativeFunction("Connect2PVS", Connect2PVS);
    AddNativeFunction("Disconnect2PVS", Disconnect2PVS);
    AddNativeFunction("SendEntity", SendEntity);
    AddNativeFunction("SendValue", SendValue);
    AddNativeFunction("ReceiveEntity", ReceiveEntity);
    AddNativeFunction("ReceiveValue", ReceiveValue);
    AddNativeFunction("TransmitGlobal", TransmitGlobal);
    AddNativeFunction("ShowPromoPopup", ShowPromoPopup);

    // Introduced in the Sega Forever versions of S1 (3.9.0) and S2 (1.7.0)
    AddNativeFunction("NativePlayerWaitingAds", NativePlayerWaitingAds);
    AddNativeFunction("NativeWaterPlayerWaitingAds", NativeWaterPlayerWaitingAds);

#if RETRO_REV03
    AddNativeFunction("NotifyCallback", NotifyCallback);
#endif

#if RETRO_USE_NETWORKING
    AddNativeFunction("SetNetworkGameName", SetNetworkGameName);
#endif

#if RETRO_USE_MOD_LOADER
    AddNativeFunction("ExitGame", ExitGame);
    AddNativeFunction("FileExists", FileExists);
    AddNativeFunction("OpenModMenu", OpenModMenu); // Opens the dev menu-based mod menu incase you cant be bothered or smth
    AddNativeFunction("AddAchievement", AddGameAchievement);
    AddNativeFunction("SetAchievementDescription", SetAchievementDescription);
    AddNativeFunction("ClearAchievements", ClearAchievements);
    AddNativeFunction("GetAchievementCount", GetAchievementCount);
    AddNativeFunction("GetAchievement", GetAchievement);
    AddNativeFunction("GetAchievementName", GetAchievementName);
    AddNativeFunction("GetAchievementDescription", GetAchievementDescription);
    AddNativeFunction("GetScreenWidth", GetScreenWidth);
    AddNativeFunction("SetScreenWidth", SetScreenWidth);
    AddNativeFunction("GetWindowScale", GetWindowScale);
    AddNativeFunction("SetWindowScale", SetWindowScale);
    AddNativeFunction("GetWindowScaleMode", GetWindowScaleMode);
    AddNativeFunction("SetWindowScaleMode", SetWindowScaleMode);
    AddNativeFunction("GetWindowFullScreen", GetWindowFullScreen);
    AddNativeFunction("SetWindowFullScreen", SetWindowFullScreen);
    AddNativeFunction("GetWindowBorderless", GetWindowBorderless);
    AddNativeFunction("SetWindowBorderless", SetWindowBorderless);
    AddNativeFunction("GetWindowVSync", GetWindowVSync);
    AddNativeFunction("SetWindowVSync", SetWindowVSync);
    AddNativeFunction("ApplyWindowChanges", ApplyWindowChanges); // Refresh window after changing window options
    AddNativeFunction("GetModCount", GetModCount);
    AddNativeFunction("GetModName", GetModName);
    AddNativeFunction("GetModDescription", GetModDescription);
    AddNativeFunction("GetModAuthor", GetModAuthor);
    AddNativeFunction("GetModVersion", GetModVersion);
    AddNativeFunction("GetModActive", GetModActive);
    AddNativeFunction("SetModActive", SetModActive);
    AddNativeFunction("MoveMod", MoveMod);
    AddNativeFunction("RefreshEngine", RefreshEngine); // Reload engine after changing mod status
#endif

#if !RETRO_USE_ORIGINAL_CODE
    if (strlen(Engine.startSceneFolder) && strlen(Engine.startSceneID)) {
        SceneInfo *scene = &stageList[STAGELIST_BONUS][0xFE]; // slot 0xFF is used for "none" startStage
        strcpy(scene->name, "_RSDK_SCENE");
        strcpy(scene->folder, Engine.startSceneFolder);
        strcpy(scene->id, Engine.startSceneID);
        startList_Game  = STAGELIST_BONUS;
        startStage_Game = 0xFE;
    }

#if RETRO_REV03
    Engine.usingOrigins = GetGlobalVariableID("game.playMode") != 0xFF;
#endif
#endif

    return loaded;
}
