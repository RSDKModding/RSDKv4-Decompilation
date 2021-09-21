#include "RetroEngine.hpp"

void SettingsScreen_Create(void *objPtr)
{
    RSDK_THIS(SettingsScreen);

    NativeEntity_PushButton *button;
    SaveGame *saveGame = (SaveGame *)saveRAM;

    NativeEntity_TextLabel *label = CREATE_ENTITY(TextLabel);
    entity->label                 = label;
    label->useRenderMatrix        = true;
    label->fontID                 = FONT_HEADING;
    label->scale                  = 0.2;
    label->alpha                  = 256;
    label->x                      = -144.0;
    label->y                      = 100.0;
    label->z                      = 16.0;
    label->state                  = TEXTLABEL_STATE_IDLE;
    SetStringToFont(entity->label->text, strSettings, FONT_HEADING);
    entity->panelMesh = LoadMesh("Data/Game/Models/Panel.bin", -1);
    SetMeshVertexColors(entity->panelMesh, 0, 0, 0, 0xC0);
    entity->arrowsTex = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);
    SetStringToFont(entity->musicText, strMusic, FONT_LABEL);
    SetStringToFont(entity->sfxText, strSoundFX, FONT_LABEL);
    if (Engine.gameType == GAME_SONIC1) {
        SetStringToFont(entity->spindashText, strSpindash, FONT_LABEL);
    }
    SetStringToFont(entity->boxArtText, strBoxArt, FONT_LABEL);
    for (int i = 0; i < 4; ++i) {
        button                   = CREATE_ENTITY(PushButton);
        entity->buttons[i]       = button;
        button->x                = 32.0f + (i % 2) * 76;
        button->y                = 62.0f - (i / 2) * 32;
        button->z                = 0.0;
        button->scale            = 0.175;
        button->bgColour         = 0x00A048;
        button->bgColourSelected = 0x00C060;
        button->useRenderMatrix  = true;
        SetStringToFont8(entity->buttons[i]->text, ((i % 2) ? "+" : "-"), FONT_LABEL);
    }

    button                                   = CREATE_ENTITY(PushButton);
    entity->buttons[SETTINGSSCREEN_BTN_SDON] = button;
    button->x                                = 32.0;
    button->y                                = -2.0;
    button->z                                = 0.0;
    button->scale                            = 0.175;
    button->useRenderMatrix                  = true;
    button->bgColourSelected                 = 0x00C060;
    button->bgColour                         = saveGame->spindashEnabled ? 0x00A048 : 0x006020;
    SetStringToFont(entity->buttons[SETTINGSSCREEN_BTN_SDON]->text, strOn, FONT_LABEL);

    button                                    = CREATE_ENTITY(PushButton);
    entity->buttons[SETTINGSSCREEN_BTN_SDOFF] = button;
    button->x                                 = 96.0;
    button->y                                 = -2.0;
    button->z                                 = 0.0;
    button->scale                             = 0.175;
    button->useRenderMatrix                   = true;
    button->bgColourSelected                  = 0x00A048;
    button->bgColour                          = !saveGame->spindashEnabled ? 0x00A048 : 0x006020;
    SetStringToFont(entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->text, strOff, FONT_LABEL);
    if (Engine.gameType != GAME_SONIC1) {
        entity->buttons[SETTINGSSCREEN_BTN_SDON]->alpha  = 0;
        entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->alpha = 0;
    }

    button                                 = CREATE_ENTITY(PushButton);
    entity->buttons[SETTINGSSCREEN_BTN_JP] = button;
    button->x                              = 4.0;
    button->y                              = -34.0;
    button->z                              = 0.0;
    button->scale                          = 0.175;
    button->useRenderMatrix                = true;
    button->bgColourSelected               = 0x00A048;
    button->bgColour                       = saveGame->boxRegion == REGION_JP ? 0x00A048 : 0x006020;
    SetStringToFont8(entity->buttons[SETTINGSSCREEN_BTN_JP]->text, "JP", FONT_LABEL);

    button                                 = CREATE_ENTITY(PushButton);
    entity->buttons[SETTINGSSCREEN_BTN_US] = button;
    button->x                              = 52.0;
    button->y                              = -34.0;
    button->z                              = 0.0;
    button->scale                          = 0.175;
    button->useRenderMatrix                = true;
    button->bgColourSelected               = 0x00C060;
    button->bgColour                       = saveGame->boxRegion == REGION_US ? 0x00A048 : 0x006020;
    SetStringToFont8(entity->buttons[SETTINGSSCREEN_BTN_US]->text, "US", FONT_LABEL);

    button                                 = CREATE_ENTITY(PushButton);
    entity->buttons[SETTINGSSCREEN_BTN_EU] = button;
    button->x                              = 100.0;
    button->y                              = -34.0;
    button->z                              = 0.0;
    button->scale                          = 0.175;
    button->useRenderMatrix                = true;
    button->bgColourSelected               = 0x00A048;
    button->bgColour                       = saveGame->boxRegion == REGION_EU ? 0x00A048 : 0x006020;
    SetStringToFont8(entity->buttons[SETTINGSSCREEN_BTN_EU]->text, "EU", FONT_LABEL);

    button                                    = CREATE_ENTITY(PushButton);
    entity->buttons[SETTINGSSCREEN_BTN_CTRLS] = button;
    button->useRenderMatrix                   = true;
    button->x                                 = -52.0;
    button->y                                 = -64.0;
    button->z                                 = 0.0;
    button->scale                             = 0.13;
    button->bgColour                          = 0x00A048;
    button->bgColourSelected                  = 0x00C060;
    SetStringToFont(button->text, strControls, FONT_LABEL);

    if (Engine.gameDeviceType == RETRO_MOBILE) {
        switch (GetGlobalVariableByName("options.physicalControls")) {
            default: break;
            case CTRLS_MOGA:
                if (timeAttackTex)
                    ReplaceTexture("Data/Game/Menu/Moga.png", timeAttackTex);
                else
                    entity->controllerTex = LoadTexture("Data/Game/Menu/Moga.png", TEXFMT_RGBA5551);
                break;
            case CTRLS_MOGAPRO:
                if (timeAttackTex)
                    ReplaceTexture("Data/Game/Menu/MogaPro.png", timeAttackTex);
                else
                    entity->controllerTex = LoadTexture("Data/Game/Menu/MogaPro.png", TEXFMT_RGBA5551);
                break;
        }
    }
    else {
        entity->controllerTex = LoadTexture("Data/Game/Menu/Generic.png", TEXFMT_RGBA8888);
    }
}

void SettingsScreen_Main(void *objPtr)
{
    RSDK_THIS(SettingsScreen);

    SaveGame *saveGame                    = (SaveGame *)saveRAM;
    NativeEntity_OptionsMenu *optionsMenu = entity->optionsMenu;

    switch (entity->state) {
        case SETTINGSSCREEN_STATE_ENTER:
            if (entity->alpha <= 255)
                entity->alpha += 8;

            entity->buttonMatScale = fminf(entity->buttonMatScale + ((1.05 - entity->buttonMatScale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            matrixScaleXYZF(&entity->buttonMatrix, entity->buttonMatScale, entity->buttonMatScale, 1.0);
            matrixTranslateXYZF(&entity->tempMatrix, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->buttonMatrix, &entity->tempMatrix);
            SetRenderMatrix(&entity->buttonMatrix);
            if (entity->controlStyle == 1) {
                entity->buttons[SETTINGSSCREEN_BTN_SDON]->y  = -1000.0;
                entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->y = -1000.0;
            }
            entity->label->renderMatrix = entity->buttonMatrix;
            for (int l = 0; l < SETTINGSSCREEN_BTN_COUNT; ++l) entity->buttons[l]->renderMatrix = entity->buttonMatrix;
            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                entity->alpha    = 256;
                entity->timer    = 0.0;
                entity->state    = SETTINGSSCREEN_STATE_MAIN;
                entity->selected = usePhysicalControls == true;
            }
            break;
        case SETTINGSSCREEN_STATE_MAIN:
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->tempMatrix);
            if (usePhysicalControls) {
                if (touches <= 0) {
                    if (keyPress.up) {
                        PlaySfxByName("Menu Move", false);
                        entity->selected--;
                        if ((Engine.gameType != GAME_SONIC1 || entity->controlStyle == 1) && entity->selected == SETTINGSSCREEN_SEL_SPINDASH)
                            entity->selected = SETTINGSSCREEN_SEL_SFXVOL;
                        if (entity->selected <= SETTINGSSCREEN_SEL_NONE)
                            entity->selected = SETTINGSSCREEN_SEL_CONTROLS;
                    }
                    if (keyPress.down) {
                        PlaySfxByName("Menu Move", false);
                        entity->selected++;
                        if ((Engine.gameType != GAME_SONIC1 || entity->controlStyle == 1) && entity->selected == SETTINGSSCREEN_SEL_SPINDASH)
                            entity->selected = SETTINGSSCREEN_SEL_REGION;
                        if (entity->selected > SETTINGSSCREEN_SEL_CONTROLS)
                            entity->selected = SETTINGSSCREEN_SEL_MUSVOL;
                    }
                    for (int i = SETTINGSSCREEN_BTN_SDON; i < SETTINGSSCREEN_BTN_COUNT; ++i) entity->buttons[i]->state = PUSHBUTTON_STATE_UNSELECTED;

                    switch (entity->selected) {
                        case SETTINGSSCREEN_SEL_MUSVOL:
                            entity->buttons[SETTINGSSCREEN_BTN_MUSUP]->state   = keyDown.left == true;
                            entity->buttons[SETTINGSSCREEN_BTN_MUSDOWN]->state = keyDown.right == true;
                            if (keyPress.left) {
                                PlaySfxByName("Menu Move", false);
                                if (saveGame->musVolume > 0)
                                    saveGame->musVolume -= (MAX_VOLUME / 5);
                                SetGameVolumes(saveGame->musVolume, saveGame->sfxVolume);
                                if (!saveGame->musVolume)
                                    musicEnabled = false;
                            }
                            else if (keyPress.right) {
                                PlaySfxByName("Menu Move", false);
                                if (saveGame->musVolume < MAX_VOLUME)
                                    saveGame->musVolume += (MAX_VOLUME / 5);
                                if (!musicEnabled) {
                                    musicEnabled = true;
                                    PlayMusic(0, 0);
                                }
                                SetGameVolumes(saveGame->musVolume, saveGame->sfxVolume);
                            }
                            break;
                        case SETTINGSSCREEN_SEL_SFXVOL:
                            entity->buttons[SETTINGSSCREEN_BTN_SFXUP]->state   = keyDown.left == true;
                            entity->buttons[SETTINGSSCREEN_BTN_SFXDOWN]->state = keyDown.right == true;
                            if (keyPress.left) {
                                PlaySfxByName("Menu Move", false);
                                if (saveGame->sfxVolume > 0)
                                    saveGame->sfxVolume -= (MAX_VOLUME / 5);
                                SetGameVolumes(saveGame->musVolume, saveGame->sfxVolume);
                            }
                            else if (keyPress.right) {
                                PlaySfxByName("Menu Move", false);
                                if (saveGame->sfxVolume < MAX_VOLUME)
                                    saveGame->sfxVolume += (MAX_VOLUME / 5);
                                SetGameVolumes(saveGame->musVolume, saveGame->sfxVolume);
                            }
                            break;
                        case SETTINGSSCREEN_SEL_SPINDASH:
                            if (saveGame->spindashEnabled)
                                entity->buttons[SETTINGSSCREEN_BTN_SDON]->state = PUSHBUTTON_STATE_SELECTED;
                            else
                                entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->state = PUSHBUTTON_STATE_SELECTED;
                            if (keyPress.left || keyPress.right) {
                                PlaySfxByName("Menu Move", false);
                                if (saveGame->spindashEnabled) {
                                    entity->buttons[SETTINGSSCREEN_BTN_SDON]->state             = PUSHBUTTON_STATE_UNSELECTED;
                                    entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->state            = PUSHBUTTON_STATE_SELECTED;
                                    entity->buttons[SETTINGSSCREEN_BTN_SDON]->bgColour          = 0x006020;
                                    entity->buttons[SETTINGSSCREEN_BTN_SDON]->bgColourSelected  = 0x00C060;
                                    entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->bgColour         = 0x00A048;
                                    entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->bgColourSelected = 0x00C060;
                                    saveGame->spindashEnabled                                   = false;
                                }
                                else {
                                    entity->buttons[SETTINGSSCREEN_BTN_SDON]->state             = PUSHBUTTON_STATE_SELECTED;
                                    entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->state            = PUSHBUTTON_STATE_UNSELECTED;
                                    entity->buttons[SETTINGSSCREEN_BTN_SDON]->bgColour          = 0x00A048;
                                    entity->buttons[SETTINGSSCREEN_BTN_SDON]->bgColourSelected  = 0x00C060;
                                    entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->bgColour         = 0x006020;
                                    entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->bgColourSelected = 0x00C060;
                                    saveGame->spindashEnabled                                   = true;
                                }
                            }
                            break;
                        case SETTINGSSCREEN_SEL_REGION:
                            if (keyPress.left || keyPress.right) {
                                if (keyPress.left) {
                                    PlaySfxByName("Menu Move", false);
                                    if (saveGame->boxRegion - 1 >= 0)
                                        saveGame->boxRegion--;
                                }
                                else {
                                    PlaySfxByName("Menu Move", false);
                                    if (saveGame->boxRegion + 1 < 3)
                                        saveGame->boxRegion++;
                                }

                                entity->buttons[SETTINGSSCREEN_BTN_JP]->bgColour                               = 0x006020;
                                entity->buttons[SETTINGSSCREEN_BTN_JP]->bgColourSelected                       = 0x00C060;
                                entity->buttons[SETTINGSSCREEN_BTN_US]->bgColour                               = 0x006020;
                                entity->buttons[SETTINGSSCREEN_BTN_US]->bgColourSelected                       = 0x00C060;
                                entity->buttons[SETTINGSSCREEN_BTN_EU]->bgColour                               = 0x006020;
                                entity->buttons[SETTINGSSCREEN_BTN_EU]->bgColourSelected                       = 0x00C060;
                                entity->buttons[SETTINGSSCREEN_BTN_JP + saveGame->boxRegion]->bgColour         = 0x00A048;
                                entity->buttons[SETTINGSSCREEN_BTN_JP + saveGame->boxRegion]->bgColourSelected = 0x00C060;
                            }
                            entity->buttons[SETTINGSSCREEN_BTN_JP + saveGame->boxRegion]->state = PUSHBUTTON_STATE_SELECTED;
                            break;
                        case SETTINGSSCREEN_SEL_CONTROLS:
                            entity->buttons[SETTINGSSCREEN_BTN_CTRLS]->state = PUSHBUTTON_STATE_SELECTED;
                            if (keyPress.start || keyPress.A) {
                                PlaySfxByName("Menu Select", false);
                                entity->buttons[SETTINGSSCREEN_BTN_CTRLS]->state = PUSHBUTTON_STATE_FLASHING;
                                entity->state                                    = SETTINGSSCREEN_STATE_ENTERCTRLS;
                            }
                            break;
                        default: break;
                    }
                    if (entity->state == SETTINGSSCREEN_STATE_MAIN && keyPress.B) {
                        PlaySfxByName("Menu Back", false);
                        entity->backPressed = false;
                        entity->state       = SETTINGSSCREEN_STATE_EXIT;
                    }
                }
                else {
                    usePhysicalControls = false;
                    entity->selected    = SETTINGSSCREEN_SEL_NONE;
                }
            }
            else {
                if (touches <= 0) {
                    if (entity->buttons[SETTINGSSCREEN_BTN_MUSUP]->state == PUSHBUTTON_STATE_SELECTED) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[SETTINGSSCREEN_BTN_MUSUP]->state = PUSHBUTTON_STATE_UNSELECTED;
                        if (saveGame->musVolume > 0) {
                            saveGame->musVolume -= (MAX_VOLUME / 5);
                        }
                        SetGameVolumes(saveGame->musVolume, saveGame->sfxVolume);
                        if (!saveGame->musVolume)
                            musicEnabled = false;
                    }

                    if (entity->buttons[SETTINGSSCREEN_BTN_MUSDOWN]->state == PUSHBUTTON_STATE_SELECTED) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[SETTINGSSCREEN_BTN_MUSDOWN]->state = PUSHBUTTON_STATE_UNSELECTED;
                        if (saveGame->musVolume < MAX_VOLUME) {
                            saveGame->musVolume += (MAX_VOLUME / 5);
                        }
                        if (!musicEnabled) {
                            musicEnabled = true;
                            PlayMusic(0, 0);
                        }
                        SetGameVolumes(saveGame->musVolume, saveGame->sfxVolume);
                    }

                    if (entity->buttons[SETTINGSSCREEN_BTN_SFXUP]->state == PUSHBUTTON_STATE_SELECTED) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[SETTINGSSCREEN_BTN_SFXUP]->state = PUSHBUTTON_STATE_UNSELECTED;
                        sfxVolume                                        = saveGame->sfxVolume;
                        if (sfxVolume > 0) {
                            sfxVolume -= (MAX_VOLUME / 5);
                            saveGame->sfxVolume = sfxVolume;
                        }
                        SetGameVolumes(saveGame->musVolume, sfxVolume);
                    }

                    if (entity->buttons[SETTINGSSCREEN_BTN_SFXDOWN]->state == PUSHBUTTON_STATE_SELECTED) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[SETTINGSSCREEN_BTN_SFXDOWN]->state = PUSHBUTTON_STATE_UNSELECTED;
                        sfxVolume                                          = saveGame->sfxVolume;
                        if (sfxVolume < MAX_VOLUME) {
                            sfxVolume += (MAX_VOLUME / 5);
                            saveGame->sfxVolume = sfxVolume;
                        }
                        SetGameVolumes(saveGame->musVolume, sfxVolume);
                    }

                    if (entity->buttons[SETTINGSSCREEN_BTN_SDON]->state == PUSHBUTTON_STATE_SELECTED) {
                        PlaySfxByName("Menu Move", false);
                        saveGame->spindashEnabled                                   = true;
                        entity->buttons[SETTINGSSCREEN_BTN_SDON]->state             = PUSHBUTTON_STATE_UNSELECTED;
                        entity->buttons[SETTINGSSCREEN_BTN_SDON]->bgColour          = 0x00A048;
                        entity->buttons[SETTINGSSCREEN_BTN_SDON]->bgColourSelected  = 0x00C060;
                        entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->bgColour         = 0x006020;
                        entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->bgColourSelected = 0x00C060;
                    }

                    if (entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->state == PUSHBUTTON_STATE_SELECTED) {
                        PlaySfxByName("Menu Move", false);
                        saveGame->spindashEnabled                                   = false;
                        entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->state            = PUSHBUTTON_STATE_UNSELECTED;
                        entity->buttons[SETTINGSSCREEN_BTN_SDON]->bgColour          = 0x006020;
                        entity->buttons[SETTINGSSCREEN_BTN_SDON]->bgColourSelected  = 0x00C060;
                        entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->bgColour         = 0x00A048;
                        entity->buttons[SETTINGSSCREEN_BTN_SDOFF]->bgColourSelected = 0x00C060;
                    }

                    if (entity->buttons[SETTINGSSCREEN_BTN_JP]->state == PUSHBUTTON_STATE_SELECTED) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[SETTINGSSCREEN_BTN_JP]->state            = PUSHBUTTON_STATE_UNSELECTED;
                        entity->buttons[SETTINGSSCREEN_BTN_JP]->bgColour         = 0x00A048;
                        entity->buttons[SETTINGSSCREEN_BTN_JP]->bgColourSelected = 0x00C060;
                        entity->buttons[SETTINGSSCREEN_BTN_US]->bgColour         = 0x006020;
                        entity->buttons[SETTINGSSCREEN_BTN_US]->bgColourSelected = 0x00C060;
                        entity->buttons[SETTINGSSCREEN_BTN_EU]->bgColour         = 0x006020;
                        entity->buttons[SETTINGSSCREEN_BTN_EU]->bgColourSelected = 0x00C060;
                        saveGame->boxRegion                                      = REGION_JP;
                    }

                    if (entity->buttons[SETTINGSSCREEN_BTN_US]->state == PUSHBUTTON_STATE_SELECTED) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[SETTINGSSCREEN_BTN_US]->state            = PUSHBUTTON_STATE_UNSELECTED;
                        entity->buttons[SETTINGSSCREEN_BTN_JP]->bgColour         = 0x006020;
                        entity->buttons[SETTINGSSCREEN_BTN_JP]->bgColourSelected = 0x00C060;
                        entity->buttons[SETTINGSSCREEN_BTN_US]->bgColour         = 0x00A048;
                        entity->buttons[SETTINGSSCREEN_BTN_US]->bgColourSelected = 0x00C060;
                        entity->buttons[SETTINGSSCREEN_BTN_EU]->bgColour         = 0x006020;
                        entity->buttons[SETTINGSSCREEN_BTN_EU]->bgColourSelected = 0x00C060;
                        saveGame->boxRegion                                      = REGION_US;
                    }

                    if (entity->buttons[SETTINGSSCREEN_BTN_EU]->state == PUSHBUTTON_STATE_SELECTED) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[SETTINGSSCREEN_BTN_EU]->state            = PUSHBUTTON_STATE_UNSELECTED;
                        entity->buttons[SETTINGSSCREEN_BTN_JP]->bgColour         = 0x006020;
                        entity->buttons[SETTINGSSCREEN_BTN_JP]->bgColourSelected = 0x00C060;
                        entity->buttons[SETTINGSSCREEN_BTN_US]->bgColour         = 0x006020;
                        entity->buttons[SETTINGSSCREEN_BTN_US]->bgColourSelected = 0x00C060;
                        entity->buttons[SETTINGSSCREEN_BTN_EU]->bgColour         = 0x00A048;
                        entity->buttons[SETTINGSSCREEN_BTN_EU]->bgColourSelected = 0x00C060;
                        saveGame->boxRegion                                      = REGION_EU;
                    }
                    if (entity->buttons[SETTINGSSCREEN_BTN_CTRLS]->state == PUSHBUTTON_STATE_SELECTED) {
                        PlaySfxByName("Menu Select", false);
                        entity->buttons[SETTINGSSCREEN_BTN_CTRLS]->state = PUSHBUTTON_STATE_FLASHING;
                        entity->state                                    = SETTINGSSCREEN_STATE_ENTERCTRLS;
                    }
                    if (entity->backPressed || keyPress.B) {
                        PlaySfxByName("Menu Back", false);
                        entity->backPressed = false;
                        entity->state       = SETTINGSSCREEN_STATE_EXIT;
                    }
                    else if (entity->state == SETTINGSSCREEN_STATE_MAIN) {
                        if (keyDown.up) {
                            entity->selected    = SETTINGSSCREEN_SEL_CONTROLS;
                            usePhysicalControls = true;
                        }
                        else if (keyDown.down) {
                            entity->selected    = SETTINGSSCREEN_SEL_MUSVOL;
                            usePhysicalControls = true;
                        }
                    }
                }
                else {
                    float touchX[] = { 32, 108, 32, 108, 32.0, 96, 4, 52, 100, entity->buttons[SETTINGSSCREEN_BTN_CTRLS]->x };
                    float touchY[] = { 54, 54, 22, 22, -10, -10, -42, -42, -42, entity->buttons[SETTINGSSCREEN_BTN_CTRLS]->y };

                    for (int i = 0; i < SETTINGSSCREEN_BTN_COUNT; ++i) {
                        NativeEntity_PushButton *button = entity->buttons[i];

                        if (i == 4 || i == 5) {
                            if (!entity->controlStyle && Engine.gameType == GAME_SONIC1)
                                button->state = CheckTouchRect(touchX[i], touchY[i], (button->textWidth + (button->scale * 64.0)) * 0.75, 12.0) >= 0;
                        }
                        else {
                            button->state = CheckTouchRect(touchX[i], touchY[i], (button->textWidth + (button->scale * 64.0)) * 0.75, 12.0) >= 0;
                        }
                    }

                    entity->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                }
            }
            break;
        case SETTINGSSCREEN_STATE_ENTERCTRLS:
            SetRenderMatrix(&entity->buttonMatrix);
            if (entity->buttons[SETTINGSSCREEN_BTN_CTRLS]->state == PUSHBUTTON_STATE_UNSELECTED) {
                byte physicalControls = GetGlobalVariableByName("options.physicalControls");
                if (Engine.gameDeviceType == RETRO_STANDARD)
                    entity->state = SETTINGSSCREEN_STATE_FLIP_CTRLS;
                else
                    entity->state = (physicalControls == CTRLS_MOGA || physicalControls == CTRLS_MOGAPRO) ? SETTINGSSCREEN_STATE_FLIP_CTRLS
                                                                                                          : SETTINGSSCREEN_STATE_FLIP_CTRLSTOUCH;
            }
            break;
        case SETTINGSSCREEN_STATE_FLIP_CTRLSTOUCH:
            entity->buttonRotY -= (10.0 * Engine.deltaTime);
            if (entity->buttonRotY < -(M_PI / 2)) {
                entity->state      = SETTINGSSCREEN_STATE_FINISHFLIP_CTRLSTOUCH;
                entity->buttonRotY = -(M_PI / 2 + M_PI);
                if (entity->stateDraw == SETTINGSSCREEN_STATEDRAW_CONFIGDPAD) {
                    RestoreNativeObjectsSettings();
                    entity->stateDraw = SETTINGSSCREEN_STATEDRAW_MAIN;
                    SetStringToFont(entity->label->text, strSettings, FONT_HEADING);
                    if (entity->controlStyle == 1)
                        SetGlobalVariableByName("options.touchControls", true);
                }
                else if (entity->stateDraw == SETTINGSSCREEN_STATEDRAW_MAIN) {
                    entity->stateDraw = SETTINGSSCREEN_STATEDRAW_CONFIGDPAD;
                    SetStringToFont(entity->label->text, strCustomizeDPad, FONT_HEADING);
                    BackupNativeObjectsSettings();
                    if (Engine.language == RETRO_JP || Engine.language == RETRO_RU)
                        entity->label->scale = 0.15;
                    else
                        entity->label->scale = 0.2;
                    for (int i = SETTINGSSCREEN_BTN_SDON; i < SETTINGSSCREEN_BTN_COUNT; ++i) RemoveNativeObject(entity->buttons[i]);
                    entity->buttons[SETTINGSSCREEN_BTN_SFXUP]->y               = entity->buttons[SETTINGSSCREEN_BTN_SFXUP]->y + 4.0;
                    entity->buttons[SETTINGSSCREEN_BTN_SFXDOWN]->y             = entity->buttons[SETTINGSSCREEN_BTN_SFXDOWN]->y + 4.0;
                    entity->buttons[SETTINGSSCREEN_BTN_SDON]                   = CREATE_ENTITY(PushButton);
                    entity->buttons[SETTINGSSCREEN_BTN_SDON]->useRenderMatrix  = true;
                    entity->buttons[SETTINGSSCREEN_BTN_SDON]->x                = 88.0;
                    entity->buttons[SETTINGSSCREEN_BTN_SDON]->y                = 6.0;
                    entity->buttons[SETTINGSSCREEN_BTN_SDON]->z                = 0.0;
                    entity->buttons[SETTINGSSCREEN_BTN_SDON]->scale            = 0.175;
                    entity->buttons[SETTINGSSCREEN_BTN_SDON]->bgColour         = 0x00A048;
                    entity->buttons[SETTINGSSCREEN_BTN_SDON]->bgColourSelected = 0x00C060;
                    SetStringToFont8(entity->buttons[SETTINGSSCREEN_BTN_SDON]->text, "RESET", FONT_LABEL);
                    SetStringToFont(entity->musicText, strDPadSize, FONT_LABEL);
                    SetStringToFont(entity->sfxText, strDPadOpacity, FONT_LABEL);
                    entity->virtualDPad           = CREATE_ENTITY(VirtualDPad);
                    entity->virtualDPad           = entity->virtualDPad;
                    entity->virtualDPad->editMode = true;
                    SetGlobalVariableByName("options.touchControls", true);
                }
            }
            NewRenderState();
            matrixRotateYF(&entity->buttonMatrix, entity->buttonRotY);
            matrixTranslateXYZF(&entity->tempMatrix, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->buttonMatrix, &entity->tempMatrix);
            SetRenderMatrix(&entity->buttonMatrix);
            for (int l = 0; l < SETTINGSSCREEN_BTN_COUNT; ++l) entity->buttons[l]->renderMatrix = entity->buttonMatrix;
            break;
        case SETTINGSSCREEN_STATE_FINISHFLIP_CTRLSTOUCH:
            entity->buttonRotY -= (10.0 * Engine.deltaTime);
            if (entity->buttonRotY < -M_PI_2) {
                if (entity->stateDraw == SETTINGSSCREEN_STATEDRAW_CONFIGDPAD)
                    entity->state = SETTINGSSCREEN_STATE_CTRLS_TOUCH;
                else if (entity->stateDraw == SETTINGSSCREEN_STATEDRAW_MAIN)
                    entity->state = SETTINGSSCREEN_STATE_MAIN;
                entity->buttonRotY = 0.0;
            }
            NewRenderState();
            matrixRotateYF(&entity->buttonMatrix, entity->buttonRotY);
            matrixTranslateXYZF(&entity->tempMatrix, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->buttonMatrix, &entity->tempMatrix);
            SetRenderMatrix(&entity->buttonMatrix);
            for (int l = 0; l < SETTINGSSCREEN_BTN_COUNT; ++l) entity->buttons[l]->renderMatrix = entity->buttonMatrix;
            break;
        case SETTINGSSCREEN_STATE_CTRLS_TOUCH:
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->tempMatrix);
            
            if (touches > 0) {

                //Size -
                NativeEntity_PushButton *buttonDec = entity->buttons[SETTINGSSCREEN_BTN_MUSUP];
                if (CheckTouchRect(32.0, 54.0, ((64.0 * buttonDec->scale) + buttonDec->textWidth) * 0.75, 12.0) >= 0) {
                    buttonDec->state = PUSHBUTTON_STATE_SELECTED;
                    if (saveGame->vDPadSize > 0x20)
                        saveGame->vDPadSize -= 4;
                    entity->virtualDPad              = entity->virtualDPad;
                    entity->virtualDPad->moveSize    = saveGame->vDPadSize * (1.0f / 256);
                    entity->virtualDPad->jumpSize    = saveGame->vDPadSize * (1.0f / 256);
                    entity->virtualDPad->pressedSize = saveGame->vDPadSize * (1.0f / 256) * 0.85;
                }
                else {
                    buttonDec->state = PUSHBUTTON_STATE_UNSELECTED;
                }

                //Size +
                NativeEntity_PushButton *buttonInc = entity->buttons[SETTINGSSCREEN_BTN_MUSDOWN];
                if (CheckTouchRect(108.0, 54.0, ((64.0 * buttonInc->scale) + buttonInc->textWidth) * 0.75, 12.0) >= 0) {
                    buttonInc->state = PUSHBUTTON_STATE_SELECTED;
                    if (saveGame->vDPadSize < 0x80)
                        saveGame->vDPadSize += 4;
                    entity->virtualDPad              = entity->virtualDPad;
                    entity->virtualDPad->moveSize    = saveGame->vDPadSize * (1.0f / 256);
                    entity->virtualDPad->jumpSize    = saveGame->vDPadSize * (1.0f / 256);
                    entity->virtualDPad->pressedSize = saveGame->vDPadSize * (1.0f / 256) * 0.85;
                }
                else {
                    entity->buttons[SETTINGSSCREEN_BTN_MUSDOWN]->state = PUSHBUTTON_STATE_UNSELECTED;
                }

                //Opacity -
                buttonDec = entity->buttons[SETTINGSSCREEN_BTN_SFXUP];
                if (CheckTouchRect(32.0, 26.0, ((64.0 * buttonDec->scale) + buttonDec->textWidth) * 0.75, 12.0) >= 0) {
                    buttonDec->state = PUSHBUTTON_STATE_SELECTED;
                    if (saveGame->vDPadOpacity > 0) {
                        saveGame->vDPadOpacity -= 4;
                    }
                }
                else {
                    buttonDec->state = PUSHBUTTON_STATE_UNSELECTED;
                }

                //Opacity +
                buttonInc = entity->buttons[SETTINGSSCREEN_BTN_SFXDOWN];
                if (CheckTouchRect(108.0, 26.0, ((64.0 * buttonInc->scale) + buttonInc->textWidth) * 0.75, 12.0) >= 0) {
                    buttonInc->state = PUSHBUTTON_STATE_SELECTED;
                    if (saveGame->vDPadOpacity < 0x100) {
                        saveGame->vDPadOpacity += 4;
                    }
                }
                else {
                    buttonInc->state = PUSHBUTTON_STATE_UNSELECTED;
                }

                entity->virtualDPad->alpha = saveGame->vDPadOpacity;

                NativeEntity_PushButton *button = entity->buttons[SETTINGSSCREEN_BTN_SDON];
                button->state                   = CheckTouchRect(88.0, -2.0, ((64.0 * button->scale) + button->textWidth) * 0.75, 12.0) >= 0;

                int moveTouch, jumpTouch;

                if (entity->virtualDPad->moveFinger == -1)
                    moveTouch = CheckTouchRect(entity->virtualDPad->moveX, entity->virtualDPad->moveY, 128.0 * entity->virtualDPad->moveSize,
                                               128.0 * entity->virtualDPad->moveSize);
                else
                    moveTouch = CheckTouchRect(-0.5 * SCREEN_CENTERX_F, SCREEN_CENTERY_F * -0.5, 0.5 * SCREEN_CENTERX_F, SCREEN_CENTERY_F * 0.5);

                if (moveTouch >= 0) {
                    if (entity->virtualDPad->moveFinger == -1) {
                        entity->virtualDPad->moveFinger = moveTouch;
                        entity->virtualDPad->relativeX  = entity->virtualDPad->moveX - touchXF[moveTouch];
                        entity->virtualDPad->relativeY  = entity->virtualDPad->moveY - touchYF[moveTouch];
                    }

                    float moveSizeScale        = 128.0 * entity->virtualDPad->moveSize;
                    entity->virtualDPad->moveX = touchXF[moveTouch] + entity->virtualDPad->relativeX;
                    entity->virtualDPad->moveY = touchYF[moveTouch] + entity->virtualDPad->relativeY;
                    if (-SCREEN_CENTERX_F > (entity->virtualDPad->moveX - moveSizeScale)) 
                        entity->virtualDPad->moveX = moveSizeScale - SCREEN_CENTERX_F;
                    if (entity->virtualDPad->moveX + moveSizeScale > 0.0f)
                        entity->virtualDPad->moveX = -moveSizeScale;

                    if (entity->virtualDPad->moveY + moveSizeScale > -8.0f)
                        entity->virtualDPad->moveY = -8.0f - moveSizeScale;

                    if (-SCREEN_CENTERY_F > entity->virtualDPad->moveY - moveSizeScale)
                        entity->virtualDPad->moveY = moveSizeScale - SCREEN_CENTERY_F;
                }
                if (entity->virtualDPad->moveFinger == -1) {
                    if (entity->virtualDPad->jumpFinger == -1)
                        jumpTouch = CheckTouchRect(entity->virtualDPad->jumpX, entity->virtualDPad->jumpY, 128.0 * entity->virtualDPad->jumpSize,
                                                   128.0 * entity->virtualDPad->jumpSize);
                    else
                        jumpTouch = CheckTouchRect(SCREEN_CENTERX_F * 0.5, SCREEN_CENTERY_F * -0.5, SCREEN_CENTERX_F * 0.5, 0.5 * SCREEN_CENTERY_F);

                    if (jumpTouch >= 0) {
                        entity->virtualDPad = entity->virtualDPad;
                        if (entity->virtualDPad->jumpFinger == -1) {
                            entity->virtualDPad->jumpFinger = jumpTouch;
                            entity->virtualDPad->relativeX  = entity->virtualDPad->jumpX - touchXF[jumpTouch];
                            entity->virtualDPad->relativeY  = entity->virtualDPad->jumpY - touchYF[jumpTouch];
                        }

                        float jumpScaleSize        = 128.0 * entity->virtualDPad->jumpSize;
                        entity->virtualDPad->jumpX = touchXF[jumpTouch] + entity->virtualDPad->relativeX;
                        entity->virtualDPad->jumpY = touchYF[jumpTouch] + entity->virtualDPad->relativeY;

                        if (entity->virtualDPad->jumpX - jumpScaleSize < 0.0f)
                            entity->virtualDPad->jumpX = jumpScaleSize;
                        if (entity->virtualDPad->jumpX + jumpScaleSize > SCREEN_CENTERX_F)
                            entity->virtualDPad->jumpX = SCREEN_CENTERX_F - jumpScaleSize;

                        if (entity->virtualDPad->jumpY + jumpScaleSize > -8.0f)
                            entity->virtualDPad->jumpY = -8.0 - jumpScaleSize;
                        if (-SCREEN_CENTERY_F > entity->virtualDPad->jumpY - jumpScaleSize)
                            entity->virtualDPad->jumpY = jumpScaleSize - SCREEN_CENTERY_F;
                    }
                }
                entity->backPressed = CheckTouchRect(136.0, 88.0, 32.0, 16.0) >= 0;
            }
            else {
                entity->virtualDPad                                = entity->virtualDPad;
                entity->virtualDPad->moveFinger                    = -1;
                entity->virtualDPad->jumpFinger                    = -1;
                entity->buttons[SETTINGSSCREEN_BTN_MUSUP]->state   = PUSHBUTTON_STATE_UNSELECTED;
                entity->buttons[SETTINGSSCREEN_BTN_MUSDOWN]->state = PUSHBUTTON_STATE_UNSELECTED;
                entity->buttons[SETTINGSSCREEN_BTN_SFXUP]->state   = PUSHBUTTON_STATE_UNSELECTED;
                entity->buttons[SETTINGSSCREEN_BTN_SFXDOWN]->state = PUSHBUTTON_STATE_UNSELECTED;
                if (entity->backPressed) {
                    PlaySfxByName("Menu Back", false);
                    entity->backPressed = false;
                    entity->state       = SETTINGSSCREEN_STATE_FLIP_CTRLSTOUCH;
                    SetGlobalVariableByName("options.touchControls", false);
                    saveGame->vDPadX_Move = (entity->virtualDPad->moveX + SCREEN_CENTERX_F);
                    saveGame->vDPadY_Move = -(entity->virtualDPad->moveY - SCREEN_CENTERY_F);
                    saveGame->vDPadX_Jump = entity->virtualDPad->jumpX - SCREEN_CENTERX_F;
                    saveGame->vDPadY_Jump = -(entity->virtualDPad->jumpY - SCREEN_CENTERY_F);
                }
                if (entity->buttons[SETTINGSSCREEN_BTN_SDON]->state == PUSHBUTTON_STATE_SELECTED) {
                    entity->buttons[SETTINGSSCREEN_BTN_SDON]->state = PUSHBUTTON_STATE_UNSELECTED;
                    PlaySfxByName("Event", false);
                    saveGame->vDPadSize              = 64;
                    saveGame->vDPadX_Move            = 56;
                    saveGame->vDPadY_Move            = 184;
                    saveGame->vDPadX_Jump            = -56;
                    saveGame->vDPadY_Jump            = 188;
                    saveGame->vDPadOpacity           = 160;
                    entity->virtualDPad->alpha       = 160;
                    entity->virtualDPad->moveX       = saveGame->vDPadX_Move - SCREEN_CENTERX_F;
                    entity->virtualDPad->moveY       = -(saveGame->vDPadY_Move - SCREEN_CENTERY_F);
                    entity->virtualDPad->jumpX       = saveGame->vDPadX_Jump + SCREEN_CENTERX_F;
                    entity->virtualDPad->jumpY       = -(saveGame->vDPadY_Jump - SCREEN_CENTERY_F);
                    entity->virtualDPad->moveFinger  = -1;
                    entity->virtualDPad->jumpFinger  = -1;
                    entity->virtualDPad->moveSize    = saveGame->vDPadSize * (1.0f / 256);
                    entity->virtualDPad->jumpSize    = saveGame->vDPadSize * (1.0f / 256);
                    entity->virtualDPad->pressedSize = saveGame->vDPadSize * (1.0f / 256) * 0.85;
                }
            }

            if (entity->state == SETTINGSSCREEN_STATE_CTRLS_TOUCH && keyPress.B) {
                PlaySfxByName("Menu Back", false);
                entity->backPressed = false;
                entity->state       = SETTINGSSCREEN_STATE_FLIP_CTRLSTOUCH;
                SetGlobalVariableByName("options.touchControls", false);
                saveGame->vDPadX_Move = (entity->virtualDPad->moveX + SCREEN_CENTERX_F);
                saveGame->vDPadY_Move = -(entity->virtualDPad->moveY - SCREEN_CENTERY_F);
                saveGame->vDPadX_Jump = entity->virtualDPad->jumpX - SCREEN_CENTERX_F;
                saveGame->vDPadY_Jump = -(entity->virtualDPad->jumpY - SCREEN_CENTERY_F);
            }
            break;
        case SETTINGSSCREEN_STATE_EXIT: {
            if (entity->alpha > 0)
                entity->alpha -= 8;

            if (entity->timer < 0.2)
                entity->buttonMatScale = fmaxf(entity->buttonMatScale + ((1.5f - entity->buttonMatScale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);
            else
                entity->buttonMatScale = fmaxf(entity->buttonMatScale + ((-1.0f - entity->buttonMatScale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);

            NewRenderState();
            matrixScaleXYZF(&entity->buttonMatrix, entity->buttonMatScale, entity->buttonMatScale, 1.0);
            matrixTranslateXYZF(&entity->tempMatrix, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->buttonMatrix, &entity->tempMatrix);
            SetRenderMatrix(&entity->buttonMatrix);
            entity->label->renderMatrix = entity->buttonMatrix;
            for (int l = 0; l < SETTINGSSCREEN_BTN_COUNT; ++l) entity->buttons[l]->renderMatrix = entity->buttonMatrix;

            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                optionsMenu->state = OPTIONSMENU_STATE_EXITSUBMENU;
                for (int i = 0; i < SETTINGSSCREEN_BTN_COUNT; ++i) RemoveNativeObject(entity->buttons[i]);

                RemoveNativeObject(entity->label);
                RemoveNativeObject(entity);
                Engine.gameMode = ENGINE_MAINGAME;
                if (saveGame->spindashEnabled) {
                    SetGlobalVariableByName("options.originalControls", false);
                    SetGlobalVariableByName("options.airSpeedCap", false);
                }
                else {
                    SetGlobalVariableByName("options.originalControls", true);
                    SetGlobalVariableByName("options.airSpeedCap", true);
                }

                if (Engine.globalBoxRegion != saveGame->boxRegion) {
                    int package = 0;
                    switch (Engine.globalBoxRegion) {
                        case REGION_JP: package = LoadTexture("Data/Game/Models/Package_JP.png", TEXFMT_RGBA8888); break;
                        case REGION_US: package = LoadTexture("Data/Game/Models/Package_US.png", TEXFMT_RGBA8888); break;
                        case REGION_EU: package = LoadTexture("Data/Game/Models/Package_EU.png", TEXFMT_RGBA8888); break;
                    }
                    Engine.globalBoxRegion = saveGame->boxRegion;
                    switch (Engine.globalBoxRegion) {
                        case REGION_JP: ReplaceTexture("Data/Game/Models/Package_JP.png", package); break;
                        case REGION_US: ReplaceTexture("Data/Game/Models/Package_US.png", package); break;
                        case REGION_EU: ReplaceTexture("Data/Game/Models/Package_EU.png", package); break;
                    }
                }
                WriteSaveRAMData();
                return;
            }
            break;
        }
        case SETTINGSSCREEN_STATE_FLIP_CTRLS: {
            entity->buttonRotY -= (10.0 * Engine.deltaTime);
            if (entity->buttonRotY < -(M_PI / 2)) {
                entity->state      = SETTINGSSCREEN_STATE_FINISHFLIP_CTRLS;
                entity->buttonRotY = -(M_PI / 2 + M_PI);
                if (entity->stateDraw == SETTINGSSCREEN_STATEDRAW_CONTROLLER) {
                    RestoreNativeObjectsSettings();
                    entity->stateDraw = SETTINGSSCREEN_STATEDRAW_MAIN;
                    SetStringToFont(entity->label->text, strSettings, FONT_HEADING);
                    if (entity->controlStyle == 1)
                        SetGlobalVariableByName("options.touchControls", true);
                }
                else if (entity->stateDraw == SETTINGSSCREEN_STATEDRAW_MAIN) {
                    entity->stateDraw = SETTINGSSCREEN_STATEDRAW_CONTROLLER;
                    BackupNativeObjectsSettings();
                    SetStringToFont(entity->label->text, strControls, FONT_HEADING);
                    for (int i = 0; i < SETTINGSSCREEN_BTN_COUNT; ++i) RemoveNativeObject(entity->buttons[i]);

                    //???
                    // entity->buttons[SETTINGSSCREEN_BTN_SFXUP]->y += 4.0;
                    // entity->buttons[SETTINGSSCREEN_BTN_SFXDOWN]->y += 4.0;
                }
            }

            NewRenderState();
            matrixRotateYF(&entity->buttonMatrix, entity->buttonRotY);
            matrixTranslateXYZF(&entity->tempMatrix, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->buttonMatrix, &entity->tempMatrix);
            SetRenderMatrix(&entity->buttonMatrix);
            for (int l = 0; l < SETTINGSSCREEN_BTN_COUNT; ++l) entity->buttons[l]->renderMatrix = entity->buttonMatrix;
            break;
        }
        case SETTINGSSCREEN_STATE_FINISHFLIP_CTRLS:
            entity->buttonRotY -= (10.0 * Engine.deltaTime);
            if (entity->buttonRotY < -(M_PI_2)) {
                if (entity->stateDraw == SETTINGSSCREEN_STATEDRAW_CONTROLLER) {
                    entity->state = SETTINGSSCREEN_STATE_CTRLS;
                }
                else if (entity->stateDraw == SETTINGSSCREEN_STATEDRAW_MAIN) {
                    entity->state = SETTINGSSCREEN_STATE_MAIN;
                }
                entity->buttonRotY = 0.0;
            }
            NewRenderState();
            matrixRotateYF(&entity->buttonMatrix, entity->buttonRotY);
            matrixTranslateXYZF(&entity->tempMatrix, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->buttonMatrix, &entity->tempMatrix);
            SetRenderMatrix(&entity->buttonMatrix);
            for (int l = 0; l < SETTINGSSCREEN_BTN_COUNT; ++l) entity->buttons[l]->renderMatrix = entity->buttonMatrix;
            break;
        case SETTINGSSCREEN_STATE_CTRLS:
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->tempMatrix);
            if (touches <= 0) {
                if (entity->backPressed) {
                    PlaySfxByName("Menu Back", false);
                    entity->backPressed = false;
                    entity->state       = SETTINGSSCREEN_STATE_FLIP_CTRLS;
                    break;
                }
            }
            else {
                entity->backPressed = CheckTouchRect(136.0, 88.0, 32.0, 16.0) >= 0;
            }
            if (keyPress.B) {
                PlaySfxByName("Menu Back", false);
                entity->backPressed = false;
                entity->state       = SETTINGSSCREEN_STATE_FLIP_CTRLS;
            }
            break;
        default: break;
    }

    RenderMesh(entity->panelMesh, MESH_COLOURS, false);
    switch (entity->stateDraw) {
        case SETTINGSSCREEN_STATEDRAW_CONFIGDPAD:
            if (Engine.language == RETRO_JP) {
                RenderText(entity->musicText, FONT_LABEL, -128.0, 58.0, 0, 0.09, 255);
                RenderText(entity->sfxText, FONT_LABEL, -128.0, 30.0, 0, 0.09, 255);
            }
            else {
                RenderText(entity->musicText, FONT_LABEL, -128.0, 58.0, 0, 0.125, 255);
                RenderText(entity->sfxText, FONT_LABEL, -128.0, 30.0, 0, 0.125, 255);
            }
            entity->virtualDPad = entity->virtualDPad;
            if (entity->virtualDPad->moveFinger >= 0) {
                RenderRect(-SCREEN_CENTERX_F, 0.0, 0.0, SCREEN_CENTERX_F, SCREEN_CENTERY_F, 255, 0, 0, 64);
                entity->virtualDPad = entity->virtualDPad;
            }
            if (entity->virtualDPad->jumpFinger >= 0)
                RenderRect(0.0, 0.0, 0.0, SCREEN_CENTERX_F, SCREEN_CENTERY_F, 255, 0, 0, 64);
            break;
        case SETTINGSSCREEN_STATEDRAW_CONTROLLER:
            RenderImage(0.0, 0.0, 0.0, 0.275, 0.275, 512.0, 256.0, 1024.0, 512.0, 0.0, 0.0, 255, entity->controllerTex);
            break;
        case SETTINGSSCREEN_STATEDRAW_MAIN:
            if (entity->selected == SETTINGSSCREEN_SEL_MUSVOL)
                SetRenderVertexColor(0xFF, 0xFF, 0x00);
            else
                SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            RenderText(entity->musicText, FONT_LABEL, -128.0, 58.0, 0, 0.125, 255);

            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            for (int i = 0; i < 5; i++) {
                int v = (saveGame->musVolume <= i * 20) ? 128 : 255;
                RenderRect(55.0 + i * 6, 56.0 + i * 4, 0.0, 4.0, 4.0 + i * 4, v, v, v, 255);
                v = (saveGame->sfxVolume <= i * 20) ? 128 : 255;
                RenderRect(55.0 + i * 6, 24.0 + i * 4, 0.0, 4.0, 4.0 + i * 4, v, v, v, 255);
            }

            if (entity->selected == SETTINGSSCREEN_SEL_SFXVOL)
                SetRenderVertexColor(0xFF, 0xFF, 0x00);
            else
                SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            if (Engine.language == RETRO_RU)
                RenderText(entity->sfxText, FONT_LABEL, -128.0, 26.0, 0, 0.09, 255);
            else
                RenderText(entity->sfxText, FONT_LABEL, -128.0, 26.0, 0, 0.125, 255);

            if (entity->selected == SETTINGSSCREEN_SEL_SPINDASH)
                SetRenderVertexColor(0xFF, 0xFF, 0x00);
            else
                SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            if (!entity->controlStyle && Engine.gameType == GAME_SONIC1) {
                if ((Engine.language - 1) <= 6 && ((1 << (Engine.language - 1)) & 0x43))
                    RenderText(entity->spindashText, FONT_LABEL, -128.0, -6.0, 0, 0.09, 255);
                else
                    RenderText(entity->spindashText, FONT_LABEL, -128.0, -6.0, 0, 0.125, 255);
            }

            if (entity->selected == SETTINGSSCREEN_SEL_REGION)
                SetRenderVertexColor(0xFF, 0xFF, 0x00);
            else
                SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            RenderText(entity->boxArtText, FONT_LABEL, -128.0, -38.0, 0, 0.125, 255);
            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            break;
    }
    NewRenderState();
    SetRenderMatrix(NULL);

    switch (entity->stateDraw) {
        case SETTINGSSCREEN_STATEDRAW_MAIN:
            RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, entity->backPressed ? 128.0 : 0, entity->alpha,
                        entity->arrowsTex);
            break;
        case SETTINGSSCREEN_STATEDRAW_CONFIGDPAD:
        case SETTINGSSCREEN_STATEDRAW_CONTROLLER:
            RenderImage(136.0, 88.0, 160.0, 0.25, 0.25, 64.0, 64.0, 128.0, 128.0, 128.0, entity->backPressed ? 128.0 : 0, entity->alpha,
                        entity->arrowsTex);
            break;
    }
}
