#include "RetroEngine.hpp"

byte timeAttackTex;

int timeAttack_ZoneCount = 12;
int timeAttack_ActCount  = 2;

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
        // GHZ-SBZ + FZ & 6 SS
        actCount = (timeAttack_ZoneCount * timeAttack_ActCount) + 7;
    }
    else {
        timeAttack_ZoneCount = 12;
        timeAttack_ActCount  = 2;
        actCount             = timeAttack_ZoneCount * timeAttack_ActCount;
    }

    bool saveRAMUpdated = false;
    for (int i = 0; i < actCount * 3; i += 3) {
        // 1st
        if (!saveGame->records[i]) {
            saveGame->records[i] = 60000;
            saveRAMUpdated       = true;
        }

        // 2nd
        if (!saveGame->records[i + 1]) {
            saveGame->records[i + 1] = 60000;
            saveRAMUpdated           = true;
        }

        // 3rd
        if (!saveGame->records[i + 2]) {
            saveGame->records[i + 2] = 60000;
            saveRAMUpdated           = true;
        }
    }
    if (saveRAMUpdated)
        WriteSaveRAMData();

    int pos = 0;
    float x = -72.0;
    for (int z = 0; z < timeAttack_ZoneCount; ++z) {
        NativeEntity_ZoneButton *zoneButton = CREATE_ENTITY(ZoneButton);
        entity->zoneButtons[z]              = zoneButton;
        zoneButton->x                       = x;
        SetStringToFont(zoneButton->zoneText, strStageList[z], FONT_TEXT);

        entity->totalTime = 0;
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
            if (z == 7) { // metropolis
                for (int a = 0; a < 3; ++a) entity->totalTime += saveGame->records[3 * (pos + a)];
                pos += 3;
            }
            else if (z < 8) {
                for (int a = 0; a < timeAttack_ActCount; ++a) entity->totalTime += saveGame->records[3 * (pos + a)];
                pos += timeAttack_ActCount;
            }
            else {
                entity->totalTime += saveGame->records[3 * pos];
                pos++;
            }
        }
        SetStringToFont8(entity->zoneButtons[z]->timeText, "", FONT_TEXT);
        AddTimeStringToFont(entity->zoneButtons[z]->timeText, entity->totalTime, FONT_TEXT);
        entity->zoneButtons[z]->textWidth = GetTextWidth(entity->zoneButtons[z]->zoneText, FONT_TEXT, 0.25) * 0.5;

        if (!((z + 1) % 3))
            x += 432.0;
        else
            x += 144.0;
    }

    entity->totalTime = 0;
    pos               = 0;
    for (int z = 0; z < timeAttack_ZoneCount; ++z) {
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
            if (z == 7) { // metropolis
                for (int a = 0; a < 3; ++a) entity->totalTime += saveGame->records[3 * (pos + a)];
                pos += 3;
            }
            else if (z < 8) {
                for (int a = 0; a < timeAttack_ActCount; ++a) entity->totalTime += saveGame->records[3 * (pos + a)];
                pos += timeAttack_ActCount;
            }
            else {
                entity->totalTime += saveGame->records[3 * pos];
                pos++;
            }
        }
    }

    int zone = saveGame->unlockedActs;
    for (int i = 0; i < 4; ++i) {
        if (saveGame->files[i].stageID > zone)
            zone = saveGame->files[i].stageID;
    }
    saveGame->unlockedActs = zone;

    float tx = 480.0f;
    float ty = 120.0f;
    for (int i = 0; i < timeAttack_ZoneCount; ++i) {
        entity->zoneButtons[i]->texX     = tx;
        entity->zoneButtons[i]->texY     = ty;
        entity->zoneButtons[i]->unlocked = false;
        if (zone > timeAttack_ActCount * (i + 1)) {
            entity->zoneButtons[i]->unlocked = true;
        }

        if (Engine.gameType == GAME_SONIC1) {
            if (i == 5) { // this sucks (final zone hack)
                tx += 320.0f;
                tx += 320.0f;
                ty += 240.0f;

                entity->zoneButtons[i + 1]->texX = tx;
                entity->zoneButtons[i + 1]->texY = ty;

                entity->zoneButtons[i + 1]->unlocked = false;
                if (zone > (timeAttack_ActCount * (i + 1)) + 1) {
                    entity->zoneButtons[i + 1]->unlocked = true;
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

                // skip icon section
                if (i == 6) {
                    tx += 320.0f;
                    if (tx >= 960.0f) {
                        tx = 120.0f;
                        ty += 240.0f;
                    }
                }
            }
            else {
                if (i == 11)
                    entity->zoneButtons[i]->unlocked = entity->zoneButtons[i - 1]->unlocked;
                if (i == 10)
                    entity->zoneButtons[i]->unlocked = saveGame->unlockedHPZ;
            }
        }
    }

    entity->y        = -400.0;
    entity->float134 = -72.0;
    matrixRotateXYZF(&entity->matrix1, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
    matrixTranslateXYZF(&entity->matrixTemp, 0.0, -36.0, 240.0);
    matrixMultiplyF(&entity->matrix1, &entity->matrixTemp);
    matrixInvertF(&entity->matrixTouch, &entity->matrix1);
    matrixRotateXYZF(&entity->matrix1, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
    matrixTranslateXYZF(&entity->matrixTemp, 0.0, entity->y, 80.0);
    matrixMultiplyF(&entity->matrix1, &entity->matrixTemp);

    entity->labelPtr         = CREATE_ENTITY(TextLabel);
    entity->labelPtr->fontID = FONT_HEADING;
    entity->labelPtr->scale  = 0.2;
    entity->labelPtr->alpha  = 0;
    entity->labelPtr->z      = 0;
    entity->labelPtr->state  = TEXTLABEL_STATE_IDLE;
    SetStringToFont(entity->labelPtr->text, strTimeAttack, FONT_HEADING);
    entity->labelPtr->alignOffset = 512.0;
    entity->float24               = DegreesToRad(22.5);
    matrixRotateYF(&entity->labelPtr->renderMatrix, entity->float24);
    matrixTranslateXYZF(&entity->matrixTemp, -128.0, 80.0, 160.0);
    matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrixTemp);
    entity->labelPtr->useRenderMatrix = true;

    entity->button          = CREATE_ENTITY(SubMenuButton);
    entity->button->matXOff = 512.0;
    entity->button->textY   = -4.0;
    entity->button->matZ    = 0.0;
    entity->button->scale   = 0.1;

    entity->float3C = DegreesToRad(16.0);
    matrixRotateYF(&entity->button->matrix, entity->float3C);
    matrixTranslateXYZF(&entity->matrixTemp, -128.0, 48.0, 160.0);
    matrixMultiplyF(&entity->button->matrix, &entity->matrixTemp);
    entity->button->useMatrix = true;
    SetStringToFont(entity->button->text, strTotalTime, FONT_LABEL);
    AddTimeStringToFont(entity->button->text, entity->totalTime, FONT_LABEL);

    entity->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);
    entity->pagePrevAlpha = 0;
    entity->pageNextAlpha = 0x100;
}
void TimeAttack_Main(void *objPtr)
{
    RSDK_THIS(TimeAttack);
    switch (entity->state) {
        case TIMEATTACK_STATE_SETUP: {
            entity->timer += Engine.deltaTime;
            if (entity->timer > 1.0) {
                entity->timer = 0.0;
                entity->state = TIMEATTACK_STATE_ENTER;
            }
            break;
        }
        case TIMEATTACK_STATE_ENTER: {
            entity->labelPtr->alignOffset /= (1.125 * (60.0 * Engine.deltaTime));
            entity->timer += (Engine.deltaTime + Engine.deltaTime);
            entity->labelPtr->alpha = (entity->timer * 256.0);
            entity->button->matXOff += ((-176.0 - entity->button->matXOff) / ((60.0 * Engine.deltaTime) * 16.0));
            entity->y += ((-36.0 - entity->y) / ((60.0 * Engine.deltaTime) * 8.0));
            matrixRotateXYZF(&entity->matrix1, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, entity->y, 80.0);
            matrixMultiplyF(&entity->matrix1, &entity->matrixTemp);

            if (entity->timer > 1.0) {
                entity->timer  = 0.0;
                entity->state  = TIMEATTACK_STATE_MAIN;
                inputPress.start = false;
                inputPress.A     = false;
            }
            break;
        }
        case TIMEATTACK_STATE_MAIN: {
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
                        if (entity->zoneButtons[i]->state == ZONEBUTTON_STATE_SELECTED) {
                            PlaySfxByName("Menu Select", false);
                            entity->zoneButtons[i]->state = ZONEBUTTON_STATE_FLASHING;
                            entity->zoneID                = i;
                            entity->state                 = TIMEATTACK_STATE_ACTION;
                        }
                    }

                    if (entity->pagePrevPressed && canPrev) {
                        entity->pagePrevPressed = false;
                        PlaySfxByName("Menu Move", false);
                        entity->state = TIMEATTACK_STATE_PAGECHANGE;
                        entity->pageID--;
                        entity->float134 = -72.0f - (720.0f * entity->pageID);
                        entity->zoneID   = (3 * entity->pageID) + 2;
                    }
                    if (entity->pageNextPressed && canNext) {
                        entity->pageNextPressed = false;
                        PlaySfxByName("Menu Move", false);
                        entity->state = TIMEATTACK_STATE_PAGECHANGE;
                        entity->pageID++;
                        entity->float134 = -72.0f - (720.0f * entity->pageID);
                        entity->zoneID   = (3 * entity->pageID);
                        entity->byte148  = 0;
                    }
                }
                else {
                    for (int i = 0; i < timeAttack_ZoneCount; ++i) entity->zoneButtons[i]->state = ZONEBUTTON_STATE_UNSELECTED;

                    if (CheckTouchRectMatrix(&entity->matrixTouch, 0.0, entity->zoneButtons[0]->y + 16.0, 512.0, 40.0) >= 0) {
                        int offset = entity->pageID * 3;

                        if (entity->zoneButtons[offset]->unlocked && CheckTouchRect(-78.0, 0.0, 48.0, 120.0) >= 0)
                            entity->zoneButtons[offset]->state = ZONEBUTTON_STATE_SELECTED;

                        if (offset + 1 < timeAttack_ZoneCount) {
                            if (entity->zoneButtons[offset + 1]->unlocked && CheckTouchRect(22.0, 0.0, 38.0, 120.0) >= 0)
                                entity->zoneButtons[offset + 1]->state = ZONEBUTTON_STATE_SELECTED;
                        }

                        if (offset + 2 < timeAttack_ZoneCount) {
                            if (entity->zoneButtons[offset + 2]->unlocked && CheckTouchRect(100.0, 0.0, 30.0, 120.0) >= 0)
                                entity->zoneButtons[offset + 2]->state = ZONEBUTTON_STATE_SELECTED;
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

                if (touches > 0 && entity->state == TIMEATTACK_STATE_MAIN) {
                    if (entity->byte148) {
                        if ((entity->float144 - touchXF[0]) < -16.0f && canPrev) {
                            PlaySfxByName("Menu Move", false);
                            entity->state = TIMEATTACK_STATE_PAGECHANGE;
                            entity->pageID--;
                            entity->float134 = -72.0f - (720.0f * entity->pageID);
                            entity->zoneID   = (3 * entity->pageID) + 2;
                            entity->byte148  = false;
                            for (int i = 0; i < timeAttack_ZoneCount; ++i) entity->zoneButtons[i]->state = ZONEBUTTON_STATE_UNSELECTED;
                        }
                        else if ((entity->float144 - touchXF[0]) > 16.0f && canNext) {
                            PlaySfxByName("Menu Move", false);
                            entity->state = TIMEATTACK_STATE_PAGECHANGE;
                            entity->pageID++;
                            entity->float134 = -72.0f - (720.0f * entity->pageID);
                            entity->zoneID   = (3 * entity->pageID);
                            entity->byte148  = false;
                            for (int i = 0; i < timeAttack_ZoneCount; ++i) entity->zoneButtons[i]->state = ZONEBUTTON_STATE_UNSELECTED;
                        }
                    }
                    else {
                        entity->byte148 = true;
                    }
                    entity->float144 = touchXF[0];
                }
                else {
                    entity->byte148 = false;
                }

                if (entity->state == TIMEATTACK_STATE_MAIN) {
                    if (inputDown.left) {
                        usePhysicalControls = true;
                        entity->zoneID      = (3 * entity->pageID);
                    }
                    if (inputDown.right) {
                        usePhysicalControls = true;
                        entity->zoneID      = (3 * entity->pageID) + 2;
                    }
                }
            }
            else {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (inputPress.left && entity->zoneID > 0) {
                        PlaySfxByName("Menu Move", false);
                        entity->zoneID--;
                        if (entity->zoneID < (entity->pageID * 3) && entity->zoneID > 0) {
                            entity->pageID--;
                            entity->state        = TIMEATTACK_STATE_PAGECHANGE;
                            entity->float134     = -72.0f - (720.0f * entity->pageID);
                            entity->storedZoneID = entity->zoneID;
                            entity->zoneID       = (3 * entity->pageID) + 2;
                        }
                    }
                    else if (inputPress.right && entity->zoneID < timeAttack_ZoneCount - 1) {
                        PlaySfxByName("Menu Move", false);
                        ++entity->zoneID;
                        if (entity->zoneID >= ((entity->pageID + 1) * 3)) {
                            entity->pageID++;
                            entity->state        = TIMEATTACK_STATE_PAGECHANGE;
                            entity->float134     = -72.0f - (720.0f * entity->pageID);
                            entity->storedZoneID = entity->zoneID;
                            entity->zoneID       = (3 * entity->pageID);
                        }
                    }

                    for (int i = 0; i < timeAttack_ZoneCount; ++i) entity->zoneButtons[i]->state = ZONEBUTTON_STATE_UNSELECTED;
                    if (entity->state == TIMEATTACK_STATE_MAIN) {
                        entity->zoneButtons[entity->zoneID]->state = ZONEBUTTON_STATE_SELECTED;
                        if (entity->zoneButtons[entity->zoneID]->unlocked && (inputPress.start || inputPress.A)) {
                            PlaySfxByName("Menu Select", false);
                            entity->zoneButtons[entity->zoneID]->state = ZONEBUTTON_STATE_FLASHING;
                            entity->state                              = TIMEATTACK_STATE_ACTION;
                        }
                    }
                }
            }

            if (entity->menuControl->state == MENUCONTROL_STATE_EXITSUBMENU) {
                entity->state = TIMEATTACK_STATE_EXIT;
            }
            break;
        }
        case TIMEATTACK_STATE_PAGECHANGE: {
            entity->pagePrevPressed = false;
            entity->pageNextPressed = false;
            entity->zoneButtons[0]->x += ((entity->float134 - entity->zoneButtons[0]->x) / ((60.0 * Engine.deltaTime) * 6.0));
            entity->timer += (Engine.deltaTime * 1.5);
            if (entity->timer > 1.0) {
                entity->timer             = 0.0;
                entity->state             = TIMEATTACK_STATE_MAIN;
                entity->zoneButtons[0]->x = entity->float134;
                if (usePhysicalControls)
                    entity->zoneID = entity->storedZoneID;
            }

            float x = entity->zoneButtons[0]->x + 144.0;
            for (int i = 1; i < timeAttack_ZoneCount; ++i) {
                entity->zoneButtons[i]->x = x;
                if (!((i + 1) % 3))
                    x += 432.0;
                else
                    x += 144.0;
            }
            break;
        }
        case TIMEATTACK_STATE_ACTION: {
            if (!entity->zoneButtons[entity->zoneID]->state) {
                entity->state   = TIMEATTACK_STATE_ENTERSUBMENU;
                entity->float2C = 0.0;
                entity->float44 = 0.02;
                entity->float28 = DegreesToRad(-90.0);
                entity->float40 = DegreesToRad(-90.0);
            }
            entity->menuControl->state = MENUCONTROL_STATE_NONE;
            break;
        }
        case TIMEATTACK_STATE_ENTERSUBMENU: {
            if (entity->pagePrevAlpha > 0)
                entity->pagePrevAlpha -= 32;
            if (entity->pagePrevAlpha > 0)
                entity->pageNextAlpha -= 32;

            entity->y += ((-512.0 - entity->y) / ((Engine.deltaTime * 60.0) * 16.0));

            matrixRotateXYZF(&entity->matrix1, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, entity->y, 80.0);
            matrixMultiplyF(&entity->matrix1, &entity->matrixTemp);

            if (entity->float24 > entity->float28) {
                entity->float2C -= (0.0025 * (60.0 * Engine.deltaTime));
                entity->float24 += ((60.0 * Engine.deltaTime) * entity->float2C);
                entity->float2C -= (0.0025 * (60.0 * Engine.deltaTime));
                matrixRotateYF(&entity->labelPtr->renderMatrix, entity->float24);
                matrixTranslateXYZF(&entity->matrixTemp, -128.0, 80.0, 160.0);
                matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrixTemp);
            }

            if (entity->float3C > entity->float40) {
                entity->float44 -= (0.0025 * (60.0 * Engine.deltaTime));
                if (entity->float44 < 0.0)
                    entity->float3C += ((60.0 * Engine.deltaTime) * entity->float44);

                entity->float44 -= (0.0025 * (60.0 * Engine.deltaTime));
                matrixRotateYF(&entity->button->matrix, entity->float3C);
                matrixTranslateXYZF(&entity->matrixTemp, -128.0, 48.0, 160.0);
                matrixMultiplyF(&entity->button->matrix, &entity->matrixTemp);
            }

            if (entity->float40 >= entity->float3C) {
                entity->state   = TIMEATTACK_STATE_SUBMENU;
                entity->float2C = 0.0;
                entity->float44 = -0.02;
                entity->float28 = DegreesToRad(22.5);
                entity->float40 = DegreesToRad(16.0);

                entity->recordsScreen             = CREATE_ENTITY(RecordsScreen);
                entity->recordsScreen->timeAttack = entity;
                entity->recordsScreen->zoneID     = entity->zoneID;
            }

            NativeEntity_AchievementsButton *button = entity->menuControl->buttons[entity->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = entity->menuControl->backButton;
            float div                               = (60.0 * Engine.deltaTime) * 16.0;
            button->x += ((512.0 - button->x) / div);
            backButton->x += ((1024.0 - button->x) / div);
            break;
        }
        case TIMEATTACK_STATE_SUBMENU: break;
        case TIMEATTACK_STATE_EXITSUBMENU: {
            if (entity->float28 > entity->float24) {
                entity->float2C += 0.0025 * (Engine.deltaTime * 60.0);
                entity->float24 += ((Engine.deltaTime * 60.0) * entity->float2C);
                if (entity->float24 > entity->float28)
                    entity->float24 = entity->float28;

                matrixRotateYF(&entity->labelPtr->renderMatrix, entity->float24);
                matrixTranslateXYZF(&entity->matrixTemp, -128.0, 80.0, 160.0);
                matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrixTemp);
            }

            entity->y += ((-38.0 - entity->y) / ((60.0 * Engine.deltaTime) * 16.0));
            matrixRotateXYZF(&entity->matrix1, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, entity->y, 80.0);
            matrixMultiplyF(&entity->matrix1, &entity->matrixTemp);

            if (entity->float40 > entity->float3C) {
                entity->float44 += (0.0025 * (60.0 * Engine.deltaTime));
                if (entity->float44 > 0.0) {
                    entity->float3C += ((60.0 * Engine.deltaTime) * entity->float44);
                }
                entity->float44 += (0.0025 * (60.0 * Engine.deltaTime));
                if (entity->float3C > entity->float40)
                    entity->float3C = entity->float40;

                matrixRotateYF(&entity->button->matrix, entity->float3C);
                matrixTranslateXYZF(&entity->matrixTemp, -128.0, 48.0, 160.0);
                matrixMultiplyF(&entity->button->matrix, &entity->matrixTemp);
            }

            NativeEntity_AchievementsButton *button = entity->menuControl->buttons[entity->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = entity->menuControl->backButton;

            button->x += ((112.0 - button->x) / ((60.0 * Engine.deltaTime) * 16.0));
            backButton->x += ((230.0 - backButton->x) / ((60.0 * Engine.deltaTime) * 16.0));
            if (backButton->x < SCREEN_YSIZE) {
                backButton->x = SCREEN_YSIZE;

                entity->state              = TIMEATTACK_STATE_MAIN;
                entity->menuControl->state = MENUCONTROL_STATE_SUBMENU;
            }
            break;
        }
        case TIMEATTACK_STATE_EXIT: {
            if (entity->pagePrevAlpha > 0)
                entity->pagePrevAlpha -= 32;
            if (entity->pageNextAlpha > 0)
                entity->pageNextAlpha -= 32;

            entity->timer += (Engine.deltaTime + Engine.deltaTime);
            entity->labelPtr->alignOffset = (10.0 * (60.0 * Engine.deltaTime)) + entity->labelPtr->alignOffset;
            entity->button->matXOff += (12.0 * (60.0 * Engine.deltaTime));
            entity->y += ((-512.0 - entity->y) / ((60.0 * Engine.deltaTime) * 16.0));

            matrixRotateXYZF(&entity->matrix1, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, entity->y, 80.0);
            matrixMultiplyF(&entity->matrix1, &entity->matrixTemp);
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
