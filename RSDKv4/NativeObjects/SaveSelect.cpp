#include "RetroEngine.hpp"

void SaveSelect_Create(void *objPtr)
{
    RSDK_THIS(SaveSelect);
    SaveGame *saveGame = (SaveGame *)saveRAM;

    entity->menuControl      = (NativeEntity_MenuControl *)GetNativeObject(0);
    entity->labelPtr         = CREATE_ENTITY(TextLabel);
    entity->labelPtr->fontID = FONT_HEADING;
    if (Engine.language != RETRO_EN)
        entity->labelPtr->scale = 0.12;
    else
        entity->labelPtr->scale = 0.2;
    entity->labelPtr->alpha = 0;
    entity->labelPtr->z     = 0;
    entity->labelPtr->state = TEXTLABEL_STATE_IDLE;
    SetStringToFont(entity->labelPtr->text, strSaveSelect, FONT_HEADING);
    entity->labelPtr->alignOffset = 512.0;

    entity->deleteRotateY = DegreesToRad(22.5);
    matrixRotateYF(&entity->labelPtr->renderMatrix, entity->deleteRotateY);
    matrixTranslateXYZF(&entity->matrix1, -128.0, 80.0, 160.0);
    matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrix1);
    entity->labelPtr->useRenderMatrix = true;

    entity->delButton    = CREATE_ENTITY(PushButton);
    entity->delButton->x = 384.0;
    entity->delButton->y = -16.0;
    if (Engine.language == RETRO_FR)
        entity->delButton->scale = 0.15;
    else
        entity->delButton->scale = 0.2;

    entity->delButton->bgColour = 0x00A048;
    SetStringToFont(entity->delButton->text, strDelete, FONT_LABEL);

    entity->saveButtons[SAVESELECT_BUTTON_NOSAVE] = CREATE_ENTITY(SubMenuButton);
    SetStringToFont(entity->saveButtons[SAVESELECT_BUTTON_NOSAVE]->text, strNoSave, FONT_LABEL);
    entity->saveButtons[SAVESELECT_BUTTON_NOSAVE]->matXOff = 512.0;
    entity->saveButtons[SAVESELECT_BUTTON_NOSAVE]->textY   = -4.0;
    entity->saveButtons[SAVESELECT_BUTTON_NOSAVE]->matZ    = 0.0;
    entity->saveButtons[SAVESELECT_BUTTON_NOSAVE]->scale   = 0.1;

    entity->rotateY[SAVESELECT_BUTTON_NOSAVE] = DegreesToRad(16.0);
    matrixRotateYF(&entity->saveButtons[SAVESELECT_BUTTON_NOSAVE]->matrix, entity->rotateY[0]);
    matrixTranslateXYZF(&entity->matrix1, -128.0, 48.0, 160.0);
    matrixMultiplyF(&entity->saveButtons[SAVESELECT_BUTTON_NOSAVE]->matrix, &entity->matrix1);
    entity->saveButtons[SAVESELECT_BUTTON_NOSAVE]->useMatrix = true;
    ReadSaveRAMData();

    float y = 18.0;
    for (int i = SAVESELECT_BUTTON_SAVE1; i < SAVESELECT_BUTTON_COUNT; ++i) {
        entity->saveButtons[i] = CREATE_ENTITY(SubMenuButton);

        int stagePos = saveGame->files[i - 1].stageID;
        if (stagePos >= 0x80) {
            SetStringToFont(entity->saveButtons[i]->text, strSaveStageList[saveGame->files[i - 1].specialStageID + 19], FONT_LABEL);
            entity->saveButtons[i]->state = SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED;
            entity->saveButtons[i]->textY = 2.0;
            entity->saveButtons[i]->scale = 0.08;
            entity->deleteEnabled         = true;
        }
        else if (stagePos > 0) {
            if (stagePos - 1 > 18 && Engine.gameType == GAME_SONIC1)
                SetStringToFont(entity->saveButtons[i]->text, strSaveStageList[25], FONT_LABEL);
            else
                SetStringToFont(entity->saveButtons[i]->text, strSaveStageList[stagePos - 1], FONT_LABEL);
            entity->saveButtons[i]->state = SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED;
            entity->saveButtons[i]->textY = 2.0;
            entity->saveButtons[i]->scale = 0.08;
            entity->deleteEnabled         = true;
        }
        else {
            SetStringToFont(entity->saveButtons[i]->text, strNewGame, FONT_LABEL);
            entity->saveButtons[i]->textY = -4.0;
            entity->saveButtons[i]->scale = 0.1;
        }

        entity->saveButtons[i]->matXOff = 512.0;
        entity->saveButtons[i]->matZ    = 0.0;
        entity->saveButtons[i]->symbol  = saveGame->files[i - 1].characterID;
        entity->saveButtons[i]->flags   = saveGame->files[i - 1].emeralds;
        entity->rotateY[i]              = DegreesToRad(16.0);
        matrixRotateYF(&entity->saveButtons[i]->matrix, entity->rotateY[i]);
        matrixTranslateXYZF(&entity->matrix1, -128.0, y, 160.0);
        matrixMultiplyF(&entity->saveButtons[i]->matrix, &entity->matrix1);
        entity->saveButtons[i]->useMatrix = true;
        y -= 30.0;
    }
}
void SaveSelect_Main(void *objPtr)
{
    RSDK_THIS(SaveSelect);
    SaveGame *saveGame = (SaveGame *)saveRAM;

    switch (entity->state) {
        case SAVESELECT_STATE_SETUP: {
            entity->timer += Engine.deltaTime;
            if (entity->timer > 1.0) {
                entity->timer = 0.0;
                entity->state = SAVESELECT_STATE_ENTER;
            }
            break;
        }
        case SAVESELECT_STATE_ENTER: {
            entity->labelPtr->alignOffset = entity->labelPtr->alignOffset / (1.125 * (60.0 * Engine.deltaTime));
            if (entity->deleteEnabled)
                entity->delButton->x = ((92.0 - entity->delButton->x) / (8.0 * (60.0 * Engine.deltaTime))) + entity->delButton->x;

            float div = (60.0 * Engine.deltaTime) * 16.0;
            for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) entity->saveButtons[i]->matXOff += ((-176.0 - entity->saveButtons[i]->matXOff) / div);

            entity->timer += (Engine.deltaTime + Engine.deltaTime);
            entity->labelPtr->alpha = (256.0 * entity->timer);
            if (entity->timer > 1.0) {
                entity->timer  = 0.0;
                entity->state  = SAVESELECT_STATE_MAIN;
                keyPress.start = false;
                keyPress.A     = false;
            }
            break;
        }
        case SAVESELECT_STATE_MAIN:
        case SAVESELECT_STATE_MAIN_DELETING: {
            if (entity->state != SAVESELECT_STATE_MAIN_DELETING) {
                if (!entity->deleteEnabled)
                    entity->delButton->x += ((512.0 - entity->delButton->x) / ((60.0 * Engine.deltaTime) * 16.0));
                else
                    entity->delButton->x += ((92.0 - entity->delButton->x) / ((60.0 * Engine.deltaTime) * 16.0));
            }

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.up) {
                        PlaySfxByName("Menu Move", false);
                        entity->selectedButton--;
                        if (entity->deleteEnabled && entity->selectedButton < SAVESELECT_BUTTON_NOSAVE) {
                            entity->selectedButton = SAVESELECT_BUTTON_COUNT;
                        }
                        else if (entity->selectedButton < SAVESELECT_BUTTON_NOSAVE) {
                            entity->selectedButton = SAVESELECT_BUTTON_COUNT - 1;
                        }
                    }
                    else if (keyPress.down) {
                        PlaySfxByName("Menu Move", false);
                        entity->selectedButton++;
                        if (entity->deleteEnabled && entity->selectedButton > SAVESELECT_BUTTON_COUNT) {
                            entity->selectedButton = SAVESELECT_BUTTON_NOSAVE;
                        }
                        else if (entity->selectedButton >= SAVESELECT_BUTTON_COUNT) {
                            entity->selectedButton = SAVESELECT_BUTTON_NOSAVE;
                        }
                    }

                    for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) entity->saveButtons[i]->b = 0xFF;

                    if (entity->deleteEnabled && (keyPress.left || keyPress.right)) {
                        if (entity->selectedButton < SAVESELECT_BUTTON_COUNT) {
                            entity->selectedButton   = SAVESELECT_BUTTON_COUNT;
                            entity->delButton->state = PUSHBUTTON_STATE_SELECTED;
                        }
                        else {
                            entity->selectedButton                         = SAVESELECT_BUTTON_NOSAVE;
                            entity->saveButtons[entity->selectedButton]->b = 0x00;
                            entity->delButton->state                       = PUSHBUTTON_STATE_UNSELECTED;
                        }
                    }
                    else {
                        if (entity->selectedButton >= SAVESELECT_BUTTON_COUNT) {
                            entity->delButton->state = PUSHBUTTON_STATE_SELECTED;
                        }
                        else {
                            entity->saveButtons[entity->selectedButton]->b = 0x00;
                            entity->delButton->state                       = PUSHBUTTON_STATE_UNSELECTED;
                        }
                    }

                    if (keyPress.start || keyPress.A) {
                        if (entity->selectedButton < SAVESELECT_BUTTON_COUNT) {
                            PlaySfxByName("Menu Select", false);
                            entity->saveButtons[entity->selectedButton]->state = SUBMENUBUTTON_STATE_FLASHING2;
                            if (entity->selectedButton > SAVESELECT_BUTTON_NOSAVE && saveGame->files[entity->selectedButton - 1].stageID > 0) {
                                StopMusic(true);
                                entity->saveButtons[entity->selectedButton]->state = SUBMENUBUTTON_STATE_SAVEBUTTON_UNSELECTED;
                            }
                            entity->saveButtons[entity->selectedButton]->b = 0xFF;
                            if (entity->state == SAVESELECT_STATE_MAIN_DELETING)
                                entity->state = SAVESELECT_STATE_DELSETUP;
                            else
                                entity->state = SAVESELECT_STATE_LOADSAVE;
                        }
                        else {
                            if (Engine.gameType == GAME_SONIC1)
                                PlaySfxByName("Lamp Post", false);
                            else
                                PlaySfxByName("Star Post", false);
                            entity->delButton->state = PUSHBUTTON_STATE_FLASHING;
                            if (entity->state == SAVESELECT_STATE_MAIN_DELETING)
                                entity->state = SAVESELECT_STATE_DELSETUP;
                            else
                                entity->state = SAVESELECT_STATE_LOADSAVE;
                        }
                    }
                }
            }
            else {
                float y = 48.0;
                for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) {
                    if (touches > 0) {
                        if (CheckTouchRect(-64.0, y, 96.0, 12.0) < 0)
                            entity->saveButtons[i]->b = 0xFF;
                        else
                            entity->saveButtons[i]->b = 0x00;
                    }
                    else if (!entity->saveButtons[i]->b) {
                        entity->selectedButton = i;
                        PlaySfxByName("Menu Select", false);
                        entity->saveButtons[i]->state = SUBMENUBUTTON_STATE_FLASHING2;
                        if (entity->selectedButton > SAVESELECT_BUTTON_NOSAVE && saveGame->files[entity->selectedButton - 1].stageID > 0) {
                            StopMusic(true);
                            entity->saveButtons[i]->state = SUBMENUBUTTON_STATE_SAVEBUTTON_UNSELECTED;
                        }
                        entity->saveButtons[i]->b = 0xFF;
                        if (entity->state == SAVESELECT_STATE_MAIN_DELETING)
                            entity->state = SAVESELECT_STATE_DELSETUP;
                        else
                            entity->state = SAVESELECT_STATE_LOADSAVE;
                        break;
                    }
                    y -= 30.0;
                }
                if (entity->state == SAVESELECT_STATE_MAIN) {
                    if (!entity->deleteEnabled) {
                        if (keyDown.up || keyDown.down || keyDown.left || keyDown.right) {
                            entity->selectedButton = SAVESELECT_BUTTON_NOSAVE;
                            usePhysicalControls    = true;
                        }
                    }
                    else {
                        if (touches <= 0) {
                            if (entity->delButton->state == PUSHBUTTON_STATE_SELECTED) {
                                entity->selectedButton = SAVESELECT_BUTTON_COUNT;
                                if (Engine.gameType == GAME_SONIC1)
                                    PlaySfxByName("Lamp Post", false);
                                else
                                    PlaySfxByName("Star Post", false);
                                entity->delButton->state = PUSHBUTTON_STATE_FLASHING;
                                if (entity->state == SAVESELECT_STATE_MAIN_DELETING)
                                    entity->state = SAVESELECT_STATE_DELSETUP;
                                else
                                    entity->state = SAVESELECT_STATE_LOADSAVE;
                            }
                            else {
                                if (keyDown.up || keyDown.down || keyDown.left || keyDown.right) {
                                    entity->selectedButton = SAVESELECT_BUTTON_NOSAVE;
                                    usePhysicalControls    = true;
                                }
                            }
                        }
                        else {
                            entity->delButton->state = CheckTouchRect(entity->delButton->x, entity->delButton->y,
                                                                      (64.0 * entity->delButton->scale) + entity->delButton->textWidth, 12.0)
                                                       >= 0;
                            if (entity->state == SAVESELECT_STATE_MAIN) {
                                if (keyDown.up || keyDown.down || keyDown.left || keyDown.right) {
                                    entity->selectedButton = SAVESELECT_BUTTON_NOSAVE;
                                    usePhysicalControls    = true;
                                }
                            }
                        }
                    }
                }
                else if (entity->state == SAVESELECT_STATE_MAIN_DELETING) {
                    if (touches > 0) {
                        if (CheckTouchRect(entity->delButton->x, entity->delButton->y,
                                           (64.0 * entity->delButton->scale) + entity->delButton->textWidth, 12.0)
                            >= 0) {
                            entity->delButton->state = PUSHBUTTON_STATE_SELECTED;
                        }
                        else {
                            entity->delButton->state = PUSHBUTTON_STATE_UNSELECTED;
                        }
                    }
                    else if (entity->delButton->state == PUSHBUTTON_STATE_SELECTED) {
                        entity->state = SAVESELECT_STATE_MAIN;
                        PlaySfxByName("Menu Back", false);
                        for (int i = SAVESELECT_BUTTON_SAVE1; i < SAVESELECT_BUTTON_COUNT; ++i) {
                            if (entity->saveButtons[i]->state == SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED)
                                entity->saveButtons[i]->useMeshH = false;
                        }
                        entity->delButton->state = PUSHBUTTON_STATE_UNSELECTED;
                    }
                }
            }

            if (entity->menuControl->state == MENUCONTROL_STATE_EXITSUBMENU)
                entity->state = SAVESELECT_STATE_EXIT;
            break;
        }
        case SAVESELECT_STATE_EXIT: {
            entity->labelPtr->alignOffset += 10.0 * (60.0 * Engine.deltaTime);
            entity->delButton->x += 10.0 * (60.0 * Engine.deltaTime);
            for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) entity->saveButtons[i]->matXOff += (11.0 * (60.0 * Engine.deltaTime));
            entity->timer += (Engine.deltaTime + Engine.deltaTime);
            if (entity->timer > 1.0) {
                entity->timer = 0.0;
                RemoveNativeObject(entity->labelPtr);
                RemoveNativeObject(entity->delButton);
                for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) RemoveNativeObject(entity->saveButtons[i]);
                RemoveNativeObject(entity);
            }
            break;
        }
        case SAVESELECT_STATE_LOADSAVE: {
            entity->menuControl->state = MENUCONTROL_STATE_NONE;
            if (!(entity->saveButtons[entity->selectedButton]->state & ~SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED)) {
                if (entity->selectedButton == SAVESELECT_BUTTON_COUNT) {
                    entity->menuControl->state = MENUCONTROL_STATE_SUBMENU;
                    entity->state              = SAVESELECT_STATE_MAIN_DELETING;
                    if (usePhysicalControls)
                        entity->selectedButton = SAVESELECT_BUTTON_SAVE1;
                    for (int i = SAVESELECT_BUTTON_SAVE1; i < SAVESELECT_BUTTON_COUNT; ++i) {
                        if (entity->saveButtons[i]->state == SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED)
                            entity->saveButtons[i]->useMeshH = true;
                    }
                }
                else if (entity->selectedButton) {
                    int saveSlot = entity->selectedButton - 1;
                    if (saveGame->files[saveSlot].stageID) {
                        entity->state = SAVESELECT_STATE_SUBMENU;
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
                        entity->state    = SAVESELECT_STATE_ENTERSUBMENU;
                        entity->field_30 = 0.0;
                        entity->field_2C = DegreesToRad(-90.0);
                        for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) entity->field_AC[i] = DegreesToRad(-90.0);

                        float val = 0.02;
                        for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) {
                            entity->field_C0[i] = val;
                            val += 0.02;
                        }
                        SetGlobalVariableByName("options.stageSelectFlag", 0);
                        debugMode = false;
                    }
                }
                else {
                    entity->state    = SAVESELECT_STATE_ENTERSUBMENU;
                    entity->field_30 = 0.0;
                    entity->field_2C = DegreesToRad(-90.0);
                    for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) entity->field_AC[i] = DegreesToRad(-90.0);

                    float val = 0.02;
                    for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) {
                        entity->field_C0[i] = val;
                        val += 0.02;
                    }
                }
            }
            break;
        }
        case SAVESELECT_STATE_ENTERSUBMENU: {
            if (entity->deleteRotateY > entity->field_2C) {
                entity->field_30 -= 0.0025 * (Engine.deltaTime * 60.0);
                entity->deleteRotateY += (Engine.deltaTime * 60.0) * entity->field_30;
                entity->field_30 -= 0.0025 * (Engine.deltaTime * 60.0);
                matrixRotateYF(&entity->labelPtr->renderMatrix, entity->deleteRotateY);
                matrixTranslateXYZF(&entity->matrix1, -128.0, 80.0, 160.0);
                matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrix1);
            }

            float y = 48.0;
            for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) {
                if (entity->rotateY[i] > entity->field_AC[i]) {
                    entity->field_C0[i] -= 0.0025 * (60.0 * Engine.deltaTime);
                    if (entity->field_C0[i] < 0.0)
                        entity->rotateY[i] += ((60.0 * Engine.deltaTime) * entity->field_C0[i]);
                    entity->field_C0[i] -= 0.0025 * (60.0 * Engine.deltaTime);
                    matrixRotateYF(&entity->saveButtons[i]->matrix, entity->rotateY[i]);
                    matrixTranslateXYZF(&entity->matrix1, -128.0, y, 160.0);
                    matrixMultiplyF(&entity->saveButtons[i]->matrix, &entity->matrix1);
                }
                y -= 30.0;
            }

            if (entity->field_AC[SAVESELECT_BUTTON_COUNT - 1] >= entity->rotateY[SAVESELECT_BUTTON_COUNT - 1]) {
                entity->state    = SAVESELECT_STATE_SUBMENU;
                entity->field_30 = 0.0;
                entity->field_2C = DegreesToRad(22.5);
                for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) entity->field_AC[i] = DegreesToRad(16.0);

                float val = -0.02;
                for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) {
                    entity->field_C0[i] = val;
                    val -= 0.02;
                }

                entity->playerSelect                                               = CREATE_ENTITY(PlayerSelectScreen);
                ((NativeEntity_PlayerSelectScreen *)entity->playerSelect)->saveSel = entity;
            }

            float div                               = (60.0 * Engine.deltaTime) * 16.0;
            NativeEntity_AchievementsButton *button = entity->menuControl->buttons[entity->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = entity->menuControl->backButton;
            button->x += ((512.0 - button->x) / div);
            backButton->x += ((1024.0 - backButton->x) / div);
            entity->delButton->x += ((512.0 - entity->delButton->x) / div);
            break;
        }
        case SAVESELECT_STATE_SUBMENU: // player select idle
            break;
        case SAVESELECT_STATE_EXITSUBMENU: {
            if (entity->field_2C > entity->deleteRotateY) {
                entity->field_30 += 0.0025 * (Engine.deltaTime * 60.0);
                entity->deleteRotateY += (Engine.deltaTime * 60.0) * entity->field_30;
                entity->field_30 += 0.0025 * (Engine.deltaTime * 60.0);
                if (entity->deleteRotateY > entity->field_2C)
                    entity->deleteRotateY = entity->field_2C;
                matrixRotateYF(&entity->labelPtr->renderMatrix, entity->deleteRotateY);
                matrixTranslateXYZF(&entity->matrix1, -128.0, 80.0, 160.0);
                matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrix1);
            }

            float y = 48.0;
            for (int i = 0; i < SAVESELECT_BUTTON_COUNT; ++i) {
                if (entity->field_AC[i] > entity->rotateY[i]) {
                    entity->field_C0[i] += 0.0025 * (60.0 * Engine.deltaTime);
                    if (entity->field_C0[i] > 0.0)
                        entity->rotateY[i] += ((60.0 * Engine.deltaTime) * entity->field_C0[i]);
                    entity->field_C0[i] += 0.0025 * (60.0 * Engine.deltaTime);
                    if (entity->rotateY[i] > entity->field_AC[i])
                        entity->rotateY[i] = entity->field_AC[i];
                    matrixRotateYF(&entity->saveButtons[i]->matrix, entity->rotateY[i]);
                    matrixTranslateXYZF(&entity->matrix1, -128.0, y, 160.0);
                    matrixMultiplyF(&entity->saveButtons[i]->matrix, &entity->matrix1);
                }
                y -= 30.0;
            }

            float div                               = (60.0 * Engine.deltaTime) * 16.0;
            NativeEntity_AchievementsButton *button = entity->menuControl->buttons[entity->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = entity->menuControl->backButton;
            button->x += ((112.0 - button->x) / div);
            backButton->x += ((230.0 - backButton->x) / div);
            if (entity->deleteEnabled)
                entity->delButton->x += ((92.0 - entity->delButton->x) / div);

            if (backButton->x < SCREEN_YSIZE) {
                backButton->x              = SCREEN_YSIZE;
                entity->state              = SAVESELECT_STATE_MAIN;
                entity->menuControl->state = MENUCONTROL_STATE_SUBMENU;
            }
            break;
        }
        case SAVESELECT_STATE_DELSETUP: {
            entity->menuControl->state = MENUCONTROL_STATE_NONE;
            if (entity->saveButtons[entity->selectedButton]->state == SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED) {
                entity->dialog = CREATE_ENTITY(DialogPanel);
                SetStringToFont(entity->dialog->text, strDeleteMessage, FONT_TEXT);
                entity->state = SAVESELECT_STATE_DIALOGWAIT;
            }
            break;
        }
        case SAVESELECT_STATE_DIALOGWAIT: {
            if (entity->dialog->selection == DLG_YES) {
                PlaySfxByName("Event", false);
                entity->saveButtons[entity->selectedButton]->useMeshH = false;
                entity->state                                         = SAVESELECT_STATE_MAIN;
                entity->menuControl->state                            = MENUCONTROL_STATE_SUBMENU;
                SetStringToFont(entity->saveButtons[entity->selectedButton]->text, strNewGame, FONT_LABEL);

                entity->saveButtons[entity->selectedButton]->state = SUBMENUBUTTON_STATE_IDLE;
                entity->saveButtons[entity->selectedButton]->textY = -4.0;
                entity->saveButtons[entity->selectedButton]->scale = 0.1;

                saveGame->files[entity->selectedButton - 1].characterID    = 0;
                saveGame->files[entity->selectedButton - 1].lives          = 3;
                saveGame->files[entity->selectedButton - 1].score          = 0;
                saveGame->files[entity->selectedButton - 1].scoreBonus     = 500000;
                saveGame->files[entity->selectedButton - 1].stageID        = 0;
                saveGame->files[entity->selectedButton - 1].emeralds       = 0;
                saveGame->files[entity->selectedButton - 1].specialStageID = 0;
                WriteSaveRAMData();

                entity->deleteEnabled = false;
                for (int i = SAVESELECT_BUTTON_SAVE1; i < SAVESELECT_BUTTON_COUNT; ++i) {
                    if (entity->saveButtons[i]->state == SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED)
                        entity->deleteEnabled = true;
                }
            }
            else if (entity->dialog->selection == DLG_NO) {
                entity->saveButtons[entity->selectedButton]->useMeshH = false;
                entity->state                                         = SAVESELECT_STATE_MAIN;
                entity->menuControl->state                            = MENUCONTROL_STATE_SUBMENU;
            }
            break;
        }
        default: break;
    }
}
