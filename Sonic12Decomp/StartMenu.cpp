#include "RetroEngine.hpp"

int titleID, mainID, modID, saveID, bgID;
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

    startMenu->state           = StartMenu_SaveSelect;
    startMenu->transitionTimer = 16;
}
void StartMenu_Main(void *objPtr)
{
    NativeEntity_StartMenu *startMenu = (NativeEntity_StartMenu *)objPtr;

    // BG
    DrawRectangle(0, 0, SCREEN_XSIZE, SCREEN_YSIZE, 255, 255, 255, 255);
    // RENDER BG SPRITES HERE

    startMenu->state(startMenu);
}

void StartMenu_Title(NativeEntity_StartMenu *menu) { menu->state = StartMenu_SaveSelect; }

void StartMenu_MainMenu(NativeEntity_StartMenu *menu) {}
void StartMenu_SaveSelect(NativeEntity_StartMenu *menu)
{
    DrawRectangle(0, 0, SCREEN_XSIZE, SCREEN_YSIZE, 255, 255, 255, 128);
    if (menu->transitioning) {
        // deal with this later
        return;
    }
    if (menu->transitionTimer--) {
        DrawRectangle(0, 0, SCREEN_XSIZE / 15 * menu->transitionTimer, SCREEN_YSIZE, 0, 0, 0, 255);
        for (int i = 0; i < SCREEN_YSIZE / 22 + 1; i++) {
            RenderSprite(SCREEN_XSIZE / 15 * menu->transitionTimer, i * 22 - 11, 12, 22, 33, 48, mainID);
        }
    }
    else
        menu->transitionTimer = 20;
}