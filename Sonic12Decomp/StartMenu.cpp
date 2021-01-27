#include "RetroEngine.hpp"

int titleID, mainID, modID, saveID, bgID;
bool isWide;

void (*target)(NativeEntity_StartMenu *) = nullptr;

// STATES
void StartMenu_Title(NativeEntity_StartMenu *menu);
void StartMenu_MainMenu(NativeEntity_StartMenu *menu);
void StartMenu_SaveSelect(NativeEntity_StartMenu *menu);

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

    // STATE
    CheckKeyDown(&keyDown);
    CheckKeyPress(&keyPress);
    menu->state(menu);

    // BOTTOM AND TOP
    if (menu->state != StartMenu_Title && menu->state != StartMenu_MainMenu) {
        for (int i = 0; i < SCREEN_XSIZE / 24 + 2; i++) {
            RenderSpriteFlipped(i * 24, 0, 24, 13, 293, 128, FLIP_Y, saveID);
            RenderSprite(i * 24, SCREEN_YSIZE - 40, 24, 45, 293, 128, saveID);
        }
    }

    // TRANSITION
    if (menu->transitioning) {
        if (menu->transTimer <= 0)
            menu->transTimer = 32;
        if (--menu->transTimer <= 17) {
            menu->transitioning = false;
            if (target)
                menu->state = target;
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

void StartMenu_Title(NativeEntity_StartMenu *menu) { menu->state = StartMenu_SaveSelect; }

void StartMenu_MainMenu(NativeEntity_StartMenu *menu) {}
void StartMenu_SaveSelect(NativeEntity_StartMenu *menu)
{
    RenderRectangle(0, 0, SCREEN_XSIZE, SCREEN_YSIZE, 255, 255, 255, 128);
    if (!menu->transitioning) {
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
                menu->selected %= 7;
            }
        }
        else if (keyPress.down) {
            menu->selected++;
            if (isWide && (menu->selected != 1 && menu->selected != 3)) {
                menu->selected += 2;
            }
            menu->selected %= 7;
        }
    }

    if (isWide) {
        for (int i = 0; i < 4; i++) {
            bool left = i < 2;
            bool down = i % 2;
            RenderSprite(SCREEN_CENTERX + (left ? -192 : 5), 39 + (down ? 51 : 0), 186, 46, 289 - (menu->selected == i ? 188 : 0), 2, saveID);
        }
    }

    // CONTROL
}