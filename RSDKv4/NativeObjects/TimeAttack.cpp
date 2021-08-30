#include "RetroEngine.hpp"

byte timeAttackTex;

int timeAttack_ZoneCount = 12;
int timeAttack_ActCount    = 2;

void TimeAttack_Create(void *objPtr)
{
    RSDK_THIS(TimeAttack);
    entity->menuControl = (NativeEntity_MenuControl *)GetNativeObject(0);
    ReadSaveRAMData();
    SaveGame *saveGame = (SaveGame *)saveRAM;

    int actCount = 0;
    if (Engine.gameType == GAME_SONIC1) {
        timeAttack_ZoneCount = 8;
        timeAttack_ActCount  = 3;
        //GHZ-SBZ + FZ & 6 SS
        actCount              = (timeAttack_ZoneCount * timeAttack_ActCount) + 7;
    }
    else {
        timeAttack_ZoneCount = 12;
        timeAttack_ActCount  = 2;
        actCount             = timeAttack_ZoneCount * timeAttack_ActCount;
    }

    bool saveRAMUpdated = false;
    for (int i = 0; i < actCount * 3; i += 3) {
        //1st
        if (!saveGame->records[i]) {
            saveGame->records[i] = 60000;
            saveRAMUpdated       = true;
        }

        //2nd
        if (!saveGame->records[i + 1]) {
            saveGame->records[i + 1] = 60000;
            saveRAMUpdated       = true;
        }

        //3rd
        if (!saveGame->records[i + 2]) {
            saveGame->records[i + 2] = 60000;
            saveRAMUpdated       = true;
        }
    }
    if (saveRAMUpdated)
        WriteSaveRAMData();

    int pos = 0;
    float x = -72.0;
    for (int i = 0; i < timeAttack_ZoneCount; ++i) {
        NativeEntity_ZoneButton *zoneButton = CREATE_ENTITY(ZoneButton);
        entity->zoneButtons[i]              = zoneButton;
        zoneButton->textX                    = x;
        SetStringToFont(zoneButton->zoneText, strStageList[i], 2);

        entity->totalTime = 0;
        if (Engine.gameType == GAME_SONIC1) {
            switch (i) {
                default:
                    for (int a = 0; a < timeAttack_ActCount; ++a) entity->totalTime += saveGame->records[3 * (pos + a)];
                    pos += timeAttack_ActCount;
                    break;
                case 6:// final zone
                    entity->totalTime += saveGame->records[3 * pos];
                    pos++;
                    break;
                case 7:// special stage
                    for (int a = 0; a < 6; ++a) entity->totalTime += saveGame->records[3 * (pos + a)];
                    pos += 6;
                    break;
            }
        }
        else {
            if (i < 9) {
                for (int a = 0; a < timeAttack_ActCount; ++a) entity->totalTime += saveGame->records[3 * (pos + a)];
                pos += timeAttack_ActCount;
            }
            else {
                entity->totalTime += saveGame->records[3 * pos];
                pos++;
            }
        }
        SetStringToFont8(entity->zoneButtons[i]->timeText, "", 2);
        AddTimeStringToFont(entity->zoneButtons[i]->timeText, entity->totalTime, 2);
        entity->zoneButtons[i]->textWidth = GetTextWidth(entity->zoneButtons[i]->zoneText, 2, 0.25) * 0.5;

        if (!((i + 1) % 3))
            x += 432.0;
        else
            x += 144.0;
    }

    entity->totalTime = 0;
    pos               = 0;
    for (int z = 0; z < timeAttack_ZoneCount; ++z) {
        // 1st
        if (Engine.gameType == GAME_SONIC1) {
            switch (z) {
                default:
                    for (int a = 0; a < timeAttack_ActCount; ++a) entity->totalTime += saveGame->records[3 * (pos + a)];
                    pos += timeAttack_ActCount;
                    break;
                case 6: // final zone
                    entity->totalTime += saveGame->records[3 * pos];
                    pos++;
                    break;
                case 7: // special stage
                    for (int a = 0; a < 6; ++a) entity->totalTime += saveGame->records[3 * (pos + a)];
                    pos += 6;
                    break;
            }
        }
        else {
            if (z < 9) {
                for (int a = 0; a < timeAttack_ActCount; ++a) entity->totalTime += saveGame->records[3 * (pos + a)];
                pos += timeAttack_ActCount;
            }
            else {
                entity->totalTime += saveGame->records[3 * pos];
                pos++;
            }
        }
    }

    int zone = saveGame->totalScore;
    for (int i = 0; i < 4; ++i) {
        if (saveGame->files[i].stageID > zone)
            zone = saveGame->files[i].stageID;
    }
    saveGame->totalScore = zone;

    float tx = 480.0f;
    float ty = 120.0f;
    for (int i = 0; i < timeAttack_ZoneCount; ++i) {
        entity->zoneButtons[i]->texX = tx;
        entity->zoneButtons[i]->texY = ty;
        entity->zoneButtons[i]->flag = false;
        if (zone > timeAttack_ActCount * (i + 1) || i > 6) {
            entity->zoneButtons[i]->flag = true;
        }

        if (Engine.gameType == GAME_SONIC1) {
            if (i == 5) { // this sucks (final zone hack)
                tx += 320.0f;
                tx += 320.0f;
                ty += 240.0f;

                entity->zoneButtons[i + 1]->texX = tx;
                entity->zoneButtons[i + 1]->texY = ty;
                if (zone > timeAttack_ActCount * (i + 1)) {
                    entity->zoneButtons[i]->flag = true;
                }
                ++i;

                tx -= 320.0f;
                tx -= 320.0f;
                ty -= 240.0f;
            }

            tx += 320.0f;
            if (tx >= 960.0f) {
                tx = 120.0f;
                ty += 240.0f;
            }
        }
        else {
            if (i < 9) { // hack this to only show the ??? icon
                tx += 320.0f;
                if (tx >= 960.0f) {
                    tx = 120.0f;
                    ty += 240.0f;
                }

                //skip icon section
                if (i == 6) {
                    tx += 320.0f;
                    if (tx >= 960.0f) {
                        tx = 120.0f;
                        ty += 240.0f;
                    }
                }
            }
        }
    }

    entity->translateY = -400.0;
    entity->float134   = -72.0;
    matrixRotateXYZF(&entity->matrix1, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
    matrixTranslateXYZF(&entity->matrix2, 0.0, -36.0, 240.0);
    matrixMultiplyF(&entity->matrix1, &entity->matrix2);
    matrixInvertF(&entity->matrix3, &entity->matrix1);
    matrixRotateXYZF(&entity->matrix1, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
    matrixTranslateXYZF(&entity->matrix2, 0.0, entity->translateY, 80.0);
    matrixMultiplyF(&entity->matrix1, &entity->matrix2);

    entity->labelPtr         = CREATE_ENTITY(TextLabel);
    entity->labelPtr->fontID = 0;
    entity->labelPtr->textScale       = 0.2;
    entity->labelPtr->textAlpha       = 0;
    entity->labelPtr->textZ           = 0;
    entity->labelPtr->state       = 0;
    SetStringToFont(entity->labelPtr->text, strTimeAttack, 0);
    entity->labelPtr->textWidth = 512.0;
    entity->float24             = DegreesToRad(22.5);
    matrixRotateYF(&entity->labelPtr->renderMatrix, entity->float24);
    matrixTranslateXYZF(&entity->matrix2, -128.0, 80.0, 160.0);
    matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrix2);
    entity->labelPtr->useRenderMatrix = true;

    entity->button                    = CREATE_ENTITY(SubMenuButton);
    entity->button->matXOff         = 512.0;
    entity->button->textY           = -4.0;
    entity->button->matZ            = 0.0;
    entity->button->textScale       = 0.1;

    entity->float3C                   = DegreesToRad(16.0);
    matrixRotateYF(&entity->button->matrix, entity->float3C);
    matrixTranslateXYZF(&entity->matrix2, -128.0, 48.0, 160.0);
    matrixMultiplyF(&entity->button->matrix, &entity->matrix2);
    entity->button->useMatrix = true;
    SetStringToFont(entity->button->text, strTotalTime, 1);
    AddTimeStringToFont(entity->button->text, entity->totalTime, 1);

    entity->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);
    entity->pagePrevAlpha = 0;
    entity->pageNextAlpha = 0x100;
}
void TimeAttack_Main(void *objPtr)
{
    RSDK_THIS(TimeAttack);
    switch (entity->state) {
        case 0: {
            entity->timer += Engine.deltaTime;
            if (entity->timer > 1.0) {
                entity->timer = 0.0;
                entity->state = 1;
            }
            break;
        }
        case 1: {
            entity->labelPtr->textWidth /= (1.125 * (60.0 * Engine.deltaTime));
            entity->timer += (Engine.deltaTime + Engine.deltaTime);
            entity->labelPtr->textAlpha = (entity->timer * 256.0);
            entity->button->matXOff += ((-176.0 - entity->button->matXOff) / ((60.0 * Engine.deltaTime) * 16.0));
            entity->translateY += ((-36.0 - entity->translateY) / ((60.0 * Engine.deltaTime) * 8.0));
            matrixRotateXYZF(&entity->matrix1, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
            matrixTranslateXYZF(&entity->matrix2, 0.0, entity->translateY, 80.0);
            matrixMultiplyF(&entity->matrix1, &entity->matrix2);

            if (entity->timer > 1.0) {
                entity->timer  = 0.0;
                entity->state  = 2;
                keyPress.start = false;
                keyPress.A     = false;
            }
            break;
        }
        case 2:
        case 3:
        case 4: {
            bool canPrev = 3 * (entity->pageID - 1) >= 0;
            bool canNext = 3 * (entity->pageID + 1) < timeAttack_ZoneCount;

            if (!canNext) {
                if (entity->pagePrevAlpha < 0x100)
                    entity->pagePrevAlpha += 0x20;
                if (entity->pageNextAlpha > 0)
                    entity->pageNextAlpha -= 0x20;
            }
            else if (!canPrev) {
                if (entity->pagePrevAlpha > 0)
                    entity->pagePrevAlpha -= 0x20;
                if (entity->pageNextAlpha < 0x100)
                    entity->pageNextAlpha += 0x20;
            }
            else {
                if (entity->pagePrevAlpha < 0x100)
                    entity->pagePrevAlpha += 0x20;
                if (entity->pageNextAlpha < 0x100)
                    entity->pageNextAlpha += 0x20;
            }

            if (!usePhysicalControls) {
                if (touches <= 0) {
                    for (int i = 0; i < timeAttack_ZoneCount; ++i) {
                        if (entity->zoneButtons[i]->state == 1) {
                            PlaySfx(22, 0);
                            entity->zoneButtons[i]->state = 2;
                            entity->zoneID                = i;
                            entity->state                 = 6;
                        }
                    }

                    if (entity->pagePrevPressed && canPrev) {
                        entity->pagePrevPressed = false;
                        PlaySfx(21, 0);
                        entity->state = 5;
                        entity->pageID--;
                        entity->float134 = -72.0f - (720.0f * entity->pageID);
                        entity->zoneID   = (3 * entity->pageID) + 2;
                    }
                    if (entity->pageNextPressed && canNext) {
                        entity->pageNextPressed = false;
                        PlaySfx(21, 0);
                        entity->state = 5;
                        entity->pageID++;
                        entity->float134 = -72.0f - (720.0f * entity->pageID);
                        entity->zoneID   = (3 * entity->pageID);
                        entity->byte148 = 0;
                    }
                }
                else {
                    for (int i = 0; i < timeAttack_ZoneCount; ++i) entity->zoneButtons[i]->state = 0;

                    if (CheckTouchRectMatrix(&entity->matrix3, 0.0, entity->zoneButtons[0]->textY + 16.0, 512.0, 40.0) >= 0) {
                        int offset = entity->pageID * 3;

                        if (entity->zoneButtons[offset]->flag && CheckTouchRect(-78.0, 0.0, 48.0, 120.0) >= 0)
                            entity->zoneButtons[offset]->state = 1;

                        if (offset + 1 < timeAttack_ZoneCount) {
                            if (entity->zoneButtons[offset + 1]->flag && CheckTouchRect(22.0, 0.0, 38.0, 120.0) >= 0)
                                entity->zoneButtons[offset + 1]->state = 1;
                        }

                        if (offset + 2 < timeAttack_ZoneCount) {
                            if (entity->zoneButtons[offset + 2]->flag && CheckTouchRect(100.0, 0.0, 30.0, 120.0) >= 0)
                                entity->zoneButtons[offset + 2]->state = 1;
                        }
                    }

                    entity->pagePrevPressed = false;
                    entity->pageNextPressed = false;
                    if (CheckTouchRect(-150.0, -40.0, 16.0, 24.0) >= 0 && canPrev) {
                        entity->pagePrevPressed = true;
                    }
                    if (CheckTouchRect(144.0, -2.0, 12.0, 24.0) >= 0 && canNext) {
                        entity->pageNextPressed = true;
                    }
                }

                if (touches > 0 && entity->state == 2) {
                    if (entity->byte148) {
                        if ((entity->float144 - touchXF[0]) < -16.0f) {
                            PlaySfx(21, 0);
                            entity->state = 5;
                            entity->pageID--;
                            entity->float134 = -72.0f - (720.0f * (entity->pageID));
                            entity->zoneID   = 3;
                            entity->byte148  = 0;
                            for (int i = 0; i < timeAttack_ZoneCount; ++i) entity->zoneButtons[i]->state = 0;
                        }
                    }
                    else {
                        entity->byte148 = 1;
                    }
                    entity->float144 = touchXF[0];
                }
                else {
                    entity->byte148 = 0;
                }

                if (entity->state == 2) {
                    if (keyDown.left) {
                        usePhysicalControls = true;
                        entity->zoneID      = 7;
                    }
                    if (keyDown.right) {
                        usePhysicalControls = true;
                        entity->zoneID      = 6;
                    }
                }
            }
            else {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.left && entity->zoneID > 0) {
                        PlaySfx(21, 0);
                        entity->zoneID--;
                        if (entity->zoneID < (entity->pageID * 3) && entity->zoneID > 0) {
                            entity->pageID--;
                            entity->state    = 5;
                            entity->float134 = -72.0f - (720.0f * (entity->pageID));
                            entity->dword12C = entity->zoneID;
                            entity->zoneID   = 3;
                        }
                    }
                    else if (keyPress.right && entity->zoneID < timeAttack_ZoneCount - 1) {
                        PlaySfx(21, 0);
                        ++entity->zoneID;
                        if (entity->zoneID >= ((entity->pageID + 1) * 3)) {
                            entity->pageID++;
                            entity->state    = 5;
                            entity->float134 = -72.0f - (720.0f * (entity->pageID));
                            entity->dword12C = entity->zoneID;
                            entity->zoneID   = 4;
                        }
                    }

                    for (int i = 0; i < timeAttack_ZoneCount; ++i) entity->zoneButtons[i]->state = 0;
                    if (entity->state == 2) {
                        entity->zoneButtons[entity->zoneID]->state = 1;
                        if (entity->zoneButtons[entity->zoneID]->flag && (keyPress.start || keyPress.A)) {
                            PlaySfx(22, 0);
                            entity->zoneButtons[entity->zoneID]->state = 2;
                            entity->state                              = 6;
                        }
                    }
                }
            }

            if (entity->menuControl->state == 5) {
                entity->state = 10;
            }
            break;
        }
        case 5: {
            entity->pagePrevPressed = false;
            entity->pageNextPressed = false;
            entity->zoneButtons[0]->textX += ((entity->float134 - entity->zoneButtons[0]->textX) / ((60.0 * Engine.deltaTime) * 6.0));
            entity->timer += (Engine.deltaTime * 1.5);
            if (entity->timer > 1.0) {
                entity->timer                 = 0.0;
                entity->state                 = 2;
                entity->zoneButtons[0]->textX = entity->float134;
                if (usePhysicalControls)
                    entity->zoneID = entity->dword12C;
            }

            float x = entity->zoneButtons[0]->textX + 144.0;
            for (int i = 1; i < timeAttack_ZoneCount; ++i) {
                entity->zoneButtons[i]->textX = x;
                if (!((i + 1) % 3))
                    x += 432.0;
                else
                    x += 144.0;
            }
            break;
        }
        case 6: {
            if (!entity->zoneButtons[entity->zoneID]->state) {
                entity->state   = 7;
                entity->float2C = 0.0;
                entity->float44 = 0.02;
                entity->float28 = DegreesToRad(-90.0);
                entity->float40 = DegreesToRad(-90.0);
            }
            entity->menuControl->state = 2;
            break;
        }
        case 7: {
            if (entity->pagePrevAlpha > 0)
                entity->pagePrevAlpha -= 32;
            if (entity->pagePrevAlpha > 0)
                entity->pageNextAlpha -= 32;

            entity->translateY += ((-512.0 - entity->translateY) / ((Engine.deltaTime * 60.0) * 16.0));

            matrixRotateXYZF(&entity->matrix1, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
            matrixTranslateXYZF(&entity->matrix2, 0.0, entity->translateY, 80.0);
            matrixMultiplyF(&entity->matrix1, &entity->matrix2);

            if (entity->float24 > entity->float28) {
                entity->float2C -= (0.0025 * (60.0 * Engine.deltaTime));
                entity->float24 += ((60.0 * Engine.deltaTime) * entity->float2C);
                entity->float2C -= (0.0025 * (60.0 * Engine.deltaTime));
                matrixRotateYF(&entity->labelPtr->renderMatrix, entity->float24);
                matrixTranslateXYZF(&entity->matrix2, -128.0, 80.0, 160.0);
                matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrix2);
            }

            if (entity->float3C > entity->float40) {
                entity->float44 -= (0.0025 * (60.0 * Engine.deltaTime));
                if (entity->float44 < 0.0)
                    entity->float3C += ((60.0 * Engine.deltaTime) * entity->float44);

                entity->float44 -= (0.0025 * (60.0 * Engine.deltaTime));
                matrixRotateYF(&entity->button->matrix, entity->float3C);
                matrixTranslateXYZF(&entity->matrix2, -128.0, 48.0, 160.0);
                matrixMultiplyF(&entity->button->matrix, &entity->matrix2);
            }

            if (entity->float40 >= entity->float3C) {
                entity->state   = 8;
                entity->float2C = 0.0;
                entity->float44 = -0.02;
                entity->float28 = DegreesToRad(22.5);
                entity->float40 = DegreesToRad(16.0);

                entity->recordsScreen = CREATE_ENTITY(RecordsScreen);
                entity->recordsScreen->timeAttack = entity;
                entity->recordsScreen->zoneID     = entity->zoneID;
            }

            NativeEntity_AchievementsButton *button = entity->menuControl->buttons[entity->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = entity->menuControl->backButton;
            float div                               = (60.0 * Engine.deltaTime) * 16.0;
            button->translateX += ((512.0 - button->translateX) / div);
            backButton->translateX += ((1024.0 - button->translateX) / div);
            break;
        }
        case 9: {
            if (entity->float28 > entity->float24) {
                entity->float2C += 0.0025 * (Engine.deltaTime * 60.0);
                entity->float24 += ((Engine.deltaTime * 60.0) * entity->float2C);
                if (entity->float24 > entity->float28)
                    entity->float24 = entity->float28;

                matrixRotateYF(&entity->labelPtr->renderMatrix, entity->float24);
                matrixTranslateXYZF(&entity->matrix2, -128.0, 80.0, 160.0);
                matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrix2);
            }

            entity->translateY += ((-38.0 - entity->translateY) / ((60.0 * Engine.deltaTime) * 16.0));
            matrixRotateXYZF(&entity->matrix1, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
            matrixTranslateXYZF(&entity->matrix2, 0.0, entity->translateY, 80.0);
            matrixMultiplyF(&entity->matrix1, &entity->matrix2);

            if (entity->float40 > entity->float3C) {
                entity->float44 += (0.0025 * (60.0 * Engine.deltaTime));
                if (entity->float44 > 0.0) {
                    entity->float3C += ((60.0 * Engine.deltaTime) * entity->float44);
                }
                entity->float44 += (0.0025 * (60.0 * Engine.deltaTime));
                if (entity->float3C > entity->float40)
                    entity->float3C = entity->float40;

                matrixRotateYF(&entity->button->matrix, entity->float3C);
                matrixTranslateXYZF(&entity->matrix2, -128.0, 48.0, 160.0);
                matrixMultiplyF(&entity->button->matrix, &entity->matrix2);
            }

            NativeEntity_AchievementsButton *button = entity->menuControl->buttons[entity->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = entity->menuControl->backButton;

            button->translateX += ((112.0 - button->translateX) / ((60.0 * Engine.deltaTime) * 16.0));
            backButton->translateX += ((230.0 - backButton->translateX) / ((60.0 * Engine.deltaTime) * 16.0));
            if (backButton->translateX < SCREEN_YSIZE) {
                backButton->translateX = SCREEN_YSIZE;

                entity->state = 2;
                entity->menuControl->state = 4;
            }
            break;
        }
        case 10: {
            if (entity->pagePrevAlpha > 0)
                entity->pagePrevAlpha -= 32;
            if (entity->pageNextAlpha > 0)
                entity->pageNextAlpha -= 32;

            entity->timer += (Engine.deltaTime + Engine.deltaTime);
            entity->labelPtr->textWidth = (10.0 * (60.0 * Engine.deltaTime)) + entity->labelPtr->textWidth;
            entity->button->matXOff += (12.0 * (60.0 * Engine.deltaTime));
            entity->translateY += ((-512.0 - entity->translateY) / ((60.0 * Engine.deltaTime) * 16.0));

            matrixRotateXYZF(&entity->matrix1, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
            matrixTranslateXYZF(&entity->matrix2, 0.0, entity->translateY, 80.0);
            matrixMultiplyF(&entity->matrix1, &entity->matrix2);
            if (entity->timer > 1.0) {
                entity->timer = 0.0;
                RemoveNativeObject(entity->button);
                RemoveNativeObject(entity->labelPtr);
                for (int i = 0; i < timeAttack_ZoneCount; ++i) RemoveNativeObject(entity->zoneButtons[i]);
                RemoveNativeObject(entity);
            }
            break;
        }
        default: break;
    }

    
    SetRenderBlendMode(RENDER_BLEND_ALPHA);
    NewRenderState();
    SetRenderMatrix(&entity->matrix1);
    RenderRect(-SCREEN_XSIZE_F, 68.0, 160.0, 3.0 * SCREEN_XSIZE_F, 120.0, 0, 0, 0, 0xC0);

    if (entity->pagePrevPressed)
        RenderImageFlipH(-152.0, 12.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, entity->pagePrevAlpha, entity->textureArrows);
    else
        RenderImageFlipH(-152.0, 12.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, entity->pagePrevAlpha, entity->textureArrows);

    if (entity->pageNextPressed)
        RenderImage(300.0, 12.0, 160.0, 0.35, 0.35, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, entity->pageNextAlpha, entity->textureArrows);
    else
        RenderImage(300.0, 12.0, 160.0, 0.35, 0.35, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, entity->pageNextAlpha, entity->textureArrows);
}
