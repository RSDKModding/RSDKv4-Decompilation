#include "RetroEngine.hpp"

#if !RETRO_USE_ORIGINAL_CODE && RETRO_USE_NETWORKING

void MultiplayerScreen_Create(void *objPtr)
{
    if (skipStartMenu) {
        // code has been copied here from SegaSplash_Create due to the possibility of loading the 2P stage without the HW menus >:(
        ResetBitmapFonts();
        int heading = 0, labelTex = 0, textTex = 0;

        if (Engine.useHighResAssets)
            heading = LoadTexture("Data/Game/Menu/Heading_EN.png", TEXFMT_RGBA4444);
        else
            heading = LoadTexture("Data/Game/Menu/Heading_EN@1x.png", TEXFMT_RGBA4444);
        LoadBitmapFont("Data/Game/Menu/Heading_EN.fnt", FONT_HEADING, heading);

        if (Engine.useHighResAssets)
            labelTex = LoadTexture("Data/Game/Menu/Label_EN.png", TEXFMT_RGBA4444);
        else
            labelTex = LoadTexture("Data/Game/Menu/Label_EN@1x.png", TEXFMT_RGBA4444);
        LoadBitmapFont("Data/Game/Menu/Label_EN.fnt", FONT_LABEL, labelTex);

        textTex = LoadTexture("Data/Game/Menu/Text_EN.png", TEXFMT_RGBA4444);
        LoadBitmapFont("Data/Game/Menu/Text_EN.fnt", FONT_TEXT, textTex);

        switch (Engine.language) {
            case RETRO_JP:
                heading = LoadTexture("Data/Game/Menu/Heading_JA@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Heading_JA.fnt", FONT_HEADING, heading);

                labelTex = LoadTexture("Data/Game/Menu/Label_JA@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Label_JA.fnt", FONT_LABEL, labelTex);

                textTex = LoadTexture("Data/Game/Menu/Text_JA@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Text_JA.fnt", FONT_TEXT, textTex);
                break;
            case RETRO_RU:
                if (Engine.useHighResAssets)
                    heading = LoadTexture("Data/Game/Menu/Heading_RU.png", TEXFMT_RGBA4444);
                else
                    heading = LoadTexture("Data/Game/Menu/Heading_RU@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Heading_RU.fnt", FONT_HEADING, heading);

                if (Engine.useHighResAssets)
                    labelTex = LoadTexture("Data/Game/Menu/Label_RU.png", TEXFMT_RGBA4444);
                else
                    labelTex = LoadTexture("Data/Game/Menu/Label_RU@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Label_RU.fnt", FONT_LABEL, labelTex);
                break;
            case RETRO_KO:
                heading = LoadTexture("Data/Game/Menu/Heading_KO@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Heading_KO.fnt", FONT_HEADING, heading);

                labelTex = LoadTexture("Data/Game/Menu/Label_KO@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Label_KO.fnt", FONT_LABEL, labelTex);

                textTex = LoadTexture("Data/Game/Menu/Text_KO.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Text_KO.fnt", FONT_TEXT, textTex);
                break;
            case RETRO_ZH:
                heading = LoadTexture("Data/Game/Menu/Heading_ZH@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Heading_ZH.fnt", FONT_HEADING, heading);

                labelTex = LoadTexture("Data/Game/Menu/Label_ZH@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Label_ZH.fnt", FONT_LABEL, labelTex);

                textTex = LoadTexture("Data/Game/Menu/Text_ZH@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Text_ZH.fnt", FONT_TEXT, textTex);
                break;
            case RETRO_ZS:
                heading = LoadTexture("Data/Game/Menu/Heading_ZHS@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Heading_ZHS.fnt", FONT_HEADING, heading);
                labelTex = LoadTexture("Data/Game/Menu/Label_ZHS@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Label_ZHS.fnt", FONT_LABEL, labelTex);
                textTex = LoadTexture("Data/Game/Menu/Text_ZHS@1x.png", TEXFMT_RGBA4444);
                LoadBitmapFont("Data/Game/Menu/Text_ZHS.fnt", FONT_TEXT, textTex);
                break;
            default: break;
        }
    }

    RSDK_THIS(MultiplayerScreen);

    self->state = MULTIPLAYERSCREEN_STATE_ENTER;

    self->label                  = CREATE_ENTITY(TextLabel);
    self->label->useRenderMatrix = true;
    self->label->fontID          = FONT_HEADING;
    self->label->scale           = 0.2;
    self->label->alpha           = 256;
    self->label->x               = -144.0;
    self->label->y               = 100.0;
    self->label->z               = 16.0;
    self->label->state           = TEXTLABEL_STATE_IDLE;
    SetStringToFont8(self->label->text, "MULTIPLAYER", FONT_HEADING);

    self->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", -2);
    SetMeshVertexColors(self->meshPanel, 0, 0, 0, 0xC0);
    self->textureArrows                                           = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);
    self->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]                  = CREATE_ENTITY(PushButton);
    self->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->useRenderMatrix = true;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->x               = 0.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->y               = 20.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->z               = 0.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->scale           = 0.25;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->bgColor         = 0x00A048;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->bgColorSelected = 0x00C060;
    SetStringToFont8(self->buttons[0]->text, "HOST", FONT_LABEL);

    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]                  = CREATE_ENTITY(PushButton);
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->useRenderMatrix = true;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->x               = 0.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->y               = -20.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->z               = 0.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->scale           = 0.25;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->bgColor         = 0x00A048;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->bgColorSelected = 0x00C060;
    SetStringToFont8(self->buttons[1]->text, "JOIN", FONT_LABEL);

    self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]                  = CREATE_ENTITY(PushButton);
    self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->useRenderMatrix = true;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->x               = 0.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->y               = -56.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->z               = 0.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->scale           = 0.175;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->bgColor         = 0x00A048;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->bgColorSelected = 0x00C060;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->alpha           = 0;
    SetStringToFont8(self->buttons[2]->text, "COPY", FONT_LABEL);

    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]                  = CREATE_ENTITY(PushButton);
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->useRenderMatrix = true;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->x               = -56.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->y               = -56.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->z               = 0.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->scale           = 0.175;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->bgColor         = 0x00A048;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->bgColorSelected = 0x00C060;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->alpha           = 0;
    SetStringToFont8(self->buttons[3]->text, "JOIN ROOM", FONT_LABEL);

    self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]                  = CREATE_ENTITY(PushButton);
    self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->useRenderMatrix = true;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->x               = 64.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->y               = -56.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->z               = 0.0;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->scale           = 0.175;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->bgColor         = 0x00A048;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->bgColorSelected = 0x00C060;
    self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->alpha           = 0;
    SetStringToFont8(self->buttons[4]->text, "PASTE", FONT_LABEL);

    for (int i = 0; i < 3; ++i) {
        self->codeLabel[i]                  = CREATE_ENTITY(TextLabel);
        self->codeLabel[i]->useRenderMatrix = true;
        self->codeLabel[i]->fontID          = FONT_LABEL;
        self->codeLabel[i]->scale           = 0.15;
        self->codeLabel[i]->alpha           = 0;
        self->codeLabel[i]->x               = 0;
        self->codeLabel[i]->y               = 0;
        self->codeLabel[i]->z               = 16.0;
        self->codeLabel[i]->state           = TEXTLABEL_STATE_IDLE;
    }

    SetStringToFont8(self->codeLabel[0]->text, "ROOM CODE", self->codeLabel[0]->fontID);
    self->codeLabel[0]->alignPtr(self->codeLabel[0], ALIGN_CENTER);

    SetStringToFont8(self->codeLabel[1]->text, "UNKNOWN", self->codeLabel[1]->fontID);
    self->codeLabel[1]->alignPtr(self->codeLabel[1], ALIGN_CENTER);
    self->codeLabel[1]->y -= 20.0f;

    self->codeLabel[2]->y     = 48.0f;
    self->codeLabel[2]->scale = 0.20f;
    SetStringToFont8(self->codeLabel[2]->text, "WAITING FOR 2P...", self->codeLabel[2]->fontID);
    self->codeLabel[2]->alignPtr(self->codeLabel[2], ALIGN_CENTER);

    char codeBuf[0x8];
    sprintf(codeBuf, "%X", 0);

    for (int i = 0; i < 8; ++i) {
        self->enterCodeLabel[i]                  = CREATE_ENTITY(TextLabel);
        self->enterCodeLabel[i]->useRenderMatrix = true;
        self->enterCodeLabel[i]->fontID          = FONT_LABEL;
        self->enterCodeLabel[i]->scale           = 0.25;
        self->enterCodeLabel[i]->alpha           = 0;
        self->enterCodeLabel[i]->x               = -102.0f + (i * 27.2) + 8.0f;
        self->enterCodeLabel[i]->y               = 0;
        self->enterCodeLabel[i]->z               = 16.0;
        self->enterCodeLabel[i]->state           = TEXTLABEL_STATE_IDLE;

        self->enterCodeLabel[i]->r = 0xFF;
        self->enterCodeLabel[i]->g = 0xFF;
        self->enterCodeLabel[i]->b = 0x00;

        SetStringToFont8(self->enterCodeLabel[i]->text, codeBuf, self->enterCodeLabel[i]->fontID);
        self->enterCodeLabel[i]->alignPtr(self->enterCodeLabel[i], ALIGN_CENTER);
    }
    for (int i = 0; i < 2; ++i) {
        self->enterCodeSlider[i]                  = CREATE_ENTITY(TextLabel);
        self->enterCodeSlider[i]->useRenderMatrix = true;
        self->enterCodeSlider[i]->fontID          = FONT_LABEL;
        self->enterCodeSlider[i]->scale           = 0.175;
        self->enterCodeSlider[i]->alpha           = 0;
        // self->enterCodeSlider[i]->textX           = -102.0f + (i * 27.2) + 8.0f;
        self->enterCodeSlider[i]->y     = (i ? -23.0 : -40.0);
        self->enterCodeSlider[i]->z     = 16.0;
        self->enterCodeSlider[i]->state = TEXTLABEL_STATE_IDLE;
        SetStringToFont8(self->enterCodeSlider[i]->text, "V", self->enterCodeSlider[i]->fontID);
        self->enterCodeSlider[i]->alignPtr(self->enterCodeSlider[i], ALIGN_CENTER);
    }
}

void MultiplayerScreen_DrawJoinCode(void *objPtr, int v)
{
    RSDK_THIS(MultiplayerScreen);
    union {
        uint val;
        byte bytes[4];
    } u;
    u.val = self->roomCode;

    for (int i = 0; i < 8; i += 2) {
        int n         = 7 - i;
        int nybbles[] = { u.bytes[n >> 1] & 0xF, ((u.bytes[n >> 1] & 0xF0) >> 4) & 0xF };

        self->enterCodeLabel[i + 0]->alpha = 0x100;
        self->enterCodeLabel[i + 1]->alpha = 0x100;

        self->enterCodeLabel[i + 0]->useColors = false;
        self->enterCodeLabel[i + 1]->useColors = false;

        char codeBuf[0x10];
        sprintf(codeBuf, "%X", nybbles[1]);
        SetStringToFont8(self->enterCodeLabel[i + 0]->text, codeBuf, self->enterCodeLabel[i + 0]->fontID);
        self->enterCodeLabel[i + 0]->alignPtr(self->enterCodeLabel[i + 0], ALIGN_CENTER);

        sprintf(codeBuf, "%X", nybbles[0]);
        SetStringToFont8(self->enterCodeLabel[i + 1]->text, codeBuf, self->enterCodeLabel[i + 1]->fontID);
        self->enterCodeLabel[i + 1]->alignPtr(self->enterCodeLabel[i + 1], ALIGN_CENTER);
    }
    self->enterCodeLabel[v]->useColors = true;
}

void MultiplayerScreen_Destroy(void *objPtr)
{
    RSDK_THIS(MultiplayerScreen);
    RemoveNativeObject(self->label);
    for (int i = 0; i < 3; ++i) RemoveNativeObject(self->codeLabel[i]);
    for (int i = 0; i < 8; ++i) RemoveNativeObject(self->enterCodeLabel[i]);
    for (int i = 0; i < 2; ++i) RemoveNativeObject(self->enterCodeSlider[i]);
    for (int i = 0; i < MULTIPLAYERSCREEN_BUTTON_COUNT; ++i) RemoveNativeObject(self->buttons[i]);
    RemoveNativeObject(self->bg);
    RemoveNativeObject(self);
}

void MultiplayerScreen_Main(void *objPtr)
{
    RSDK_THIS(MultiplayerScreen);

    if (dcError && self->state == MULTIPLAYERSCREEN_STATE_HOSTSCR)
        CREATE_ENTITY(MultiplayerHandler);

    switch (self->state) {
        case MULTIPLAYERSCREEN_STATE_ENTER: {
            if (self->arrowAlpha < 0x100)
                self->arrowAlpha += 8;

            self->scale = fminf(self->scale + ((1.05 - self->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            MatrixScaleXYZF(&self->renderMatrix, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);

            memcpy(&self->label->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 3; ++i) memcpy(&self->codeLabel[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < MULTIPLAYERSCREEN_BUTTON_COUNT; ++i) memcpy(&self->buttons[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 8; ++i) memcpy(&self->enterCodeLabel[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            memcpy(&self->enterCodeSlider[1]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            MatrixRotateZF(&self->enterCodeSlider[0]->renderMatrix, DegreesToRad(180));
            MatrixMultiplyF(&self->enterCodeSlider[0]->renderMatrix, &self->renderMatrix);

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                self->timer = 0.0;
                self->state = MULTIPLAYERSCREEN_STATE_MAIN;
            }
            break;
        }
        case MULTIPLAYERSCREEN_STATE_MAIN: {
            CheckKeyDown(&inputDown);
            CheckKeyPress(&inputPress);
            SetRenderMatrix(&self->renderMatrix);

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (inputPress.up) {
                        PlaySfxByName("Menu Move", false);
                        self->selectedButton--;
                        if (self->selectedButton < MULTIPLAYERSCREEN_BUTTON_HOST)
                            self->selectedButton = MULTIPLAYERSCREEN_BUTTON_JOIN;
                    }
                    else if (inputPress.down) {
                        PlaySfxByName("Menu Move", false);
                        self->selectedButton++;
                        if (self->selectedButton > MULTIPLAYERSCREEN_BUTTON_JOIN)
                            self->selectedButton = MULTIPLAYERSCREEN_BUTTON_HOST;
                    }

                    self->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->state = PUSHBUTTON_STATE_UNSELECTED;
                    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->state = PUSHBUTTON_STATE_UNSELECTED;
                    self->buttons[self->selectedButton]->state          = PUSHBUTTON_STATE_SELECTED;

                    if (inputPress.start || inputPress.A) {
                        PlaySfxByName("Menu Select", false);
                        self->buttons[self->selectedButton]->state = PUSHBUTTON_STATE_FLASHING;
                        self->state                                = MULTIPLAYERSCREEN_STATE_ACTION;
                    }
                    else if (inputPress.B || self->backPressed) {
                        PlaySfxByName("Menu Back", false);
                        self->backPressed             = false;
                        self->state                   = MULTIPLAYERSCREEN_STATE_EXIT;
                        self->timer                   = 0;
                        NativeEntity_FadeScreen *fade = CREATE_ENTITY(FadeScreen);
                        fade->delay                   = 1.1f;
                        fade->state                   = FADESCREEN_STATE_FADEOUT;
                    }
                }
            }
            else {
                if (touches > 0) {
                    for (int i = 0; i < 2; ++i) {
                        self->buttons[i]->state =
                            CheckTouchRect(0, 20 - (i * 40), ((64.0 * self->buttons[i]->scale) + self->buttons[i]->textWidth) * 0.75, 20.4) >= 0;
                    }
                    self->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (self->state == MULTIPLAYERSCREEN_STATE_MAIN) {
                        if (inputDown.left) {
                            self->selectedButton = MULTIPLAYERSCREEN_BUTTON_JOIN;
                            usePhysicalControls  = true;
                        }
                        if (inputDown.right) {
                            self->selectedButton = MULTIPLAYERSCREEN_BUTTON_HOST;
                            usePhysicalControls  = true;
                        }
                    }
                }
                else {
                    for (int i = 0; i < 2; ++i) {
                        if (self->buttons[i]->state == PUSHBUTTON_STATE_SELECTED) {
                            PlaySfxByName("Menu Select", false);
                            self->buttons[i]->state = PUSHBUTTON_STATE_FLASHING;
                            self->selectedButton    = i;
                            self->state             = MULTIPLAYERSCREEN_STATE_ACTION;
                            break;
                        }
                    }

                    if (inputPress.B || self->backPressed) {
                        PlaySfxByName("Menu Back", false);
                        self->backPressed             = false;
                        self->state                   = MULTIPLAYERSCREEN_STATE_EXIT;
                        self->timer                   = 0;
                        NativeEntity_FadeScreen *fade = CREATE_ENTITY(FadeScreen);
                        fade->delay                   = 1.0;
                        fade->state                   = FADESCREEN_STATE_FADEOUT;
                    }
                    else {
                        if (self->state == MULTIPLAYERSCREEN_STATE_MAIN) {
                            if (inputDown.down) {
                                self->selectedButton = MULTIPLAYERSCREEN_BUTTON_JOIN;
                                usePhysicalControls  = true;
                            }
                            if (inputDown.up) {
                                self->selectedButton = MULTIPLAYERSCREEN_BUTTON_HOST;
                                usePhysicalControls  = true;
                            }
                        }
                    }
                }
            }
            break;
        }
        case MULTIPLAYERSCREEN_STATE_ACTION: { // action
            CheckKeyDown(&inputDown);
            SetRenderMatrix(&self->renderMatrix);

            if (self->buttons[self->selectedButton]->state == PUSHBUTTON_STATE_UNSELECTED) {
                self->state = MULTIPLAYERSCREEN_STATE_MAIN;
                switch (self->selectedButton) {
                    default: break;
                    case MULTIPLAYERSCREEN_BUTTON_HOST:
                        self->state         = MULTIPLAYERSCREEN_STATE_FLIP;
                        self->nextState     = MULTIPLAYERSCREEN_STATE_HOSTSCR;
                        self->nextStateDraw = MULTIPLAYERSCREEN_STATEDRAW_HOST;
                        break;
                    case MULTIPLAYERSCREEN_BUTTON_JOIN:
                        self->state         = MULTIPLAYERSCREEN_STATE_FLIP;
                        self->nextState     = MULTIPLAYERSCREEN_STATE_JOINSCR;
                        self->nextStateDraw = MULTIPLAYERSCREEN_STATEDRAW_JOIN;
                        break;
                    case MULTIPLAYERSCREEN_BUTTON_JOINROOM: {
                        self->state                   = MULTIPLAYERSCREEN_STATE_STARTGAME;
                        NativeEntity_FadeScreen *fade = CREATE_ENTITY(FadeScreen);
                        fade->state                   = FADESCREEN_STATE_GAMEFADEOUT;
                        fade->delay                   = 1.1f;
                        break;
                    }
                }
            }
            break;
        }
        case MULTIPLAYERSCREEN_STATE_STARTGAME:
        case MULTIPLAYERSCREEN_STATE_EXIT: {
            if (self->dialog)
                self->dialog->state = DIALOGPANEL_STATE_IDLE;
            if (self->arrowAlpha > 0)
                self->arrowAlpha -= 8;

            /*if (self->timer < 0.2)
                self->scale = fmaxf(self->scale + ((1.5f - self->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);
            else
                self->scale = fmaxf(self->scale + ((-1.0f - self->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);
            //*/
            NewRenderState();
            MatrixScaleXYZF(&self->renderMatrix, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);

            memcpy(&self->label->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 3; ++i) memcpy(&self->codeLabel[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < MULTIPLAYERSCREEN_BUTTON_COUNT; ++i) memcpy(&self->buttons[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 8; ++i) memcpy(&self->enterCodeLabel[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            memcpy(&self->enterCodeSlider[1]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            MatrixRotateZF(&self->enterCodeSlider[0]->renderMatrix, DegreesToRad(180));
            MatrixMultiplyF(&self->enterCodeSlider[0]->renderMatrix, &self->renderMatrix);

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                if (self->state == MULTIPLAYERSCREEN_STATE_EXIT) {
                    if (skipStartMenu) {
                        StopMusic(true);
                        // is there a better way of removing the pop up message? :(
                        if (self->dialog) {
                            RemoveNativeObject(self->dialog);
                            RemoveNativeObject(self->label);
                            RemoveNativeObject(self->label);
                        }
                        SetGlobalVariableByName("options.stageSelectFlag", 0);
                        activeStageList   = 0;
                        stageMode         = STAGEMODE_LOAD;
                        Engine.gameMode   = ENGINE_MAINGAME;
                        stageListPosition = 0;
                    }
                    else
                        Engine.gameMode = ENGINE_RESETGAME;
                }
                else {
                    if (self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state == PUSHBUTTON_STATE_UNSELECTED) { /// hhhhhhack
                        SetRoomCode(self->roomCode);
                        ServerPacket send;
                        send.header = CL_JOIN;
                        vsPlayerID  = 1; // we are.... Little Guy

                        SendServerPacket(send, true);
                    }
                }
                MultiplayerScreen_Destroy(self);
                MatrixScaleXYZF(&self->renderMatrix, Engine.windowScale, Engine.windowScale, 1.0);
                MatrixTranslateXYZF(&self->matrixTemp, 0.0, 0.0, 160.0);
                MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
                SetRenderMatrix(&self->renderMatrix);

                RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, 255);
                return;
            }
            break;
        }
        case MULTIPLAYERSCREEN_STATE_FLIP: { // panel flip
            if (self->flipDir) {
                self->rotationY += (10.0 * Engine.deltaTime);
                if (self->rotationY > (M_PI_2)) {
                    self->state     = self->nextState;
                    self->rotationY = 0.0;
                }
                else if (self->rotationY > M_PI) {
                    self->stateDraw     = self->nextStateDraw;
                    self->nextStateDraw = MULTIPLAYERSCREEN_STATEDRAW_NONE;
                }
            }
            else {
                self->rotationY -= (10.0 * Engine.deltaTime);
                if (self->rotationY < -(M_PI_2)) {
                    self->state     = self->nextState;
                    self->rotationY = 0.0;
                }
                else if (self->rotationY < -M_PI) {
                    self->stateDraw     = self->nextStateDraw;
                    self->nextStateDraw = MULTIPLAYERSCREEN_STATEDRAW_NONE;
                }
            }
            NewRenderState();
            MatrixRotateYF(&self->renderMatrix, self->rotationY);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);

            for (int i = 0; i < 3; ++i) memcpy(&self->codeLabel[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < MULTIPLAYERSCREEN_BUTTON_COUNT; ++i) memcpy(&self->buttons[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 8; ++i) memcpy(&self->enterCodeLabel[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            memcpy(&self->enterCodeSlider[1]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            MatrixRotateZF(&self->enterCodeSlider[0]->renderMatrix, DegreesToRad(180));
            MatrixMultiplyF(&self->enterCodeSlider[0]->renderMatrix, &self->renderMatrix);

            break;
        }
        case MULTIPLAYERSCREEN_STATE_HOSTSCR: {
            CheckKeyDown(&inputDown);
            CheckKeyPress(&inputPress);
            SetRenderMatrix(&self->renderMatrix);

            if (!self->roomCode) {
                int code = GetRoomCode();
                if (code) {
                    char buffer[0x30];
                    int code = GetRoomCode();
                    sprintf(buffer, "%08X", code);
                    SetStringToFont8(self->codeLabel[1]->text, buffer, self->codeLabel[1]->fontID);
                    self->codeLabel[1]->alignPtr(self->codeLabel[1], ALIGN_CENTER);
                    self->roomCode = code;
                }
            }
            else {
                // listen.
                if (Engine.gameMode != ENGINE_WAIT2PVS) {
                    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state = PUSHBUTTON_STATE_UNSELECTED; // HAck
                    self->selectedButton                                    = MULTIPLAYERSCREEN_BUTTON_JOINROOM;
                    self->state                                             = MULTIPLAYERSCREEN_STATE_ACTION;
                }
            }

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (inputPress.A || inputPress.start) {
                        PlaySfxByName("Menu Select", false);
                        char buffer[0x30];
                        int code = GetRoomCode();
                        sprintf(buffer, "%08X", code);
                        SDL_SetClipboardText(buffer);
                    }
                    if (inputPress.B) {
                        self->dialog = CREATE_ENTITY(DialogPanel);
                        SetStringToFont8(self->dialog->text,
                                         "Are you sure you want to exit?\rThis will close the room,\rand you will return to the main menu.", 2);
                        self->state = MULTIPLAYERSCREEN_STATE_DIALOGWAIT;
                        PlaySfxByName("Resume", false);
                    }
                }
            }
            else {
                if (touches > 0) {
                    self->backPressed                                   = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->state = CheckTouchRect(0, -64.0f,
                                                                                         ((64.0 * self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->scale)
                                                                                          + self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->textWidth)
                                                                                             * 0.75,
                                                                                         12.0)
                                                                          >= 0;
                }
                else {
                    self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->state |= inputPress.A || inputPress.start;
                    if (self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->state) {
                        self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->state = PUSHBUTTON_STATE_UNSELECTED;
                        if (inputPress.A || inputPress.start)
                            usePhysicalControls = true;
                        PlaySfxByName("Menu Select", false);
                        char buffer[0x30];
                        int code = GetRoomCode();
                        sprintf(buffer, "%08X", code);
                        SDL_SetClipboardText(buffer);
                    }
                    if (inputPress.B || self->backPressed) {
                        self->backPressed = false;
                        self->dialog      = CREATE_ENTITY(DialogPanel);
                        SetStringToFont8(self->dialog->text,
                                         "Are you sure you want to exit?\rThis will close the room,\rand you will return to the main menu.",
                                         FONT_TEXT);
                        self->state = MULTIPLAYERSCREEN_STATE_DIALOGWAIT;
                        PlaySfxByName("Resume", false);
                    }
                }
            }
            break;
        }
        case MULTIPLAYERSCREEN_STATE_JOINSCR: {
            CheckKeyDown(&inputDown);
            CheckKeyPress(&inputPress);
            SetRenderMatrix(&self->renderMatrix);

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (inputPress.left) {
                        PlaySfxByName("Menu Move", false);
                        self->selectedButton--;
                        if (self->selectedButton < 3)
                            self->selectedButton = 12;
                    }
                    else if (inputPress.right) {
                        PlaySfxByName("Menu Move", false);
                        self->selectedButton++;
                        if (self->selectedButton > 12)
                            self->selectedButton = 3;
                    }

                    if ((self->selectedButton != MULTIPLAYERSCREEN_BUTTON_JOINROOM && self->selectedButton != MULTIPLAYERSCREEN_BUTTON_PASTE)
                        && (inputPress.up || inputPress.down)) {
                        union {
                            int val;
                            byte bytes[4];
                        } u;
                        u.val         = self->roomCode;
                        int n         = 7 - (self->selectedButton - 5);
                        int nybbles[] = { u.bytes[n >> 1] & 0xF, ((u.bytes[n >> 1] & 0xF0) >> 4) & 0xF };

                        if (inputPress.up) {
                            PlaySfxByName("Menu Move", false);
                            nybbles[n & 1] = (nybbles[n & 1] + 1) & 0xF;
                        }
                        else if (inputPress.down) {
                            PlaySfxByName("Menu Move", false);
                            nybbles[n & 1] = (nybbles[n & 1] - 1) & 0xF;
                        }

                        u.bytes[n >> 1] = (nybbles[1] << 4) | (nybbles[0] & 0xF);
                        self->roomCode  = u.val;

                        MultiplayerScreen_DrawJoinCode(self, n);
                    }

                    for (int i = 0; i < 8; ++i) self->enterCodeLabel[i]->useColors = false;
                    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state = PUSHBUTTON_STATE_UNSELECTED;
                    self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->state    = PUSHBUTTON_STATE_UNSELECTED;
                    self->enterCodeSlider[0]->alpha                         = 0;
                    self->enterCodeSlider[1]->alpha                         = 0;

                    if (self->selectedButton == MULTIPLAYERSCREEN_BUTTON_JOINROOM)
                        self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state = PUSHBUTTON_STATE_SELECTED;
                    else if (self->selectedButton == MULTIPLAYERSCREEN_BUTTON_PASTE)
                        self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->state = PUSHBUTTON_STATE_SELECTED;
                    else if (self->selectedButton > 4) {
                        self->enterCodeSlider[0]->x     = self->enterCodeLabel[7 - (self->selectedButton - 5)]->x - 2.5;
                        self->enterCodeSlider[1]->x     = -(self->enterCodeLabel[7 - (self->selectedButton - 5)]->x - 2.5);
                        self->enterCodeSlider[0]->alpha = 0x100;
                        self->enterCodeSlider[1]->alpha = 0x100;

                        self->enterCodeLabel[self->selectedButton - 5]->useColors = true;
                    }

                    if (inputPress.start || inputPress.A) {
                        if (self->selectedButton == MULTIPLAYERSCREEN_BUTTON_JOINROOM) {
                            PlaySfxByName("Menu Select", false);
                            self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state = PUSHBUTTON_STATE_UNSELECTED;
                            self->selectedButton                                    = MULTIPLAYERSCREEN_BUTTON_JOINROOM;
                            self->state                                             = MULTIPLAYERSCREEN_STATE_ACTION;
                        }
                        else if (self->selectedButton == MULTIPLAYERSCREEN_BUTTON_PASTE) {
                            self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->state = PUSHBUTTON_STATE_FLASHING;
                            char buf[0x30];
                            char *txt = SDL_GetClipboardText(); // easier bc we must SDL free after
                            if (StrLength(txt) && StrLength(txt) < 0x30 - 2) {
                                StrCopy(buf, "0x");
                                StrAdd(buf, txt);
                                int before = self->roomCode;
                                if (ConvertStringToInteger(buf, &self->roomCode)) {
                                    MultiplayerScreen_DrawJoinCode(self, 0);
                                    self->enterCodeLabel[0]->useColors = false;
                                    self->selectedButton               = MULTIPLAYERSCREEN_BUTTON_JOINROOM;
                                    if (Engine.gameType == GAME_SONIC1) //??
                                        PlaySfxByName("Lamp Post", false);
                                    else
                                        PlaySfxByName("Star Post", false);
                                }
                                else {
                                    self->roomCode = before;
                                    PlaySfxByName("Hurt", false);
                                }
                            }
                            else
                                PlaySfxByName("Hurt", false);
                            SDL_free(txt);
                        }
                    }
                    else if (inputPress.B) {
                        PlaySfxByName("Menu Back", false);
                        self->state         = MULTIPLAYERSCREEN_STATE_FLIP;
                        self->nextState     = MULTIPLAYERSCREEN_STATE_MAIN;
                        self->nextStateDraw = MULTIPLAYERSCREEN_STATEDRAW_MAIN;
                    }
                }
            }
            else {
                if (touches > 0) {
                    float w = self->enterCodeLabel[1]->x - self->enterCodeLabel[0]->x;
                    for (int i = 0; i < 8; ++i) {
                        if (CheckTouchRect(self->enterCodeLabel[i]->x, 16.0f, w / 2, 16.0) >= 0)
                            self->touchedUpID = i;
                        if (CheckTouchRect(self->enterCodeLabel[i]->x, -16.0f, w / 2, 16.0) >= 0)
                            self->touchedDownID = i;
                    }

                    for (int i = 0; i < 8; ++i) self->enterCodeLabel[i]->useColors = false;

                    int id = self->touchedDownID;
                    if (self->touchedUpID >= 0)
                        id = self->touchedUpID;
                    if (id >= 0) {
                        self->selectedButton            = id + 5;
                        self->enterCodeSlider[0]->x     = self->enterCodeLabel[7 - id]->x - 2.5;
                        self->enterCodeSlider[1]->x     = -(self->enterCodeLabel[7 - id]->x - 2.5);
                        self->enterCodeSlider[0]->alpha = 0x100;
                        self->enterCodeSlider[1]->alpha = 0x100;

                        self->enterCodeLabel[id]->useColors = true;
                    }

                    self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state =
                        CheckTouchRect(-56.0f, -64.0f,
                                       ((64.0 * self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->scale)
                                        + self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->textWidth)
                                           * 0.75,
                                       12.0)
                        >= 0;
                    self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->state =
                        CheckTouchRect(
                            64.0f, -64.0f,
                            ((64.0 * self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->scale) + self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->textWidth)
                                * 0.75,
                            12.0)
                        >= 0;

                    self->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (inputDown.left || inputDown.right) {
                        usePhysicalControls = true;
                    }
                }
                else {
                    if (self->touchedUpID >= 0 || self->touchedDownID >= 0) {
                        int id = self->touchedDownID;
                        if (self->touchedUpID >= 0)
                            id = self->touchedUpID;

                        union {
                            int val;
                            byte bytes[4];
                        } u;
                        u.val         = self->roomCode;
                        int n         = 7 - id;
                        int nybbles[] = { u.bytes[n >> 1] & 0xF, ((u.bytes[n >> 1] & 0xF0) >> 4) & 0xF };

                        if (self->touchedUpID >= 0) {
                            PlaySfxByName("Menu Move", false);
                            nybbles[n & 1] = (nybbles[n & 1] + 1) & 0xF;
                        }
                        else if (self->touchedDownID >= 0) {
                            PlaySfxByName("Menu Move", false);
                            nybbles[n & 1] = (nybbles[n & 1] - 1) & 0xF;
                        }

                        u.bytes[n >> 1] = (nybbles[1] << 4) | (nybbles[0] & 0xF);
                        self->roomCode  = u.val;

                        for (int i = 0; i < 8; i += 2) {
                            int n         = 7 - i;
                            int nybbles[] = { u.bytes[n >> 1] & 0xF, ((u.bytes[n >> 1] & 0xF0) >> 4) & 0xF };

                            self->enterCodeLabel[i + 0]->alpha = 0x100;
                            self->enterCodeLabel[i + 1]->alpha = 0x100;

                            self->enterCodeLabel[i + 0]->useColors = false;
                            self->enterCodeLabel[i + 1]->useColors = false;

                            char codeBuf[0x10];
                            sprintf(codeBuf, "%X", nybbles[1]);
                            SetStringToFont8(self->enterCodeLabel[i + 0]->text, codeBuf, self->enterCodeLabel[i + 0]->fontID);
                            self->enterCodeLabel[i + 0]->alignPtr(self->enterCodeLabel[i + 0], ALIGN_CENTER);

                            sprintf(codeBuf, "%X", nybbles[0]);
                            SetStringToFont8(self->enterCodeLabel[i + 1]->text, codeBuf, self->enterCodeLabel[i + 1]->fontID);
                            self->enterCodeLabel[i + 1]->alignPtr(self->enterCodeLabel[i + 1], ALIGN_CENTER);
                        }
                        self->enterCodeLabel[id]->useColors = true;

                        self->touchedUpID   = -1;
                        self->touchedDownID = -1;
                    }

                    if (self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state == PUSHBUTTON_STATE_SELECTED) {
                        PlaySfxByName("Menu Select", false);
                        self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state = PUSHBUTTON_STATE_UNSELECTED;
                        self->selectedButton                                    = MULTIPLAYERSCREEN_BUTTON_JOINROOM;
                        self->state                                             = MULTIPLAYERSCREEN_STATE_ACTION;
                    }
                    else if (self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->state == PUSHBUTTON_STATE_SELECTED) {
                        self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->state = PUSHBUTTON_STATE_FLASHING;
                        char buf[0x30];
                        char *txt = SDL_GetClipboardText(); // easier bc we must SDL free after
                        if (StrLength(txt) && StrLength(txt) < 0x30 - 2) {
                            StrCopy(buf, "0x");
                            StrAdd(buf, txt);
                            int before = self->roomCode;
                            if (ConvertStringToInteger(buf, &self->roomCode)) {
                                MultiplayerScreen_DrawJoinCode(self, 0);
                                self->enterCodeLabel[0]->useColors = false;
                                self->selectedButton               = 5;
                                if (Engine.gameType == GAME_SONIC1)
                                    PlaySfxByName("Lamp Post", false);
                                else
                                    PlaySfxByName("Star Post", false);
                            }
                            else {
                                self->roomCode = before;
                                PlaySfxByName("Hurt", false);
                            }
                        }
                        else
                            PlaySfxByName("Hurt", false);
                        SDL_free(txt);
                    }

                    if (inputPress.B || self->backPressed) {
                        PlaySfxByName("Menu Back", false);
                        self->backPressed   = false;
                        self->state         = MULTIPLAYERSCREEN_STATE_FLIP;
                        self->nextState     = MULTIPLAYERSCREEN_STATE_MAIN;
                        self->nextStateDraw = MULTIPLAYERSCREEN_STATEDRAW_MAIN;
                    }
                    else {
                        if (inputDown.left) {
                            self->selectedButton = 5;
                            usePhysicalControls  = true;
                        }
                        if (inputDown.right) {
                            self->selectedButton = 12;
                            usePhysicalControls  = true;
                        }
                    }
                }
            }
            break;
        }
        case MULTIPLAYERSCREEN_STATE_DIALOGWAIT: {
            SetRenderMatrix(&self->renderMatrix);
            if (self->dialog->selection == DLG_NO || self->dialog->selection == DLG_OK) {
                self->state = MULTIPLAYERSCREEN_STATE_HOSTSCR;
            }
            else if (self->dialog->selection == DLG_YES) {
                PlaySfxByName("Menu Back", false);
                self->backPressed             = false;
                self->state                   = MULTIPLAYERSCREEN_STATE_EXIT;
                self->timer                   = 0;
                NativeEntity_FadeScreen *fade = CREATE_ENTITY(FadeScreen);
                fade->delay                   = 1.1f;
                fade->state                   = FADESCREEN_STATE_FADEOUT;
                DisconnectNetwork();
                InitNetwork();
            }
            break;
        }
    }

    switch (self->stateDraw) {
        default: break;
        case MULTIPLAYERSCREEN_STATEDRAW_MAIN:
            for (int i = 0; i < MULTIPLAYERSCREEN_BUTTON_COUNT; ++i) self->buttons[i]->alpha = 0;
            self->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->alpha = 0x100;
            self->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->alpha = 0x100;

            for (int i = 0; i < 3; ++i) self->codeLabel[i]->alpha = 0;
            for (int i = 0; i < 8; ++i) self->enterCodeLabel[i]->alpha = 0;
            for (int i = 0; i < 2; ++i) self->enterCodeSlider[i]->alpha = 0;

            self->selectedButton = MULTIPLAYERSCREEN_BUTTON_HOST;
            break;
        case MULTIPLAYERSCREEN_STATEDRAW_HOST: {
            for (int i = 0; i < MULTIPLAYERSCREEN_BUTTON_COUNT; ++i) self->buttons[i]->alpha = 0;
            for (int i = 0; i < 3; ++i) self->codeLabel[i]->alpha = 0x100;
            for (int i = 0; i < 8; ++i) self->enterCodeLabel[i]->alpha = 0;
            for (int i = 0; i < 2; ++i) self->enterCodeSlider[i]->alpha = 0;

            self->selectedButton                                = MULTIPLAYERSCREEN_BUTTON_COPY;
            self->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->alpha = 0x100;

            self->roomCode = 0;
            SetStringToFont8(self->codeLabel[1]->text, "FETCHING...", self->codeLabel[1]->fontID);
            self->codeLabel[1]->alignPtr(self->codeLabel[1], ALIGN_CENTER);

            ServerPacket send;
            send.header = CL_REQUEST_CODE;
            // send over a preferred roomcode style
            if (!vsGameLength)
                vsGameLength = 4;
            if (!vsItemMode)
                vsItemMode = 1;
            send.data.multiData.type    = 0x00000FF0;
            send.data.multiData.data[0] = (vsGameLength << 4) | (vsItemMode << 8);
            vsPlayerID                  = 0; // we are... Big Host

            SendServerPacket(send, true);
            break;
        }
        case MULTIPLAYERSCREEN_STATEDRAW_JOIN: {
            for (int i = 0; i < MULTIPLAYERSCREEN_BUTTON_COUNT; ++i) self->buttons[i]->alpha = 0;
            self->selectedButton = MULTIPLAYERSCREEN_BUTTON_COUNT;
            self->touchedUpID    = -1;
            self->touchedDownID  = -1;

            for (int i = 0; i < 3; ++i) self->codeLabel[i]->alpha = 0;

            self->roomCode = 0;
            for (int i = 0; i < 8; ++i) {
                self->enterCodeLabel[i]->alpha     = 0x100;
                self->enterCodeLabel[i]->useColors = false;

                char codeBuf[0x10];
                sprintf(codeBuf, "%X", 0);
                SetStringToFont8(self->enterCodeLabel[i]->text, codeBuf, self->enterCodeLabel[i]->fontID);
                self->enterCodeLabel[i]->alignPtr(self->enterCodeLabel[i], ALIGN_CENTER);
            }
            self->enterCodeLabel[0]->useColors                      = true;
            self->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->alpha = 0x100;
            self->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->alpha    = 0x100;

            break;
        }
    }
    self->stateDraw = MULTIPLAYERSCREEN_STATEDRAW_NONE;

    SetRenderBlendMode(RENDER_BLEND_ALPHA);
    RenderMesh(self->meshPanel, MESH_COLORS, false);
    SetRenderBlendMode(RENDER_BLEND_ALPHA);
    NewRenderState();
    SetRenderMatrix(NULL);

    if (self->backPressed)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, self->arrowAlpha, self->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, self->arrowAlpha, self->textureArrows);
}
#endif
