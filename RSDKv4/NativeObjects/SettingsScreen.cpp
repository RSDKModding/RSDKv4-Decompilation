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
    label->state                  = 0;
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

    button                   = CREATE_ENTITY(PushButton);
    entity->buttons[4]       = button;
    button->x                = 32.0;
    button->y                = -2.0;
    button->z                = 0.0;
    button->scale            = 0.175;
    button->useRenderMatrix  = true;
    button->bgColourSelected = 0x00C060;
    button->bgColour         = saveGame->spindashEnabled ? 0x00A048 : 0x006020;
    SetStringToFont(entity->buttons[4]->text, strOn, FONT_LABEL);

    button                   = CREATE_ENTITY(PushButton);
    entity->buttons[5]       = button;
    button->x                = 96.0;
    button->y                = -2.0;
    button->z                = 0.0;
    button->scale            = 0.175;
    button->useRenderMatrix  = true;
    button->bgColourSelected = 0x00A048;
    button->bgColour         = !saveGame->spindashEnabled ? 0x00A048 : 0x006020;
    SetStringToFont(entity->buttons[5]->text, strOff, FONT_LABEL);
    if (Engine.gameType != GAME_SONIC1) {
        entity->buttons[4]->alpha = 0;
        entity->buttons[5]->alpha = 0;
    }

    button                   = CREATE_ENTITY(PushButton);
    entity->buttons[6]       = button;
    button->x                = 4.0;
    button->y                = -34.0;
    button->z                = 0.0;
    button->scale            = 0.175;
    button->useRenderMatrix  = true;
    button->bgColourSelected = 0x00A048;
    button->bgColour         = saveGame->boxRegion == REGION_JP ? 0x00A048 : 0x006020;
    SetStringToFont8(entity->buttons[6]->text, "JP", FONT_LABEL);

    button                   = CREATE_ENTITY(PushButton);
    entity->buttons[7]       = button;
    button->x                = 52.0;
    button->y                = -34.0;
    button->z                = 0.0;
    button->scale            = 0.175;
    button->useRenderMatrix  = true;
    button->bgColourSelected = 0x00C060;
    button->bgColour         = saveGame->boxRegion == REGION_US ? 0x00A048 : 0x006020;
    SetStringToFont8(entity->buttons[7]->text, "US", FONT_LABEL);

    button                   = CREATE_ENTITY(PushButton);
    entity->buttons[8]       = button;
    button->x                = 100.0;
    button->y                = -34.0;
    button->z                = 0.0;
    button->scale            = 0.175;
    button->useRenderMatrix  = true;
    button->bgColourSelected = 0x00A048;
    button->bgColour         = saveGame->boxRegion == REGION_EU ? 0x00A048 : 0x006020;
    SetStringToFont8(entity->buttons[8]->text, "EU", FONT_LABEL);

    button                   = CREATE_ENTITY(PushButton);
    entity->buttons[9]       = button;
    button->useRenderMatrix  = true;
    button->x                = -52.0;
    button->y                = -64.0;
    button->z                = 0.0;
    button->scale            = 0.13;
    button->bgColour         = 0x00A048;
    button->bgColourSelected = 0x00C060;
    SetStringToFont(button->text, strControls, FONT_LABEL);

    if (Engine.gameDeviceType == RETRO_MOBILE) {
        switch (GetGlobalVariableByName("options.physicalControls")) {
            default: break;
            case 2:
                if (timeAttackTex)
                    ReplaceTexture("Data/Game/Menu/Moga.png", timeAttackTex);
                else
                    entity->controllerTex = LoadTexture("Data/Game/Menu/Moga.png", TEXFMT_RGBA5551);
                break;
            case 3:
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
        case 0:
            if (entity->alpha <= 255)
                entity->alpha += 8;

            entity->buttonMatScale = fminf(entity->buttonMatScale + ((1.05 - entity->buttonMatScale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            matrixScaleXYZF(&entity->buttonMatrix, entity->buttonMatScale, entity->buttonMatScale, 1.0);
            matrixTranslateXYZF(&entity->tempMatrix, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->buttonMatrix, &entity->tempMatrix);
            SetRenderMatrix(&entity->buttonMatrix);
            if (entity->controlStyle == 1) {
                entity->buttons[4]->y = -1000.0;
                entity->buttons[5]->y = -1000.0;
            }
            entity->label->renderMatrix = entity->buttonMatrix;
            for (int l = 0; l != 10; ++l) entity->buttons[l]->renderMatrix = entity->buttonMatrix;
            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                entity->alpha    = 256;
                entity->timer    = 0.0;
                entity->state    = 1;
                entity->selected = usePhysicalControls == true;
            }
            break;
        case 1:
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->tempMatrix);
            if (usePhysicalControls) {
                if (touches <= 0) {
                    if (keyPress.up) {
                        PlaySfxByName("Menu Move", false);
                        entity->selected--;
                        if (Engine.gameType != GAME_SONIC1 && entity->selected == 3)
                            entity->selected = 2;
                        if (entity->controlStyle == 1 && entity->selected == 4)
                            entity->selected = 2;
                        if (entity->selected <= 0)
                            entity->selected = 5;
                    }
                    if (keyPress.down) {
                        PlaySfxByName("Menu Move", false);
                        entity->selected++;
                        if (Engine.gameType != GAME_SONIC1 && entity->selected == 3)
                            entity->selected = 4;
                        if (entity->controlStyle == 1 && entity->selected == 2)
                            entity->selected = 4;
                        if (entity->selected > 5)
                            entity->selected = 1;
                    }
                    for (int i = 4; i < 10; ++i) entity->buttons[i]->state = 0;

                    switch (entity->selected) {
                        case 1:
                            entity->buttons[0]->state = keyDown.left == 1;
                            entity->buttons[1]->state = keyDown.right == 1;
                            if (keyPress.left == 1) {
                                PlaySfxByName("Menu Move", false);
                                if (saveGame->musVolume > 0)
                                    saveGame->musVolume -= (MAX_VOLUME / 5);
                                SetGameVolumes(saveGame->musVolume, saveGame->sfxVolume);
                                if (!saveGame->musVolume)
                                    musicEnabled = 0;
                                if (entity->state == 1 && keyPress.B) {
                                    PlaySfxByName("Menu Back", false);
                                    entity->backPressed = 0;
                                    entity->state       = 6;
                                }
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
                                if (entity->state == 1 && keyPress.B) {
                                    PlaySfxByName("Menu Back", false);
                                    entity->backPressed = 0;
                                    entity->state       = 6;
                                }
                            }
                            else {
                                if (entity->state == 1 && keyPress.B) {
                                    PlaySfxByName("Menu Back", false);
                                    entity->backPressed = 0;
                                    entity->state       = 6;
                                }
                            }
                            break;
                        case 2:
                            entity->buttons[2]->state = keyDown.left == true;
                            entity->buttons[3]->state = keyDown.right == true;
                            if (keyPress.left) {
                                PlaySfxByName("Menu Move", false);
                                if (saveGame->sfxVolume > 0)
                                    saveGame->sfxVolume -= (MAX_VOLUME / 5);
                            }
                            else {
                                if (keyPress.right) {
                                    PlaySfxByName("Menu Move", false);
                                    if (saveGame->sfxVolume < MAX_VOLUME)
                                        saveGame->sfxVolume += (MAX_VOLUME / 5);
                                }
                                else {
                                    if (entity->state == 1 && keyPress.B) {
                                        PlaySfxByName("Menu Back", false);
                                        entity->backPressed = 0;
                                        entity->state       = 6;
                                    }
                                    break;
                                }
                            }
                            SetGameVolumes(saveGame->musVolume, saveGame->sfxVolume);
                            if (entity->state == 1 && keyPress.B) {
                                PlaySfxByName("Menu Back", false);
                                entity->backPressed = 0;
                                entity->state       = 6;
                            }
                            break;
                        case 3:
                            if (saveGame->spindashEnabled)
                                entity->buttons[4]->state = 1;
                            else
                                entity->buttons[5]->state = 1;
                            if (keyPress.left || keyPress.right) {
                                PlaySfxByName("Menu Move", false);
                                if (saveGame->spindashEnabled) {
                                    entity->buttons[4]->state            = 0;
                                    entity->buttons[5]->state            = 1;
                                    saveGame->spindashEnabled            = 0;
                                    entity->buttons[4]->bgColour         = 0x006020;
                                    entity->buttons[4]->bgColourSelected = 0x00C060;
                                    entity->buttons[5]->bgColour         = 0x00A048;
                                }
                                else {
                                    entity->buttons[4]->state            = 1;
                                    entity->buttons[5]->state            = 0;
                                    saveGame->spindashEnabled            = 1;
                                    entity->buttons[4]->bgColour         = 0x00A048;
                                    entity->buttons[4]->bgColourSelected = 0x00C060;
                                    entity->buttons[5]->bgColour         = 0x006020;
                                }
                                entity->buttons[5]->bgColourSelected = 0x00C060;
                            }
                            if (entity->state == 1 && keyPress.B) {
                                PlaySfxByName("Menu Back", false);
                                entity->backPressed = 0;
                                entity->state       = 6;
                            }
                            break;
                        case 4:
                            if (keyPress.left || keyPress.right) {
                                int boxRegion;
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

                                entity->buttons[6]->bgColour                               = 0x006020;
                                entity->buttons[6]->bgColourSelected                       = 0x00C060;
                                entity->buttons[7]->bgColour                               = 0x006020;
                                entity->buttons[7]->bgColourSelected                       = 0x00C060;
                                entity->buttons[8]->bgColour                               = 0x006020;
                                entity->buttons[8]->bgColourSelected                       = 0x00C060;
                                entity->buttons[saveGame->boxRegion + 6]->bgColour         = 0x00A048;
                                entity->buttons[saveGame->boxRegion + 6]->bgColourSelected = 0x00C060;
                            }
                            entity->buttons[saveGame->boxRegion + 6]->state = 1;
                            if (entity->state == 1 && keyPress.B) {
                                PlaySfxByName("Menu Back", false);
                                entity->backPressed = 0;
                                entity->state       = 6;
                            }
                            break;
                        case 5:
                            entity->buttons[9]->state = 1;
                            if (keyPress.start || keyPress.A) {
                                PlaySfxByName("Menu Select", false);
                                entity->buttons[9]->state = 2;
                                entity->state             = 2;
                            }
                            else {
                                if (entity->state == 1 && keyPress.B) {
                                    PlaySfxByName("Menu Back", false);
                                    entity->backPressed = 0;
                                    entity->state       = 6;
                                }
                                break;
                            }
                            break;
                        default:
                            if (entity->state == 1 && keyPress.B) {
                                PlaySfxByName("Menu Back", false);
                                entity->backPressed = 0;
                                entity->state       = 6;
                            }
                            break;
                    }
                }
                else {
                    usePhysicalControls = false;
                    entity->selected    = 0;
                }
            }
            else {
                if (touches <= 0) {
                    if (entity->buttons[0]->state == 1) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[0]->state = 0;
                        if (saveGame->musVolume > 0) {
                            saveGame->musVolume -= (MAX_VOLUME / 5);
                        }
                        SetGameVolumes(saveGame->musVolume, saveGame->sfxVolume);
                        if (!saveGame->musVolume)
                            musicEnabled = false;
                    }
                    if (entity->buttons[1]->state == 1) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[1]->state = 0;
                        if (saveGame->musVolume < MAX_VOLUME) {
                            saveGame->musVolume += (MAX_VOLUME / 5);
                        }
                        if (!musicEnabled) {
                            musicEnabled = true;
                            PlayMusic(0, 0);
                        }
                        SetGameVolumes(saveGame->musVolume, saveGame->sfxVolume);
                    }
                    if (entity->buttons[2]->state == 1) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[2]->state = 0;
                        sfxVolume                 = saveGame->sfxVolume;
                        if (sfxVolume > 0) {
                            sfxVolume -= (MAX_VOLUME / 5);
                            saveGame->sfxVolume = sfxVolume;
                        }
                        SetGameVolumes(saveGame->musVolume, sfxVolume);
                    }
                    if (entity->buttons[3]->state == 1) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[3]->state = 0;
                        sfxVolume                 = saveGame->sfxVolume;
                        if (sfxVolume < MAX_VOLUME) {
                            sfxVolume += (MAX_VOLUME / 5);
                            saveGame->sfxVolume = sfxVolume;
                        }
                        SetGameVolumes(saveGame->musVolume, sfxVolume);
                    }
                    if (entity->buttons[4]->state == 1) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[4]->state            = 0;
                        saveGame->spindashEnabled            = true;
                        entity->buttons[4]->bgColour         = 0x00A048;
                        entity->buttons[4]->bgColourSelected = 0x00C060;
                        entity->buttons[5]->bgColour         = 0x006020;
                        entity->buttons[5]->bgColourSelected = 0x00C060;
                    }
                    if (entity->buttons[5]->state == 1) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[5]->state            = 0;
                        saveGame->spindashEnabled            = 0;
                        entity->buttons[4]->bgColour         = 0x006020;
                        entity->buttons[4]->bgColourSelected = 0x00C060;
                        entity->buttons[5]->bgColour         = 0x00A048;
                        entity->buttons[5]->bgColourSelected = 0x00C060;
                    }
                    if (entity->buttons[6]->state == 1) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[6]->state            = 0;
                        entity->buttons[6]->bgColour         = 0x00A048;
                        entity->buttons[6]->bgColourSelected = 0x00C060;
                        entity->buttons[7]->bgColour         = 0x006020;
                        entity->buttons[7]->bgColourSelected = 0x00C060;
                        entity->buttons[8]->bgColour         = 0x006020;
                        entity->buttons[8]->bgColourSelected = 0x00C060;
                        saveGame->boxRegion                  = REGION_JP;
                    }

                    if (entity->buttons[7]->state == 1) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[7]->state            = 0;
                        entity->buttons[6]->bgColour         = 0x006020;
                        entity->buttons[6]->bgColourSelected = 0x00C060;
                        entity->buttons[7]->bgColour         = 0x00A048;
                        entity->buttons[7]->bgColourSelected = 0x00C060;
                        entity->buttons[8]->bgColour         = 0x006020;
                        entity->buttons[8]->bgColourSelected = 0x00C060;
                        saveGame->boxRegion                  = REGION_US;
                    }

                    if (entity->buttons[8]->state == 1) {
                        PlaySfxByName("Menu Move", false);
                        entity->buttons[8]->state            = 0;
                        entity->buttons[6]->bgColour         = 0x006020;
                        entity->buttons[6]->bgColourSelected = 0x00C060;
                        entity->buttons[7]->bgColour         = 0x006020;
                        entity->buttons[7]->bgColourSelected = 0x00C060;
                        entity->buttons[8]->bgColour         = 0x00A048;
                        entity->buttons[8]->bgColourSelected = 0x00C060;
                        saveGame->boxRegion                  = REGION_EU;
                    }
                    if (entity->buttons[9]->state == 1) {
                        PlaySfxByName("Menu Select", false);
                        entity->buttons[9]->state = 2;
                        entity->state             = 2;
                    }
                    if (entity->backPressed || keyPress.B) {
                        PlaySfxByName("Menu Back", false);
                        entity->backPressed = 0;
                        entity->state       = 6;
                    }
                    else if (entity->state == 1) {
                        if (keyDown.up) {
                            entity->selected    = 5;
                            usePhysicalControls = true;
                        }
                        else if (keyDown.down) {
                            entity->selected    = 1;
                            usePhysicalControls = true;
                        }
                    }
                }
                else { // pain inc.
                    NativeEntity_PushButton *button = entity->buttons[0];
                    int touchBuf                    = CheckTouchRect(32.0, 54.0, (button->textWidth + (button->scale * 64.0)) * 0.75, 12.0);

                    if (touchBuf < 0) {
                        button->state = 0;
                        touchBuf = CheckTouchRect(108.0, 54.0, (entity->buttons[1]->textWidth + (entity->buttons[1]->scale * 64.0)) * 0.75, 12.0);
                        button   = entity->buttons[1];
                    }
                    button->state = touchBuf >= 0;

                    touchBuf      = CheckTouchRect(32.0, 22.0, (entity->buttons[2]->textWidth + (entity->buttons[2]->scale * 64.0)) * 0.75, 12.0);
                    button        = entity->buttons[2];
                    if (touchBuf < 0) {
                        button->state = 0;
                        touchBuf = CheckTouchRect(108.0, 22.0, (entity->buttons[3]->textWidth + (entity->buttons[3]->scale * 64.0)) * 0.75, 12.0);
                        button   = entity->buttons[3];
                    }
                    button->state = touchBuf >= 0;

                    if (!entity->controlStyle && Engine.gameType == GAME_SONIC1) {
                        touchBuf = CheckTouchRect(32.0, -10.0, (entity->buttons[4]->textWidth + (entity->buttons[4]->scale * 64.0)) * 0.75, 12.0);
                        button   = entity->buttons[4];
                        if (touchBuf < 0) {
                            button->state = 0;
                            touchBuf = CheckTouchRect(96.0, -10.0, (entity->buttons[5]->textWidth + (entity->buttons[5]->scale * 64.0)) * 0.75, 12.0);
                            button   = entity->buttons[5];
                        }
                        button->state = touchBuf >= 0;
                    }

                    touchBuf = CheckTouchRect(4.0, -42.0, (entity->buttons[6]->textWidth + (entity->buttons[6]->scale * 64.0)) * 0.75, 12.0);
                    button   = entity->buttons[6];
                    if (touchBuf < 0) {
                        button->state = 0,
                        touchBuf = CheckTouchRect(52.0, -42.0, (entity->buttons[7]->textWidth + (entity->buttons[7]->scale * 64.0)) * 0.75, 12.0),
                        button   = entity->buttons[7];
                    }
                    if (touchBuf < 0) {
                        button->state = 0;
                        touchBuf = CheckTouchRect(100.0, -42.0, (entity->buttons[8]->textWidth + (entity->buttons[8]->scale * 64.0)) * 0.75, 12.0);
                        button   = entity->buttons[8];
                    }
                    button->state             = touchBuf >= 0;
                    entity->buttons[9]->state = CheckTouchRect(entity->buttons[9]->x, entity->buttons[9]->y,
                                                               (entity->buttons[9]->textWidth + (entity->buttons[9]->scale * 64.0)) * 0.75, 12.0)
                                                >= 0;

                    entity->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                }
            }
            break;
        case 2:
            SetRenderMatrix(&entity->buttonMatrix);
            if (entity->buttons[9]->state)
                break;
            if (Engine.gameDeviceType == RETRO_STANDARD) {
                entity->state = 7;
                break;
            }
            entity->state = (GetGlobalVariableByName("options.physicalControls") - 2) < 2 ? 7 : 3;
            break;
        case 3:
            entity->buttonRotY -= (10.0 * Engine.deltaTime);
            if (entity->buttonRotY < -(M_PI / 2)) {
                entity->state      = 4;
                entity->buttonRotY = -(M_PI / 2 + M_PI);
                if (entity->subState) {
                    if (entity->subState == 1) {
                        RestoreNativeObjectsSettings();
                        entity->subState = 0;
                        SetStringToFont(entity->label->text, strSettings, FONT_HEADING);
                        if (entity->controlStyle == 1)
                            SetGlobalVariableByName("options.touchControls", true);
                    }
                }
                else {
                    entity->subState = 1;
                    SetStringToFont(entity->label->text, strCustomizeDPad, FONT_HEADING);
                    BackupNativeObjectsSettings();
                    if (Engine.language == RETRO_JP || Engine.language == RETRO_RU)
                        entity->label->scale = 0.15;
                    else
                        entity->label->scale = 0.2;
                    for (int i = 4; i < 10; ++i) RemoveNativeObject(entity->buttons[i]);
                    entity->buttons[2]->y                = entity->buttons[2]->y + 4.0;
                    entity->buttons[3]->y                = entity->buttons[3]->y + 4.0;
                    entity->buttons[4]                   = CREATE_ENTITY(PushButton);
                    entity->buttons[4]->useRenderMatrix  = true;
                    entity->buttons[4]->x                = 88.0;
                    entity->buttons[4]->y                = 6.0;
                    entity->buttons[4]->z                = 0.0;
                    entity->buttons[4]->scale            = 0.175;
                    entity->buttons[4]->bgColour         = 0x00A048;
                    entity->buttons[4]->bgColourSelected = 0x00C060;
                    SetStringToFont8(entity->buttons[4]->text, "RESET", FONT_LABEL);
                    SetStringToFont(entity->musicText, strDPadSize, FONT_LABEL);
                    SetStringToFont(entity->sfxText, strDPadOpacity, FONT_LABEL);
                    entity->virtualDPad           = CREATE_ENTITY(VirtualDPad);
                    entity->virtualDPad           = entity->virtualDPad;
                    entity->virtualDPad->editMode = 1;
                    SetGlobalVariableByName("options.touchControls", true);
                }
            }
            NewRenderState();
            matrixRotateYF(&entity->buttonMatrix, entity->buttonRotY);
            matrixTranslateXYZF(&entity->tempMatrix, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->buttonMatrix, &entity->tempMatrix);
            SetRenderMatrix(&entity->buttonMatrix);
            for (int l = 0; l < 10; ++l) entity->buttons[l]->renderMatrix = entity->buttonMatrix;
            break;
        case 4:
            entity->buttonRotY -= (10.0 * Engine.deltaTime);
            if (entity->buttonRotY < -(2 * M_PI)) {
                if (entity->subState) {
                    if (entity->subState == 1)
                        entity->state = 5;
                }
                else {
                    entity->state = 1;
                }
                entity->buttonRotY = 0.0;
            }
            NewRenderState();
            matrixRotateYF(&entity->buttonMatrix, entity->buttonRotY);
            matrixTranslateXYZF(&entity->tempMatrix, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->buttonMatrix, &entity->tempMatrix);
            SetRenderMatrix(&entity->buttonMatrix);
            for (int l = 0; l < 10; ++l) entity->buttons[l]->renderMatrix = entity->buttonMatrix;
            break;
        case 5:
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->tempMatrix);
            int touchCheck;
            if (touches > 0) {
                touchCheck = CheckTouchRect(32.0, 54.0, ((64.0 * entity->buttons[0]->scale) + entity->buttons[0]->textWidth) * 0.75, 12.0) < 0;
                if (touchCheck) {
                    entity->buttons[0]->state = 0;
                    touchCheck = CheckTouchRect(108.0, 54.0, ((64.0 * entity->buttons[1]->scale) + entity->buttons[1]->textWidth) * 0.75, 12.0) < 0;
                    if (touchCheck) {
                        entity->buttons[1]->state = 0;
                    }
                    else {
                        entity->buttons[1]->state = 1;
                        if (saveGame->vDPadSize < 0x80)
                            saveGame->vDPadSize += 4;
                        entity->virtualDPad              = entity->virtualDPad;
                        entity->virtualDPad->moveSize    = saveGame->vDPadSize * (1.0f / 256);
                        entity->virtualDPad->jumpSize    = saveGame->vDPadSize * (1.0f / 256);
                        entity->virtualDPad->unknownSize = saveGame->vDPadSize * (1.0f / 256) * 0.85;
                    }
                }
                else {
                    entity->buttons[0]->state = 1;
                    if (saveGame->vDPadSize > 0x20)
                        saveGame->vDPadSize -= 4;
                    entity->virtualDPad              = entity->virtualDPad;
                    entity->virtualDPad->moveSize    = saveGame->vDPadSize * (1.0f / 256);
                    entity->virtualDPad->jumpSize    = saveGame->vDPadSize * (1.0f / 256);
                    entity->virtualDPad->unknownSize = saveGame->vDPadSize * (1.0f / 256) * 0.85;
                }

                touchCheck = CheckTouchRect(32.0, 26.0, ((64.0 * entity->buttons[2]->scale) + entity->buttons[2]->textWidth) * 0.75, 12.0) < 0;
                if (touchCheck) {
                    entity->buttons[2]->state = 0;
                    touchCheck = CheckTouchRect(108.0, 26.0, ((64.0 * entity->buttons[3]->scale) + entity->buttons[3]->textWidth) * 0.75, 12.0) < 0;
                    if (!touchCheck) {
                        entity->buttons[3]->state = 1;
                        if (saveGame->vDPadOpacity <= 255)
                            saveGame->vDPadOpacity += 4;
                    }
                    else
                        entity->buttons[3]->state = 0;
                }
                else {
                    entity->buttons[2]->state = 1;
                    if (saveGame->vDPadOpacity > 0) {
                        saveGame->vDPadOpacity -= 4;
                    }
                }
                entity->virtualDPad->alpha = saveGame->vDPadOpacity;
                touchCheck = CheckTouchRect(88.0, -2.0, ((64.0 * entity->buttons[9]->scale) + entity->buttons[9]->textWidth) * 0.75, 12.0) < 0;
                entity->buttons[4]->state = !touchCheck;
                entity->virtualDPad       = entity->virtualDPad;
                int moveTouch, jumpTouch;
                float relativeX, relativeY, touchX, touchY, relTouchH, relTouchW;
                if (entity->virtualDPad->moveFinger == -1)
                    moveTouch = CheckTouchRect(entity->virtualDPad->moveX, entity->virtualDPad->moveY, 128.0 * entity->virtualDPad->moveSize,
                                               128.0 * entity->virtualDPad->moveSize);
                else
                    moveTouch = CheckTouchRect(-0.5 * SCREEN_CENTERX_F, SCREEN_CENTERY_F * -0.5, 0.5 * SCREEN_CENTERX_F, SCREEN_CENTERY_F * 0.5);
                entity->virtualDPad = entity->virtualDPad;
                if (moveTouch >= 0) {
                    touchX = touchXF[moveTouch];
                    touchY = touchYF[moveTouch];
                    if (entity->virtualDPad->moveFinger == -1) {
                        relativeX                       = entity->virtualDPad->moveX - touchX;
                        relativeY                       = entity->virtualDPad->moveY - touchY;
                        entity->virtualDPad->moveFinger = moveTouch;
                        entity->virtualDPad->relativeX  = relativeX;
                        entity->virtualDPad->relativeY  = relativeY;
                    }
                    else {
                        relativeX = entity->virtualDPad->relativeX;
                        relativeY = entity->virtualDPad->relativeY;
                    }
                    relTouchH                  = touchY + relativeY;
                    relTouchW                  = relativeX + touchX;
                    float moveSizeScale        = 128.0 * entity->virtualDPad->moveSize;
                    entity->virtualDPad->moveY = relTouchH;
                    entity->virtualDPad->moveX = relTouchW;
                    if (-SCREEN_CENTERX_F > (relTouchW - moveSizeScale)) {
                        relTouchW                  = moveSizeScale - SCREEN_CENTERX_F;
                        entity->virtualDPad->moveX = moveSizeScale - SCREEN_CENTERX_F;
                    }
                    if ((relTouchW + moveSizeScale) > 0.0)
                        entity->virtualDPad->moveX = -moveSizeScale;
                    if ((relTouchH + moveSizeScale) > -8.0) {
                        relTouchH                  = -8.0 - moveSizeScale;
                        entity->virtualDPad->moveY = -8.0 - moveSizeScale;
                    }
                    if (-SCREEN_CENTERY_F > (relTouchH - moveSizeScale))
                        entity->virtualDPad->moveY = moveSizeScale - SCREEN_CENTERY_F;
                }
                if (entity->virtualDPad->moveFinger == -1) {
                    jumpTouch = entity->virtualDPad->jumpFinger == -1
                                    ? CheckTouchRect(entity->virtualDPad->jumpX, entity->virtualDPad->jumpY, 128.0 * entity->virtualDPad->jumpSize,
                                                     128.0 * entity->virtualDPad->jumpSize)
                                    : CheckTouchRect(SCREEN_CENTERX_F * 0.5, SCREEN_CENTERY_F * -0.5, SCREEN_CENTERX_F * 0.5, 0.5 * SCREEN_CENTERY_F);
                    if (jumpTouch >= 0) {
                        entity->virtualDPad = entity->virtualDPad;
                        touchX              = touchXF[jumpTouch];
                        touchY              = touchYF[jumpTouch];
                        if (entity->virtualDPad->jumpFinger == -1) {
                            relativeX                       = entity->virtualDPad->jumpX - touchX;
                            relativeY                       = entity->virtualDPad->jumpY - touchY;
                            entity->virtualDPad->jumpFinger = jumpTouch;
                            entity->virtualDPad->relativeX  = relativeX;
                            entity->virtualDPad->relativeY  = relativeY;
                        }
                        else {
                            relativeX = entity->virtualDPad->relativeX;
                            relativeY = entity->virtualDPad->relativeY;
                        }
                        relTouchH                  = touchY + relativeY;
                        relTouchW                  = relativeX + touchX;
                        float jumpScaleSize        = 128.0 * entity->virtualDPad->jumpSize;
                        entity->virtualDPad->jumpY = relTouchH;
                        entity->virtualDPad->jumpX = relTouchW;
                        if ((relTouchW - jumpScaleSize) < 0.0) {
                            entity->virtualDPad->jumpX = jumpScaleSize;
                            relTouchW                  = jumpScaleSize;
                        }
                        if ((relTouchW + jumpScaleSize) > SCREEN_CENTERX_F)
                            entity->virtualDPad->jumpX = SCREEN_CENTERX_F - jumpScaleSize;
                        if ((relTouchH + jumpScaleSize) > -8.0) {
                            relTouchH                  = -8.0 - jumpScaleSize;
                            entity->virtualDPad->jumpY = -8.0 - jumpScaleSize;
                        }
                        if (-SCREEN_CENTERY_F > (relTouchH - jumpScaleSize))
                            entity->virtualDPad->jumpY = jumpScaleSize - SCREEN_CENTERY_F;
                    }
                }
                entity->backPressed = CheckTouchRect(136.0, 88.0, 32.0, 16.0) >= 0;
            }
            else {
                entity->virtualDPad             = entity->virtualDPad;
                entity->virtualDPad->moveFinger = -1;
                entity->virtualDPad->jumpFinger = -1;
                entity->buttons[0]->state       = 0;
                entity->buttons[1]->state       = 0;
                entity->buttons[2]->state       = 0;
                entity->buttons[3]->state       = 0;
                if (entity->backPressed) {
                    PlaySfxByName("Menu Back", false);
                    entity->backPressed = false;
                    entity->state       = 3;
                    SetGlobalVariableByName("options.touchControls", 0);
                    saveGame->vDPadX_Move = (entity->virtualDPad->moveX + SCREEN_CENTERX_F);
                    saveGame->vDPadY_Move = -(entity->virtualDPad->moveY - SCREEN_CENTERY_F);
                    saveGame->vDPadX_Jump = entity->virtualDPad->jumpX - SCREEN_CENTERX_F;
                    saveGame->vDPadY_Jump = -(entity->virtualDPad->jumpY - SCREEN_CENTERY_F);
                }
                if (entity->buttons[4]->state == 1) {
                    entity->buttons[4]->state = 0;
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
                    entity->virtualDPad->unknownSize = saveGame->vDPadSize * (1.0f / 256) * 0.85;
                }
            }

            if (entity->state == 5 && keyPress.B) {
                PlaySfxByName("Menu Back", false);
                entity->backPressed = 0;
                entity->state       = 3;
                SetGlobalVariableByName("options.touchControls", 0);
                saveGame->vDPadX_Move = (entity->virtualDPad->moveX + SCREEN_CENTERX_F);
                saveGame->vDPadY_Move = -(entity->virtualDPad->moveY - SCREEN_CENTERY_F);
                saveGame->vDPadX_Jump = entity->virtualDPad->jumpX - SCREEN_CENTERX_F;
                saveGame->vDPadY_Jump = -(entity->virtualDPad->jumpY - SCREEN_CENTERY_F);
            }
            break;
        case 6: {
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
            for (int l = 0; l != 10; ++l) entity->buttons[l]->renderMatrix = entity->buttonMatrix;

            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                optionsMenu->state = 7;
                RemoveNativeObject(entity->buttons[9]);
                RemoveNativeObject(entity->buttons[8]);
                RemoveNativeObject(entity->buttons[7]);
                RemoveNativeObject(entity->buttons[6]);
                RemoveNativeObject(entity->buttons[5]);
                RemoveNativeObject(entity->buttons[4]);
                RemoveNativeObject(entity->buttons[3]);
                RemoveNativeObject(entity->buttons[2]);
                RemoveNativeObject(entity->buttons[1]);
                RemoveNativeObject(entity->buttons[0]);
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
                int gameRegion = saveGame->boxRegion, newBoxTex;
                if (Engine.globalBoxRegion != gameRegion) {
                    int package = 0;
                    switch (Engine.globalBoxRegion) {
                        case REGION_JP: package = LoadTexture("Data/Game/Models/Package_JP.png", TEXFMT_RGBA8888); break;
                        case REGION_US: package = LoadTexture("Data/Game/Models/Package_US.png", TEXFMT_RGBA8888); break;
                        case REGION_EU: package = LoadTexture("Data/Game/Models/Package_EU.png", TEXFMT_RGBA8888); break;
                    }
                    Engine.globalBoxRegion = gameRegion;
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
        case 7: {
            entity->buttonRotY -= (10.0 * Engine.deltaTime);
            if (entity->buttonRotY < -(M_PI / 2)) {
                entity->state      = 8;
                entity->buttonRotY = -(M_PI / 2 + M_PI);
                if (entity->subState) {
                    if (entity->subState == 2) {
                        RestoreNativeObjectsSettings();
                        entity->subState = 0;
                        SetStringToFont(entity->label->text, strSettings, FONT_HEADING);
                        if (entity->controlStyle == 1)
                            SetGlobalVariableByName("options.touchControls", true);
                    }
                }
                else {
                    entity->subState = 2;
                    BackupNativeObjectsSettings();
                    SetStringToFont(entity->label->text, strControls, FONT_HEADING);
                    RemoveNativeObject(entity->buttons[9]);
                    RemoveNativeObject(entity->buttons[8]);
                    RemoveNativeObject(entity->buttons[7]);
                    RemoveNativeObject(entity->buttons[6]);
                    RemoveNativeObject(entity->buttons[5]);
                    RemoveNativeObject(entity->buttons[4]);
                    RemoveNativeObject(entity->buttons[3]);
                    RemoveNativeObject(entity->buttons[2]);
                    RemoveNativeObject(entity->buttons[1]);
                    RemoveNativeObject(entity->buttons[0]);
                    entity->buttons[2]->y = entity->buttons[2]->y + 4.0;
                    entity->buttons[3]->y = entity->buttons[3]->y + 4.0;
                }
            }
            NewRenderState();
            matrixRotateYF(&entity->buttonMatrix, entity->buttonRotY);
            matrixTranslateXYZF(&entity->tempMatrix, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->buttonMatrix, &entity->tempMatrix);
            SetRenderMatrix(&entity->buttonMatrix);
            for (int l = 0; l < 10; ++l) entity->buttons[l]->renderMatrix = entity->buttonMatrix;
            break;
        }
        case 8:
            entity->buttonRotY -= (10.0 * Engine.deltaTime);
            if (entity->buttonRotY < -(M_PI * 2)) {
                if (entity->subState) {
                    if (entity->subState == 2)
                        entity->state = 9;
                }
                else {
                    entity->state = 1;
                }
                entity->buttonRotY = 0.0;
            }
            NewRenderState();
            matrixRotateYF(&entity->buttonMatrix, entity->buttonRotY);
            matrixTranslateXYZF(&entity->tempMatrix, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->buttonMatrix, &entity->tempMatrix);
            SetRenderMatrix(&entity->buttonMatrix);
            for (int l = 0; l < 10; ++l) entity->buttons[l]->renderMatrix = entity->buttonMatrix;
            break;
        case 9:
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->tempMatrix);
            if (touches <= 0) {
                if (entity->backPressed) {
                    PlaySfxByName("Menu Back", false);
                    entity->backPressed = false;
                    entity->state       = 7;
                    break;
                }
            }
            else {
                entity->backPressed = CheckTouchRect(136.0, 88.0, 32.0, 16.0) >= 0;
            }
            if (keyPress.B) {
                PlaySfxByName("Menu Back", false);
                entity->backPressed = false;
                entity->state       = 7;
            }
            break;
        default: break;
    }
    RenderMesh(entity->panelMesh, MESH_COLOURS, false);
    switch (entity->subState) {
        case 1:
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
        case 2: RenderImage(0.0, 0.0, 0.0, 0.275, 0.275, 512.0, 256.0, 1024.0, 512.0, 0.0, 0.0, 255, entity->controllerTex); break;
        case 0:
            if (entity->selected == 1)
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

            if (entity->selected == 2)
                SetRenderVertexColor(0xFF, 0xFF, 0x00);
            else
                SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            if (Engine.language == RETRO_RU)
                RenderText(entity->sfxText, FONT_LABEL, -128.0, 26.0, 0, 0.09, 255);
            else
                RenderText(entity->sfxText, FONT_LABEL, -128.0, 26.0, 0, 0.125, 255);

            if (entity->selected == 3)
                SetRenderVertexColor(0xFF, 0xFF, 0x00);
            else
                SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            if (!entity->controlStyle && Engine.gameType == GAME_SONIC1) {
                if ((Engine.language - 1) <= 6 && ((1 << (Engine.language - 1)) & 0x43))
                    RenderText(entity->spindashText, FONT_LABEL, -128.0, -6.0, 0, 0.09, 255);
                else
                    RenderText(entity->spindashText, FONT_LABEL, -128.0, -6.0, 0, 0.125, 255);
            }

            if (entity->selected == 4)
                SetRenderVertexColor(0xFF, 0xFF, 0x00);
            else
                SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            RenderText(entity->boxArtText, FONT_LABEL, -128.0, -38.0, 0, 0.125, 255);
            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            break;
    }
    NewRenderState();
    SetRenderMatrix(NULL);
    if (entity->subState) {
        if (entity->subState >= 0 && entity->subState <= 2) {
            RenderImage(136.0, 88.0, 160.0, 0.25, 0.25, 64.0, 64.0, 128.0, 128.0, 128.0, entity->backPressed ? 128.0 : 0, entity->alpha,
                        entity->arrowsTex);
        }
    }
    else {
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, entity->backPressed ? 128.0 : 0, entity->alpha,
                    entity->arrowsTex);
    }
}
