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
    entity->labelPtr->state = 0;
    SetStringToFont(entity->labelPtr->text, strHelpAndOptions, FONT_HEADING);
    entity->labelPtr->textWidth = 512.0;
    entity->labelRotateY        = DegreesToRad(22.5);
    matrixRotateYF(&entity->labelPtr->renderMatrix, entity->labelRotateY);
    matrixTranslateXYZF(&entity->matrix1, -128.0, 80.0, 160.0);
    matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrix1);
    entity->labelPtr->useRenderMatrix = true;

    float y = 48.0;
    for (int i = 0; i < 4; ++i) {
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
    SetStringToFont(entity->buttons[0]->text, strInstructions, FONT_LABEL);
    SetStringToFont(entity->buttons[1]->text, strSettings, FONT_LABEL);
    SetStringToFont(entity->buttons[2]->text, strAbout, FONT_LABEL);
    SetStringToFont(entity->buttons[3]->text, strStaffCredits, FONT_LABEL);
}
void OptionsMenu_Main(void *objPtr)
{
    RSDK_THIS(OptionsMenu);
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

            float div                   = (60.0 * Engine.deltaTime) * 16.0;
            entity->buttons[0]->matXOff = ((-176.0 - entity->buttons[0]->matXOff) / div) + entity->buttons[0]->matXOff;
            entity->buttons[1]->matXOff = ((-176.0 - entity->buttons[1]->matXOff) / div) + entity->buttons[1]->matXOff;
            entity->buttons[2]->matXOff = ((-176.0 - entity->buttons[2]->matXOff) / div) + entity->buttons[2]->matXOff;
            entity->buttons[3]->matXOff = ((-176.0 - entity->buttons[3]->matXOff) / div) + entity->buttons[3]->matXOff;

            entity->timer += (Engine.deltaTime + Engine.deltaTime);
            entity->labelPtr->alpha = (256.0 * entity->timer);
            if (entity->timer > 1.0) {
                entity->timer  = 0.0;
                entity->state  = 2;
                keyPress.start = false;
                keyPress.A     = false;
            }
            break;
        }
        case 2: {
            if (entity->menuControl->state == 5) {
                entity->state = 3;
            }
            else if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.up) {
                        PlaySfxByName("Menu Move", false);
                        entity->selectedButton--;
                        if (entity->selectedButton < 0)
                            entity->selectedButton = 3;
                    }
                    else if (keyPress.down) {
                        PlaySfxByName("Menu Move", false);
                        entity->selectedButton++;
                        if (entity->selectedButton >= 4)
                            entity->selectedButton = 0;
                    }
                    entity->buttons[0]->b                      = 0xFF;
                    entity->buttons[1]->b                      = 0xFF;
                    entity->buttons[2]->b                      = 0xFF;
                    entity->buttons[3]->b                      = 0xFF;
                    entity->buttons[entity->selectedButton]->b = 0x00;

                    if (entity->buttons[entity->selectedButton]->g > 0x80 && (keyPress.start || keyPress.A)) {
                        PlaySfxByName("Menu Select", false);
                        entity->buttons[entity->selectedButton]->state = 2;
                        entity->buttons[entity->selectedButton]->b     = -1;
                        entity->state                                  = 4;
                    }
                }
            }
            else {
                float y = 48.0;
                for (int i = 0; i < 4; ++i) {
                    if (touches > 0) {
                        if (CheckTouchRect(-64.0, y, 96.0, 12.0) < 0)
                            entity->buttons[i]->b = 0xFF;
                        else
                            entity->buttons[i]->b = 0x00;
                    }
                    else if (!entity->buttons[i]->b) {
                        entity->selectedButton = i;
                        PlaySfxByName("Menu Select", false);
                        entity->buttons[i]->state = 2;
                        entity->buttons[i]->b     = 0xFF;
                        entity->state             = 4;
                        break;
                    }
                    y -= 30.0;
                }

                if (entity->state == 2 && (keyDown.up || keyDown.down)) {
                    entity->selectedButton = 0;
                    usePhysicalControls    = true;
                }
            }
            break;
        }
        case 3: {
            entity->labelPtr->textWidth = (10.0 * (60.0 * Engine.deltaTime)) + entity->labelPtr->textWidth;
            entity->timer += (Engine.deltaTime + Engine.deltaTime);

            entity->buttons[0]->matXOff = (12.0 * (60.0 * Engine.deltaTime)) + entity->buttons[0]->matXOff;
            entity->buttons[1]->matXOff = (13.0 * (60.0 * Engine.deltaTime)) + entity->buttons[1]->matXOff;
            entity->buttons[2]->matXOff = (14.0 * (60.0 * Engine.deltaTime)) + entity->buttons[2]->matXOff;
            entity->buttons[3]->matXOff = (15.0 * (60.0 * Engine.deltaTime)) + entity->buttons[3]->matXOff;
            if (entity->timer > 1.0) {
                entity->timer = 0.0;
                RemoveNativeObject(entity->labelPtr);
                for (int i = 0; i < 4; ++i) RemoveNativeObject(entity->buttons[i]);
                RemoveNativeObject(entity);
            }
            break;
        }
        case 4: {
            entity->menuControl->state = 2;
            if (!entity->buttons[entity->selectedButton]->state) {
                entity->state = 5;

                entity->field_3C = 0.0;
                entity->float38  = DegreesToRad(-90.0);
                for (int i = 0; i < 4; ++i) entity->field_A4[i] = DegreesToRad(-90.0);
                entity->field_B4[0] = 0.02;
                entity->field_B4[1] = 0.04;
                entity->field_B4[2] = 0.06;
                entity->field_B4[3] = 0.08;
            }
            break;
        }
        case 5: {
            if (entity->labelRotateY > entity->float38) {
                entity->field_3C -= 0.0025 * (Engine.deltaTime * 60.0);
                entity->labelRotateY += (Engine.deltaTime * 60.0) * entity->field_3C;
                entity->field_3C -= 0.0025 * (Engine.deltaTime * 60.0);
                matrixRotateYF(&entity->labelPtr->renderMatrix, entity->labelRotateY);
                matrixTranslateXYZF(&entity->matrix1, -128.0, 80.0, 160.0);
                matrixMultiplyF(&entity->labelPtr->renderMatrix, &entity->matrix1);
            }

            float y = 48.0;
            for (int i = 0; i < 4; ++i) {
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

            if (entity->field_A4[3] >= entity->buttonRotateY[3]) {
                entity->state    = 6;
                entity->field_3C = 0.0;
                entity->float38  = DegreesToRad(22.5);
                for (int i = 0; i < 4; ++i) entity->field_A4[i] = DegreesToRad(16.0);
                entity->field_B4[0] = -0.02;
                entity->field_B4[1] = -0.04;
                entity->field_B4[2] = -0.06;
                entity->field_B4[3] = -0.08;

                switch (entity->selectedButton) {
                    default: break;
                    case 0:
                        entity->instructionsScreen              = CREATE_ENTITY(InstructionsScreen);
                        entity->instructionsScreen->optionsMenu = entity;
                        break;
                    case 1:
                        entity->settingsScreen              = CREATE_ENTITY(SettingsScreen);
                        entity->settingsScreen->optionsMenu = entity;
                        break;
                    case 2:
                        entity->aboutScreen              = CREATE_ENTITY(AboutScreen);
                        entity->aboutScreen->optionsMenu = entity;
                        break;
                    case 3:
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
        case 6: // sub menu idle
            break;
        case 7: {
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
            for (int i = 0; i < 4; ++i) {
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
                entity->state              = 2;
                entity->menuControl->state = 4;
            }
            break;
        }
        default: break;
    }
}
