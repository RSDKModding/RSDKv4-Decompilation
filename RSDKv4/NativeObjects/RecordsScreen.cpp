#include "RetroEngine.hpp"

float timeAttackU[] = { 1.0, 321.0, 641.0, 1.0, 321.0, 641.0, 641.0 };
float timeAttackV[] = { 1.0, 1.0, 1.0, 241.0, 241.0, 241.0, 721.0 };

void RecordsScreen_Create(void *objPtr)
{
    RSDK_THIS(RecordsScreen);
    self->labelPtr                  = CREATE_ENTITY(TextLabel);
    self->labelPtr->fontID          = FONT_HEADING;
    self->labelPtr->scale           = 0.15;
    self->labelPtr->alpha           = 256;
    self->labelPtr->x               = -144.0;
    self->labelPtr->y               = 100.0;
    self->labelPtr->z               = 16.0;
    self->labelPtr->state           = TEXTLABEL_STATE_IDLE;
    self->labelPtr->useRenderMatrix = true;

    self->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", -1);
    SetMeshVertexColors(self->meshPanel, 0, 0, 0, 0xC0);

    self->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA5551);
    SetStringToFont(self->textRecords, strRecords, FONT_LABEL);

    self->recordTextWidth = GetTextWidth(self->textRecords, FONT_LABEL, 0.125) * 0.5;

    self->buttons[RECORDSSCREEN_BUTTON_PLAY]                  = CREATE_ENTITY(PushButton);
    self->buttons[RECORDSSCREEN_BUTTON_PLAY]->useRenderMatrix = true;
    self->buttons[RECORDSSCREEN_BUTTON_PLAY]->x               = -64.0;
    self->buttons[RECORDSSCREEN_BUTTON_PLAY]->y               = -52.0;
    self->buttons[RECORDSSCREEN_BUTTON_PLAY]->z               = 0.0;
    self->buttons[RECORDSSCREEN_BUTTON_PLAY]->scale           = 0.175;
    self->buttons[RECORDSSCREEN_BUTTON_PLAY]->bgColor         = 0x00A048;
    self->buttons[RECORDSSCREEN_BUTTON_PLAY]->bgColorSelected = 0x00C060;
    SetStringToFont(self->buttons[RECORDSSCREEN_BUTTON_PLAY]->text, strPlay, FONT_LABEL);

    self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]                  = CREATE_ENTITY(PushButton);
    self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->useRenderMatrix = true;
    self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->x               = 64.0;
    self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->y               = -52.0;
    self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->z               = 0.0;
    self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->scale           = 0.175;
    self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->bgColor         = 0x00A048;
    self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->bgColorSelected = 0x00C060;
    SetStringToFont(self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->text, strNextAct, FONT_LABEL);

    self->state = RECORDSSCREEN_STATE_SETUP;
    debugMode   = false;
}
void RecordsScreen_Main(void *objPtr)
{
    RSDK_THIS(RecordsScreen);
    NativeEntity_TimeAttack *timeAttack = (NativeEntity_TimeAttack *)self->timeAttack;
    SaveGame *saveGame                  = (SaveGame *)saveRAM;

    switch (self->state) {
        case RECORDSSCREEN_STATE_SETUP: {
            int textureID = (self->zoneID * timeAttack_ActCount) / 6;
            textureID++;

            self->actCount     = timeAttack_ActCount;
            self->recordOffset = timeAttack_ActCount * self->zoneID;
            char pathBuf[0x40];
            sprintf(pathBuf, "Data/Game/Menu/TimeAttack%d.png", textureID);
            if (Engine.gameType == GAME_SONIC1 && self->zoneID == 6) // dumb stupid dumb
                sprintf(pathBuf, "Data/Game/Menu/Intro.png");

            if (timeAttackTex) {
                ReplaceTexture(pathBuf, timeAttackTex);
            }
            else {
                self->textureTimeAttack = LoadTexture(pathBuf, TEXFMT_RGBA5551);
            }

            if (Engine.gameType == GAME_SONIC1) {
                self->actCount = 3;
                if (self->zoneID == 6) { // final
                    self->actCount     = 1;
                    self->recordOffset = (timeAttack_ActCount * 6);
                }
                else if (self->zoneID == 7) { // special
                    self->actCount     = 6;
                    self->recordOffset = (timeAttack_ActCount * 6) + 1;
                }
            }
            else if (Engine.gameType == GAME_SONIC2) {
                if (self->zoneID == 7) // metropolis sux
                    self->actCount = 3;

                if (self->zoneID >= 8) {
                    self->actCount = 1;
                    switch (self->zoneID) {
                        case 8: self->recordOffset = (timeAttack_ActCount * 8) + 1; break;
                        case 9: self->recordOffset = 22; break;
                        case 10: self->recordOffset = 20; break;
#if !RETRO_USE_ORIGINAL_CODE
                        case 11: self->recordOffset = 23; break;
#endif
                    }
                }
            }

            self->state = RECORDSSCREEN_STATE_ENTER;
            if (Engine.gameType == GAME_SONIC2 && self->zoneID >= 9) {
                switch (self->zoneID) {
                    default: break;
                    case 9:
#if !RETRO_USE_ORIGINAL_CODE
                    case 11:
#endif
                        self->timeAttackU = timeAttackU[0];
                        self->timeAttackV = timeAttackV[0];
                        break;
                    case 10:
                        self->timeAttackU = timeAttackU[1];
                        self->timeAttackV = timeAttackV[1];
                        break;
                }
            }
            else if (Engine.gameType == GAME_SONIC1 && self->zoneID >= 6) {
                switch (self->zoneID) {
                    default: break;
                    case 6: // dumber stupider dumber
                        self->timeAttackU = timeAttackU[6];
                        self->timeAttackV = timeAttackV[6];
                        break;
                    case 7:
                        self->timeAttackU = timeAttackU[(self->recordOffset - 1) % 6];
                        self->timeAttackV = timeAttackV[(self->recordOffset - 1) % 6];
                        break;
                }
            }
            else {
                self->timeAttackU = timeAttackU[self->recordOffset % 6];
                self->timeAttackV = timeAttackV[self->recordOffset % 6];
            }

            int pos = 0;
            if (Engine.gameType == GAME_SONIC1) {
                switch (self->zoneID) {
                    default: pos = timeAttack_ActCount * self->zoneID; break;
                    case 7: // special stage
                        pos = timeAttack_ActCount * 6;
                        pos++;
                        break;
                }
            }
            else {
                if (self->zoneID >= 8) {
                    pos = timeAttack_ActCount * 8;
                    ++pos;
                    pos += self->zoneID - 8;
                }
                else {
                    pos = timeAttack_ActCount * self->zoneID;
                }
            }
            pos *= 3;

            int nameID = self->recordOffset == (timeAttack_ActCount * 8) + 1 ? self->recordOffset + 1 : self->recordOffset;
            SetStringToFont(self->labelPtr->text, strSaveStageList[nameID], 0);
            SetStringToFont8(self->rank1st, "1.", FONT_LABEL);
            AddTimeStringToFont(self->rank1st, saveGame->records[pos + (3 * self->actID)], FONT_LABEL);
            SetStringToFont8(self->rank2nd, "2.", FONT_LABEL);
            AddTimeStringToFont(self->rank2nd, saveGame->records[pos + (3 * self->actID) + 1], FONT_LABEL);
            SetStringToFont8(self->rank3rd, "3.", FONT_LABEL);
            AddTimeStringToFont(self->rank3rd, saveGame->records[pos + (3 * self->actID) + 2], FONT_LABEL);
        }
            // fallthrough

        case RECORDSSCREEN_STATE_ENTER: {
            if (self->buttonAlpha < 0x100)
                self->buttonAlpha += 8;

            self->scale = fminf(self->scale + ((1.05 - self->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            MatrixScaleXYZF(&self->renderMatrix, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);
            memcpy(&self->labelPtr->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            memcpy(&self->buttons[RECORDSSCREEN_BUTTON_PLAY]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            if (self->actCount > 1)
                memcpy(&self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                self->timer       = 0.0;
                self->state       = RECORDSSCREEN_STATE_MAIN;
                self->buttonAlpha = 256;
            }
            break;
        }

        case RECORDSSCREEN_STATE_MAIN: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&self->matrixTemp);
            if (!usePhysicalControls) {
                if (touches <= 0) {
                    if (self->buttons[RECORDSSCREEN_BUTTON_PLAY]->state == PUSHBUTTON_STATE_SELECTED) {
                        self->state = RECORDSSCREEN_STATE_LOADSTAGE;
                        PlaySfxByName("Menu Select", false);
                        self->buttons[RECORDSSCREEN_BUTTON_PLAY]->state = PUSHBUTTON_STATE_FLASHING;
                    }
                    if (self->actCount > 1 && self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->state == PUSHBUTTON_STATE_SELECTED) {
                        PlaySfxByName("Menu Move", false);
                        self->state                                        = RECORDSSCREEN_STATE_FLIP;
                        self->flipRight                                    = false;
                        self->actID                                        = (self->actID + 1) % self->actCount;
                        self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->state = PUSHBUTTON_STATE_UNSELECTED;
                    }
                }
                else {
                    self->buttons[RECORDSSCREEN_BUTTON_PLAY]->state    = PUSHBUTTON_STATE_UNSELECTED;
                    self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->state = PUSHBUTTON_STATE_UNSELECTED;
                    NativeEntity_PushButton *button                    = self->buttons[RECORDSSCREEN_BUTTON_PLAY];
                    if (CheckTouchRect(-64.0, -58.0, ((64.0 * button->scale) + button->textWidth) * 0.8, 12.0) >= 0) {
                        self->buttons[RECORDSSCREEN_BUTTON_PLAY]->state = PUSHBUTTON_STATE_SELECTED;
                    }
                    else if (self->actCount > 1) {
                        button = self->buttons[RECORDSSCREEN_BUTTON_NEXTACT];
                        if (CheckTouchRect(64.0, -58.0, ((64.0 * button->scale) + button->textWidth) * 0.8, 12.0) >= 0) {
                            self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->state = PUSHBUTTON_STATE_SELECTED;
                        }
                    }
                }

                if (touches > 0 && self->state == RECORDSSCREEN_STATE_MAIN && self->actCount > 1) {
                    if (self->selectionEnabled) {
                        if (self->lastTouchX - touchXF[0] > 16.0f) {
                            PlaySfxByName("Menu Move", false);
                            self->state            = RECORDSSCREEN_STATE_FLIP;
                            self->flipRight        = false;
                            self->selectionEnabled = false;
                            self->actID            = (self->actID + 1) % self->actCount;
                        }
                        else if (self->lastTouchX - touchXF[0] < -16.0f) {
                            PlaySfxByName("Menu Move", false);
                            if (--self->actID < 0)
                                self->actID = self->actCount - 1;
                            self->state            = RECORDSSCREEN_STATE_FLIP;
                            self->flipRight        = true;
                            self->selectionEnabled = false;
                        }
                    }
                    else {
                        self->selectionEnabled = true;
                    }
                    self->lastTouchX = touchXF[0];
                }
                else {
                    self->selectionEnabled = false;
                }

                if (touches <= 0) {
                    if (self->prevActPressed) {
                        PlaySfxByName("Menu Move", false);
                        if (--self->actID < 0)
                            self->actID = self->actCount - 1;
                        self->state          = RECORDSSCREEN_STATE_FLIP;
                        self->prevActPressed = false;
                        self->flipRight      = true;
                    }
                    if (self->nextActPressed) {
                        PlaySfxByName("Menu Move", false);
                        self->state          = RECORDSSCREEN_STATE_FLIP;
                        self->nextActPressed = false;
                        self->flipRight      = false;
                        self->actID          = (self->actID + 1) % self->actCount;
                    }
                    if (self->backPressed) {
                        PlaySfxByName("Menu Back", false);
                        self->backPressed = false;
                        self->state       = RECORDSSCREEN_STATE_EXIT;
                    }
                }
                else {
                    self->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (self->actCount > 1) {
                        self->prevActPressed = CheckTouchRect(-162.0, 0.0, 32.0, 32.0) >= 0;
                        self->nextActPressed = CheckTouchRect(162.0, 0.0, 32.0, 32.0) >= 0;
                    }
                }

                if (self->state == RECORDSSCREEN_STATE_MAIN) {
                    if (keyDown.left) {
                        self->selectedButton = RECORDSSCREEN_BUTTON_NEXTACT;
                        usePhysicalControls  = true;
                    }
                    else {
                        if (keyDown.right) {
                            self->selectedButton = RECORDSSCREEN_BUTTON_PLAY;
                            usePhysicalControls  = true;
                        }
                        else if (keyPress.B) {
                            PlaySfxByName("Menu Back", false);
                            self->backPressed = false;
                            self->state       = RECORDSSCREEN_STATE_EXIT;
                        }
                    }
                }
            }
            else {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    self->prevActPressed = false;
                    self->nextActPressed = false;
                    self->backPressed    = false;

                    if (self->actCount > 1) {
                        if (keyPress.left) {
                            PlaySfxByName("Menu Move", false);
                            self->selectedButton--;
                            if (self->selectedButton < 0) {
                                self->selectedButton = 1;
                                PlaySfxByName("Menu Move", false);
                                if (--self->actID < 0)
                                    self->actID = self->actCount - 1;
                                self->state     = RECORDSSCREEN_STATE_FLIP;
                                self->flipRight = true;
                            }
                        }
                        else if (keyPress.right) {
                            PlaySfxByName("Menu Move", false);
                            self->selectedButton++;
                            if (self->selectedButton >= 2) {
                                self->selectedButton = 0;
                                PlaySfxByName("Menu Move", false);
                                self->state     = RECORDSSCREEN_STATE_FLIP;
                                self->flipRight = false;
                                self->actID     = (self->actID + 1) % self->actCount;
                            }
                        }
                        for (int i = 0; i < 2; ++i) self->buttons[i]->state = PUSHBUTTON_STATE_UNSELECTED;
                        self->buttons[self->selectedButton]->state = PUSHBUTTON_STATE_SELECTED;
                    }
                    else {
                        self->buttons[self->selectedButton]->state = PUSHBUTTON_STATE_SELECTED;
                    }

                    if (keyPress.start || keyPress.A) {
                        if (self->selectedButton) {
                            PlaySfxByName("Menu Move", false);
                            self->state     = RECORDSSCREEN_STATE_FLIP;
                            self->flipRight = 0;
                            self->actID     = (self->actID + 1) % self->actCount;
                        }
                        else {
                            self->state = RECORDSSCREEN_STATE_LOADSTAGE;
                            PlaySfxByName("Menu Select", false);
                            self->buttons[RECORDSSCREEN_BUTTON_PLAY]->state = PUSHBUTTON_STATE_FLASHING;
                        }
                    }
                    else if (keyPress.B) {
                        PlaySfxByName("Menu Back", false);
                        self->backPressed = false;
                        self->state       = RECORDSSCREEN_STATE_EXIT;
                    }
                }
            }
            break;
        }

        case RECORDSSCREEN_STATE_FLIP: {
            int pos = 0;
            if (Engine.gameType == GAME_SONIC1) {
                switch (self->zoneID) {
                    default: pos += timeAttack_ActCount * self->zoneID; break;
                    case 7: // special stage
                        pos += timeAttack_ActCount * 6;
                        pos++;
                        break;
                }
            }
            else {
                if (self->zoneID >= 8) {
                    pos += timeAttack_ActCount * 8;
                    ++pos; // metropolis moment
                    pos += self->zoneID - 8;
                }
                else {
                    pos += timeAttack_ActCount * self->zoneID;
                }
            }
            pos *= 3;

            if (!self->flipRight) {
                self->rotationY -= (10.0 * Engine.deltaTime);
            }
            else {
                self->rotationY += (10.0 * Engine.deltaTime);
            }

            if (abs(self->rotationY) > (M_PI * 0.5)) {
                self->state     = RECORDSSCREEN_STATE_FINISHFLIP;
                self->rotationY = self->rotationY < 0.0f ? -(M_PI * 1.5) : (M_PI * 1.5);

                if (Engine.gameType == GAME_SONIC2 && self->zoneID >= 9) {
                    switch (self->zoneID) {
                        default: break;
                        case 9:
#if !RETRO_USE_ORIGINAL_CODE
                        case 11:
#endif
                            self->timeAttackU = timeAttackU[0];
                            self->timeAttackV = timeAttackV[0];
                            break;
                        case 10:
                            self->timeAttackU = timeAttackU[1];
                            self->timeAttackV = timeAttackV[1];
                            break;
                    }
                }
                else if (Engine.gameType == GAME_SONIC1 && self->zoneID >= 6) {
                    switch (self->zoneID) {
                        default: break;
                        case 6: // dumber stupider dumber
                            self->timeAttackU = timeAttackU[6];
                            self->timeAttackV = timeAttackV[6];
                            break;
                        case 7:
                            self->timeAttackU = timeAttackU[(self->recordOffset + self->actID - 1) % 6];
                            self->timeAttackV = timeAttackV[(self->recordOffset + self->actID - 1) % 6];
                            break;
                    }
                }
                else {
                    self->timeAttackU = timeAttackU[(self->recordOffset + self->actID) % 6];
                    self->timeAttackV = timeAttackV[(self->recordOffset + self->actID) % 6];
                }
                int nameID = self->recordOffset == (timeAttack_ActCount * 8) + 1 ? self->recordOffset + 1 : self->recordOffset;
                SetStringToFont(self->labelPtr->text, strSaveStageList[nameID + self->actID], FONT_HEADING);
                SetStringToFont8(self->rank1st, "1.", FONT_LABEL);
                AddTimeStringToFont(self->rank1st, saveGame->records[pos + (3 * self->actID)], FONT_LABEL);
                SetStringToFont8(self->rank2nd, "2.", FONT_LABEL);
                AddTimeStringToFont(self->rank2nd, saveGame->records[pos + (3 * self->actID) + 1], FONT_LABEL);
                SetStringToFont8(self->rank3rd, "3.", FONT_LABEL);
                AddTimeStringToFont(self->rank3rd, saveGame->records[pos + (3 * self->actID) + 2], FONT_LABEL);
            }

            NewRenderState();
            MatrixRotateYF(&self->renderMatrix, self->rotationY);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);
            break;
        }

        case RECORDSSCREEN_STATE_FINISHFLIP: {
            if (!self->flipRight) {
                self->rotationY -= (10.0 * Engine.deltaTime);
                if (self->rotationY < -(M_PI_2)) {
                    self->state     = RECORDSSCREEN_STATE_MAIN;
                    self->rotationY = 0.0;
                }
            }
            else {
                self->rotationY += (10.0 * Engine.deltaTime);
                if (self->rotationY > M_PI_2) {
                    self->state     = RECORDSSCREEN_STATE_MAIN;
                    self->rotationY = 0.0;
                }
            }
            NewRenderState();
            MatrixRotateYF(&self->renderMatrix, self->rotationY);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);
            break;
        }

        case RECORDSSCREEN_STATE_EXIT: {
            if (self->buttonAlpha > 0)
                self->buttonAlpha -= 8;

            if (self->timer < 0.2)
                self->scale = fmaxf(self->scale + ((1.5f - self->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);
            else
                self->scale = fmaxf(self->scale + ((-1.0f - self->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);

            NewRenderState();
            MatrixScaleXYZF(&self->renderMatrix, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);

            memcpy(&self->labelPtr->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            memcpy(&self->buttons[RECORDSSCREEN_BUTTON_PLAY]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            if (self->actCount > 1)
                memcpy(&self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                timeAttack->state = TIMEATTACK_STATE_EXITSUBMENU;
                RemoveNativeObject(self->buttons[RECORDSSCREEN_BUTTON_PLAY]);
                RemoveNativeObject(self->buttons[RECORDSSCREEN_BUTTON_NEXTACT]);
                RemoveNativeObject(self->labelPtr);
                RemoveNativeObject(self);
                return;
            }
            break;
        }

        case RECORDSSCREEN_STATE_LOADSTAGE: {
            SetRenderMatrix(&self->matrixTemp);
            if (self->buttons[RECORDSSCREEN_BUTTON_PLAY]->state == PUSHBUTTON_STATE_UNSELECTED) {
                SetGlobalVariableByName("options.saveSlot", 0);
                SetGlobalVariableByName("options.gameMode", 2);
                SetGlobalVariableByName("player.lives", 1);
                SetGlobalVariableByName("player.score", 0);
                SetGlobalVariableByName("player.scoreBonus", 50000);
                SetGlobalVariableByName("specialStage.listPos", 0);
                SetGlobalVariableByName("specialStage.emeralds", 0);
                SetGlobalVariableByName("specialStage.nextZone", 0);
                SetGlobalVariableByName("specialStage.nextZone", 0); // We gotta make extra sure it's reset, I guess
#if !RETRO_USE_ORIGINAL_CODE
                SetGlobalVariableByName("lampPostID", 0);
#endif
                // Bug Details:
                // Despite using the variable "lampPostID" for checkpoints, Sonic 1 still tries to set "starPostID", which only exists in Sonic 2
                // As a result, the checkpoint variable doesn't get reset in Sonic 1, allowing you to start an act further into the level if exploited
                // (This is fixed in the decomp with the above line)
                SetGlobalVariableByName("starPostID", 0);
                SetGlobalVariableByName("timeAttack.result", 0);

                if (self->zoneID >= 7 && Engine.gameType == GAME_SONIC1)
                    InitStartingStage(STAGELIST_SPECIAL, self->actID, 0);
                else if (self->zoneID >= 9 && Engine.gameType == GAME_SONIC2) {
                    switch (self->zoneID) {
                        default: break;
                        case 9: InitStartingStage(STAGELIST_BONUS, 1, 0); break;
                        case 10: InitStartingStage(STAGELIST_REGULAR, self->zoneID * timeAttack_ActCount, 0); break;
#if !RETRO_USE_ORIGINAL_CODE
                        case 11: InitStartingStage(STAGELIST_BONUS, 0, 0); break;
#endif
                    }
                }
#if !RETRO_USE_ORIGINAL_CODE
                // There's a bit of a bug in the code that makes Wing Fortress go to Metropolis 3, even though the records save perfectly fine.
                // So, if you are in Sonic 2 and the zone selected is Wing Fortress, load Stage 18 (this is always Wing Fortress under normal circumstances).
                else if (Engine.gameType == GAME_SONIC2 && self->zoneID == 8)
                    InitStartingStage(STAGELIST_REGULAR, 18, 0);
#endif
                else
                    InitStartingStage(STAGELIST_REGULAR, self->zoneID * timeAttack_ActCount + self->actID, 0);

                self->state      = RECORDSSCREEN_STATE_SHOWRESULTS;
                self->flashTimer = 0.0;
                self->taResultID = GetGlobalVariableID("timeAttack.result");
                BackupNativeObjects();
                CREATE_ENTITY(FadeScreen);
            }
            break;
        }
        case RECORDSSCREEN_STATE_SHOWRESULTS: {
            int pos = 0;
            if (Engine.gameType == GAME_SONIC1) {
                switch (self->zoneID) {
                    default: pos += timeAttack_ActCount * self->zoneID; break;
                    case 7: // special stage
                        pos += timeAttack_ActCount * 6;
                        pos++;
                        break;
                }
            }
            else {
                if (self->zoneID >= 8) {
                    pos += timeAttack_ActCount * 8;
                    ++pos;
                    pos += self->zoneID - 8;
                }
                else {
                    pos += timeAttack_ActCount * self->zoneID;
                }
            }
            pos *= 3;

            SetRenderMatrix(&self->matrixTemp);
            self->state = RECORDSSCREEN_STATE_MAIN;
            SetMeshVertexColors(self->meshPanel, 0, 0, 0, 0xC0);
            if (globalVariables[self->taResultID] > 0) {
                int *records = &saveGame->records[pos + (3 * self->actID)];
                int time     = globalVariables[self->taResultID];

                for (int r = 0; r < 3; ++r) {
                    if (time < records[r]) {
                        for (int s = 2; s > r; --s) records[s] = records[s - 1];
                        records[r] = time;
                        self->rank = r + 1;
                        break;
                    }
                }

                if (self->rank) {
                    SetStringToFont8(self->rank1st, "1.", FONT_LABEL);
                    AddTimeStringToFont(self->rank1st, records[0], FONT_LABEL);
                    SetStringToFont8(self->rank2nd, "2.", FONT_LABEL);
                    AddTimeStringToFont(self->rank2nd, records[1], FONT_LABEL);
                    SetStringToFont8(self->rank3rd, "3.", FONT_LABEL);
                    AddTimeStringToFont(self->rank3rd, records[2], FONT_LABEL);

                    self->timer = 0.0;
                    self->state = RECORDSSCREEN_STATE_EXITRESULTS;
                    PlaySfxByName("Event", false);
                    WriteSaveRAMData();
                }

                int pos = 0;
                for (int i = 0; i < timeAttack_ZoneCount; ++i) {
                    timeAttack->totalTime = 0;
                    if (Engine.gameType == GAME_SONIC1) {
                        switch (i) {
                            default:
                                for (int a = 0; a < timeAttack_ActCount; ++a) timeAttack->totalTime += saveGame->records[3 * (pos + a)];
                                pos += timeAttack_ActCount;
                                break;
                            case 6: // final zone
                                timeAttack->totalTime += saveGame->records[3 * pos];
                                pos++;
                                break;
                            case 7: // special stage
                                for (int a = 0; a < 6; ++a) timeAttack->totalTime += saveGame->records[3 * (pos + a)];
                                pos += 6;
                                break;
                        }
                    }
                    else {
                        if (i < 9) {
                            for (int a = 0; a < timeAttack_ActCount; ++a) timeAttack->totalTime += saveGame->records[3 * (pos + a)];
                            pos += timeAttack_ActCount;
                        }
                        else {
                            timeAttack->totalTime += saveGame->records[3 * pos];
                            pos++;
                        }
                    }
                    SetStringToFont8(timeAttack->zoneButtons[i]->timeText, "", FONT_TEXT);
                    AddTimeStringToFont(timeAttack->zoneButtons[i]->timeText, timeAttack->totalTime, FONT_TEXT);
                    timeAttack->zoneButtons[i]->textWidth = GetTextWidth(timeAttack->zoneButtons[i]->zoneText, FONT_TEXT, 0.25) * 0.5;
                }

                pos                   = 0;
                timeAttack->totalTime = 0;
                for (int z = 0; z < timeAttack_ZoneCount; ++z) {
                    // 1st
                    if (Engine.gameType == GAME_SONIC1) {
                        switch (z) {
                            default:
                                for (int a = 0; a < timeAttack_ActCount; ++a) timeAttack->totalTime += saveGame->records[3 * (pos + a)];
                                pos += timeAttack_ActCount;
                                break;
                            case 6: // final zone
                                timeAttack->totalTime += saveGame->records[3 * pos];
                                pos++;
                                break;
                            case 7: // special stage
                                for (int a = 0; a < 6; ++a) timeAttack->totalTime += saveGame->records[3 * (pos + a)];
                                pos += 6;
                                break;
                        }
                    }
                    else {
                        if (z < 9) {
#if !RETRO_USE_ORIGINAL_CODE
                            if (z < 11)
                                for (int a = 0; a < timeAttack_ActCount; ++a) timeAttack->totalTime += saveGame->records[3 * (pos + a)];
#else
                            for (int a = 0; a < timeAttack_ActCount; ++a) timeAttack->totalTime += saveGame->records[3 * (pos + a)];
#endif
                            pos += timeAttack_ActCount;
                        }
                        else {
                            timeAttack->totalTime += saveGame->records[3 * pos];
                            pos++;
                        }
                    }
                }

                SetStringToFont(timeAttack->button->text, strTotalTime, FONT_LABEL);
                AddTimeStringToFont(timeAttack->button->text, timeAttack->totalTime, FONT_LABEL);
                if (timeAttack->totalTime <= 270000) {
                    int ach    = -1;
                    int status = 100;
                    for (int i = 0; i < achievementCount; ++i) {
                        if (StrComp(achievements[i].name, "Beat the Clock")) {
                            ach = i;
                            break;
                        }
                    }
                    if (ach >= 0)
                        SetAchievement(&ach, &status);
                }
            }
            break;
        }
        case RECORDSSCREEN_STATE_EXITRESULTS: {
            SetRenderMatrix(&self->matrixTemp);
            self->flashTimer += Engine.deltaTime;
            if (self->flashTimer > 0.1)
                self->flashTimer -= 0.1;

            self->timer += Engine.deltaTime;
            if (self->timer > 2.0) {
                self->timer      = 0.0;
                self->flashTimer = 0.0;
                self->state      = RECORDSSCREEN_STATE_MAIN;
                self->rank       = 0;
            }
            break;
        }
        default: break;
    }

    RenderMesh(self->meshPanel, MESH_COLORS, false);
    RenderRect(-124.0, 69.0, 0.0, 128.0, 98.0, 128, 128, 128, 255);
    RenderImage(-60.0, 20.0, 0.0, 0.38, 0.38, 159.0, 119.0, 318.0, 238.0, self->timeAttackU, self->timeAttackV, 255, self->textureTimeAttack);
    RenderText(self->textRecords, FONT_LABEL, 72.0 - self->recordTextWidth, 56.0, 0.0, 0.125, 255);

    if (self->flashTimer < 0.05 || self->rank != 1)
        RenderText(self->rank1st, FONT_LABEL, 24.0, 32.0, 0.0, 0.125, 255);

    if (self->flashTimer < 0.05 || self->rank != 2)
        RenderText(self->rank2nd, FONT_LABEL, 24.0, 8.0, 0.0, 0.125, 255);

    if (self->flashTimer < 0.05 || self->rank != 3)
        RenderText(self->rank3rd, FONT_LABEL, 24.0, -16.0, 0.0, 0.125, 255);

    NewRenderState();
    SetRenderMatrix(NULL);
    if (self->actCount > 1) {
        if (self->prevActPressed)
            RenderImageFlipH(-146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, self->buttonAlpha, self->textureArrows);
        else
            RenderImageFlipH(-146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, self->buttonAlpha, self->textureArrows);

        if (self->nextActPressed)
            RenderImage(146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, self->buttonAlpha, self->textureArrows);
        else
            RenderImage(146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, self->buttonAlpha, self->textureArrows);
    }

    if (self->backPressed)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, self->buttonAlpha, self->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, self->buttonAlpha, self->textureArrows);
}