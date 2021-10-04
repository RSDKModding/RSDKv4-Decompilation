#include "RetroEngine.hpp"

void PlayerSelectScreen_Create(void *objPtr)
{
    RSDK_THIS(PlayerSelectScreen);
    entity->labelPtr                  = CREATE_ENTITY(TextLabel);
    entity->labelPtr->useRenderMatrix = true;
    entity->labelPtr->fontID          = FONT_HEADING;
    entity->labelPtr->scale           = 0.2;
    entity->labelPtr->alpha           = 256;
    entity->labelPtr->x               = -144.0;
    entity->labelPtr->y               = 100.0;
    entity->labelPtr->z               = 16.0;
    entity->labelPtr->state           = TEXTLABEL_STATE_IDLE;
    SetStringToFont(entity->labelPtr->text, strPlayerSelect, FONT_HEADING);

    SetStringToFont(entity->textSonic, strSonic, FONT_TEXT);

    entity->sonicX = (GetTextWidth(entity->textSonic, FONT_TEXT, 0.2) * -0.5) - 88.0;

    SetStringToFont(entity->textTails, strTails, FONT_TEXT);
    entity->tailsX = GetTextWidth(entity->textTails, FONT_TEXT, 0.2) * -0.5;

    SetStringToFont(entity->textKnux, strKnuckles, FONT_TEXT);
    entity->knuxX = (GetTextWidth(entity->textKnux, FONT_TEXT, 0.2) * -0.5) + 88.0;

    entity->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", 255);
    SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 0xC0);

    entity->textureArrows    = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);
    entity->texturePlayerSel = LoadTexture("Data/Game/Menu/PlayerSelect.png", TEXFMT_RGBA8888);
    entity->backPressed      = false;
    entity->flag             = true;
}
void PlayerSelectScreen_Main(void *objPtr)
{
    RSDK_THIS(PlayerSelectScreen);

    NativeEntity_SaveSelect *saveSel = entity->saveSel;
    NativeEntity_TextLabel *label    = entity->labelPtr;
    SaveGame *saveGame               = (SaveGame *)saveRAM;

    switch (entity->state) {
        case PLAYERSELECTSCREEN_STATE_ENTER:
            if (entity->alpha < 0x100)
                entity->alpha += 8;

            entity->scale += ((1.025 - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0));
            if (entity->scale < -8.0)
                entity->scale = -8.0;

            NewRenderState();
            matrixScaleXYZF(&entity->matrix1, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->matrix1, &entity->matrixTemp);
            SetRenderMatrix(&entity->matrix1);
            label->renderMatrix = entity->matrix1;

            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                entity->timer  = 0.0;
                entity->state  = PLAYERSELECTSCREEN_STATE_MAIN;
                inputPress.start = false;
                inputPress.A     = false;
                entity->alpha  = 256;
                if (usePhysicalControls)
                    entity->playerID = SAVESEL_SONIC;
            }
            break;
        case PLAYERSELECTSCREEN_STATE_MAIN:
            CheckKeyDown(&inputDown);
            CheckKeyPress(&inputPress);
            SetRenderMatrix(&entity->matrixTemp);
            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (inputPress.left) {
                        if (saveGame->knuxUnlocked) {
                            PlaySfxByName("Menu Move", false);
                            if (--entity->playerID < 0)
                                entity->playerID = SAVESEL_KNUX;
                        }
                        else if (saveGame->tailsUnlocked) {
                            PlaySfxByName("Menu Move", false);
                            if (--entity->playerID > 0)
                                entity->playerID = SAVESEL_TAILS;
                        }
                    }
                    else if (inputPress.right) {
                        if (saveGame->knuxUnlocked) {
                            PlaySfxByName("Menu Move", false);

                            if (++entity->playerID > SAVESEL_KNUX)
                                entity->playerID = SAVESEL_SONIC;
                        }
                        else if (saveGame->tailsUnlocked) {
                            PlaySfxByName("Menu Move", false);

                            if (++entity->playerID > SAVESEL_TAILS)
                                entity->playerID = SAVESEL_SONIC;
                        }
                        else {
                            entity->playerID = SAVESEL_SONIC;
                        }
                    }
                    if (inputPress.start || inputPress.A) {
                        PlaySfxByName("Menu Select", false);
                        StopMusic(true);
                        entity->state = PLAYERSELECTSCREEN_STATE_ACTION;
                    }
                    else if (inputPress.B) {
                        PlaySfxByName("Menu Back", false);
                        entity->backPressed = false;
                        entity->state       = PLAYERSELECTSCREEN_STATE_EXIT;
                    }
                }
            }
            else {
                if (touches > 0) {
                    if (CheckTouchRect(-88.0, 24.0, 40.0, 40.0) < 0) {
                        if (entity->playerID == SAVESEL_SONIC)
                            entity->playerID = SAVESEL_NONE;
                    }
                    else {
                        entity->playerID = SAVESEL_SONIC;
                    }
                    if (saveGame->tailsUnlocked) {
                        if (CheckTouchRect(-84.0, -64.0, 16.0, 16.0) < 0) {
                            if (entity->playerID == SAVESEL_ST)
                                entity->playerID = SAVESEL_SONIC;
                        }
                        else {
                            entity->playerID = SAVESEL_ST;
                        }
                        if (CheckTouchRect(0.0, -16.0, 40.0, 40.0) < 0) {
                            if (entity->playerID == SAVESEL_TAILS)
                                entity->playerID = SAVESEL_NONE;
                        }
                        else {
                            entity->playerID = SAVESEL_TAILS;
                        }
                    }
                    if (saveGame->knuxUnlocked) {
                        if (CheckTouchRect(88.0, 24.0, 40.0, 40.0) < 0) {
                            if (entity->playerID == SAVESEL_KNUX)
                                entity->playerID = SAVESEL_NONE;
                        }
                        else {
                            entity->playerID = SAVESEL_KNUX;
                        }
                    }
                    entity->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (entity->state == PLAYERSELECTSCREEN_STATE_MAIN && (inputDown.left || inputDown.right)) {
                        usePhysicalControls = true;
                        entity->playerID    = SAVESEL_SONIC;
                    }
                }
                else {
                    if (entity->playerID > 0) {
                        PlaySfxByName("Menu Select", false);
                        StopMusic(true);
                        entity->state = PLAYERSELECTSCREEN_STATE_ACTION;
                    }
                    if (entity->backPressed || inputPress.B) {
                        PlaySfxByName("Menu Back", false);
                        entity->backPressed = false;
                        entity->state       = PLAYERSELECTSCREEN_STATE_EXIT;
                    }
                    else if (entity->state == PLAYERSELECTSCREEN_STATE_MAIN && (inputDown.left || inputDown.right)) {
                        usePhysicalControls = true;
                        entity->playerID    = SAVESEL_SONIC;
                    }
                }
            }
            break;
        case 2:
            SetRenderMatrix(&entity->matrixTemp);

            entity->timer2 += Engine.deltaTime;
            entity->timer += Engine.deltaTime;
            if (entity->timer2 > 0.1) {
                entity->timer2 -= 0.1;
            }
            entity->flag = entity->timer2 > 0.05;

            if (entity->timer > 1.0) {
                entity->flag  = true;
                entity->timer = 0.0;
                entity->state = PLAYERSELECTSCREEN_STATE_IDLE;
                if (saveSel->selectedButton <= 0) {
                    SetGlobalVariableByName("options.saveSlot", 0);
                    SetGlobalVariableByName("options.gameMode", 0);
                }
                else {
                    SetGlobalVariableByName("options.saveSlot", saveSel->selectedButton - 1);
                    SetGlobalVariableByName("options.gameMode", 1);

                    switch (entity->playerID) {
                        case SAVESEL_SONIC: saveGame->files[saveSel->selectedButton - 1].characterID = 0; break;
                        case SAVESEL_TAILS: saveGame->files[saveSel->selectedButton - 1].characterID = 1; break;
                        case SAVESEL_KNUX: saveGame->files[saveSel->selectedButton - 1].characterID = 2; break;
                        case SAVESEL_ST: saveGame->files[saveSel->selectedButton - 1].characterID = 3; break;
                    }

                    saveGame->files[saveSel->selectedButton - 1].lives          = 3;
                    saveGame->files[saveSel->selectedButton - 1].score          = 0;
                    saveGame->files[saveSel->selectedButton - 1].scoreBonus     = 500000;
                    saveGame->files[saveSel->selectedButton - 1].stageID        = 1;
                    saveGame->files[saveSel->selectedButton - 1].emeralds       = 0;
                    saveGame->files[saveSel->selectedButton - 1].specialStageID = 0;
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
                    case SAVESEL_SONIC: saveGame->files[saveSel->selectedButton - 1].characterID = 0; break;
                    case SAVESEL_TAILS: saveGame->files[saveSel->selectedButton - 1].characterID = 1; break;
                    case SAVESEL_KNUX: saveGame->files[saveSel->selectedButton - 1].characterID = 2; break;
                    case SAVESEL_ST: saveGame->files[saveSel->selectedButton - 1].characterID = 3; break;
                }
                InitStartingStage(STAGELIST_PRESENTATION, 0, saveGame->files[saveSel->selectedButton - 1].characterID);

                CREATE_ENTITY(FadeScreen);
            }
            break;
        case PLAYERSELECTSCREEN_STATE_IDLE: SetRenderMatrix(&entity->matrixTemp); break;
        case PLAYERSELECTSCREEN_STATE_EXIT:
            if (entity->alpha > 0)
                entity->alpha -= 8;

            if (entity->timer >= 0.2)
                entity->scale += ((-1.0f - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0));
            else
                entity->scale += ((1.5 - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0));
            if (entity->scale < 0.0)
                entity->scale = 0.0;

            NewRenderState();
            matrixScaleXYZF(&entity->matrix1, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->matrix1, &entity->matrixTemp);
            SetRenderMatrix(&entity->matrix1);
            label->renderMatrix = entity->matrix1;

            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                saveSel->state = SAVESELECT_STATE_EXITSUBMENU;
                RemoveNativeObject(label);
                RemoveNativeObject(entity);
                return;
            }
            break;
        default: break;
    }

    RenderMesh(entity->meshPanel, MESH_COLOURS, false);
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
        SetRenderVertexColor(0xFF, 0xFF, 0x40);
    else
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    RenderText(entity->textSonic, FONT_TEXT, entity->sonicX, -22.0, 8.0, 0.2, 255);

    if (saveGame->tailsUnlocked) {
        if ((entity->playerID == SAVESEL_TAILS || entity->playerID == SAVESEL_ST) && entity->flag)
            SetRenderVertexColor(0xFF, 0xFF, 0x40);
        else
            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    }
    else {
        SetRenderVertexColor(0xA0, 0xA0, 0xA0);
    }
    RenderText(entity->textTails, FONT_TEXT, entity->tailsX, -64.0, 8.0, 0.2, 0xFF);
    if (saveGame->knuxUnlocked) {
        if (entity->playerID == SAVESEL_KNUX && entity->flag)
            SetRenderVertexColor(0xFF, 0xFF, 0x40);
        else
            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    }
    else {
        SetRenderVertexColor(0xA0, 0xA0, 0xA0);
    }
    RenderText(entity->textKnux, FONT_TEXT, entity->knuxX, -22.0, 8.0, 0.2, 0xFF);

    NewRenderState();
    SetRenderMatrix(NULL);
    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    if (entity->backPressed)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, entity->alpha, entity->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, entity->alpha, entity->textureArrows);
}
