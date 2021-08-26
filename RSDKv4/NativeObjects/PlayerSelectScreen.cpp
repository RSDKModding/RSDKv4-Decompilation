#include "RetroEngine.hpp"

void PlayerSelectScreen_Create(void *objPtr)
{
    RSDK_THIS(PlayerSelectScreen);
    entity->labelPtr            = CREATE_ENTITY(TextLabel);
    entity->labelPtr->useMatrix = true;
    entity->labelPtr->fontID    = 0;
    entity->labelPtr->textScale = 0.2;
    entity->labelPtr->textAlpha = 256;
    entity->labelPtr->textX     = -144.0;
    entity->labelPtr->textY     = 100.0;
    entity->labelPtr->textZ     = 16.0;
    entity->labelPtr->alignment = 0;
    SetStringToFont(entity->labelPtr->text, strPlayerSelect, 0);

    SetStringToFont(entity->textSonic, strSonic, 2);

    entity->sonicX = (GetTextWidth(entity->textSonic, 2, 0.2) * -0.5) - 88.0;

    SetStringToFont(entity->textTails, strTails, 2);
    entity->tailsX = GetTextWidth(entity->textTails, 2, 0.2) * -0.5;

    SetStringToFont(entity->textKnux, strKnuckles, 2);
    entity->knuxX = (GetTextWidth(entity->textKnux, 2, 0.2) * -0.5) + 88.0;

    entity->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", 255);
    SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 0xC0);

    entity->textureArrows    = LoadTexture("Data/Game/Menu/ArrowButtons.png", 1);
    entity->texturePlayerSel = LoadTexture("Data/Game/Menu/PlayerSelect.png", 3);
    entity->touchValid       = 0;
    entity->flag             = true;
}
void PlayerSelectScreen_Main(void *objPtr)
{
    RSDK_THIS(PlayerSelectScreen);

    NativeEntity_SaveSelect *saveSel = entity->saveSel;
    NativeEntity_TextLabel *label    = entity->labelPtr;
    SaveGame *saveGame               = (SaveGame *)saveRAM;

    switch (entity->state) {
        case 0:
            if (entity->alpha <= 255)
                entity->alpha += 8;

            entity->scale += ((1.025 - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0));
            if (entity->scale < -8.0)
                entity->scale = -8.0;

            NewRenderState();
            matrixScaleXYZF(&entity->matrix1, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrix2, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->matrix1, &entity->matrix2);
            SetRenderMatrix(&entity->matrix1);
            label->renderMatrix = entity->matrix1;

            entity->field_14 += Engine.deltaTime;
            if (entity->field_14 > 0.5) {
                entity->field_14 = 0.0;
                entity->state    = 1;
                keyPress.start   = false;
                keyPress.A       = false;
                entity->alpha    = 256;
                if (usePhysicalControls)
                    entity->playerID = SAVESEL_SONIC;
            }
            break;
        case 1:
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->matrix2);
            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.left) {
                        if (saveGame->knuxUnlocked) {
                            PlaySfx(21, 0);
                            if (entity->playerID - 1 > 0)
                                entity->playerID--;
                            else
                                entity->playerID = SAVESEL_KNUX;
                        }
                        else if (saveGame->tailsUnlocked) {
                            PlaySfx(21, 0);
                            if (entity->playerID - 1 > 0)
                                entity->playerID--;
                            else
                                entity->playerID = SAVESEL_TAILS;
                        }
                    }
                    else if (keyPress.right) {
                        if (saveGame->knuxUnlocked) {
                            PlaySfx(21, 0);
                            entity->playerID++;
                            if (entity->playerID > SAVESEL_KNUX) {
                                entity->playerID = SAVESEL_SONIC;
                            }
                        }
                        else if (saveGame->tailsUnlocked) {
                            PlaySfx(21, 0);
                            entity->playerID++;
                            if (entity->playerID > SAVESEL_TAILS) {
                                entity->playerID = SAVESEL_SONIC;
                            }
                        }
                        else {
                            entity->playerID = SAVESEL_SONIC;
                        }
                    }
                    if (keyPress.start || keyPress.A) {
                        PlaySfx(22, 0);
                        StopMusic(true);
                        entity->state = 2;
                    }
                    else if (keyPress.B) {
                        PlaySfx(23, 0);
                        entity->touchValid = 0;
                        entity->state      = 4;
                    }
                }
            }
            else {
                if (touches > 0) {
                    if (CheckTouchRect(-88.0, 24.0, 40.0, 40.0) < 0) {
                        if (entity->playerID == SAVESEL_SONIC)
                            entity->playerID = 0;
                    }
                    else {
                        entity->playerID = SAVESEL_SONIC;
                    }
                    if (saveGame->tailsUnlocked) {
                        if (CheckTouchRect(-84.0, -64.0, 16.0, 16.0) < 0) {
                            if (entity->playerID == SAVESEL_ST)
                                entity->playerID = 0;
                        }
                        else {
                            entity->playerID = SAVESEL_ST;
                        }
                        if (CheckTouchRect(0.0, -16.0, 40.0, 40.0) < 0) {
                            if (entity->playerID == SAVESEL_TAILS)
                                entity->playerID = 0;
                        }
                        else {
                            entity->playerID = SAVESEL_TAILS;
                        }
                    }
                    if (saveGame->knuxUnlocked) {
                        if (CheckTouchRect(88.0, 24.0, 40.0, 40.0) < 0) {
                            if (entity->playerID == SAVESEL_KNUX)
                                entity->playerID = 0;
                        }
                        else {
                            entity->playerID = SAVESEL_KNUX;
                        }
                    }
                    entity->touchValid = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (entity->state == 1 && (keyDown.left || keyDown.right)) {
                        usePhysicalControls = true;
                        entity->playerID    = SAVESEL_SONIC;
                    }
                }
                else {
                    if (entity->playerID > 0) {
                        PlaySfx(22, 0);
                        StopMusic(true);
                        entity->state = 2;
                    }
                    if (entity->touchValid || keyPress.B) {
                        PlaySfx(23, 0);
                        entity->touchValid = false;
                        entity->state      = 4;
                    }
                    else if (entity->state == 1 && (keyDown.left || keyDown.right)) {
                        usePhysicalControls = true;
                        entity->playerID    = SAVESEL_SONIC;
                    }
                }
            }
            break;
        case 2:
            SetRenderMatrix(&entity->matrix2);

            entity->field_18 += Engine.deltaTime;
            entity->field_14 += Engine.deltaTime;
            if (entity->field_18 > 0.1) {
                entity->field_18 -= 0.1;
            }
            entity->flag = entity->field_18 > 0.05;

            if (entity->field_14 > 1.0) {
                entity->flag     = true;
                entity->field_14 = 0.0;
                entity->state    = 3;
                if (saveSel->selectedSave <= 0) {
                    SetGlobalVariableByName("options.saveSlot", 0);
                    SetGlobalVariableByName("options.gameMode", 0);
                }
                else {
                    SetGlobalVariableByName("options.saveSlot", saveSel->selectedSave - 1);
                    SetGlobalVariableByName("options.gameMode", 1);

                    switch (entity->playerID) {
                        case SAVESEL_SONIC: saveGame->files[saveSel->selectedSave - 1].characterID = 0; break;
                        case SAVESEL_TAILS: saveGame->files[saveSel->selectedSave - 1].characterID = 1; break;
                        case SAVESEL_KNUX: saveGame->files[saveSel->selectedSave - 1].characterID = 2; break;
                        case SAVESEL_ST: saveGame->files[saveSel->selectedSave - 1].characterID = 3; break;
                    }

                    saveGame->files[saveSel->selectedSave - 1].lives         = 3;
                    saveGame->files[saveSel->selectedSave - 1].score         = 0;
                    saveGame->files[saveSel->selectedSave - 1].scoreBonus    = 500000;
                    saveGame->files[saveSel->selectedSave - 1].zoneID        = 1;
                    saveGame->files[saveSel->selectedSave - 1].emeralds      = 0;
                    saveGame->files[saveSel->selectedSave - 1].specialZoneID = 0;
                    WriteSaveRAMData();
                }
                SetGlobalVariableByName("player.lives", 3);
                SetGlobalVariableByName("player.score", 0);
                SetGlobalVariableByName("player.scoreBonus", 50000);
                SetGlobalVariableByName("specialStage.listPos", 0);
                SetGlobalVariableByName("specialStage.emeralds", 0);
                SetGlobalVariableByName("specialStage.nextZone", 0);
                SetGlobalVariableByName("timeAttack.result", 0);
                SetGlobalVariableByName("lampPostID", 0);
                SetGlobalVariableByName("starPostID", 0);

                switch (entity->playerID) {
                    case SAVESEL_SONIC: saveGame->files[saveSel->selectedSave - 1].characterID = 0; break;
                    case SAVESEL_TAILS: saveGame->files[saveSel->selectedSave - 1].characterID = 1; break;
                    case SAVESEL_KNUX: saveGame->files[saveSel->selectedSave - 1].characterID = 2; break;
                    case SAVESEL_ST: saveGame->files[saveSel->selectedSave - 1].characterID = 3; break;
                }
                InitStartingStage(0, 0, saveGame->files[saveSel->selectedSave - 1].characterID);

                CREATE_ENTITY(FadeScreen);
            }
            break;
        case 3: SetRenderMatrix(&entity->matrix2); break;
        case 4:
            if (entity->alpha > 0)
                entity->alpha -= 8;

            if (entity->field_14 >= 0.2)
                entity->scale += ((-1.0f - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0));
            else
                entity->scale += ((1.5 - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0));
            if (entity->scale < 0.0)
                entity->scale = 0.0;

            NewRenderState();
            matrixScaleXYZF(&entity->matrix1, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrix2, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->matrix1, &entity->matrix2);
            SetRenderMatrix(&entity->matrix1);
            label->renderMatrix = entity->matrix1;

            entity->field_14 += Engine.deltaTime;
            if (entity->field_14 > 0.5) {
                saveSel->state = 7;
                RemoveNativeObject(label);
                RemoveNativeObject(entity);
                return;
            }
            break;
        default: break;
    }

    RenderMesh(entity->meshPanel, 0, 0);
    if ((entity->playerID == SAVESEL_SONIC || entity->playerID == SAVESEL_ST) && entity->flag) {
        SetRenderVertexColor(255, 192, 0);
        if (entity->playerID == SAVESEL_SONIC && usePhysicalControls) {
            RenderImage(-88.0, 28.0, 8.0, 0.325, 0.325, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 255, entity->texturePlayerSel);
        }
        else {
            RenderImage(-88.0, 28.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 255, entity->texturePlayerSel);
        }
    }
    else {
        SetRenderVertexColor(255, 255, 0);
        RenderImage(-88.0, 28.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 255, entity->texturePlayerSel);
    }

    SetRenderVertexColor(255, 255, 255);
    RenderImage(-88.0, 32.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 0.0, 0.0, 255, entity->texturePlayerSel);
    if ((entity->playerID == SAVESEL_TAILS || entity->playerID == SAVESEL_ST) && entity->flag) {
        SetRenderVertexColor(0, 96, 0xFF);
    }
    else {
        SetRenderVertexColor(0, 0x80, 0xFF);
        RenderImage(0.0, -12.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 255, entity->texturePlayerSel);
    }
    if (entity->playerID == SAVESEL_TAILS && usePhysicalControls) {
        RenderImage(0.0, -12.0, 8.0, 0.325, 0.325, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 0xFF, entity->texturePlayerSel);
    }
    else {
        RenderImage(0.0, -12.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 0xFF, entity->texturePlayerSel);
    }

    if (saveGame->tailsUnlocked)
        SetRenderVertexColor(255, 255, 255);
    else
        SetRenderVertexColor(0, 0, 0);
    RenderImage(0.0, -8.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 256.0, 0.0, 255, entity->texturePlayerSel);
    if (entity->playerID == SAVESEL_KNUX && entity->flag) {
        SetRenderVertexColor(0, 128, 0);
        if (usePhysicalControls) {
            RenderImage(88.0, 28.0, 8.0, 0.325, 0.325, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 0xFF, entity->texturePlayerSel);
        }
        else {
            RenderImage(88.0, 28.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 0xFF, entity->texturePlayerSel);
        }
    }
    else {
        SetRenderVertexColor(0, 144, 0);
        RenderImage(88.0, 28.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 0xFF, entity->texturePlayerSel);
    }

    if (saveGame->knuxUnlocked)
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    else
        SetRenderVertexColor(0, 0, 0);
    RenderImage(88.0, 32.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 0.0, 256.0, 0xFF, entity->texturePlayerSel);

    if (saveGame->tailsUnlocked) {
        if (entity->playerID == SAVESEL_ST && entity->flag) {
            SetRenderVertexColor(255, 64, 0);
            RenderImage(-88.0, -56.0, 8.0, 0.135, 0.135, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 0xFF, entity->texturePlayerSel);
            RenderRect(-96.0, -54.0, 8.0, 16.0, 4.0, 0xFF, 255, 0x40, 0xFF);
            RenderRect(-90.0, -48.0, 8.0, 4.0, 16.0, 0xFF, 255, 0x40, 0xFF);
            RenderRect(-36.0, -54.0, 8.0, 10.0, 4.0, 0xFF, 255, 0x40, 0xFF);
            RenderRect(-52.0, -54.0, 8.0, 10.0, 4.0, 0xFF, 255, 0x40, 0xFF);
            RenderRect(-68.0, -54.0, 8.0, 10.0, 4.0, 0xFF, 255, 0x40, 0xFF);
            RenderRect(-90.0, -26.0, 8.0, 4.0, 10.0, 0xFF, 255, 0x40, 0xFF);
        }
        else {
            SetRenderVertexColor(255, 0, 0);
            RenderImage(-88.0, -56.0, 8.0, 0.125, 0.125, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 0xFF, entity->texturePlayerSel);
            RenderRect(-96.0, -54.0, 8.0, 16.0, 4.0, 0xFF, 0xFF, 0xFF, 0xFF);
            RenderRect(-90.0, -48.0, 8.0, 4.0, 16.0, 0xFF, 0xFF, 0xFF, 0xFF);
            RenderRect(-36.0, -54.0, 8.0, 10.0, 4.0, 0xFF, 0xFF, 0xFF, 0xFF);
            RenderRect(-52.0, -54.0, 8.0, 10.0, 4.0, 0xFF, 0xFF, 0xFF, 0xFF);
            RenderRect(-68.0, -54.0, 8.0, 10.0, 4.0, 0xFF, 0xFF, 0xFF, 0xFF);
            RenderRect(-90.0, -26.0, 8.0, 4.0, 10.0, 0xFF, 0xFF, 0xFF, 0xFF);
        }
    }
    if ((entity->playerID == SAVESEL_SONIC || entity->playerID == SAVESEL_ST) && entity->flag)
        SetRenderVertexColor(0xFF, 0xFF, 64);
    else
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    RenderText(entity->textSonic, 2, entity->sonicX, -22.0, 8.0, 0.2, 255);

    if (saveGame->tailsUnlocked) {
        if ((entity->playerID == SAVESEL_TAILS || entity->playerID == SAVESEL_ST) && entity->flag)
            SetRenderVertexColor(0xFF, 0xFF, 64);
        else
            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    }
    else {
        SetRenderVertexColor(160, 160, 160);
    }
    RenderText(entity->textTails, 2, entity->tailsX, -64.0, 8.0, 0.2, 0xFF);
    if (saveGame->knuxUnlocked) {
        if (entity->playerID == SAVESEL_KNUX && entity->flag)
            SetRenderVertexColor(0xFF, 0xFF, 0x40);
        else
            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    }
    else {
        SetRenderVertexColor(160, 160, 160);
    }
    RenderText(entity->textKnux, 2, entity->knuxX, -22.0, 8.0, 0.2, 0xFF);

    NewRenderState();
    SetRenderMatrix(NULL);
    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    if (entity->touchValid)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, entity->alpha, entity->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, entity->alpha, entity->textureArrows);
}
