#include "RetroEngine.hpp"

#if RETRO_USE_NETWORKING

void MultiplayerScreen_Create(void *objPtr)
{
    RSDK_THIS(MultiplayerScreen);

    entity->state = MULTIPLAYERSCREEN_STATE_ENTER;

    entity->label                  = CREATE_ENTITY(TextLabel);
    entity->label->useRenderMatrix = true;
    entity->label->fontID          = FONT_HEADING;
    entity->label->scale           = 0.2;
    entity->label->alpha           = 256;
    entity->label->x               = -144.0;
    entity->label->y               = 100.0;
    entity->label->z               = 16.0;
    entity->label->state           = TEXTLABEL_STATE_IDLE;
    SetStringToFont8(entity->label->text, "MULTIPLAYER", FONT_HEADING);

    entity->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", -2);
    SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 0xC0);
    entity->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);

    entity->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]                   = CREATE_ENTITY(PushButton);
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->useRenderMatrix  = true;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->x                = 0.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->y                = 20.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->z                = 0.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->scale            = 0.25;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->bgColour         = 0x00A048;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->bgColourSelected = 0x00C060;
    SetStringToFont8(entity->buttons[0]->text, "HOST", FONT_LABEL);

    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]                   = CREATE_ENTITY(PushButton);
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->useRenderMatrix  = true;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->x                = 0.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->y                = -20.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->z                = 0.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->scale            = 0.25;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->bgColour         = 0x00A048;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->bgColourSelected = 0x00C060;
    SetStringToFont8(entity->buttons[1]->text, "JOIN", FONT_LABEL);

    entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]                   = CREATE_ENTITY(PushButton);
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->useRenderMatrix  = true;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->x                = 0.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->y                = -56.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->z                = 0.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->scale            = 0.175;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->bgColour         = 0x00A048;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->bgColourSelected = 0x00C060;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->alpha            = 0;
    SetStringToFont8(entity->buttons[2]->text, "COPY", FONT_LABEL);

    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]                   = CREATE_ENTITY(PushButton);
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->useRenderMatrix  = true;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->x                = -56.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->y                = -56.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->z                = 0.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->scale            = 0.175;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->bgColour         = 0x00A048;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->bgColourSelected = 0x00C060;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->alpha            = 0;
    SetStringToFont8(entity->buttons[3]->text, "JOIN ROOM", FONT_LABEL);

    entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]                   = CREATE_ENTITY(PushButton);
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->useRenderMatrix  = true;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->x                = 64.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->y                = -56.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->z                = 0.0;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->scale            = 0.175;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->bgColour         = 0x00A048;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->bgColourSelected = 0x00C060;
    entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->alpha            = 0;
    SetStringToFont8(entity->buttons[4]->text, "PASTE", FONT_LABEL);

    for (int i = 0; i < 3; ++i) {
        entity->codeLabel[i]                  = CREATE_ENTITY(TextLabel);
        entity->codeLabel[i]->useRenderMatrix = true;
        entity->codeLabel[i]->fontID          = FONT_LABEL;
        entity->codeLabel[i]->scale           = 0.15;
        entity->codeLabel[i]->alpha           = 0;
        entity->codeLabel[i]->x               = 0;
        entity->codeLabel[i]->y               = 0;
        entity->codeLabel[i]->z               = 16.0;
        entity->codeLabel[i]->state           = TEXTLABEL_STATE_IDLE;
    }

    SetStringToFont8(entity->codeLabel[0]->text, "ROOM CODE", entity->codeLabel[0]->fontID);
    entity->codeLabel[0]->alignPtr(entity->codeLabel[0], ALIGN_CENTER);

    SetStringToFont8(entity->codeLabel[1]->text, "UNKNOWN", entity->codeLabel[1]->fontID);
    entity->codeLabel[1]->alignPtr(entity->codeLabel[1], ALIGN_CENTER);
    entity->codeLabel[1]->y -= 20.0f;

    entity->codeLabel[2]->y     = 48.0f;
    entity->codeLabel[2]->scale = 0.20f;
    SetStringToFont8(entity->codeLabel[2]->text, "WAITING FOR 2P...", entity->codeLabel[2]->fontID);
    entity->codeLabel[2]->alignPtr(entity->codeLabel[2], ALIGN_CENTER);

    char codeBuf[0x8];
    sprintf(codeBuf, "%X", 0);

    for (int i = 0; i < 8; ++i) {
        entity->enterCodeLabel[i]                  = CREATE_ENTITY(TextLabel);
        entity->enterCodeLabel[i]->useRenderMatrix = true;
        entity->enterCodeLabel[i]->fontID          = FONT_LABEL;
        entity->enterCodeLabel[i]->scale           = 0.25;
        entity->enterCodeLabel[i]->alpha           = 0;
        entity->enterCodeLabel[i]->x               = -102.0f + (i * 27.2) + 8.0f;
        entity->enterCodeLabel[i]->y               = 0;
        entity->enterCodeLabel[i]->z               = 16.0;
        entity->enterCodeLabel[i]->state           = TEXTLABEL_STATE_IDLE;

        entity->enterCodeLabel[i]->r = 0xFF;
        entity->enterCodeLabel[i]->g = 0xFF;
        entity->enterCodeLabel[i]->b = 0x00;

        SetStringToFont8(entity->enterCodeLabel[i]->text, codeBuf, entity->enterCodeLabel[i]->fontID);
        entity->enterCodeLabel[i]->alignPtr(entity->enterCodeLabel[i], ALIGN_CENTER);
    }
    for (int i = 0; i < 2; ++i) {
        entity->enterCodeSlider[i]                  = CREATE_ENTITY(TextLabel);
        entity->enterCodeSlider[i]->useRenderMatrix = true;
        entity->enterCodeSlider[i]->fontID          = FONT_LABEL;
        entity->enterCodeSlider[i]->scale           = 0.175;
        entity->enterCodeSlider[i]->alpha           = 0;
        // entity->enterCodeSlider[i]->textX           = -102.0f + (i * 27.2) + 8.0f;
        entity->enterCodeSlider[i]->y     = (i ? -23.0 : -40.0);
        entity->enterCodeSlider[i]->z     = 16.0;
        entity->enterCodeSlider[i]->state = TEXTLABEL_STATE_IDLE;
        SetStringToFont8(entity->enterCodeSlider[i]->text, "V", entity->enterCodeSlider[i]->fontID);
        entity->enterCodeSlider[i]->alignPtr(entity->enterCodeSlider[i], ALIGN_CENTER);
    }
}

void MultiplayerScreen_DrawJoinCode(void *objPtr, int v)
{
    RSDK_THIS(MultiplayerScreen);
    union {
        uint val;
        byte bytes[4];
    } u;
    u.val = entity->roomCode;

    for (int i = 0; i < 8; i += 2) {
        int n         = 7 - i;
        int nybbles[] = { u.bytes[n >> 1] & 0xF, ((u.bytes[n >> 1] & 0xF0) >> 4) & 0xF };

        entity->enterCodeLabel[i + 0]->alpha = 0x100;
        entity->enterCodeLabel[i + 1]->alpha = 0x100;

        entity->enterCodeLabel[i + 0]->useColours = false;
        entity->enterCodeLabel[i + 1]->useColours = false;

        char codeBuf[0x10];
        sprintf(codeBuf, "%X", nybbles[1]);
        SetStringToFont8(entity->enterCodeLabel[i + 0]->text, codeBuf, entity->enterCodeLabel[i + 0]->fontID);
        entity->enterCodeLabel[i + 0]->alignPtr(entity->enterCodeLabel[i + 0], ALIGN_CENTER);

        sprintf(codeBuf, "%X", nybbles[0]);
        SetStringToFont8(entity->enterCodeLabel[i + 1]->text, codeBuf, entity->enterCodeLabel[i + 1]->fontID);
        entity->enterCodeLabel[i + 1]->alignPtr(entity->enterCodeLabel[i + 1], ALIGN_CENTER);
    }
    entity->enterCodeLabel[v]->useColours = true;
}

void MultiplayerScreen_Main(void *objPtr)
{
    RSDK_THIS(MultiplayerScreen);

    switch (entity->state) {
        case MULTIPLAYERSCREEN_STATE_ENTER: {
            if (entity->arrowAlpha < 0x100)
                entity->arrowAlpha += 8;

            entity->scale = fminf(entity->scale + ((1.05 - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrixTemp);
            SetRenderMatrix(&entity->renderMatrix);

            memcpy(&entity->label->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 3; ++i) memcpy(&entity->codeLabel[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < MULTIPLAYERSCREEN_BUTTON_COUNT; ++i)
                memcpy(&entity->buttons[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 8; ++i) memcpy(&entity->enterCodeLabel[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            memcpy(&entity->enterCodeSlider[1]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            matrixRotateZF(&entity->enterCodeSlider[0]->renderMatrix, DegreesToRad(180));
            matrixMultiplyF(&entity->enterCodeSlider[0]->renderMatrix, &entity->renderMatrix);

            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                entity->timer = 0.0;
                entity->state = MULTIPLAYERSCREEN_STATE_MAIN;
            }
            break;
        }
        case MULTIPLAYERSCREEN_STATE_MAIN: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->renderMatrix);

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.up) {
                        PlaySfxByName("Menu Move", false);
                        entity->selectedButton--;
                        if (entity->selectedButton < MULTIPLAYERSCREEN_BUTTON_HOST)
                            entity->selectedButton = MULTIPLAYERSCREEN_BUTTON_JOIN;
                    }
                    else if (keyPress.down) {
                        PlaySfxByName("Menu Move", false);
                        entity->selectedButton++;
                        if (entity->selectedButton > MULTIPLAYERSCREEN_BUTTON_JOIN)
                            entity->selectedButton = MULTIPLAYERSCREEN_BUTTON_HOST;
                    }

                    entity->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->state = PUSHBUTTON_STATE_UNSELECTED;
                    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->state = PUSHBUTTON_STATE_UNSELECTED;
                    entity->buttons[entity->selectedButton]->state        = PUSHBUTTON_STATE_SELECTED;

                    if (keyPress.start || keyPress.A) {
                        PlaySfxByName("Menu Select", false);
                        entity->buttons[entity->selectedButton]->state = PUSHBUTTON_STATE_FLASHING;
                        entity->state                                  = MULTIPLAYERSCREEN_STATE_ACTION;
                    }
                    else if (keyPress.B) {
                        PlaySfxByName("Menu Back", false);
                        entity->backPressed = false;
                        entity->state       = MULTIPLAYERSCREEN_STATE_EXIT;
                    }
                }
            }
            else {
                if (touches > 0) {
                    for (int i = 0; i < 2; ++i) {
                        entity->buttons[i]->state =
                            CheckTouchRect(0, 20 - (i * 40), ((64.0 * entity->buttons[i]->scale) + entity->buttons[i]->textWidth) * 0.75, 20.4) >= 0;
                    }
                    entity->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (entity->state == MULTIPLAYERSCREEN_STATE_MAIN) {
                        if (keyDown.left) {
                            entity->selectedButton = MULTIPLAYERSCREEN_BUTTON_JOIN;
                            usePhysicalControls    = true;
                        }
                        if (keyDown.right) {
                            entity->selectedButton = MULTIPLAYERSCREEN_BUTTON_HOST;
                            usePhysicalControls    = true;
                        }
                    }
                }
                else {
                    for (int i = 0; i < 2; ++i) {
                        if (entity->buttons[i]->state == PUSHBUTTON_STATE_SELECTED) {
                            PlaySfxByName("Menu Select", false);
                            entity->buttons[i]->state = PUSHBUTTON_STATE_FLASHING;
                            entity->selectedButton    = i;
                            entity->state             = MULTIPLAYERSCREEN_STATE_ACTION;
                            break;
                        }
                    }

                    if (keyPress.B || entity->backPressed) {
                        PlaySfxByName("Menu Back", false);
                        entity->backPressed           = false;
                        entity->state                 = MULTIPLAYERSCREEN_STATE_EXIT;
                        NativeEntity_FadeScreen *fade = CREATE_ENTITY(FadeScreen); // fade back to menu
                        fade->fadeSpeed               = 1.0;
                        fade->timeLimit               = 0.5;
                        fade->state                   = FADESCREEN_STATE_FADEIN_CLEAR;
                    }
                    else {
                        if (entity->state == MULTIPLAYERSCREEN_STATE_MAIN) {
                            if (keyDown.down) {
                                entity->selectedButton = MULTIPLAYERSCREEN_BUTTON_JOIN;
                                usePhysicalControls    = true;
                            }
                            if (keyDown.up) {
                                entity->selectedButton = MULTIPLAYERSCREEN_BUTTON_HOST;
                                usePhysicalControls    = true;
                            }
                        }
                    }
                }
            }
            break;
        }
        case MULTIPLAYERSCREEN_STATE_ACTION: { // action
            CheckKeyDown(&keyDown);
            SetRenderMatrix(&entity->renderMatrix);

            if (entity->buttons[entity->selectedButton]->state == PUSHBUTTON_STATE_UNSELECTED) {
                entity->state = MULTIPLAYERSCREEN_STATE_MAIN;
                switch (entity->selectedButton) {
                    default: break;
                    case MULTIPLAYERSCREEN_BUTTON_HOST:
                        entity->state         = MULTIPLAYERSCREEN_STATE_FLIP;
                        entity->nextState     = MULTIPLAYERSCREEN_STATE_HOSTSCR;
                        entity->nextStateDraw = MULTIPLAYERSCREEN_STATEDRAW_HOST;
                        break;
                    case MULTIPLAYERSCREEN_BUTTON_JOIN:
                        entity->state         = MULTIPLAYERSCREEN_STATE_FLIP;
                        entity->nextState     = MULTIPLAYERSCREEN_STATE_JOINSCR;
                        entity->nextStateDraw = MULTIPLAYERSCREEN_STATEDRAW_JOIN;
                        break;
                    case MULTIPLAYERSCREEN_BUTTON_JOINROOM: {
                        if (entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state == PUSHBUTTON_STATE_UNSELECTED) { /// hhhhhhack
                            setRoomCode(entity->roomCode);
                            ServerPacket send;
                            send.header                 = 0;
                            send.data.multiData.data[1] = (int)strlen(networkGame);
                            StrCopy((char *)&send.data.multiData.data[2], networkGame);

                            sendServerPacket(send);
                        }
                        entity->state = MULTIPLAYERSCREEN_STATE_STARTGAME;

                        NativeEntity_FadeScreen *fade = CREATE_ENTITY(FadeScreen); // TODO: get rdc to figure out what's going on here
                        fade->state                   = FADESCREEN_STATE_GAMEFADEOUT;
                        fade->fadeSpeed               = 1.0f;
                        fade->timeLimit               = 0.5f;
                        break;
                    }
                }
            }
            break;
        }
        case MULTIPLAYERSCREEN_STATE_STARTGAME:
        case MULTIPLAYERSCREEN_STATE_EXIT: {
            if (entity->arrowAlpha > 0)
                entity->arrowAlpha -= 8;

            if (entity->timer < 0.2)
                entity->scale = fmaxf(entity->scale + ((1.5f - entity->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);
            else
                entity->scale = fmaxf(entity->scale + ((-1.0f - entity->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrixTemp);
            SetRenderMatrix(&entity->renderMatrix);

            memcpy(&entity->label->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 3; ++i) memcpy(&entity->codeLabel[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < MULTIPLAYERSCREEN_BUTTON_COUNT; ++i)
                memcpy(&entity->buttons[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 8; ++i) memcpy(&entity->enterCodeLabel[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            memcpy(&entity->enterCodeSlider[1]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            matrixRotateZF(&entity->enterCodeSlider[0]->renderMatrix, DegreesToRad(180));
            matrixMultiplyF(&entity->enterCodeSlider[0]->renderMatrix, &entity->renderMatrix);

            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                RemoveNativeObject(entity->label);
                if (entity->dialog)
                    RemoveNativeObject(entity->dialog);
                for (int i = 0; i < 3; ++i) RemoveNativeObject(entity->codeLabel[i]);
                for (int i = 0; i < 8; ++i) RemoveNativeObject(entity->enterCodeLabel[i]);
                for (int i = 0; i < 2; ++i) RemoveNativeObject(entity->enterCodeSlider[i]);
                for (int i = 0; i < MULTIPLAYERSCREEN_BUTTON_COUNT; ++i) RemoveNativeObject(entity->buttons[i]);
                RemoveNativeObject(entity->bg);
                RemoveNativeObject(entity);
                if (entity->state == MULTIPLAYERSCREEN_STATE_EXIT)
                    Engine.gameMode = ENGINE_RESETGAME;
                return;
            }
            break;
        }
        case MULTIPLAYERSCREEN_STATE_FLIP: { // panel flip
            if (entity->flipDir) {
                entity->rotationY += (10.0 * Engine.deltaTime);
                if (entity->rotationY > (M_PI_2)) {
                    entity->state     = entity->nextState;
                    entity->rotationY = 0.0;
                }
                else if (entity->rotationY > M_PI) {
                    entity->stateDraw     = entity->nextStateDraw;
                    entity->nextStateDraw = MULTIPLAYERSCREEN_STATEDRAW_NONE;
                }
            }
            else {
                entity->rotationY -= (10.0 * Engine.deltaTime);
                if (entity->rotationY < -(M_PI_2)) {
                    entity->state     = entity->nextState;
                    entity->rotationY = 0.0;
                }
                else if (entity->rotationY < -M_PI) {
                    entity->stateDraw     = entity->nextStateDraw;
                    entity->nextStateDraw = MULTIPLAYERSCREEN_STATEDRAW_NONE;
                }
            }
            NewRenderState();
            matrixRotateYF(&entity->renderMatrix, entity->rotationY);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrixTemp);
            SetRenderMatrix(&entity->renderMatrix);

            for (int i = 0; i < 3; ++i) memcpy(&entity->codeLabel[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < MULTIPLAYERSCREEN_BUTTON_COUNT; ++i)
                memcpy(&entity->buttons[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 8; ++i) memcpy(&entity->enterCodeLabel[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            memcpy(&entity->enterCodeSlider[1]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            matrixRotateZF(&entity->enterCodeSlider[0]->renderMatrix, DegreesToRad(180));
            matrixMultiplyF(&entity->enterCodeSlider[0]->renderMatrix, &entity->renderMatrix);

            break;
        }
        case MULTIPLAYERSCREEN_STATE_HOSTSCR: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->renderMatrix);

            if (!entity->roomCode) {
                int code = getRoomCode();
                if (code) {
                    char buffer[0x30];
                    int code = getRoomCode();
                    sprintf(buffer, "%08X", code);
                    SetStringToFont8(entity->codeLabel[1]->text, buffer, entity->codeLabel[1]->fontID);
                    entity->codeLabel[1]->alignPtr(entity->codeLabel[1], ALIGN_CENTER);
                    entity->roomCode = code;
                }
            }
            else {
                // listen.
                if (Engine.gameMode != ENGINE_WAIT2PVS) {
                    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state = PUSHBUTTON_STATE_SELECTED; // HAck
                    entity->selectedButton                                    = MULTIPLAYERSCREEN_BUTTON_JOINROOM;
                    entity->state                                             = MULTIPLAYERSCREEN_STATE_ACTION;
                }
            }

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.A || keyPress.start) {
                        PlaySfxByName("Menu Select", false);
                        char buffer[0x30];
                        int code = getRoomCode();
                        sprintf(buffer, "%08X", code);
                        SDL_SetClipboardText(buffer);
                    }
                    if (keyPress.B) {
                        entity->dialog = CREATE_ENTITY(DialogPanel);
                        SetStringToFont8(entity->dialog->text,
                                         "Are you sure you want to exit?\rThis will close the room,\rand you will return to the main menu.", 2);
                        entity->state = MULTIPLAYERSCREEN_STATE_DIALOGWAIT;
                        PlaySfxByName("Resume", false);
                    }
                }
            }
            else {
                if (touches > 0) {
                    entity->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->state =
                        CheckTouchRect(0, -64.0f,
                                       ((64.0 * entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->scale)
                                        + entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->textWidth)
                                           * 0.75,
                                       12.0)
                        >= 0;
                }
                else {
                    entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->state |= keyPress.A || keyPress.start;
                    if (entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->state) {
                        entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->state = PUSHBUTTON_STATE_UNSELECTED;
                        if (keyPress.A || keyPress.start)
                            usePhysicalControls = true;
                        PlaySfxByName("Menu Select", false);
                        char buffer[0x30];
                        int code = getRoomCode();
                        sprintf(buffer, "%08X", code);
                        SDL_SetClipboardText(buffer);
                    }
                    if (keyPress.B || entity->backPressed) {
                        entity->dialog = CREATE_ENTITY(DialogPanel);
                        SetStringToFont8(entity->dialog->text,
                                         "Are you sure you want to exit?\rThis will close the room,\rand you will return to the main menu.",
                                         FONT_TEXT);
                        entity->state = MULTIPLAYERSCREEN_STATE_DIALOGWAIT;
                        PlaySfxByName("Resume", false);
                    }
                }
            }
            break;
        }
        case MULTIPLAYERSCREEN_STATE_JOINSCR: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->renderMatrix);

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.left) {
                        PlaySfxByName("Menu Move", false);
                        entity->selectedButton--;
                        if (entity->selectedButton < 3)
                            entity->selectedButton = 12;
                    }
                    else if (keyPress.right) {
                        PlaySfxByName("Menu Move", false);
                        entity->selectedButton++;
                        if (entity->selectedButton > 12)
                            entity->selectedButton = 3;
                    }

                    if ((entity->selectedButton != MULTIPLAYERSCREEN_BUTTON_JOINROOM && entity->selectedButton != MULTIPLAYERSCREEN_BUTTON_PASTE)
                        && (keyPress.up || keyPress.down)) {
                        union {
                            int val;
                            byte bytes[4];
                        } u;
                        u.val         = entity->roomCode;
                        int n         = 7 - (entity->selectedButton - 5);
                        int nybbles[] = { u.bytes[n >> 1] & 0xF, ((u.bytes[n >> 1] & 0xF0) >> 4) & 0xF };

                        if (keyPress.up) {
                            PlaySfxByName("Menu Move", false);
                            nybbles[n & 1] = (nybbles[n & 1] + 1) & 0xF;
                        }
                        else if (keyPress.down) {
                            PlaySfxByName("Menu Move", false);
                            nybbles[n & 1] = (nybbles[n & 1] - 1) & 0xF;
                        }

                        u.bytes[n >> 1]  = (nybbles[1] << 4) | (nybbles[0] & 0xF);
                        entity->roomCode = u.val;

                        MultiplayerScreen_DrawJoinCode(entity, n);
                    }

                    for (int i = 0; i < 8; ++i) entity->enterCodeLabel[i]->useColours = false;
                    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state = PUSHBUTTON_STATE_UNSELECTED;
                    entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->state    = PUSHBUTTON_STATE_UNSELECTED;
                    entity->enterCodeSlider[0]->alpha                         = 0;
                    entity->enterCodeSlider[1]->alpha                         = 0;

                    if (entity->selectedButton == MULTIPLAYERSCREEN_BUTTON_JOINROOM)
                        entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state = PUSHBUTTON_STATE_SELECTED;
                    else if (entity->selectedButton == MULTIPLAYERSCREEN_BUTTON_PASTE)
                        entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->state = PUSHBUTTON_STATE_SELECTED;
                    else if (entity->selectedButton > 4) {
                        entity->enterCodeSlider[0]->x     = entity->enterCodeLabel[7 - (entity->selectedButton - 5)]->x - 2.5;
                        entity->enterCodeSlider[1]->x     = -(entity->enterCodeLabel[7 - (entity->selectedButton - 5)]->x - 2.5);
                        entity->enterCodeSlider[0]->alpha = 0x100;
                        entity->enterCodeSlider[1]->alpha = 0x100;

                        entity->enterCodeLabel[entity->selectedButton - 5]->useColours = true;
                    }

                    if (keyPress.start || keyPress.A) {
                        if (entity->selectedButton == MULTIPLAYERSCREEN_BUTTON_JOINROOM) {
                            PlaySfxByName("Menu Select", false);
                            entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state = PUSHBUTTON_STATE_FLASHING;
                            entity->selectedButton                                    = MULTIPLAYERSCREEN_BUTTON_JOINROOM;
                            entity->state                                             = MULTIPLAYERSCREEN_STATE_ACTION;
                        }
                        else if (entity->selectedButton == MULTIPLAYERSCREEN_BUTTON_PASTE) {
                            entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->state = PUSHBUTTON_STATE_FLASHING;
                            char buf[0x30];
                            char *txt = SDL_GetClipboardText(); // easier bc we must SDL free after
                            if (StrLength(txt) && StrLength(txt) < 0x30 - 2) {
                                StrCopy(buf, "0x");
                                StrAdd(buf, txt);
                                int before = entity->roomCode;
                                if (ConvertStringToInteger(buf, &entity->roomCode)) {
                                    MultiplayerScreen_DrawJoinCode(entity, 0);
                                    entity->enterCodeLabel[0]->useColours = false;
                                    entity->selectedButton                = MULTIPLAYERSCREEN_BUTTON_JOINROOM;
                                    if (Engine.gameType == GAME_SONIC1)
                                        PlaySfxByName("Lamp Post", false);
                                    else
                                        PlaySfxByName("Star Post", false);
                                }
                                else {
                                    entity->roomCode = before;
                                    PlaySfxByName("Hurt", false);
                                }
                            }
                            else
                                PlaySfxByName("Hurt", false);
                            SDL_free(txt);
                        }
                    }
                    else if (keyPress.B) {
                        PlaySfxByName("Menu Back", false);
                        entity->state         = MULTIPLAYERSCREEN_STATE_FLIP;
                        entity->nextState     = MULTIPLAYERSCREEN_STATE_MAIN;
                        entity->nextStateDraw = MULTIPLAYERSCREEN_STATEDRAW_MAIN;
                    }
                }
            }
            else {
                if (touches > 0) {
                    float w = entity->enterCodeLabel[1]->x - entity->enterCodeLabel[0]->x;
                    for (int i = 0; i < 8; ++i) {
                        if (CheckTouchRect(entity->enterCodeLabel[i]->x, 16.0f, w / 2, 16.0) >= 0)
                            entity->touchedUpID = i;
                        if (CheckTouchRect(entity->enterCodeLabel[i]->x, -16.0f, w / 2, 16.0) >= 0)
                            entity->touchedDownID = i;
                    }

                    for (int i = 0; i < 8; ++i) entity->enterCodeLabel[i]->useColours = false;

                    int id = entity->touchedDownID;
                    if (entity->touchedUpID >= 0)
                        id = entity->touchedUpID;
                    if (id >= 0) {
                        entity->selectedButton            = id + 5;
                        entity->enterCodeSlider[0]->x     = entity->enterCodeLabel[7 - id]->x - 2.5;
                        entity->enterCodeSlider[1]->x     = -(entity->enterCodeLabel[7 - id]->x - 2.5);
                        entity->enterCodeSlider[0]->alpha = 0x100;
                        entity->enterCodeSlider[1]->alpha = 0x100;

                        entity->enterCodeLabel[id]->useColours = true;
                    }

                    entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state =
                        CheckTouchRect(-56.0f, -64.0f,
                                       ((64.0 * entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->scale)
                                        + entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->textWidth)
                                           * 0.75,
                                       12.0)
                        >= 0;
                    entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->state =
                        CheckTouchRect(64.0f, -64.0f,
                                       ((64.0 * entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->scale)
                                        + entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->textWidth)
                                           * 0.75,
                                       12.0)
                        >= 0;

                    entity->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (keyDown.left || keyDown.right) {
                        usePhysicalControls = true;
                    }
                }
                else {
                    if (entity->touchedUpID >= 0 || entity->touchedDownID >= 0) {
                        int id = entity->touchedDownID;
                        if (entity->touchedUpID >= 0)
                            id = entity->touchedUpID;

                        union {
                            int val;
                            byte bytes[4];
                        } u;
                        u.val         = entity->roomCode;
                        int n         = 7 - id;
                        int nybbles[] = { u.bytes[n >> 1] & 0xF, ((u.bytes[n >> 1] & 0xF0) >> 4) & 0xF };

                        if (entity->touchedUpID >= 0) {
                            PlaySfxByName("Menu Move", false);
                            nybbles[n & 1] = (nybbles[n & 1] + 1) & 0xF;
                        }
                        else if (entity->touchedDownID >= 0) {
                            PlaySfxByName("Menu Move", false);
                            nybbles[n & 1] = (nybbles[n & 1] - 1) & 0xF;
                        }

                        u.bytes[n >> 1]  = (nybbles[1] << 4) | (nybbles[0] & 0xF);
                        entity->roomCode = u.val;

                        for (int i = 0; i < 8; i += 2) {
                            int n         = 7 - i;
                            int nybbles[] = { u.bytes[n >> 1] & 0xF, ((u.bytes[n >> 1] & 0xF0) >> 4) & 0xF };

                            entity->enterCodeLabel[i + 0]->alpha = 0x100;
                            entity->enterCodeLabel[i + 1]->alpha = 0x100;

                            entity->enterCodeLabel[i + 0]->useColours = false;
                            entity->enterCodeLabel[i + 1]->useColours = false;

                            char codeBuf[0x10];
                            sprintf(codeBuf, "%X", nybbles[1]);
                            SetStringToFont8(entity->enterCodeLabel[i + 0]->text, codeBuf, entity->enterCodeLabel[i + 0]->fontID);
                            entity->enterCodeLabel[i + 0]->alignPtr(entity->enterCodeLabel[i + 0], ALIGN_CENTER);

                            sprintf(codeBuf, "%X", nybbles[0]);
                            SetStringToFont8(entity->enterCodeLabel[i + 1]->text, codeBuf, entity->enterCodeLabel[i + 1]->fontID);
                            entity->enterCodeLabel[i + 1]->alignPtr(entity->enterCodeLabel[i + 1], ALIGN_CENTER);
                        }
                        entity->enterCodeLabel[id]->useColours = true;

                        entity->touchedUpID   = -1;
                        entity->touchedDownID = -1;
                    }

                    if (entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state == PUSHBUTTON_STATE_SELECTED) {
                        PlaySfxByName("Menu Select", false);
                        entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->state = PUSHBUTTON_STATE_FLASHING;
                        entity->selectedButton                                    = MULTIPLAYERSCREEN_BUTTON_JOINROOM;
                        entity->state                                             = MULTIPLAYERSCREEN_STATE_ACTION;
                    }
                    else if (entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->state == PUSHBUTTON_STATE_SELECTED) {
                        entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->state = PUSHBUTTON_STATE_FLASHING;
                        char buf[0x30];
                        char *txt = SDL_GetClipboardText(); // easier bc we must SDL free after
                        if (StrLength(txt) && StrLength(txt) < 0x30 - 2) {
                            StrCopy(buf, "0x");
                            StrAdd(buf, txt);
                            int before = entity->roomCode;
                            if (ConvertStringToInteger(buf, &entity->roomCode)) {
                                MultiplayerScreen_DrawJoinCode(entity, 0);
                                entity->enterCodeLabel[0]->useColours = false;
                                entity->selectedButton                = 5;
                                if (Engine.gameType == GAME_SONIC1)
                                    PlaySfxByName("Lamp Post", false);
                                else
                                    PlaySfxByName("Star Post", false);
                            }
                            else {
                                entity->roomCode = before;
                                PlaySfxByName("Hurt", false);
                            }
                        }
                        else
                            PlaySfxByName("Hurt", false);
                        SDL_free(txt);
                    }

                    if (keyPress.B || entity->backPressed) {
                        PlaySfxByName("Menu Back", false);
                        entity->backPressed   = false;
                        entity->state         = MULTIPLAYERSCREEN_STATE_FLIP;
                        entity->nextState     = MULTIPLAYERSCREEN_STATE_MAIN;
                        entity->nextStateDraw = MULTIPLAYERSCREEN_STATEDRAW_MAIN;
                    }
                    else {
                        if (keyDown.left) {
                            entity->selectedButton = 5;
                            usePhysicalControls    = true;
                        }
                        if (keyDown.right) {
                            entity->selectedButton = 12;
                            usePhysicalControls    = true;
                        }
                    }
                }
            }
            break;
        }
        case MULTIPLAYERSCREEN_STATE_DIALOGWAIT: {
            if (entity->dialog->selection == DLG_NO || entity->dialog->selection == DLG_OK) {
                entity->state = MULTIPLAYERSCREEN_STATE_HOSTSCR;
            }
            else if (entity->dialog->selection == DLG_YES) {
                PlaySfxByName("Menu Back", false);
                entity->backPressed = false;
                entity->state       = MULTIPLAYERSCREEN_STATE_EXIT;
                entity->timer       = 0;
                disconnectNetwork();
                initNetwork();
            }
            break;
        }
    }

    switch (entity->stateDraw) {
        default: break;
        case MULTIPLAYERSCREEN_STATEDRAW_MAIN:
            for (int i = 0; i < MULTIPLAYERSCREEN_BUTTON_COUNT; ++i) entity->buttons[i]->alpha = 0;
            entity->buttons[MULTIPLAYERSCREEN_BUTTON_HOST]->alpha = 0x100;
            entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOIN]->alpha = 0x100;

            for (int i = 0; i < 3; ++i) entity->codeLabel[i]->alpha = 0;
            for (int i = 0; i < 8; ++i) entity->enterCodeLabel[i]->alpha = 0;
            for (int i = 0; i < 2; ++i) entity->enterCodeSlider[i]->alpha = 0;

            entity->selectedButton = MULTIPLAYERSCREEN_BUTTON_HOST;
            break;
        case MULTIPLAYERSCREEN_STATEDRAW_HOST: {
            for (int i = 0; i < MULTIPLAYERSCREEN_BUTTON_COUNT; ++i) entity->buttons[i]->alpha = 0;
            for (int i = 0; i < 3; ++i) entity->codeLabel[i]->alpha = 0x100;
            for (int i = 0; i < 8; ++i) entity->enterCodeLabel[i]->alpha = 0;
            for (int i = 0; i < 2; ++i) entity->enterCodeSlider[i]->alpha = 0;

            entity->selectedButton                                = MULTIPLAYERSCREEN_BUTTON_COPY;
            entity->buttons[MULTIPLAYERSCREEN_BUTTON_COPY]->alpha = 0x100;

            entity->roomCode = 0;
            SetStringToFont8(entity->codeLabel[1]->text, "FETCHING...", entity->codeLabel[1]->fontID);
            entity->codeLabel[1]->alignPtr(entity->codeLabel[1], ALIGN_CENTER);

            ServerPacket send;
            send.header = 0;
            // send over a preferred roomcode style
            if (!vsGameLength)
                vsGameLength = 4;
            if (!vsItemMode)
                vsItemMode = 1;
            send.data.multiData.type    = 0x00000FF0;
            send.data.multiData.data[0] = (vsGameLength << 4) | (vsItemMode << 8);
            send.data.multiData.data[1] = (int)strlen(networkGame);
            StrCopy((char *)&send.data.multiData.data[2], networkGame);

            sendServerPacket(send);
            break;
        }
        case MULTIPLAYERSCREEN_STATEDRAW_JOIN: {
            for (int i = 0; i < MULTIPLAYERSCREEN_BUTTON_COUNT; ++i) entity->buttons[i]->alpha = 0;
            entity->selectedButton = MULTIPLAYERSCREEN_BUTTON_COUNT;
            entity->touchedUpID    = -1;
            entity->touchedDownID  = -1;

            for (int i = 0; i < 3; ++i) entity->codeLabel[i]->alpha = 0;

            entity->roomCode = 0;
            for (int i = 0; i < 8; ++i) {
                entity->enterCodeLabel[i]->alpha      = 0x100;
                entity->enterCodeLabel[i]->useColours = false;

                char codeBuf[0x10];
                sprintf(codeBuf, "%X", 0);
                SetStringToFont8(entity->enterCodeLabel[i]->text, codeBuf, entity->enterCodeLabel[i]->fontID);
                entity->enterCodeLabel[i]->alignPtr(entity->enterCodeLabel[i], ALIGN_CENTER);
            }
            entity->enterCodeLabel[0]->useColours                     = true;
            entity->buttons[MULTIPLAYERSCREEN_BUTTON_JOINROOM]->alpha = 0x100;
            entity->buttons[MULTIPLAYERSCREEN_BUTTON_PASTE]->alpha    = 0x100;

            break;
        }
    }
    entity->stateDraw = MULTIPLAYERSCREEN_STATEDRAW_NONE;

    SetRenderBlendMode(RENDER_BLEND_ALPHA);
    RenderMesh(entity->meshPanel, MESH_COLOURS, false);
    SetRenderBlendMode(RENDER_BLEND_ALPHA);
    NewRenderState();
    SetRenderMatrix(NULL);

    if (entity->backPressed)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, entity->arrowAlpha, entity->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, entity->arrowAlpha, entity->textureArrows);
}
#endif
