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

    if (Engine.gameType == GAME_SONIC2) {
        entity->buttons[entity->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(MultiplayerButton);
        entity->buttonFlags[entity->buttonCount] = BUTTON_MULTIPLAYER;
        entity->buttonCount++;
    }

#if RETRO_USE_MOD_LOADER
    entity->buttons[entity->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(ModsButton);
    entity->buttonFlags[entity->buttonCount] = BUTTON_MODS;
    entity->buttonCount++;
#endif

    // if (gameOnlineActive) {
    entity->buttons[entity->buttonCount]     = CREATE_ENTITY(AchievementsButton);
    entity->buttonFlags[entity->buttonCount] = BUTTON_ACHIEVEMENTS;
    entity->buttonCount++;
    //}

    if (Engine.gameType == GAME_SONIC2) {
        entity->buttons[entity->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(LeaderboardsButton);
        entity->buttonFlags[entity->buttonCount] = BUTTON_LEADERBOARDS;
        entity->buttonCount++;
    }

    entity->buttons[entity->buttonCount]     = (NativeEntity_AchievementsButton *)CREATE_ENTITY(OptionsButton);
    entity->buttonFlags[entity->buttonCount] = BUTTON_OPTIONS;
    entity->buttonCount++;

    
    entity->backButton                       = CREATE_ENTITY(BackButton);
    entity->backButton->visible      = false;
    entity->backButton->translateX   = 240.0;
    entity->backButton->translateY   = -160.0;
    entity->backButton->translateZ   = 0.0;
    
    entity->segaIDButton             = CREATE_ENTITY(SegaIDButton);
    entity->segaIDButton->translateY = -92.0;
    entity->segaIDButton->texX       = 0.0;
    entity->segaIDButton->translateX = SCREEN_CENTERX_F - 32.0;

    entity->float28                  = 0.15707964;
    entity->float2C                  = 0.078539819;
    entity->float30                  = (entity->buttonCount * 0.15707964) * 0.5;

    float offset = 0.0;
    for (int b = 0; b < entity->buttonCount; ++b) {
        NativeEntity_AchievementsButton *button             = entity->buttons[b];
        float sin                               = sinf(offset + entity->float18);
        float cos                                     = cosf(entity->float18 + offset);
        button->translateX = 1024.0 * sin;
        button->translateZ = (cos * -512.0) + 672.0;
        button->translateY = (128.0 * sin) + 16.0;
        button->visible    = button->translateZ <= 288.0;
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
                                    if (CheckTouchRect(entity->segaIDButton->translateX, entity->segaIDButton->translateY, 20.0, 20.0) >= 0
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
                            PlaySfx(22, 0);
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
                    case 2: { //touch release
                        entity->field_70 = entity->field_70 / (1.125 * (60.0 * Engine.deltaTime));
                        entity->float18 += entity->field_70;

                        bool flag = false;
                        if ((-(entity->float30 - entity->float2C) - 0.05) > entity->float18 || entity->float18 > 0.05) {
                            entity->field_70 = 0.0;
                            flag             = true;
                        }
                        else if (entity->field_70 >= 0.0) {
                            flag = entity->field_70 < 0.0025;
                        }
                        else {
                            flag = entity->field_70 > -0.0025;
                        }

                        if (flag) {
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

                                if (entity->float1C <= -(entity->float30 - entity->float2C)) {
                                    entity->float1C = -(entity->float30 - entity->float2C);
                                }
                            }
                            else {
                                if ((entity->float2C + entity->float20) > (ceilf(entity->float18 / entity->float2C) * entity->float2C)) {
                                    entity->float1C = entity->float2C + entity->float20;
                                }
                                else {
                                    entity->float1C = ceilf(entity->float18 / entity->float2C) * entity->float2C;
                                }

                                if (entity->float1C > 0.0) {
                                    entity->float1C = 0.0;
                                }
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
                            float move = entity->float1C - entity->float18;
                            bool flag  = false;

                            if (move < 0.0)
                                flag = move > -0.00025;
                            else
                                flag = move < 0.00025;
                            if (flag) {
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
                                entity->buttons[entity->buttonID]->labelPtr->alignment = 2;
                                entity->float14                                        = 0.0;
                                entity->state                                          = 1;
                                PlaySfx(22, 0);
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
                    float move = entity->float1C - entity->float18;

                    bool flag = false;
                    if (move < 0.0)
                        flag = move > -0.001;
                    else
                        flag = move < 0.001;

                    if (flag) {
                        entity->float18    = entity->float1C;
                        entity->stateInput = 0;
                    }
                }
                else {
                    if (touches <= 0) {
                        if (keyPress.right && entity->float18 > -(entity->float30 - entity->float2C)) {
                            entity->stateInput = 1;
                            entity->float1C -= entity->float2C;
                            PlaySfx(21, 0);
                            entity->float24 = -0.01;
                            entity->buttonID++;
                            if (entity->buttonID >= entity->buttonCount)
                                entity->buttonID = entity->buttonCount - 1;
                        }
                        else if (keyPress.left && entity->float18 < 0.0) {
                            entity->stateInput = 1;
                            entity->float1C += entity->float2C;
                            PlaySfx(21, 0);
                            entity->float24 = 0.01;
                            entity->buttonID--;
                            if (entity->buttonID > entity->buttonCount)
                                entity->buttonID = 0;
                        }
                        else if ((keyPress.start || keyPress.A) && !Engine.nativeMenuFadeIn) {
                            BackupNativeObjects();
                            entity->buttons[entity->buttonID]->labelPtr->alignment = 2;
                            entity->float14                                        = 0.0;
                            entity->state                                          = 1;
                            PlaySfx(22, 0);
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
                button->translateX                      = 1024.0 * sinf(entity->float18 + offset);
                button->translateY                      = (sinf(entity->float18 + offset) * 128.0) + 16.0;
                button->translateZ                      = (cosf(entity->float18 + offset) * -512.0) + 672.0;
                button->visible                         = button->translateZ <= 288.0;
                offset += entity->float28;
            }

            if (!entity->stateInput) {
                if (entity->timer) {
                    entity->timer--;
                }
                else if (keyPress.B) {
                    entity->dialog = CREATE_ENTITY(DialogPanel);
                    SetStringToFont(entity->dialog->text, strExitGame, 2);
                    entity->state = 6;
                    PlaySfx(40, 0);
                }
            }
            break;
        }
        case 1: {
            entity->float14 += Engine.deltaTime;
            if (entity->float14 > 0.5) {
                entity->float14                         = 0.0;
                NativeEntity_AchievementsButton *button = entity->buttons[entity->buttonID];
                switch (entity->buttonFlags[entity->buttonID]) {
                    case BUTTON_STARTGAME:
                        entity->state                                          = 3;
                        entity->field_70                                       = 0.0;
                        button->g                                              = 0xFF;
                        entity->buttons[entity->buttonID]->labelPtr->alignment = -1;
                        entity->backButton->visible                            = true;
                        SetGlobalVariableByName("options.vsMode", false);
                        CREATE_ENTITY(SaveSelect);
                        break;
                    case BUTTON_TIMEATTACK:
                        entity->state               = 3;
                        entity->field_70            = 0.0;
                        button->g                   = 0xFF;
                        button->labelPtr->alignment = -1;
                        entity->backButton->visible = true;
                        CREATE_ENTITY(TimeAttack);
                        break;
                    case BUTTON_MULTIPLAYER:
                        entity->state               = 2;
                        button->labelPtr->alignment = 0;
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
                        SetGlobalVariableByName("starPostID", 0);
                        if (Engine.onlineActive) {
                            InitStartingStage(0, 3, 0);
                            CREATE_ENTITY(FadeScreen);
                        }
                        else {
                            entity->dialog              = CREATE_ENTITY(DialogPanel);
                            entity->dialog->buttonCount = 1;
                            SetStringToFont(entity->dialog->text, strNetworkMessage, 2);
                            entity->state = 6;
                        }
                        break;
                    case BUTTON_ACHIEVEMENTS:
                        entity->state = 0;
                        if (Engine.onlineActive) {
                            ShowAchievementsScreen();
                        }
                        else {
                            entity->dialog              = CREATE_ENTITY(DialogPanel);
                            entity->dialog->buttonCount = 1;
                            SetStringToFont(entity->dialog->text, strNetworkMessage, 2);
                            entity->state = 6;
                        }
                        break;
                    case BUTTON_LEADERBOARDS:
                        entity->state = 0;
                        if (Engine.onlineActive) {
                            ShowLeaderboardsScreen();
                        }
                        else {
                            entity->dialog              = CREATE_ENTITY(DialogPanel);
                            entity->dialog->buttonCount = 1;
                            SetStringToFont(entity->dialog->text, strNetworkMessage, 2);
                            entity->state = 6;
                        }
                        button->labelPtr->alignment = 0;
                        break;
                    case BUTTON_OPTIONS:
                        entity->state               = 3;
                        entity->field_70            = 0.0;
                        button->g                   = 0xFF;
                        button->labelPtr->alignment = -1;
                        entity->backButton->visible = true;
                        CREATE_ENTITY(OptionsMenu);
                        break;
#if RETRO_USE_MOD_LOADER
                    case BUTTON_MODS:
                        entity->state               = 3;
                        entity->field_70            = 0.0;
                        button->g                   = 0xFF;
                        button->labelPtr->alignment = -1;
                        entity->backButton->visible = true;
                        CREATE_ENTITY(ModsMenu);
                        break;
#endif
                    default:
                        entity->state               = 0;
                        button->labelPtr->alignment = 0;
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
                    NativeEntity_AchievementsButton *button = entity->buttons[i];
                    if (entity->buttonID != i)
                        entity->buttons[i]->translateZ += ((60.0 * Engine.deltaTime) * entity->field_70);
                }
            }

            entity->float14 += Engine.deltaTime;
            entity->field_70 -= 0.125 * (60.0 * Engine.deltaTime);

            if (entity->float14 > 0.5) {
                NativeEntity_AchievementsButton *button = entity->buttons[entity->buttonID];
                float div                               = (60.0 * Engine.deltaTime) * 16.0;

                button->translateX += ((112.0 - button->translateX) / div);
                button->translateY += ((64.0 - button->translateY) / div);
                button->translateZ += ((200.0 - button->translateZ) / div);
                entity->backButton->translateZ += ((320.0 - entity->backButton->translateZ) / div);
            }

            if (entity->float14 > 1.5) {
                entity->float14 = 0.0;
                entity->state   = 4;

                for (int i = 0; i < entity->buttonCount; ++i) {
                    if (entity->buttonID != i) {
                        NativeEntity_AchievementsButton *button = entity->buttons[i];
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
                    PlaySfx(23, 0);
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
                PlaySfx(23, 0);
                entity->backButton->g = 0xFF;
                entity->state         = 5;
            }
            break;
        }
        case 5: {
            entity->backButton->translateZ =
                ((0.0 - entity->backButton->translateZ) / (16.0 * (60.0 * Engine.deltaTime))) + entity->backButton->translateZ;
            entity->float14 += Engine.deltaTime;
            if (entity->float14 > 0.25) {
                float offset = entity->float18;
                float div    = (60.0 * Engine.deltaTime) * 8.0;

                for (int i = 0; i < entity->buttonCount; ++i) {
                    if (entity->buttonID != i) {
                        NativeEntity_AchievementsButton *button = entity->buttons[i];
                        button->translateZ = ((((cosf(offset + entity->float18) * -512.0) + 672.0) - button->translateZ) / div) + button->translateZ;
                        button->visible    = true;
                    }
                    offset += entity->float28;
                }

                NativeEntity_AchievementsButton *curButton = entity->buttons[entity->buttonID];
                curButton->labelPtr->alignment             = 0;
                curButton->translateX += ((0.0 - curButton->translateX) / div);
                curButton->translateY += ((16.0 - curButton->translateY) / div);
                curButton->translateZ += ((160.0 - curButton->translateZ) / div);
            }

            if (entity->float14 > 1.0) {
                entity->float14  = 0.0;
                entity->field_70 = 0.0;
                entity->state    = 0;
            }
            break;
        }
        case 6: {
            if (entity->dialog->selection == 2 || entity->dialog->selection == 3) {
                entity->state = 0;
                entity->timer = 50;
            }
            else if (entity->dialog->selection == 1) {
                ExitGame();
                entity->timer = 50;
                entity->state = 0;
            }
            break;
        }
        default: break;
    }
}
