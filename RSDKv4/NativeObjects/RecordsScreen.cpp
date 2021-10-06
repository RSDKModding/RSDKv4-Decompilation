#include "RetroEngine.hpp"

float timeAttackU[] = {
    1.0, 321.0, 641.0, 1.0, 321.0, 641.0,
};
float timeAttackV[] = { 1.0, 1.0, 1.0, 241.0, 241.0, 241.0 };

void RecordsScreen_Create(void *objPtr)
{
    RSDK_THIS(RecordsScreen);
    entity->labelPtr                  = CREATE_ENTITY(TextLabel);
    entity->labelPtr->fontID          = FONT_HEADING;
    entity->labelPtr->scale           = 0.15;
    entity->labelPtr->alpha           = 256;
    entity->labelPtr->x               = -144.0;
    entity->labelPtr->y               = 100.0;
    entity->labelPtr->z               = 16.0;
    entity->labelPtr->state           = TEXTLABEL_STATE_IDLE;
    entity->labelPtr->useRenderMatrix = true;

    entity->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", -1);
    SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 0xC0);

    entity->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA5551);
    SetStringToFont(entity->textRecords, strRecords, FONT_LABEL);

    entity->field_D4 = GetTextWidth(entity->textRecords, FONT_LABEL, 0.125) * 0.5;

    entity->buttons[RECORDSSCREEN_BUTTON_PLAY]                   = CREATE_ENTITY(PushButton);
    entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->useRenderMatrix  = true;
    entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->x                = -64.0;
    entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->y                = -52.0;
    entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->z                = 0.0;
    entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->scale            = 0.175;
    entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->bgColour         = 0x00A048;
    entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->bgColourSelected = 0x00C060;
    SetStringToFont(entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->text, strPlay, FONT_LABEL);

    entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]                   = CREATE_ENTITY(PushButton);
    entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->useRenderMatrix  = true;
    entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->x                = 64.0;
    entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->y                = -52.0;
    entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->z                = 0.0;
    entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->scale            = 0.175;
    entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->bgColour         = 0x00A048;
    entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->bgColourSelected = 0x00C060;
    SetStringToFont(entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->text, strNextAct, FONT_LABEL);

    entity->state = RECORDSSCREEN_STATE_SETUP;
    debugMode     = false;
}
void RecordsScreen_Main(void *objPtr)
{
    RSDK_THIS(RecordsScreen);
    NativeEntity_TimeAttack *timeAttack = (NativeEntity_TimeAttack *)entity->timeAttack;
    SaveGame *saveGame                  = (SaveGame *)saveRAM;

    switch (entity->state) {
        case RECORDSSCREEN_STATE_SETUP: {
            int textureID = (entity->zoneID * timeAttack_ActCount) / 6;
            textureID++;

            entity->actCount     = timeAttack_ActCount;
            entity->recordOffset = timeAttack_ActCount * entity->zoneID;
            char pathBuf[0x40];
            sprintf(pathBuf, "Data/Game/Menu/TimeAttack%d.png", textureID);
            if (Engine.gameType == GAME_SONIC1 && entity->zoneID == 6) // dumb stupid dumb
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
                if (entity->zoneID == 7) // metropolis sux
                    entity->actCount = 3;

                if (entity->zoneID >= 8) {
                    entity->actCount = 1;
                    switch (entity->zoneID) {
                        case 8: entity->recordOffset = (timeAttack_ActCount * 8) + 1; break;
                        case 9: entity->recordOffset = 22; break;
                        case 10: entity->recordOffset = 20; break;
                        case 11: entity->recordOffset = 23; break;
                    }
                }
            }

            entity->state = RECORDSSCREEN_STATE_ENTER;
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

            int nameID = entity->recordOffset == (timeAttack_ActCount * 8) + 1 ? entity->recordOffset + 1 : entity->recordOffset;
            SetStringToFont(entity->labelPtr->text, strSaveStageList[nameID], 0);
            SetStringToFont8(entity->rank1st, "1.", FONT_LABEL);
            AddTimeStringToFont(entity->rank1st, saveGame->records[pos + (3 * entity->actID)], FONT_LABEL);
            SetStringToFont8(entity->rank2nd, "2.", FONT_LABEL);
            AddTimeStringToFont(entity->rank2nd, saveGame->records[pos + (3 * entity->actID) + 1], FONT_LABEL);
            SetStringToFont8(entity->rank3rd, "3.", FONT_LABEL);
            AddTimeStringToFont(entity->rank3rd, saveGame->records[pos + (3 * entity->actID) + 2], FONT_LABEL);
        }
        // fallthrough
        case RECORDSSCREEN_STATE_ENTER: {
            if (entity->buttonAlpha < 0x100)
                entity->buttonAlpha += 8;

            entity->scale = fminf(entity->scale + ((1.05 - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrixTemp);
            SetRenderMatrix(&entity->renderMatrix);
            memcpy(&entity->labelPtr->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            memcpy(&entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            if (entity->actCount > 1)
                memcpy(&entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            entity->field_28 += Engine.deltaTime;
            if (entity->field_28 > 0.5) {
                entity->field_28    = 0.0;
                entity->state       = RECORDSSCREEN_STATE_MAIN;
                entity->buttonAlpha = 256;
            }
            break;
        }
        case RECORDSSCREEN_STATE_MAIN: {
            CheckKeyDown(&inputDown);
            CheckKeyPress(&inputPress);
            SetRenderMatrix(&entity->matrixTemp);
            if (!usePhysicalControls) {
                if (touches <= 0) {
                    if (entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->state == PUSHBUTTON_STATE_SELECTED) {
                        entity->state = RECORDSSCREEN_STATE_LOADSTAGE;
                        PlaySfxByName("Menu Select", false);
                        entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->state = PUSHBUTTON_STATE_FLASHING;
                    }
                    if (entity->actCount > 1 && entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->state == PUSHBUTTON_STATE_SELECTED) {
                        PlaySfxByName("Menu Move", false);
                        entity->state                                        = RECORDSSCREEN_STATE_FLIP;
                        entity->flipRight                                    = false;
                        entity->actID                                        = (entity->actID + 1) % entity->actCount;
                        entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->state = PUSHBUTTON_STATE_UNSELECTED;
                    }
                }
                else {
                    entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->state    = PUSHBUTTON_STATE_UNSELECTED;
                    entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->state = PUSHBUTTON_STATE_UNSELECTED;
                    if (CheckTouchRect(
                            -64.0, -58.0,
                            ((64.0 * entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->scale) + entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->textWidth)
                                * 0.8,
                            12.0)
                        >= 0) {
                        entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->state = PUSHBUTTON_STATE_SELECTED;
                    }
                    else if (entity->actCount > 1) {
                        if (CheckTouchRect(64.0, -58.0,
                                           ((64.0 * entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->scale)
                                            + entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->textWidth)
                                               * 0.8,
                                           12.0)
                            >= 0) {
                            entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->state = PUSHBUTTON_STATE_SELECTED;
                        }
                    }
                }

                if (touches > 0 && entity->state == RECORDSSCREEN_STATE_MAIN && entity->actCount > 1) {
                    if (entity->field_190) {
                        if (entity->field_18C - touchXF[0] > 16.0f) {
                            PlaySfxByName("Menu Move", false);
                            entity->state     = RECORDSSCREEN_STATE_FLIP;
                            entity->flipRight = false;
                            entity->field_190 = false;
                            entity->actID     = (entity->actID + 1) % entity->actCount;
                        }
                        else if (entity->field_18C - touchXF[0] < -16.0f) {
                            PlaySfxByName("Menu Move", false);
                            if (--entity->actID < 0)
                                entity->actID = entity->actCount - 1;
                            entity->state     = RECORDSSCREEN_STATE_FLIP;
                            entity->flipRight = true;
                            entity->field_190 = false;
                        }
                    }
                    else {
                        entity->field_190 = true;
                    }
                    entity->field_18C = touchXF[0];
                }
                else {
                    entity->field_190 = false;
                }

                if (touches <= 0) {
                    if (entity->prevActPressed) {
                        PlaySfxByName("Menu Move", false);
                        if (--entity->actID < 0)
                            entity->actID = entity->actCount - 1;
                        entity->state          = RECORDSSCREEN_STATE_FLIP;
                        entity->prevActPressed = false;
                        entity->flipRight      = true;
                    }
                    if (entity->nextActPressed) {
                        PlaySfxByName("Menu Move", false);
                        entity->state          = RECORDSSCREEN_STATE_FLIP;
                        entity->nextActPressed = false;
                        entity->flipRight      = false;
                        entity->actID          = (entity->actID + 1) % entity->actCount;
                    }
                    if (entity->backPressed) {
                        PlaySfxByName("Menu Back", false);
                        entity->backPressed = false;
                        entity->state       = RECORDSSCREEN_STATE_EXIT;
                    }
                }
                else {
                    entity->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (entity->actCount > 1) {
                        entity->prevActPressed = CheckTouchRect(-162.0, 0.0, 32.0, 32.0) >= 0;
                        entity->nextActPressed = CheckTouchRect(162.0, 0.0, 32.0, 32.0) >= 0;
                    }
                }

                if (entity->state == RECORDSSCREEN_STATE_MAIN) {
                    if (inputDown.left) {
                        entity->selectedButton = RECORDSSCREEN_BUTTON_NEXTACT;
                        usePhysicalControls    = true;
                    }
                    else {
                        if (inputDown.right) {
                            entity->selectedButton = RECORDSSCREEN_BUTTON_PLAY;
                            usePhysicalControls    = true;
                        }
                        else if (inputPress.B) {
                            PlaySfxByName("Menu Back", false);
                            entity->backPressed = false;
                            entity->state       = RECORDSSCREEN_STATE_EXIT;
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
                        if (inputPress.left) {
                            PlaySfxByName("Menu Move", false);
                            entity->selectedButton--;
                            if (entity->selectedButton < 0) {
                                entity->selectedButton = 1;
                                PlaySfxByName("Menu Move", false);
                                if (--entity->actID < 0)
                                    entity->actID = entity->actCount - 1;
                                entity->state     = RECORDSSCREEN_STATE_FLIP;
                                entity->flipRight = true;
                            }
                        }
                        else if (inputPress.right) {
                            PlaySfxByName("Menu Move", false);
                            entity->selectedButton++;
                            if (entity->selectedButton >= 2) {
                                entity->selectedButton = 0;
                                PlaySfxByName("Menu Move", false);
                                entity->state     = RECORDSSCREEN_STATE_FLIP;
                                entity->flipRight = false;
                                entity->actID     = (entity->actID + 1) % entity->actCount;
                            }
                        }
                        for (int i = 0; i < 2; ++i) entity->buttons[i]->state = PUSHBUTTON_STATE_UNSELECTED;
                        entity->buttons[entity->selectedButton]->state = PUSHBUTTON_STATE_SELECTED;
                    }
                    else {
                        entity->buttons[entity->selectedButton]->state = PUSHBUTTON_STATE_SELECTED;
                    }

                    if (inputPress.start || inputPress.A) {
                        if (entity->selectedButton) {
                            PlaySfxByName("Menu Move", false);
                            entity->state     = RECORDSSCREEN_STATE_FLIP;
                            entity->flipRight = 0;
                            entity->actID     = (entity->actID + 1) % entity->actCount;
                        }
                        else {
                            entity->state = RECORDSSCREEN_STATE_LOADSTAGE;
                            PlaySfxByName("Menu Select", false);
                            entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->state = PUSHBUTTON_STATE_FLASHING;
                        }
                    }
                    else if (inputPress.B) {
                        PlaySfxByName("Menu Back", false);
                        entity->backPressed = false;
                        entity->state       = RECORDSSCREEN_STATE_EXIT;
                    }
                }
            }
            break;
        }
        case RECORDSSCREEN_STATE_FLIP: {
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

            if (!entity->flipRight) {
                entity->rotationY -= (10.0 * Engine.deltaTime);
            }
            else {
                entity->rotationY += (10.0 * Engine.deltaTime);
            }

            if (abs(entity->rotationY) > (M_PI * 0.5)) {
                entity->state     = RECORDSSCREEN_STATE_FINISHFLIP;
                entity->rotationY = entity->rotationY < 0.0f ? -(M_PI * 1.5) : (M_PI * 1.5);

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
                int nameID = entity->recordOffset == (timeAttack_ActCount * 8) + 1 ? entity->recordOffset + 1 : entity->recordOffset;
                SetStringToFont(entity->labelPtr->text, strSaveStageList[nameID + entity->actID], FONT_HEADING);
                SetStringToFont8(entity->rank1st, "1.", FONT_LABEL);
                AddTimeStringToFont(entity->rank1st, saveGame->records[pos + (3 * entity->actID)], FONT_LABEL);
                SetStringToFont8(entity->rank2nd, "2.", FONT_LABEL);
                AddTimeStringToFont(entity->rank2nd, saveGame->records[pos + (3 * entity->actID) + 1], FONT_LABEL);
                SetStringToFont8(entity->rank3rd, "3.", FONT_LABEL);
                AddTimeStringToFont(entity->rank3rd, saveGame->records[pos + (3 * entity->actID) + 2], FONT_LABEL);
            }

            NewRenderState();
            matrixRotateYF(&entity->renderMatrix, entity->rotationY);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrixTemp);
            SetRenderMatrix(&entity->renderMatrix);
            break;
        }
        case RECORDSSCREEN_STATE_FINISHFLIP: {
            if (!entity->flipRight) {
                entity->rotationY -= (10.0 * Engine.deltaTime);
                if (entity->rotationY < -(M_PI_2)) {
                    entity->state     = RECORDSSCREEN_STATE_MAIN;
                    entity->rotationY = 0.0;
                }
            }
            else {
                entity->rotationY += (10.0 * Engine.deltaTime);
                if (entity->rotationY > M_PI_2) {
                    entity->state     = RECORDSSCREEN_STATE_MAIN;
                    entity->rotationY = 0.0;
                }
            }
            NewRenderState();
            matrixRotateYF(&entity->renderMatrix, entity->rotationY);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrixTemp);
            SetRenderMatrix(&entity->renderMatrix);
            break;
        }
        case RECORDSSCREEN_STATE_EXIT: {
            if (entity->buttonAlpha > 0)
                entity->buttonAlpha -= 8;

            if (entity->field_28 < 0.2)
                entity->scale = fmaxf(entity->scale + ((1.5f - entity->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);
            else
                entity->scale = fmaxf(entity->scale + ((-1.0f - entity->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrixTemp);
            SetRenderMatrix(&entity->renderMatrix);

            memcpy(&entity->labelPtr->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            memcpy(&entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            if (entity->actCount > 1)
                memcpy(&entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            entity->field_28 += Engine.deltaTime;
            if (entity->field_28 > 0.5) {
                timeAttack->state = TIMEATTACK_STATE_EXITSUBMENU;
                RemoveNativeObject(entity->buttons[RECORDSSCREEN_BUTTON_PLAY]);
                RemoveNativeObject(entity->buttons[RECORDSSCREEN_BUTTON_NEXTACT]);
                RemoveNativeObject(entity->labelPtr);
                RemoveNativeObject(entity);
                return;
            }
            break;
        }
        case RECORDSSCREEN_STATE_LOADSTAGE: {
            SetRenderMatrix(&entity->matrixTemp);
            if (entity->buttons[RECORDSSCREEN_BUTTON_PLAY]->state == PUSHBUTTON_STATE_UNSELECTED) {
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
                    InitStartingStage(STAGELIST_SPECIAL, entity->actID, 0);
                else if (entity->zoneID >= 9 && Engine.gameType == GAME_SONIC2) {
                    switch (entity->zoneID) {
                        default: break;
                        case 9: InitStartingStage(STAGELIST_BONUS, 1, 0); break;
                        case 10: InitStartingStage(STAGELIST_REGULAR, entity->zoneID * timeAttack_ActCount, 0); break;
                        case 11: InitStartingStage(STAGELIST_BONUS, 0, 0); break;
                    }
                }
                else
                    InitStartingStage(STAGELIST_REGULAR, entity->zoneID * timeAttack_ActCount + entity->actID, 0);

                entity->state      = RECORDSSCREEN_STATE_SHOWRESULTS;
                entity->taResultID = GetGlobalVariableID("timeAttack.result");
                BackupNativeObjects();
                CREATE_ENTITY(FadeScreen);
            }
            break;
        }
        case RECORDSSCREEN_STATE_SHOWRESULTS: {
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

            SetRenderMatrix(&entity->matrixTemp);
            entity->state = RECORDSSCREEN_STATE_MAIN;
            SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 0xC0);
            if (globalVariables[entity->taResultID] > 0) {
                int *records  = &saveGame->records[pos + (3 * entity->actID)];
                int time = globalVariables[entity->taResultID];

                for (int r = 0; r < 3; ++r) {
                    if (time < records[r]) {
                        for (int s = r + 1; s < 3; ++s) records[s + 1] = records[s];
                        records[r]   = time;
                        entity->rank = r + 1;
                        break;
                    }
                }

                SetStringToFont8(entity->rank1st, "1.", FONT_LABEL);
                AddTimeStringToFont(entity->rank1st, records[0], FONT_LABEL);
                SetStringToFont8(entity->rank2nd, "2.", FONT_LABEL);
                AddTimeStringToFont(entity->rank2nd, records[1], FONT_LABEL);
                SetStringToFont8(entity->rank3rd, "3.", FONT_LABEL);
                AddTimeStringToFont(entity->rank3rd, records[2], FONT_LABEL);

                entity->field_28 = 0.0;
                entity->state    = RECORDSSCREEN_STATE_EXITRESULTS;
                PlaySfxByName("Event", false);
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
                            for (int a = 0; a < timeAttack_ActCount; ++a) timeAttack->totalTime += saveGame->records[3 * (pos + a)];
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
            SetRenderMatrix(&entity->matrixTemp);
            entity->field_2C += Engine.deltaTime;
            if (entity->field_2C > 0.1)
                entity->field_2C -= 0.1;

            entity->field_28 += Engine.deltaTime;
            if (entity->field_28 > 2.0) {
                entity->field_28 = 0.0;
                entity->field_2C = 0.0;
                entity->state    = RECORDSSCREEN_STATE_MAIN;
                entity->rank     = 0;
            }
            break;
        }
        default: break;
    }

    RenderMesh(entity->meshPanel, MESH_COLOURS, false);
    RenderRect(-124.0, 69.0, 0.0, 128.0, 98.0, 128, 128, 128, 255);
    RenderImage(-60.0, 20.0, 0.0, 0.38, 0.38, 159.0, 119.0, 318.0, 238.0, entity->timeAttackU, entity->timeAttackV, 255, entity->textureTimeAttack);
    RenderText(entity->textRecords, FONT_LABEL, 72.0 - entity->field_D4, 56.0, 0.0, 0.125, 255);

    if (entity->field_2C < 0.05 || entity->rank != 1)
        RenderText(entity->rank1st, FONT_LABEL, 24.0, 32.0, 0.0, 0.125, 255);

    if (entity->field_2C < 0.05 || entity->rank != 2)
        RenderText(entity->rank2nd, FONT_LABEL, 24.0, 8.0, 0.0, 0.125, 255);

    if (entity->field_2C < 0.05 || entity->rank != 3)
        RenderText(entity->rank3rd, FONT_LABEL, 24.0, -16.0, 0.0, 0.125, 255);
    NewRenderState();
    SetRenderMatrix(NULL);
    if (entity->actCount > 1) {
        if (entity->prevActPressed)
            RenderImageFlipH(-146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, entity->buttonAlpha, entity->textureArrows);
        else
            RenderImageFlipH(-146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, entity->buttonAlpha, entity->textureArrows);

        if (entity->nextActPressed)
            RenderImage(146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, entity->buttonAlpha, entity->textureArrows);
        else
            RenderImage(146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, entity->buttonAlpha, entity->textureArrows);
    }

    if (entity->backPressed)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, entity->buttonAlpha, entity->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, entity->buttonAlpha, entity->textureArrows);
}