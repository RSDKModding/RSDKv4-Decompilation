#include "RetroEngine.hpp"
#include <algorithm>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

ushort *fbcopy    = new ushort[SCREEN_XSIZE * SCREEN_YSIZE];
ushort *displayed = new ushort[SCREEN_XSIZE * SCREEN_YSIZE];
ushort *lookup    = new ushort[0xFFFF];

void PauseMenu_Create(void *objPtr)
{
    NativeEntity_PauseMenu *pauseMenu = (NativeEntity_PauseMenu *)objPtr;
    pauseMenu->state                  = 0;
    pauseMenu->timer                  = 0;
    pauseMenu->triTimer               = 0;
    pauseMenu->barTimer               = 0;
    pauseMenu->direction              = false;
    pauseMenu->selectedOption         = 0;
    pauseMenu->barPos                 = SCREEN_XSIZE + 67;
    pauseMenu->slowTimer              = 0;

    snapDataFile(1);
    LoadPalette("Menu/Pause/PauseMenu.act", 7, 0, 0, 56);
    snapDataFile(0);

    SetPaletteEntry(7, 8, 0xca, 0x51, 0);
    SetPaletteEntry(7, 9, 0xca, 0x51, 0);

    memcpy(fbcopy, Engine.frameBuffer, (SCREEN_XSIZE * SCREEN_YSIZE) * sizeof(ushort));
}

inline int lerp(float a, float b, float amount)
{
    if (amount > 1)
        amount = 1;
    return a + amount * (b - a);
}

void PauseMenu_Destroy(NativeEntity_PauseMenu *pauseMenu)
{

    RemoveNativeObject(pauseMenu);
    pauseMenu = nullptr;
#if RETRO_DEVICETYPE == RETRO_STANDARD && (RETRO_USING_SDL1 || RETRO_USING_SDL2)
    if (Engine.isFullScreen)
        SDL_ShowCursor(SDL_FALSE);
#endif
}
void PauseMenu_Main(void *objPtr)
{
    CheckKeyDown(&keyDown);
    CheckKeyPress(&keyPress);

    NativeEntity_PauseMenu *pauseMenu = (NativeEntity_PauseMenu *)objPtr;

    int amount = (pauseMenu->direction ? 20 - (pauseMenu->barTimer * 1.5f) : pauseMenu->barTimer) * 17.5f;
    if (amount < 0)
        amount = 0;
    if (amount > 0xFF)
        amount = 0xFF;

    if (pauseMenu->direction) // we copy to ensure it gets sepia'd
        memcpy(fbcopy, Engine.frameBuffer, (SCREEN_XSIZE * SCREEN_YSIZE) * sizeof(ushort));

    switch (pauseMenu->state) {
        case 0:
            // wait
            pauseMenu->barPos -= cos256(pauseMenu->timer * 3) / 21;
            if (++pauseMenu->timer > 21) {
                pauseMenu->state++;
                pauseMenu->timer = 0;
            }
            break;
        case 1:
            if (keyPress.up || keyPress.down) {
                pauseMenu->selectedOption = (pauseMenu->selectedOption + (keyPress.up ? -1 : 1)) % (3 + (int)Engine.devMenu);
                if (pauseMenu->selectedOption < 0)
                    pauseMenu->selectedOption = (2 + (int)Engine.devMenu);
                PlaySFXByName("MenuMove", 0);
            }

            if (keyPress.A || keyPress.start) {
                switch (pauseMenu->selectedOption) {
                    case 0: {
                        Engine.gameMode     = ENGINE_EXITPAUSE;
                        pauseMenu->state    = 2;
                        pauseMenu->barTimer = 1;
                        break;
                    }
                    case 1:
                    case 2:
                    case 3: pauseMenu->state = pauseMenu->selectedOption + 2; break;
                }
                PlaySFXByName("MenuSelect", 0);
            }
            else if (keyPress.B) {
                Engine.gameMode = ENGINE_EXITPAUSE;
                PlaySFXByName("MenuBack", 0);
                pauseMenu->barTimer = 1;
                pauseMenu->state    = 6;
            }
            break;
        case 2: pauseMenu->revokeTimer++;
        case 6:
            if (pauseMenu->state != 2 || pauseMenu->revokeTimer >= 12) {
                pauseMenu->barPos += sin256(pauseMenu->timer * 3) / 21;
                if (++pauseMenu->timer > 21)
                    return PauseMenu_Destroy(pauseMenu);
            }
            break;
        case 3:
        case 4:
        case 5:
            // wait (again)
            if (pauseMenu->revokeTimer >= 12)
                pauseMenu->barPos -= pow(pauseMenu->timer++, 2) / 10;
            pauseMenu->revokeTimer++;

            if (pauseMenu->barPos + 128 < 0 && pauseMenu->slowTimer++ > 7) {

                switch (pauseMenu->state) {
                    case 3:
                        stageMode       = STAGEMODE_LOAD;
                        Engine.gameMode = ENGINE_MAINGAME;
                        break;
                    case 4: initStartMenu(0); break;
                    case 5:
                        Engine.gameMode = ENGINE_DEVMENU;
                        initDevMenu();
                        break;
                }
                return PauseMenu_Destroy(pauseMenu);
            }
            break;
    }

    memset(lookup, 0, 0xFFFF * sizeof(ushort));
    float famount = (float)amount / 0xFF;

    for (int i = 0; i < SCREEN_XSIZE * SCREEN_YSIZE; i++) {
        ushort color = fbcopy[i];
        if (!color) {
            displayed[i] = 0;
            continue;
        }
        if (lookup[color]) {
            displayed[i] = lookup[color];
            continue;
        }
        int r = ((color >> 11) * 527 + 23) >> 6;
        int g = (((color >> 5) & 0b111111) * 259 + 33) >> 6;
        int b = ((color & 0b11111) * 527 + 23) >> 6;

        int tr, tg, tb;
        tr = 0.393 * r + 0.769 * g + 0.189 * b + 10;
        tg = 0.349 * r + 0.686 * g + 0.168 * b - 9;
        tb = 0.272 * r + 0.534 * g + 0.131 * b - 30;
        if (tg < 0)
            tg = 0;
        if (tb < 0)
            tb = 0;

        // lerp
        int fr, fg, fb;
        if (amount < 0xFF) {
            fr = lerp(r, tr, famount);
            fg = lerp(g, tg, famount);
            fb = lerp(b, tb, famount);
        }
        else {
            fr = tr;
            fg = tg;
            fb = tb;
        }
        fr = MIN(255, fr);
        fg = MIN(255, fg);
        fb = MIN(255, fb);

        lookup[color] = RGB888_TO_RGB565(fr, fg, fb);
        displayed[i]  = lookup[color];
    }
    memcpy(Engine.frameBuffer, displayed, (SCREEN_XSIZE * SCREEN_YSIZE) * sizeof(ushort));
    pauseMenu->direction = pauseMenu->state == 2 || pauseMenu->state == 6;

    SetActivePalette(7, 0, SCREEN_YSIZE);

    // EXTRA BARS + PAUSED TEXT
    if (!pauseMenu->direction) {
        DrawRectangle(14, 0, 22, pauseMenu->barTimer * SCREEN_YSIZE / 14, 0, 0, 0, 255);
        DrawRectangle(0, SCREEN_YSIZE - 38, pauseMenu->barTimer * SCREEN_XSIZE / 14, 13, 0, 0, 0, 255);
        DrawRectangle(0, SCREEN_YSIZE - 19, pauseMenu->barTimer * SCREEN_XSIZE / 14, 6, 0, 0, 0, 255);

        DrawSprite(pauseMenu->barTimer * SCREEN_XSIZE / 14 - SCREEN_XSIZE, SCREEN_YSIZE - 38 - 14, 89, 22, 0, 0, SURFACE_MAX - 1);
    }
    else {
        DrawRectangle(14, pauseMenu->barTimer * SCREEN_YSIZE / 14, 22, SCREEN_YSIZE, 0, 0, 0, 255);
        DrawRectangle(pauseMenu->barTimer * SCREEN_XSIZE / 14, SCREEN_YSIZE - 38, SCREEN_XSIZE, 13, 0, 0, 0, 255);
        DrawRectangle(pauseMenu->barTimer * SCREEN_XSIZE / 14, SCREEN_YSIZE - 19, SCREEN_XSIZE, 6, 0, 0, 0, 255);

        DrawSprite((15 - pauseMenu->barTimer) * SCREEN_XSIZE / 14 - SCREEN_XSIZE, SCREEN_YSIZE - 38 - 14, 89, 22, 0, 0, SURFACE_MAX - 1);
    }

    // RIGHT BAR
    DrawRectangle(pauseMenu->barPos, 0, SCREEN_XSIZE - pauseMenu->barPos, SCREEN_YSIZE, 0, 0, 0, 0xFF);
    for (int i = 0; i < SCREEN_YSIZE / 20 + 2; i++) {
        DrawSprite(pauseMenu->barPos - 11, i * 20 + (pauseMenu->triTimer / 2) - 20, 11, 20, 97, 37, SURFACE_MAX - 1);
    }

    // DOODLES
    if (pauseMenu->pressTimer--) {
        // DrawSprite(pauseMenu->barPos)
    }
    // OPTIONS
    int margins[4][3] = { { 0, 46, 65 }, { 0, 58, 55 }, { 66, 46, 30 }, { 56, 58, 64 } };

    for (int i = 0; i < 4; i++) {
        if (i == pauseMenu->selectedOption) {
            DrawSprite(pauseMenu->barPos + 10, i * 32 + 52, 78, 4, 0, 94, SURFACE_MAX - 1);
            if (pauseMenu->revokeTimer % 6 > 2)
                continue;
        }

        DrawSprite(pauseMenu->barPos + 12, i * 32 + 38, margins[i][2], 11, margins[i][0], margins[i][1] + (pauseMenu->selectedOption == i) * 24,
                   SURFACE_MAX - 1);
    }

    if (pauseMenu->barTimer < 15)
        pauseMenu->barTimer++;

    pauseMenu->triTimer = (pauseMenu->triTimer + 1) % 40;

    SetActivePalette(0, 0, SCREEN_YSIZE);
}