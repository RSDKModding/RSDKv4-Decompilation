#include "RetroEngine.hpp"

int titleID, mainID, modID, saveID, bgID, fontID;
bool isWide;

void (*target)(NativeEntity_StartMenu *) = nullptr;

// STATES
void StartMenu_Title(NativeEntity_StartMenu *menu);
void StartMenu_MainMenu(NativeEntity_StartMenu *menu);
void StartMenu_SaveSelect(NativeEntity_StartMenu *menu);
void StartMenu_LoadStage(NativeEntity_StartMenu *menu);

void RenderText(ushort *text, int xpos, int ypos, int wrap, bool hilight)
{
    for (int i = 0; i < StrLengthW(text); i++) {
        int w = 8, h = 8;
        if (wrap && i / wrap) {
            ypos += 8;
            xpos = 0;
        }
        ushort chr = text[i];
        if (chr == ' ') {
            xpos += 2;
            continue;
        }
        int xsp = 0;
        int ysp = 0;
        if (chr >= 'A') {
            if (chr >= 'a') {
                ysp += 8 * 3;
                chr -= 0x20;
            }
            chr -= 'A';
            ysp += 8 * (chr / 9);
            xsp += 8 * (chr % 9);
        }
        else if (chr >= '0') {
            chr -= '0';
            ysp = 8 * 6;
            xsp = 8 * chr;
        }
        else {
            ysp = 8 * 7;
            /* clang-format off */
            switch (chr) {
                case '*':  w = 7; break;
                case '&':  xsp = 7;  w = 7; break;
                case '!':  xsp = 14; w = 6; break;
                case '?':  xsp = 20; w = 7; break;
                case ',':  xsp = 28; w = 4; break;
                case '\'': xsp = 32; w = 4; break;
                case '/':  xsp = 36; w = 7; break;
                case '.':  xsp = 44; w = 4; break;
                // what about the jojo thingies
                default:   ysp = 0; break;
            }
            /* clang-format on */
        }
        if (hilight)
            ysp += 8 * 8;

        RenderSprite(xpos, ypos, w, h, xsp, ysp, fontID);
        xpos += 8;
    }
}

void StartMenu_Destroy(NativeEntity_StartMenu *menu) { RemoveNativeObject(menu); }

void StartMenu_Create(void *objPtr)
{
    NativeEntity_StartMenu *startMenu = (NativeEntity_StartMenu *)objPtr;
    memset(startMenu->timers, 0, 10 * sizeof(int));

    // LET THE SHEET LOADING COMMENSE
    titleID = LoadTexture("Menu/Main/TitleScreen.png", 1);
    modID   = LoadTexture("Menu/Main/ModMenu.png", 1);
    saveID  = LoadTexture("Menu/Main/SaveSelect.png", 1);
    bgID    = LoadTexture("Menu/Main/Background.png", 1);
    mainID  = LoadTexture("Menu/Main/MainMenu.png", 1);
    fontID  = LoadTexture("Menu/Main/Font.png", 1);
    snapDataFile(0);

    // REMOVE THIS LATER BECAUSE TITLE SCREEN i just like the music
    SetMusicTrack("MainMenu.ogg", 0, 1, 106596);
    PlayMusic(0, 0);

    isWide = SCREEN_XSIZE >= 400;

    startMenu->state         = StartMenu_MainMenu;
    target                   = StartMenu_SaveSelect;
    startMenu->transitioning = true;
}
void StartMenu_Main(void *objPtr)
{
    NativeEntity_StartMenu *menu = (NativeEntity_StartMenu *)objPtr;

    // BG
    DrawRectangle(0, 0, SCREEN_XSIZE, SCREEN_YSIZE, 255, 255, 255, 255);
    // RENDER BG SPRITES HERE

    // USE RENDER AFTER ALL OF THIS POINT
    // STATE
    CheckKeyDown(&keyDown);
    CheckKeyPress(&keyPress);
    menu->state(menu);

    // BOTTOM AND TOP
    if (menu->state != StartMenu_Title && menu->state != StartMenu_MainMenu) {
        for (int i = 0; i < SCREEN_XSIZE / 24 + 2; i++) {
            RenderSpriteFlipped(i * 24 + ((48 - menu->timers[9]) / 2) - 24, 0, 24, 22, 293, 127, FLIP_Y, saveID);
            RenderSprite(i * 24 + (menu->timers[9] / 2) - 24, SCREEN_YSIZE - 22, 24, 22, 293, 127, saveID);
        }
    }

    menu->timers[9] = (menu->timers[9] + 1) % 48;

    // TRANSITION
    if (menu->transitioning) {
        if (menu->transTimer <= 0)
            menu->transTimer = 32;
        if (--menu->transTimer <= 17) {
            menu->transitioning = false;
            if (target)
                menu->state = target;
            menu->selected = 0;
        }
        int val = (menu->transTimer - 17);
        RenderRectangle(SCREEN_XSIZE / 15 * val, 0, SCREEN_XSIZE, SCREEN_YSIZE, 0, 0, 0, 255);
        for (int i = 0; i < SCREEN_YSIZE / 22 + 2; i++) {
            RenderSpriteFlipped(SCREEN_XSIZE / 15 * val - 12, i * 22 - 11, 12, 22, 33, 48, FLIP_X, mainID);
        }
    }
    else if (menu->transTimer--) {
        RenderRectangle(0, 0, SCREEN_XSIZE / 15 * menu->transTimer, SCREEN_YSIZE, 0, 0, 0, 255);
        for (int i = 0; i < SCREEN_YSIZE / 22 + 2; i++) {
            RenderSprite(SCREEN_XSIZE / 15 * menu->transTimer, i * 22 - 11, 12, 22, 33, 48, mainID);
        }
    }
}

void StartMenu_Title(NativeEntity_StartMenu *menu) {}
void StartMenu_MainMenu(NativeEntity_StartMenu *menu) { RenderRectangle(0, 0, SCREEN_XSIZE, SCREEN_YSIZE, 0, 0, 0, 255); }
void StartMenu_SaveSelect(NativeEntity_StartMenu *menu)
{
    // soft BG
    RenderRectangle(0, 0, SCREEN_XSIZE, SCREEN_YSIZE, 255, 255, 255, 128);

    // CONTROLS
    if (!menu->transitioning) {
        if (keyPress.A || keyPress.start) {
            if (menu->selected > 3)
                PlaySFXByName("Hurt", 0);
            else {
                menu->transitioning = true;
                target              = StartMenu_LoadStage;
                menu->variables[9]  = menu->selected;
                PlaySFXByName("MenuSelect", 0);
                StopMusic();
            }
        }
        if (keyPress.B) {
            menu->transitioning = true;
            target              = StartMenu_MainMenu;
            PlaySFXByName("MenuBack", 0);
        }
        if (keyPress.up || keyPress.down)
            PlaySFXByName("MenuMove", 0);
        if (keyPress.up) {
            menu->selected--;
            if (menu->selected < 0) {
                if (isWide)
                    menu->selected = 4;
                else
                    menu->selected = 7;
            }
            else if (isWide && (menu->selected != 0 && menu->selected != 2)) {
                menu->selected += 5;
                if (menu->selected == 9)
                    menu->selected = 1;
                else
                    menu->selected %= 7;
            }
        }
        else if (keyPress.down) {
            menu->selected++;
            if (isWide && (menu->selected != 1 && menu->selected != 3)) {
                menu->selected += 2;
                if (menu->selected == 8)
                    menu->selected = 0;
            }
            menu->selected %= 7;
        }
        if (isWide) {
            if (keyPress.left || keyPress.right)
                PlaySFXByName("MenuMove", 0);
            if (keyPress.left) {
                if (menu->selected > 4)
                    menu->selected--;
                else if (menu->selected < 2 || menu->selected == 4)
                    menu->selected += 2;
                else
                    menu->selected -= 2;
            }
            else if (keyPress.right) {
                if (menu->selected >= 4) {
                    menu->selected++;
                    if (menu->selected == 7)
                        menu->selected = 4;
                }
                else if (menu->selected < 2)
                    menu->selected += 2;
                else
                    menu->selected -= 2;
            }
        }
    }

    RenderSprite(SCREEN_CENTERX - 41, 31, 82, 13, 2, 1, saveID);

    // RENDER
    for (int i = 0; i < 4; i++) {
        if (i == menu->selected && menu->transitioning) {
            if (menu->transTimer / 6 > 2)
                continue;
        }
        ushort buf[0x30];
        memset(buf, 0, 0x30 * sizeof(ushort));
        ushort act[0x10];
        memset(act, 0, 0x10 * sizeof(ushort));

        int xp = SCREEN_CENTERX - (186 / 2);
        int yp = 55 + i * (3 + 46) - menu->offset;
        if (isWide) {
            xp = SCREEN_CENTERX + (i < 2 ? -(5 + 186) : 5);
            yp = 55 + (i % 2 ? 51 : 0);
        }
        RenderSprite(xp, yp, 186, 46, 289 - (menu->selected == i ? 188 : 0), 2, saveID);
        int stagePos = saveRAM[i * 8 + 4];
        if (stagePos) {
            if (stagePos >= 0x80) {
                StrAddW(buf, "SPECIAL STAGE ");
                AppendIntegerToStringW(buf, saveRAM[i * 8 + 6] + 1);
            }
            else if (StrComp("STAGE MENU", stageList[STAGELIST_REGULAR][stagePos - 1].name))
                StrAddW(buf, "*COMPLETE*");
            else {
                StrAddW(buf, strSaveStageList[(saveRAM[i * 8 + 4] - 1)]);
                for (int i = 0; i < StrLengthW(buf); i++) {
                    ushort actbuf[4];
                    actbuf[3] = 0;
                    for (int j = 0; j < 3; j++) actbuf[j] = buf[i + j];
                    if (StrCompW(actbuf, " - ")) {
                        buf[i] = 0;
                        StrCopyW(act, &buf[i + 3]);
                        break;
                    }
                }
            }
            byte player = saveRAM[i * 8 + 0];
            SpriteFrame toUse;
            if (Engine.gameType == GAME_SONIC1) {
                toUse.sheetID = LoadTexture("Data/Sprites/Special/Objects.gif", 0);
                toUse.sprX    = 399;
                toUse.sprY    = 376;
            }
            else {
                toUse.sheetID = LoadTexture("Data/Sprites/Continue/Objects.gif", 0);
                toUse.sprX    = 1;
                toUse.sprY    = 18;
            }
            if (player != 3)
                RenderSprite(xp + 8, yp + 13, 16, 23, toUse.sprX + player * 18, toUse.sprY, toUse.sheetID);
            else {
                RenderSpriteFlipped(xp + 2, yp + 13, 16, 23, toUse.sprX + 18, toUse.sprY, FLIP_X, toUse.sheetID);
                RenderSprite(xp + 8, yp + 13, 16, 23, toUse.sprX, toUse.sprY, toUse.sheetID);
            }

            ushort buf[3];
            MEM_ZERO(buf);
            ushort asterisk[] = { '*', 0 }; // really?
            AppendIntegerToStringW(buf, saveRAM[i * 8 + 1] / 10);
            AppendIntegerToStringW(buf, saveRAM[i * 8 + 1] % 10);
            RenderText(asterisk, xp + 9 + 14, yp + 13 + 21, 0, false);
            RenderText(buf, xp + 9 + 16 + 5, yp + 13 + 21, 0, false);
        }
        else
            StrAddW(buf, strNewGame);
        RenderText(buf, xp + 28, yp + 12, 0, false);
        if (StrLengthW(act))
            RenderText(act, xp + 28, yp + 21, 0, true);
    }
}

void StartMenu_LoadStage(NativeEntity_StartMenu *menu)
{
    RenderRectangle(0, 0, SCREEN_XSIZE, SCREEN_YSIZE, 0, 0, 0, 255);
    if (menu->transTimer)
        return;
    int savePos = menu->variables[9] << 3;
    SetGlobalVariableByName("options.saveSlot", menu->variables[9]);
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
    StartMenu_Destroy(menu);
}