#include "RetroEngine.hpp"

void SaveSelect_Create(void *objPtr)
{
    RSDK_THIS(SaveSelect);
    SaveGame *saveGame = (SaveGame *)saveRAM;

    entity->menuControl      = (NativeEntity_MenuControl *)GetNativeObject(0);
    entity->labelPtr         = CREATE_ENTITY(TextLabel);
    entity->labelPtr->fontID = 0;
    if (Engine.language != RETRO_EN)
        entity->labelPtr->textScale = 0.12;
    else
        entity->labelPtr->textScale = 0.2;
    entity->labelPtr->textAlpha = 0;
    entity->labelPtr->textZ     = 0;
    entity->labelPtr->alignment = 0;
    SetStringToFont(entity->labelPtr->text, strSaveSelect, 0);
    entity->labelPtr->textWidth = 512.0;

    entity->deleteRotateY = DegreesToRad(22.5);
    matrixRotateYF(&entity->labelPtr->renderMatrix, entity->deleteRotateY);
    matrixTranslateXYZF(&entity->matrix1, -128.0, 80.0, 160.0);
    matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrix1);
    entity->labelPtr->useMatrix = true;

    entity->delButton    = CREATE_ENTITY(PushButton);
    entity->delButton->x = 384.0;
    entity->delButton->y = -16.0;
    if (Engine.language == RETRO_FR)
        entity->delButton->scale = 0.15;
    else
        entity->delButton->scale = 0.2;

    entity->delButton->blue = 0x00A048;
    SetStringToFont(entity->delButton->text, strDelete, 1);

    entity->saveButtons[0] = CREATE_ENTITY(SubMenuButton);
    SetStringToFont(entity->saveButtons[0]->text, strNoSave, 1);
    entity->saveButtons[0]->matXOff   = 512.0;
    entity->saveButtons[0]->textY     = -4.0;
    entity->saveButtons[0]->matZ      = 0.0;
    entity->saveButtons[0]->textScale = 0.1;

    entity->rotateY[0] = DegreesToRad(16.0);
    matrixRotateYF(&entity->saveButtons[0]->matrix, entity->rotateY[0]);
    matrixTranslateXYZF(&entity->matrix1, -128.0, 48.0, 160.0);
    matrixMultiplyF(&entity->saveButtons[0]->matrix, &entity->matrix1);
    entity->saveButtons[0]->useMatrix = true;
    ReadSaveRAMData();

    float y = 18.0;
    for (int i = 1; i <= 4; ++i) {
        entity->saveButtons[i] = CREATE_ENTITY(SubMenuButton);

        int stagePos = saveGame->files[i - 1].zoneID;
        if (stagePos >= 0x80) {
            SetStringToFont(entity->saveButtons[i]->text, strSaveStageList[saveGame->files[i - 1].specialZoneID + 19], 1);
            entity->saveButtons[i]->state     = 4;
            entity->saveButtons[i]->textY     = 2.0;
            entity->saveButtons[i]->textScale = 0.08;
            entity->deleteEnabled             = true;
        }
        else if (stagePos > 0) {
            if (stagePos - 1 > 18 && Engine.gameType == GAME_SONIC1)
                SetStringToFont(entity->saveButtons[i]->text, strSaveStageList[25], 1);
            else
                SetStringToFont(entity->saveButtons[i]->text, strSaveStageList[stagePos - 1], 1);
            entity->saveButtons[i]->state     = 4;
            entity->saveButtons[i]->textY     = 2.0;
            entity->saveButtons[i]->textScale = 0.08;
            entity->deleteEnabled             = true;
        }
        else {
            SetStringToFont(entity->saveButtons[i]->text, strNewGame, 1);
            entity->saveButtons[i]->textY     = -4.0;
            entity->saveButtons[i]->textScale = 0.1;
        }

        entity->saveButtons[i]->matXOff = 512.0;
        entity->saveButtons[i]->matZ    = 0.0;
        entity->saveButtons[i]->symbol  = saveGame->files[i - 1].characterID;
        entity->saveButtons[i]->flags   = saveGame->files[i - 1].emeralds;
        entity->rotateY[i]           = DegreesToRad(16.0);
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
        case 0: {
            entity->field_14 += Engine.deltaTime;
            if (entity->field_14 > 1.0) {
                entity->field_14 = 0.0;
                entity->state    = 1;
            }
            break;
        }
        case 1: {
            entity->labelPtr->textWidth = entity->labelPtr->textWidth / (1.125 * (60.0 * Engine.deltaTime));
            if (entity->deleteEnabled)
                entity->delButton->x = ((92.0 - entity->delButton->x) / (8.0 * (60.0 * Engine.deltaTime))) + entity->delButton->x;

            float div                       = (60.0 * Engine.deltaTime) * 16.0;
            entity->saveButtons[0]->matXOff = ((-176.0 - entity->saveButtons[0]->matXOff) / div) + entity->saveButtons[0]->matXOff;
            entity->saveButtons[1]->matXOff = ((-176.0 - entity->saveButtons[1]->matXOff) / div) + entity->saveButtons[1]->matXOff;
            entity->saveButtons[2]->matXOff = ((-176.0 - entity->saveButtons[2]->matXOff) / div) + entity->saveButtons[2]->matXOff;
            entity->saveButtons[3]->matXOff = ((-176.0 - entity->saveButtons[3]->matXOff) / div) + entity->saveButtons[3]->matXOff;
            entity->saveButtons[4]->matXOff = ((-176.0 - entity->saveButtons[4]->matXOff) / div) + entity->saveButtons[4]->matXOff;

            entity->field_14 += (Engine.deltaTime + Engine.deltaTime);
            entity->labelPtr->textAlpha = (256.0 * entity->field_14);
            if (entity->field_14 > 1.0) {
                entity->field_14 = 0.0;
                entity->state    = 2;
                keyPress.start   = false;
                keyPress.A       = false;
            }
            break;
        }
        case 2: {
            if (!entity->deleteEnabled)
                entity->delButton->x += ((512.0 - entity->delButton->x) / ((60.0 * Engine.deltaTime) * 16.0));
            else
                entity->delButton->x += ((92.0 - entity->delButton->x) / ((60.0 * Engine.deltaTime) * 16.0));

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.up) {
                        PlaySfx(21, 0);
                        entity->selectedSave--;
                        if (entity->deleteEnabled) {
                            if (entity->selectedSave < 0)
                                entity->selectedSave = 5;
                        }
                        else if (entity->selectedSave < 0) {
                            entity->selectedSave = 4;
                        }
                    }
                    else if (keyPress.down) {
                        PlaySfx(21, 0);
                        entity->selectedSave++;
                        if (entity->deleteEnabled) {
                            if (entity->selectedSave > 5) {
                                entity->selectedSave = 0;
                            }
                        }
                        else if (entity->selectedSave > 4) {
                            entity->selectedSave = 0;
                        }
                    }
                    entity->saveButtons[0]->b = 0xFF;
                    entity->saveButtons[1]->b = 0xFF;
                    entity->saveButtons[2]->b = 0xFF;
                    entity->saveButtons[3]->b = 0xFF;
                    entity->saveButtons[4]->b = 0xFF;
                    if (entity->deleteEnabled && (keyPress.left || keyPress.right)) {
                        if (entity->selectedSave <= 4) {
                            entity->selectedSave     = 5;
                            entity->delButton->state = 1;
                        }
                        else {
                            entity->selectedSave                         = 0;
                            entity->saveButtons[entity->selectedSave]->b = 0;
                            entity->delButton->state                     = 0;
                        }
                    }
                    else {
                        if (entity->selectedSave > 4) {
                            entity->delButton->state = 1;
                        }
                        else {
                            entity->saveButtons[entity->selectedSave]->b = 0;
                            entity->delButton->state                     = 0;
                        }
                    }

                    if (keyPress.start || keyPress.A) {
                        if (entity->selectedSave <= 4) {
                            PlaySfx(22, 0);
                            entity->saveButtons[entity->selectedSave]->state = 2;
                            if (entity->selectedSave > 0 && saveGame->files[entity->selectedSave - 1].zoneID > 0) {
                                StopMusic(true);
                                entity->saveButtons[entity->selectedSave]->state = 3;
                            }
                            entity->saveButtons[entity->selectedSave]->b = 0xFF;
                            entity->state                                = 4;
                        }
                        else {
                            PlaySfx(9, 0);
                            entity->delButton->state = 2;
                            entity->state            = 4;
                        }
                    }
                }
            }
            else {
                float y = 48.0;
                for (int i = 0; i <= 4; ++i) {
                    if (touches > 0) {
                        if (CheckTouchRect(-64.0, y, 96.0, 12.0) < 0)
                            entity->saveButtons[i]->b = 0xFF;
                        else
                            entity->saveButtons[i]->b = 0x00;
                    }
                    else if (!entity->saveButtons[i]->b) {
                        entity->selectedSave = i;
                        PlaySfx(22, 0);
                        entity->saveButtons[i]->state = 2;
                        if (entity->selectedSave > 0 && saveGame->files[entity->selectedSave - 1].zoneID > 0) {
                            StopMusic(true);
                            entity->saveButtons[i]->state = 3;
                        }
                        entity->saveButtons[i]->b = 0xFF;
                        entity->state             = 4;
                        break;
                    }
                    y -= 30.0;
                }
                if (entity->state == 2) {
                    if (!entity->deleteEnabled) {
                        if (keyDown.up || keyDown.down || keyDown.left || keyDown.right) {
                            entity->selectedSave = 0;
                            usePhysicalControls  = true;
                        }
                    }
                    else {
                        if (touches <= 0) {
                            if (entity->delButton->state == 1) {
                                entity->selectedSave = 5;
                                PlaySfx(9, 0);
                                entity->delButton->state = 2;
                                entity->state            = 4;
                            }
                            else {
                                if (keyDown.up || keyDown.down || keyDown.left || keyDown.right) {
                                    entity->selectedSave = 0;
                                    usePhysicalControls  = true;
                                }
                            }
                        }
                        else {
                            entity->delButton->state = CheckTouchRect(entity->delButton->x, entity->delButton->y,
                                                                      (64.0 * entity->delButton->scale) + entity->delButton->textWidth, 12.0)
                                                       >= 0;
                            if (entity->state == 2) {
                                if (keyDown.up || keyDown.down || keyDown.left || keyDown.right) {
                                    entity->selectedSave = 0;
                                    usePhysicalControls  = true;
                                }
                            }
                        }
                    }
                }
            }

            if (entity->menuControl->state == 5)
                entity->state = 3;
            break;
        }
        case 3: {
            entity->labelPtr->textWidth += 10.0 * (60.0 * Engine.deltaTime);
            entity->delButton->x += 10.0 * (60.0 * Engine.deltaTime);
            for (int i = 0; i <= 4; ++i) {
                entity->saveButtons[i]->matXOff += (11.0 * (60.0 * Engine.deltaTime));
            }
            entity->field_14 += (Engine.deltaTime + Engine.deltaTime);
            if (entity->field_14 > 1.0) {
                entity->field_14 = 0.0;
                RemoveNativeObject(entity->labelPtr);
                RemoveNativeObject(entity->delButton);
                RemoveNativeObject(entity->saveButtons[0]);
                RemoveNativeObject(entity->saveButtons[1]);
                RemoveNativeObject(entity->saveButtons[2]);
                RemoveNativeObject(entity->saveButtons[3]);
                RemoveNativeObject(entity->saveButtons[4]);
                RemoveNativeObject(entity);
            }
            break;
        }
        case 4: {
            entity->menuControl->state = 2;
            if (!(entity->saveButtons[entity->selectedSave]->state & -5)) {
                if (entity->selectedSave == 5) {
                    entity->menuControl->state = 4;
                    entity->state              = 8;
                    if (usePhysicalControls)
                        entity->selectedSave = 1;
                    for (int i = 1; i <= 4; ++i) {
                        if (entity->saveButtons[i]->state == 4)
                            entity->saveButtons[i]->useMeshH = true;
                    }
                }
                else if (entity->selectedSave) {
                    int saveSlot = entity->selectedSave - 1;
                    if (saveGame->files[saveSlot].zoneID) {
                        entity->state = 6;
                        SetGlobalVariableByName("options.saveSlot", saveSlot);
                        SetGlobalVariableByName("options.gameMode", 1);
                        SetGlobalVariableByName("options.stageSelectFlag", 0);
                        SetGlobalVariableByName("player.lives", saveGame->files[saveSlot].lives);
                        SetGlobalVariableByName("player.score", saveGame->files[saveSlot].score);
                        SetGlobalVariableByName("player.scoreBonus", saveGame->files[saveSlot].scoreBonus);
                        SetGlobalVariableByName("specialStage.listPos", saveGame->files[saveSlot].specialZoneID);
                        SetGlobalVariableByName("specialStage.emeralds", saveGame->files[saveSlot].emeralds);
                        SetGlobalVariableByName("lampPostID", 0);
                        SetGlobalVariableByName("starPostID", 0);
                        debugMode = false;
                        if (saveGame->files[saveSlot].zoneID >= 0x80) {
                            SetGlobalVariableByName("specialStage.nextZone", saveGame->files[saveSlot].zoneID - 129);
                            InitStartingStage(3, saveGame->files[saveSlot].specialZoneID, saveGame->files[saveSlot].characterID);
                        }
                        else {
                            SetGlobalVariableByName("specialStage.nextZone", saveGame->files[saveSlot].zoneID - 1);
                            InitStartingStage(1, saveGame->files[saveSlot].zoneID - 1, saveGame->files[saveSlot].characterID);
                        }
                        CREATE_ENTITY(FadeScreen);
                    }
                    else {
                        entity->state    = 5;
                        entity->field_30 = 0.0;
                        entity->field_2C = DegreesToRad(-90.0);
                        for (int i = 0; i <= 4; ++i) entity->field_AC[i] = DegreesToRad(-90.0);

                        entity->field_C0[0] = 0.02;
                        entity->field_C0[1] = 0.04;
                        entity->field_C0[2] = 0.06;
                        entity->field_C0[3] = 0.08;
                        entity->field_C0[4] = 0.1;
                        SetGlobalVariableByName("options.stageSelectFlag", 0);
                        debugMode = false;
                    }
                }
                else {
                    entity->state    = 5;
                    entity->field_30 = 0.0;
                    entity->field_2C = DegreesToRad(-90.0);
                    for (int i = 0; i <= 4; ++i) entity->field_AC[i] = DegreesToRad(-90.0);
                    entity->field_C0[0] = 0.02;
                    entity->field_C0[1] = 0.04;
                    entity->field_C0[2] = 0.06;
                    entity->field_C0[3] = 0.08;
                    entity->field_C0[4] = 0.1;
                }
            }
            break;
        }
        case 5: {
            if (entity->deleteRotateY > entity->field_2C) {
                entity->field_30 -= 0.0025 * (Engine.deltaTime * 60.0);
                entity->deleteRotateY += (Engine.deltaTime * 60.0) * entity->field_30;
                entity->field_30 -= 0.0025 * (Engine.deltaTime * 60.0);
                matrixRotateYF(&entity->labelPtr->renderMatrix, entity->deleteRotateY);
                matrixTranslateXYZF(&entity->matrix1, -128.0, 80.0, 160.0);
                matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrix1);
            }

            float y = 48.0;
            for (int i = 0; i <= 4; ++i) {
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

            if (entity->field_AC[4] >= entity->rotateY[4]) {
                entity->state    = 6;
                entity->field_30 = 0.0;
                entity->field_2C = DegreesToRad(22.5);
                for (int i = 0; i <= 4; ++i) entity->field_AC[i] = DegreesToRad(16.0);
                entity->field_C0[0] = -0.02;
                entity->field_C0[1] = -0.04;
                entity->field_C0[2] = -0.06;
                entity->field_C0[3] = -0.08;
                entity->field_C0[4] = -0.1;

                entity->playerSelect                                               = CREATE_ENTITY(PlayerSelectScreen);
                ((NativeEntity_PlayerSelectScreen *)entity->playerSelect)->saveSel = entity;
            }

            float div                               = (60.0 * Engine.deltaTime) * 16.0;
            NativeEntity_AchievementsButton *button = entity->menuControl->buttons[entity->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = entity->menuControl->backButton;
            button->translateX += ((512.0 - button->translateX) / div);
            backButton->translateX += ((1024.0 - backButton->translateX) / div);
            entity->delButton->x += ((512.0 - entity->delButton->x) / div);
            break;
        }
        case 6: //player select idle
            break;
        case 7: {
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
            for (int i = 0; i <= 4; ++i) {
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
            button->translateX += ((112.0 - button->translateX) / div);
            backButton->translateX += ((230.0 - backButton->translateX) / div);
            if (entity->deleteEnabled)
                entity->delButton->x += ((92.0 - entity->delButton->x) / div);

            if (backButton->translateX < SCREEN_YSIZE) {
                backButton->translateX     = SCREEN_YSIZE;
                entity->state              = 2;
                entity->menuControl->state = 4;
            }
            break;
        }
        case 8: {
            if (!usePhysicalControls) {
                float y = 48.0;
                for (int i = 0; i <= 4; ++i) {

                    if (CheckTouchRect(-64.0, y, 96.0, 12.0) >= 0)
                        entity->saveButtons[i]->b = 0xFF;
                    else
                        entity->saveButtons[i]->b = 0;

                    if (!entity->saveButtons[i]->b) {
                        entity->saveButtons[i]->b = 0xFF;
                        if (entity->saveButtons[i]->state == 4) {
                            entity->selectedSave = i;
                            PlaySfx(9, 0);
                            entity->state                                       = 9;
                            entity->saveButtons[1]->useMeshH                    = false;
                            entity->saveButtons[2]->useMeshH                    = false;
                            entity->saveButtons[3]->useMeshH                    = false;
                            entity->saveButtons[4]->useMeshH                    = false;
                            entity->saveButtons[entity->selectedSave]->useMeshH = true;
                            entity->saveButtons[entity->selectedSave]->state    = 3;
                        }
                        else {
                            entity->state = 2;
                            PlaySfx(23, 0);
                            entity->saveButtons[1]->useMeshH = false;
                            entity->saveButtons[2]->useMeshH = false;
                            entity->saveButtons[3]->useMeshH = false;
                            entity->saveButtons[4]->useMeshH = false;
                        }
                        break;
                    }
                    y -= 30.0;
                }

                if (entity->state == 8) {
                    if (touches > 0) {
                        if (CheckTouchRect(entity->delButton->x, entity->delButton->y,
                                           (64.0 * entity->delButton->scale) + entity->delButton->textWidth, 12.0)
                            >= 0) {
                            entity->delButton->state = 1;
                        }
                        else {
                            entity->delButton->state = 0;
                        }
                    }
                    else if (entity->delButton->state == 1) {
                        entity->state = 2;
                        PlaySfx(23, 0);
                        for (int i = 1; i <= 4; ++i) {
                            if (entity->saveButtons[i]->state == 4)
                                entity->saveButtons[i]->useMeshH = false;
                        }
                        entity->delButton->state = 0;
                    }
                }
            }
            else {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.up) {
                        PlaySfx(21, 0);
                        entity->selectedSave--;
                        if (entity->deleteEnabled) {
                            if (entity->selectedSave < 0)
                                entity->selectedSave = 5;
                        }
                        else if (entity->selectedSave < 0) {
                            entity->selectedSave = 4;
                        }
                    }
                    else if (keyPress.down) {
                        PlaySfx(21, 0);
                        entity->selectedSave++;
                        if (entity->deleteEnabled) {
                            if (entity->selectedSave > 5)
                                entity->selectedSave = 0;
                        }
                        else if (entity->selectedSave > 4) {
                            entity->selectedSave = 0;
                        }

                        entity->saveButtons[0]->b = 0xFF;
                        entity->saveButtons[1]->b = 0xFF;
                        entity->saveButtons[2]->b = 0xFF;
                        entity->saveButtons[3]->b = 0xFF;
                        entity->saveButtons[4]->b = 0xFF;
                        if (entity->deleteEnabled && (keyPress.left || keyPress.right)) {
                            if (entity->selectedSave <= 4) {
                                entity->selectedSave     = 5;
                                entity->delButton->state = 1;
                            }
                            else {
                                entity->selectedSave = 0;
                            }
                        }
                        else if (entity->selectedSave > 4) {
                            entity->delButton->state = 1;
                        }
                        else {
                            entity->saveButtons[entity->selectedSave]->b = 0;
                            entity->delButton->state                     = 0;
                        }
                    }

                    if (keyPress.start && keyPress.A) {
                        if (entity->selectedSave <= 4) {
                            entity->saveButtons[entity->selectedSave]->b = -1;
                            if (entity->saveButtons[entity->selectedSave]->state == 4) {
                                PlaySfx(9, 0);
                                entity->saveButtons[entity->selectedSave]->state = 3;
                                entity->state                                    = 9;
                                for (int i = 1; i <= 4; ++i) entity->saveButtons[i]->useMeshH = false;
                                entity->saveButtons[entity->selectedSave]->useMeshH = 1;
                            }
                            else {
                                entity->state = 2;
                                PlaySfx(23, 0);
                                entity->saveButtons[1]->useMeshH = 0;
                                entity->saveButtons[2]->useMeshH = 0;
                                entity->saveButtons[3]->useMeshH = 0;
                                entity->saveButtons[4]->useMeshH = 0;
                            }
                        }
                        else {
                            entity->state = 2;
                            PlaySfx(23, 0);
                            for (int i = 1; i <= 4; ++i) {
                                if (entity->saveButtons[i]->state == 4)
                                    entity->saveButtons[i]->useMeshH = false;
                            }
                            entity->delButton->state = 0;
                        }
                    }
                }
            }
            if (entity->menuControl->state == 5)
                entity->state = 3;
            break;
        }
        case 9: {
            entity->menuControl->state = 2;
            if (entity->saveButtons[entity->selectedSave]->state == 4) {
                entity->dialog = CREATE_ENTITY(DialogPanel);
                SetStringToFont(entity->dialog->text, strDeleteMessage, 2);
                entity->state = 10;
            }
            break;
        }
        case 10: {
            if (entity->dialog->selection == 1) {
                PlaySfx(39, 0);
                entity->saveButtons[entity->selectedSave]->useMeshH = false;
                entity->state                                       = 2;
                entity->menuControl->state                          = 4;
                SetStringToFont(entity->saveButtons[entity->selectedSave]->text, strNewGame, 1);

                entity->saveButtons[entity->selectedSave]->state     = 0;
                entity->saveButtons[entity->selectedSave]->textY     = -4.0;
                entity->saveButtons[entity->selectedSave]->textScale = 0.1;

                saveGame->files[entity->selectedSave - 1].characterID   = 0;
                saveGame->files[entity->selectedSave - 1].lives         = 3;
                saveGame->files[entity->selectedSave - 1].score         = 0;
                saveGame->files[entity->selectedSave - 1].scoreBonus    = 500000;
                saveGame->files[entity->selectedSave - 1].zoneID        = 0;
                saveGame->files[entity->selectedSave - 1].emeralds      = 0;
                saveGame->files[entity->selectedSave - 1].specialZoneID = 0;
                WriteSaveRAMData();

                entity->deleteEnabled = 0;
                for (int i = 1; i <= 4; ++i) {
                    if (entity->saveButtons[i]->state == 4)
                        entity->deleteEnabled = true;
                }
            }
            else if (entity->dialog->selection == 2) {
                entity->saveButtons[entity->selectedSave]->useMeshH = 0;
                entity->state                                       = 2;
                entity->menuControl->state                          = 4;
            }
            break;
        }
        default: break;
    }
}
