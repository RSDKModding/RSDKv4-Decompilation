#include "RetroEngine.hpp"

ushort helpText[0x1000];

void InstructionsScreen_Create(void *objPtr)
{
    RSDK_THIS(InstructionsScreen);

    entity->labelPtr                  = CREATE_ENTITY(TextLabel);
    entity->labelPtr->useRenderMatrix = true;
    entity->labelPtr->fontID          = FONT_HEADING;
    entity->labelPtr->scale           = 0.2;
    entity->labelPtr->alpha           = 256;
    entity->labelPtr->x               = -144.0;
    entity->labelPtr->y               = 100.0;
    entity->labelPtr->z               = 16.0;
    entity->labelPtr->state           = TEXTLABEL_STATE_IDLE;
    SetStringToFont(entity->labelPtr->text, strInstructions, FONT_HEADING);
    SetStringToFont8(entity->pageIDText, "1 / 5", FONT_TEXT);
    SetStringToFont(helpText, strHelpText1, FONT_TEXT);

    entity->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", -1);
    SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 0xC0);
    entity->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA5551);
    entity->touchedPrev   = false;
    entity->textY         = 68.0;
    entity->textHeight    = (GetTextHeight(helpText, FONT_TEXT, 0.14) - 152.0) + 68.0;
    if (Engine.gameDeviceType == RETRO_STANDARD) {
        entity->textureDPad = LoadTexture("Data/Game/Menu/Generic.png", TEXFMT_RGBA8888);
        if (timeAttackTex)
            ReplaceTexture("Data/Game/Menu/HelpGraphics.png", timeAttackTex);
        else
            entity->textureHelp = LoadTexture("Data/Game/Menu/HelpGraphics.png", TEXFMT_RGBA5551);
    }
    else {
        entity->textureDPad = LoadTexture("Data/Game/Menu/VirtualDPad.png", TEXFMT_RGBA8888);
        if (timeAttackTex)
            ReplaceTexture("Data/Game/Menu/HelpGraphics.png", timeAttackTex);
        else
            entity->textureHelp = LoadTexture("Data/Game/Menu/HelpGraphics.png", TEXFMT_RGBA5551);
    }
}
void InstructionsScreen_Main(void *objPtr)
{
    RSDK_THIS(InstructionsScreen);
    NativeEntity_OptionsMenu *optionsMenu = (NativeEntity_OptionsMenu *)entity->optionsMenu;
    switch (entity->state) {
        case INSTRUCTIONSCREEN_STATE_ENTER: {
            if (entity->arrowAlpha < 0x100)
                entity->arrowAlpha += 8;

            entity->scale = fminf(entity->scale + ((1.05 - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrixTemp);
            SetRenderMatrix(&entity->renderMatrix);

            memcpy(&entity->labelPtr->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            entity->field_1C += Engine.deltaTime;
            if (entity->field_1C > 1.0) {
                entity->arrowAlpha = 256;
                entity->field_1C   = 0.0;
                entity->state      = INSTRUCTIONSCREEN_STATE_MAIN;
            }
            break;
        }
        case INSTRUCTIONSCREEN_STATE_MAIN: {
            CheckKeyDown(&inputDown);
            CheckKeyPress(&inputPress);
            SetRenderMatrix(&entity->matrixTemp);
            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    entity->touchedPrev = false;
                    entity->touchedNext = false;
                    entity->touchedBack = false;
                    if (entity->textHeight > 68.0) {
                        if (inputDown.up)
                            entity->textY = fmaxf(68.0, entity->textY - 2.0);
                        if (inputDown.down)
                            entity->textY = fminf(entity->textHeight, entity->textY + 2.0);
                    }
                    if (inputDown.left) {
                        PlaySfxByName("Menu Move", false);
                        entity->state       = INSTRUCTIONSCREEN_STATE_FLIP;
                        entity->stateInput  = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;
                        entity->touchedPrev = false;
                        entity->flipRight   = true;
                        if (--entity->pageID < 0)
                            entity->pageID = 4;
                    }
                    else if (inputDown.right) {
                        PlaySfxByName("Menu Move", false);
                        entity->state       = INSTRUCTIONSCREEN_STATE_FLIP;
                        entity->stateInput  = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;
                        entity->touchedNext = false;
                        entity->flipRight   = false;
                        entity->pageID      = (entity->pageID + 1) % 5;
                    }
                }
            }
            else {
                switch (entity->stateInput) {
                    case INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS:
                        if (touches > 0) {
                            if (CheckTouchRect(0.0, -8.0, 128.0, 96.0) >= 0 && entity->textHeight > 68.0) {
                                entity->stateInput = INSTRUCTIONSCREEN_STATEINPUT_HANDLEMOVEMENT;
                                entity->field_D4   = 0.0;
                                entity->field_CC   = touchYF[0];
                            }
                        }
                        else {
                            entity->field_E0 = 0;
                        }
                        if (inputDown.up || inputDown.down)
                            usePhysicalControls = true;
                        break;
                    case INSTRUCTIONSCREEN_STATEINPUT_HANDLEMOVEMENT:
                        if (touches > 0) {
                            entity->field_D8 = entity->field_CC - touchYF[0];
                            if (entity->field_D4 > 0.0 || entity->field_D4 < 0.0)
                                entity->field_D0 = entity->field_D4 - entity->field_D8;
                            else
                                entity->field_D0 = 0.0;
                            entity->field_D4 = entity->field_D8;
                            entity->textY += entity->field_D0;
                        }
                        else {
                            entity->stateInput = INSTRUCTIONSCREEN_STATEINPUT_HANDLESCROLL;
                        }
                        break;
                    case INSTRUCTIONSCREEN_STATEINPUT_HANDLESCROLL:
                        if (touches <= 0) {
                            float val = entity->field_D0 / ((60.0 * Engine.deltaTime) * 1.1);
                            entity->textY += val;
                            entity->field_D0 = val;

                            if (abs(entity->field_D0) < 0.0025)
                                entity->stateInput = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;

                            if (68.0 - abs(entity->field_D0 * 4.0) > entity->textY) {
                                entity->field_D8   = 68.0;
                                entity->stateInput = INSTRUCTIONSCREEN_STATEINPUT_HANDLESWIPE;
                            }

                            if (entity->textY > (abs(entity->field_D0 * 4.0) + entity->textHeight)) {
                                entity->field_D8   = entity->textHeight;
                                entity->stateInput = INSTRUCTIONSCREEN_STATEINPUT_HANDLESWIPE;
                            }
                        }
                        else if (CheckTouchRect(0.0, -8.0, 128.0, 96.0) >= 0) {
                            entity->stateInput = INSTRUCTIONSCREEN_STATEINPUT_HANDLEMOVEMENT;
                            entity->field_D4   = 0.0;
                            entity->field_CC   = touchYF[0];
                        }
                        break;
                    case INSTRUCTIONSCREEN_STATEINPUT_HANDLESWIPE:
                        if (touches <= 0) {
                            entity->textY = ((entity->field_D8 - entity->textY) / ((60.0 * Engine.deltaTime) * 8.0)) + entity->textY;

                            if (abs(entity->field_D8 - entity->textY) < 0.025) {
                                entity->textY      = entity->field_D8;
                                entity->stateInput = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;
                            }
                        }
                        else if (CheckTouchRect(0.0, -8.0, 128.0, 96.0) >= 0) {
                            entity->stateInput = INSTRUCTIONSCREEN_STATEINPUT_HANDLEMOVEMENT;
                            entity->field_D4   = 0.0;
                            entity->field_CC   = touchYF[0];
                        }
                        break;
                }
            }

            if (touches <= 0) {
                if (entity->touchedBack) {
                    PlaySfxByName("Menu Back", false);
                    entity->touchedBack = false;
                    entity->state       = INSTRUCTIONSCREEN_STATE_EXIT;
                }
                if (entity->touchedPrev) {
                    PlaySfxByName("Menu Move", false);
                    entity->state       = INSTRUCTIONSCREEN_STATE_FLIP;
                    entity->stateInput  = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;
                    entity->touchedPrev = false;
                    entity->flipRight   = true;
                    if (--entity->pageID < 0)
                        entity->pageID = 4;
                }
                if (entity->touchedNext) {
                    PlaySfxByName("Menu Move", false);
                    entity->state       = INSTRUCTIONSCREEN_STATE_FLIP;
                    entity->stateInput  = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;
                    entity->touchedNext = false;
                    entity->flipRight   = false;
                    entity->pageID      = (entity->pageID + 1) % 5;
                }
            }
            else {
                if (entity->state == 1) {
                    entity->touchedBack = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    entity->touchedPrev = CheckTouchRect(-162.0, 0.0, 32.0, 32.0) >= 0;
                    entity->touchedNext = CheckTouchRect(162.0, 0.0, 32.0, 32.0) >= 0;

                    if (entity->field_E0) {
                        if (entity->lastTouchX - touchXF[0] > 16.0f) {
                            PlaySfxByName("Menu Move", false);
                            entity->state      = INSTRUCTIONSCREEN_STATE_FLIP;
                            entity->stateInput = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;
                            entity->flipRight  = false;
                            entity->field_E0   = false;
                            entity->pageID     = (entity->pageID + 1) % 5;
                        }
                        else if (entity->lastTouchX - touchXF[0] < -16.0f) {
                            PlaySfxByName("Menu Move", false);
                            entity->state      = INSTRUCTIONSCREEN_STATE_FLIP;
                            entity->stateInput = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;
                            entity->flipRight  = true;
                            entity->field_E0   = false;
                            if (--entity->pageID < 0)
                                entity->pageID = 4;
                        }
                    }
                    else {
                        entity->field_E0 = true;
                    }
                    entity->lastTouchX = touchXF[0];

                    if (inputDown.up || inputDown.down || inputDown.left || inputDown.right)
                        usePhysicalControls = true;
                }
                else {
                    entity->field_E0 = false;
                }
            }

            if (entity->state == 1 && inputPress.B) {
                PlaySfxByName("Menu Back", false);
                entity->touchedBack = false;
                entity->state       = INSTRUCTIONSCREEN_STATE_EXIT;
            }
            break;
        }
        case INSTRUCTIONSCREEN_STATE_FLIP: {
            if (!entity->flipRight) {
                entity->rotationY -= (10.0 * Engine.deltaTime);
            }
            else {
                entity->rotationY += (10.0 * Engine.deltaTime);
            }

            if (abs(entity->rotationY) > (M_PI * 0.5)) {
                entity->state     = INSTRUCTIONSCREEN_STATE_FINISHFLIP;
                entity->rotationY = entity->rotationY < 0.0f ? -4.712389 : 4.712389;
                switch (entity->pageID) {
                    case 0:
                        SetStringToFont(helpText, strHelpText1, FONT_TEXT);
                        SetStringToFont8(entity->pageIDText, "1 / 5", 2);
                        break;
                    case 1:
                        if (Engine.gameDeviceType == RETRO_MOBILE)
                            SetStringToFont(helpText, strHelpText2, FONT_TEXT);
                        else
                            SetStringToFont8(helpText, "                                                      CONTROLS", FONT_TEXT);
                        SetStringToFont8(entity->pageIDText, "2 / 5", FONT_TEXT);
                        break;
                    case 2:
                        SetStringToFont(helpText, strHelpText3, 2);
                        SetStringToFont8(entity->pageIDText, "3 / 5", FONT_TEXT);
                        break;
                    case 3:
                        SetStringToFont(helpText, strHelpText4, 2);
                        SetStringToFont8(entity->pageIDText, "4 / 5", FONT_TEXT);
                        break;
                    case 4:
                        SetStringToFont(helpText, strHelpText5, 2);
                        SetStringToFont8(entity->pageIDText, "5 / 5", FONT_TEXT);
                        break;
                    default: break;
                }
                entity->shownPage  = entity->pageID;
                entity->textY      = 68.0;
                entity->textHeight = (GetTextHeight(helpText, FONT_TEXT, 0.14) - 152.0) + 68.0;
            }

            NewRenderState();
            matrixRotateYF(&entity->renderMatrix, entity->rotationY);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrixTemp);
            SetRenderMatrix(&entity->renderMatrix);
            break;
        }
        case INSTRUCTIONSCREEN_STATE_FINISHFLIP: {
            if (entity->flipRight) {
                entity->rotationY += (10.0 * Engine.deltaTime);
                if (entity->rotationY > (M_PI_2)) {
                    entity->state     = INSTRUCTIONSCREEN_STATE_MAIN;
                    entity->rotationY = 0.0;
                }
            }
            else {
                entity->rotationY -= (10.0 * Engine.deltaTime);
                if (entity->rotationY < -(M_PI_2)) {
                    entity->state     = INSTRUCTIONSCREEN_STATE_MAIN;
                    entity->rotationY = 0.0;
                }
            }
            NewRenderState();
            matrixRotateYF(&entity->renderMatrix, entity->rotationY);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrixTemp);
            SetRenderMatrix(&entity->renderMatrix);
            break;
        }
        case INSTRUCTIONSCREEN_STATE_EXIT: {
            if (entity->arrowAlpha > 0)
                entity->arrowAlpha -= 8;

            if (entity->field_1C < 0.2)
                entity->scale = fmaxf(entity->scale + ((1.5f - entity->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);
            else
                entity->scale = fmaxf(entity->scale + ((-1.0f - entity->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrixTemp);
            SetRenderMatrix(&entity->renderMatrix);

            memcpy(&entity->labelPtr->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            entity->field_1C += Engine.deltaTime;
            if (entity->field_1C > 0.5) {
                optionsMenu->state = OPTIONSMENU_STATE_EXITSUBMENU;
                RemoveNativeObject(entity->labelPtr);
                RemoveNativeObject(entity);
                return;
            }
            break;
        }
        default: break;
    }

    RenderMesh(entity->meshPanel, MESH_COLOURS, false);
    RenderTextClipped(helpText, FONT_TEXT, -138.0, entity->textY, 8.0, 0.14, 255);

    switch (entity->shownPage) {
        case 0:
            RenderImageClipped(0.0, entity->textY - 36.0, 8.0, 0.3, 0.3, 72.0, 72.0, 144.0, 144.0, 4.0, 4.0, 255, entity->textureHelp);
            switch (Engine.language) {
                default:
                    RenderImageClipped(0.0, entity->textY - 164.0, 8.0, 0.3, 0.3, 72.0, 72.0, 144.0, 144.0, 312.0, 300.0, 255, entity->textureHelp);
                    break;
                case RETRO_FR:
                    RenderImageClipped(0.0, entity->textY - 172.0, 8.0, 0.3, 0.3, 72.0, 72.0, 144.0, 144.0, 312.0, 300.0, 255, entity->textureHelp);
                    break;
                case RETRO_JP:
                    RenderImageClipped(0.0, entity->textY - 248.0, 8.0, 0.3, 0.3, 72.0, 72.0, 144.0, 144.0, 312.0, 300.0, 255, entity->textureHelp);
                    break;
                case RETRO_KO:
                    RenderImageClipped(0.0, entity->textY - 268.0, 8.0, 0.3, 0.3, 72.0, 72.0, 144.0, 144.0, 312.0, 300.0, 255, entity->textureHelp);
                    break;
            }
            break;
        case 1:
            if (Engine.gameDeviceType == RETRO_MOBILE) {
                RenderImageClipped(-96.0, entity->textY - 28.0, 8.0, 0.2, 0.2, 128.0, 128.0, 256.0, 256.0, 0.0, 0.0, 255, entity->textureDPad);
                RenderImageClipped(-96.0, entity->textY - 28.0, 8.0, 0.2, 0.2, 84.0, 84.0, 168.0, 168.0, 16.0, 328.0, 255, entity->textureDPad);
                RenderImageClipped(-96.0, entity->textY - 92.0, 8.0, 0.2, 0.2, 128.0, 128.0, 256.0, 256.0, 0.0, 0.0, 255, entity->textureDPad);
                RenderImageClipped(-96.0, entity->textY - 92.0, 8.0, 0.2, 0.2, 128.0, 128.0, 256.0, 120.0, 256.0, 256.0, 255, entity->textureDPad);
                RenderImageClipped(-96.0, entity->textY - 92.0, 8.0, 0.2, 0.2, 84.0, 104.0, 168.0, 168.0, 16.0, 328.0, 255, entity->textureDPad);
                RenderImageClipped(-96.0, entity->textY - 156.0, 8.0, 0.2, 0.2, 128.0, 128.0, 256.0, 256.0, 0.0, 0.0, 255, entity->textureDPad);
                RenderImageClipped(-96.0, entity->textY - 156.0, 8.0, 0.2, 0.2, 128.0, -8.0, 256.0, 120.0, 256.0, 392.0, 255, entity->textureDPad);
                RenderImageClipped(-96.0, entity->textY - 156.0, 8.0, 0.2, 0.2, 84.0, 64.0, 168.0, 168.0, 16.0, 328.0, 255, entity->textureDPad);
                RenderImageClipped(-96.0, entity->textY - 220.0, 8.0, 0.2, 0.2, 128.0, 128.0, 256.0, 256.0, 256.0, 0.0, 255, entity->textureDPad);
                RenderImageClipped(-96.0, entity->textY - 220.0, 8.0, 0.25, 0.25, 84.0, 84.0, 168.0, 168.0, 16.0, 328.0, 255, entity->textureDPad);
                RenderImageClipped(-96.0, entity->textY - 284.0, 8.0, 0.5, 0.5, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, 255, entity->textureDPad);
            }
            else {
                RenderImage(0.0, 0.0, 0.0, 0.275, 0.275, 512.0, 256.0, 1024.0, 512.0, 0.0, 0.0, 255, entity->textureDPad);
            }
            break;
        case 2: break;
        case 3:
            switch (Engine.language) {
                default:
                    RenderImageClipped(0.0, entity->textY - 128.0, 8.0, 0.3, 0.3, 253.0, 45.0, 506.0, 90.0, 155.0, 10.0, 255, entity->textureHelp);
                    RenderImageClipped(0.0, entity->textY - 220.0, 8.0, 0.3, 0.3, 24.0, 96.0, 48.0, 192.0, 4.0, 152.0, 255, entity->textureHelp);
                    break;
                case RETRO_JP:
                    RenderImageClipped(0.0, entity->textY - 112.0, 8.0, 0.3, 0.3, 253.0, 45.0, 506.0, 90.0, 155.0, 10.0, 255, entity->textureHelp);
                    RenderImageClipped(0.0, entity->textY - 232.0, 8.0, 0.3, 0.3, 24.0, 96.0, 48.0, 192.0, 4.0, 152.0, 255, entity->textureHelp);
                    break;
                case RETRO_KO:
                    RenderImageClipped(0.0, entity->textY - 132.0, 8.0, 0.3, 0.3, 253.0, 45.0, 506.0, 90.0, 155.0, 10.0, 255, entity->textureHelp);
                    RenderImageClipped(0.0, entity->textY - 232.0, 8.0, 0.3, 0.3, 24.0, 96.0, 48.0, 192.0, 4.0, 152.0, 255, entity->textureHelp);
                    break;
                case RETRO_ZS:
                    RenderImageClipped(0.0, entity->textY - 122.0, 8.0, 0.3, 0.3, 253.0, 45.0, 506.0, 90.0, 155.0, 10.0, 255, entity->textureHelp);
                    RenderImageClipped(0.0, entity->textY - 210.0, 8.0, 0.3, 0.3, 24.0, 96.0, 48.0, 192.0, 4.0, 152.0, 255, entity->textureHelp);
                    break;
            }
            break;
        case 4:
            RenderImageClipped(0.0, entity->textY - 56.0, 8.0, 0.5, 0.5, 162.0, 96.0, 324.0, 192.0, 152.0, 104.0, 255, entity->textureHelp);

            if (Engine.gameType == GAME_SONIC1) {
                switch (Engine.language) {
                    default:
                        RenderImageClipped(0.0, entity->textY - 188.0, 8.0, 0.3, 0.3, 204.0, 24.0, 408.0, 48.0, 16.0, 448.0, 255,
                                           entity->textureHelp);
                        RenderImageClipped(0.0, entity->textY - 240.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 4.0, 360.0, 255, entity->textureHelp);
                        RenderImageClipped(0.0, entity->textY - 288.0, 8.0, 0.3, 0.3, 42.0, 42.0, 84.0, 84.0, 56.0, 152.0, 255, entity->textureHelp);
                        RenderImageClipped(0.0, entity->textY - 336.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 80.0, 360.0, 255, entity->textureHelp);
                        RenderImageClipped(0.0, entity->textY - 380.0, 8.0, 0.3, 0.3, 76.0, 36.0, 152.0, 72.0, 156.0, 360.0, 255,
                                           entity->textureHelp);
                        break;
                    case RETRO_JP:
                        RenderImageClipped(0.0, (entity->textY - 188.0) - 32.0, 8.0, 0.3, 0.3, 204.0, 24.0, 408.0, 48.0, 16.0, 448.0, 255,
                                           entity->textureHelp);
                        RenderImageClipped(0.0, (entity->textY - 240.0) - 26.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 4.0, 360.0, 255,
                                           entity->textureHelp);
                        RenderImageClipped(0.0, (entity->textY - 288.0) - 32.0, 8.0, 0.3, 0.3, 42.0, 42.0, 84.0, 84.0, 56.0, 152.0, 255,
                                           entity->textureHelp);
                        RenderImageClipped(0.0, (entity->textY - 336.0) - 32.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 80.0, 360.0, 255,
                                           entity->textureHelp);
                        RenderImageClipped(0.0, (entity->textY - 380.0) - 42.0, 8.0, 0.3, 0.3, 76.0, 36.0, 152.0, 72.0, 156.0, 360.0, 255,
                                           entity->textureHelp);
                        break;
                    case RETRO_RU:
                        RenderImageClipped(0.0, entity->textY - 204.0, 8.0, 0.3, 0.3, 204.0, 24.0, 408.0, 48.0, 16.0, 448.0, 255,
                                           entity->textureHelp);
                        RenderImageClipped(0.0, entity->textY - 256.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 4.0, 360.0, 255, entity->textureHelp);
                        RenderImageClipped(0.0, entity->textY - 304.0, 8.0, 0.3, 0.3, 42.0, 42.0, 84.0, 84.0, 56.0, 152.0, 255, entity->textureHelp);
                        RenderImageClipped(0.0, entity->textY - 352.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 80.0, 360.0, 255, entity->textureHelp);
                        RenderImageClipped(0.0, entity->textY - 396.0, 8.0, 0.3, 0.3, 76.0, 36.0, 152.0, 72.0, 156.0, 360.0, 255,
                                           entity->textureHelp);
                        break;
                    case RETRO_ZH:
                        RenderImageClipped(0.0, (entity->textY - 188.0) - 22.0, 8.0, 0.3, 0.3, 204.0, 24.0, 408.0, 48.0, 16.0, 448.0, 255,
                                           entity->textureHelp);
                        RenderImageClipped(0.0, (entity->textY - 240.0) - 20.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 4.0, 360.0, 255,
                                           entity->textureHelp);
                        RenderImageClipped(0.0, (entity->textY - 288.0) - 22.0, 8.0, 0.3, 0.3, 42.0, 42.0, 84.0, 84.0, 56.0, 152.0, 255,
                                           entity->textureHelp);
                        RenderImageClipped(0.0, (entity->textY - 336.0) - 22.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 80.0, 360.0, 255,
                                           entity->textureHelp);
                        RenderImageClipped(0.0, (entity->textY - 380.0) - 22.0, 8.0, 0.3, 0.3, 76.0, 36.0, 152.0, 72.0, 156.0, 360.0, 255,
                                           entity->textureHelp);
                        break;
                    case RETRO_ZS:
                        RenderImageClipped(0.0, entity->textY - 196.0, 8.0, 0.3, 0.3, 204.0, 24.0, 408.0, 48.0, 16.0, 448.0, 255,
                                           entity->textureHelp);
                        RenderImageClipped(0.0, entity->textY - 240.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 4.0, 360.0, 255, entity->textureHelp);
                        RenderImageClipped(0.0, entity->textY - 288.0, 8.0, 0.3, 0.3, 42.0, 42.0, 84.0, 84.0, 56.0, 152.0, 255, entity->textureHelp);
                        RenderImageClipped(0.0, entity->textY - 336.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 80.0, 360.0, 255, entity->textureHelp);
                        RenderImageClipped(0.0, entity->textY - 380.0, 8.0, 0.3, 0.3, 76.0, 36.0, 152.0, 72.0, 156.0, 360.0, 255,
                                           entity->textureHelp);
                        break;
                }
            }
            break;
    }

    NewRenderState();
    SetRenderMatrix(NULL);

    if (entity->touchedPrev)
        RenderImageFlipH(-146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, entity->arrowAlpha, entity->textureArrows);
    else
        RenderImageFlipH(-146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, entity->arrowAlpha, entity->textureArrows);

    if (entity->touchedNext)
        RenderImage(146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, entity->arrowAlpha, entity->textureArrows);
    else
        RenderImage(146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, entity->arrowAlpha, entity->textureArrows);

    if (entity->touchedBack)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, entity->arrowAlpha, entity->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, entity->arrowAlpha, entity->textureArrows);
    SetRenderVertexColor(0x00, 0x00, 0x40);
    RenderText(entity->pageIDText, FONT_TEXT, -18.0, -110.0, 160.0, 0.25, 255);
    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
}
