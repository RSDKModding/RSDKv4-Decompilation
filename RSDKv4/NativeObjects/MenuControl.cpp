#include "RetroEngine.hpp"

bool usePhysicalControls = false;

void MenuControl_Create(void *objPtr)
{
    RSDK_THIS(MenuControl);
    SetMusicTrack("MainMenu.ogg", 0, true, 106596);
    CREATE_ENTITY(MenuBG);

    entity->buttons[entity->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(StartGameButton);
    entity->buttonFlags[entity->buttonCount] = BUTTON_STARTGAME;
    entity->buttonCount++;

    entity->buttons[entity->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(TimeAttackButton);
    entity->buttonFlags[entity->buttonCount] = BUTTON_TIMEATTACK;
    entity->buttonCount++;

#if RETRO_USE_MOD_LOADER
    int vsID = GetSceneID(STAGELIST_PRESENTATION, "2P VS");
    if (vsID != -1) {
#else
    if (Engine.gameType == GAME_SONIC2) {
#endif
        entity->buttons[entity->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(MultiplayerButton);
        entity->buttonFlags[entity->buttonCount] = BUTTON_MULTIPLAYER;
        entity->buttonCount++;
    }

#if RETRO_USE_MOD_LOADER
    entity->buttons[entity->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(ModsButton);
    entity->buttonFlags[entity->buttonCount] = BUTTON_MODS;
    entity->buttonCount++;
#endif

    if (Engine.onlineActive) {
        entity->buttons[entity->buttonCount]     = CREATE_ENTITY(AchievementsButton);
        entity->buttonFlags[entity->buttonCount] = BUTTON_ACHIEVEMENTS;
        entity->buttonCount++;

        entity->buttons[entity->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(LeaderboardsButton);
        entity->buttonFlags[entity->buttonCount] = BUTTON_LEADERBOARDS;
        entity->buttonCount++;
    }

    entity->buttons[entity->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(OptionsButton);
    entity->buttonFlags[entity->buttonCount] = BUTTON_OPTIONS;
    entity->buttonCount++;

    entity->backButton          = CREATE_ENTITY(BackButton);
    entity->backButton->visible = false;
    entity->backButton->x       = 240.0;
    entity->backButton->y       = -160.0;
    entity->backButton->z       = 0.0;

    entity->segaIDButton       = CREATE_ENTITY(SegaIDButton);
    entity->segaIDButton->y    = -92.0;
    entity->segaIDButton->texX = 0.0;
    entity->segaIDButton->x    = SCREEN_CENTERX_F - 32.0;

    entity->float28 = 0.15707964;
    entity->float2C = 0.078539819;
    entity->float30 = (entity->buttonCount * 0.15707964) * 0.5;

    float offset = 0.0;
    for (int b = 0; b < entity->buttonCount; ++b) {
        NativeEntity_AchievementsButton *button = entity->buttons[b];
        float sin                               = sinf(entity->float18 + offset);
        float cos                               = cosf(entity->float18 + offset);
        button->x                               = 1024.0 * sin;
        button->z                               = (cos * -512.0) + 672.0;
        button->y                               = (128.0 * sin) + 16.0;
        button->visible                         = button->z <= 288.0;
        offset += entity->float28;
    }

    PlayMusic(0, 0);
    if (Engine.gameDeviceType == RETRO_STANDARD)
        usePhysicalControls = true;
    BackupNativeObjects();
}
void MenuControl_Main(void *objPtr)
{
    RSDK_THIS(MenuControl);
    NativeEntity_SegaIDButton *segaIDButton = entity->segaIDButton;
    NativeEntity_BackButton *backButton     = entity->backButton;

    switch (entity->state) {
        case 0: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);

            if (segaIDButton->alpha < 0x100 && Engine.language != RETRO_JP && !(Engine.language == RETRO_ZH || Engine.language == RETRO_ZS)
                && Engine.gameDeviceType == RETRO_MOBILE)
                segaIDButton->alpha += 8;

            if (!usePhysicalControls) {
                switch (entity->stateInput) {
                    case 0: { // checking for touches
                        if (touches > 0) {
                            if (!keyDown.left && !keyDown.right) {
                                segaIDButton->state = 0;
                                if (CheckTouchRect(0.0, 16.0, 56.0, 56.0) >= 0) {
                                    BackupNativeObjects();
                                    entity->touchX2                      = touchXF[0];
                                    entity->stateInput                   = 4;
                                    entity->buttonID                     = ceilf(entity->float18 / -entity->float2C);
                                    entity->buttons[entity->buttonID]->g = 0xC0;
                                }
                                else {
                                    if (CheckTouchRect(entity->segaIDButton->x, entity->segaIDButton->y, 20.0, 20.0) >= 0
                                        && segaIDButton->alpha > 64) {
                                        segaIDButton->state = 1;
                                    }
                                    else {
                                        entity->stateInput = 1;
                                        entity->field_74   = 0.0;
                                        entity->field_6C   = touchXF[0];
                                        entity->float20    = entity->float18;
                                    }
                                }
                            }
                            else {
                                segaIDButton->state                  = 0;
                                usePhysicalControls                  = true;
                                entity->buttonID                     = ceilf(entity->float18 / -entity->float2C);
                                entity->buttons[entity->buttonID]->g = 0xC0;
                            }
                        }
                        else if (segaIDButton->state == 1) {
                            segaIDButton->state = 0;
                            PlaySfxByName("Menu Select", false);
                            ShowPromoPopup(0, "MoreGames");
                        }
                        else if (keyDown.left || keyDown.right) {
                            segaIDButton->state                  = 0;
                            usePhysicalControls                  = true;
                            entity->buttonID                     = ceilf(entity->float18 / -entity->float2C);
                            entity->buttons[entity->buttonID]->g = 0xC0;
                        }
                        break;
                    }
                    case 1: { // managing dragging
                        if (touches <= 0) {
                            entity->stateInput = 2;
                        }
                        else {
                            entity->field_70 = 0.0;
                            entity->field_78 = (entity->field_6C - touchXF[0]) * -0.0007;
                            if (entity->field_74 > 0.0 || entity->field_74 < 0.0) {
                                entity->field_70 = entity->field_78 - entity->field_74;
                                entity->float18 += entity->field_70;
                            }
                            entity->field_74 = entity->field_78;
                        }
                        break;
                    }
                    case 2: { // touch release
                        entity->field_70 /= (1.125 * (60.0 * Engine.deltaTime));
                        entity->float18 += entity->field_70;
                        float val = -(entity->float30 - entity->float2C);

                        if ((val - 0.05) > entity->float18 || entity->float18 > 0.05) {
                            entity->field_70 = 0.0;
                        }

                        if (abs(entity->field_70) < 0.0025) {
                            if (entity->float18 == entity->float20 && entity->field_6C < 0.0) {
                                entity->float18 += 0.00001;
                            }

                            if (entity->float18 <= entity->float20) {
                                if ((floorf(entity->float18 / entity->float2C) * entity->float2C) > (entity->float20 - entity->float2C)) {
                                    entity->float1C = entity->float20 - entity->float2C;
                                }
                                else {
                                    entity->float1C = floorf(entity->float18 / entity->float2C) * entity->float2C;
                                }

                                if (entity->float1C <= val)
                                    entity->float1C = val;
                            }
                            else {
                                if ((entity->float2C + entity->float20) > (ceilf(entity->float18 / entity->float2C) * entity->float2C)) {
                                    entity->float1C = entity->float2C + entity->float20;
                                }
                                else {
                                    entity->float1C = ceilf(entity->float18 / entity->float2C) * entity->float2C;
                                }

                                if (entity->float1C > 0.0)
                                    entity->float1C = 0.0;
                            }

                            entity->stateInput = 3;
                            entity->float18 += ((entity->float1C - entity->float18) / ((60.0 * Engine.deltaTime) * 8.0));
                        }
                        break;
                    }
                    case 3:
                        if (touches > 0) {
                            entity->stateInput = 1;
                            entity->field_74   = 0.0;
                            entity->field_6C   = touchXF[0];
                        }
                        else {
                            entity->float18 += ((entity->float1C - entity->float18) / ((60.0 * Engine.deltaTime) * 6.0));
                            if (abs(entity->float1C - entity->float18) < 0.00025) {
                                entity->float18    = entity->float1C;
                                entity->stateInput = 0;
                            }
                        }
                        break;
                    case 4:
                        if (touches > 0) {
                            if (CheckTouchRect(0.0, 16.0, 56.0, 56.0) < 0) {
                                entity->buttons[entity->buttonID]->g = 0xFF;
                            }
                            else {
                                entity->buttons[entity->buttonID]->g = 0xC0;
                                float move                           = entity->touchX2 - touchXF[0];

                                bool flag = false;
                                if (move < 0.0)
                                    flag = move < -8.0;
                                else
                                    flag = move > 8.0;
                                if (flag) {
                                    entity->stateInput                   = 1;
                                    entity->field_6C                     = entity->buttonID;
                                    entity->field_74                     = 0.0;
                                    entity->float20                      = entity->float18;
                                    entity->buttons[entity->buttonID]->g = 0xFF;
                                }
                            }
                        }
                        else {
                            if (entity->buttons[entity->buttonID]->g == 0xC0) {
                                entity->buttons[entity->buttonID]->labelPtr->state = 2;
                                entity->timer                                      = 0.0;
                                entity->state                                      = 1;
                                PlaySfxByName("Menu Select", false);
                            }
                            entity->buttons[entity->buttonID]->g = 0xFF;
                            entity->stateInput                   = 0;
                        }
                        break;
                    default: break;
                }
            }
            else {
                if (entity->stateInput == 1) {
                    entity->float18 += (((entity->float24 + entity->float1C) - entity->float18) / ((60.0 * Engine.deltaTime) * 8.0));

                    if (abs(entity->float1C - entity->float18) < 0.001) {
                        entity->float18    = entity->float1C;
                        entity->stateInput = 0;
                    }
                }
                else {
                    if (touches <= 0) {
                        if (keyPress.right && entity->float18 > -(entity->float30 - entity->float2C)) {
                            entity->stateInput = 1;
                            entity->float1C -= entity->float2C;
                            PlaySfxByName("Menu Move", false);
                            entity->float24 = -0.01;
                            entity->buttonID++;
                            if (entity->buttonID >= entity->buttonCount)
                                entity->buttonID = entity->buttonCount - 1;
                        }
                        else if (keyPress.left && entity->float18 < 0.0) {
                            entity->stateInput = 1;
                            entity->float1C += entity->float2C;
                            PlaySfxByName("Menu Move", false);
                            entity->float24 = 0.01;
                            entity->buttonID--;
                            if (entity->buttonID > entity->buttonCount)
                                entity->buttonID = 0;
                        }
                        else if ((keyPress.start || keyPress.A) && !Engine.nativeMenuFadeIn) {
                            BackupNativeObjects();
                            entity->buttons[entity->buttonID]->labelPtr->state = 2;
                            entity->timer                                      = 0.0;
                            entity->state                                      = 1;
                            PlaySfxByName("Menu Select", false);
                        }

                        for (int i = 0; i < entity->buttonCount; ++i) {
                            entity->buttons[i]->g = 0xFF;
                        }
                        entity->buttons[entity->buttonID]->g = 0xC0;
                    }
                    else {
                        usePhysicalControls = false;
                        for (int i = 0; i < entity->buttonCount; ++i) {
                            entity->buttons[i]->g = 0xFF;
                        }
                    }
                }
            }

            float offset = entity->float18;
            for (int i = 0; i < entity->buttonCount; ++i) {
                NativeEntity_AchievementsButton *button = entity->buttons[i];
                button->x                               = 1024.0 * sinf(entity->float18 + offset);
                button->y                               = (sinf(entity->float18 + offset) * 128.0) + 16.0;
                button->z                               = (cosf(entity->float18 + offset) * -512.0) + 672.0;
                button->visible                         = button->z <= 288.0;
                offset += entity->float28;
            }

            if (!entity->stateInput) {
                if (entity->dialogTimer) {
                    entity->dialogTimer--;
                }
                else if (keyPress.B) {
                    entity->dialog = CREATE_ENTITY(DialogPanel);
                    SetStringToFont(entity->dialog->text, strExitGame, FONT_TEXT);
                    entity->state = 6;
                    PlaySfxByName("Resume", false);
                }
            }
            break;
        }
        case 1: {
            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                entity->timer                           = 0.0;
                NativeEntity_AchievementsButton *button = entity->buttons[entity->buttonID];
                switch (entity->buttonFlags[entity->buttonID]) {
                    case BUTTON_STARTGAME:
                        entity->state                                      = 3;
                        entity->field_70                                   = 0.0;
                        button->g                                          = 0xFF;
                        entity->buttons[entity->buttonID]->labelPtr->state = -1;
                        entity->backButton->visible                        = true;
                        SetGlobalVariableByName("options.vsMode", false);
                        CREATE_ENTITY(SaveSelect);
                        break;
                    case BUTTON_TIMEATTACK:
                        entity->state               = 3;
                        entity->field_70            = 0.0;
                        button->g                   = 0xFF;
                        button->labelPtr->state     = -1;
                        entity->backButton->visible = true;
                        CREATE_ENTITY(TimeAttack);
                        break;
                    case BUTTON_MULTIPLAYER:
                        entity->state           = 0;
                        button->labelPtr->state = 0;
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
                        if (!Engine.onlineActive) {
                            disconnectNetwork();
                            initNetwork(); // let's see if we can turn it on
                        }
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
                            entity->dialog              = CREATE_ENTITY(DialogPanel);
                            entity->dialog->buttonCount = DLGTYPE_OK;
                            SetStringToFont(entity->dialog->text, strNetworkMessage, FONT_TEXT);
                            entity->state = 6;
                        }
                        break;
                    case BUTTON_ACHIEVEMENTS:
                        if (Engine.onlineActive) {
                            entity->state               = 3;
                            entity->field_70            = 0.0;
                            button->g                   = 0xFF;
                            button->labelPtr->state     = -1;
                            entity->backButton->visible = true;
                            ShowAchievementsScreen();
                        }
                        else {
                            entity->state               = 0;
                            entity->dialog              = CREATE_ENTITY(DialogPanel);
                            entity->dialog->buttonCount = DLGTYPE_OK;
                            SetStringToFont(entity->dialog->text, strNetworkMessage, FONT_TEXT);
                            entity->state           = 6;
                            button->labelPtr->state = 0;
                        }
                        break;
                    case BUTTON_LEADERBOARDS:
                        entity->state = 0;
                        if (Engine.onlineActive) {
                            ShowLeaderboardsScreen();
                        }
                        else {
                            entity->dialog              = CREATE_ENTITY(DialogPanel);
                            entity->dialog->buttonCount = DLGTYPE_OK;
                            SetStringToFont(entity->dialog->text, strNetworkMessage, FONT_TEXT);
                            entity->state = 6;
                        }
                        button->labelPtr->state = 0;
                        break;
                    case BUTTON_OPTIONS:
                        entity->state               = 3;
                        entity->field_70            = 0.0;
                        button->g                   = 0xFF;
                        button->labelPtr->state     = -1;
                        entity->backButton->visible = true;
                        CREATE_ENTITY(OptionsMenu);
                        break;
#if RETRO_USE_MOD_LOADER
                    case BUTTON_MODS:
                        entity->state               = 3;
                        entity->field_70            = 0.0;
                        button->g                   = 0xFF;
                        button->labelPtr->state     = -1;
                        entity->backButton->visible = true;
                        CREATE_ENTITY(ModsMenu);
                        break;
#endif
                    default:
                        entity->state           = 0;
                        button->labelPtr->state = 0;
                        break;
                }
            }
            break;
        }
        case 3: {
            if (segaIDButton->alpha > 0)
                segaIDButton->alpha -= 8;

            entity->field_70 -= 0.125 * (60.0 * Engine.deltaTime);

            for (int i = 0; i < entity->buttonCount; ++i) {
                if (entity->buttonID != i) {
                    if (entity->buttonID != i)
                        entity->buttons[i]->z += ((60.0 * Engine.deltaTime) * entity->field_70);
                }
            }

            entity->timer += Engine.deltaTime;
            entity->field_70 -= 0.125 * (60.0 * Engine.deltaTime);

            if (entity->timer > 0.5) {
                NativeEntity_AchievementsButton *button = entity->buttons[entity->buttonID];
                float div                               = (60.0 * Engine.deltaTime) * 16.0;

                button->x += ((112.0 - button->x) / div);
                button->y += ((64.0 - button->y) / div);
                button->z += ((200.0 - button->z) / div);
                entity->backButton->z += ((320.0 - entity->backButton->z) / div);
            }

            if (entity->timer > 1.5) {
                entity->timer = 0.0;
                entity->state = 4;

                for (int i = 0; i < entity->buttonCount; ++i) {
                    if (entity->buttonID != i) {
                        if (entity->buttonID != i)
                            entity->buttons[i]->visible = false;
                    }
                }
            }
            break;
        }
        case 4: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            if (touches <= 0) {
                if (entity->backButton->g == 0xC0) {
                    PlaySfxByName("Menu Back", false);
                    entity->backButton->g = 0xFF;
                    entity->state         = 5;
                }
            }
            else {
                backButton = entity->backButton;
                if (CheckTouchRect(122.0, -80.0, 32.0, 32.0) < 0)
                    backButton->g = 0xFF;
                else
                    backButton->g = 0xC0;
            }
            if (keyPress.B) {
                PlaySfxByName("Menu Back", false);
                entity->backButton->g = 0xFF;
                entity->state         = 5;
            }
            break;
        }
        case 5: {
            entity->backButton->z = ((0.0 - entity->backButton->z) / (16.0 * (60.0 * Engine.deltaTime))) + entity->backButton->z;
            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.25) {
                float offset = entity->float18;
                float div    = (60.0 * Engine.deltaTime) * 8.0;

                for (int i = 0; i < entity->buttonCount; ++i) {
                    if (entity->buttonID != i) {
                        NativeEntity_AchievementsButton *button = entity->buttons[i];
                        button->z       = ((((cosf(offset + entity->float18) * -512.0) + 672.0) - button->z) / div) + button->z;
                        button->visible = true;
                    }
                    offset += entity->float28;
                }

                NativeEntity_AchievementsButton *curButton = entity->buttons[entity->buttonID];
                curButton->labelPtr->state                 = 0;
                curButton->x += ((0.0 - curButton->x) / div);
                curButton->y += ((16.0 - curButton->y) / div);
                curButton->z += ((160.0 - curButton->z) / div);
            }

            if (entity->timer > 1.0) {
                entity->timer    = 0.0;
                entity->field_70 = 0.0;
                entity->state    = 0;
            }
            break;
        }
        case 6: {
            if (entity->dialog->selection == DLG_NO || entity->dialog->selection == DLG_OK) {
                entity->state       = 0;
                entity->dialogTimer = 50;
            }
            else if (entity->dialog->selection == DLG_YES) {
                ExitGame();
                entity->dialogTimer = 50;
                entity->state       = 0;
            }
            break;
        }
        default: break;
    }
}
