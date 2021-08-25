#include "RetroEngine.hpp"

TextMenu pauseTextMenu;

int CheckTouchRect(int x1, int y1, int x2, int y2)
{
    for (int f = 0; f < touches; ++f) {                                                                        
        if (touchDown[f] && touchX[f] > x1 && touchX[f] < x2 && touchY[f] > y1 && touchY[f] < y2)
        {                                      
            return f;
        }
    }
    return -1;
}

void PauseMenu_Create(void *objPtr)
{
    NativeEntity_PauseMenu *entity = (NativeEntity_PauseMenu *)objPtr;
    entity->state                  = 0;
    entity->timer                  = 0;
    entity->selectedOption         = 0;
    entity->barPos                 = SCREEN_XSIZE + 64;
    entity->menu                   = &pauseTextMenu;
    MEM_ZEROP(entity->menu);

    AddTextMenuEntry(entity->menu, "RESUME");
    AddTextMenuEntry(entity->menu, "");
    AddTextMenuEntry(entity->menu, "");
    AddTextMenuEntry(entity->menu, "");
    AddTextMenuEntry(entity->menu, "RESTART");
    AddTextMenuEntry(entity->menu, "");
    AddTextMenuEntry(entity->menu, "");
    AddTextMenuEntry(entity->menu, "");
    AddTextMenuEntry(entity->menu, "EXIT");
    if (Engine.devMenu) {
        AddTextMenuEntry(entity->menu, "");
        AddTextMenuEntry(entity->menu, "");
        AddTextMenuEntry(entity->menu, "");
        AddTextMenuEntry(entity->menu, "DEV MENU");
    }
    entity->menu->alignment      = MENU_ALIGN_CENTER;
    entity->menu->selectionCount = Engine.devMenu ? 3 : 2;
    entity->menu->selection1     = 0;
    entity->menu->selection2     = 0;
    entity->lastSurfaceNo        = textMenuSurfaceNo;
    textMenuSurfaceNo               = SURFACE_MAX - 1;

    SetPaletteEntryPacked(7, 0x08, GetPaletteEntryPacked(0, 8));
    SetPaletteEntryPacked(7, 0xFF, 0xFFFFFF);
}
void PauseMenu_Main(void *objPtr)
{
    CheckKeyDown(&keyDown);
    CheckKeyPress(&keyPress);
    NativeEntity_PauseMenu *entity = (NativeEntity_PauseMenu *)objPtr;

    int lives = GetGlobalVariableByName("player.lives");

    switch (entity->state) {
        case 0:
            // wait
            entity->barPos -= 16;
            if (entity->barPos + 64 < SCREEN_XSIZE) {
                entity->state++;
            }
            break;
        case 1:
            if (!entity->touchControls) {
                if (keyPress.up) {
                    if (entity->selectedOption - 1 < 0) {
                        if (!Engine.devMenu)
                            entity->selectedOption = 3;
                        else
                            entity->selectedOption = 4;
                    }
                    --entity->selectedOption;

                    if (entity->selectedOption == 1 && lives <= 1)
                        entity->selectedOption--;

                    PlaySFXByName("MenuMove", 0);
                }
                else if (keyPress.down) {
                    if (!Engine.devMenu)
                        entity->selectedOption = ++entity->selectedOption % 3;
                    else
                        entity->selectedOption = ++entity->selectedOption % 4;

                    if (entity->selectedOption == 1 && lives <= 1)
                        entity->selectedOption++;

                    PlaySFXByName("MenuMove", 0);
                }

                entity->menu->selection1 = entity->selectedOption * 4;

                if (keyPress.A || keyPress.start) {
                    switch (entity->selectedOption) {
                        case 0: {
                            Engine.gameMode  = ENGINE_EXITPAUSE;
                            entity->state = 2;
                            break;
                        }
                        case 1: {
                            entity->state = 3;
                            break;
                        }
                        case 2: {
                            entity->state = 4;
                            break;
                        }
                        case 3: {
                            entity->state = 5;
                            break;
                        }
                    }
                    PlaySFXByName("MenuSelect", 0);
                }
                else if (keyPress.B) {
                    Engine.gameMode = ENGINE_EXITPAUSE;
                    PlaySFXByName("MenuBack", 0);
                    entity->state = 6;
                }

                if (CheckTouchRect(0, 0, SCREEN_XSIZE, SCREEN_YSIZE) >= 0)
                    entity->touchControls = true;
            }
            else {
                int posY = SCREEN_CENTERY - 0x30;

                int touch = CheckTouchRect(0, 0, SCREEN_XSIZE, SCREEN_YSIZE);

                
                if (CheckTouchRect(0, 0, entity->barPos, SCREEN_YSIZE) >= 0)
                {
                    Engine.gameMode = ENGINE_EXITPAUSE;
                    PlaySFXByName("MenuBack", 0);
                    entity->state = 6;
                }
                else {
                    if (CheckTouchRect(entity->barPos, posY - 12, SCREEN_XSIZE, posY + 12) > -1) {
                        entity->selectedOption = 0;
                    }
                    else if (entity->selectedOption == 0) {
                        if (touch < 0) {
                            PlaySFXByName("MenuSelect", 0);
                            Engine.gameMode  = ENGINE_EXITPAUSE;
                            entity->state = 2;
                        }
                        else {
                            entity->selectedOption = -1;
                        }
                    }

                    posY += 0x20;
                    if (lives > 1) {
                        if (CheckTouchRect(entity->barPos, posY - 12, SCREEN_XSIZE, posY + 12) > -1) {
                            entity->selectedOption = 1;
                        }
                        else if (entity->selectedOption == 1) {
                            if (touch < 0) {
                                PlaySFXByName("MenuSelect", 0);
                                entity->state = 3;
                            }
                            else {
                                entity->selectedOption = -1;
                            }
                        }
                    }

                    posY += 0x20;
                    if (CheckTouchRect(entity->barPos, posY - 12, SCREEN_XSIZE, posY + 12) > -1) {
                        entity->selectedOption = 2;
                    }
                    else if (entity->selectedOption == 2) {
                        if (touch < 0) {
                            PlaySFXByName("MenuSelect", 0);
                            entity->state = 4;
                        }
                        else {
                            entity->selectedOption = -1;
                        }
                    }

                    posY += 0x20;
                    if (Engine.devMenu) {
                        if (CheckTouchRect(entity->barPos, posY - 12, SCREEN_XSIZE, posY + 12) > -1) {
                            entity->selectedOption = 3;
                        }
                        else if (entity->selectedOption == 3) {
                            if (touch < 0) {
                                PlaySFXByName("MenuSelect", 0);
                                entity->state = 5;
                            }
                            else {
                                entity->selectedOption = -1;
                            }
                        }
                    }

                    entity->menu->selection1 = entity->selectedOption * 4;
                }

                if (keyPress.up || keyPress.down)
                    entity->touchControls = false;
            }
            break;
        case 2:
        case 6:
            entity->barPos += 16;
            if (entity->barPos > SCREEN_XSIZE + 64) {
                textMenuSurfaceNo = entity->lastSurfaceNo;
                RemoveNativeObject(entity);
                return;
            }
            break;
        case 3:
        case 4:
        case 5:
            // wait (again)
            entity->barPos -= 16;
            if (entity->barPos + 64 < 0) {
                textMenuSurfaceNo = entity->lastSurfaceNo;
                switch (entity->state) {
                    default: break;
                    case 3:
                        stageMode       = STAGEMODE_LOAD;
                        Engine.gameMode = ENGINE_MAINGAME;
                        if (GetGlobalVariableByName("options.gameMode") <= 1) {
                            SetGlobalVariableByName("player.lives", GetGlobalVariableByName("player.lives") - 1);
                        }
                        SetGlobalVariableByName("lampPostID", 0);
                        SetGlobalVariableByName("starPostID", 0);
                        break;
                    case 4: initStartMenu(0); break;
                    case 5:
                        Engine.gameMode = ENGINE_DEVMENU;
                        initDevMenu();
                        break;
                }
                RemoveNativeObject(entity);
                return;
            }
            break;
    }

    if (entity->menu) {
        entity->menu->selection2 = entity->menu->selection1;

        SetActivePalette(7, 0, SCREEN_YSIZE);

        DrawRectangle(entity->barPos, 0, SCREEN_XSIZE - entity->barPos, SCREEN_YSIZE, 0, 0, 0, 0xFF);
        DrawTextMenu(entity->menu, entity->barPos + 0x28, SCREEN_CENTERY - 0x30);

        SetActivePalette(0, 0, SCREEN_YSIZE);
    }
}