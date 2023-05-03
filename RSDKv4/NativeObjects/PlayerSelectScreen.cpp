#include "RetroEngine.hpp"

void PlayerSelectScreen_Create(void *objPtr)
{
    RSDK_THIS(PlayerSelectScreen);
    self->labelPtr                  = CREATE_ENTITY(TextLabel);
    self->labelPtr->useRenderMatrix = true;
    self->labelPtr->fontID          = FONT_HEADING;
    self->labelPtr->scale           = 0.2;
    self->labelPtr->alpha           = 256;
    self->labelPtr->x               = -144.0;
    self->labelPtr->y               = 100.0;
    self->labelPtr->z               = 16.0;
    self->labelPtr->state           = TEXTLABEL_STATE_IDLE;
    SetStringToFont(self->labelPtr->text, strPlayerSelect, FONT_HEADING);

    SetStringToFont(self->textSonic, strSonic, FONT_TEXT);

    self->sonicX = (GetTextWidth(self->textSonic, FONT_TEXT, 0.2) * -0.5) - 88.0;

    SetStringToFont(self->textTails, strTails, FONT_TEXT);
    self->tailsX = GetTextWidth(self->textTails, FONT_TEXT, 0.2) * -0.5;

    SetStringToFont(self->textKnux, strKnuckles, FONT_TEXT);
    self->knuxX = (GetTextWidth(self->textKnux, FONT_TEXT, 0.2) * -0.5) + 88.0;

    self->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", 255);
    SetMeshVertexColors(self->meshPanel, 0, 0, 0, 0xC0);

    self->textureArrows    = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);
    self->texturePlayerSel = LoadTexture("Data/Game/Menu/PlayerSelect.png", TEXFMT_RGBA8888);
    self->backPressed      = false;
    self->flag             = true;
}
void PlayerSelectScreen_Main(void *objPtr)
{
    RSDK_THIS(PlayerSelectScreen);

    NativeEntity_SaveSelect *saveSel = self->saveSel;
    NativeEntity_TextLabel *label    = self->labelPtr;
    SaveGame *saveGame               = (SaveGame *)saveRAM;

    switch (self->state) {
        case PLAYERSELECTSCREEN_STATE_ENTER:
            if (self->alpha < 0x100)
                self->alpha += 8;

            self->scale += ((1.025 - self->scale) / ((60.0 * Engine.deltaTime) * 8.0));
            if (self->scale < -8.0)
                self->scale = -8.0;

            NewRenderState();
            MatrixScaleXYZF(&self->matrix1, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->matrix1, &self->matrixTemp);
            SetRenderMatrix(&self->matrix1);
            label->renderMatrix = self->matrix1;

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                self->timer      = 0.0;
                self->state      = PLAYERSELECTSCREEN_STATE_MAIN;
                keyPress.start = false;
                keyPress.A     = false;
                self->alpha      = 256;
                if (usePhysicalControls)
                    self->playerID = SAVESEL_SONIC;
            }
            break;

        case PLAYERSELECTSCREEN_STATE_MAIN:
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&self->matrixTemp);
            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.left) {
                        if (saveGame->knuxUnlocked) {
                            PlaySfxByName("Menu Move", false);
                            if (--self->playerID < SAVESEL_SONIC)
                                self->playerID = SAVESEL_KNUX;
                        }
                        else if (saveGame->tailsUnlocked) {
                            PlaySfxByName("Menu Move", false);
                            if (--self->playerID > SAVESEL_SONIC)
                                self->playerID = SAVESEL_TAILS;
                        }
                    }
                    else if (keyPress.right) {
                        if (saveGame->knuxUnlocked) {
                            PlaySfxByName("Menu Move", false);

                            if (++self->playerID > SAVESEL_KNUX)
                                self->playerID = SAVESEL_SONIC;
                        }
                        else if (saveGame->tailsUnlocked) {
                            PlaySfxByName("Menu Move", false);

                            if (++self->playerID > SAVESEL_TAILS)
                                self->playerID = SAVESEL_SONIC;
                        }
                        else {
                            self->playerID = SAVESEL_SONIC;
                        }
                    }
                    if (keyPress.start || keyPress.A) {
                        PlaySfxByName("Menu Select", false);
                        StopMusic(true);
                        self->state = PLAYERSELECTSCREEN_STATE_ACTION;
                    }
                    else if (keyPress.B) {
                        PlaySfxByName("Menu Back", false);
                        self->backPressed = false;
                        self->state       = PLAYERSELECTSCREEN_STATE_EXIT;
                    }
                }
            }
            else {
                if (touches > 0) {
                    if (CheckTouchRect(-88.0, 24.0, 40.0, 40.0) < 0) {
                        if (self->playerID == SAVESEL_SONIC)
                            self->playerID = SAVESEL_NONE;
                    }
                    else {
                        self->playerID = SAVESEL_SONIC;
                    }
                    if (saveGame->tailsUnlocked) {
                        if (CheckTouchRect(-84.0, -64.0, 16.0, 16.0) < 0) {
                            if (self->playerID == SAVESEL_ST)
                                self->playerID = SAVESEL_SONIC;
                        }
                        else {
                            self->playerID = SAVESEL_ST;
                        }
                        if (CheckTouchRect(0.0, -16.0, 40.0, 40.0) < 0) {
                            if (self->playerID == SAVESEL_TAILS)
                                self->playerID = SAVESEL_NONE;
                        }
                        else {
                            self->playerID = SAVESEL_TAILS;
                        }
                    }
                    if (saveGame->knuxUnlocked) {
                        if (CheckTouchRect(88.0, 24.0, 40.0, 40.0) < 0) {
                            if (self->playerID == SAVESEL_KNUX)
                                self->playerID = SAVESEL_NONE;
                        }
                        else {
                            self->playerID = SAVESEL_KNUX;
                        }
                    }
                    self->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (self->state == PLAYERSELECTSCREEN_STATE_MAIN && (keyDown.left || keyDown.right)) {
                        usePhysicalControls = true;
                        self->playerID      = SAVESEL_SONIC;
                    }
                }
                else {
                    if (self->playerID > 0) {
                        PlaySfxByName("Menu Select", false);
                        StopMusic(true);
                        self->state = PLAYERSELECTSCREEN_STATE_ACTION;
                    }
                    if (self->backPressed || keyPress.B) {
                        PlaySfxByName("Menu Back", false);
                        self->backPressed = false;
                        self->state       = PLAYERSELECTSCREEN_STATE_EXIT;
                    }
                    else if (self->state == PLAYERSELECTSCREEN_STATE_MAIN && (keyDown.left || keyDown.right)) {
                        usePhysicalControls = true;
                        self->playerID      = SAVESEL_SONIC;
                    }
                }
            }
            break;

        case PLAYERSELECTSCREEN_STATE_ACTION:
            SetRenderMatrix(&self->matrixTemp);

            self->timer2 += Engine.deltaTime;
            self->timer += Engine.deltaTime;
            if (self->timer2 > 0.1) {
                self->timer2 -= 0.1;
            }
            self->flag = self->timer2 > 0.05;

            if (self->timer > 1.0) {
                self->flag  = true;
                self->timer = 0.0;
                self->state = PLAYERSELECTSCREEN_STATE_IDLE;
                if (saveSel->selectedButton <= 0) {
                    SetGlobalVariableByName("options.saveSlot", 0);
                    SetGlobalVariableByName("options.gameMode", 0);
                }
                else {
                    SetGlobalVariableByName("options.saveSlot", saveSel->selectedButton - 1);
                    SetGlobalVariableByName("options.gameMode", 1);

                    switch (self->playerID) {
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
                SetGlobalVariableByName("options.stageSelectFlag", 0);
                SetGlobalVariableByName("player.lives", 3);
                SetGlobalVariableByName("player.score", 0);
                SetGlobalVariableByName("player.scoreBonus", 50000);
                SetGlobalVariableByName("specialStage.listPos", 0);
                SetGlobalVariableByName("specialStage.emeralds", 0);
                SetGlobalVariableByName("specialStage.nextZone", 0);
                SetGlobalVariableByName("timeAttack.result", 0);
                SetGlobalVariableByName("lampPostID", 0);
                SetGlobalVariableByName("starPostID", 0);
                debugMode = false;

                switch (self->playerID) {
                    case SAVESEL_SONIC: saveGame->files[saveSel->selectedButton - 1].characterID = 0; break;
                    case SAVESEL_TAILS: saveGame->files[saveSel->selectedButton - 1].characterID = 1; break;
                    case SAVESEL_KNUX: saveGame->files[saveSel->selectedButton - 1].characterID = 2; break;
                    case SAVESEL_ST: saveGame->files[saveSel->selectedButton - 1].characterID = 3; break;
                }
                InitStartingStage(STAGELIST_PRESENTATION, 0, saveGame->files[saveSel->selectedButton - 1].characterID);

                CREATE_ENTITY(FadeScreen);
            }
            break;

        case PLAYERSELECTSCREEN_STATE_IDLE: SetRenderMatrix(&self->matrixTemp); break;

        case PLAYERSELECTSCREEN_STATE_EXIT:
            if (self->alpha > 0)
                self->alpha -= 8;

            if (self->timer >= 0.2)
                self->scale += ((-1.0f - self->scale) / ((60.0 * Engine.deltaTime) * 8.0));
            else
                self->scale += ((1.5 - self->scale) / ((60.0 * Engine.deltaTime) * 8.0));
            if (self->scale < 0.0)
                self->scale = 0.0;

            NewRenderState();
            MatrixScaleXYZF(&self->matrix1, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->matrix1, &self->matrixTemp);
            SetRenderMatrix(&self->matrix1);
            label->renderMatrix = self->matrix1;

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                saveSel->state = SAVESELECT_STATE_EXITSUBMENU;
                RemoveNativeObject(label);
                RemoveNativeObject(self);
                return;
            }
            break;

        default: break;
    }

    RenderMesh(self->meshPanel, MESH_COLORS, false);
    if ((self->playerID == SAVESEL_SONIC || self->playerID == SAVESEL_ST) && self->flag) {
        SetRenderVertexColor(255, 192, 0);
        if (self->playerID == SAVESEL_SONIC && usePhysicalControls) {
            RenderImage(-88.0, 28.0, 8.0, 0.325, 0.325, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 255, self->texturePlayerSel);
        }
        else {
            RenderImage(-88.0, 28.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 255, self->texturePlayerSel);
        }
    }
    else {
        SetRenderVertexColor(255, 255, 0);
        RenderImage(-88.0, 28.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 255, self->texturePlayerSel);
    }

    SetRenderVertexColor(255, 255, 255);
    RenderImage(-88.0, 32.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 0.0, 0.0, 255, self->texturePlayerSel);
    if ((self->playerID == SAVESEL_TAILS || self->playerID == SAVESEL_ST) && self->flag) {
        SetRenderVertexColor(0, 96, 0xFF);
    }
    else {
        SetRenderVertexColor(0, 0x80, 0xFF);
        RenderImage(0.0, -12.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 255, self->texturePlayerSel);
    }
    if (self->playerID == SAVESEL_TAILS && usePhysicalControls) {
        RenderImage(0.0, -12.0, 8.0, 0.325, 0.325, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 0xFF, self->texturePlayerSel);
    }
    else {
        RenderImage(0.0, -12.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 0xFF, self->texturePlayerSel);
    }

    if (saveGame->tailsUnlocked)
        SetRenderVertexColor(255, 255, 255);
    else
        SetRenderVertexColor(0, 0, 0);
    RenderImage(0.0, -8.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 256.0, 0.0, 255, self->texturePlayerSel);
    if (self->playerID == SAVESEL_KNUX && self->flag) {
        SetRenderVertexColor(0, 128, 0);
        if (usePhysicalControls) {
            RenderImage(88.0, 28.0, 8.0, 0.325, 0.325, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 0xFF, self->texturePlayerSel);
        }
        else {
            RenderImage(88.0, 28.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 0xFF, self->texturePlayerSel);
        }
    }
    else {
        SetRenderVertexColor(0, 144, 0);
        RenderImage(88.0, 28.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 0xFF, self->texturePlayerSel);
    }

    if (saveGame->knuxUnlocked)
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    else
        SetRenderVertexColor(0, 0, 0);
    RenderImage(88.0, 32.0, 8.0, 0.3, 0.3, 128.0, 128.0, 256.0, 256.0, 0.0, 256.0, 0xFF, self->texturePlayerSel);

    if (saveGame->tailsUnlocked) {
        if (self->playerID == SAVESEL_ST && self->flag) {
            SetRenderVertexColor(255, 64, 0);
            RenderImage(-88.0, -56.0, 8.0, 0.135, 0.135, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 0xFF, self->texturePlayerSel);
            RenderRect(-96.0, -54.0, 8.0, 16.0, 4.0, 0xFF, 255, 0x40, 0xFF);
            RenderRect(-90.0, -48.0, 8.0, 4.0, 16.0, 0xFF, 255, 0x40, 0xFF);
            RenderRect(-36.0, -54.0, 8.0, 10.0, 4.0, 0xFF, 255, 0x40, 0xFF);
            RenderRect(-52.0, -54.0, 8.0, 10.0, 4.0, 0xFF, 255, 0x40, 0xFF);
            RenderRect(-68.0, -54.0, 8.0, 10.0, 4.0, 0xFF, 255, 0x40, 0xFF);
            RenderRect(-90.0, -26.0, 8.0, 4.0, 10.0, 0xFF, 255, 0x40, 0xFF);
        }
        else {
            SetRenderVertexColor(255, 0, 0);
            RenderImage(-88.0, -56.0, 8.0, 0.125, 0.125, 128.0, 128.0, 256.0, 256.0, 256.0, 256.0, 0xFF, self->texturePlayerSel);
            RenderRect(-96.0, -54.0, 8.0, 16.0, 4.0, 0xFF, 0xFF, 0xFF, 0xFF);
            RenderRect(-90.0, -48.0, 8.0, 4.0, 16.0, 0xFF, 0xFF, 0xFF, 0xFF);
            RenderRect(-36.0, -54.0, 8.0, 10.0, 4.0, 0xFF, 0xFF, 0xFF, 0xFF);
            RenderRect(-52.0, -54.0, 8.0, 10.0, 4.0, 0xFF, 0xFF, 0xFF, 0xFF);
            RenderRect(-68.0, -54.0, 8.0, 10.0, 4.0, 0xFF, 0xFF, 0xFF, 0xFF);
            RenderRect(-90.0, -26.0, 8.0, 4.0, 10.0, 0xFF, 0xFF, 0xFF, 0xFF);
        }
    }
    if ((self->playerID == SAVESEL_SONIC || self->playerID == SAVESEL_ST) && self->flag)
        SetRenderVertexColor(0xFF, 0xFF, 0x40);
    else
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    RenderText(self->textSonic, FONT_TEXT, self->sonicX, -22.0, 8.0, 0.2, 255);

    if (saveGame->tailsUnlocked) {
        if ((self->playerID == SAVESEL_TAILS || self->playerID == SAVESEL_ST) && self->flag)
            SetRenderVertexColor(0xFF, 0xFF, 0x40);
        else
            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    }
    else {
        SetRenderVertexColor(0xA0, 0xA0, 0xA0);
    }
    RenderText(self->textTails, FONT_TEXT, self->tailsX, -64.0, 8.0, 0.2, 0xFF);
    if (saveGame->knuxUnlocked) {
        if (self->playerID == SAVESEL_KNUX && self->flag)
            SetRenderVertexColor(0xFF, 0xFF, 0x40);
        else
            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    }
    else {
        SetRenderVertexColor(0xA0, 0xA0, 0xA0);
    }
    RenderText(self->textKnux, FONT_TEXT, self->knuxX, -22.0, 8.0, 0.2, 0xFF);

    NewRenderState();
    SetRenderMatrix(NULL);
    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    if (self->backPressed)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, self->alpha, self->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, self->alpha, self->textureArrows);
}
