#include "RetroEngine.hpp"

void OptionsMenu_Create(void *objPtr)
{
    RSDK_THIS(OptionsMenu);
    entity->menuControl      = (NativeEntity_MenuControl *)GetNativeObject(0);
    entity->labelPtr         = CREATE_ENTITY(TextLabel);
    entity->labelPtr->fontID = FONT_HEADING;
    if (Engine.language == RETRO_RU)
        entity->labelPtr->scale = 0.125;
    else
        entity->labelPtr->scale = 0.2;
    entity->labelPtr->alpha = 0;
    entity->labelPtr->z     = 0;
    entity->labelPtr->state = TEXTLABEL_STATE_IDLE;
    SetStringToFont(entity->labelPtr->text, strHelpAndOptions, FONT_HEADING);
    entity->labelPtr->alignOffset = 512.0;
    entity->labelRotateY          = DegreesToRad(22.5);
    matrixRotateYF(&entity->labelPtr->renderMatrix, entity->labelRotateY);
    matrixTranslateXYZF(&entity->matrix1, -128.0, 80.0, 160.0);
    matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrix1);
    entity->labelPtr->useRenderMatrix = true;

    float y = 48.0;
    for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) {
        entity->buttons[i] = CREATE_ENTITY(SubMenuButton);

        entity->buttons[i]->matXOff = 512.0;
        entity->buttons[i]->textY   = -4.0;
        entity->buttons[i]->matZ    = 0.0;
        entity->buttons[i]->scale   = 0.1;
        entity->buttonRotateY[i]    = DegreesToRad(16.0);
        matrixRotateYF(&entity->buttons[i]->matrix, entity->buttonRotateY[i]);
        matrixTranslateXYZF(&entity->matrix1, -128.0, y, 160.0);
        matrixMultiplyF(&entity->buttons[i]->matrix, &entity->matrix1);
        entity->buttons[i]->useMatrix = true;
        y -= 30.0;
    }
#if !RETRO_USE_ORIGINAL_CODE
    if (!Engine.devMenu)
        SetStringToFont(entity->buttons[OPTIONSMENU_BUTTON_INSTRUCTIONS]->text, strInstructions, FONT_LABEL);
    else
        SetStringToFont(entity->buttons[OPTIONSMENU_BUTTON_INSTRUCTIONS]->text, strDevMenu, FONT_LABEL);
#else
    SetStringToFont(entity->buttons[OPTIONSMENU_BUTTON_INSTRUCTIONS]->text, strInstructions, FONT_LABEL);
#endif
    SetStringToFont(entity->buttons[OPTIONSMENU_BUTTON_SETTINGS]->text, strSettings, FONT_LABEL);
    SetStringToFont(entity->buttons[OPTIONSMENU_BUTTON_ABOUT]->text, strAbout, FONT_LABEL);
    SetStringToFont(entity->buttons[OPTIONSMENU_BUTTON_CREDITS]->text, strStaffCredits, FONT_LABEL);
}
void OptionsMenu_Main(void *objPtr)
{
    RSDK_THIS(OptionsMenu);
    switch (entity->state) {
        case OPTIONSMENU_STATE_SETUP: {
            entity->timer += Engine.deltaTime;
            if (entity->timer > 1.0) {
                entity->timer = 0.0;
                entity->state = OPTIONSMENU_STATE_ENTER;
            }
            break;
        }
        case OPTIONSMENU_STATE_ENTER: {
            entity->labelPtr->alignOffset /= (1.125 * (60.0 * Engine.deltaTime));

            float div = (60.0 * Engine.deltaTime) * 16.0;
            for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) entity->buttons[i]->matXOff += ((-176.0 - entity->buttons[i]->matXOff) / div);

            entity->timer += (Engine.deltaTime + Engine.deltaTime);
            entity->labelPtr->alpha = (256.0 * entity->timer);
            if (entity->timer > 1.0) {
                entity->timer    = 0.0;
                entity->state    = OPTIONSMENU_STATE_MAIN;
                inputPress.start = false;
                inputPress.A     = false;
            }
            break;
        }
        case OPTIONSMENU_STATE_MAIN: {
            if (entity->menuControl->state == MENUCONTROL_STATE_EXITSUBMENU) {
                entity->state = OPTIONSMENU_STATE_EXIT;
            }
            else if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (inputPress.up) {
                        PlaySfxByName("Menu Move", false);
                        entity->selectedButton--;
                        if (entity->selectedButton < 0)
                            entity->selectedButton = OPTIONSMENU_BUTTON_COUNT - 1;
                    }
                    else if (inputPress.down) {
                        PlaySfxByName("Menu Move", false);
                        entity->selectedButton++;
                        if (entity->selectedButton >= OPTIONSMENU_BUTTON_COUNT)
                            entity->selectedButton = 0;
                    }
                    for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) entity->buttons[i]->b = 0xFF;
                    entity->buttons[entity->selectedButton]->b = 0x00;

                    if (entity->buttons[entity->selectedButton]->g > 0x80 && (inputPress.start || inputPress.A)) {
                        PlaySfxByName("Menu Select", false);
                        entity->buttons[entity->selectedButton]->state = SUBMENUBUTTON_STATE_FLASHING2;
                        entity->buttons[entity->selectedButton]->b     = 0xFF;
                        entity->state                                  = OPTIONSMENU_STATE_ACTION;
                    }
                }
            }
            else {
                float y = 48.0;
                for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) {
                    if (touches > 0) {
                        if (CheckTouchRect(-64.0, y, 96.0, 12.0) >= 0)
                            entity->buttons[i]->b = 0x00;
                        else
                            entity->buttons[i]->b = 0xFF;
                    }
                    else if (!entity->buttons[i]->b) {
                        entity->selectedButton = i;
                        PlaySfxByName("Menu Select", false);
                        entity->buttons[i]->state = SUBMENUBUTTON_STATE_FLASHING2;
                        entity->buttons[i]->b     = 0xFF;
                        entity->state             = OPTIONSMENU_STATE_ACTION;
                        break;
                    }
                    y -= 30.0;
                }

                if (entity->state == OPTIONSMENU_STATE_MAIN && (inputDown.up || inputDown.down)) {
                    entity->selectedButton = 0;
                    usePhysicalControls    = true;
                }
            }
            break;
        }
        case OPTIONSMENU_STATE_EXIT: {
            entity->labelPtr->alignOffset = (10.0 * (60.0 * Engine.deltaTime)) + entity->labelPtr->alignOffset;
            entity->timer += (Engine.deltaTime + Engine.deltaTime);

            for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) entity->buttons[i]->matXOff += (12.0 * (60.0 * Engine.deltaTime));

            if (entity->timer > 1.0) {
                entity->timer = 0.0;
                RemoveNativeObject(entity->labelPtr);
                for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) RemoveNativeObject(entity->buttons[i]);
                RemoveNativeObject(entity);
            }
            break;
        }
        case OPTIONSMENU_STATE_ACTION: {
            entity->menuControl->state = MENUCONTROL_STATE_NONE;
            if (!entity->buttons[entity->selectedButton]->state) {
                entity->state = OPTIONSMENU_STATE_ENTERSUBMENU;

                entity->field_3C = 0.0;
                entity->float38  = DegreesToRad(-90.0);
                for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) entity->field_A4[i] = DegreesToRad(-90.0);
                float val = 0.02;
                for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) {
                    entity->field_B4[i] = val;
                    val += 0.02;
                }
            }
            break;
        }
        case OPTIONSMENU_STATE_ENTERSUBMENU: {
            if (entity->labelRotateY > entity->float38) {
                entity->field_3C -= 0.0025 * (Engine.deltaTime * 60.0);
                entity->labelRotateY += (Engine.deltaTime * 60.0) * entity->field_3C;
                entity->field_3C -= 0.0025 * (Engine.deltaTime * 60.0);
                matrixRotateYF(&entity->labelPtr->renderMatrix, entity->labelRotateY);
                matrixTranslateXYZF(&entity->matrix1, -128.0, 80.0, 160.0);
                matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrix1);
            }

            float y = 48.0;
            for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) {
                if (entity->buttonRotateY[i] > entity->field_A4[i]) {
                    entity->field_B4[i] -= 0.0025 * (60.0 * Engine.deltaTime);
                    if (entity->field_B4[i] < 0.0)
                        entity->buttonRotateY[i] += ((60.0 * Engine.deltaTime) * entity->field_B4[i]);
                    entity->field_B4[i] -= 0.0025 * (60.0 * Engine.deltaTime);
                    matrixRotateYF(&entity->buttons[i]->matrix, entity->buttonRotateY[i]);
                    matrixTranslateXYZF(&entity->matrix1, -128.0, y, 160.0);
                    matrixMultiplyF(&entity->buttons[i]->matrix, &entity->matrix1);
                }
                y -= 30.0;
            }

            if (entity->field_A4[OPTIONSMENU_BUTTON_COUNT - 1] >= entity->buttonRotateY[OPTIONSMENU_BUTTON_COUNT - 1]) {
                entity->state    = OPTIONSMENU_STATE_SUBMENU;
                entity->field_3C = 0.0;
                entity->float38  = DegreesToRad(22.5);
                for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) entity->field_A4[i] = DegreesToRad(16.0);
                float val = -0.02;
                for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) {
                    entity->field_B4[i] = val;
                    val -= 0.02;
                }

                switch (entity->selectedButton) {
                    default: break;
                    case OPTIONSMENU_BUTTON_INSTRUCTIONS:
#if !RETRO_USE_ORIGINAL_CODE
                        if (!Engine.devMenu) {
                            entity->instructionsScreen              = CREATE_ENTITY(InstructionsScreen);
                            entity->instructionsScreen->optionsMenu = entity;
                        }
                        else {
                            CREATE_ENTITY(FadeScreen);
                            Engine.gameMode = ENGINE_INITDEVMENU;
                        }
#else
                        entity->instructionsScreen              = CREATE_ENTITY(InstructionsScreen);
                        entity->instructionsScreen->optionsMenu = entity;
#endif
                        break;
                    case OPTIONSMENU_BUTTON_SETTINGS:
                        entity->settingsScreen              = CREATE_ENTITY(SettingsScreen);
                        entity->settingsScreen->optionsMenu = entity;
                        break;
                    case OPTIONSMENU_BUTTON_ABOUT:
                        entity->aboutScreen              = CREATE_ENTITY(AboutScreen);
                        entity->aboutScreen->optionsMenu = entity;
                        break;
                    case OPTIONSMENU_BUTTON_CREDITS:
                        entity->staffCredits              = CREATE_ENTITY(StaffCredits);
                        entity->staffCredits->optionsMenu = entity;
                        break;
                }
            }

            float div                               = (60.0 * Engine.deltaTime) * 16.0;
            NativeEntity_AchievementsButton *button = entity->menuControl->buttons[entity->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = entity->menuControl->backButton;
            button->x += ((512.0 - button->x) / div);
            backButton->x += ((1024.0 - backButton->x) / div);
            break;
        }
        case OPTIONSMENU_STATE_SUBMENU: // sub menu idle
            break;
        case OPTIONSMENU_STATE_EXITSUBMENU: {
            if (entity->float38 > entity->labelRotateY) {
                entity->field_3C += 0.0025 * (Engine.deltaTime * 60.0);
                entity->labelRotateY += (Engine.deltaTime * 60.0) * entity->field_3C;
                entity->field_3C += 0.0025 * (Engine.deltaTime * 60.0);
                if (entity->labelRotateY > entity->float38)
                    entity->labelRotateY = entity->float38;
                matrixRotateYF(&entity->labelPtr->renderMatrix, entity->labelRotateY);
                matrixTranslateXYZF(&entity->matrix1, -128.0, 80.0, 160.0);
                matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrix1);
            }

            float y = 48.0;
            for (int i = 0; i < OPTIONSMENU_BUTTON_COUNT; ++i) {
                if (entity->field_A4[i] > entity->buttonRotateY[i]) {
                    entity->field_B4[i] += 0.0025 * (60.0 * Engine.deltaTime);
                    if (entity->field_B4[i] > 0.0)
                        entity->buttonRotateY[i] += ((60.0 * Engine.deltaTime) * entity->field_B4[i]);
                    entity->field_B4[i] += 0.0025 * (60.0 * Engine.deltaTime);
                    if (entity->buttonRotateY[i] > entity->field_A4[i])
                        entity->buttonRotateY[i] = entity->field_A4[i];
                    matrixRotateYF(&entity->buttons[i]->matrix, entity->buttonRotateY[i]);
                    matrixTranslateXYZF(&entity->matrix1, -128.0, y, 160.0);
                    matrixMultiplyF(&entity->buttons[i]->matrix, &entity->matrix1);
                }
                y -= 30.0;
            }

            float div                               = (60.0 * Engine.deltaTime) * 16.0;
            NativeEntity_AchievementsButton *button = entity->menuControl->buttons[entity->menuControl->buttonID];
            NativeEntity_BackButton *backButton     = entity->menuControl->backButton;
            button->x += ((112.0 - button->x) / div);
            backButton->x += ((230.0 - backButton->x) / div);

            if (backButton->x < SCREEN_YSIZE) {
                backButton->x              = SCREEN_YSIZE;
                entity->state              = OPTIONSMENU_STATE_MAIN;
                entity->menuControl->state = MENUCONTROL_STATE_SUBMENU;
            }
            break;
        }
        default: break;
    }
}
