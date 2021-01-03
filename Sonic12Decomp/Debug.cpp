#include "RetroEngine.hpp"

#if RSDK_DEBUG
bool endLine = true;
#endif

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
    textMenuSurfaceNo = 0;
    LoadGIFFile("Data/Game/SystemText.gif", 0);
    SetPaletteEntry(-1, 0xF0, 0x00, 0x00, 0x00);
    SetPaletteEntry(-1, 0xFF, 0xFF, 0xFF, 0xFF);
    stageMode = DEVMENU_MAIN;
    SetupTextMenu(&gameMenu[0], 0);
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
    gameMenu[0].alignment        = 2;
    gameMenu[0].selectionCount   = 2;
    gameMenu[0].selection1       = 0;
    gameMenu[0].selection2       = 9;
    gameMenu[1].visibleRowCount  = 0;
    gameMenu[1].visibleRowOffset = 0;
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
    textMenuSurfaceNo = 0;
    LoadGIFFile("Data/Game/SystemText.gif", 0);
    SetPaletteEntry(-1, 0xF0, 0x00, 0x00, 0x00);
    SetPaletteEntry(-1, 0xFF, 0xFF, 0xFF, 0xFF);
    gameMenu[0].alignment        = 2;
    gameMenu[0].selectionCount   = 1;
    gameMenu[0].selection1       = 0;
    gameMenu[1].visibleRowCount  = 0;
    gameMenu[1].visibleRowOffset = 0;
    stageMode                    = DEVMENU_SCRIPTERROR;
}
void processStageSelect()
{
    ClearScreen(0xF0);
    CheckKeyDown(&keyDown, 0x13);
    CheckKeyPress(&keyPress, 0xB3);

    switch (stageMode) {
        case DEVMENU_MAIN: // Main Menu
        {
            if (keyPress.down)
                gameMenu[0].selection2 += 2;

            if (keyPress.up)
                gameMenu[0].selection2 -= 2;

            if (gameMenu[0].selection2 > 11)
                gameMenu[0].selection2 = 9;
            if (gameMenu[0].selection2 < 9)
                gameMenu[0].selection2 = 11;

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
                else {
                    SetupTextMenu(&gameMenu[0], 0);
                    AddTextMenuEntry(&gameMenu[0], "SELECT A PLAYER");
                    SetupTextMenu(&gameMenu[1], 0);
                    LoadConfigListText(&gameMenu[1], 0);
                    gameMenu[1].alignment      = 0;
                    gameMenu[1].selectionCount = 1;
                    gameMenu[1].selection1     = 0;
                    stageMode                  = DEVMENU_PLAYERSEL;
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
                SetupTextMenu(&gameMenu[0], 0);
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
                gameMenu[0].alignment  = 0;
                gameMenu[0].selection2 = 3;
                stageMode              = DEVMENU_STAGELISTSEL;
            }
            else if (keyPress.B) {
                stageMode = DEVMENU_MAIN;
                SetupTextMenu(&gameMenu[0], 0);
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
                gameMenu[0].alignment        = 2;
                gameMenu[0].selectionCount   = 2;
                gameMenu[0].selection1       = 0;
                gameMenu[0].selection2       = 9;
                gameMenu[1].visibleRowCount  = 0;
                gameMenu[1].visibleRowOffset = 0;
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
                case 3: //Presentation
                    if (stageListCount[0] > 0)
                        nextMenu = true;
                    activeStageList = 0;
                    break;
                case 5: //Regular
                    if (stageListCount[1] > 0)
                        nextMenu = true;
                    activeStageList = 1;
                    break;
                case 7: //Special
                    if (stageListCount[3] > 0)
                        nextMenu = true;
                    activeStageList = 3;
                    break;
                case 9: //Bonus
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
                gameMenu[0].alignment      = 2;
                gameMenu[1].alignment      = 0;
                gameMenu[1].selectionCount = 1;
                gameMenu[1].selection1     = playerListPos;
                stageMode                  = DEVMENU_PLAYERSEL;
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
            }
            else if (keyPress.B) {
                SetupTextMenu(&gameMenu[0], 0);
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
                gameMenu[0].selection2     = (activeStageList << 1) + 3;
                gameMenu[0].selection2     = gameMenu[0].selection2 == 7 ? 9 : gameMenu[0].selection2 == 9 ? 7 : gameMenu[0].selection2;
                gameMenu[0].selectionCount = 2;
                stageMode                  = DEVMENU_STAGELISTSEL;
            }
            break;
        }
        case DEVMENU_SCRIPTERROR: // Script Error
        {
            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX, 72);
            if (keyPress.start || keyPress.A) {
                stageMode = DEVMENU_STAGESEL;
                SetupTextMenu(&gameMenu[0], 0);
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
                gameMenu[0].alignment        = 2;
                gameMenu[0].selectionCount   = 2;
                gameMenu[0].selection1       = 0;
                gameMenu[0].selection2       = 9;
                gameMenu[1].visibleRowCount  = 0;
                gameMenu[1].visibleRowOffset = 0;
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

void initStartMenu(int mode) {
    // DrawStageGFXHQ = 0;
    xScrollOffset = 0;
    yScrollOffset = 0;
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    fadeMode        = 0;
    playerListPos   = 0;
    Engine.gameMode = ENGINE_MAINGAME;
    Engine.finishedStartMenu = false;
    ClearGraphicsData();
    ClearAnimationData();
    SetActivePalette(0, 0, 256);
    textMenuSurfaceNo = 0;
    LoadGIFFile("Data/Game/SystemText.gif", 0);
    SetPaletteEntry(-1, 0xF0, 0x00, 0x00, 0x00);
    SetPaletteEntry(-1, 0xFF, 0xFF, 0xFF, 0xFF);

    if (mode == 0) {
        stageMode = STARTMENU_MAIN;
        SetupTextMenu(&gameMenu[0], 0);
        char title[0x80];
        StrCopy(title, Engine.gameWindowText);
        StrAdd(title, " START MENU");
        AddTextMenuEntry(&gameMenu[0], title);
        AddTextMenuEntry(&gameMenu[0], " ");
        AddTextMenuEntry(&gameMenu[0], " ");
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
    }
    else {
        //finished TA act
        int listPos = stageListPosition;
        int max     = listPos < stageListCount[STAGELIST_REGULAR];
        for (int s = 0; s < stageListCount[STAGELIST_REGULAR]; ++s) {
            if (StrComp(stageList[STAGELIST_REGULAR][s].name, "STAGE MENU")) {
                max = s;
                break;
            }
        }

        if (stageListPosition >= max) {
            if (activeStageList == STAGELIST_BONUS) {
                listPos ^= 1;
                listPos += max;
            }
        }

        int result = GetGlobalVariableByName("timeAttack.result");
        if (result < saveRAM[3 * listPos + 0x40]) {
            saveRAM[3 * listPos + 0x40] = result;
        }
        else if (result < saveRAM[3 * listPos + 0x41]) {
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
            AppendIntegerToSting(strBuffer, 0);
        AppendIntegerToSting(strBuffer, mins);
        StrAdd(strBuffer, ":");
        if (secs < 10)
            AppendIntegerToSting(strBuffer, 0);
        AppendIntegerToSting(strBuffer, secs);
        StrAdd(strBuffer, ":");
        if (ms < 10)
            AppendIntegerToSting(strBuffer, 0);
        AppendIntegerToSting(strBuffer, ms);
        AddTextMenuEntry(&gameMenu[0], strBuffer);
        AddTextMenuEntry(&gameMenu[0], "");
        StrCopy(strBuffer, "2ND: ");
        mins = saveRAM[3 * (listPos) + 0x41] / 6000;
        secs = saveRAM[3 * (listPos) + 0x41] / 100 % 60;
        ms   = saveRAM[3 * (listPos) + 0x41] % 100;
        if (mins < 10)
            AppendIntegerToSting(strBuffer, 0);
        AppendIntegerToSting(strBuffer, mins);
        StrAdd(strBuffer, ":");
        if (secs < 10)
            AppendIntegerToSting(strBuffer, 0);
        AppendIntegerToSting(strBuffer, secs);
        StrAdd(strBuffer, ":");
        if (ms < 10)
            AppendIntegerToSting(strBuffer, 0);
        AppendIntegerToSting(strBuffer, ms);
        AddTextMenuEntry(&gameMenu[0], strBuffer);
        AddTextMenuEntry(&gameMenu[0], "");
        StrCopy(strBuffer, "3RD: ");
        mins = saveRAM[3 * (listPos) + 0x42] / 6000;
        secs = saveRAM[3 * (listPos) + 0x42] / 100 % 60;
        ms   = saveRAM[3 * (listPos) + 0x42] % 100;
        if (mins < 10)
            AppendIntegerToSting(strBuffer, 0);
        AppendIntegerToSting(strBuffer, mins);
        StrAdd(strBuffer, ":");
        if (secs < 10)
            AppendIntegerToSting(strBuffer, 0);
        AppendIntegerToSting(strBuffer, secs);
        StrAdd(strBuffer, ":");
        if (ms < 10)
            AppendIntegerToSting(strBuffer, 0);
        AppendIntegerToSting(strBuffer, ms);
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

void processStartMenu() {
    ClearScreen(0xF0);
    CheckKeyDown(&keyDown, 0xFF);
    CheckKeyPress(&keyPress, 0xFF);

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
                    ushort strBuffer[0x100];
                    SetupTextMenu(&gameMenu[0], 0);
                    AddTextMenuEntry(&gameMenu[0], "SELECT A SAVE FILE");
                    SetupTextMenu(&gameMenu[1], 0);
                    AddTextMenuEntry(&gameMenu[1], "DELETE SAVE FILE");
                    AddTextMenuEntry(&gameMenu[1], "");
                    AddTextMenuEntryW(&gameMenu[1], strNoSave);

                    for (int s = 0; s < 4; ++s) {
                        AddTextMenuEntry(&gameMenu[1], "");

                        StrCopyW(strBuffer, "SAVE ");
                        AppendIntegerToStingW(strBuffer, s + 1);
                        StrAddW(strBuffer, " - ");
                        if (saveRAM[s * 4 + 4]) {
                            StrAddW(strBuffer, playerListText[saveRAM[s * 4 + 0]]);
                            StrAddW(strBuffer, " - ");
                            StrAddW(strBuffer, strStageList[(saveRAM[s * 4 + 4] - 1) / 2]);
                            StrAddW(strBuffer, " ");
                            AppendIntegerToStingW(strBuffer, (saveRAM[s * 4 + 4] - 1) % 2 + 1);
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
                    stageMode                  = STARTMENU_SAVESEL;
                }
                else if (gameMenu[0].selection2 == 11) {
                    //TA
                    ushort strBuffer[0x100];
                    SetupTextMenu(&gameMenu[0], 0);
                    AddTextMenuEntry(&gameMenu[0], "SELECT A STAGE");
                    SetupTextMenu(&gameMenu[1], 0);
                    int cnt = 0;
                    for (int i = 0; i < stageStrCount; ++i) {
                        if (strSaveStageList[i] && !StrCompW(strSaveStageList[i], "Complete")
                            && !(Engine.gameType == GAME_SONIC2 && StrCompW(strSaveStageList[i], "Special Stage 6"))) {
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
                    stageMode                  = STARTMENU_TASTAGESEL;
                }
                else if (gameMenu[0].selection2 == 13) {
                    SetupTextMenu(&gameMenu[0], 0);
                    AddTextMenuEntry(&gameMenu[0], "ACHIEVEMENTS LIST");
                    SetupTextMenu(&gameMenu[1], 0);

                    char strBuffer[0x80];
                    for (int i = 0; i < ACHIEVEMENT_MAX; ++i) {
                        if (!StrComp(achievements[i].name, "")) {
                            AddTextMenuEntry(&gameMenu[1], "");
                            StrCopy(strBuffer, achievements[i].name);
                            StrAdd(strBuffer, ": ");
                            StrAdd(strBuffer, achievements[i].status == 100 ? "achieved" : "not achieved");
                            AddTextMenuEntry(&gameMenu[1], strBuffer);
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
                    stageMode = STARTMENU_ACHIEVEMENTS;
                }
                else if (gameMenu[0].selection2 == 15) {
                    //TODO:
                }
                else {
                    //2P VS
                    SetGlobalVariableByName("options.saveSlot", 0);
                    SetGlobalVariableByName("options.gameMode", 0);
                    SetGlobalVariableByName("options.vsMode", 0);
                    SetGlobalVariableByName("player.lives", 3);
                    SetGlobalVariableByName("player.score", 0);
                    SetGlobalVariableByName("player.scoreBonus", 50000);
                    SetGlobalVariableByName("specialStage.listPos", 0);
                    SetGlobalVariableByName("specialStage.emeralds", 0);
                    SetGlobalVariableByName("specialStage.nextZone", 0);
                    SetGlobalVariableByName("timeAttack.result", 0);
                    SetGlobalVariableByName("lampPostID", 0); // For S1
                    SetGlobalVariableByName("starPostID", 0); // For S2

                    //if (Engine.onlineActive)
                    InitStartingStage(STAGELIST_PRESENTATION, 3, 0);
                    Engine.finishedStartMenu = true;
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

            if (gameMenu[1].selection1 > 10)
                gameMenu[1].selection1 = 0;
            if (gameMenu[1].selection1 < 0)
                gameMenu[1].selection1 = 10;

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 4, 72);
            DrawTextMenu(&gameMenu[1], 16, 96);
            if (keyPress.start || keyPress.A) {
                if (gameMenu[1].selection1 == 0) {
                    if (!gameMenu[1].selection2) {
                        SetTextMenuEntry(&gameMenu[1], "CANCEL", 0);
                        gameMenu[1].selection2 ^= 1;
                    }
                    else {
                        SetTextMenuEntry(&gameMenu[1], "DELETE SAVE FILE", 0);
                        gameMenu[1].selection2 ^= 1;
                    }
                }
                else {
                    int saveSlot = (gameMenu[1].selection1 / 2) - 2;
                    if (!gameMenu[1].selection2) {
                        if (saveSlot >= 0 && saveSlot < 4) {
                            if (saveRAM[8 * saveSlot + 4]) {
                                SetGlobalVariableByName("options.saveSlot", saveSlot);
                                SetGlobalVariableByName("options.gameMode", 1);
                                SetGlobalVariableByName("options.stageSelectFlag", 0);
                                SetGlobalVariableByName("player.lives", saveRAM[8 * saveSlot + 1]);
                                SetGlobalVariableByName("player.score", saveRAM[8 * saveSlot + 2]);
                                SetGlobalVariableByName("player.scoreBonus", saveRAM[8 * saveSlot + 3]);
                                SetGlobalVariableByName("specialStage.emeralds", saveRAM[8 * saveSlot + 5]);
                                SetGlobalVariableByName("specialStage.listPos", saveRAM[8 * saveSlot + 6]);
                                SetGlobalVariableByName("lampPostID", 0); // For S1
                                SetGlobalVariableByName("starPostID", 0); // For S2
                                SetGlobalVariableByName("options.vsMode", 0);

                                int nextZone = saveRAM[8 * saveSlot + 4];
                                if (nextZone > 127) {
                                    SetGlobalVariableByName("specialStage.nextZone", nextZone - 129);
                                    InitStartingStage(STAGELIST_SPECIAL, saveRAM[8 * saveSlot + 6], saveRAM[8 * saveSlot + 0]);
                                }
                                else {
                                    SetGlobalVariableByName("specialStage.nextZone", nextZone - 1);
                                    InitStartingStage(STAGELIST_REGULAR, saveRAM[8 * saveSlot + 4] - 1, saveRAM[8 * saveSlot + 0]);
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
                            saveRAM[8 * saveSlot + 0] = 0;
                            saveRAM[8 * saveSlot + 1] = 3;
                            saveRAM[8 * saveSlot + 2] = 0;
                            saveRAM[8 * saveSlot + 3] = 50000;
                            saveRAM[8 * saveSlot + 4] = 0;
                            saveRAM[8 * saveSlot + 5] = 0;
                            saveRAM[8 * saveSlot + 6] = 0;
                            saveRAM[8 * saveSlot + 7] = 0;

                            ushort strBuffer[0x100];
                            StrCopyW(strBuffer, "SAVE ");
                            AppendIntegerToStingW(strBuffer, saveSlot + 1);
                            StrAddW(strBuffer, " - ");
                            StrAddW(strBuffer, strNewGame);

                            SetTextMenuEntryW(&gameMenu[1], strBuffer, gameMenu[1].selection1);
                        }
                    }
                }
            }
            else if(keyPress.B) { initStartMenu(0); }
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

                if (saveSlot < 0) {
                    SetGlobalVariableByName("options.gameMode", 0);
                    saveSlot = 0;
                }
                else {
                    SetGlobalVariableByName("options.gameMode", 1);
                    SetGlobalVariableByName("options.stageSelectFlag", 0);
                }
                saveRAM[8 * saveSlot + 0] = gameMenu[1].selection1;
                saveRAM[8 * saveSlot + 1] = 3;
                saveRAM[8 * saveSlot + 2] = 0;
                saveRAM[8 * saveSlot + 3] = 50000;
                saveRAM[8 * saveSlot + 4] = 0;
                saveRAM[8 * saveSlot + 5] = 0;
                saveRAM[8 * saveSlot + 6] = 0;
                saveRAM[8 * saveSlot + 7] = 0;

                SetGlobalVariableByName("options.saveSlot", saveSlot);
                SetGlobalVariableByName("player.lives", saveRAM[8 * saveSlot + 1]);
                SetGlobalVariableByName("player.score", saveRAM[8 * saveSlot + 2]);
                SetGlobalVariableByName("player.scoreBonus", saveRAM[8 * saveSlot + 3]);
                SetGlobalVariableByName("specialStage.emeralds", saveRAM[8 * saveSlot + 5]);
                SetGlobalVariableByName("specialStage.listPos", saveRAM[8 * saveSlot + 6]);
                SetGlobalVariableByName("lampPostID", 0); //For S1
                SetGlobalVariableByName("starPostID", 0); //For S2
                SetGlobalVariableByName("options.vsMode", 0);

                InitStartingStage(STAGELIST_REGULAR, 0, saveRAM[8 * saveSlot + 0]);
                Engine.finishedStartMenu = true;
            }
            else if (keyPress.B) {
                ushort strBuffer[0x100];
                SetupTextMenu(&gameMenu[0], 0);
                AddTextMenuEntry(&gameMenu[0], "SELECT A SAVE FILE");
                SetupTextMenu(&gameMenu[1], 0);
                StrCopyW(strBuffer, "");
                StrAddW(strBuffer, strNoSave);
                AddTextMenuEntry(&gameMenu[1], "DELETE SAVE FILE");
                AddTextMenuEntry(&gameMenu[1], "");
                AddTextMenuEntryW(&gameMenu[1], strBuffer);

                for (int s = 0; s < 4; ++s) {
                    AddTextMenuEntry(&gameMenu[1], "");

                    StrCopyW(strBuffer, "SAVE ");
                    AppendIntegerToStingW(strBuffer, s + 1);
                    StrAddW(strBuffer, " - ");
                    if (saveRAM[s * 4 + 4]) {
                        StrAddW(strBuffer, playerListText[saveRAM[s * 4 + 0]]);
                        StrAddW(strBuffer, " - ");
                        StrAddW(strBuffer, strStageList[(saveRAM[s * 4 + 4] - 1) / 2]);
                        StrAddW(strBuffer, " ");
                        AppendIntegerToStingW(strBuffer, (saveRAM[s * 4 + 4] - 1) % 2 + 1);
                    }
                    else {
                        StrAddW(strBuffer, strNewGame);
                    }

                    AddTextMenuEntryW(&gameMenu[1], strBuffer);
                }
                gameMenu[1].alignment      = 0;
                gameMenu[1].selectionCount = 1;
                gameMenu[1].selection1     = 0;
                stageMode                  = STARTMENU_SAVESEL;
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
                int max     = listPos < stageListCount[STAGELIST_REGULAR];
                for (int s = 0; s < stageListCount[STAGELIST_REGULAR]; ++s) {
                    if (StrComp(stageList[STAGELIST_REGULAR][s].name, "STAGE MENU")) {
                        max = s;
                        break;
                    }
                }

                if (listPos < max) {
                    activeStageList = STAGELIST_REGULAR;
                    stageListPosition = listPos;
                }
                else {
                    if (Engine.gameType == GAME_SONIC1) {
                        activeStageList   = STAGELIST_SPECIAL;
                        stageListPosition = listPos - max;
                        if (stageListPosition < 2)
                            stageListPosition ^= 1;
                    }
                    else if (Engine.gameType == GAME_SONIC2) {
                        activeStageList = STAGELIST_BONUS;
                        stageListPosition = listPos - max;
                        if (stageListPosition < 2)
                            stageListPosition ^= 1;
                    }
                }

                if (!saveRAM[3 * (listPos) + 0x40]) {
                    for (int s = 0; s < (gameMenu[1].rowCount / 2) * 3; ++s) {
                        saveRAM[s + 0x40] = 60000;
                    }
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
                    AppendIntegerToSting(strBuffer, 0);
                AppendIntegerToSting(strBuffer, mins);
                StrAdd(strBuffer, ":");
                if (secs < 10)
                    AppendIntegerToSting(strBuffer, 0);
                AppendIntegerToSting(strBuffer, secs);
                StrAdd(strBuffer, ":");
                if (ms < 10)
                    AppendIntegerToSting(strBuffer, 0);
                AppendIntegerToSting(strBuffer, ms);
                AddTextMenuEntry(&gameMenu[0], strBuffer);
                AddTextMenuEntry(&gameMenu[0], "");
                StrCopy(strBuffer, "2ND: ");
                mins = saveRAM[3 * (listPos) + 0x41] / 6000;
                secs = saveRAM[3 * (listPos) + 0x41] / 100 % 60;
                ms   = saveRAM[3 * (listPos) + 0x41] % 100;
                if (mins < 10)
                    AppendIntegerToSting(strBuffer, 0);
                AppendIntegerToSting(strBuffer, mins);
                StrAdd(strBuffer, ":");
                if (secs < 10)
                    AppendIntegerToSting(strBuffer, 0);
                AppendIntegerToSting(strBuffer, secs);
                StrAdd(strBuffer, ":");
                if (ms < 10)
                    AppendIntegerToSting(strBuffer, 0);
                AppendIntegerToSting(strBuffer, ms);
                AddTextMenuEntry(&gameMenu[0], strBuffer);
                AddTextMenuEntry(&gameMenu[0], "");
                StrCopy(strBuffer, "3RD: ");
                mins = saveRAM[3 * (listPos) + 0x42] / 6000;
                secs = saveRAM[3 * (listPos) + 0x42] / 100 % 60;
                ms   = saveRAM[3 * (listPos) + 0x42] % 100;
                if (mins < 10)
                    AppendIntegerToSting(strBuffer, 0);
                AppendIntegerToSting(strBuffer, mins);
                StrAdd(strBuffer, ":");
                if (secs < 10)
                    AppendIntegerToSting(strBuffer, 0);
                AppendIntegerToSting(strBuffer, secs);
                StrAdd(strBuffer, ":");
                if (ms < 10)
                    AppendIntegerToSting(strBuffer, 0);
                AppendIntegerToSting(strBuffer, ms);
                AddTextMenuEntry(&gameMenu[0], strBuffer);
                AddTextMenuEntry(&gameMenu[0], "");

                SetupTextMenu(&gameMenu[1], 0);
                AddTextMenuEntry(&gameMenu[1], "PLAY");
                AddTextMenuEntry(&gameMenu[1], "");
                AddTextMenuEntry(&gameMenu[1], "BACK");
                AddTextMenuEntry(&gameMenu[1], "");

                gameMenu[1].alignment      = 2;
                gameMenu[1].selectionCount = 1;
                gameMenu[1].selection1     = 0;
                gameMenu[1].selection2     = listPos;
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
                gameMenu[1].selection1 = 3;

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 4, 40);
            DrawTextMenu(&gameMenu[1], SCREEN_CENTERX, SCREEN_CENTERY);

            if (keyPress.start || keyPress.A) {
                if (gameMenu[1].selection1 == 0) {
                    SetGlobalVariableByName("options.saveSlot", 0);
                    SetGlobalVariableByName("options.gameMode", 2);
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
                    InitStartingStage(activeStageList, stageListPosition, 0);
                    Engine.finishedStartMenu = true;
                }
                else {
                    // TA
                    ushort strBuffer[0x100];
                    SetupTextMenu(&gameMenu[0], 0);
                    AddTextMenuEntry(&gameMenu[0], "SELECT A STAGE");
                    SetupTextMenu(&gameMenu[1], 0);
                    int cnt = 0;
                    for (int i = 0; i < stageStrCount; ++i) {
                        if (strSaveStageList[i] && !StrCompW(strSaveStageList[i], "Complete") && !(Engine.gameType == GAME_SONIC2 && StrCompW(strSaveStageList[i], "Special Stage 6"))) {
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
                    stageMode                  = STARTMENU_TASTAGESEL;
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
            DrawTextMenu(&gameMenu[1], SCREEN_CENTERX - 40, 96);
            if (keyPress.B) {
                initStartMenu(0);
            }
            break;
        }
        default: break;
    }
}