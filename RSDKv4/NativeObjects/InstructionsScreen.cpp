#include "RetroEngine.hpp"

ushort helpText[0x1000];

void InstructionsScreen_Create(void *objPtr)
{
    RSDK_THIS(InstructionsScreen);

    self->labelPtr                  = CREATE_ENTITY(TextLabel);
    self->labelPtr->useRenderMatrix = true;
    self->labelPtr->fontID          = FONT_HEADING;
    self->labelPtr->scale           = 0.2;
    self->labelPtr->alpha           = 256;
    self->labelPtr->x               = -144.0;
    self->labelPtr->y               = 100.0;
    self->labelPtr->z               = 16.0;
    self->labelPtr->state           = TEXTLABEL_STATE_IDLE;
    SetStringToFont(self->labelPtr->text, strInstructions, FONT_HEADING);
    SetStringToFont8(self->pageIDText, "1 / 5", FONT_TEXT);
    SetStringToFont(helpText, strHelpText1, FONT_TEXT);

    self->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", -1);
    SetMeshVertexColors(self->meshPanel, 0, 0, 0, 0xC0);
    self->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA5551);
    self->touchedPrev   = false;
    self->textY         = 68.0;
    self->textHeight    = (GetTextHeight(helpText, FONT_TEXT, 0.14) - 152.0) + 68.0;

    if (Engine.gameDeviceType == RETRO_STANDARD) {
        self->textureDPad = LoadTexture("Data/Game/Menu/Generic.png", TEXFMT_RGBA8888);
        if (timeAttackTex)
            ReplaceTexture("Data/Game/Menu/HelpGraphics.png", timeAttackTex);
        else
            self->textureHelp = LoadTexture("Data/Game/Menu/HelpGraphics.png", TEXFMT_RGBA5551);
    }
    else {
        self->textureDPad = LoadTexture("Data/Game/Menu/VirtualDPad.png", TEXFMT_RGBA8888);
        if (timeAttackTex)
            ReplaceTexture("Data/Game/Menu/HelpGraphics.png", timeAttackTex);
        else
            self->textureHelp = LoadTexture("Data/Game/Menu/HelpGraphics.png", TEXFMT_RGBA5551);
    }
}
void InstructionsScreen_Main(void *objPtr)
{
    RSDK_THIS(InstructionsScreen);
    NativeEntity_OptionsMenu *optionsMenu = (NativeEntity_OptionsMenu *)self->optionsMenu;
    switch (self->state) {
        case INSTRUCTIONSCREEN_STATE_ENTER: {
            if (self->arrowAlpha < 0x100)
                self->arrowAlpha += 8;

            self->scale = fminf(self->scale + ((1.05 - self->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            MatrixScaleXYZF(&self->renderMatrix, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);

            memcpy(&self->labelPtr->renderMatrix, &self->renderMatrix, sizeof(MatrixF));

            self->timer += Engine.deltaTime;
            if (self->timer > 1.0) {
                self->arrowAlpha = 256;
                self->timer      = 0.0;
                self->state      = INSTRUCTIONSCREEN_STATE_MAIN;
            }
            break;
        }

        case INSTRUCTIONSCREEN_STATE_MAIN: {
            CheckKeyDown(&inputDown);
            CheckKeyPress(&inputPress);
            SetRenderMatrix(&self->matrixTemp);

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    self->touchedPrev = false;
                    self->touchedNext = false;
                    self->touchedBack = false;

                    if (self->textHeight > 68.0) {
                        if (inputDown.up)
                            self->textY = fmaxf(68.0, self->textY - 2.0);
                        if (inputDown.down)
                            self->textY = fminf(self->textHeight, self->textY + 2.0);
                    }

                    if (inputDown.left) {
                        PlaySfxByName("Menu Move", false);
                        self->state       = INSTRUCTIONSCREEN_STATE_FLIP;
                        self->stateInput  = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;
                        self->touchedPrev = false;
                        self->flipRight   = true;
                        if (--self->pageID < 0)
                            self->pageID = 4;
                    }
                    else if (inputDown.right) {
                        PlaySfxByName("Menu Move", false);
                        self->state       = INSTRUCTIONSCREEN_STATE_FLIP;
                        self->stateInput  = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;
                        self->touchedNext = false;
                        self->flipRight   = false;
                        self->pageID      = (self->pageID + 1) % 5;
                    }
                }
            }
            else {
                switch (self->stateInput) {
                    case INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS:
                        if (touches > 0) {
                            if (CheckTouchRect(0.0, -8.0, 128.0, 96.0) >= 0 && self->textHeight > 68.0) {
                                self->stateInput         = INSTRUCTIONSCREEN_STATEINPUT_HANDLEMOVEMENT;
                                self->lastSwipeDistanceV = 0.0;
                                self->lastTouchY         = touchYF[0];
                            }
                        }
                        else {
                            self->selectionEnabled = false;
                        }

                        if (inputDown.up || inputDown.down)
                            usePhysicalControls = true;
                        break;

                    case INSTRUCTIONSCREEN_STATEINPUT_HANDLEMOVEMENT:
                        if (touches > 0) {
                            self->swipeDistanceV = self->lastTouchY - touchYF[0];
                            if (self->lastSwipeDistanceV > 0.0 || self->lastSwipeDistanceV < 0.0)
                                self->textVelocity = self->lastSwipeDistanceV - self->swipeDistanceV;
                            else
                                self->textVelocity = 0.0;
                            self->lastSwipeDistanceV = self->swipeDistanceV;
                            self->textY += self->textVelocity;
                        }
                        else {
                            self->stateInput = INSTRUCTIONSCREEN_STATEINPUT_HANDLESCROLL;
                        }
                        break;

                    case INSTRUCTIONSCREEN_STATEINPUT_HANDLESCROLL:
                        if (touches <= 0) {
                            float acc = self->textVelocity / ((60.0 * Engine.deltaTime) * 1.1);
                            self->textY += acc;
                            self->textVelocity = acc;

                            if (abs(self->textVelocity) < 0.0025)
                                self->stateInput = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;

                            if (68.0 - abs(self->textVelocity * 4.0) > self->textY) {
                                self->swipeDistanceV = 68.0;
                                self->stateInput     = INSTRUCTIONSCREEN_STATEINPUT_HANDLESWIPE;
                            }

                            if (self->textY > (abs(self->textVelocity * 4.0) + self->textHeight)) {
                                self->swipeDistanceV = self->textHeight;
                                self->stateInput     = INSTRUCTIONSCREEN_STATEINPUT_HANDLESWIPE;
                            }
                        }
                        else if (CheckTouchRect(0.0, -8.0, 128.0, 96.0) >= 0) {
                            self->stateInput         = INSTRUCTIONSCREEN_STATEINPUT_HANDLEMOVEMENT;
                            self->lastSwipeDistanceV = 0.0;
                            self->lastTouchY         = touchYF[0];
                        }
                        break;

                    case INSTRUCTIONSCREEN_STATEINPUT_HANDLESWIPE:
                        if (touches <= 0) {
                            self->textY = ((self->swipeDistanceV - self->textY) / ((60.0 * Engine.deltaTime) * 8.0)) + self->textY;

                            if (abs(self->swipeDistanceV - self->textY) < 0.025) {
                                self->textY      = self->swipeDistanceV;
                                self->stateInput = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;
                            }
                        }
                        else if (CheckTouchRect(0.0, -8.0, 128.0, 96.0) >= 0) {
                            self->stateInput         = INSTRUCTIONSCREEN_STATEINPUT_HANDLEMOVEMENT;
                            self->lastSwipeDistanceV = 0.0;
                            self->lastTouchY         = touchYF[0];
                        }
                        break;
                }
            }

            if (touches <= 0) {
                if (self->touchedBack) {
                    PlaySfxByName("Menu Back", false);
                    self->touchedBack = false;
                    self->state       = INSTRUCTIONSCREEN_STATE_EXIT;
                }

                if (self->touchedPrev) {
                    PlaySfxByName("Menu Move", false);
                    self->state       = INSTRUCTIONSCREEN_STATE_FLIP;
                    self->stateInput  = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;
                    self->touchedPrev = false;
                    self->flipRight   = true;

                    if (--self->pageID < 0)
                        self->pageID = 4;
                }

                if (self->touchedNext) {
                    PlaySfxByName("Menu Move", false);
                    self->state       = INSTRUCTIONSCREEN_STATE_FLIP;
                    self->stateInput  = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;
                    self->touchedNext = false;
                    self->flipRight   = false;
                    self->pageID      = (self->pageID + 1) % 5;
                }
            }
            else if (self->state == 1) {
                self->touchedBack = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                self->touchedPrev = CheckTouchRect(-162.0, 0.0, 32.0, 32.0) >= 0;
                self->touchedNext = CheckTouchRect(162.0, 0.0, 32.0, 32.0) >= 0;

                if (self->selectionEnabled) {
                    if (self->lastTouchX - touchXF[0] > 16.0f) {
                        PlaySfxByName("Menu Move", false);
                        self->state            = INSTRUCTIONSCREEN_STATE_FLIP;
                        self->stateInput       = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;
                        self->flipRight        = false;
                        self->selectionEnabled = false;
                        self->pageID           = (self->pageID + 1) % 5;
                    }
                    else if (self->lastTouchX - touchXF[0] < -16.0f) {
                        PlaySfxByName("Menu Move", false);
                        self->state            = INSTRUCTIONSCREEN_STATE_FLIP;
                        self->stateInput       = INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS;
                        self->flipRight        = true;
                        self->selectionEnabled = false;
                        if (--self->pageID < 0)
                            self->pageID = 4;
                    }
                }
                else {
                    self->selectionEnabled = true;
                }
                self->lastTouchX = touchXF[0];
            }
            else {
                self->selectionEnabled = false;
            }

            if (inputDown.up || inputDown.down || inputDown.left || inputDown.right)
                usePhysicalControls = true;

            if (self->state == 1 && inputPress.B) {
                PlaySfxByName("Menu Back", false);
                self->touchedBack = false;
                self->state       = INSTRUCTIONSCREEN_STATE_EXIT;
            }
            break;
        }

        case INSTRUCTIONSCREEN_STATE_FLIP: {
            if (!self->flipRight) {
                self->rotationY -= (10.0 * Engine.deltaTime);
            }
            else {
                self->rotationY += (10.0 * Engine.deltaTime);
            }

            if (abs(self->rotationY) > (M_PI * 0.5)) {
                self->state     = INSTRUCTIONSCREEN_STATE_FINISHFLIP;
                self->rotationY = self->rotationY < 0.0f ? -4.712389 : 4.712389;
                switch (self->pageID) {
                    case 0:
                        SetStringToFont(helpText, strHelpText1, FONT_TEXT);
                        SetStringToFont8(self->pageIDText, "1 / 5", 2);
                        break;

                    case 1:
                        if (Engine.gameDeviceType == RETRO_MOBILE)
                            SetStringToFont(helpText, strHelpText2, FONT_TEXT);
                        else
                            SetStringToFont8(helpText, "                                                      CONTROLS", FONT_TEXT);
                        SetStringToFont8(self->pageIDText, "2 / 5", FONT_TEXT);
                        break;

                    case 2:
                        SetStringToFont(helpText, strHelpText3, 2);
                        SetStringToFont8(self->pageIDText, "3 / 5", FONT_TEXT);
                        break;

                    case 3:
                        SetStringToFont(helpText, strHelpText4, 2);
                        SetStringToFont8(self->pageIDText, "4 / 5", FONT_TEXT);
                        break;

                    case 4:
                        SetStringToFont(helpText, strHelpText5, 2);
                        SetStringToFont8(self->pageIDText, "5 / 5", FONT_TEXT);
                        break;

                    default: break;
                }
                self->shownPage  = self->pageID;
                self->textY      = 68.0;
                self->textHeight = (GetTextHeight(helpText, FONT_TEXT, 0.14) - 152.0) + 68.0;
            }

            NewRenderState();
            MatrixRotateYF(&self->renderMatrix, self->rotationY);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);
            break;
        }

        case INSTRUCTIONSCREEN_STATE_FINISHFLIP: {
            if (self->flipRight) {
                self->rotationY += (10.0 * Engine.deltaTime);
                if (self->rotationY > (M_PI_2)) {
                    self->state     = INSTRUCTIONSCREEN_STATE_MAIN;
                    self->rotationY = 0.0;
                }
            }
            else {
                self->rotationY -= (10.0 * Engine.deltaTime);
                if (self->rotationY < -(M_PI_2)) {
                    self->state     = INSTRUCTIONSCREEN_STATE_MAIN;
                    self->rotationY = 0.0;
                }
            }
            NewRenderState();
            MatrixRotateYF(&self->renderMatrix, self->rotationY);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);
            break;
        }

        case INSTRUCTIONSCREEN_STATE_EXIT: {
            if (self->arrowAlpha > 0)
                self->arrowAlpha -= 8;

            if (self->timer < 0.2)
                self->scale = fmaxf(self->scale + ((1.5f - self->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);
            else
                self->scale = fmaxf(self->scale + ((-1.0f - self->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);

            NewRenderState();
            MatrixScaleXYZF(&self->renderMatrix, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);

            memcpy(&self->labelPtr->renderMatrix, &self->renderMatrix, sizeof(MatrixF));

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                optionsMenu->state = OPTIONSMENU_STATE_EXITSUBMENU;
                RemoveNativeObject(self->labelPtr);
                RemoveNativeObject(self);
                return;
            }
            break;
        }
        default: break;
    }

    RenderMesh(self->meshPanel, MESH_COLORS, false);
    RenderTextClipped(helpText, FONT_TEXT, -138.0, self->textY, 8.0, 0.14, 255);

    switch (self->shownPage) {
        case 0:
            if (Engine.gameType == GAME_SONIC1) {
                RenderImageClipped(0.0, self->textY - 36.0, 8.0, 0.3, 0.3, 72.0, 72.0, 144.0, 144.0, 4.0, 4.0, 255, self->textureHelp);
                switch (Engine.language) {
                    default:
                        RenderImageClipped(0.0, self->textY - 164.0, 8.0, 0.3, 0.3, 72.0, 72.0, 144.0, 144.0, 312.0, 300.0, 255, self->textureHelp);
                        break;

                    case RETRO_FR:
                        RenderImageClipped(0.0, self->textY - 172.0, 8.0, 0.3, 0.3, 72.0, 72.0, 144.0, 144.0, 312.0, 300.0, 255, self->textureHelp);
                        break;

                    case RETRO_JP:
                        RenderImageClipped(0.0, self->textY - 248.0, 8.0, 0.3, 0.3, 72.0, 72.0, 144.0, 144.0, 312.0, 300.0, 255, self->textureHelp);
                        break;

                    case RETRO_KO:
                        RenderImageClipped(0.0, self->textY - 268.0, 8.0, 0.3, 0.3, 72.0, 72.0, 144.0, 144.0, 312.0, 300.0, 255, self->textureHelp);
                        break;
                }
            }
            else {
                RenderImageClipped(0.0, self->textY - 36.0, 8.0, 0.3, 0.3, 72.0, 72.0, 144.0, 144.0, 312.0, 300.0, 255, self->textureHelp);
            }
            break;

        case 1:
            if (Engine.gameDeviceType == RETRO_MOBILE) {
                RenderImageClipped(-96.0, self->textY - 28.0, 8.0, 0.2, 0.2, 128.0, 128.0, 256.0, 256.0, 0.0, 0.0, 255, self->textureDPad);
                RenderImageClipped(-96.0, self->textY - 28.0, 8.0, 0.2, 0.2, 84.0, 84.0, 168.0, 168.0, 16.0, 328.0, 255, self->textureDPad);
                RenderImageClipped(-96.0, self->textY - 92.0, 8.0, 0.2, 0.2, 128.0, 128.0, 256.0, 256.0, 0.0, 0.0, 255, self->textureDPad);
                RenderImageClipped(-96.0, self->textY - 92.0, 8.0, 0.2, 0.2, 128.0, 128.0, 256.0, 120.0, 256.0, 256.0, 255, self->textureDPad);
                RenderImageClipped(-96.0, self->textY - 92.0, 8.0, 0.2, 0.2, 84.0, 104.0, 168.0, 168.0, 16.0, 328.0, 255, self->textureDPad);
                RenderImageClipped(-96.0, self->textY - 156.0, 8.0, 0.2, 0.2, 128.0, 128.0, 256.0, 256.0, 0.0, 0.0, 255, self->textureDPad);
                RenderImageClipped(-96.0, self->textY - 156.0, 8.0, 0.2, 0.2, 128.0, -8.0, 256.0, 120.0, 256.0, 392.0, 255, self->textureDPad);
                RenderImageClipped(-96.0, self->textY - 156.0, 8.0, 0.2, 0.2, 84.0, 64.0, 168.0, 168.0, 16.0, 328.0, 255, self->textureDPad);
                RenderImageClipped(-96.0, self->textY - 220.0, 8.0, 0.2, 0.2, 128.0, 128.0, 256.0, 256.0, 256.0, 0.0, 255, self->textureDPad);
                RenderImageClipped(-96.0, self->textY - 220.0, 8.0, 0.25, 0.25, 84.0, 84.0, 168.0, 168.0, 16.0, 328.0, 255, self->textureDPad);
                RenderImageClipped(-96.0, self->textY - 284.0, 8.0, 0.5, 0.5, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, 255, self->textureDPad);
            }
            else {
                RenderImage(0.0, 0.0, 0.0, 0.275, 0.275, 512.0, 256.0, 1024.0, 512.0, 0.0, 0.0, 255, self->textureDPad);
            }
            break;

        case 2: break;

        case 3:
            switch (Engine.language) {
                default:
                    RenderImageClipped(0.0, self->textY - 128.0, 8.0, 0.3, 0.3, 253.0, 45.0, 506.0, 90.0, 155.0, 10.0, 255, self->textureHelp);
                    RenderImageClipped(0.0, self->textY - 220.0, 8.0, 0.3, 0.3, 24.0, 96.0, 48.0, 192.0, 4.0, 152.0, 255, self->textureHelp);
                    break;

                case RETRO_JP:
                    RenderImageClipped(0.0, self->textY - 112.0, 8.0, 0.3, 0.3, 253.0, 45.0, 506.0, 90.0, 155.0, 10.0, 255, self->textureHelp);
                    RenderImageClipped(0.0, self->textY - 232.0, 8.0, 0.3, 0.3, 24.0, 96.0, 48.0, 192.0, 4.0, 152.0, 255, self->textureHelp);
                    break;

                case RETRO_KO:
                    RenderImageClipped(0.0, self->textY - 132.0, 8.0, 0.3, 0.3, 253.0, 45.0, 506.0, 90.0, 155.0, 10.0, 255, self->textureHelp);
                    RenderImageClipped(0.0, self->textY - 232.0, 8.0, 0.3, 0.3, 24.0, 96.0, 48.0, 192.0, 4.0, 152.0, 255, self->textureHelp);
                    break;

                case RETRO_ZS:
                    RenderImageClipped(0.0, self->textY - 122.0, 8.0, 0.3, 0.3, 253.0, 45.0, 506.0, 90.0, 155.0, 10.0, 255, self->textureHelp);
                    RenderImageClipped(0.0, self->textY - 210.0, 8.0, 0.3, 0.3, 24.0, 96.0, 48.0, 192.0, 4.0, 152.0, 255, self->textureHelp);
                    break;
            }
            break;

        case 4:
            RenderImageClipped(0.0, self->textY - 56.0, 8.0, 0.5, 0.5, 162.0, 96.0, 324.0, 192.0, 152.0, 104.0, 255, self->textureHelp);

            if (Engine.gameType == GAME_SONIC1) {
                switch (Engine.language) {
                    default:
                        RenderImageClipped(0.0, self->textY - 188.0, 8.0, 0.3, 0.3, 204.0, 24.0, 408.0, 48.0, 16.0, 448.0, 255, self->textureHelp);
                        RenderImageClipped(0.0, self->textY - 240.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 4.0, 360.0, 255, self->textureHelp);
                        RenderImageClipped(0.0, self->textY - 288.0, 8.0, 0.3, 0.3, 42.0, 42.0, 84.0, 84.0, 56.0, 152.0, 255, self->textureHelp);
                        RenderImageClipped(0.0, self->textY - 336.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 80.0, 360.0, 255, self->textureHelp);
                        RenderImageClipped(0.0, self->textY - 380.0, 8.0, 0.3, 0.3, 76.0, 36.0, 152.0, 72.0, 156.0, 360.0, 255, self->textureHelp);
                        break;

                    case RETRO_JP:
                        RenderImageClipped(0.0, (self->textY - 188.0) - 32.0, 8.0, 0.3, 0.3, 204.0, 24.0, 408.0, 48.0, 16.0, 448.0, 255,
                                           self->textureHelp);
                        RenderImageClipped(0.0, (self->textY - 240.0) - 26.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 4.0, 360.0, 255,
                                           self->textureHelp);
                        RenderImageClipped(0.0, (self->textY - 288.0) - 32.0, 8.0, 0.3, 0.3, 42.0, 42.0, 84.0, 84.0, 56.0, 152.0, 255,
                                           self->textureHelp);
                        RenderImageClipped(0.0, (self->textY - 336.0) - 32.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 80.0, 360.0, 255,
                                           self->textureHelp);
                        RenderImageClipped(0.0, (self->textY - 380.0) - 42.0, 8.0, 0.3, 0.3, 76.0, 36.0, 152.0, 72.0, 156.0, 360.0, 255,
                                           self->textureHelp);
                        break;

                    case RETRO_RU:
                        RenderImageClipped(0.0, self->textY - 204.0, 8.0, 0.3, 0.3, 204.0, 24.0, 408.0, 48.0, 16.0, 448.0, 255, self->textureHelp);
                        RenderImageClipped(0.0, self->textY - 256.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 4.0, 360.0, 255, self->textureHelp);
                        RenderImageClipped(0.0, self->textY - 304.0, 8.0, 0.3, 0.3, 42.0, 42.0, 84.0, 84.0, 56.0, 152.0, 255, self->textureHelp);
                        RenderImageClipped(0.0, self->textY - 352.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 80.0, 360.0, 255, self->textureHelp);
                        RenderImageClipped(0.0, self->textY - 396.0, 8.0, 0.3, 0.3, 76.0, 36.0, 152.0, 72.0, 156.0, 360.0, 255, self->textureHelp);
                        break;

                    case RETRO_ZH:
                        RenderImageClipped(0.0, (self->textY - 188.0) - 22.0, 8.0, 0.3, 0.3, 204.0, 24.0, 408.0, 48.0, 16.0, 448.0, 255,
                                           self->textureHelp);
                        RenderImageClipped(0.0, (self->textY - 240.0) - 20.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 4.0, 360.0, 255,
                                           self->textureHelp);
                        RenderImageClipped(0.0, (self->textY - 288.0) - 22.0, 8.0, 0.3, 0.3, 42.0, 42.0, 84.0, 84.0, 56.0, 152.0, 255,
                                           self->textureHelp);
                        RenderImageClipped(0.0, (self->textY - 336.0) - 22.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 80.0, 360.0, 255,
                                           self->textureHelp);
                        RenderImageClipped(0.0, (self->textY - 380.0) - 22.0, 8.0, 0.3, 0.3, 76.0, 36.0, 152.0, 72.0, 156.0, 360.0, 255,
                                           self->textureHelp);
                        break;

                    case RETRO_ZS:
                        RenderImageClipped(0.0, self->textY - 196.0, 8.0, 0.3, 0.3, 204.0, 24.0, 408.0, 48.0, 16.0, 448.0, 255, self->textureHelp);
                        RenderImageClipped(0.0, self->textY - 240.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 4.0, 360.0, 255, self->textureHelp);
                        RenderImageClipped(0.0, self->textY - 288.0, 8.0, 0.3, 0.3, 42.0, 42.0, 84.0, 84.0, 56.0, 152.0, 255, self->textureHelp);
                        RenderImageClipped(0.0, self->textY - 336.0, 8.0, 0.3, 0.3, 36.0, 36.0, 72.0, 72.0, 80.0, 360.0, 255, self->textureHelp);
                        RenderImageClipped(0.0, self->textY - 380.0, 8.0, 0.3, 0.3, 76.0, 36.0, 152.0, 72.0, 156.0, 360.0, 255, self->textureHelp);
                        break;
                }
            }
            break;
    }

    NewRenderState();
    SetRenderMatrix(NULL);

    if (self->touchedPrev)
        RenderImageFlipH(-146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, self->arrowAlpha, self->textureArrows);
    else
        RenderImageFlipH(-146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, self->arrowAlpha, self->textureArrows);

    if (self->touchedNext)
        RenderImage(146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, self->arrowAlpha, self->textureArrows);
    else
        RenderImage(146.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, self->arrowAlpha, self->textureArrows);

    if (self->touchedBack)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, self->arrowAlpha, self->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, self->arrowAlpha, self->textureArrows);

    SetRenderVertexColor(0x00, 0x00, 0x40);
    RenderText(self->pageIDText, FONT_TEXT, -18.0, -110.0, 160.0, 0.25, 255);
    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
}
