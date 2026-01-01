#include "RetroEngine.hpp"

bool usePhysicalControls = false;

void MenuControl_Create(void *objPtr)
{
    RSDK_THIS(MenuControl);
    SetMusicTrack("MainMenu.ogg", 0, true, 106596);
    CREATE_ENTITY(MenuBG);

    self->buttons[self->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(StartGameButton);
    self->buttonFlags[self->buttonCount] = BUTTON_STARTGAME;
    self->buttonCount++;

    self->buttons[self->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(TimeAttackButton);
    self->buttonFlags[self->buttonCount] = BUTTON_TIMEATTACK;
    self->buttonCount++;
#if !RETRO_USE_V6
#if RETRO_USE_MOD_LOADER
    int vsID = GetSceneID(STAGELIST_PRESENTATION, "2P VS");
    if (vsID != -1) {
#else
    if (Engine.gameType == GAME_SONIC2) {
#endif
        self->buttons[self->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(MultiplayerButton);
        self->buttonFlags[self->buttonCount] = BUTTON_MULTIPLAYER;
        self->buttonCount++;
    }

    if (Engine.onlineActive) {
        self->buttons[self->buttonCount]     = CREATE_ENTITY(AchievementsButton);
        self->buttonFlags[self->buttonCount] = BUTTON_ACHIEVEMENTS;
        self->buttonCount++;

        self->buttons[self->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(LeaderboardsButton);
        self->buttonFlags[self->buttonCount] = BUTTON_LEADERBOARDS;
        self->buttonCount++;
    }

    self->buttons[self->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(OptionsButton);
    self->buttonFlags[self->buttonCount] = BUTTON_OPTIONS;
    self->buttonCount++;
#endif

    self->backButton          = CREATE_ENTITY(BackButton);
    #if RETRO_USE_V6
    self->backButton->visible = false;
    #else
    self->backButton->visible = true;
    #endif
    self->backButton->x       = 240.0;
    self->backButton->y       = -160.0;
    self->backButton->z       = 0.0;

    self->segaIDButton       = CREATE_ENTITY(SegaIDButton);
    self->segaIDButton->y    = -92.0;
    self->segaIDButton->texX = 0.0;
    self->segaIDButton->x    = SCREEN_CENTERX_F - 32.0;

    self->buttonIncline = 0.15707964f;  // this but less precise ---> M_PI / 2
    self->buttonSpacing = 0.078539819f; // this but less precise ---> M_PI / 4
    self->menuEndPos    = (self->buttonCount * self->buttonIncline) * 0.5;

    float offset = 0.0;
    for (int b = 0; b < self->buttonCount; ++b) {
        NativeEntity_AchievementsButton *button = self->buttons[b];
        float sin                               = sinf(self->buttonMovePos + offset);
        float cos                               = cosf(self->buttonMovePos + offset);
        button->x                               = 1024.0 * sin;
        button->z                               = (cos * -512.0) + 672.0;
        button->y                               = (128.0 * sin) + 16.0;
        button->visible                         = button->z <= 288.0;
        offset += self->buttonIncline;
    }

    PlayMusic(0, 0);
    if (Engine.gameDeviceType == RETRO_STANDARD)
        usePhysicalControls = true;
    BackupNativeObjects();
    #if RETRO_USE_V6 //I had to do this because resetting to this native object wasnt fully replicating the behavior
    self->state = MENUCONTROL_STATE_ACTION;
    #endif
}
void MenuControl_Main(void *objPtr)
{
    RSDK_THIS(MenuControl);
    NativeEntity_SegaIDButton *segaIDButton = self->segaIDButton;
    NativeEntity_BackButton *backButton     = self->backButton;

    switch (self->state) {
        case MENUCONTROL_STATE_MAIN: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);

            if (segaIDButton->alpha < 0x100 && Engine.language != RETRO_JP && !(Engine.language == RETRO_ZH || Engine.language == RETRO_ZS)
                && Engine.gameDeviceType == RETRO_MOBILE)
                segaIDButton->alpha += 8;

            if (!usePhysicalControls) {
                switch (self->stateInput) {
                    case MENUCONTROL_STATEINPUT_CHECKTOUCH: {
                        if (touches > 0) {
                            if (!keyDown.left && !keyDown.right) {
                                segaIDButton->state = SEGAIDBUTTON_STATE_IDLE;
                                if (CheckTouchRect(0.0, 16.0, 56.0, 56.0) >= 0) {
                                    BackupNativeObjects();
                                    self->releaseTouchX              = touchXF[0];
                                    self->stateInput                 = MENUCONTROL_STATEINPUT_HANDLERELEASE;
                                    self->buttonID                   = ceilf(self->buttonMovePos / -self->buttonSpacing);
                                    self->buttons[self->buttonID]->g = 0xC0;
                                }
                                else {
                                    if (CheckTouchRect(self->segaIDButton->x, self->segaIDButton->y, 20.0, 20.0) >= 0 && segaIDButton->alpha > 64) {
                                        segaIDButton->state = SEGAIDBUTTON_STATE_PRESSED;
                                    }
                                    else {
                                        self->stateInput            = MENUCONTROL_STATEINPUT_HANDLEDRAG;
                                        self->lastDragTouchDistance = 0.0;
                                        self->dragTouchX            = touchXF[0];
                                        self->lastButtonMovePos     = self->buttonMovePos;
                                    }
                                }
                            }
                            else {
                                segaIDButton->state              = SEGAIDBUTTON_STATE_IDLE;
                                usePhysicalControls              = true;
                                self->buttonID                   = ceilf(self->buttonMovePos / -self->buttonSpacing);
                                self->buttons[self->buttonID]->g = 0xC0;
                            }
                        }
                        else if (segaIDButton->state == SEGAIDBUTTON_STATE_PRESSED) {
                            segaIDButton->state = SEGAIDBUTTON_STATE_IDLE;
                            PlaySfxByName("Menu Select", false);
                            PlaySfxByName("Select", false);
                            ShowPromoPopup(0, "MoreGames");
                        }
                        else if (keyDown.left || keyDown.right) {
                            segaIDButton->state              = SEGAIDBUTTON_STATE_IDLE;
                            usePhysicalControls              = true;
                            self->buttonID                   = ceilf(self->buttonMovePos / -self->buttonSpacing);
                            self->buttons[self->buttonID]->g = 0xC0;
                        }
                        break;
                    }

                    case MENUCONTROL_STATEINPUT_HANDLEDRAG: {
                        if (touches <= 0) {
                            self->stateInput = MENUCONTROL_STATEINPUT_HANDLEMOVEMENT;
                        }
                        else {
                            self->autoButtonMoveVelocity = 0.0;
                            self->dragTouchDistance      = (self->dragTouchX - touchXF[0]) * -0.0007;
                            if (abs(self->lastDragTouchDistance) > 0.0) {
                                self->autoButtonMoveVelocity = self->dragTouchDistance - self->lastDragTouchDistance;
                                self->buttonMovePos += self->autoButtonMoveVelocity;
                            }
                            self->lastDragTouchDistance = self->dragTouchDistance;
                        }
                        break;
                    }

                    case MENUCONTROL_STATEINPUT_HANDLEMOVEMENT: {
                        self->autoButtonMoveVelocity /= (1.125 * (60.0 * Engine.deltaTime));
                        self->buttonMovePos += self->autoButtonMoveVelocity;
                        float max = -(self->menuEndPos - self->buttonSpacing);

                        if (max - 0.05 > self->buttonMovePos || self->buttonMovePos > 0.05)
                            self->autoButtonMoveVelocity = 0.0;

                        if (abs(self->autoButtonMoveVelocity) < 0.0025) {
                            if (self->buttonMovePos == self->lastButtonMovePos && self->dragTouchX < 0.0) {
                                self->buttonMovePos += 0.00001;
                            }

                            if (self->buttonMovePos <= self->lastButtonMovePos) {
                                self->targetButtonMovePos = floorf(self->buttonMovePos / self->buttonSpacing) * self->buttonSpacing;

                                if (self->targetButtonMovePos > self->lastButtonMovePos - self->buttonSpacing)
                                    self->targetButtonMovePos = self->lastButtonMovePos - self->buttonSpacing;

                                if (self->targetButtonMovePos < max)
                                    self->targetButtonMovePos = max;
                            }
                            else {
                                self->targetButtonMovePos = ceilf(self->buttonMovePos / self->buttonSpacing) * self->buttonSpacing;

                                if (self->targetButtonMovePos < self->buttonSpacing + self->lastButtonMovePos)
                                    self->targetButtonMovePos = self->buttonSpacing + self->lastButtonMovePos;

                                if (self->targetButtonMovePos > 0.0)
                                    self->targetButtonMovePos = 0.0;
                            }

                            self->stateInput = MENUCONTROL_STATEINPUT_MOVE;
                            self->buttonMovePos += (self->targetButtonMovePos - self->buttonMovePos) / ((60.0 * Engine.deltaTime) * 8.0);
                        }
                        break;
                    }

                    case MENUCONTROL_STATEINPUT_MOVE: {
                        if (touches > 0) {
                            self->stateInput            = MENUCONTROL_STATEINPUT_HANDLEDRAG;
                            self->lastDragTouchDistance = 0.0;
                            self->dragTouchX            = touchXF[0];
                        }
                        else {
                            self->buttonMovePos += (self->targetButtonMovePos - self->buttonMovePos) / ((60.0 * Engine.deltaTime) * 6.0);
                            if (abs(self->targetButtonMovePos - self->buttonMovePos) < 0.00025) {
                                self->buttonMovePos = self->targetButtonMovePos;
                                self->stateInput    = MENUCONTROL_STATEINPUT_CHECKTOUCH;
                            }
                        }
                        break;
                    }

                    case MENUCONTROL_STATEINPUT_HANDLERELEASE: {
                        if (touches > 0) {
                            if (CheckTouchRect(0.0, 16.0, 56.0, 56.0) < 0) {
                                self->buttons[self->buttonID]->g = 0xFF;
                            }
                            else {
                                self->buttons[self->buttonID]->g = 0xC0;
                                if (abs(self->releaseTouchX - touchXF[0]) > 8.0f) {
                                    self->stateInput                 = MENUCONTROL_STATEINPUT_HANDLEDRAG;
                                    self->dragTouchX                 = self->buttonID;
                                    self->lastDragTouchDistance      = 0.0;
                                    self->lastButtonMovePos          = self->buttonMovePos;
                                    self->buttons[self->buttonID]->g = 0xFF;
                                }
                            }
                        }
                        else {
                            if (self->buttons[self->buttonID]->g == 0xC0) {
                                self->buttons[self->buttonID]->labelPtr->state = TEXTLABEL_STATE_BLINK_FAST;
                                self->timer                                    = 0.0;
                                self->state                                    = MENUCONTROL_STATE_ACTION;
                                PlaySfxByName("Menu Select", false);
                                PlaySfxByName("Select", false);
                            }
                            self->buttons[self->buttonID]->g = 0xFF;
                            self->stateInput                 = MENUCONTROL_STATEINPUT_CHECKTOUCH;
                        }
                        break;
                    }

                    default: break;
                }
            }
            else {
                if (self->stateInput == MENUCONTROL_STATEINPUT_HANDLEDRAG) {
                    self->buttonMovePos +=
                        (((self->buttonMoveVelocity + self->targetButtonMovePos) - self->buttonMovePos) / ((60.0 * Engine.deltaTime) * 8.0));

                    if (abs(self->targetButtonMovePos - self->buttonMovePos) < 0.001) {
                        self->buttonMovePos = self->targetButtonMovePos;
                        self->stateInput    = MENUCONTROL_STATEINPUT_CHECKTOUCH;
                    }
                }
                else {
                    if (touches <= 0) {
                        if (keyPress.right && self->buttonMovePos > -(self->menuEndPos - self->buttonSpacing)) {
                            self->stateInput = MENUCONTROL_STATEINPUT_HANDLEDRAG;
                            self->targetButtonMovePos -= self->buttonSpacing;
                            PlaySfxByName("Menu Move", false);
                            PlaySfxByName("MenuButton", false);
                            self->buttonMoveVelocity = -0.01;
                            self->buttonID++;
                            if (self->buttonID >= self->buttonCount)
                                self->buttonID = self->buttonCount - 1;
                        }
                        else if (keyPress.left && self->buttonMovePos < 0.0) {
                            self->stateInput = MENUCONTROL_STATEINPUT_HANDLEDRAG;
                            self->targetButtonMovePos += self->buttonSpacing;
                            PlaySfxByName("Menu Move", false);
                            PlaySfxByName("MenuButton", false);
                            self->buttonMoveVelocity = 0.01;
                            self->buttonID--;
                            if (self->buttonID > self->buttonCount)
                                self->buttonID = 0;
                        }
                        else if ((keyPress.start || keyPress.A) && !Engine.nativeMenuFadeIn) {
                            BackupNativeObjects();
                            self->buttons[self->buttonID]->labelPtr->state = TEXTLABEL_STATE_BLINK_FAST;
                            self->timer                                    = 0.0;
                            self->state                                    = MENUCONTROL_STATE_ACTION;
                            PlaySfxByName("Menu Select", false);
                            PlaySfxByName("Select", false);
                        }

                        for (int i = 0; i < self->buttonCount; ++i) {
                            self->buttons[i]->g = 0xFF;
                        }
                        self->buttons[self->buttonID]->g = 0xC0;
                    }
                    else {
                        usePhysicalControls = false;
                        for (int i = 0; i < self->buttonCount; ++i) {
                            self->buttons[i]->g = 0xFF;
                        }
                    }
                }
            }

            float offset = self->buttonMovePos;
            for (int i = 0; i < self->buttonCount; ++i) {
                NativeEntity_AchievementsButton *button = self->buttons[i];
                button->x                               = 1024.0 * sinf(self->buttonMovePos + offset);
                button->y                               = (sinf(self->buttonMovePos + offset) * 128.0) + 16.0;
                button->z                               = (cosf(self->buttonMovePos + offset) * -512.0) + 672.0;
                button->visible                         = button->z <= 288.0;
                offset += self->buttonIncline;
            }

            if (!self->stateInput) {
                if (self->dialogTimer) {
                    self->dialogTimer--;
                }
                else if (keyPress.B && !Engine.nativeMenuFadeIn) {
                    self->dialog = CREATE_ENTITY(DialogPanel);
                    SetStringToFont(self->dialog->text, strExitGame, FONT_TEXT);
                    self->state = MENUCONTROL_STATE_DIALOGWAIT;
                    PlaySfxByName("Resume", false);
                }
            }
            break;
        }

        case MENUCONTROL_STATE_ACTION: {
            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                self->timer                             = 0.0;
                NativeEntity_AchievementsButton *button = self->buttons[self->buttonID];
                switch (self->buttonFlags[self->buttonID]) {
                    case BUTTON_STARTGAME:
                        self->state                                    = MENUCONTROL_STATE_ENTERSUBMENU;
                        self->autoButtonMoveVelocity                   = 0.0;
                        button->g                                      = 0xFF;
                        self->buttons[self->buttonID]->labelPtr->state = TEXTLABEL_STATE_NONE;
                        #if RETRO_USE_V6
                        self->backButton->visible     = false;
                        #else
                        self->backButton->visible    = true;
                        #endif
                        SetGlobalVariableByName("options.vsMode", false);
                        CREATE_ENTITY(SaveSelect);
                        break;

                    case BUTTON_TIMEATTACK:
                    #if !RETRO_USE_V6
                        self->state                  = MENUCONTROL_STATE_ENTERSUBMENU;
                        self->autoButtonMoveVelocity = 0.0;
                        button->g                    = 0xFF;
                        button->labelPtr->state      = TEXTLABEL_STATE_NONE;
                        self->backButton->visible    = true;
                        CREATE_ENTITY(TimeAttack);
                    #else
                        
                        if (Engine.gameType == GAME_SONICCD){
                            //it also sets these variables
                            SetGlobalVariableByName("options.gameMode",2);
                            SetGlobalVariableByName("options.saveSlot",0);
                            SetGlobalVariableByName("player.lives",1);
                            SetGlobalVariableByName("player.score",0);
                            SetGlobalVariableByName("player.scoreBonus",50000);
                            SetGlobalVariableByName("specialStage.listPos",0);
                            SetGlobalVariableByName("specialStage.emeralds",0);
                            SetGlobalVariableByName("specialStage.timeStones",0);
                            SetGlobalVariableByName("specialStage.nextZone",0);
                            SetGlobalVariableByName("timeAttack.round",0xffffffff);
                            SetGlobalVariableByName("timeAttack.result",0);
                            SetGlobalVariableByName("timeAttack.zone",0);
                            SetGlobalVariableByName("lampPostID",0);
                            SetGlobalVariableByName("starPostID",0);
                            SetGlobalVariableByName("goodFuture.list",0);
                            SetGlobalVariableByName("metalSonic.list",0);
                            //the v6 engine uses this function, which ive neatly ported
                            InitStartingStageMode(STAGELIST_PRESENTATION, 2);
                            CreateNativeObject(FadeScreen_Create, FadeScreen_Main);
                        }
                        else{
                            CREATE_ENTITY(TimeAttack);
                            self->state                  = MENUCONTROL_STATE_ENTERSUBMENU;
                            self->autoButtonMoveVelocity = 0.0;
                            button->g                    = 0xFF;
                            button->labelPtr->state      = TEXTLABEL_STATE_NONE;
                            #if RETRO_USE_V6
                            self->backButton->visible    = false;
                            #else
                            self->backButton->visible    = true;
                            #endif
                        }
                    #endif
                        break;

                    case BUTTON_MULTIPLAYER:
                        self->state             = MENUCONTROL_STATE_MAIN;
                        button->labelPtr->state = TEXTLABEL_STATE_IDLE;
                        SetGlobalVariableByName("options.saveSlot", 0);
                        SetGlobalVariableByName("options.gameMode", 0);
                        SetGlobalVariableByName("options.vsMode", 0);
                        SetGlobalVariableByName("player.lives", 3);
                        SetGlobalVariableByName("player.score", 0);
                        SetGlobalVariableByName("player.scoreBonus", 50000);
                        SetGlobalVariableByName("specialStage.listPos", 0);
                        SetGlobalVariableByName("specialStage.emeralds", 0);
                        SetGlobalVariableByName("specialStage.nextZone", 0);
                        SetGlobalVariableByName("timeAttack.result", 0);
                        SetGlobalVariableByName("lampPostID", 0);
                        SetGlobalVariableByName("starPostID", 0);
                        if (Engine.onlineActive) {
#if !RETRO_USE_ORIGINAL_CODE
                            BackupNativeObjects();
                            int id = GetSceneID(STAGELIST_PRESENTATION, "2P VS");
                            if (id == -1)
                                id = 3;
                            InitStartingStage(STAGELIST_PRESENTATION, id, 0);
#else
                            InitStartingStage(STAGELIST_PRESENTATION, 3, 0);
#endif
                            CREATE_ENTITY(FadeScreen);
                        }
                        else {
                            self->dialog              = CREATE_ENTITY(DialogPanel);
                            self->dialog->buttonCount = DLGTYPE_OK;
                            SetStringToFont(self->dialog->text, strNetworkMessage, FONT_TEXT);
                            self->state = MENUCONTROL_STATE_DIALOGWAIT;
                        }
                        break;

                    case BUTTON_ACHIEVEMENTS:
                        if (Engine.onlineActive && false) {
                            ShowAchievementsScreen();
                        }
                        else {
                            self->state               = MENUCONTROL_STATE_MAIN;
                            self->dialog              = CREATE_ENTITY(DialogPanel);
                            self->dialog->buttonCount = DLGTYPE_OK;
                            SetStringToFont(self->dialog->text, strNetworkMessage, FONT_TEXT);
                            self->state = MENUCONTROL_STATE_DIALOGWAIT;
                        }
                        button->labelPtr->state = TEXTLABEL_STATE_IDLE;
                        break;

                    case BUTTON_LEADERBOARDS:
                        self->state = MENUCONTROL_STATE_MAIN;
                        if (Engine.onlineActive && false) {
                            ShowLeaderboardsScreen();
                        }
                        else {
                            self->dialog              = CREATE_ENTITY(DialogPanel);
                            self->dialog->buttonCount = DLGTYPE_OK;
                            SetStringToFont(self->dialog->text, strNetworkMessage, FONT_TEXT);
                            self->state = MENUCONTROL_STATE_DIALOGWAIT;
                        }
                        button->labelPtr->state = TEXTLABEL_STATE_IDLE;
                        break;

                    case BUTTON_OPTIONS:
                        self->state                  = MENUCONTROL_STATE_ENTERSUBMENU;
                        self->autoButtonMoveVelocity = 0.0;
                        button->g                    = 0xFF;
                        button->labelPtr->state      = TEXTLABEL_STATE_NONE;
                        #if RETRO_USE_V6
                        self->backButton->visible    = false;
                        #else
                        self->backButton->visible    = true;
                        #endif
                        CREATE_ENTITY(OptionsMenu);
                        break;

                    default:
                        self->state             = MENUCONTROL_STATE_MAIN;
                        button->labelPtr->state = TEXTLABEL_STATE_IDLE;
                        break;
                }
            }
            break;
        }

        case MENUCONTROL_STATE_NONE: break;

        case MENUCONTROL_STATE_ENTERSUBMENU: {
            if (segaIDButton->alpha > 0)
                segaIDButton->alpha -= 8;

            self->autoButtonMoveVelocity -= 0.125 * (60.0 * Engine.deltaTime);

            for (int i = 0; i < self->buttonCount; ++i) {
                if (self->buttonID != i) {
                    if (self->buttonID != i)
                        self->buttons[i]->z += ((60.0 * Engine.deltaTime) * self->autoButtonMoveVelocity);
                }
            }

            self->timer += Engine.deltaTime;
            self->autoButtonMoveVelocity -= 0.125 * (60.0 * Engine.deltaTime);

            if (self->timer > 0.5) {
                NativeEntity_AchievementsButton *button = self->buttons[self->buttonID];
                float div                               = (60.0 * Engine.deltaTime) * 16.0;

                button->x += ((112.0 - button->x) / div);
                button->y += ((64.0 - button->y) / div);
                button->z += ((200.0 - button->z) / div);
                self->backButton->z += ((320.0 - self->backButton->z) / div);
            }

            if (self->timer > 1.5) {
                self->timer = 0.0;
                self->state = MENUCONTROL_STATE_SUBMENU;

                for (int i = 0; i < self->buttonCount; ++i) {
                    if (self->buttonID != i) {
                        if (self->buttonID != i)
                            self->buttons[i]->visible = false;
                    }
                }
            }
            break;
        }

        case MENUCONTROL_STATE_SUBMENU: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            if (touches <= 0) {
                if (self->backButton->g == 0xC0) {
                    PlaySfxByName("Menu Back", false);
                    self->backButton->g = 0xFF;
                    self->state         = MENUCONTROL_STATE_EXITSUBMENU;
                }
            }
            else {
                backButton = self->backButton;
                if (CheckTouchRect(122.0, -80.0, 32.0, 32.0) < 0)
                    backButton->g = 0xFF;
                else
                    backButton->g = 0xC0;
            }
            if (keyPress.B) {
                PlaySfxByName("Menu Back", false);
                self->backButton->g = 0xFF;
                self->state         = MENUCONTROL_STATE_EXITSUBMENU;
            }
            break;
        }

        case MENUCONTROL_STATE_EXITSUBMENU: {
            self->backButton->z = ((0.0 - self->backButton->z) / (16.0 * (60.0 * Engine.deltaTime))) + self->backButton->z;
            self->timer += Engine.deltaTime;
            if (self->timer > 0.25) {
                float offset = self->buttonMovePos;
                float div    = (60.0 * Engine.deltaTime) * 8.0;

                for (int i = 0; i < self->buttonCount; ++i) {
                    if (self->buttonID != i) {
                        NativeEntity_AchievementsButton *button = self->buttons[i];
                        button->z       = ((((cosf(offset + self->buttonMovePos) * -512.0) + 672.0) - button->z) / div) + button->z;
                        button->visible = true;
                    }
                    offset += self->buttonIncline;
                }

                NativeEntity_AchievementsButton *curButton = self->buttons[self->buttonID];
                curButton->labelPtr->state                 = TEXTLABEL_STATE_IDLE;
                curButton->x += ((0.0 - curButton->x) / div);
                curButton->y += ((16.0 - curButton->y) / div);
                curButton->z += ((160.0 - curButton->z) / div);
            }

            if (self->timer > 1.0) {
                self->timer                  = 0.0;
                self->autoButtonMoveVelocity = 0.0;
                self->state                  = MENUCONTROL_STATE_MAIN;
            }
            break;
        }

        case MENUCONTROL_STATE_DIALOGWAIT: {
            if (self->dialog->selection == DLG_NO || self->dialog->selection == DLG_OK) {
                self->state       = MENUCONTROL_STATE_MAIN;
                self->dialogTimer = 50;
            }
            else if (self->dialog->selection == DLG_YES) {
                ExitGame();
                self->dialogTimer = 50;
                self->state       = MENUCONTROL_STATE_MAIN;
            }
            break;
        }

        default: break;
    }
}
