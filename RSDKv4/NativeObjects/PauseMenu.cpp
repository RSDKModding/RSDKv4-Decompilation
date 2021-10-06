#include "RetroEngine.hpp"

int pauseMenuButtonCount;

void PauseMenu_Create(void *objPtr)
{
    RSDK_THIS(PauseMenu);

    // code has been here from TitleScreen_Create due to the possibility of opening the dev menu before this loads :(
#if !RETRO_USE_ORIGINAL_CODE
    int heading = -1, labelTex = -1, textTex = -1;

    if (fontList[FONT_HEADING].count <= 2) {
        if (Engine.useHighResAssets)
            heading = LoadTexture("Data/Game/Menu/Heading_EN.png", TEXFMT_RGBA4444);
        else
            heading = LoadTexture("Data/Game/Menu/Heading_EN@1x.png", TEXFMT_RGBA4444);
        LoadBitmapFont("Data/Game/Menu/Heading_EN.fnt", FONT_HEADING, heading);
    }

    if (fontList[FONT_LABEL].count <= 2) {
        if (Engine.useHighResAssets)
            labelTex = LoadTexture("Data/Game/Menu/Label_EN.png", TEXFMT_RGBA4444);
        else
            labelTex = LoadTexture("Data/Game/Menu/Label_EN@1x.png", TEXFMT_RGBA4444);
        LoadBitmapFont("Data/Game/Menu/Label_EN.fnt", FONT_LABEL, labelTex);
    }

    if (fontList[FONT_TEXT].count <= 2) {
        textTex = LoadTexture("Data/Game/Menu/Text_EN.png", TEXFMT_RGBA4444);
        LoadBitmapFont("Data/Game/Menu/Text_EN.fnt", FONT_TEXT, textTex);
    }

    switch (Engine.language) {
        case RETRO_JP:
            if (heading >= 0) {
                heading = LoadTexture("Data/Game/Menu/Heading_JA@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Heading_JA.fnt", FONT_HEADING, heading);
            }

            if (labelTex >= 0) {
                labelTex = LoadTexture("Data/Game/Menu/Label_JA@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Label_JA.fnt", FONT_LABEL, labelTex);
            }

            if (textTex >= 0) {
                textTex = LoadTexture("Data/Game/Menu/Text_JA@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Text_JA.fnt", FONT_TEXT, textTex);
            }
            break;
        case RETRO_RU:
            if (heading >= 0) {
                if (Engine.useHighResAssets)
                    heading = LoadTexture("Data/Game/Menu/Heading_RU.png", TEXFMT_RGBA4444);
                else
                    heading = LoadTexture("Data/Game/Menu/Heading_RU@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Heading_RU.fnt", FONT_HEADING, heading);
            }

            if (labelTex >= 0) {
                if (Engine.useHighResAssets)
                    labelTex = LoadTexture("Data/Game/Menu/Label_RU.png", TEXFMT_RGBA4444);
                else
                    labelTex = LoadTexture("Data/Game/Menu/Label_RU@1x.png", TEXFMT_RGBA4444);
            }
            break;
        case RETRO_KO:
            if (heading >= 0) {
                heading = LoadTexture("Data/Game/Menu/Heading_KO@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Heading_KO.fnt", FONT_HEADING, heading);
            }

            if (labelTex >= 0) {
                labelTex = LoadTexture("Data/Game/Menu/Label_KO@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Label_KO.fnt", FONT_LABEL, labelTex);
            }

            if (textTex >= 0) {
                textTex = LoadTexture("Data/Game/Menu/Text_KO.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Text_KO.fnt", FONT_TEXT, textTex);
            }
            break;
        case RETRO_ZH:
            if (heading >= 0) {
                heading = LoadTexture("Data/Game/Menu/Heading_ZH@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Heading_ZH.fnt", FONT_HEADING, heading);
            }

            if (labelTex >= 0) {
                labelTex = LoadTexture("Data/Game/Menu/Label_ZH@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Label_ZH.fnt", FONT_LABEL, labelTex);
            }

            if (textTex >= 0) {
                textTex = LoadTexture("Data/Game/Menu/Text_ZH@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Text_ZH.fnt", FONT_TEXT, textTex);
            }
            break;
        case RETRO_ZS:
            if (heading >= 0) {
                heading = LoadTexture("Data/Game/Menu/Heading_ZHS@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Heading_ZHS.fnt", FONT_HEADING, heading);
            }

            if (labelTex >= 0) {
                labelTex = LoadTexture("Data/Game/Menu/Label_ZHS@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Label_ZHS.fnt", FONT_LABEL, labelTex);
            }

            if (textTex >= 0) {
                textTex = LoadTexture("Data/Game/Menu/Text_ZHS@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Text_ZHS.fnt", FONT_TEXT, textTex);
            }
            break;
        default: break;
    }
#endif
    pauseMenuButtonCount = PMB_COUNT;
    if (PMB_COUNT == 5 && !Engine.devMenu)
        pauseMenuButtonCount--;

    entity->retroGameLoop = (NativeEntity_RetroGameLoop *)GetNativeObject(0);
    entity->label         = CREATE_ENTITY(TextLabel);
    entity->label->state  = TEXTLABEL_STATE_IDLE;
    entity->label->z      = 0.0;
    entity->label->scale  = 0.2;
    entity->label->alpha  = 0;
    entity->label->fontID = FONT_HEADING;
    SetStringToFont(entity->label->text, strPause, FONT_HEADING);
    entity->label->alignOffset = 512.0;
    entity->renderRot          = DegreesToRad(22.5);
    matrixRotateYF(&entity->label->renderMatrix, DegreesToRad(22.5));
    matrixTranslateXYZF(&entity->matrix, -128.0, 80.0, 160.0);
    matrixMultiplyF(&entity->label->renderMatrix, &entity->matrix);
    entity->label->useRenderMatrix = true;
    entity->buttonX                = ((SCREEN_CENTERX_F + -160.0) * -0.5) + -128.0;
    for (int i = 0; i < pauseMenuButtonCount; ++i) {
        NativeEntity_SubMenuButton *button = CREATE_ENTITY(SubMenuButton);
        entity->buttons[i]                 = button;
        button->scale                      = 0.1;
        button->matZ                       = 0.0;
        button->matXOff                    = 512.0;
        button->textY                      = -4.0;
        entity->buttonRot[i]               = DegreesToRad(16.0);
        matrixRotateYF(&button->matrix, DegreesToRad(16.0));
        matrixTranslateXYZF(&entity->matrix, entity->buttonX, 48.0 - i * 30, 160.0);
        matrixMultiplyF(&entity->buttons[i]->matrix, &entity->matrix);
        button->symbol    = 1;
        button->useMatrix = true;
    }
    if ((GetGlobalVariableByName("player.lives") <= 1 && GetGlobalVariableByName("options.gameMode") <= 1) || !activeStageList
        || GetGlobalVariableByName("options.attractMode") == 1 || GetGlobalVariableByName("options.vsMode") == 1) {
        entity->buttons[PMB_RESTART]->r = 0x80;
        entity->buttons[PMB_RESTART]->g = 0x80;
        entity->buttons[PMB_RESTART]->b = 0x80;
    }
    SetStringToFont(entity->buttons[PMB_CONTINUE]->text, strContinue, FONT_LABEL);
    SetStringToFont(entity->buttons[PMB_RESTART]->text, strRestart, FONT_LABEL);
    SetStringToFont(entity->buttons[PMB_SETTINGS]->text, strSettings, FONT_LABEL);
    SetStringToFont(entity->buttons[PMB_EXIT]->text, strExit, FONT_LABEL);
    if (pauseMenuButtonCount == 5)
        SetStringToFont(entity->buttons[PMB_DEVMENU]->text, strDevMenu, FONT_LABEL);
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
    entity->state             = PAUSEMENU_STATE_ENTER;
    entity->miniPauseDisabled = true;
    entity->dpadX             = SCREEN_CENTERX_F - 76.0;
    entity->dpadXSpecial      = SCREEN_CENTERX_F - 52.0;
}
void PauseMenu_Main(void *objPtr)
{
    RSDK_THIS(PauseMenu);
    switch (entity->state) {
        case PAUSEMENU_STATE_SETUP: {
            entity->timer += Engine.deltaTime;
            if (entity->timer > 1.0) {
                entity->timer = 0.0;
                entity->state = PAUSEMENU_STATE_ENTER;
            }
            break;
        }
        case PAUSEMENU_STATE_ENTER: {
            if (entity->unusedAlpha < 0x100)
                entity->unusedAlpha += 8;
            entity->timer += Engine.deltaTime * 2;
            entity->label->alignOffset = entity->label->alignOffset / (1.125 * (60.0 * Engine.deltaTime));
            entity->label->alpha       = (256.0 * entity->timer);
            for (int i = 0; i < pauseMenuButtonCount; ++i)
                entity->buttons[i]->matXOff += ((-176.0 - entity->buttons[i]->matXOff) / (16.0 * (60.0 * Engine.deltaTime)));
            entity->matrixX += ((entity->width - entity->matrixX) / ((60.0 * Engine.deltaTime) * 12.0));
            entity->matrixZ += ((512.0 - entity->matrixZ) / ((60.0 * Engine.deltaTime) * 12.0));
            entity->rotationY += ((entity->rotYOff - entity->rotationY) / (16.0 * (60.0 * Engine.deltaTime)));
            if (entity->timer > 1.0) {
                entity->timer = 0.0;
                entity->state = PAUSEMENU_STATE_MAIN;
            }
            break;
        }
        case PAUSEMENU_STATE_MAIN: {
            CheckKeyDown(&inputDown);
            CheckKeyPress(&inputPress);
            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (inputPress.up) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttonSelected--;
                        if (entity->buttonSelected < PMB_CONTINUE)
                            entity->buttonSelected = pauseMenuButtonCount - 1;
                    }
                    else if (inputPress.down) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttonSelected++;
                        if (entity->buttonSelected >= pauseMenuButtonCount)
                            entity->buttonSelected = PMB_CONTINUE;
                    }
                    for (int i = 0; i < pauseMenuButtonCount; ++i) entity->buttons[i]->b = entity->buttons[i]->r;
                    entity->buttons[entity->buttonSelected]->b = 0;
                    if (entity->buttons[entity->buttonSelected]->g > 0x80 && (inputPress.start || inputPress.A)) {
                        PlaySfxByName("Menu Select", false);
                        entity->buttons[entity->buttonSelected]->state = SUBMENUBUTTON_STATE_FLASHING2;
                        entity->buttons[entity->buttonSelected]->b     = 0xFF;
                        entity->state                                  = PAUSEMENU_STATE_ACTION;
                    }
                }
            }
            else {
                for (int i = 0; i < pauseMenuButtonCount; ++i) {
                    if (touches > 0) {
                        if (entity->buttons[i]->g > 0x80)
                            entity->buttons[i]->b = (CheckTouchRect(-80.0, 48.0 - i * 30, 112.0, 12.0) < 0) * 0xFF;
                        else
                            entity->buttons[i]->b = 0x80;
                    }
                    else if (!entity->buttons[i]->b) {
                        entity->buttonSelected = i;
                        PlaySfxByName("Menu Select", false);
                        entity->buttons[i]->state = SUBMENUBUTTON_STATE_FLASHING2;
                        entity->buttons[i]->b     = 0xFF;
                        entity->state             = PAUSEMENU_STATE_ACTION;
                        break;
                    }
                }

                if (entity->state == PAUSEMENU_STATE_MAIN && (inputDown.up || inputDown.down)) {
                    entity->buttonSelected = PMB_CONTINUE;
                    usePhysicalControls    = true;
                }
            }
            if (touches > 0) {
                if (!entity->miniPauseDisabled && CheckTouchRect(SCREEN_CENTERX_F, SCREEN_CENTERY_F, 112.0, 24.0) >= 0) {
                    entity->buttonSelected = PMB_CONTINUE;
                    PlaySfxByName("Resume", false);
                    entity->state = PAUSEMENU_STATE_ACTION;
                    break;
                }
                break;
            }
            entity->miniPauseDisabled = false;
            if (entity->makeSound) {
                PlaySfxByName("Menu Select", false);
                entity->makeSound = false;
            }
            break;
        }
        case PAUSEMENU_STATE_CONTINUE: {
            entity->label->alignOffset += 10.0 * (60.0 * Engine.deltaTime);
            entity->timer += Engine.deltaTime * 2;
            for (int i = 0; i < pauseMenuButtonCount; ++i) entity->buttons[i]->matXOff += ((12.0 + i) * (60.0 * Engine.deltaTime));
            entity->matrixX += ((-entity->matrixX) / (5.0 * (60.0 * Engine.deltaTime)));
            entity->matrixZ += ((160.0 - entity->matrixZ) / (5.0 * (60.0 * Engine.deltaTime)));
            entity->rotationY += ((entity->rotYOff - entity->rotationY) / ((60.0 * Engine.deltaTime) * 6.0));

            if (entity->timer > 0.9) {
                mixFiltersOnJekyll = true;
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
            }
            break;
        }
        case PAUSEMENU_STATE_ACTION: {
            if (entity->buttons[entity->buttonSelected]->state) {
                switch (entity->buttonSelected) {
                    case PMB_CONTINUE:
                        entity->state   = PAUSEMENU_STATE_CONTINUE;
                        entity->rotYOff = 0.0;
                        break;
                    case PMB_RESTART:
                        entity->dialog = CREATE_ENTITY(DialogPanel);
                        SetStringToFont(entity->dialog->text, GetGlobalVariableByName("options.gameMode") ? strRestartMessage : strNSRestartMessage,
                                        FONT_TEXT);
                        entity->state = PAUSEMENU_STATE_RESTART;
                        break;
                    case PMB_SETTINGS:
                        entity->state        = PAUSEMENU_STATE_ENTERSUBMENU;
                        entity->rotInc       = 0.0;
                        entity->renderRotMax = DegreesToRad(-90.0);
                        for (int i = 0; i < pauseMenuButtonCount; ++i) {
                            entity->rotMax[i]     = DegreesToRad(-90.0);
                            entity->buttonRotY[i] = 0.02 * (i + 1);
                        }

                        break;
                    case PMB_EXIT:
                        entity->dialog = CREATE_ENTITY(DialogPanel);
                        SetStringToFont(entity->dialog->text, GetGlobalVariableByName("options.gameMode") ? strExitMessage : strNSExitMessage,
                                        FONT_TEXT);
                        entity->state = PAUSEMENU_STATE_EXIT;
                        if (Engine.gameType == GAME_SONIC1)
                            SetGlobalVariableByName("timeAttack.result", 1000000);
                        break;
#if !RETRO_USE_ORIGINAL_CODE
                    case PMB_DEVMENU:
                        entity->state = PAUSEMENU_STATE_DEVMENU;
                        entity->timer = 0.0;
                        break;
#endif
                    default: break;
                }
            }
            break;
        }
        case PAUSEMENU_STATE_ENTERSUBMENU: {
            if (entity->renderRot > entity->renderRotMax) {
                entity->rotInc -= 0.0025 * (Engine.deltaTime * 60.0);
                entity->renderRot += (Engine.deltaTime * 60.0) * entity->rotInc;
                entity->rotInc -= 0.0025 * (Engine.deltaTime * 60.0);
                matrixRotateYF(&entity->label->renderMatrix, entity->renderRot);
                matrixTranslateXYZF(&entity->matrix, entity->buttonX, 80.0, 160.0);
                matrixMultiplyF(&entity->label->renderMatrix, &entity->matrix);
            }
            for (int i = 0; i < pauseMenuButtonCount; ++i) {
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
            if (entity->rotMax[pauseMenuButtonCount - 1] >= entity->buttonRot[pauseMenuButtonCount - 1]) {
                entity->state = PAUSEMENU_STATE_SUBMENU;

                entity->rotInc       = 0.0;
                entity->renderRotMax = DegreesToRad(22.5);
                for (int i = 0; i < pauseMenuButtonCount; ++i) {
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
        }
        case PAUSEMENU_STATE_SUBMENU: break;
        case PAUSEMENU_STATE_EXITSUBMENU: {
            if (entity->renderRotMax > entity->renderRot) {
                entity->rotInc += 0.0025 * (Engine.deltaTime * 60.0);
                entity->renderRot += (Engine.deltaTime * 60.0) * entity->rotInc;
                entity->rotInc += 0.0025 * (Engine.deltaTime * 60.0);
                matrixRotateYF(&entity->label->renderMatrix, entity->renderRot);
                matrixTranslateXYZF(&entity->matrix, entity->buttonX, 80.0, 160.0);
                matrixMultiplyF(&entity->label->renderMatrix, &entity->matrix);
            }

            for (int i = 0; i < pauseMenuButtonCount; ++i) {
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

            float div = (60.0 * Engine.deltaTime) * 16.0;
            entity->matrixX += (((entity->width - 32.0) - entity->matrixX) / div);
            if (entity->width - 16.0 > entity->matrixX) {
                entity->matrixX = entity->width - 16.0;
                entity->state   = PAUSEMENU_STATE_MAIN;
            }
            break;
        }
        case PAUSEMENU_STATE_RESTART: {
            if (entity->dialog->selection == DLG_YES) {
                entity->state   = PAUSEMENU_STATE_SUBMENU;
                Engine.gameMode = ENGINE_EXITPAUSE;
                stageMode       = STAGEMODE_LOAD;
                if (GetGlobalVariableByName("options.gameMode") <= 1) {
                    SetGlobalVariableByName("player.lives", GetGlobalVariableByName("player.lives") - 1);
                }
                SetGlobalVariableByName("lampPostID", 0);
                SetGlobalVariableByName("starPostID", 0);
                entity->dialog->state = DIALOGPANEL_STATE_IDLE;
                CREATE_ENTITY(FadeScreen);
                break;
            }
            if (entity->dialog->selection == DLG_NO)
                entity->state = PAUSEMENU_STATE_MAIN;
            break;
        }
        case PAUSEMENU_STATE_EXIT: {
            if (entity->dialog->selection == DLG_YES) {
                entity->state         = PAUSEMENU_STATE_SUBMENU;
                Engine.gameMode       = (GetGlobalVariableByName("options.gameMode") > 1) + ENGINE_ENDGAME;
                entity->dialog->state = DIALOGPANEL_STATE_IDLE;
                CREATE_ENTITY(FadeScreen);
            }
            else {
                if (entity->dialog->selection == DLG_YES || entity->dialog->selection == DLG_NO || entity->dialog->selection == DLG_OK) {
                    entity->state   = PAUSEMENU_STATE_MAIN;
                    entity->dwordFC = 50;
                }
            }
            break;
        }
#if !RETRO_USE_ORIGINAL_CODE
        case PAUSEMENU_STATE_DEVMENU:
            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                if (!entity->devMenuFade) {
                    entity->devMenuFade        = CREATE_ENTITY(FadeScreen);
                    entity->devMenuFade->state = FADESCREEN_STATE_FADEOUT;
                }
                if (!entity->devMenuFade->delay || entity->devMenuFade->timer >= entity->devMenuFade->delay) {
                    ClearNativeObjects();
                    RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, 255);
                    CREATE_ENTITY(RetroGameLoop);
                    if (Engine.gameDeviceType == RETRO_MOBILE)
                        CREATE_ENTITY(VirtualDPad);
                    Engine.gameMode = ENGINE_INITDEVMENU;
                    return;
                }
            }
            break;
#endif
        default: break;
    }

    SetRenderBlendMode(RENDER_BLEND_NONE);
    NewRenderState();
    matrixRotateYF(&entity->matrixTemp, entity->rotationY);
    matrixTranslateXYZF(&entity->matrix, entity->matrixX, entity->matrixY, entity->matrixZ);
    matrixMultiplyF(&entity->matrixTemp, &entity->matrix);
    SetRenderMatrix(&entity->matrixTemp);
    RenderRect(-SCREEN_CENTERX_F - 4.0, SCREEN_CENTERY_F + 4.0, 0.0, SCREEN_XSIZE_F + 8.0, SCREEN_YSIZE_F + 8.0, 0, 0, 0, 255);
    RenderRetroBuffer(64, 0.0);
    NewRenderState();
    SetRenderMatrix(NULL);
    if (Engine.gameDeviceType == RETRO_MOBILE && entity->state != PAUSEMENU_STATE_SUBMENU) {
        if (activeStageList == STAGELIST_SPECIAL)
            RenderImage(entity->dpadXSpecial, entity->dpadY, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, 255, entity->textureDPad);
        else
            RenderImage(entity->dpadX, entity->dpadY, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, 255, entity->textureDPad);
    }
    SetRenderBlendMode(RENDER_BLEND_ALPHA);
    NewRenderState();
}
