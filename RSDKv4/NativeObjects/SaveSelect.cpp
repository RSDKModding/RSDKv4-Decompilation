#include "RetroEngine.hpp"

void SaveSelect_Create(void *objPtr)
{
    RSDK_THIS(SaveSelect);
    SaveGame *saveGame = (SaveGame *)saveRAM;

    self->menuControl      = (NativeEntity_MenuControl *)GetNativeObject(0);
    self->labelPtr         = CREATE_ENTITY(TextLabel);
    self->labelPtr->fontID = FONT_HEADING;
    if (Engine.language != RETRO_EN)
        self->labelPtr->scale = 0.12;
    else
        self->labelPtr->scale = 0.2;
    self->labelPtr->alpha = 0;
    self->labelPtr->z     = 0;
    self->labelPtr->state = TEXTLABEL_STATE_IDLE;
    SetStringToFont(self->labelPtr->text, strSaveSelect, FONT_HEADING);
    self->labelPtr->alignOffset = 512.0;

    self->deleteRotateY = DegreesToRad(22.5);
    MatrixRotateYF(&self->labelPtr->renderMatrix, self->deleteRotateY);
    MatrixTranslateXYZF(&self->matrix1, -128.0, 80.0, 160.0);
    MatrixMultiplyF(&self->labelPtr->renderMatrix, &self->matrix1);
    self->labelPtr->useRenderMatrix = true;

    self->delButton    = CREATE_ENTITY(PushButton);
    self->delButton->x = 384.0;
    self->delButton->y = -16.0;
    if (Engine.language == RETRO_FR)
        self->delButton->scale = 0.15;
    else
        self->delButton->scale = 0.2;

    self->delButton->bgColor = 0x00A048;
    SetStringToFont(self->delButton->text, strDelete, FONT_LABEL);

    self->saveButtons[SAVESELECT_BUTTON_NOSAVE] = CREATE_ENTITY(SubMenuButton);
    SetStringToFont(self->saveButtons[SAVESELECT_BUTTON_NOSAVE]->text, strNoSave, FONT_LABEL);
    self->saveButtons[SAVESELECT_BUTTON_NOSAVE]->matXOff = 512.0;
    self->saveButtons[SAVESELECT_BUTTON_NOSAVE]->textY   = -4.0;
    self->saveButtons[SAVESELECT_BUTTON_NOSAVE]->matZ    = 0.0;
    self->saveButtons[SAVESELECT_BUTTON_NOSAVE]->scale   = 0.1;

    self->rotateY[SAVESELECT_BUTTON_NOSAVE] = DegreesToRad(16.0);
    MatrixRotateYF(&self->saveButtons[SAVESELECT_BUTTON_NOSAVE]->matrix, self->rotateY[0]);
    MatrixTranslateXYZF(&self->matrix1, -128.0, 48.0, 160.0);
    MatrixMultiplyF(&self->saveButtons[SAVESELECT_BUTTON_NOSAVE]->matrix, &self->matrix1);
    self->saveButtons[SAVESELECT_BUTTON_NOSAVE]->useMatrix = true;
    ReadSaveRAMData();

    float y = 18.0;
    for (int i = SAVESELECT_BUTTON_SAVE1; i < SAVESELECT_BUTTON_COUNT; ++i) {
        self->saveButtons[i] = CREATE_ENTITY(SubMenuButton);

        int stagePos = saveGame->files[i - 1].stageID;
        if (stagePos >= 0x80) {
            SetStringToFont(self->saveButtons[i]->text, strSaveStageList[saveGame->files[i - 1].specialStageID + 19], FONT_LABEL);
            self->saveButtons[i]->state = SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED;
            self->saveButtons[i]->textY = 2.0;
            self->saveButtons[i]->scale = 0.08;
            self->deleteEnabled         = true;
        }
        else if (stagePos > 0) {
            if (stagePos - 1 > 18 && Engine.gameType == GAME_SONIC1)
                SetStringToFont(self->saveButtons[i]->text, strSaveStageList[25], FONT_LABEL);
            else
                SetStringToFont(self->saveButtons[i]->text, strSaveStageList[stagePos - 1], FONT_LABEL);
            self->saveButtons[i]->state = SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED;
            self->saveButtons[i]->textY = 2.0;
            self->saveButtons[i]->scale = 0.08;
            self->deleteEnabled         = true;
        }
        else {
            SetStringToFont(self->saveButtons[i]->text, strNewGame, FONT_LABEL);
            self->saveButtons[i]->textY = -4.0;
            self->saveButtons[i]->scale = 0.1;
        }

        self->saveButtons[i]->matXOff = 512.0;
        self->saveButtons[i]->matZ    = 0.0;
        self->saveButtons[i]->symbol  = saveGame->files[i - 1].characterID;
        self->saveButtons[i]->flags   = saveGame->files[i - 1].emeralds;
        self->rotateY[i]              = DegreesToRad(16.0);
        MatrixRotateYF(&self->saveButtons[i]->matrix, self->rotateY[i]);
        MatrixTranslateXYZF(&self->matrix1, -128.0, y, 160.0);
        MatrixMultiplyF(&self->saveButtons[i]->matrix, &self->matrix1);
        self->saveButtons[i]->useMatrix = true;
        y -= 30.0;
    }
}
void SaveSelect_Main(void *objPtr)
{
    RSDK_THIS(SaveSelect);
    SaveGame *saveGame = (SaveGame *)saveRAM;

    switch (self->state) {
        case SAVESELECT_STATE_SETUP: {
            self->timer += Engine.deltaTime;
            if (self->timer > 1.0) {
                self->timer = 0.0;
                self->state = SAVESELECT_STATE_ENTER;
            }
            break;
        }
        case SAVESELECT_STATE_ENTER: {
            self->labelPtr->alignOffset = self->labelPtr->alignOffset / (1.125 * (60.0 * Engine.deltaTime));
            if (self->deleteEnabled)
                self->delButton->x = ((92.0 - self->delButton->x) / (8.0 * (60.0 * Engine.deltaTime))) + self->delButton->x;

            float div = (60.0 * Engine.deltaTime) * 16.0;
            for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) self->saveButtons[i]->matXOff += ((-176.0 - self->saveButtons[i]->matXOff) / div);

            self->timer += (Engine.deltaTime + Engine.deltaTime);
            self->labelPtr->alpha = (256.0 * self->timer);
            if (self->timer > 1.0) {
                self->timer    = 0.0;
                self->state    = SAVESELECT_STATE_MAIN;
                inputPress.start = false;
                inputPress.A     = false;
            }
            break;
        }
        case SAVESELECT_STATE_MAIN:
        case SAVESELECT_STATE_MAIN_DELETING: {
            if (self->state != SAVESELECT_STATE_MAIN_DELETING) {
                if (!self->deleteEnabled)
                    self->delButton->x += ((512.0 - self->delButton->x) / ((60.0 * Engine.deltaTime) * 16.0));
                else
                    self->delButton->x += ((92.0 - self->delButton->x) / ((60.0 * Engine.deltaTime) * 16.0));
            }

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (inputPress.up) {
                        PlaySfxByName("Menu Move", false);
                        self->selectedButton--;
                        if (self->deleteEnabled && self->selectedButton < SAVESELECT_BUTTON_NOSAVE) {
                            self->selectedButton = SAVESELECT_BUTTON_COUNT;
                        }
                        else if (self->selectedButton < SAVESELECT_BUTTON_NOSAVE) {
                            self->selectedButton = SAVESELECT_BUTTON_COUNT - 1;
                        }
                    }
                    else if (inputPress.down) {
                        PlaySfxByName("Menu Move", false);
                        self->selectedButton++;
                        if (self->deleteEnabled && self->selectedButton > SAVESELECT_BUTTON_COUNT) {
                            self->selectedButton = SAVESELECT_BUTTON_NOSAVE;
                        }
                        else if (self->selectedButton >= SAVESELECT_BUTTON_COUNT) {
                            self->selectedButton = SAVESELECT_BUTTON_NOSAVE;
                        }
                    }

                    for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) self->saveButtons[i]->b = 0xFF;

                    if (self->deleteEnabled && (inputPress.left || inputPress.right)) {
                        if (self->selectedButton < SAVESELECT_BUTTON_COUNT) {
                            self->selectedButton   = SAVESELECT_BUTTON_COUNT;
                            self->delButton->state = PUSHBUTTON_STATE_SELECTED;
                        }
                        else {
                            self->selectedButton                         = SAVESELECT_BUTTON_NOSAVE;
                            self->saveButtons[self->selectedButton]->b = 0x00;
                            self->delButton->state                       = PUSHBUTTON_STATE_UNSELECTED;
                        }
                    }
                    else {
                        if (self->selectedButton >= SAVESELECT_BUTTON_COUNT) {
                            self->delButton->state = PUSHBUTTON_STATE_SELECTED;
                        }
                        else {
                            self->saveButtons[self->selectedButton]->b = 0x00;
                            self->delButton->state                       = PUSHBUTTON_STATE_UNSELECTED;
                        }
                    }

                    if (inputPress.start || inputPress.A) {
                        if (self->selectedButton < SAVESELECT_BUTTON_COUNT) {
                            if (self->state == SAVESELECT_STATE_MAIN_DELETING) {
                                if (self->selectedButton > SAVESELECT_BUTTON_NOSAVE && saveGame->files[self->selectedButton - 1].stageID > 0) {
                                    PlaySfxByName("Menu Select", false);
                                    self->state                                      = SAVESELECT_STATE_DELSETUP;
                                    self->saveButtons[self->selectedButton]->b     = 0xFF;
                                    self->saveButtons[self->selectedButton]->state = SUBMENUBUTTON_STATE_SAVEBUTTON_UNSELECTED;
                                }
                            }
                            else {
                                PlaySfxByName("Menu Select", false);
                                self->saveButtons[self->selectedButton]->state = SUBMENUBUTTON_STATE_FLASHING2;
                                if (self->selectedButton > SAVESELECT_BUTTON_NOSAVE && saveGame->files[self->selectedButton - 1].stageID > 0) {
                                    StopMusic(true);
                                    self->saveButtons[self->selectedButton]->state = SUBMENUBUTTON_STATE_SAVEBUTTON_UNSELECTED;
                                }
                                self->saveButtons[self->selectedButton]->b = 0xFF;
                                self->state                                  = SAVESELECT_STATE_LOADSAVE;
                            }
                        }
                        else {
                            if (Engine.gameType == GAME_SONIC1)
                                PlaySfxByName("Lamp Post", false);
                            else
                                PlaySfxByName("Star Post", false);
                            self->delButton->state = PUSHBUTTON_STATE_FLASHING;
                            if (self->state == SAVESELECT_STATE_MAIN_DELETING) {
                                self->state = SAVESELECT_STATE_MAIN;
                                PlaySfxByName("Menu Back", false);
                                for (int i = SAVESELECT_BUTTON_SAVE1; i < SAVESELECT_BUTTON_COUNT; ++i) {
                                    if (self->saveButtons[i]->state == SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED)
                                        self->saveButtons[i]->useMeshH = false;
                                }
                                self->delButton->state = PUSHBUTTON_STATE_UNSELECTED;
                            }
                            else
                                self->state = SAVESELECT_STATE_LOADSAVE;
                        }
                    }
                }
            }
            else {
                float y = 48.0;
                for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) {
                    if (touches > 0) {
                        if (CheckTouchRect(-64.0, y, 96.0, 12.0) < 0)
                            self->saveButtons[i]->b = 0xFF;
                        else
                            self->saveButtons[i]->b = 0x00;
                    }
                    else if (!self->saveButtons[i]->b) {
                        self->selectedButton = i;
                        if (self->state == SAVESELECT_STATE_MAIN_DELETING) {
                            if (self->selectedButton > SAVESELECT_BUTTON_NOSAVE && saveGame->files[self->selectedButton - 1].stageID > 0) {
                                PlaySfxByName("Menu Select", false);
                                self->state                                      = SAVESELECT_STATE_DELSETUP;
                                self->saveButtons[self->selectedButton]->b     = 0xFF;
                                self->saveButtons[self->selectedButton]->state = SUBMENUBUTTON_STATE_SAVEBUTTON_UNSELECTED;
                            }
                        }
                        else {
                            PlaySfxByName("Menu Select", false);
                            self->saveButtons[self->selectedButton]->state = SUBMENUBUTTON_STATE_FLASHING2;
                            if (self->selectedButton > SAVESELECT_BUTTON_NOSAVE && saveGame->files[self->selectedButton - 1].stageID > 0) {
                                StopMusic(true);
                                self->saveButtons[self->selectedButton]->state = SUBMENUBUTTON_STATE_SAVEBUTTON_UNSELECTED;
                            }
                            self->saveButtons[self->selectedButton]->b = 0xFF;
                            self->state                                  = SAVESELECT_STATE_LOADSAVE;
                        }

                        break;
                    }
                    y -= 30.0;
                }

                if (self->state == SAVESELECT_STATE_MAIN) {
                    if (!self->deleteEnabled) {
                        if (inputDown.up || inputDown.down || inputDown.left || inputDown.right) {
                            self->selectedButton = SAVESELECT_BUTTON_NOSAVE;
                            usePhysicalControls    = true;
                        }
                    }
                    else {
                        if (touches <= 0) {
                            if (self->delButton->state == PUSHBUTTON_STATE_SELECTED) {
                                self->selectedButton = SAVESELECT_BUTTON_COUNT;
                                if (Engine.gameType == GAME_SONIC1)
                                    PlaySfxByName("Lamp Post", false);
                                else
                                    PlaySfxByName("Star Post", false);
                                self->delButton->state = PUSHBUTTON_STATE_FLASHING;
                                self->state            = SAVESELECT_STATE_LOADSAVE;
                            }
                            else {
                                if (inputDown.up || inputDown.down || inputDown.left || inputDown.right) {
                                    self->selectedButton = SAVESELECT_BUTTON_NOSAVE;
                                    usePhysicalControls    = true;
                                }
                            }
                        }
                        else {
                            self->delButton->state = CheckTouchRect(self->delButton->x, self->delButton->y,
                                                                      (64.0 * self->delButton->scale) + self->delButton->textWidth, 12.0)
                                                       >= 0;
                            if (self->state == SAVESELECT_STATE_MAIN) {
                                if (inputDown.up || inputDown.down || inputDown.left || inputDown.right) {
                                    self->selectedButton = SAVESELECT_BUTTON_NOSAVE;
                                    usePhysicalControls    = true;
                                }
                            }
                        }
                    }
                }
                else if (self->state == SAVESELECT_STATE_MAIN_DELETING) {
                    if (touches > 0) {
                        if (CheckTouchRect(self->delButton->x, self->delButton->y,
                                           (64.0 * self->delButton->scale) + self->delButton->textWidth, 12.0)
                            >= 0) {
                            self->delButton->state = PUSHBUTTON_STATE_SELECTED;
                        }
                        else {
                            self->delButton->state = PUSHBUTTON_STATE_UNSELECTED;
                        }
                    }
                    else if (self->delButton->state == PUSHBUTTON_STATE_SELECTED) {
                        self->state = SAVESELECT_STATE_MAIN;
                        PlaySfxByName("Menu Back", false);
                        for (int i = SAVESELECT_BUTTON_SAVE1; i < SAVESELECT_BUTTON_COUNT; ++i) {
                            if (self->saveButtons[i]->state == SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED)
                                self->saveButtons[i]->useMeshH = false;
                        }
                        self->delButton->state = PUSHBUTTON_STATE_UNSELECTED;
                    }
                }
            }

            if (self->menuControl->state == MENUCONTROL_STATE_EXITSUBMENU)
                self->state = SAVESELECT_STATE_EXIT;
            break;
        }
        case SAVESELECT_STATE_EXIT: {
            self->labelPtr->alignOffset += 10.0 * (60.0 * Engine.deltaTime);
            self->delButton->x += 10.0 * (60.0 * Engine.deltaTime);
            for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) self->saveButtons[i]->matXOff += (11.0 * (60.0 * Engine.deltaTime));
            self->timer += (Engine.deltaTime + Engine.deltaTime);
            if (self->timer > 1.0) {
                self->timer = 0.0;
                RemoveNativeObject(self->labelPtr);
                RemoveNativeObject(self->delButton);
                for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) RemoveNativeObject(self->saveButtons[i]);
                RemoveNativeObject(self);
            }
            break;
        }
        case SAVESELECT_STATE_LOADSAVE: {
            self->menuControl->state = MENUCONTROL_STATE_NONE;
            if (!(self->saveButtons[self->selectedButton]->state & ~SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED)) {
                if (self->selectedButton == SAVESELECT_BUTTON_COUNT) {
                    self->menuControl->state = MENUCONTROL_STATE_SUBMENU;
                    self->state              = SAVESELECT_STATE_MAIN_DELETING;
                    if (usePhysicalControls)
                        self->selectedButton = SAVESELECT_BUTTON_SAVE1;
                    for (int i = SAVESELECT_BUTTON_SAVE1; i < SAVESELECT_BUTTON_COUNT; ++i) {
                        if (self->saveButtons[i]->state == SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED)
                            self->saveButtons[i]->useMeshH = true;
                    }
                }
                else if (self->selectedButton) {
                    int saveSlot = self->selectedButton - 1;
                    if (saveGame->files[saveSlot].stageID) {
                        self->state = SAVESELECT_STATE_SUBMENU;
                        SetGlobalVariableByName("options.saveSlot", saveSlot);
                        SetGlobalVariableByName("options.gameMode", 1);
                        SetGlobalVariableByName("options.stageSelectFlag", 0);
                        SetGlobalVariableByName("player.lives", saveGame->files[saveSlot].lives);
                        SetGlobalVariableByName("player.score", saveGame->files[saveSlot].score);
                        SetGlobalVariableByName("player.scoreBonus", saveGame->files[saveSlot].scoreBonus);
                        SetGlobalVariableByName("specialStage.listPos", saveGame->files[saveSlot].specialStageID);
                        SetGlobalVariableByName("specialStage.emeralds", saveGame->files[saveSlot].emeralds);
                        SetGlobalVariableByName("lampPostID", 0);
                        SetGlobalVariableByName("starPostID", 0);
                        debugMode = false;
                        if (saveGame->files[saveSlot].stageID >= 0x80) {
                            SetGlobalVariableByName("specialStage.nextZone", saveGame->files[saveSlot].stageID - 0x81);
                            InitStartingStage(STAGELIST_SPECIAL, saveGame->files[saveSlot].specialStageID, saveGame->files[saveSlot].characterID);
                        }
                        else {
                            SetGlobalVariableByName("specialStage.nextZone", saveGame->files[saveSlot].stageID - 1);
                            InitStartingStage(STAGELIST_REGULAR, saveGame->files[saveSlot].stageID - 1, saveGame->files[saveSlot].characterID);
                        }
                        CREATE_ENTITY(FadeScreen);
                    }
                    else {
                        self->state                 = SAVESELECT_STATE_ENTERSUBMENU;
                        self->deleteRotateYVelocity = 0.0;
                        self->targetDeleteRotateY   = DegreesToRad(-90.0);
                        for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) self->targetRotateY[i] = DegreesToRad(-90.0);

                        float val = 0.02;
                        for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) {
                            self->rotateYVelocity[i] = val;
                            val += 0.02;
                        }
                    }
                }
                else {
                    self->state                 = SAVESELECT_STATE_ENTERSUBMENU;
                    self->deleteRotateYVelocity = 0.0;
                    self->targetDeleteRotateY   = DegreesToRad(-90.0);
                    for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) self->targetRotateY[i] = DegreesToRad(-90.0);

                    float val = 0.02;
                    for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) {
                        self->rotateYVelocity[i] = val;
                        val += 0.02;
                    }
                }
            }
            break;
        }
        case SAVESELECT_STATE_ENTERSUBMENU: {
            if (self->deleteRotateY > self->targetDeleteRotateY) {
                self->deleteRotateYVelocity -= 0.0025 * (Engine.deltaTime * 60.0);
                self->deleteRotateY += (Engine.deltaTime * 60.0) * self->deleteRotateYVelocity;
                self->deleteRotateYVelocity -= 0.0025 * (Engine.deltaTime * 60.0);
                MatrixRotateYF(&self->labelPtr->renderMatrix, self->deleteRotateY);
                MatrixTranslateXYZF(&self->matrix1, -128.0, 80.0, 160.0);
                MatrixMultiplyF(&self->labelPtr->renderMatrix, &self->matrix1);
            }

            float y = 48.0;
            for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) {
                if (self->rotateY[i] > self->targetRotateY[i]) {
                    self->rotateYVelocity[i] -= 0.0025 * (60.0 * Engine.deltaTime);
                    if (self->rotateYVelocity[i] < 0.0)
                        self->rotateY[i] += ((60.0 * Engine.deltaTime) * self->rotateYVelocity[i]);
                    self->rotateYVelocity[i] -= 0.0025 * (60.0 * Engine.deltaTime);
                    MatrixRotateYF(&self->saveButtons[i]->matrix, self->rotateY[i]);
                    MatrixTranslateXYZF(&self->matrix1, -128.0, y, 160.0);
                    MatrixMultiplyF(&self->saveButtons[i]->matrix, &self->matrix1);
                }
                y -= 30.0;
            }

            if (self->targetRotateY[SAVESELECT_BUTTON_COUNT - 1] >= self->rotateY[SAVESELECT_BUTTON_COUNT - 1]) {
                self->state                 = SAVESELECT_STATE_SUBMENU;
                self->deleteRotateYVelocity = 0.0;
                self->targetDeleteRotateY   = DegreesToRad(22.5);
                for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) self->targetRotateY[i] = DegreesToRad(16.0);

                float val = -0.02;
                for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) {
                    self->rotateYVelocity[i] = val;
                    val -= 0.02;
                }

                self->playerSelect                                               = CREATE_ENTITY(PlayerSelectScreen);
                ((NativeEntity_PlayerSelectScreen *)self->playerSelect)->saveSel = self;
            }

            float div                               = (60.0 * Engine.deltaTime) * 16.0;
            NativeEntity_AchievementsButton *button = self->menuControl->buttons[self->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = self->menuControl->backButton;
            button->x += ((512.0 - button->x) / div);
            backButton->x += ((1024.0 - backButton->x) / div);
            self->delButton->x += ((512.0 - self->delButton->x) / div);
            break;
        }
        case SAVESELECT_STATE_SUBMENU: // player select idle
            break;
        case SAVESELECT_STATE_EXITSUBMENU: {
            if (self->targetDeleteRotateY > self->deleteRotateY) {
                self->deleteRotateYVelocity += 0.0025 * (Engine.deltaTime * 60.0);
                self->deleteRotateY += (Engine.deltaTime * 60.0) * self->deleteRotateYVelocity;
                self->deleteRotateYVelocity += 0.0025 * (Engine.deltaTime * 60.0);
                if (self->deleteRotateY > self->targetDeleteRotateY)
                    self->deleteRotateY = self->targetDeleteRotateY;
                MatrixRotateYF(&self->labelPtr->renderMatrix, self->deleteRotateY);
                MatrixTranslateXYZF(&self->matrix1, -128.0, 80.0, 160.0);
                MatrixMultiplyF(&self->labelPtr->renderMatrix, &self->matrix1);
            }

            float y = 48.0;
            for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) {
                if (self->targetRotateY[i] > self->rotateY[i]) {
                    self->rotateYVelocity[i] += 0.0025 * (60.0 * Engine.deltaTime);
                    if (self->rotateYVelocity[i] > 0.0)
                        self->rotateY[i] += ((60.0 * Engine.deltaTime) * self->rotateYVelocity[i]);

                    self->rotateYVelocity[i] += 0.0025 * (60.0 * Engine.deltaTime);
                    if (self->rotateY[i] > self->targetRotateY[i])
                        self->rotateY[i] = self->targetRotateY[i];
                    MatrixRotateYF(&self->saveButtons[i]->matrix, self->rotateY[i]);
                    MatrixTranslateXYZF(&self->matrix1, -128.0, y, 160.0);
                    MatrixMultiplyF(&self->saveButtons[i]->matrix, &self->matrix1);
                }
                y -= 30.0;
            }

            float div                               = (60.0 * Engine.deltaTime) * 16.0;
            NativeEntity_AchievementsButton *button = self->menuControl->buttons[self->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = self->menuControl->backButton;
            button->x += ((112.0 - button->x) / div);
            backButton->x += ((230.0 - backButton->x) / div);
            if (self->deleteEnabled)
                self->delButton->x += ((92.0 - self->delButton->x) / div);

            if (backButton->x < SCREEN_YSIZE) {
                backButton->x              = SCREEN_YSIZE;
                self->state              = SAVESELECT_STATE_MAIN;
                self->menuControl->state = MENUCONTROL_STATE_SUBMENU;
            }
            break;
        }
        case SAVESELECT_STATE_DELSETUP: {
            self->menuControl->state = MENUCONTROL_STATE_NONE;
            if (self->saveButtons[self->selectedButton]->state == SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED) {
                self->dialog = CREATE_ENTITY(DialogPanel);
                SetStringToFont(self->dialog->text, strDeleteMessage, FONT_TEXT);
                self->state = SAVESELECT_STATE_DIALOGWAIT;
            }
            break;
        }
        case SAVESELECT_STATE_DIALOGWAIT: {
            if (self->dialog->selection == DLG_YES) {
                PlaySfxByName("Event", false);
                for (int i = SAVESELECT_BUTTON_SAVE1; i < SAVESELECT_BUTTON_COUNT; ++i) {
                    if (self->saveButtons[i]->state == SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED)
                        self->saveButtons[i]->useMeshH = false;
                }
                self->state              = SAVESELECT_STATE_MAIN;
                self->menuControl->state = MENUCONTROL_STATE_SUBMENU;
                SetStringToFont(self->saveButtons[self->selectedButton]->text, strNewGame, FONT_LABEL);

                self->saveButtons[self->selectedButton]->state = SUBMENUBUTTON_STATE_IDLE;
                self->saveButtons[self->selectedButton]->textY = -4.0;
                self->saveButtons[self->selectedButton]->scale = 0.1;

                saveGame->files[self->selectedButton - 1].characterID    = 0;
                saveGame->files[self->selectedButton - 1].lives          = 3;
                saveGame->files[self->selectedButton - 1].score          = 0;
                saveGame->files[self->selectedButton - 1].scoreBonus     = 500000;
                saveGame->files[self->selectedButton - 1].stageID        = 0;
                saveGame->files[self->selectedButton - 1].emeralds       = 0;
                saveGame->files[self->selectedButton - 1].specialStageID = 0;
                WriteSaveRAMData();

                self->deleteEnabled = false;
                for (int i = SAVESELECT_BUTTON_SAVE1; i < SAVESELECT_BUTTON_COUNT; ++i) {
                    if (self->saveButtons[i]->state == SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED)
                        self->deleteEnabled = true;
                }
            }
            else if (self->dialog->selection == DLG_NO) {
                for (int i = SAVESELECT_BUTTON_SAVE1; i < SAVESELECT_BUTTON_COUNT; ++i) {
                    if (self->saveButtons[i]->state == SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED)
                        self->saveButtons[i]->useMeshH = false;
                }
                self->state              = SAVESELECT_STATE_MAIN;
                self->menuControl->state = MENUCONTROL_STATE_SUBMENU;
            }
            break;
        }
        default: break;
    }
}
