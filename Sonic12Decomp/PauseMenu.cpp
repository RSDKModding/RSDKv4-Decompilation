#include "RetroEngine.hpp"

TextMenu pauseTextMenu;

void PauseMenu_Create(void *objPtr)
{
    NativeEntity_PauseMenu *pauseMenu = (NativeEntity_PauseMenu *)objPtr;
    pauseMenu->state                  = 0;
    pauseMenu->timer                  = 0;
    pauseMenu->selectedOption         = 0;
    pauseMenu->barPos                 = SCREEN_XSIZE + 64;
    pauseMenu->menu                   = &pauseTextMenu;
    MEM_ZEROP(pauseMenu->menu);

    AddTextMenuEntry(pauseMenu->menu, "RESUME");
    AddTextMenuEntry(pauseMenu->menu, "");
    AddTextMenuEntry(pauseMenu->menu, "");
    AddTextMenuEntry(pauseMenu->menu, "");
    AddTextMenuEntry(pauseMenu->menu, "RESTART");
    AddTextMenuEntry(pauseMenu->menu, "");
    AddTextMenuEntry(pauseMenu->menu, "");
    AddTextMenuEntry(pauseMenu->menu, "");
    AddTextMenuEntry(pauseMenu->menu, "EXIT");
    if (Engine.devMenu) {
        AddTextMenuEntry(pauseMenu->menu, "");
        AddTextMenuEntry(pauseMenu->menu, "");
        AddTextMenuEntry(pauseMenu->menu, "");
        AddTextMenuEntry(pauseMenu->menu, "DEV MENU");
    }
    pauseMenu->menu->alignment      = MENU_ALIGN_CENTER;
    pauseMenu->menu->selectionCount = Engine.devMenu ? 3 : 2;
    pauseMenu->menu->selection1     = 0;
    pauseMenu->menu->selection2     = 0;
    pauseMenu->lastSurfaceNo        = textMenuSurfaceNo;
    textMenuSurfaceNo               = 0;

    pauseMenu->paletteStore[0] = GetPaletteEntryPacked(0, 0xF0);
    pauseMenu->paletteStore[1] = GetPaletteEntryPacked(0, 0xFF);
    SetPaletteEntry(0, 0xF0, 0x00, 0x00, 0x00);
    SetPaletteEntry(0, 0xFF, 0xFF, 0xFF, 0xFF);
}
void PauseMenu_Main(void *objPtr)
{
    CheckKeyDown(&keyDown, 0xFF);
    CheckKeyPress(&keyPress, 0xFF);

    NativeEntity_PauseMenu *pauseMenu = (NativeEntity_PauseMenu *)objPtr;

    switch (pauseMenu->state) {
        case 0:
            // wait
            pauseMenu->barPos -= 16;
            if (pauseMenu->barPos + 64 < SCREEN_XSIZE) {
                pauseMenu->state++;
            }
            break;
        case 1:
            if (keyPress.up) {
                if (pauseMenu->selectedOption - 1 < 0) {
                    if (!Engine.devMenu)
                        pauseMenu->selectedOption = 3;
                    else
                        pauseMenu->selectedOption = 4;
                }
                --pauseMenu->selectedOption;
                PlaySFXByName("MenuMove", 0);
            }
            else if (keyPress.down) {
                if (!Engine.devMenu)
                    pauseMenu->selectedOption = ++pauseMenu->selectedOption % 3;
                else
                    pauseMenu->selectedOption = ++pauseMenu->selectedOption % 4;
                PlaySFXByName("MenuMove", 0);
            }

            pauseMenu->menu->selection1 = pauseMenu->selectedOption * 4;

            if (keyPress.A || keyPress.start) {
                switch (pauseMenu->selectedOption) {
                    case 0: {
                        Engine.gameMode  = ENGINE_EXITPAUSE;
                        pauseMenu->state = 2;
                        break;
                    }
                    case 1: {
                        pauseMenu->state = 3;
                        break;
                    }
                    case 2: {
                        pauseMenu->state = 4;
                        break;
                    }
                    case 3: {
                        pauseMenu->state = 5;
                        break;
                    }
                }
                PlaySFXByName("MenuSelect", 0);
            }
            else if (keyPress.B) {
                Engine.gameMode = ENGINE_EXITPAUSE;
                PlaySFXByName("MenuBack", 0);
                pauseMenu->state = 6;
            }
            break;
        case 2:
        case 6:
            pauseMenu->barPos += 16;
            if (pauseMenu->barPos > SCREEN_XSIZE + 64) {
                SetPaletteEntryPacked(0, 0xF0, pauseMenu->paletteStore[0]);
                SetPaletteEntryPacked(0, 0xFF, pauseMenu->paletteStore[1]);
                RemoveNativeObject(pauseMenu);
                return;
            }
            break;
        case 3:
        case 4:
        case 5:
            // wait (again)
            pauseMenu->barPos -= 16;
            if (pauseMenu->barPos + 64 < 0) {
                switch (pauseMenu->state) {
                    default: break;
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
                RemoveNativeObject(pauseMenu);
                return;
            }
            break;
    }

    if (pauseMenu->menu) {
        SetActivePalette(0, 0, SCREEN_YSIZE);

        DrawRectangle(pauseMenu->barPos, 0, SCREEN_XSIZE - pauseMenu->barPos, SCREEN_YSIZE, 0, 0, 0, 0xFF);
        DrawTextMenu(pauseMenu->menu, pauseMenu->barPos + 0x28, SCREEN_CENTERY - 0x30);
    }
}