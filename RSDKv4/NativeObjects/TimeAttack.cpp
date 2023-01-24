#include "RetroEngine.hpp"

byte timeAttackTex;

int timeAttack_ZoneCount   = 11;
int timeAttack_ActCount    = 2;
int timeAttack_ExZoneCount = 2;

void TimeAttack_Create(void *objPtr)
{
    RSDK_THIS(TimeAttack);
    self->menuControl = (NativeEntity_MenuControl *)GetNativeObject(0);
    ReadSaveRAMData();
    SaveGame *saveGame = (SaveGame *)saveRAM;

    int actCount = 0;
    if (Engine.gameType == GAME_SONIC1) {
        timeAttack_ZoneCount   = 6;
        timeAttack_ActCount    = 3;
        timeAttack_ExZoneCount = 2;
        // GHZ-SBZ + FZ
        actCount = (timeAttack_ZoneCount * timeAttack_ActCount) + 1;
    }
    else {
        timeAttack_ZoneCount   = 11;
#if !RETRO_USE_ORIGINAL_CODE
        FileInfo info;
        if (LoadFile("Data/Stages/ZoneM/StageConfig.bin", &info)) {
            timeAttack_ZoneCount++;
            CloseFile();
        }
#endif
        timeAttack_ActCount    = 2;
        timeAttack_ExZoneCount = 0;
        actCount               = timeAttack_ZoneCount * timeAttack_ActCount;
    }

    bool saveRAMUpdated = false;
    // Regular Stages
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

    // Special Stages (S1 Only)
    if (Engine.gameType == GAME_SONIC1) {
        int offset            = actCount * 3;

        int specialStageCount = 6;
        for (int i = 0; i < specialStageCount * 3; i += 3) {
            // 1st
            if (!saveGame->records[offset + i]) {
                saveGame->records[offset + i] = 30000;
                saveRAMUpdated       = true;
            }

            // 2nd
            if (!saveGame->records[offset + i + 1]) {
                saveGame->records[offset + i + 1] = 30000;
                saveRAMUpdated           = true;
            }

            // 3rd
            if (!saveGame->records[offset + i + 2]) {
                saveGame->records[offset + i + 2] = 30000;
                saveRAMUpdated           = true;
            }
        }
    }

    if (saveRAMUpdated)
        WriteSaveRAMData();

    int pos = 0;
    float x = -72.0;
    for (int z = 0; z < timeAttack_ZoneCount; ++z) {
        NativeEntity_ZoneButton *zoneButton = CREATE_ENTITY(ZoneButton);
        self->zoneButtons[z]                = zoneButton;
        zoneButton->x                       = x;
        SetStringToFont(zoneButton->zoneText, strStageList[z], FONT_TEXT);

        self->totalTime = 0;
        if (Engine.gameType == GAME_SONIC1) {
            // Regular Stages (GHZ-SBZ)
            for (int a = 0; a < timeAttack_ActCount; ++a) self->totalTime += saveGame->records[3 * (pos + a)];
            pos += timeAttack_ActCount;
        }
        else {
            if (z == 7) { // metropolis
                for (int a = 0; a < 3; ++a) self->totalTime += saveGame->records[3 * (pos + a)];
                pos += 3;
            }
            else if (z < 8) {
                for (int a = 0; a < timeAttack_ActCount; ++a) self->totalTime += saveGame->records[3 * (pos + a)];
                pos += timeAttack_ActCount;
            }
            else {
                self->totalTime += saveGame->records[3 * pos];
                pos++;
            }
        }
        SetStringToFont8(self->zoneButtons[z]->timeText, "", FONT_TEXT);
        AddTimeStringToFont(self->zoneButtons[z]->timeText, self->totalTime, FONT_TEXT);
        self->zoneButtons[z]->textWidth = GetTextWidth(self->zoneButtons[z]->zoneText, FONT_TEXT, 0.25) * 0.5;

        if (!((z + 1) % 3))
            x += 432.0;
        else
            x += 144.0;
    }

    if (Engine.gameType == GAME_SONIC1) {
        // final zone
        int z                               = 6;
        NativeEntity_ZoneButton *zoneButton = CREATE_ENTITY(ZoneButton);
        self->zoneButtons[z]                = zoneButton;
        zoneButton->x                       = x;
        SetStringToFont(zoneButton->zoneText, strStageList[z], FONT_TEXT);

        self->totalTime = 0;
        self->totalTime += saveGame->records[3 * pos];
        pos++;
        SetStringToFont8(self->zoneButtons[z]->timeText, "", FONT_TEXT);
        AddTimeStringToFont(self->zoneButtons[z]->timeText, self->totalTime, FONT_TEXT);
        self->zoneButtons[z]->textWidth = GetTextWidth(self->zoneButtons[z]->zoneText, FONT_TEXT, 0.25) * 0.5;

        if (!((z + 1) % 3))
            x += 432.0;
        else
            x += 144.0;

        // special stages
        z                = 7;
        zoneButton           = CREATE_ENTITY(ZoneButton);
        self->zoneButtons[z] = zoneButton;
        zoneButton->x        = x;
        SetStringToFont(zoneButton->zoneText, strStageList[z], FONT_TEXT);

        self->totalTime = 0;
        for (int a = 0; a < 6; ++a) self->totalTime += saveGame->records[3 * (pos + a)];
        pos += 6;
        SetStringToFont8(self->zoneButtons[z]->timeText, "", FONT_TEXT);
        AddTimeStringToFont(self->zoneButtons[z]->timeText, self->totalTime, FONT_TEXT);
        self->zoneButtons[z]->textWidth = GetTextWidth(self->zoneButtons[z]->zoneText, FONT_TEXT, 0.25) * 0.5;
    }

    self->totalTime = 0;
    pos             = 0;
    for (int z = 0; z < timeAttack_ZoneCount; ++z) {
        if (Engine.gameType == GAME_SONIC1) {
            // Regular Stages (GHZ-SBZ)
            for (int a = 0; a < timeAttack_ActCount; ++a) self->totalTime += saveGame->records[3 * (pos + a)];
            pos += timeAttack_ActCount;
        }
        else {
            if (z == 7) { // metropolis
                for (int a = 0; a < 3; ++a) self->totalTime += saveGame->records[3 * (pos + a)];
                pos += 3;
            }
            else if (z < 8) {
                for (int a = 0; a < timeAttack_ActCount; ++a) self->totalTime += saveGame->records[3 * (pos + a)];
                pos += timeAttack_ActCount;
            }
            else {
                self->totalTime += saveGame->records[3 * pos];
                pos++;
            }
        }
    }

    if (Engine.gameType == GAME_SONIC1) {
        // final zone
        self->totalTime += saveGame->records[3 * pos];
        pos++;

        // special stages
        for (int a = 0; a < 6; ++a) self->totalTime += saveGame->records[3 * (pos + a)];
        pos += 6;
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
        self->zoneButtons[i]->texX     = tx;
        self->zoneButtons[i]->texY     = ty;
        self->zoneButtons[i]->unlocked = false;
        if (zone > timeAttack_ActCount * (i + 1)) {
            self->zoneButtons[i]->unlocked = true;
        }

        if (Engine.gameType == GAME_SONIC1) {
            if (i == 5) { // this sucks (final zone hack)
                tx += 320.0f;
                tx += 320.0f;
                ty += 240.0f;

                ++i;
                self->zoneButtons[i]->texX = tx;
                self->zoneButtons[i]->texY = ty;

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
                if (i == 11) // Boss Attack
                    self->zoneButtons[i]->unlocked = self->zoneButtons[i - 1]->unlocked;
                if (i == 10) // HPZ
                    self->zoneButtons[i]->unlocked = saveGame->unlockedHPZ;
            }
        }
    }

    if (Engine.gameType == GAME_SONIC1) {
        // final zone
        self->zoneButtons[6]->unlocked = false;
        if (zone > timeAttack_ActCount * 6) { // if listPos == final zone OR complete
            self->zoneButtons[6]->unlocked = true;
        }

        // special stages
        self->zoneButtons[7]->texX     = tx;
        self->zoneButtons[7]->texY     = ty;
        self->zoneButtons[7]->unlocked = false;
        if (zone > (timeAttack_ActCount * 6) + 1) { // if listPos == complete
            self->zoneButtons[7]->unlocked = true;
        }
    }

    self->y             = -400.0;
    self->zoneButtonVel = -72.0;
    MatrixRotateXYZF(&self->matRender, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
    MatrixTranslateXYZF(&self->matrixTemp, 0.0, -36.0, 240.0);
    MatrixMultiplyF(&self->matRender, &self->matrixTemp);
    MatrixInvertF(&self->matrixTouch, &self->matRender);
    MatrixRotateXYZF(&self->matRender, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
    MatrixTranslateXYZF(&self->matrixTemp, 0.0, self->y, 80.0);
    MatrixMultiplyF(&self->matRender, &self->matrixTemp);

    self->labelPtr         = CREATE_ENTITY(TextLabel);
    self->labelPtr->fontID = FONT_HEADING;
    self->labelPtr->scale  = 0.2;
    self->labelPtr->alpha  = 0;
    self->labelPtr->z      = 0;
    self->labelPtr->state  = TEXTLABEL_STATE_IDLE;
    SetStringToFont(self->labelPtr->text, strTimeAttack, FONT_HEADING);
    self->labelPtr->alignOffset = 512.0;
    self->rotationY             = DegreesToRad(22.5);
    MatrixRotateYF(&self->labelPtr->renderMatrix, self->rotationY);
    MatrixTranslateXYZF(&self->matrixTemp, -128.0, 80.0, 160.0);
    MatrixMultiplyF(&self->labelPtr->renderMatrix, &self->matrixTemp);
    self->labelPtr->useRenderMatrix = true;

    self->button          = CREATE_ENTITY(SubMenuButton);
    self->button->matXOff = 512.0;
    self->button->textY   = -4.0;
    self->button->matZ    = 0.0;
    self->button->scale   = 0.1;

    self->buttonRotationY = DegreesToRad(16.0);
    MatrixRotateYF(&self->button->matrix, self->buttonRotationY);
    MatrixTranslateXYZF(&self->matrixTemp, -128.0, 48.0, 160.0);
    MatrixMultiplyF(&self->button->matrix, &self->matrixTemp);
    self->button->useMatrix = true;
    SetStringToFont(self->button->text, strTotalTime, FONT_LABEL);
    AddTimeStringToFont(self->button->text, self->totalTime, FONT_LABEL);

    self->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);
    self->pagePrevAlpha = 0;
    self->pageNextAlpha = 0x100;
}

void TimeAttack_Main(void *objPtr)
{
    RSDK_THIS(TimeAttack);

    switch (self->state) {
        case TIMEATTACK_STATE_SETUP: {
            self->timer += Engine.deltaTime;
            if (self->timer > 1.0) {
                self->timer = 0.0;
                self->state = TIMEATTACK_STATE_ENTER;
            }
            break;
        }

        case TIMEATTACK_STATE_ENTER: {
            self->labelPtr->alignOffset /= (1.125 * (60.0 * Engine.deltaTime));
            self->timer += (Engine.deltaTime + Engine.deltaTime);
            self->labelPtr->alpha = (self->timer * 256.0);
            self->button->matXOff += ((-176.0 - self->button->matXOff) / ((60.0 * Engine.deltaTime) * 16.0));
            self->y += ((-36.0 - self->y) / ((60.0 * Engine.deltaTime) * 8.0));
            MatrixRotateXYZF(&self->matRender, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, self->y, 80.0);
            MatrixMultiplyF(&self->matRender, &self->matrixTemp);

            if (self->timer > 1.0) {
                self->timer      = 0.0;
                self->state      = TIMEATTACK_STATE_MAIN;
                keyPress.start = false;
                keyPress.A     = false;
            }
            break;
        }

        case TIMEATTACK_STATE_MAIN: {
            bool canPrev = 3 * (self->pageID - 1) >= 0;
            bool canNext = 3 * (self->pageID + 1) < (timeAttack_ZoneCount + timeAttack_ExZoneCount);

            if (!canNext) {
                if (self->pagePrevAlpha < 0x100)
                    self->pagePrevAlpha += 0x20;
                if (self->pageNextAlpha > 0)
                    self->pageNextAlpha -= 0x20;
            }
            else if (!canPrev) {
                if (self->pagePrevAlpha > 0)
                    self->pagePrevAlpha -= 0x20;
                if (self->pageNextAlpha < 0x100)
                    self->pageNextAlpha += 0x20;
            }
            else {
                if (self->pagePrevAlpha < 0x100)
                    self->pagePrevAlpha += 0x20;
                if (self->pageNextAlpha < 0x100)
                    self->pageNextAlpha += 0x20;
            }

            if (!usePhysicalControls) {
                if (touches <= 0) {
                    for (int i = 0; i < (timeAttack_ZoneCount + timeAttack_ExZoneCount); ++i) {
                        if (self->zoneButtons[i]->state == ZONEBUTTON_STATE_SELECTED) {
                            PlaySfxByName("Menu Select", false);
                            self->zoneButtons[i]->state = ZONEBUTTON_STATE_FLASHING;
                            self->zoneID                = i;
                            self->state                 = TIMEATTACK_STATE_ACTION;
                        }
                    }

                    if (self->pagePrevPressed && canPrev) {
                        self->pagePrevPressed = false;
                        PlaySfxByName("Menu Move", false);
                        self->state = TIMEATTACK_STATE_PAGECHANGE;
                        self->pageID--;
                        self->zoneButtonVel = -72.0f - (720.0f * self->pageID);
                        self->zoneID        = (3 * self->pageID) + 2;
                    }
                    if (self->pageNextPressed && canNext) {
                        self->pageNextPressed = false;
                        PlaySfxByName("Menu Move", false);
                        self->state = TIMEATTACK_STATE_PAGECHANGE;
                        self->pageID++;
                        self->zoneButtonVel    = -72.0f - (720.0f * self->pageID);
                        self->zoneID           = (3 * self->pageID);
                        self->selectionEnabled = 0;
                    }
                }
                else {
                    for (int i = 0; i < (timeAttack_ZoneCount + timeAttack_ExZoneCount); ++i)
                        self->zoneButtons[i]->state = ZONEBUTTON_STATE_UNSELECTED;

                    if (CheckTouchRectMatrix(&self->matrixTouch, 0.0, self->zoneButtons[0]->y + 16.0, 512.0, 40.0) >= 0) {
                        int offset = self->pageID * 3;

                        if (self->zoneButtons[offset]->unlocked && CheckTouchRect(-78.0, 0.0, 48.0, 120.0) >= 0)
                            self->zoneButtons[offset]->state = ZONEBUTTON_STATE_SELECTED;

                        if (offset + 1 < (timeAttack_ZoneCount + timeAttack_ExZoneCount)) {
                            if (self->zoneButtons[offset + 1]->unlocked && CheckTouchRect(22.0, 0.0, 38.0, 120.0) >= 0)
                                self->zoneButtons[offset + 1]->state = ZONEBUTTON_STATE_SELECTED;
                        }

                        if (offset + 2 < (timeAttack_ZoneCount + timeAttack_ExZoneCount)) {
                            if (self->zoneButtons[offset + 2]->unlocked && CheckTouchRect(100.0, 0.0, 30.0, 120.0) >= 0)
                                self->zoneButtons[offset + 2]->state = ZONEBUTTON_STATE_SELECTED;
                        }
                    }

                    self->pagePrevPressed = false;
                    self->pageNextPressed = false;
                    if (CheckTouchRect(-150.0, -40.0, 16.0, 24.0) >= 0 && canPrev) {
                        self->pagePrevPressed = true;
                    }
                    if (CheckTouchRect(144.0, -2.0, 12.0, 24.0) >= 0 && canNext) {
                        self->pageNextPressed = true;
                    }
                }

                if (touches > 0 && self->state == TIMEATTACK_STATE_MAIN) {
                    if (self->selectionEnabled) {
                        if ((self->lastTouchX - touchXF[0]) < -16.0f && canPrev) {
                            PlaySfxByName("Menu Move", false);
                            self->state = TIMEATTACK_STATE_PAGECHANGE;
                            self->pageID--;
                            self->zoneButtonVel    = -72.0f - (720.0f * self->pageID);
                            self->zoneID           = (3 * self->pageID) + 2;
                            self->selectionEnabled = false;

                            for (int i = 0; i < (timeAttack_ZoneCount + timeAttack_ExZoneCount); ++i)
                                self->zoneButtons[i]->state = ZONEBUTTON_STATE_UNSELECTED;
                        }
                        else if ((self->lastTouchX - touchXF[0]) > 16.0f && canNext) {
                            PlaySfxByName("Menu Move", false);
                            self->state = TIMEATTACK_STATE_PAGECHANGE;
                            self->pageID++;
                            self->zoneButtonVel    = -72.0f - (720.0f * self->pageID);
                            self->zoneID           = (3 * self->pageID);
                            self->selectionEnabled = false;
                            for (int i = 0; i < (timeAttack_ZoneCount + timeAttack_ExZoneCount); ++i)
                                self->zoneButtons[i]->state = ZONEBUTTON_STATE_UNSELECTED;
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

                if (self->state == TIMEATTACK_STATE_MAIN) {
                    if (keyDown.left) {
                        usePhysicalControls = true;
                        self->zoneID        = (3 * self->pageID);
                    }
                    if (keyDown.right) {
                        usePhysicalControls = true;
                        self->zoneID        = (3 * self->pageID) + 2;
                    }
                }
            }
            else {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.left && self->zoneID > 0) {
                        PlaySfxByName("Menu Move", false);
                        self->zoneID--;
                        if (self->zoneID < (self->pageID * 3) && self->zoneID > 0) {
                            self->pageID--;
                            self->state         = TIMEATTACK_STATE_PAGECHANGE;
                            self->zoneButtonVel = -72.0f - (720.0f * self->pageID);
                            self->storedZoneID  = self->zoneID;
                            self->zoneID        = (3 * self->pageID) + 2;
                        }
                    }
                    else if (keyPress.right && self->zoneID < (timeAttack_ZoneCount + timeAttack_ExZoneCount) - 1) {
                        PlaySfxByName("Menu Move", false);
                        ++self->zoneID;
                        if (self->zoneID >= ((self->pageID + 1) * 3)) {
                            self->pageID++;
                            self->state         = TIMEATTACK_STATE_PAGECHANGE;
                            self->zoneButtonVel = -72.0f - (720.0f * self->pageID);
                            self->storedZoneID  = self->zoneID;
                            self->zoneID        = (3 * self->pageID);
                        }
                    }

                    for (int i = 0; i < (timeAttack_ZoneCount + timeAttack_ExZoneCount); ++i)
                        self->zoneButtons[i]->state = ZONEBUTTON_STATE_UNSELECTED;
                    if (self->state == TIMEATTACK_STATE_MAIN) {
                        self->zoneButtons[self->zoneID]->state = ZONEBUTTON_STATE_SELECTED;
                        if (self->zoneButtons[self->zoneID]->unlocked && (keyPress.start || keyPress.A)) {
                            PlaySfxByName("Menu Select", false);
                            self->zoneButtons[self->zoneID]->state = ZONEBUTTON_STATE_FLASHING;
                            self->state                            = TIMEATTACK_STATE_ACTION;
                        }
                    }
                }
            }

            if (self->menuControl->state == MENUCONTROL_STATE_EXITSUBMENU) {
                self->state = TIMEATTACK_STATE_EXIT;
            }
            break;
        }

        case TIMEATTACK_STATE_PAGECHANGE: {
            self->pagePrevPressed = false;
            self->pageNextPressed = false;
            self->zoneButtons[0]->x += ((self->zoneButtonVel - self->zoneButtons[0]->x) / ((60.0 * Engine.deltaTime) * 6.0));
            self->timer += (Engine.deltaTime * 1.5);
            if (self->timer > 1.0) {
                self->timer             = 0.0;
                self->state             = TIMEATTACK_STATE_MAIN;
                self->zoneButtons[0]->x = self->zoneButtonVel;
                if (usePhysicalControls)
                    self->zoneID = self->storedZoneID;
            }

            float x = self->zoneButtons[0]->x + 144.0;
            for (int i = 1; i < (timeAttack_ZoneCount + timeAttack_ExZoneCount); ++i) {
                self->zoneButtons[i]->x = x;
                if (!((i + 1) % 3))
                    x += 432.0;
                else
                    x += 144.0;
            }
            break;
        }

        case TIMEATTACK_STATE_ACTION: {
            if (!self->zoneButtons[self->zoneID]->state) {
                self->state                   = TIMEATTACK_STATE_ENTERSUBMENU;
                self->rotationYVel            = 0.0;
                self->buttonRotationYVelocity = 0.02;
                self->targetRotationY         = DegreesToRad(-90.0);
                self->targetButtonRotationY   = DegreesToRad(-90.0);
            }
            self->menuControl->state = MENUCONTROL_STATE_NONE;
            break;
        }

        case TIMEATTACK_STATE_ENTERSUBMENU: {
            if (self->pagePrevAlpha > 0)
                self->pagePrevAlpha -= 32;
            if (self->pagePrevAlpha > 0)
                self->pageNextAlpha -= 32;

            self->y += ((-512.0 - self->y) / ((Engine.deltaTime * 60.0) * 16.0));

            MatrixRotateXYZF(&self->matRender, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, self->y, 80.0);
            MatrixMultiplyF(&self->matRender, &self->matrixTemp);

            if (self->rotationY > self->targetRotationY) {
                self->rotationYVel -= (0.0025 * (60.0 * Engine.deltaTime));
                self->rotationY += ((60.0 * Engine.deltaTime) * self->rotationYVel);

                self->rotationYVel -= (0.0025 * (60.0 * Engine.deltaTime));
                MatrixRotateYF(&self->labelPtr->renderMatrix, self->rotationY);
                MatrixTranslateXYZF(&self->matrixTemp, -128.0, 80.0, 160.0);
                MatrixMultiplyF(&self->labelPtr->renderMatrix, &self->matrixTemp);
            }

            if (self->buttonRotationY > self->targetButtonRotationY) {
                self->buttonRotationYVelocity -= (0.0025 * (60.0 * Engine.deltaTime));
                if (self->buttonRotationYVelocity < 0.0)
                    self->buttonRotationY += ((60.0 * Engine.deltaTime) * self->buttonRotationYVelocity);

                self->buttonRotationYVelocity -= (0.0025 * (60.0 * Engine.deltaTime));
                MatrixRotateYF(&self->button->matrix, self->buttonRotationY);
                MatrixTranslateXYZF(&self->matrixTemp, -128.0, 48.0, 160.0);
                MatrixMultiplyF(&self->button->matrix, &self->matrixTemp);
            }

            if (self->targetButtonRotationY >= self->buttonRotationY) {
                self->state                   = TIMEATTACK_STATE_SUBMENU;
                self->rotationYVel            = 0.0;
                self->buttonRotationYVelocity = -0.02;
                self->targetRotationY         = DegreesToRad(22.5);
                self->targetButtonRotationY   = DegreesToRad(16.0);

                self->recordsScreen             = CREATE_ENTITY(RecordsScreen);
                self->recordsScreen->timeAttack = self;
                self->recordsScreen->zoneID     = self->zoneID;
            }

            NativeEntity_AchievementsButton *button = self->menuControl->buttons[self->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = self->menuControl->backButton;
            float div                               = (60.0 * Engine.deltaTime) * 16.0;
            button->x += ((512.0 - button->x) / div);
            backButton->x += ((1024.0 - button->x) / div);
            break;
        }

        case TIMEATTACK_STATE_SUBMENU: break;

        case TIMEATTACK_STATE_EXITSUBMENU: {
            if (self->targetRotationY > self->rotationY) {
                self->rotationYVel += 0.0025 * (Engine.deltaTime * 60.0);
                self->rotationY += ((Engine.deltaTime * 60.0) * self->rotationYVel);
                if (self->rotationY > self->targetRotationY)
                    self->rotationY = self->targetRotationY;

                MatrixRotateYF(&self->labelPtr->renderMatrix, self->rotationY);
                MatrixTranslateXYZF(&self->matrixTemp, -128.0, 80.0, 160.0);
                MatrixMultiplyF(&self->labelPtr->renderMatrix, &self->matrixTemp);
            }

            self->y += ((-38.0 - self->y) / ((60.0 * Engine.deltaTime) * 16.0));
            MatrixRotateXYZF(&self->matRender, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, self->y, 80.0);
            MatrixMultiplyF(&self->matRender, &self->matrixTemp);

            if (self->targetButtonRotationY > self->buttonRotationY) {
                self->buttonRotationYVelocity += (0.0025 * (60.0 * Engine.deltaTime));
                if (self->buttonRotationYVelocity > 0.0) {
                    self->buttonRotationY += ((60.0 * Engine.deltaTime) * self->buttonRotationYVelocity);
                }
                self->buttonRotationYVelocity += (0.0025 * (60.0 * Engine.deltaTime));
                if (self->buttonRotationY > self->targetButtonRotationY)
                    self->buttonRotationY = self->targetButtonRotationY;

                MatrixRotateYF(&self->button->matrix, self->buttonRotationY);
                MatrixTranslateXYZF(&self->matrixTemp, -128.0, 48.0, 160.0);
                MatrixMultiplyF(&self->button->matrix, &self->matrixTemp);
            }

            NativeEntity_AchievementsButton *button = self->menuControl->buttons[self->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = self->menuControl->backButton;

            button->x += ((112.0 - button->x) / ((60.0 * Engine.deltaTime) * 16.0));
            backButton->x += ((230.0 - backButton->x) / ((60.0 * Engine.deltaTime) * 16.0));
            if (backButton->x < SCREEN_YSIZE) {
                backButton->x = SCREEN_YSIZE;

                self->state              = TIMEATTACK_STATE_MAIN;
                self->menuControl->state = MENUCONTROL_STATE_SUBMENU;
            }
            break;
        }
        case TIMEATTACK_STATE_EXIT: {
            if (self->pagePrevAlpha > 0)
                self->pagePrevAlpha -= 32;
            if (self->pageNextAlpha > 0)
                self->pageNextAlpha -= 32;

            self->timer += (Engine.deltaTime + Engine.deltaTime);
            self->labelPtr->alignOffset = (10.0 * (60.0 * Engine.deltaTime)) + self->labelPtr->alignOffset;
            self->button->matXOff += (12.0 * (60.0 * Engine.deltaTime));
            self->y += ((-512.0 - self->y) / ((60.0 * Engine.deltaTime) * 16.0));

            MatrixRotateXYZF(&self->matRender, 0.0, DegreesToRad(12.25), DegreesToRad(6.125));
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, self->y, 80.0);
            MatrixMultiplyF(&self->matRender, &self->matrixTemp);
            if (self->timer > 1.0) {
                self->timer = 0.0;
                RemoveNativeObject(self->button);
                RemoveNativeObject(self->labelPtr);
                for (int i = 0; i < (timeAttack_ZoneCount + timeAttack_ExZoneCount); ++i) RemoveNativeObject(self->zoneButtons[i]);
                RemoveNativeObject(self);
            }
            break;
        }
        default: break;
    }

    SetRenderBlendMode(RENDER_BLEND_ALPHA);
    NewRenderState();
    SetRenderMatrix(&self->matRender);
    RenderRect(-SCREEN_XSIZE_F, 68.0, 160.0, 3.0 * SCREEN_XSIZE_F, 120.0, 0, 0, 0, 0xC0);

    if (self->pagePrevPressed)
        RenderImageFlipH(-152.0, 12.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, self->pagePrevAlpha, self->textureArrows);
    else
        RenderImageFlipH(-152.0, 12.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, self->pagePrevAlpha, self->textureArrows);

    if (self->pageNextPressed)
        RenderImage(300.0, 12.0, 160.0, 0.35, 0.35, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, self->pageNextAlpha, self->textureArrows);
    else
        RenderImage(300.0, 12.0, 160.0, 0.35, 0.35, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, self->pageNextAlpha, self->textureArrows);
}
