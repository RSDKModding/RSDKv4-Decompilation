#include "RetroEngine.hpp"

bool endLine   = true;
int touchTimer = 0;

int taListStore = 0;

void initDevMenu()
{
    // DrawStageGFXHQ = 0;
    xScrollOffset = 0;
    yScrollOffset = 0;
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    fadeMode        = 0;
    playerListPos   = 0;
    Engine.gameMode = ENGINE_DEVMENU;
    ClearGraphicsData();
    ClearAnimationData();
    SetActivePalette(0, 0, 256);
    textMenuSurfaceNo = SURFACE_MAX - 1;
    LoadGIFFile("Data/Game/SystemText.gif", SURFACE_MAX - 1);
    SetPaletteEntry(-1, 0xF0, 0x00, 0x00, 0x00);
    SetPaletteEntry(-1, 0xFF, 0xFF, 0xFF, 0xFF);
    setTextMenu(DEVMENU_MAIN);
    drawStageGFXHQ           = false;
    Engine.finishedStartMenu = true;
    touchTimer               = 0;
}
void initErrorMessage()
{
    xScrollOffset = 0;
    yScrollOffset = 0;
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    fadeMode        = 0;
    playerListPos   = 0;
    Engine.gameMode = ENGINE_DEVMENU;
    ClearGraphicsData();
    ClearAnimationData();
    SetActivePalette(0, 0, 256);
    textMenuSurfaceNo = SURFACE_MAX - 1;
    LoadGIFFile("Data/Game/SystemText.gif", SURFACE_MAX - 1);
    SetPaletteEntry(-1, 0xF0, 0x00, 0x00, 0x00);
    SetPaletteEntry(-1, 0xFF, 0xFF, 0xFF, 0xFF);
    gameMenu[0].alignment        = 2;
    gameMenu[0].selectionCount   = 1;
    gameMenu[0].selection1       = 0;
    gameMenu[1].visibleRowCount  = 0;
    gameMenu[1].visibleRowOffset = 0;
    stageMode                    = DEVMENU_SCRIPTERROR;
    drawStageGFXHQ               = false;
    Engine.finishedStartMenu     = true;
    touchTimer                   = 0;
}
void processStageSelect()
{
    ClearScreen(0xF0);
    keyDown.start = false;
    keyDown.up    = false;
    keyDown.down  = false;

    CheckKeyDown(&keyDown);
    CheckKeyPress(&keyPress);

#if defined RETRO_USING_MOUSE || defined RETRO_USING_TOUCH
    DrawSprite(32, 0x42, 16, 16, 78, 240, textMenuSurfaceNo);
    DrawSprite(32, 0xB2, 16, 16, 95, 240, textMenuSurfaceNo);
    DrawSprite(SCREEN_XSIZE - 32, SCREEN_YSIZE - 32, 16, 16, 112, 240, textMenuSurfaceNo);
#endif

    if (!keyDown.start && !keyDown.up && !keyDown.down) {
        if (touches > 0) {
            if (touchDown[0] && !(touchTimer % 8)) {
                if (touchX[0] < SCREEN_CENTERY) {
                    if (touchY[0] >= SCREEN_CENTERY) {
                        if (!keyDown.down)
                            keyPress.down = true;
                        keyDown.down = true;
                    }
                    else {
                        if (!keyDown.up)
                            keyPress.up = true;
                        keyDown.up = true;
                    }
                }
                else if (touchX[0] != SCREEN_CENTERY && touchY[0] > SCREEN_CENTERY) {
                    if (!keyDown.start)
                        keyPress.start = true;
                    keyDown.start = true;
                }
            }
        }
    }

    touchTimer++;

    switch (stageMode) {
        case DEVMENU_MAIN: // Main Menu
        {
            if (keyPress.down)
                gameMenu[0].selection2 += 2;

            if (keyPress.up)
                gameMenu[0].selection2 -= 2;

            if (gameMenu[0].selection2 > 15)
                gameMenu[0].selection2 = 9;
            if (gameMenu[0].selection2 < 9)
                gameMenu[0].selection2 = 15;

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX, 72);
            if (keyPress.start || keyPress.A) {
                if (gameMenu[0].selection2 == 9) {
                    ClearGraphicsData();
                    ClearAnimationData();
                    activeStageList   = 0;
                    stageMode         = STAGEMODE_LOAD;
                    Engine.gameMode   = ENGINE_MAINGAME;
                    stageListPosition = 0;
                }
                else if (gameMenu[0].selection2 == 11) {
                    SetupTextMenu(&gameMenu[0], 0);
                    AddTextMenuEntry(&gameMenu[0], "SELECT A PLAYER");
                    SetupTextMenu(&gameMenu[1], 0);
                    LoadConfigListText(&gameMenu[1], 0);
                    gameMenu[1].alignment      = 0;
                    gameMenu[1].selectionCount = 1;
                    gameMenu[1].selection1     = 0;
                    stageMode                  = DEVMENU_PLAYERSEL;
                }
                else if (gameMenu[0].selection2 == 13) {
                    initStartMenu(0);
                }
                else {
                    Engine.running = false;
                }
            }
            else if (keyPress.B) {
                ClearGraphicsData();
                ClearAnimationData();
                activeStageList   = 0;
                stageMode         = STAGEMODE_LOAD;
                Engine.gameMode   = ENGINE_MAINGAME;
                stageListPosition = 0;
            }
            break;
        }
        case DEVMENU_PLAYERSEL: // Selecting Player
        {
            if (keyPress.down)
                ++gameMenu[1].selection1;
            if (keyPress.up)
                --gameMenu[1].selection1;
            if (gameMenu[1].selection1 == gameMenu[1].rowCount)
                gameMenu[1].selection1 = 0;

            if (gameMenu[1].selection1 < 0)
                gameMenu[1].selection1 = gameMenu[1].rowCount - 1;

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 4, 72);
            DrawTextMenu(&gameMenu[1], SCREEN_CENTERX - 40, 96);
            if (keyPress.start || keyPress.A) {
                playerListPos = gameMenu[1].selection1;
                setTextMenu(DEVMENU_STAGELISTSEL);
            }
            else if (keyPress.B) {
                setTextMenu(DEVMENU_MAIN);
            }
            break;
        }
        case DEVMENU_STAGELISTSEL: // Selecting Category
        {
            if (keyPress.down)
                gameMenu[0].selection2 += 2;
            if (keyPress.up)
                gameMenu[0].selection2 -= 2;

            if (gameMenu[0].selection2 > 9)
                gameMenu[0].selection2 = 3;

            if (gameMenu[0].selection2 < 3)
                gameMenu[0].selection2 = 9;

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 80, 72);
            bool nextMenu = false;
            switch (gameMenu[0].selection2) {
                case 3: // Presentation
                    if (stageListCount[0] > 0)
                        nextMenu = true;
                    activeStageList = 0;
                    break;
                case 5: // Regular
                    if (stageListCount[1] > 0)
                        nextMenu = true;
                    activeStageList = 1;
                    break;
                case 7: // Special
                    if (stageListCount[3] > 0)
                        nextMenu = true;
                    activeStageList = 3;
                    break;
                case 9: // Bonus
                    if (stageListCount[2] > 0)
                        nextMenu = true;
                    activeStageList = 2;
                    break;
                default: break;
            }

            if ((keyPress.start || keyPress.A) && nextMenu) {
                SetupTextMenu(&gameMenu[0], 0);
                AddTextMenuEntry(&gameMenu[0], "SELECT A STAGE");
                SetupTextMenu(&gameMenu[1], 0);
                LoadConfigListText(&gameMenu[1], ((gameMenu[0].selection2 - 3) >> 1) + 1);
                gameMenu[1].alignment      = 1;
                gameMenu[1].selectionCount = 3;
                gameMenu[1].selection1     = 0;
                if (gameMenu[1].rowCount > 18)
                    gameMenu[1].visibleRowCount = 18;
                else
                    gameMenu[1].visibleRowCount = 0;

                gameMenu[0].alignment      = 2;
                gameMenu[0].selectionCount = 1;
                gameMenu[1].timer          = 0;
                stageMode                  = DEVMENU_STAGESEL;
            }
            else if (keyPress.B) {
                SetupTextMenu(&gameMenu[0], 0);
                AddTextMenuEntry(&gameMenu[0], "SELECT A PLAYER");
                SetupTextMenu(&gameMenu[1], 0);
                LoadConfigListText(&gameMenu[1], 0);
                gameMenu[0].alignment        = 2;
                gameMenu[1].alignment        = 0;
                gameMenu[1].selectionCount   = 1;
                gameMenu[1].visibleRowCount  = 0;
                gameMenu[1].visibleRowOffset = 0;
                gameMenu[1].selection1       = playerListPos;
                stageMode                    = DEVMENU_PLAYERSEL;
            }
            break;
        }
        case DEVMENU_STAGESEL: // Selecting Stage
        {
            if (keyDown.down) {
                gameMenu[1].timer += 1;
                if (gameMenu[1].timer > 8) {
                    gameMenu[1].timer = 0;
                    keyPress.down     = true;
                }
            }
            else {
                if (keyDown.up) {
                    gameMenu[1].timer -= 1;
                    if (gameMenu[1].timer < -8) {
                        gameMenu[1].timer = 0;
                        keyPress.up       = true;
                    }
                }
                else {
                    gameMenu[1].timer = 0;
                }
            }
            if (keyPress.down) {
                gameMenu[1].selection1++;
                if (gameMenu[1].selection1 - gameMenu[1].visibleRowOffset >= gameMenu[1].visibleRowCount) {
                    gameMenu[1].visibleRowOffset += 1;
                }
            }
            if (keyPress.up) {
                gameMenu[1].selection1--;
                if (gameMenu[1].selection1 - gameMenu[1].visibleRowOffset < 0) {
                    gameMenu[1].visibleRowOffset -= 1;
                }
            }
            if (gameMenu[1].selection1 == gameMenu[1].rowCount) {
                gameMenu[1].selection1       = 0;
                gameMenu[1].visibleRowOffset = 0;
            }
            if (gameMenu[1].selection1 < 0) {
                gameMenu[1].selection1       = gameMenu[1].rowCount - 1;
                gameMenu[1].visibleRowOffset = gameMenu[1].rowCount - gameMenu[1].visibleRowCount;
            }

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 4, 40);
            DrawTextMenu(&gameMenu[1], SCREEN_CENTERX + 100, 64);
            if (keyPress.start || keyPress.A) {
                debugMode         = keyDown.A;
                stageMode         = STAGEMODE_LOAD;
                Engine.gameMode   = ENGINE_MAINGAME;
                stageListPosition = gameMenu[1].selection1;
                SetGlobalVariableByName("options.gameMode", 0);
                SetGlobalVariableByName("lampPostID", 0); // For S1
                SetGlobalVariableByName("starPostID", 0); // For S2
            }
            else if (keyPress.B) {
                setTextMenu(DEVMENU_STAGELISTSEL);
            }
            break;
        }
        case DEVMENU_SCRIPTERROR: // Script Error
        {
            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX, 72);
            if (keyPress.start || keyPress.A) {
                setTextMenu(DEVMENU_MAIN);
            }
            else if (keyPress.B) {
                ClearGraphicsData();
                ClearAnimationData();
                activeStageList   = 0;
                stageMode         = DEVMENU_STAGESEL;
                Engine.gameMode   = ENGINE_MAINGAME;
                stageListPosition = 0;
            }
            break;
        }
        default: break;
    }
}

void initStartMenu(int mode)
{
    // DrawStageGFXHQ = 0;
    xScrollOffset = 0;
    yScrollOffset = 0;
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    fadeMode                 = 0;
    playerListPos            = 0;
    Engine.gameMode          = ENGINE_MAINGAME;
    Engine.finishedStartMenu = false;
    ClearGraphicsData();
    ClearAnimationData();
    SetActivePalette(0, 0, 256);
    textMenuSurfaceNo = 0;
    LoadGIFFile("Data/Game/SystemText.gif", 0);
    SetPaletteEntry(-1, 0xF0, 0x00, 0x00, 0x00);
    SetPaletteEntry(-1, 0xFF, 0xFF, 0xFF, 0xFF);

    ReadSaveRAMData();
    if (saveRAM[0x100] != Engine.gameType) {
        saveRAM[0x100] = Engine.gameType;
        if (Engine.gameType == GAME_SONIC1) {
            saveRAM[0x101] = 1;
            saveRAM[0x102] = 0;
            saveRAM[0x103] = 0;
            saveRAM[0x104] = 0;
            saveRAM[0x105] = 0;
        }
        else {
            saveRAM[0x101] = 0;
            saveRAM[0x102] = 1;
            saveRAM[0x103] = 1;
            saveRAM[0x104] = 0;
            saveRAM[0x105] = 0;
        }
        WriteSaveRAMData();
    }
    else {
        if (Engine.gameType == GAME_SONIC1) {
            SetGlobalVariableByName("options.spindash", saveRAM[0x101]);
            SetGlobalVariableByName("options.speedCap", saveRAM[0x102]);
            SetGlobalVariableByName("options.airSpeedCap", saveRAM[0x103]);
            SetGlobalVariableByName("options.spikeBehavior", saveRAM[0x104]);
            SetGlobalVariableByName("options.shieldType", saveRAM[0x105]);
            SetGlobalVariableByName("options.superStates", saveRAM[0x106]);
        }
        else {
            SetGlobalVariableByName("options.airSpeedCap", saveRAM[0x101]);
            SetGlobalVariableByName("options.tailsFlight", saveRAM[0x102]);
            SetGlobalVariableByName("options.superTails", saveRAM[0x103]);
            SetGlobalVariableByName("options.spikeBehavior", saveRAM[0x104]);
            SetGlobalVariableByName("options.shieldType", saveRAM[0x105]);
        }
    }

    if (mode == 0 || !GetGlobalVariableByName("timeAttack.result")) {
        setTextMenu(STARTMENU_MAIN);
    }
    else {
        // finished TA act
        int listPos = taListStore;

        int result = GetGlobalVariableByName("timeAttack.result");
        if (result < saveRAM[3 * listPos + 0x40]) {
            saveRAM[3 * listPos + 0x42] = saveRAM[3 * listPos + 0x41];
            saveRAM[3 * listPos + 0x41] = saveRAM[3 * listPos + 0x40];
            saveRAM[3 * listPos + 0x40] = result;
        }
        else if (result < saveRAM[3 * listPos + 0x41]) {
            saveRAM[3 * listPos + 0x42] = saveRAM[3 * listPos + 0x41];
            saveRAM[3 * listPos + 0x41] = result;
        }
        else if (result < saveRAM[3 * listPos + 0x42]) {
            saveRAM[3 * listPos + 0x42] = result;
        }
        WriteSaveRAMData();

        char strBuffer[0x100];
        SetupTextMenu(&gameMenu[0], 0);
        AddTextMenuEntry(&gameMenu[0], "BEST TIMES");
        AddTextMenuEntry(&gameMenu[0], "");
        StrCopy(strBuffer, "1ST: ");
        int mins = saveRAM[3 * (listPos) + 0x40] / 6000;
        int secs = saveRAM[3 * (listPos) + 0x40] / 100 % 60;
        int ms   = saveRAM[3 * (listPos) + 0x40] % 100;
        if (mins < 10)
            AppendIntegerToString(strBuffer, 0);
        AppendIntegerToString(strBuffer, mins);
        StrAdd(strBuffer, ":");
        if (secs < 10)
            AppendIntegerToString(strBuffer, 0);
        AppendIntegerToString(strBuffer, secs);
        StrAdd(strBuffer, ":");
        if (ms < 10)
            AppendIntegerToString(strBuffer, 0);
        AppendIntegerToString(strBuffer, ms);
        AddTextMenuEntry(&gameMenu[0], strBuffer);
        AddTextMenuEntry(&gameMenu[0], "");
        StrCopy(strBuffer, "2ND: ");
        mins = saveRAM[3 * (listPos) + 0x41] / 6000;
        secs = saveRAM[3 * (listPos) + 0x41] / 100 % 60;
        ms   = saveRAM[3 * (listPos) + 0x41] % 100;
        if (mins < 10)
            AppendIntegerToString(strBuffer, 0);
        AppendIntegerToString(strBuffer, mins);
        StrAdd(strBuffer, ":");
        if (secs < 10)
            AppendIntegerToString(strBuffer, 0);
        AppendIntegerToString(strBuffer, secs);
        StrAdd(strBuffer, ":");
        if (ms < 10)
            AppendIntegerToString(strBuffer, 0);
        AppendIntegerToString(strBuffer, ms);
        AddTextMenuEntry(&gameMenu[0], strBuffer);
        AddTextMenuEntry(&gameMenu[0], "");
        StrCopy(strBuffer, "3RD: ");
        mins = saveRAM[3 * (listPos) + 0x42] / 6000;
        secs = saveRAM[3 * (listPos) + 0x42] / 100 % 60;
        ms   = saveRAM[3 * (listPos) + 0x42] % 100;
        if (mins < 10)
            AppendIntegerToString(strBuffer, 0);
        AppendIntegerToString(strBuffer, mins);
        StrAdd(strBuffer, ":");
        if (secs < 10)
            AppendIntegerToString(strBuffer, 0);
        AppendIntegerToString(strBuffer, secs);
        StrAdd(strBuffer, ":");
        if (ms < 10)
            AppendIntegerToString(strBuffer, 0);
        AppendIntegerToString(strBuffer, ms);
        AddTextMenuEntry(&gameMenu[0], strBuffer);
        AddTextMenuEntry(&gameMenu[0], "");

        SetupTextMenu(&gameMenu[1], 0);
        AddTextMenuEntry(&gameMenu[1], "PLAY");
        AddTextMenuEntry(&gameMenu[1], "");
        AddTextMenuEntry(&gameMenu[1], "BACK");
        AddTextMenuEntry(&gameMenu[1], "");

        gameMenu[1].alignment       = 2;
        gameMenu[1].selectionCount  = 1;
        gameMenu[1].selection1      = 0;
        gameMenu[1].selection2      = listPos;
        gameMenu[1].visibleRowCount = 0;

        gameMenu[0].alignment      = 2;
        gameMenu[0].selectionCount = 1;
        gameMenu[1].timer          = 0;

        stageMode = STARTMENU_TACONFIRMSEL;
    }
}

void setTextMenu(int sm)
{
    ushort strBuffer[0x100];
    stageMode = sm;
    SetupTextMenu(&gameMenu[0], 0);
    SetupTextMenu(&gameMenu[1], 0);
    switch (sm) {
        case DEVMENU_MAIN: {
            AddTextMenuEntry(&gameMenu[0], "RETRO ENGINE DEV MENU");
            AddTextMenuEntry(&gameMenu[0], " ");
            char version[0x80];
            StrCopy(version, Engine.gameWindowText);
            StrAdd(version, " Version");
            AddTextMenuEntry(&gameMenu[0], version);
            AddTextMenuEntry(&gameMenu[0], Engine.gameVersion);
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "START GAME");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "STAGE SELECT");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "START MENU");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "EXIT GAME");
            gameMenu[0].alignment        = 2;
            gameMenu[0].selectionCount   = 2;
            gameMenu[0].selection1       = 0;
            gameMenu[0].selection2       = 9;
            gameMenu[1].visibleRowCount  = 0;
            gameMenu[1].visibleRowOffset = 0;
            break;
        }
        case DEVMENU_STAGELISTSEL:
            AddTextMenuEntry(&gameMenu[0], "SELECT A STAGE LIST");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "   PRESENTATION");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "   REGULAR");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "   SPECIAL");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "   BONUS");
            gameMenu[0].alignment      = 0;
            gameMenu[0].selection2     = 3;
            gameMenu[0].selectionCount = 2;
            break;
        case STARTMENU_MAIN: {
            char title[0x80];
            StringUpperCase(title, Engine.gameWindowText);
            StrAdd(title, " START MENU");
            AddTextMenuEntry(&gameMenu[0], title);
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], Engine.gameVersion);
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "START GAME");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "TIME ATTACK");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "ACHIEVEMENTS");
            AddTextMenuEntry(&gameMenu[0], " ");
            AddTextMenuEntry(&gameMenu[0], "LEADERBOARDS");
            if (Engine.gameType == GAME_SONIC2) {
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "2P VERSUS");
            }
            LoadConfigListText(&gameMenu[1], 0); // to get the data stored
            gameMenu[0].alignment        = 2;
            gameMenu[0].selectionCount   = 2;
            gameMenu[0].selection1       = 0;
            gameMenu[0].selection2       = 9;
            gameMenu[1].visibleRowCount  = 0;
            gameMenu[1].visibleRowOffset = 0;
            break;
        }
        case STARTMENU_SAVESEL: {
            AddTextMenuEntry(&gameMenu[0], "SELECT A SAVE FILE");
            AddTextMenuEntry(&gameMenu[1], "GAME OPTIONS");
            AddTextMenuEntry(&gameMenu[1], "");
            AddTextMenuEntry(&gameMenu[1], "DELETE SAVE FILE");
            AddTextMenuEntry(&gameMenu[1], "");
            AddTextMenuEntryW(&gameMenu[1], strNoSave);

            for (int s = 0; s < 4; ++s) {
                AddTextMenuEntry(&gameMenu[1], "");

                StrCopyW(strBuffer, "SAVE ");
                AppendIntegerToStringW(strBuffer, s + 1);
                StrAddW(strBuffer, " - ");
                int stagePos = saveRAM[s * 8 + 4];
                if (stagePos) {
                    if (stagePos >= 0x80) {
                        StrAddW(strBuffer, playerListText[saveRAM[s * 8 + 0]]);
                        StrAddW(strBuffer, "-");
                        StrAddW(strBuffer, "SPECIAL STAGE ");
                        AppendIntegerToStringW(strBuffer, saveRAM[s * 8 + 6] + 1);
                    }
                    else {
                        if (StrComp("STAGE MENU", stageList[STAGELIST_REGULAR][stagePos - 1].name)) {
                            StrAddW(strBuffer, playerListText[saveRAM[s * 8 + 0]]);
                            StrAddW(strBuffer, "-");
                            StrAddW(strBuffer, "COMPLETE");
                        }
                        else {
                            StrAddW(strBuffer, playerListText[saveRAM[s * 8 + 0]]);
                            StrAddW(strBuffer, "-");
                            StrAddW(strBuffer, strSaveStageList[(saveRAM[s * 8 + 4] - 1)]);
                        }
                    }
                }
                else {
                    StrAddW(strBuffer, strNewGame);
                }

                AddTextMenuEntryW(&gameMenu[1], strBuffer);
            }
            gameMenu[1].alignment      = 0;
            gameMenu[1].selectionCount = 1;
            gameMenu[1].selection1     = 0;
            gameMenu[1].selection2     = 0;
            break;
        }
        case STARTMENU_TASTAGESEL: {
            AddTextMenuEntry(&gameMenu[0], "SELECT A STAGE");
            int cnt = 0;
            for (int i = 0; i < stageStrCount; ++i) {
                if (strSaveStageList[i] && !StrCompW(strSaveStageList[i], "Complete")
                    && !(Engine.gameType == GAME_SONIC2
                         && (StrCompW(strSaveStageList[i], "Special Stage 6") || StrCompW(strSaveStageList[i], "SKY CHASE ZONE")
                             || StrCompW(strSaveStageList[i], "DEATH EGG ZONE")))) {
                    AddTextMenuEntry(&gameMenu[1], "");
                    AddTextMenuEntryW(&gameMenu[1], strSaveStageList[i]);
                    cnt++;
                }
            }
            gameMenu[1].alignment      = 2;
            gameMenu[1].selectionCount = 1;
            gameMenu[1].selection1     = 1;
            gameMenu[1].selection2     = 0;
            if (gameMenu[1].rowCount > 18)
                gameMenu[1].visibleRowCount = 18;
            else
                gameMenu[1].visibleRowCount = 0;

            gameMenu[0].alignment      = 2;
            gameMenu[0].selectionCount = 1;
            gameMenu[1].timer          = 0;
            gameMenu[0].timer          = cnt;
            break;
        }
        case STARTMENU_ACHIEVEMENTS: {
            AddTextMenuEntry(&gameMenu[0], "ACHIEVEMENTS LIST");

            char strBuffer[0x80];
            for (int i = 0; i < ACHIEVEMENT_MAX; ++i) {
                if (!StrComp(achievements[i].name, "")) {
                    AddTextMenuEntry(&gameMenu[1], "");
                    StrCopy((char *)strBuffer, achievements[i].name);
                    StrAdd((char *)strBuffer, ": ");
                    StrAdd((char *)strBuffer, achievements[i].status == 100 ? "achieved" : "not achieved");
                    AddTextMenuEntry(&gameMenu[1], (char *)strBuffer);
                }
            }
            gameMenu[1].alignment      = 0;
            gameMenu[1].selectionCount = 1;
            gameMenu[1].selection1     = 1;
            gameMenu[1].selection2     = 0;
            if (gameMenu[1].rowCount > 15)
                gameMenu[1].visibleRowCount = 15;
            else
                gameMenu[1].visibleRowCount = 0;

            gameMenu[0].alignment      = 2;
            gameMenu[0].selectionCount = 1;
            gameMenu[1].timer          = 0;
            break;
        }
        case STARTMENU_GAMEOPTS: {
            AddTextMenuEntry(&gameMenu[0], "GAME OPTIONS");
            if (Engine.gameType == GAME_SONIC1) {
                if (GetGlobalVariableByName("options.spindash"))
                    AddTextMenuEntry(&gameMenu[1], "SPINDASH: ENABLED");
                else
                    AddTextMenuEntry(&gameMenu[1], "SPINDASH: DISABLED");
                AddTextMenuEntry(&gameMenu[1], "");
                if (GetGlobalVariableByName("options.speedCap"))
                    AddTextMenuEntry(&gameMenu[1], "GROUND SPEED CAP: ENABLED");
                else
                    AddTextMenuEntry(&gameMenu[1], "GROUND SPEED CAP: DISABLED");
                AddTextMenuEntry(&gameMenu[1], "");
                if (GetGlobalVariableByName("options.airSpeedCap"))
                    AddTextMenuEntry(&gameMenu[1], "AIR SPEED CAP: ENABLED");
                else
                    AddTextMenuEntry(&gameMenu[1], "AIR SPEED CAP: DISABLED");
                AddTextMenuEntry(&gameMenu[1], "");
                if (GetGlobalVariableByName("options.spikeBehavior"))
                    AddTextMenuEntry(&gameMenu[1], "S1 SPIKES: ENABLED");
                else
                    AddTextMenuEntry(&gameMenu[1], "S1 SPIKES: DISABLED");
                AddTextMenuEntry(&gameMenu[1], "");

                char itemBoxTypes[4][0x20] = { "ITEM TYPE: S1", "ITEM TYPE: S2", "ITEM TYPE: S1+S3", "ITEM TYPE: S2+S3" };
                AddTextMenuEntry(&gameMenu[1], itemBoxTypes[GetGlobalVariableByName("options.shieldType")]);

                AddTextMenuEntry(&gameMenu[1], "");
                if (GetGlobalVariableByName("options.superStates"))
                    AddTextMenuEntry(&gameMenu[1], "SUPER FORMS: ENABLED");
                else
                    AddTextMenuEntry(&gameMenu[1], "SUPER FORMS: DISABLED");
            }
            else {
                if (GetGlobalVariableByName("options.airSpeedCap"))
                    AddTextMenuEntry(&gameMenu[1], "AIR SPEED CAP: ENABLED");
                else
                    AddTextMenuEntry(&gameMenu[1], "AIR SPEED CAP: DISABLED");
                AddTextMenuEntry(&gameMenu[1], "");
                if (GetGlobalVariableByName("options.tailsFlight"))
                    AddTextMenuEntry(&gameMenu[1], "TAILS FLIGHT: ENABLED");
                else
                    AddTextMenuEntry(&gameMenu[1], "TAILS FLIGHT: DISABLED");
                AddTextMenuEntry(&gameMenu[1], "");
                if (GetGlobalVariableByName("options.superTails"))
                    AddTextMenuEntry(&gameMenu[1], "SUPER TAILS: ENABLED");
                else
                    AddTextMenuEntry(&gameMenu[1], "SUPER TAILS: DISABLED");
                AddTextMenuEntry(&gameMenu[1], "");
                if (GetGlobalVariableByName("options.spikeBehavior"))
                    AddTextMenuEntry(&gameMenu[1], "S1 SPIKES: ENABLED");
                else
                    AddTextMenuEntry(&gameMenu[1], "S1 SPIKES: DISABLED");
                AddTextMenuEntry(&gameMenu[1], "");

                char itemBoxTypes[4][0x20] = { "ITEM TYPE: S2", "ITEM TYPE: S2+S3", "ITEM TYPE: RANDOM", "ITEM TYPE: RANDOM+S3" };
                AddTextMenuEntry(&gameMenu[1], itemBoxTypes[GetGlobalVariableByName("options.shieldType")]);
            }
        }
    }
}

void processStartMenu()
{
    ClearScreen(0xF0);
    keyDown.start = false;
    keyDown.up    = false;
    keyDown.down  = false;

    CheckKeyDown(&keyDown);
    CheckKeyPress(&keyPress);

    if (!keyDown.start && !keyDown.up && !keyDown.down) {
        if (touches > 0) {
            if (touchDown[0] && !(touchTimer % 8)) {
                if (touchX[0] < SCREEN_CENTERX) {
                    if (touchY[0] >= SCREEN_CENTERY) {
                        if (!keyDown.down)
                            keyPress.down = true;
                        keyDown.down = true;
                    }
                    else {
                        if (!keyDown.up)
                            keyPress.up = true;
                        keyDown.up = true;
                    }
                }
                else if (touchX[0] > SCREEN_CENTERX) {
                    if (touchY[0] > SCREEN_CENTERY) {
                        if (!keyDown.start)
                            keyPress.start = true;
                        keyDown.start = true;
                    }
                    else {
                        if (!keyDown.B)
                            keyPress.B = true;
                        keyDown.B = true;
                    }
                }
            }
        }
    }

    touchTimer++;

    switch (stageMode) {
        case STARTMENU_MAIN: {
            if (keyPress.down)
                gameMenu[0].selection2 += 2;

            if (keyPress.up)
                gameMenu[0].selection2 -= 2;

            if (Engine.gameType == GAME_SONIC2) {
                if (gameMenu[0].selection2 > 17)
                    gameMenu[0].selection2 = 9;
                if (gameMenu[0].selection2 < 9)
                    gameMenu[0].selection2 = 17;
            }
            else {
                if (gameMenu[0].selection2 > 15)
                    gameMenu[0].selection2 = 9;
                if (gameMenu[0].selection2 < 9)
                    gameMenu[0].selection2 = 15;
            }

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX, 72);
            if (keyPress.start || keyPress.A) {
                if (gameMenu[0].selection2 == 9) {
                    setTextMenu(STARTMENU_SAVESEL);
                }
                else if (gameMenu[0].selection2 == 11) {
                    setTextMenu(STARTMENU_TASTAGESEL);
                }
                else if (gameMenu[0].selection2 == 13) {
                    setTextMenu(STARTMENU_ACHIEVEMENTS);
                }
                else if (gameMenu[0].selection2 == 15) {
                    PlaySFXByName("Hurt", 0);
                }
                else {
                    PlaySFXByName("Hurt", 0);

                    // TODO: add networking code and remove this if statement
                    if (false) {
                        // 2P VS
                        SetGlobalVariableByName("options.saveSlot", 0);
                        SetGlobalVariableByName("options.gameMode", 0);
                        SetGlobalVariableByName("options.vsMode", 0);
                        SetGlobalVariableByName("stage.player2Enabled", true); // 2P
                        SetGlobalVariableByName("player.lives", 3);
                        SetGlobalVariableByName("player.score", 0);
                        SetGlobalVariableByName("player.scoreBonus", 50000);
                        SetGlobalVariableByName("specialStage.listPos", 0);
                        SetGlobalVariableByName("specialStage.emeralds", 0);
                        SetGlobalVariableByName("specialStage.nextZone", 0);
                        SetGlobalVariableByName("timeAttack.result", 0);
                        SetGlobalVariableByName("lampPostID", 0); // For S1
                        SetGlobalVariableByName("starPostID", 0); // For S2

                        // if (Engine.onlineActive)
                        InitStartingStage(STAGELIST_PRESENTATION, 3, 0);
                        Engine.finishedStartMenu = true;
                    }
                }
            }
            else if (keyPress.B) {
                Engine.running = false;
            }
            break;
        }
        case STARTMENU_SAVESEL: {
            if (keyPress.down)
                gameMenu[1].selection1 += 2;

            if (keyPress.up)
                gameMenu[1].selection1 -= 2;

            if (gameMenu[1].selection1 > 12)
                gameMenu[1].selection1 = 0;
            if (gameMenu[1].selection1 < 0)
                gameMenu[1].selection1 = 12;

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 4, 72);
            DrawTextMenu(&gameMenu[1], 16, 96);
            if (keyPress.start || keyPress.A) {
                if (gameMenu[1].selection1 == 0) {
                    setTextMenu(STARTMENU_GAMEOPTS);
                }
                else if (gameMenu[1].selection1 == 2) {
                    if (!gameMenu[1].selection2) {
                        SetTextMenuEntry(&gameMenu[1], "CANCEL", 2);
                        gameMenu[1].selection2 ^= 1;
                    }
                    else {
                        SetTextMenuEntry(&gameMenu[1], "DELETE SAVE FILE", 2);
                        gameMenu[1].selection2 ^= 1;
                    }
                }
                else {
                    int saveSlot = (gameMenu[1].selection1 - 6) / 2;
                    if (!gameMenu[1].selection2) {
                        if (saveSlot >= 0 && saveSlot < 4) {
                            int savePos = saveSlot << 3;
                            if (saveRAM[savePos + 4]) {
                                SetGlobalVariableByName("options.saveSlot", saveSlot);
                                SetGlobalVariableByName("options.gameMode", 1);
                                SetGlobalVariableByName("options.stageSelectFlag", 0);
                                SetGlobalVariableByName("player.lives", saveRAM[savePos + 1]);
                                SetGlobalVariableByName("player.score", saveRAM[savePos + 2]);
                                SetGlobalVariableByName("player.scoreBonus", saveRAM[savePos + 3]);
                                SetGlobalVariableByName("specialStage.emeralds", saveRAM[savePos + 5]);
                                SetGlobalVariableByName("specialStage.listPos", saveRAM[savePos + 6]);
                                SetGlobalVariableByName("stage.player2Enabled", saveRAM[savePos + 0] == 3);
                                SetGlobalVariableByName("lampPostID", 0); // For S1
                                SetGlobalVariableByName("starPostID", 0); // For S2
                                SetGlobalVariableByName("options.vsMode", 0);

                                int nextZone = saveRAM[savePos + 4];
                                if (nextZone > 127) {
                                    SetGlobalVariableByName("specialStage.nextZone", nextZone - 129);
                                    InitStartingStage(STAGELIST_SPECIAL, saveRAM[savePos + 6], saveRAM[savePos + 0]);
                                }
                                else {
                                    SetGlobalVariableByName("specialStage.nextZone", nextZone - 1);
                                    InitStartingStage(STAGELIST_REGULAR, saveRAM[savePos + 4] - 1, saveRAM[savePos + 0]);
                                }
                                Engine.finishedStartMenu = true;
                            }
                            else {
                                // new save
                                SetupTextMenu(&gameMenu[0], 0);
                                AddTextMenuEntry(&gameMenu[0], "SELECT A PLAYER");
                                SetupTextMenu(&gameMenu[1], 0);
                                LoadConfigListText(&gameMenu[1], 0);
                                gameMenu[1].alignment      = 0;
                                gameMenu[1].selectionCount = 1;
                                gameMenu[1].selection1     = 0;
                                gameMenu[1].selection2     = saveSlot;
                                stageMode                  = STARTMENU_PLAYERSEL;
                            }
                        }
                        else {
                            // nosave
                            SetupTextMenu(&gameMenu[0], 0);
                            AddTextMenuEntry(&gameMenu[0], "SELECT A PLAYER");
                            SetupTextMenu(&gameMenu[1], 0);
                            LoadConfigListText(&gameMenu[1], 0);
                            gameMenu[1].alignment      = 0;
                            gameMenu[1].selectionCount = 1;
                            gameMenu[1].selection1     = 0;
                            gameMenu[1].selection2     = saveSlot;
                            stageMode                  = STARTMENU_PLAYERSEL;
                        }
                    }
                    else {
                        if (saveSlot >= 0 && saveSlot < 4) {
                            int savePos          = saveSlot << 3;
                            saveRAM[savePos + 0] = 0;
                            saveRAM[savePos + 1] = 3;
                            saveRAM[savePos + 2] = 0;
                            saveRAM[savePos + 3] = 50000;
                            saveRAM[savePos + 4] = 0;
                            saveRAM[savePos + 5] = 0;
                            saveRAM[savePos + 6] = 0;
                            saveRAM[savePos + 7] = 0;

                            ushort strBuffer[0x100];
                            StrCopyW(strBuffer, "SAVE ");
                            AppendIntegerToStringW(strBuffer, saveSlot + 1);
                            StrAddW(strBuffer, " - ");
                            StrAddW(strBuffer, strNewGame);

                            SetTextMenuEntryW(&gameMenu[1], strBuffer, gameMenu[1].selection1);
                        }
                    }
                }
            }
            else if (keyPress.B) {
                initStartMenu(0);
            }
            break;
        }
        case STARTMENU_PLAYERSEL: {
            if (keyPress.down)
                ++gameMenu[1].selection1;
            if (keyPress.up)
                --gameMenu[1].selection1;
            if (gameMenu[1].selection1 == gameMenu[1].rowCount)
                gameMenu[1].selection1 = 0;

            if (gameMenu[1].selection1 < 0)
                gameMenu[1].selection1 = gameMenu[1].rowCount - 1;

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 4, 72);
            DrawTextMenu(&gameMenu[1], SCREEN_CENTERX - 40, 96);
            if (keyPress.start || keyPress.A) {
                int saveSlot = gameMenu[1].selection2;
                int savePos  = saveSlot << 3;

                if (saveSlot < 0) {
                    SetGlobalVariableByName("options.gameMode", 0);
                    saveSlot = 0;
                }
                else {
                    SetGlobalVariableByName("options.gameMode", 1);
                    SetGlobalVariableByName("options.stageSelectFlag", 0);
                }
                saveRAM[savePos + 0] = gameMenu[1].selection1;
                saveRAM[savePos + 1] = 3;
                saveRAM[savePos + 2] = 0;
                saveRAM[savePos + 3] = 50000;
                saveRAM[savePos + 4] = 1;
                saveRAM[savePos + 5] = 0;
                saveRAM[savePos + 6] = 0;
                saveRAM[savePos + 7] = 0;

                SetGlobalVariableByName("options.saveSlot", saveSlot);
                SetGlobalVariableByName("player.lives", saveRAM[savePos + 1]);
                SetGlobalVariableByName("player.score", saveRAM[savePos + 2]);
                SetGlobalVariableByName("player.scoreBonus", saveRAM[savePos + 3]);
                SetGlobalVariableByName("specialStage.emeralds", saveRAM[savePos + 5]);
                SetGlobalVariableByName("specialStage.listPos", saveRAM[savePos + 6]);
                SetGlobalVariableByName("stage.player2Enabled", saveRAM[savePos + 0] == 3);
                SetGlobalVariableByName("lampPostID", 0); // For S1
                SetGlobalVariableByName("starPostID", 0); // For S2
                SetGlobalVariableByName("options.vsMode", 0);
                WriteSaveRAMData();

                InitStartingStage(STAGELIST_PRESENTATION, 0, saveRAM[savePos + 0]);
                Engine.finishedStartMenu = true;
            }
            else if (keyPress.B) {
                setTextMenu(STARTMENU_SAVESEL);
            }
            break;
        }
        case STARTMENU_GAMEOPTS: {
            if (keyPress.down)
                gameMenu[1].selection1 += 2;
            if (keyPress.up)
                gameMenu[1].selection1 -= 2;
            if (gameMenu[1].selection1 >= gameMenu[1].rowCount)
                gameMenu[1].selection1 = 0;

            if (gameMenu[1].selection1 < 0)
                gameMenu[1].selection1 = gameMenu[1].rowCount - 1;

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 4, 72);
            DrawTextMenu(&gameMenu[1], SCREEN_CENTERX - 40, 96);
            if (keyPress.left || keyPress.right || keyPress.start) {
                if (Engine.gameType == GAME_SONIC1) {
                    switch (gameMenu[1].selection1) {
                        case 0: // Spindash
                            SetGlobalVariableByName("options.spindash", GetGlobalVariableByName("options.spindash") ^ 1);
                            if (GetGlobalVariableByName("options.spindash"))
                                SetTextMenuEntry(&gameMenu[1], "SPINDASH: ENABLED", 0);
                            else
                                SetTextMenuEntry(&gameMenu[1], "SPINDASH: DISABLED", 0);
                            break;
                        case 2: // Ground Spd Cap
                            SetGlobalVariableByName("options.speedCap", GetGlobalVariableByName("options.speedCap") ^ 1);
                            if (GetGlobalVariableByName("options.speedCap"))
                                SetTextMenuEntry(&gameMenu[1], "GROUND SPEED CAP: ENABLED", 2);
                            else
                                SetTextMenuEntry(&gameMenu[1], "GROUND SPEED CAP: DISABLED", 2);
                            break;
                        case 4: // Air Spd Cap
                            SetGlobalVariableByName("options.airSpeedCap", GetGlobalVariableByName("options.airSpeedCap") ^ 1);
                            if (GetGlobalVariableByName("options.airSpeedCap"))
                                SetTextMenuEntry(&gameMenu[1], "AIR SPEED CAP: ENABLED", 4);
                            else
                                SetTextMenuEntry(&gameMenu[1], "AIR SPEED CAP: DISABLED", 4);
                            break;
                        case 6: // S1 Spikes
                            SetGlobalVariableByName("options.spikeBehavior", GetGlobalVariableByName("options.spikeBehavior") ^ 1);
                            if (GetGlobalVariableByName("options.spikeBehavior"))
                                SetTextMenuEntry(&gameMenu[1], "S1 SPIKES: ENABLED", 6);
                            else
                                SetTextMenuEntry(&gameMenu[1], "S1 SPIKES: DISABLED", 6);
                            break;
                        case 8: {
                            if (keyPress.left) {
                                int var = (GetGlobalVariableByName("options.shieldType") - 1);
                                if (var < 0)
                                    var = 3;
                                SetGlobalVariableByName("options.shieldType", var);
                            }
                            else
                                SetGlobalVariableByName("options.shieldType", (GetGlobalVariableByName("options.shieldType") + 1) % 4);

                            int type                   = GetGlobalVariableByName("options.shieldType");
                            char itemBoxTypes[4][0x20] = { "ITEM TYPE: S1", "ITEM TYPE: S2", "ITEM TYPE: S1+S3", "ITEM TYPE: S2+S3" };
                            SetTextMenuEntry(&gameMenu[1], itemBoxTypes[type], 8);
                            break;
                        }
                        case 10: // Super forms
                            SetGlobalVariableByName("options.superStates", GetGlobalVariableByName("options.superStates") ^ 1);
                            if (GetGlobalVariableByName("options.superStates"))
                                SetTextMenuEntry(&gameMenu[1], "SUPER FORMS: ENABLED", 10);
                            else
                                SetTextMenuEntry(&gameMenu[1], "SUPER FORMS: DISABLED", 10);
                            break;
                    }
                }
                else {
                    switch (gameMenu[1].selection1) {
                        case 0:
                            SetGlobalVariableByName("options.airSpeedCap", GetGlobalVariableByName("options.airSpeedCap") ^ 1);
                            if (GetGlobalVariableByName("options.airSpeedCap"))
                                SetTextMenuEntry(&gameMenu[1], "AIR SPEED CAP: ENABLED", 0);
                            else
                                SetTextMenuEntry(&gameMenu[1], "AIR SPEED CAP: DISABLED", 0);
                            break;
                        case 2:
                            SetGlobalVariableByName("options.tailsFlight", GetGlobalVariableByName("options.tailsFlight") ^ 1);
                            if (GetGlobalVariableByName("options.tailsFlight"))
                                SetTextMenuEntry(&gameMenu[1], "TAILS FLIGHT: ENABLED", 2);
                            else
                                SetTextMenuEntry(&gameMenu[1], "TAILS FLIGHT: DISABLED", 2);
                            break;
                        case 4:
                            SetGlobalVariableByName("options.superTails", GetGlobalVariableByName("options.superTails") ^ 1);
                            if (GetGlobalVariableByName("options.superTails"))
                                SetTextMenuEntry(&gameMenu[1], "SUPER TAILS: ENABLED", 4);
                            else
                                SetTextMenuEntry(&gameMenu[1], "SUPER TAILS: DISABLED", 4);
                            break;
                        case 6:
                            SetGlobalVariableByName("options.spikeBehavior", GetGlobalVariableByName("options.spikeBehavior") ^ 1);
                            if (GetGlobalVariableByName("options.spikeBehavior"))
                                SetTextMenuEntry(&gameMenu[1], "S1 SPIKES: ENABLED", 6);
                            else
                                SetTextMenuEntry(&gameMenu[1], "S1 SPIKES: DISABLED", 6);
                            break;
                        case 8: {
                            if (keyPress.left) {
                                int var = (GetGlobalVariableByName("options.shieldType") - 1);
                                if (var < 0)
                                    var = 3;
                                SetGlobalVariableByName("options.shieldType", var);
                            }
                            else
                                SetGlobalVariableByName("options.shieldType", (GetGlobalVariableByName("options.shieldType") + 1) % 4);

                            int type                   = GetGlobalVariableByName("options.shieldType");
                            char itemBoxTypes[4][0x20] = { "ITEM TYPE: S2", "ITEM TYPE: S2+S3", "ITEM TYPE: RANDOM", "ITEM TYPE: RANDOM+S3" };
                            SetTextMenuEntry(&gameMenu[1], itemBoxTypes[type], 8);
                            break;
                        }
                    }
                }
            }
            else if (keyPress.B) {
                setTextMenu(STARTMENU_SAVESEL);

                saveRAM[0x100] = Engine.gameType;
                if (Engine.gameType == GAME_SONIC1) {
                    saveRAM[0x101] = GetGlobalVariableByName("options.spindash");
                    saveRAM[0x102] = GetGlobalVariableByName("options.speedCap");
                    saveRAM[0x103] = GetGlobalVariableByName("options.airSpeedCap");
                    saveRAM[0x104] = GetGlobalVariableByName("options.spikeBehavior");
                    saveRAM[0x105] = GetGlobalVariableByName("options.shieldType");
                    saveRAM[0x106] = GetGlobalVariableByName("options.superStates");
                }
                else {
                    saveRAM[0x101] = GetGlobalVariableByName("options.airSpeedCap");
                    saveRAM[0x102] = GetGlobalVariableByName("options.tailsFlight");
                    saveRAM[0x103] = GetGlobalVariableByName("options.superTails");
                    saveRAM[0x104] = GetGlobalVariableByName("options.spikeBehavior");
                    saveRAM[0x105] = GetGlobalVariableByName("options.shieldType");
                }
                WriteSaveRAMData();
            }
            break;
        }
        case STARTMENU_TASTAGESEL: {
            if (keyDown.down) {
                gameMenu[1].timer += 1;
                if (gameMenu[1].timer > 8) {
                    gameMenu[1].timer = 0;
                    keyPress.down     = true;
                }
            }
            else {
                if (keyDown.up) {
                    gameMenu[1].timer -= 1;
                    if (gameMenu[1].timer < -8) {
                        gameMenu[1].timer = 0;
                        keyPress.up       = true;
                    }
                }
                else {
                    gameMenu[1].timer = 0;
                }
            }
            if (keyPress.down) {
                gameMenu[1].selection1 += 2;
                if (gameMenu[1].selection1 - gameMenu[1].visibleRowOffset >= gameMenu[1].visibleRowCount) {
                    gameMenu[1].visibleRowOffset += 2;
                }
            }
            if (keyPress.up) {
                gameMenu[1].selection1 -= 2;
                if (gameMenu[1].selection1 - gameMenu[1].visibleRowOffset < 0) {
                    gameMenu[1].visibleRowOffset -= 2;
                }
            }
            if (gameMenu[1].selection1 >= gameMenu[1].rowCount) {
                gameMenu[1].selection1       = 1;
                gameMenu[1].visibleRowOffset = 0;
            }
            if (gameMenu[1].selection1 < 0) {
                gameMenu[1].selection1       = gameMenu[1].rowCount - 1;
                gameMenu[1].visibleRowOffset = gameMenu[1].rowCount - gameMenu[1].visibleRowCount;
            }

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 4, 40);
            DrawTextMenu(&gameMenu[1], SCREEN_CENTERX, 64);

            if (keyPress.start || keyPress.A) {
                int listPos = (gameMenu[1].selection1 - 1) / 2;
                int max     = stageListCount[STAGELIST_REGULAR];
                for (int s = 0; s < stageListCount[STAGELIST_REGULAR]; ++s) {
                    if (StrComp(stageList[STAGELIST_REGULAR][s].name, "STAGE MENU")) {
                        max = s;
                        break;
                    }
                }

                if (Engine.gameType == GAME_SONIC2) {
                    if (listPos >= 17)
                        listPos++; // SCZ patch
                    if (listPos >= 19)
                        listPos++; // DEZ patch
                }

                if (listPos < max) {
                    activeStageList   = STAGELIST_REGULAR;
                    stageListPosition = listPos;
                }
                else {
                    if (Engine.gameType == GAME_SONIC1) {
                        activeStageList   = STAGELIST_SPECIAL;
                        stageListPosition = listPos - max;
                    }
                    else if (Engine.gameType == GAME_SONIC2) {
                        activeStageList   = STAGELIST_BONUS;
                        stageListPosition = listPos - max;
                        if (stageListPosition < 2)
                            stageListPosition ^= 1;
                    }
                }

                if (!saveRAM[0x40]) {
                    for (int s = 0; s < (gameMenu[1].rowCount / 2) * 3; ++s) {
                        saveRAM[s + 0x40] = 60000;
                    }
                    WriteSaveRAMData();
                }
                if (!saveRAM[3 * (listPos) + 0x40]) {
                    for (int s = 0; s < 3; ++s) {
                        saveRAM[(3 * (listPos) + 0x40) + s] = 60000;
                    }
                    WriteSaveRAMData();
                }

                if (Engine.gameType == GAME_SONIC2) {
                    if (listPos >= 17)
                        listPos--; // SCZ patch 2
                    if (listPos >= 19)
                        listPos--; // DEZ patch 2
                }

                char strBuffer[0x100];
                SetupTextMenu(&gameMenu[0], 0);
                AddTextMenuEntry(&gameMenu[0], "BEST TIMES");
                AddTextMenuEntry(&gameMenu[0], "");
                StrCopy(strBuffer, "1ST: ");
                int mins = saveRAM[3 * (listPos) + 0x40] / 6000;
                int secs = saveRAM[3 * (listPos) + 0x40] / 100 % 60;
                int ms   = saveRAM[3 * (listPos) + 0x40] % 100;
                if (mins < 10)
                    AppendIntegerToString(strBuffer, 0);
                AppendIntegerToString(strBuffer, mins);
                StrAdd(strBuffer, ":");
                if (secs < 10)
                    AppendIntegerToString(strBuffer, 0);
                AppendIntegerToString(strBuffer, secs);
                StrAdd(strBuffer, ":");
                if (ms < 10)
                    AppendIntegerToString(strBuffer, 0);
                AppendIntegerToString(strBuffer, ms);
                AddTextMenuEntry(&gameMenu[0], strBuffer);
                AddTextMenuEntry(&gameMenu[0], "");
                StrCopy(strBuffer, "2ND: ");
                mins = saveRAM[3 * (listPos) + 0x41] / 6000;
                secs = saveRAM[3 * (listPos) + 0x41] / 100 % 60;
                ms   = saveRAM[3 * (listPos) + 0x41] % 100;
                if (mins < 10)
                    AppendIntegerToString(strBuffer, 0);
                AppendIntegerToString(strBuffer, mins);
                StrAdd(strBuffer, ":");
                if (secs < 10)
                    AppendIntegerToString(strBuffer, 0);
                AppendIntegerToString(strBuffer, secs);
                StrAdd(strBuffer, ":");
                if (ms < 10)
                    AppendIntegerToString(strBuffer, 0);
                AppendIntegerToString(strBuffer, ms);
                AddTextMenuEntry(&gameMenu[0], strBuffer);
                AddTextMenuEntry(&gameMenu[0], "");
                StrCopy(strBuffer, "3RD: ");
                mins = saveRAM[3 * (listPos) + 0x42] / 6000;
                secs = saveRAM[3 * (listPos) + 0x42] / 100 % 60;
                ms   = saveRAM[3 * (listPos) + 0x42] % 100;
                if (mins < 10)
                    AppendIntegerToString(strBuffer, 0);
                AppendIntegerToString(strBuffer, mins);
                StrAdd(strBuffer, ":");
                if (secs < 10)
                    AppendIntegerToString(strBuffer, 0);
                AppendIntegerToString(strBuffer, secs);
                StrAdd(strBuffer, ":");
                if (ms < 10)
                    AppendIntegerToString(strBuffer, 0);
                AppendIntegerToString(strBuffer, ms);
                AddTextMenuEntry(&gameMenu[0], strBuffer);
                AddTextMenuEntry(&gameMenu[0], "");

                SetupTextMenu(&gameMenu[1], 0);
                AddTextMenuEntry(&gameMenu[1], "PLAY");
                AddTextMenuEntry(&gameMenu[1], "");
                AddTextMenuEntry(&gameMenu[1], "BACK");
                AddTextMenuEntry(&gameMenu[1], "");

                gameMenu[1].alignment       = 2;
                gameMenu[1].selectionCount  = 1;
                gameMenu[1].selection1      = 0;
                gameMenu[1].selection2      = listPos;
                gameMenu[1].visibleRowCount = 0;

                gameMenu[0].alignment      = 2;
                gameMenu[0].selectionCount = 1;
                gameMenu[1].timer          = 0;

                stageMode = STARTMENU_TACONFIRMSEL;
            }
            else if (keyPress.B) {
                initStartMenu(0);
            }
            break;
        }
        case STARTMENU_TACONFIRMSEL: {
            if (keyPress.down)
                gameMenu[1].selection1 += 2;
            if (keyPress.up)
                gameMenu[1].selection1 -= 2;

            if (gameMenu[1].selection1 > 3)
                gameMenu[1].selection1 = 0;

            if (gameMenu[1].selection1 < 0)
                gameMenu[1].selection1 = 2;

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 4, 40);
            DrawTextMenu(&gameMenu[1], SCREEN_CENTERX, SCREEN_CENTERY);

            if (keyPress.start || keyPress.A) {
                if (gameMenu[1].selection1 == 0) {
                    SetGlobalVariableByName("options.saveSlot", 0);
                    SetGlobalVariableByName("options.gameMode", 2);
                    SetGlobalVariableByName("stage.player2Enabled", false);
                    SetGlobalVariableByName("player.lives", 1);
                    SetGlobalVariableByName("player.score", 0);
                    SetGlobalVariableByName("player.scoreBonus", 50000);
                    SetGlobalVariableByName("specialStage.listPos", 0);
                    SetGlobalVariableByName("specialStage.emeralds", 0);
                    SetGlobalVariableByName("specialStage.nextZone", 0);
                    SetGlobalVariableByName("specialStage.nextZone", 0);
                    SetGlobalVariableByName("lampPostID", 0); // For S1
                    SetGlobalVariableByName("starPostID", 0); // For S2
                    SetGlobalVariableByName("timeAttack.result", 0);

                    if (Engine.gameType == GAME_SONIC1) {
                        SetGlobalVariableByName("options.spindash", 1);
                        SetGlobalVariableByName("options.speedCap", 0);
                        SetGlobalVariableByName("options.airSpeedCap", 0);
                        SetGlobalVariableByName("options.spikeBehavior", 0);
                        SetGlobalVariableByName("options.shieldType", 0);
                        SetGlobalVariableByName("options.superStates", 0);
                    }
                    else {
                        SetGlobalVariableByName("options.airSpeedCap", 0);
                        SetGlobalVariableByName("options.tailsFlight", 1);
                        SetGlobalVariableByName("options.superTails", 1);
                        SetGlobalVariableByName("options.spikeBehavior", 0);
                        SetGlobalVariableByName("options.shieldType", 0);
                    }

                    taListStore = gameMenu[1].selection2;
                    InitStartingStage(activeStageList, stageListPosition, 0);
                    Engine.finishedStartMenu = true;
                }
                else {
                    // TA
                    setTextMenu(STARTMENU_TASTAGESEL);
                }
            }
            break;
        }
        case STARTMENU_ACHIEVEMENTS: {
            if (keyDown.down) {
                gameMenu[1].timer += 1;
                if (gameMenu[1].timer > 8) {
                    gameMenu[1].timer = 0;
                    keyPress.down     = true;
                }
            }
            else {
                if (keyDown.up) {
                    gameMenu[1].timer -= 1;
                    if (gameMenu[1].timer < -8) {
                        gameMenu[1].timer = 0;
                        keyPress.up       = true;
                    }
                }
                else {
                    gameMenu[1].timer = 0;
                }
            }
            if (keyPress.down) {
                gameMenu[1].selection1 += 2;
                if (gameMenu[1].selection1 - gameMenu[1].visibleRowOffset >= gameMenu[1].visibleRowCount) {
                    gameMenu[1].visibleRowOffset += 2;
                }
            }
            if (keyPress.up) {
                gameMenu[1].selection1 -= 2;
                if (gameMenu[1].selection1 - gameMenu[1].visibleRowOffset < 0) {
                    gameMenu[1].visibleRowOffset -= 2;
                }
            }
            if (gameMenu[1].selection1 >= gameMenu[1].rowCount) {
                gameMenu[1].selection1       = 1;
                gameMenu[1].visibleRowOffset = 0;
            }
            if (gameMenu[1].selection1 < 0) {
                gameMenu[1].selection1       = gameMenu[1].rowCount - 1;
                gameMenu[1].visibleRowOffset = gameMenu[1].rowCount - gameMenu[1].visibleRowCount;
            }

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 4, 72);
            DrawTextMenu(&gameMenu[1], 16, 96);
            if (keyPress.B) {
                initStartMenu(0);
            }
            break;
        }
        default: break;
    }

    if (!Engine.finishedStartMenu) {
#if defined RETRO_USING_MOUSE || defined RETRO_USING_TOUCH
        DrawSprite(32, 0x42, 16, 16, 78, 240, textMenuSurfaceNo);
        DrawSprite(32, 0xB2, 16, 16, 95, 240, textMenuSurfaceNo);
        DrawSprite(SCREEN_XSIZE - 32, SCREEN_YSIZE - 32, 16, 16, 112, 240, textMenuSurfaceNo);
#endif
    }
}