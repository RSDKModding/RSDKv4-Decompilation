#include "RetroEngine.hpp"

void OptionsMenu_Create(void *objPtr)
{
    RSDK_THIS(OptionsMenu);
    self->menuControl      = (NativeEntity_MenuControl *)GetNativeObject(0);
    self->labelPtr         = CREATE_ENTITY(TextLabel);
    self->labelPtr->fontID = FONT_HEADING;
    if (Engine.language == RETRO_RU)
        self->labelPtr->scale = 0.125;
    else
        self->labelPtr->scale = 0.2;
    self->labelPtr->alpha = 0;
    self->labelPtr->z     = 0;
    self->labelPtr->state = TEXTLABEL_STATE_IDLE;
    SetStringToFont(self->labelPtr->text, strHelpAndOptions, FONT_HEADING);
    self->labelPtr->alignOffset = 512.0;
    self->labelRotateY          = DegreesToRad(22.5);
    MatrixRotateYF(&self->labelPtr->renderMatrix, self->labelRotateY);
    MatrixTranslateXYZF(&self->matrix1, -128.0, 80.0, 160.0);
    MatrixMultiplyF(&self->labelPtr->renderMatrix, &self->matrix1);
    self->labelPtr->useRenderMatrix = true;

    float y = 48.0;
    for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) {
        self->buttons[i] = CREATE_ENTITY(SubMenuButton);

        self->buttons[i]->matXOff = 512.0;
        self->buttons[i]->textY   = -4.0;
        self->buttons[i]->matZ    = 0.0;
        self->buttons[i]->scale   = 0.1;
        self->buttonRotateY[i]    = DegreesToRad(16.0);
        MatrixRotateYF(&self->buttons[i]->matrix, self->buttonRotateY[i]);
        MatrixTranslateXYZF(&self->matrix1, -128.0, y, 160.0);
        MatrixMultiplyF(&self->buttons[i]->matrix, &self->matrix1);
        self->buttons[i]->useMatrix = true;
        y -= 30.0;
    }
#if !RETRO_USE_ORIGINAL_CODE
    if (!Engine.devMenu)
        SetStringToFont(self->buttons[OPTIONSMENU_BUTTON_INSTRUCTIONS]->text, strInstructions, FONT_LABEL);
    else
        SetStringToFont(self->buttons[OPTIONSMENU_BUTTON_INSTRUCTIONS]->text, strDevMenu, FONT_LABEL);
#else
    SetStringToFont(self->buttons[OPTIONSMENU_BUTTON_INSTRUCTIONS]->text, strInstructions, FONT_LABEL);
#endif
    SetStringToFont(self->buttons[OPTIONSMENU_BUTTON_SETTINGS]->text, strSettings, FONT_LABEL);
    SetStringToFont(self->buttons[OPTIONSMENU_BUTTON_ABOUT]->text, strAbout, FONT_LABEL);
    SetStringToFont(self->buttons[OPTIONSMENU_BUTTON_CREDITS]->text, strStaffCredits, FONT_LABEL);
}
void OptionsMenu_Main(void *objPtr)
{
    RSDK_THIS(OptionsMenu);
    switch (self->state) {
        case OPTIONSMENU_STATE_SETUP: {
            self->timer += Engine.deltaTime;
            if (self->timer > 1.0) {
                self->timer = 0.0;
                self->state = OPTIONSMENU_STATE_ENTER;
            }
            break;
        }
        case OPTIONSMENU_STATE_ENTER: {
            self->labelPtr->alignOffset /= (1.125 * (60.0 * Engine.deltaTime));

            float div = (60.0 * Engine.deltaTime) * 16.0;
            for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) self->buttons[i]->matXOff += ((-176.0 - self->buttons[i]->matXOff) / div);

            self->timer += (Engine.deltaTime + Engine.deltaTime);
            self->labelPtr->alpha = (256.0 * self->timer);
            if (self->timer > 1.0) {
                self->timer      = 0.0;
                self->state      = OPTIONSMENU_STATE_MAIN;
                keyPress.start = false;
                keyPress.A     = false;
            }
            break;
        }
        case OPTIONSMENU_STATE_MAIN: {
            if (self->menuControl->state == MENUCONTROL_STATE_EXITSUBMENU) {
                self->state = OPTIONSMENU_STATE_EXIT;
            }
            else if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.up) {
                        PlaySfxByName("Menu Move", false);
                        self->selectedButton--;
                        if (self->selectedButton < 0)
                            self->selectedButton = OPTIONSMENU_BUTTON_COUNT - 1;
                    }
                    else if (keyPress.down) {
                        PlaySfxByName("Menu Move", false);
                        self->selectedButton++;
                        if (self->selectedButton >= OPTIONSMENU_BUTTON_COUNT)
                            self->selectedButton = 0;
                    }
                    for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) self->buttons[i]->b = 0xFF;
                    self->buttons[self->selectedButton]->b = 0x00;

                    if (self->buttons[self->selectedButton]->g > 0x80 && (keyPress.start || keyPress.A)) {
                        PlaySfxByName("Menu Select", false);
                        self->buttons[self->selectedButton]->state = SUBMENUBUTTON_STATE_FLASHING2;
                        self->buttons[self->selectedButton]->b     = 0xFF;
                        self->state                                = OPTIONSMENU_STATE_ACTION;
#if !RETRO_USE_ORIGINAL_CODE
                        self->unused1 = Engine.devMenu && self->selectedButton == OPTIONSMENU_BUTTON_INSTRUCTIONS && !keyDown.X;
                        if (self->unused1)
                            StopMusic(true);
#endif
                    }
                }
            }
            else {
                float y = 48.0;
                for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) {
                    if (touches > 0) {
                        if (CheckTouchRect(-64.0, y, 96.0, 12.0) >= 0)
                            self->buttons[i]->b = 0x00;
                        else
                            self->buttons[i]->b = 0xFF;
                    }
                    else if (!self->buttons[i]->b) {
                        self->selectedButton = i;
                        PlaySfxByName("Menu Select", false);
                        self->buttons[i]->state = SUBMENUBUTTON_STATE_FLASHING2;
                        self->buttons[i]->b     = 0xFF;
                        self->state             = OPTIONSMENU_STATE_ACTION;
                        break;
                    }
                    y -= 30.0;
                }

                if (self->state == OPTIONSMENU_STATE_MAIN && (keyDown.up || keyDown.down)) {
                    self->selectedButton = 0;
                    usePhysicalControls  = true;
                }
            }
            break;
        }
        case OPTIONSMENU_STATE_EXIT: {
            self->labelPtr->alignOffset = (10.0 * (60.0 * Engine.deltaTime)) + self->labelPtr->alignOffset;
            self->timer += (Engine.deltaTime + Engine.deltaTime);

            for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) self->buttons[i]->matXOff += (12.0 * (60.0 * Engine.deltaTime));

            if (self->timer > 1.0) {
                self->timer = 0.0;
                RemoveNativeObject(self->labelPtr);
                for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) RemoveNativeObject(self->buttons[i]);
                RemoveNativeObject(self);
            }
            break;
        }
        case OPTIONSMENU_STATE_ACTION: {
            self->menuControl->state = MENUCONTROL_STATE_NONE;
            if (!self->buttons[self->selectedButton]->state) {
#if !RETRO_USE_ORIGINAL_CODE
                if (!self->unused1) {
#endif
                    self->state = OPTIONSMENU_STATE_ENTERSUBMENU;

                    self->labelRotateYVelocity = 0.0;
                    self->targetLabelRotateY   = DegreesToRad(-90.0);
                    for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) self->targetButtonRotateY[i] = DegreesToRad(-90.0);
                    float val = 0.02;
                    for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) {
                        self->buttonRotateYVelocity[i] = val;
                        val += 0.02;
                    }
#if !RETRO_USE_ORIGINAL_CODE
                }
                else {
                    self->state = OPTIONSMENU_STATE_SUBMENU;
                    CREATE_ENTITY(FadeScreen);
                    Engine.gameMode = ENGINE_INITDEVMENU;
                }
#endif
            }
            break;
        }
        case OPTIONSMENU_STATE_ENTERSUBMENU: {
            if (self->labelRotateY > self->targetLabelRotateY) {
                self->labelRotateYVelocity -= 0.0025 * (Engine.deltaTime * 60.0);
                self->labelRotateY += (Engine.deltaTime * 60.0) * self->labelRotateYVelocity;
                self->labelRotateYVelocity -= 0.0025 * (Engine.deltaTime * 60.0);
                MatrixRotateYF(&self->labelPtr->renderMatrix, self->labelRotateY);
                MatrixTranslateXYZF(&self->matrix1, -128.0, 80.0, 160.0);
                MatrixMultiplyF(&self->labelPtr->renderMatrix, &self->matrix1);
            }

            float y = 48.0;
            for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) {
                if (self->buttonRotateY[i] > self->targetButtonRotateY[i]) {
                    self->buttonRotateYVelocity[i] -= 0.0025 * (60.0 * Engine.deltaTime);
                    if (self->buttonRotateYVelocity[i] < 0.0)
                        self->buttonRotateY[i] += ((60.0 * Engine.deltaTime) * self->buttonRotateYVelocity[i]);
                    self->buttonRotateYVelocity[i] -= 0.0025 * (60.0 * Engine.deltaTime);
                    MatrixRotateYF(&self->buttons[i]->matrix, self->buttonRotateY[i]);
                    MatrixTranslateXYZF(&self->matrix1, -128.0, y, 160.0);
                    MatrixMultiplyF(&self->buttons[i]->matrix, &self->matrix1);
                }
                y -= 30.0;
            }

            if (self->targetButtonRotateY[OPTIONSMENU_BUTTON_COUNT - 1] >= self->buttonRotateY[OPTIONSMENU_BUTTON_COUNT - 1]) {
                self->state                = OPTIONSMENU_STATE_SUBMENU;
                self->labelRotateYVelocity = 0.0;
                self->targetLabelRotateY   = DegreesToRad(22.5);
                for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) self->targetButtonRotateY[i] = DegreesToRad(16.0);
                float val = -0.02;
                for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) {
                    self->buttonRotateYVelocity[i] = val;
                    val -= 0.02;
                }

                switch (self->selectedButton) {
                    default: break;
                    case OPTIONSMENU_BUTTON_INSTRUCTIONS:
                        self->instructionsScreen              = CREATE_ENTITY(InstructionsScreen);
                        self->instructionsScreen->optionsMenu = self;
                        break;
                    case OPTIONSMENU_BUTTON_SETTINGS:
                        self->settingsScreen              = CREATE_ENTITY(SettingsScreen);
                        self->settingsScreen->optionsMenu = self;
                        break;
                    case OPTIONSMENU_BUTTON_ABOUT:
                        self->aboutScreen              = CREATE_ENTITY(AboutScreen);
                        self->aboutScreen->optionsMenu = self;
                        break;
                    case OPTIONSMENU_BUTTON_CREDITS:
                        self->staffCredits              = CREATE_ENTITY(StaffCredits);
                        self->staffCredits->optionsMenu = self;
                        break;
                }
            }

            float div                               = (60.0 * Engine.deltaTime) * 16.0;
            NativeEntity_AchievementsButton *button = self->menuControl->buttons[self->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = self->menuControl->backButton;
            button->x += ((512.0 - button->x) / div);
            backButton->x += ((1024.0 - backButton->x) / div);
            break;
        }
        case OPTIONSMENU_STATE_SUBMENU: // sub menu idle
            break;
        case OPTIONSMENU_STATE_EXITSUBMENU: {
            if (self->targetLabelRotateY > self->labelRotateY) {
                self->labelRotateYVelocity += 0.0025 * (Engine.deltaTime * 60.0);
                self->labelRotateY += (Engine.deltaTime * 60.0) * self->labelRotateYVelocity;
                self->labelRotateYVelocity += 0.0025 * (Engine.deltaTime * 60.0);
                if (self->labelRotateY > self->targetLabelRotateY)
                    self->labelRotateY = self->targetLabelRotateY;
                MatrixRotateYF(&self->labelPtr->renderMatrix, self->labelRotateY);
                MatrixTranslateXYZF(&self->matrix1, -128.0, 80.0, 160.0);
                MatrixMultiplyF(&self->labelPtr->renderMatrix, &self->matrix1);
            }

            float y = 48.0;
            for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) {
                if (self->targetButtonRotateY[i] > self->buttonRotateY[i]) {
                    self->buttonRotateYVelocity[i] += 0.0025 * (60.0 * Engine.deltaTime);
                    if (self->buttonRotateYVelocity[i] > 0.0)
                        self->buttonRotateY[i] += ((60.0 * Engine.deltaTime) * self->buttonRotateYVelocity[i]);
                    self->buttonRotateYVelocity[i] += 0.0025 * (60.0 * Engine.deltaTime);
                    if (self->buttonRotateY[i] > self->targetButtonRotateY[i])
                        self->buttonRotateY[i] = self->targetButtonRotateY[i];
                    MatrixRotateYF(&self->buttons[i]->matrix, self->buttonRotateY[i]);
                    MatrixTranslateXYZF(&self->matrix1, -128.0, y, 160.0);
                    MatrixMultiplyF(&self->buttons[i]->matrix, &self->matrix1);
                }
                y -= 30.0;
            }

            float div                               = (60.0 * Engine.deltaTime) * 16.0;
            NativeEntity_AchievementsButton *button = self->menuControl->buttons[self->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = self->menuControl->backButton;
            button->x += ((112.0 - button->x) / div);
            backButton->x += ((230.0 - backButton->x) / div);

            if (backButton->x < SCREEN_YSIZE) {
                backButton->x            = SCREEN_YSIZE;
                self->state              = OPTIONSMENU_STATE_MAIN;
                self->menuControl->state = MENUCONTROL_STATE_SUBMENU;
            }
            break;
        }
        default: break;
    }
}
