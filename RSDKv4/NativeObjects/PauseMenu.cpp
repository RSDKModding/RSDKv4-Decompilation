#include "RetroEngine.hpp"

int buttonCount;

void PauseMenu_Create(void *objPtr)
{
    RSDK_THIS(PauseMenu);
    buttonCount = PMB_COUNT;
    if (PMB_COUNT == 5 && !Engine.devMenu)
        buttonCount--;

    entity->retroGameLoop    = (NativeEntity_RetroGameLoop *)GetNativeObject(0);
    entity->label            = CREATE_ENTITY(TextLabel);
    entity->label->alignment = 0;
    entity->label->textZ     = 0.0;
    entity->label->textScale = 0.2;
    entity->label->textAlpha = 0;
    entity->label->fontID    = 0;
    SetStringToFont(entity->label->text, strPause, 0);
    entity->label->textWidth = 512.0;
    entity->renderRot        = DegreesToRad(22.5);
    matrixRotateYF(&entity->label->renderMatrix, DegreesToRad(22.5));
    matrixTranslateXYZF(&entity->matrix, -128.0, 80.0, 160.0);
    matrixMultiplyF(&entity->label->renderMatrix, &entity->matrix);
    entity->label->useMatrix = 1;
    entity->buttonX          = ((SCREEN_CENTERX_F + -160.0) * -0.5) + -128.0;
    for (int i = 0; i < buttonCount; ++i) {
        NativeEntity_SubMenuButton *button = CREATE_ENTITY(SubMenuButton);
        entity->buttons[i]                 = button;
        button->textScale                  = 0.1;
        button->matZ                       = 0.0;
        button->matXOff                    = 512.0;
        button->textY                      = -4.0;
        entity->buttonRot[i]               = DegreesToRad(16.0);
        matrixRotateYF(&button->matrix, DegreesToRad(16.0));
        matrixTranslateXYZF(&entity->matrix, entity->buttonX, 48.0 - i * 30, 160.0);
        matrixMultiplyF(&entity->buttons[i]->matrix, &entity->matrix);
        button->symbol    = 1;
        button->useMatrix = 1;
    }
    if (GetGlobalVariableByName("player.lives") <= 1 && GetGlobalVariableByName("options.gameMode") <= 1 || !activeStageList
        || GetGlobalVariableByName("options.attractMode") == 1) {
        entity->buttons[1]->r = 0x80;
        entity->buttons[1]->g = 0x80;
        entity->buttons[1]->b = 0x80;
    }
    SetStringToFont(entity->buttons[0]->text, strContinue, 1);
    SetStringToFont(entity->buttons[1]->text, strRestart, 1);
    SetStringToFont(entity->buttons[2]->text, strSettings, 1);
    SetStringToFont(entity->buttons[3]->text, strExit, 1);
    if (buttonCount == 5)
        SetStringToFont8(entity->buttons[4]->text, (char *)"DEV MENU", 1);
    entity->textureCircle = LoadTexture("Data/Game/Menu/Circle.png", TEXFMT_RGBA4444);
    entity->rotationY     = 0.0;
    entity->rotYOff       = DegreesToRad(-16.0);
    entity->matrixX       = 0.0;
    entity->matrixY       = 0.0;
    entity->matrixZ       = 160.0;
    entity->width         = (1.75 * SCREEN_CENTERX_F) - ((SCREEN_CENTERX_F - 160) * 2);
    if (Engine.gameDeviceType == RETRO_MOBILE)
        entity->textureDPad = LoadTexture("Data/Game/Menu/VirtualDPad.png", TEXFMT_RGBA8888);
    entity->dpadY             = 104.0;
    entity->state             = 1;
    entity->miniPauseDisabled = 1;
    entity->dpadX             = SCREEN_CENTERX_F - 76.0;
    entity->dpadXSpecial      = SCREEN_CENTERX_F - 52.0;
}
void PauseMenu_Main(void *objPtr)
{
    RSDK_THIS(PauseMenu);
    switch (entity->state) {
        case 0:
            entity->timer += Engine.deltaTime;
            if (entity->timer > 1.0) {
                entity->timer = 0.0;
                entity->state = 1;
                break;
            }
            break;
        case 1:
            if (entity->unusedAlpha <= 255)
                entity->unusedAlpha += 8;
            entity->timer += Engine.deltaTime * 2;
            entity->label->textWidth = entity->label->textWidth / (1.125 * (60.0 * Engine.deltaTime));
            entity->label->textAlpha = (256.0 * entity->timer);
            for (int i = 0; i < buttonCount; ++i)
                entity->buttons[i]->matXOff += ((-176.0 - entity->buttons[i]->matXOff) / (16.0 * (60.0 * Engine.deltaTime)));
            entity->matrixX += ((entity->width - entity->matrixX) / ((60.0 * Engine.deltaTime) * 12.0));
            entity->matrixZ += ((512.0 - entity->matrixZ) / ((60.0 * Engine.deltaTime) * 12.0));
            entity->rotationY += ((entity->rotYOff - entity->rotationY) / (16.0 * (60.0 * Engine.deltaTime)));
            if (entity->timer <= 1.0)
                break;
            entity->timer = 0.0;
            entity->state = 2;
            break;
        case 2: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.up) {
                        PlaySfx(21, 0);
                        entity->buttonSelected--;
                        if (entity->buttonSelected < 0)
                            entity->buttonSelected = buttonCount - 1;
                    }
                    else if (keyPress.down) {
                        PlaySfx(21, 0);
                        entity->buttonSelected++;
                        if (entity->buttonSelected >= buttonCount)
                            entity->buttonSelected = 0;
                    }
                    for (int i = 0; i < buttonCount; ++i) entity->buttons[i]->b = 0xFF;
                    entity->buttons[entity->buttonSelected]->b = 0;
                    if (entity->buttons[entity->buttonSelected]->g > 0x80 && (keyPress.start || keyPress.A)) {
                        PlaySfx(22, 0);
                        entity->buttons[entity->buttonSelected]->state = 2;
                        entity->buttons[entity->buttonSelected]->b     = -1;
                        entity->state                                  = 4;
                    }
                }
            }
            else {
                for (int i = 0; i < buttonCount; ++i) {
                    if (touches > 0) {
                        if (entity->buttons[i]->g > 0x80u)
                            entity->buttons[i]->b = (CheckTouchRect(-80.0, 48.0 - i * 30, 112.0, 12.0) < 0) * 0xFF;
                    }
                    else if (!entity->buttons[i]->b) {
                        entity->buttonSelected = i;
                        PlaySfx(22, 0);
                        entity->buttons[i]->state = 2;
                        entity->buttons[i]->b     = 0xFF;
                        entity->state             = 4;
                        break;
                    }
                }

                if (entity->state == 2 && (keyDown.up || keyDown.down)) {
                    entity->buttonSelected = 0;
                    usePhysicalControls    = true;
                }
            }
            if (touches > 0) {
                if (!entity->miniPauseDisabled && CheckTouchRect(SCREEN_CENTERX_F, SCREEN_CENTERY_F, 112.0, 24.0) >= 0) {
                    entity->buttonSelected = 0;
                    PlaySfx(40, 0);
                    entity->state = 4;
                    break;
                }
                break;
            }
            entity->miniPauseDisabled = 0;
            if (entity->makeSound != 1)
                break;
            PlaySfx(22, 0);
            entity->makeSound = 0;
            break;
        }

        case 3:
            entity->label->textWidth += 10.0 * (60.0 * Engine.deltaTime);
            entity->timer += Engine.deltaTime * 2;
            for (int i = 0; i < buttonCount; ++i) entity->buttons[i]->matXOff += ((12.0 + i) * (60.0 * Engine.deltaTime));
            entity->matrixX += ((-entity->matrixX) / (5.0 * (60.0 * Engine.deltaTime)));
            entity->matrixZ += ((160.0 - entity->matrixZ) / (5.0 * (60.0 * Engine.deltaTime)));
            entity->rotationY += ((entity->rotYOff - entity->rotationY) / ((60.0 * Engine.deltaTime) * 6.0));
            if (entity->timer <= 0.9)
                break;
            mixFiltersOnJekyll = 1;
            RenderRetroBuffer(64, 160.0);
            if (Engine.gameDeviceType == RETRO_MOBILE) {
                if (activeStageList == STAGELIST_SPECIAL)
                    RenderImage(entity->dpadXSpecial, entity->dpadY, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, 255,
                                entity->textureDPad);
                else
                    RenderImage(entity->dpadX, entity->dpadY, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, 255, entity->textureDPad);
            }
            entity->timer = 0.0;
            ClearNativeObjects();
            CREATE_ENTITY(RetroGameLoop);
            if (Engine.gameDeviceType == RETRO_MOBILE)
                CREATE_ENTITY(VirtualDPad)->pauseAlpha = 255;
            ResumeSound();
            Engine.gameMode = ENGINE_MAINGAME;
            return;
        case 4:
            if (entity->buttons[entity->buttonSelected]->state) {
                switch (entity->buttonSelected) {
                    case PMB_CONTINUE:
                        entity->state   = 3;
                        entity->rotYOff = 0.0;
                        break;
                    case PMB_RESTART:
                        entity->dialog = CREATE_ENTITY(DialogPanel);
                        SetStringToFont(entity->dialog->text, GetGlobalVariableByName("options.gameMode") ? strRestartMessage : strNSRestartMessage,
                                        2);
                        entity->state = 8;
                        break;
                    case PMB_SETTINGS:
                        entity->state        = 5;
                        entity->rotInc       = 0.0;
                        entity->renderRotMax = DegreesToRad(-90.0);
                        for (int i = 0; i < buttonCount; ++i) {
                            entity->rotMax[i]     = DegreesToRad(-90.0);
                            entity->buttonRotY[i] = 0.02 * (i + 1);
                        }

                        break;
                    case PMB_EXIT:
                        entity->dialog = CREATE_ENTITY(DialogPanel);
                        SetStringToFont(entity->dialog->text, GetGlobalVariableByName("options.gameMode") ? strExitMessage : strNSExitMessage, 2);
                        entity->state = 9;
                        SetGlobalVariableByName("timeAttack.result", 0xF4240);
                        break;
#if !RETRO_USE_ORIGINAL_CODE
                    case PMB_DEVMENU:
                        entity->state = 10;
                        entity->timer = 0.0;
                        break;
#endif
                    default: break;
                }
            }
            break;
        case 5:
            if (entity->renderRot > entity->renderRotMax) {
                entity->rotInc -= 0.0025 * (Engine.deltaTime * 60.0);
                entity->renderRot += (Engine.deltaTime * 60.0) * entity->rotInc;
                entity->rotInc -= 0.0025 * (Engine.deltaTime * 60.0);
                matrixRotateYF(&entity->label->renderMatrix, entity->renderRot);
                matrixTranslateXYZF(&entity->matrix, entity->buttonX, 80.0, 160.0);
                matrixMultiplyF(&entity->label->renderMatrix, &entity->matrix);
            }
            for (int i = 0; i < buttonCount; ++i) {
                if (entity->buttonRot[i] > entity->rotMax[i]) {
                    entity->buttonRotY[i] -= 0.0025 * (60.0 * Engine.deltaTime);
                    if (entity->buttonRotY[i] < 0.0) {
                        entity->buttonRot[i] += ((60.0 * Engine.deltaTime) * entity->buttonRotY[i]);
                    }
                    entity->buttonRotY[i] -= 0.0025 * (60.0 * Engine.deltaTime); // do it again ????
                    matrixRotateYF(&entity->buttons[i]->matrix, entity->buttonRot[i]);
                    matrixTranslateXYZF(&entity->matrix, entity->buttonX, 48.0 - i * 30, 160.0);
                    matrixMultiplyF(&entity->buttons[i]->matrix, &entity->matrix);
                }
            }
            if (entity->rotMax[buttonCount - 1] >= entity->buttonRot[buttonCount - 1]) {
                entity->state = 6;

                entity->rotInc       = 0.0;
                entity->renderRotMax = DegreesToRad(22.5);
                for (int i = 0; i < buttonCount; ++i) {
                    entity->rotMax[i]     = DegreesToRad(16.0);
                    entity->buttonRotY[i] = -0.02 * (i + 1);
                }
                if (entity->buttonSelected == 2) {
                    entity->settingsScreen               = CREATE_ENTITY(SettingsScreen);
                    entity->settingsScreen->optionsMenu  = (NativeEntity_OptionsMenu *)entity;
                    entity->settingsScreen->controlStyle = 1;
                }
            }
            entity->matrixX += ((1024.0 - entity->matrixX) / ((60.0 * Engine.deltaTime) * 16.0));
            break;
        case 7: {
            if (entity->renderRotMax > entity->renderRot) {
                entity->rotInc += 0.0025 * (Engine.deltaTime * 60.0);
                entity->renderRot += (Engine.deltaTime * 60.0) * entity->rotInc;
                entity->rotInc += 0.0025 * (Engine.deltaTime * 60.0);
                matrixRotateYF(&entity->label->renderMatrix, entity->renderRot);
                matrixTranslateXYZF(&entity->matrix, entity->buttonX, 80.0, 160.0);
                matrixMultiplyF(&entity->label->renderMatrix, &entity->matrix);
            }

            for (int i = 0; i < buttonCount; ++i) {
                if (entity->rotMax[i] > entity->buttonRot[i]) {
                    entity->buttonRotY[i] += 0.0025 * (60.0 * Engine.deltaTime);
                    if (entity->buttonRotY[i] > 0.0)
                        entity->buttonRot[i] += ((60.0 * Engine.deltaTime) * entity->buttonRotY[i]);
                    entity->buttonRotY[i] += 0.0025 * (60.0 * Engine.deltaTime);
                    if (entity->buttonRot[i] > entity->rotMax[i])
                        entity->buttonRot[i] = entity->rotMax[i];
                    matrixRotateYF(&entity->buttons[i]->matrix, entity->buttonRot[i]);
                    matrixTranslateXYZF(&entity->matrix, entity->buttonX, 48.0 - i * 30, 160.0);
                    matrixMultiplyF(&entity->buttons[i]->matrix, &entity->matrix);
                }
            }
            int matrixX    = entity->width - 16.0;
            int matrixXMin = (((entity->width - 32.0) - entity->matrixX) / ((60.0 * Engine.deltaTime) * 16.0)) + entity->matrixX;
            if (matrixX > matrixXMin) {
                entity->matrixX = matrixX;
                entity->state   = 2;
            }
            else {
                entity->matrixX = matrixXMin;
            }
            break;
        }
        case 8:
            if (entity->dialog->selection == 1) {
                entity->state   = 6;
                Engine.gameMode = ENGINE_EXITPAUSE;
                stageMode       = STAGEMODE_LOAD;
                if (GetGlobalVariableByName("options.gameMode") <= 1) {
                    SetGlobalVariableByName("player.lives", GetGlobalVariableByName("player.lives") - 1);
                }
                SetGlobalVariableByName("lampPostID", 0);
                entity->dialog->state = 5;
                CREATE_ENTITY(FadeScreen);
                break;
            }
            if (entity->dialog->selection != 2)
                break;
            entity->state = 2;
            break;
        case 9:
            if (entity->dialog->selection == 1) {
                entity->state         = 6;
                Engine.gameMode       = (GetGlobalVariableByName("options.gameMode") > 1) + 7;
                entity->dialog->state = 5;
                CREATE_ENTITY(FadeScreen);
            }
            else {
                if (entity->dialog->selection >= 1 && entity->dialog->selection <= 3) {
                    entity->state   = 2;
                    entity->dwordFC = 50;
                }
            }
            break;
#if !RETRO_USE_ORIGINAL_CODE
        case 10:
            entity->timer += Engine.deltaTime;
            if (entity->timer <= 0.5)
                break;
            if (!entity->devMenuFade) {
                entity->devMenuFade        = CREATE_ENTITY(FadeScreen);
                entity->devMenuFade->state = 1;
            }
            if (!entity->devMenuFade->timeLimit || entity->devMenuFade->timer >= entity->devMenuFade->timeLimit) {
                ClearNativeObjects();
                RenderRect(0, 0, 1, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, 255);
                CREATE_ENTITY(RetroGameLoop);
                Engine.gameMode = ENGINE_INITDEVMENU;
                return;
            }
#endif
        default: break;
    }
    SetRenderBlendMode(0);
    NewRenderState();
    matrixRotateYF(&entity->matrix2, entity->rotationY);
    matrixTranslateXYZF(&entity->matrix, entity->matrixX, entity->matrixY, entity->matrixZ);
    matrixMultiplyF(&entity->matrix2, &entity->matrix);
    SetRenderMatrix(&entity->matrix2);
    RenderRect(-SCREEN_CENTERX_F - 4.0, SCREEN_CENTERY_F + 4.0, 0.0, SCREEN_XSIZE_F + 8.0, SCREEN_YSIZE_F + 8.0, 0, 0, 0, 255);
    RenderRetroBuffer(64, 0.0);
    NewRenderState();
    SetRenderMatrix(0);
    if (Engine.gameDeviceType == 1 && entity->state != 6) {
        if (activeStageList == 3)
            RenderImage(entity->dpadXSpecial, entity->dpadY, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, 255, entity->textureDPad);
        else
            RenderImage(entity->dpadX, entity->dpadY, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, 255, entity->textureDPad);
    }
    SetRenderBlendMode(1);
    NewRenderState();
    return;
}