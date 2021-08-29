#include "RetroEngine.hpp"

float timeAttackU[] = {
    1.0, 321.0, 641.0, 1.0, 321.0, 641.0,
};
float timeAttackV[] = { 1.0, 1.0, 1.0, 241.0, 241.0, 241.0 };

void RecordsScreen_Create(void *objPtr)
{
    RSDK_THIS(RecordsScreen);
    entity->labelPtr            = CREATE_ENTITY(TextLabel);
    entity->labelPtr->fontID    = 0;
    entity->labelPtr->textScale = 0.15;
    entity->labelPtr->textAlpha = 256;
    entity->labelPtr->textX     = -144.0;
    entity->labelPtr->textY     = 100.0;
    entity->labelPtr->textZ     = 16.0;
    entity->labelPtr->alignment = 0;
    entity->labelPtr->useMatrix = true;

    entity->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", -1);
    SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 0xC0);

    entity->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", 1);
    SetStringToFont(entity->textRecords, strRecords, 1);

    entity->field_D4 = GetTextWidth(entity->textRecords, 1, 0.125) * 0.5;

    entity->buttons[0]                  = CREATE_ENTITY(PushButton);
    entity->buttons[0]->useRenderMatrix = true;
    entity->buttons[0]->x               = -64.0;
    entity->buttons[0]->y               = -52.0;
    entity->buttons[0]->z               = 0.0;
    entity->buttons[0]->scale           = 0.175;
    entity->buttons[0]->blue            = 0x00A048;
    entity->buttons[0]->blue2           = 0x00C060;
    SetStringToFont(entity->buttons[0]->text, strPlay, 1);

    entity->buttons[1]                  = CREATE_ENTITY(PushButton);
    entity->buttons[1]->useRenderMatrix = true;
    entity->buttons[1]->x               = 64.0;
    entity->buttons[1]->y               = -52.0;
    entity->buttons[1]->z               = 0.0;
    entity->buttons[1]->scale           = 0.175;
    entity->buttons[1]->blue            = 0x00A048;
    entity->buttons[1]->blue2           = 0x00C060;
    SetStringToFont(entity->buttons[1]->text, strNextAct, 1);

    entity->state = -1;
    debugMode     = false;
}
void RecordsScreen_Main(void *objPtr)
{
    RSDK_THIS(RecordsScreen);
    NativeEntity_TimeAttack *timeAttack = (NativeEntity_TimeAttack *)entity->timeAttack;
    SaveGame *saveGame                  = (SaveGame *)saveRAM;

    switch (entity->state) {
        case -1: {
            int textureID = (entity->zoneID * timeAttack_ActCount) / 6;
            textureID++;

            entity->actCount = timeAttack_ActCount;
            entity->recordOffset = timeAttack_ActCount * entity->zoneID;
            char pathBuf[0x40];
            sprintf(pathBuf, "Data/Game/Menu/TimeAttack%d.png", textureID);
            if (Engine.gameType == GAME_SONIC1 && entity->zoneID == 6) //dumb stupid dumb
                sprintf(pathBuf, "Data/Game/Menu/Intro.png");

            if (timeAttackTex) {
                ReplaceTexture(pathBuf, timeAttackTex);
            }
            else {
                entity->textureTimeAttack = LoadTexture(pathBuf, TEXFMT_RGBA5551);
            }

            if (Engine.gameType == GAME_SONIC1) {
                entity->actCount = 3;
                if (entity->zoneID == 6) { // final
                    entity->actCount     = 1;
                    entity->recordOffset = (timeAttack_ActCount * 6);
                }
                else if (entity->zoneID == 7) { // special
                    entity->actCount     = 6;
                    entity->recordOffset = (timeAttack_ActCount * 6) + 1;
                }
            }
            else if (Engine.gameType == GAME_SONIC2) {
                if (entity->zoneID == 7) //metropolis sux
                    entity->actCount = 3;

                if (entity->zoneID >= 8) {
                    entity->actCount     = 1;
                    switch (entity->zoneID) {
                        case 8: entity->recordOffset = (timeAttack_ActCount * 8) + 1; break;
                        case 9: entity->recordOffset = 22; break;
                        case 10: entity->recordOffset = 20; break;
                        case 11: entity->recordOffset = 23; break;
                    }
                }
            }

            entity->state    = 0;
            if (Engine.gameType == GAME_SONIC2 && entity->zoneID >= 9) {
                switch (entity->zoneID) {
                    default: break;
                    case 9:
                    case 11:
                        entity->timeAttackU = timeAttackU[0];
                        entity->timeAttackV = timeAttackV[0];
                        break;
                    case 10:
                        entity->timeAttackU = timeAttackU[1];
                        entity->timeAttackV = timeAttackV[1];
                        break;
                }
            }
            else {
                entity->timeAttackU = timeAttackU[entity->recordOffset % 6];
                entity->timeAttackV = timeAttackV[entity->recordOffset % 6];
            }

            int pos = 0;
            if (Engine.gameType == GAME_SONIC1) {
                switch (entity->zoneID) {
                    default: pos = timeAttack_ActCount * entity->zoneID; break;
                    case 7: // special stage
                        pos = 6 * entity->zoneID;
                        pos++;
                        break;
                }
            }
            else {
                if (entity->zoneID >= 8) {
                    pos = timeAttack_ActCount * 8;
                    ++pos;
                    pos += entity->zoneID - 8;
                }
                else {
                    pos = timeAttack_ActCount * entity->zoneID;
                }
            }
            pos *= 3;

            SetStringToFont(entity->labelPtr->text, strSaveStageList[entity->recordOffset], 0);
            SetStringToFont8(entity->rank1st, "1.", 1);
            AddTimeStringToFont(entity->rank1st, saveGame->records[pos + (3 * entity->actID)], 1);
            SetStringToFont8(entity->rank2nd, "2.", 1);
            AddTimeStringToFont(entity->rank2nd, saveGame->records[pos + (3 * entity->actID) + 1], 1);
            SetStringToFont8(entity->rank3rd, "3.", 1);
            AddTimeStringToFont(entity->rank3rd, saveGame->records[pos + (3 * entity->actID) + 2], 1);
        }
        // fallthrough
        case 0: {
            if (entity->field_188 < 0x100)
                entity->field_188 += 8;

            entity->scale = fminf(entity->scale + ((1.05 - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrix2, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix2);
            SetRenderMatrix(&entity->renderMatrix);
            memcpy(&entity->labelPtr->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            memcpy(&entity->buttons[0]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            if (entity->actCount > 1) {
                memcpy(&entity->buttons[1]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            }

            entity->field_28 += Engine.deltaTime;
            if (entity->field_28 > 0.5) {
                entity->field_28  = 0.0;
                entity->state     = 1;
                entity->field_188 = 256;
            }
            break;
        }
        case 1: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->matrix2);
            if (!usePhysicalControls) {
                if (touches <= 0) {
                    if (entity->buttons[0]->state == 1) {
                        entity->state = 5;
                        PlaySfx(22, 0);
                        entity->buttons[0]->state = 2;
                    }
                    if (entity->actCount > 1 && entity->buttons[1]->state == 1) {
                        PlaySfx(21, 0);
                        entity->state             = 2;
                        entity->field_191         = 0;
                        entity->actID             = (entity->actID + 1) % entity->actCount;
                        entity->buttons[1]->state = 0;
                    }
                }
                else {
                    entity->buttons[0]->state = 0;
                    entity->buttons[1]->state = 0;
                    if (CheckTouchRect(-64.0, -58.0, ((64.0 * entity->buttons[0]->scale) + entity->buttons[0]->textWidth) * 0.8, 12.0) >= 0) {
                        entity->buttons[0]->state = 1;
                    }
                    else if (entity->actCount > 1) {
                        if (CheckTouchRect(64.0, -58.0, ((64.0 * entity->buttons[1]->scale) + entity->buttons[1]->textWidth) * 0.8, 12.0) >= 0) {
                            entity->buttons[1]->state = 1;
                        }
                    }
                }

                if (touches > 0 && entity->state == 1 && entity->actCount > 1) {
                    if (entity->field_190) {
                        if (entity->field_18C - touchXF[0] > 16.0f) {
                            PlaySfx(21, 0);
                            entity->state     = 2;
                            entity->field_191 = 0;
                            entity->field_190 = 0;
                            entity->actID     = (entity->actID + 1) % entity->actCount;
                        }
                        else if (entity->field_18C - touchXF[0] < -16.0f) {
                            PlaySfx(21, 0);
                            if (--entity->actID < 0)
                                entity->actID = entity->actCount - 1;
                            entity->state     = 2;
                            entity->field_191 = 1;
                            entity->field_190 = 0;
                        }
                    }
                    else {
                        entity->field_190 = 1;
                    }
                    entity->field_18C = touchXF[0];
                }
                else {
                    entity->field_190 = 0;
                }

                if (touches <= 0) {
                    if (entity->prevActPressed) {
                        PlaySfx(21, 0);
                        if (--entity->actID < 0)
                            entity->actID = entity->actCount - 1;
                        entity->state          = 2;
                        entity->prevActPressed = false;
                        entity->field_191      = 1;
                    }
                    if (entity->nextActPressed) {
                        PlaySfx(21, 0);
                        entity->state          = 2;
                        entity->nextActPressed = false;
                        entity->field_191      = 0;
                        entity->actID          = (entity->actID + 1) % entity->actCount;
                    }
                    if (entity->backPressed) {
                        PlaySfx(23, 0);
                        entity->backPressed = false;
                        entity->state       = 4;
                    }
                }
                else {
                    entity->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (entity->actCount > 1) {
                        entity->prevActPressed = CheckTouchRect(-162.0, 0.0, 32.0, 32.0) >= 0;
                        entity->nextActPressed = CheckTouchRect(162.0, 0.0, 32.0, 32.0) >= 0;
                    }
                }

                if (entity->state == 1) {
                    if (keyDown.left) {
                        entity->selectedButton   = 1;
                        usePhysicalControls = true;
                    }
                    else {
                        if (keyDown.right) {
                            entity->selectedButton   = 0;
                            usePhysicalControls = true;
                        }
                        else if (keyPress.B) {
                            PlaySfx(23, 0);
                            entity->backPressed = false;
                            entity->state       = 4;
                        }
                    }
                }
            }
            else {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    entity->prevActPressed = false;
                    entity->nextActPressed = false;
                    entity->backPressed    = false;

                    if (entity->actCount > 1) {
                        if (keyPress.left) {
                            PlaySfx(21, 0);
                            entity->selectedButton--;
                            if (entity->selectedButton < 0) {
                                entity->selectedButton = 1;
                                PlaySfx(21, 0);
                                if (--entity->actID < 0)
                                    entity->actID = entity->actCount - 1;
                                entity->state     = 2;
                                entity->field_191 = 1;
                            }
                        }
                        else if (keyPress.right) {
                            PlaySfx(21, 0);
                            entity->selectedButton++;
                            if (entity->selectedButton >= 2) {
                                entity->selectedButton = 0;
                                PlaySfx(21, 0);
                                entity->state     = 2;
                                entity->field_191 = 0;
                                entity->actID     = (entity->actID + 1) % entity->actCount;
                            }
                        }
                        for (int i = 0; i < 2; ++i) entity->buttons[i]->state = 0;
                        entity->buttons[entity->selectedButton]->state = 1;
                    }
                    else {
                        entity->buttons[entity->selectedButton]->state = 1;
                    }

                    if (keyPress.start || keyPress.A) {
                        if (entity->selectedButton) {
                            PlaySfx(21, 0);
                            entity->state     = 2;
                            entity->field_191 = 0;
                            entity->actID     = (entity->actID + 1) % entity->actCount;
                        }
                        else {
                            entity->state = 5;
                            PlaySfx(22, 0);
                            entity->buttons[0]->state = 2;
                        }
                    }
                    else if (keyPress.B) {
                        PlaySfx(23, 0);
                        entity->backPressed = false;
                        entity->state       = 4;
                    }
                }
            }
            break;
        }
        case 2: {
            int pos = 0;
            if (Engine.gameType == GAME_SONIC1) {
                switch (entity->zoneID) {
                    default: pos += timeAttack_ActCount * entity->zoneID; break;
                    case 7: // special stage
                        pos += 6 * entity->zoneID;
                        pos++;
                        break;
                }
            }
            else {
                if (entity->zoneID >= 8) {
                    pos += timeAttack_ActCount * 8;
                    ++pos;
                    pos += entity->zoneID - 8;
                }
                else {
                    pos += timeAttack_ActCount * entity->zoneID;
                }
            }
            pos *= 3;

            if (entity->field_191) {
                entity->rotationY += (10.0 * Engine.deltaTime);
                if (entity->rotationY > (M_PI * 0.5)) {
                    entity->state     = 3;
                    entity->rotationY = (M_PI * 1.5);
                }
                else {
                    if (Engine.gameType == GAME_SONIC2 && entity->zoneID >= 9) {
                        switch (entity->zoneID) {
                            default: break;
                            case 9:
                            case 11:
                                entity->timeAttackU = timeAttackU[0];
                                entity->timeAttackV = timeAttackV[0];
                                break;
                            case 10:
                                entity->timeAttackU = timeAttackU[1];
                                entity->timeAttackV = timeAttackV[1];
                                break;
                        }
                    }
                    else {
                        entity->timeAttackU = timeAttackU[(entity->recordOffset + entity->actID) % 6];
                        entity->timeAttackV = timeAttackV[(entity->recordOffset + entity->actID) % 6];
                    }
                    SetStringToFont(entity->labelPtr->text, strSaveStageList[entity->recordOffset], 0);
                    SetStringToFont8(entity->rank1st, "1.", 1);
                    AddTimeStringToFont(entity->rank1st, saveGame->records[pos + (3 * entity->actID)], 1);
                    SetStringToFont8(entity->rank2nd, "2.", 1);
                    AddTimeStringToFont(entity->rank2nd, saveGame->records[pos + (3 * entity->actID) + 1], 1);
                    SetStringToFont8(entity->rank3rd, "3.", 1);
                    AddTimeStringToFont(entity->rank3rd, saveGame->records[pos + (3 * entity->actID) + 2], 1);
                }
            }
            else {
                entity->rotationY -= (10.0 * Engine.deltaTime);
                if (entity->rotationY < -(M_PI * 0.5)) {
                    entity->state     = 3;
                    entity->rotationY = -(M_PI * 1.5);
                }
                else {
                    if (Engine.gameType == GAME_SONIC2 && entity->zoneID >= 9) {
                        switch (entity->zoneID) {
                            default: break;
                            case 9: 
                            case 11:
                                entity->timeAttackU = timeAttackU[0];
                                entity->timeAttackV = timeAttackV[0];
                                break;
                            case 10:
                                entity->timeAttackU = timeAttackU[1];
                                entity->timeAttackV = timeAttackV[1];
                                break;
                        }
                    }
                    else {
                        entity->timeAttackU = timeAttackU[(entity->recordOffset + entity->actID) % 6];
                        entity->timeAttackV = timeAttackV[(entity->recordOffset + entity->actID) % 6];
                    }
                    SetStringToFont(entity->labelPtr->text, strSaveStageList[entity->recordOffset], 0);
                    SetStringToFont8(entity->rank1st, "1.", 1);
                    AddTimeStringToFont(entity->rank1st, saveGame->records[pos + (3 * entity->actID)], 1);
                    SetStringToFont8(entity->rank2nd, "2.", 1);
                    AddTimeStringToFont(entity->rank2nd, saveGame->records[pos + (3 * entity->actID) + 1], 1);
                    SetStringToFont8(entity->rank3rd, "3.", 1);
                    AddTimeStringToFont(entity->rank3rd, saveGame->records[pos + (3 * entity->actID) + 2], 1);
                }
            }
            NewRenderState();
            matrixRotateYF(&entity->renderMatrix, entity->rotationY);
            matrixTranslateXYZF(&entity->matrix2, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix2);
            SetRenderMatrix(&entity->renderMatrix);
            break;
        }
        case 3: {
            if (!entity->field_191) {
                entity->rotationY -= (10.0 * Engine.deltaTime);
                if (entity->rotationY < -(M_PI * 2)) {
                    entity->state     = 1;
                    entity->rotationY = 0.0;
                }
            }
            else {
                entity->rotationY += (10.0 * Engine.deltaTime);
                if (entity->rotationY > M_PI * 2) {
                    entity->state     = 1;
                    entity->rotationY = 0.0;
                }
            }
            NewRenderState();
            matrixRotateYF(&entity->renderMatrix, entity->rotationY);
            matrixTranslateXYZF(&entity->matrix2, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix2);
            SetRenderMatrix(&entity->renderMatrix);
            break;
        }
        case 4: {
            if (entity->field_188 > 0)
                entity->field_188 -= 8;

            if (entity->field_28 < 0.2)
                entity->scale = fmaxf(entity->scale + ((1.5f - entity->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);
            else
                entity->scale = fmaxf(entity->scale + ((-1.0f - entity->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrix2, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix2);
            SetRenderMatrix(&entity->renderMatrix);

            memcpy(&entity->labelPtr->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            memcpy(&entity->buttons[0]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            if (entity->actCount > 1) {
                memcpy(&entity->buttons[1]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            }

            entity->field_28 += Engine.deltaTime;
            if (entity->field_28 > 0.5) {
                timeAttack->state = 9;
                RemoveNativeObject(entity->buttons[0]);
                if (entity->actCount > 1) {
                    RemoveNativeObject(entity->buttons[1]);
                }
                RemoveNativeObject(entity->labelPtr);
                RemoveNativeObject(entity);
                return;
            }
            break;
        }
        case 5: {
            SetRenderMatrix(&entity->matrix2);
            if (!entity->buttons[0]->state) {
                SetGlobalVariableByName("options.saveSlot", 0);
                SetGlobalVariableByName("options.gameMode", 2);
                SetGlobalVariableByName("player.lives", 1);
                SetGlobalVariableByName("player.score", 0);
                SetGlobalVariableByName("player.scoreBonus", 50000);
                SetGlobalVariableByName("specialStage.listPos", 0);
                SetGlobalVariableByName("specialStage.emeralds", 0);
                SetGlobalVariableByName("specialStage.nextZone", 0);
                SetGlobalVariableByName("specialStage.nextZone", 0);
                SetGlobalVariableByName("lampPostID", 0);
                SetGlobalVariableByName("starPostID", 0);
                SetGlobalVariableByName("timeAttack.result", 0);

                if (entity->zoneID >= 7 && Engine.gameType == GAME_SONIC1)
                    InitStartingStage(3, entity->actID, 0);
                else if (entity->zoneID >= 9 && Engine.gameType == GAME_SONIC2) {
                    switch (entity->zoneID) {
                        default: break;
                        case 9: InitStartingStage(2, 1, 0); break;
                        case 10: InitStartingStage(1, entity->zoneID * timeAttack_ActCount, 0); break;
                        case 11: InitStartingStage(2, 0, 0); break;
                    }
                } 
                else
                    InitStartingStage(1, entity->zoneID * timeAttack_ActCount + entity->actID, 0);
                entity->state      = 6;
                entity->taResultID = GetGlobalVariableID("timeAttack.result");
                BackupNativeObjects();
                CREATE_ENTITY(FadeScreen);
            }
            break;
        }
        case 6: {
            int pos = 0;
            if (Engine.gameType == GAME_SONIC1) {
                switch (entity->zoneID) {
                    default: pos += timeAttack_ActCount * entity->zoneID; break;
                    case 7: // special stage
                        pos += 6 * entity->zoneID;
                        pos++;
                        break;
                }
            }
            else {
                if (entity->zoneID >= 8) {
                    pos += timeAttack_ActCount * 8;
                    ++pos;
                    pos += entity->zoneID - 8;
                }
                else {
                    pos += timeAttack_ActCount * entity->zoneID;
                }
            }
            pos *= 3;

            SetRenderMatrix(&entity->matrix2);
            if (globalVariables[entity->taResultID] > 0) {
                int *records  = &saveGame->records[pos + (3 * entity->actID)];
                entity->state = 1;
                SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 192);
                int time = globalVariables[entity->taResultID];

                for (int r = 0; r < 3; ++r) {
                    if (time < records[r]) {
                        for (int s = r + 1; s < 3; ++s) records[s + 1] = records[s];
                        records[r]   = time;
                        entity->rank = r + 1;
                        break;
                    }
                }

                SetStringToFont8(entity->rank1st, "1.", 1);
                AddTimeStringToFont(entity->rank1st, records[0], 1);
                SetStringToFont8(entity->rank2nd, "2.", 1);
                AddTimeStringToFont(entity->rank2nd, records[1], 1);
                SetStringToFont8(entity->rank3rd, "3.", 1);
                AddTimeStringToFont(entity->rank3rd, records[2], 1);

                entity->field_28 = 0.0;
                entity->state    = 7;
                PlaySfx(39, 0);
                WriteSaveRAMData();

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
                    SetStringToFont8(timeAttack->zoneButtons[i]->timeText, "", 2);
                    AddTimeStringToFont(timeAttack->zoneButtons[i]->timeText, timeAttack->totalTime, 2);
                    timeAttack->zoneButtons[i]->textWidth = GetTextWidth(timeAttack->zoneButtons[i]->zoneText, 2, 0.25) * 0.5;
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
                            for (int a = 0; a < timeAttack_ActCount; ++a) timeAttack->totalTime += saveGame->records[3 * (pos + a)];
                            pos += timeAttack_ActCount;
                        }
                        else {
                            timeAttack->totalTime += saveGame->records[3 * pos];
                            pos++;
                        }
                    }
                }

                SetStringToFont(timeAttack->button->text, strTotalTime, 1);
                AddTimeStringToFont(timeAttack->button->text, timeAttack->totalTime, 1);
                if (timeAttack->totalTime <= 270000) {
                    int ach    = 11;
                    int status = 100;
                    SetAchievement(&ach, &status);
                }
            }
            break;
        }
        case 7: {
            SetRenderMatrix(&entity->matrix2);
            entity->field_2C += Engine.deltaTime;
            if (entity->field_2C > 0.1)
                entity->field_2C -= 0.1;

            entity->field_28 += Engine.deltaTime;
            if (entity->field_28 > 2.0) {
                entity->field_28 = 0.0;
                entity->field_2C = 0.0;
                entity->state    = 1;
                entity->rank     = 0;
            }
            break;
        }
        default: break;
    }

    RenderMesh(entity->meshPanel, 0, false);
    RenderRect(-124.0, 69.0, 0.0, 128.0, 98.0, 128, 128, 128, 255);
    RenderImage(-60.0, 20.0, 0.0, 0.38, 0.38, 159.0, 119.0, 318.0, 238.0, entity->timeAttackU, entity->timeAttackV, 255, entity->textureTimeAttack);
    RenderText(entity->textRecords, 1, 72.0 - entity->field_D4, 56.0, 0.0, 0.125, 255);

    if (entity->field_2C < 0.05 || entity->rank != 1)
        RenderText(entity->rank1st, 1, 24.0, 32.0, 0.0, 0.125, 255);

    if (entity->field_2C < 0.05 || entity->rank != 2)
        RenderText(entity->rank2nd, 1, 24.0, 8.0, 0.0, 0.125, 255);

    if (entity->field_2C < 0.05 || entity->rank != 3)
        RenderText(entity->rank3rd, 1, 24.0, -16.0, 0.0, 0.125, 255);
    NewRenderState();
    SetRenderMatrix(NULL);
    if (entity->actCount > 1) {
        if (entity->prevActPressed)
            RenderImageFlipH(-146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, entity->field_188, entity->textureArrows);
        else
            RenderImageFlipH(-146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, entity->field_188, entity->textureArrows);

        if (entity->nextActPressed)
            RenderImage(146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, entity->field_188, entity->textureArrows);
        else
            RenderImage(146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, entity->field_188, entity->textureArrows);
    }

    if (entity->backPressed)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, entity->field_188, entity->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, entity->field_188, entity->textureArrows);
}
