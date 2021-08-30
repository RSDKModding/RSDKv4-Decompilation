#include "RetroEngine.hpp"

#if !RETRO_USE_ORIGINAL_CODE
void MultiplayerScreen_Create(void *objPtr)
{
    RSDK_THIS(MultiplayerScreen);

    entity->label                  = CREATE_ENTITY(TextLabel);
    entity->label->useRenderMatrix = true;
    entity->label->fontID          = 0;
    entity->label->textScale       = 0.2;
    entity->label->textAlpha       = 256;
    entity->label->textX           = -144.0;
    entity->label->textY           = 100.0;
    entity->label->textZ           = 16.0;
    entity->label->state           = 0;
    SetStringToFont8(entity->label->text, "MULTIPLAYER", 0);

    entity->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", -2);
    SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 0xC0);
    entity->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);

    entity->buttons[0]                   = CREATE_ENTITY(PushButton);
    entity->buttons[0]->useRenderMatrix  = true;
    entity->buttons[0]->x                = -64.0;
    entity->buttons[0]->y                = -56.0;
    entity->buttons[0]->z                = 0.0;
    entity->buttons[0]->scale            = 0.175;
    entity->buttons[0]->bgColour         = 0x00A048;
    entity->buttons[0]->bgColourSelected = 0x00C060;
    SetStringToFont8(entity->buttons[0]->text, "HOST", 1);

    entity->buttons[1]                   = CREATE_ENTITY(PushButton);
    entity->buttons[1]->useRenderMatrix  = true;
    entity->buttons[1]->x                = 64.0;
    entity->buttons[1]->y                = -56.0;
    entity->buttons[1]->z                = 0.0;
    entity->buttons[1]->scale            = 0.175;
    entity->buttons[1]->bgColour         = 0x00A048;
    entity->buttons[1]->bgColourSelected = 0x00C060;
    SetStringToFont8(entity->buttons[1]->text, "JOIN", 1);

    entity->buttons[2]                   = CREATE_ENTITY(PushButton);
    entity->buttons[2]->useRenderMatrix  = true;
    entity->buttons[2]->x                = 0;
    entity->buttons[2]->y                = -56.0;
    entity->buttons[2]->z                = 0.0;
    entity->buttons[2]->scale            = 0.175;
    entity->buttons[2]->bgColour         = 0x00A048;
    entity->buttons[2]->bgColourSelected = 0x00C060;
    entity->buttons[2]->alpha            = 0;
    SetStringToFont8(entity->buttons[2]->text, "JOIN ROOM", 1);

    for (int i = 0; i < 3; ++i) {
        entity->codeLabel[i]                  = CREATE_ENTITY(TextLabel);
        entity->codeLabel[i]->useRenderMatrix = true;
        entity->codeLabel[i]->fontID          = 1;
        entity->codeLabel[i]->textScale       = 0.15;
        entity->codeLabel[i]->textAlpha       = 0;
        entity->codeLabel[i]->textX           = 0;
        entity->codeLabel[i]->textY           = 0;
        entity->codeLabel[i]->textZ           = 16.0;
        entity->codeLabel[i]->state           = 0;
    }

    SetStringToFont8(entity->codeLabel[0]->text, "ROOM CODE", entity->codeLabel[0]->fontID);
    entity->codeLabel[0]->alignPtr(entity->codeLabel[0], 1);

    SetStringToFont8(entity->codeLabel[1]->text, "UNKNOWN", entity->codeLabel[1]->fontID);
    entity->codeLabel[1]->alignPtr(entity->codeLabel[1], 1);
    entity->codeLabel[1]->textY -= 20.0f;

    entity->codeLabel[2]->textY     = 48.0f;
    entity->codeLabel[2]->textScale = 0.20f;
    SetStringToFont8(entity->codeLabel[2]->text, "WAITING FOR 2P...", entity->codeLabel[2]->fontID);
    entity->codeLabel[2]->alignPtr(entity->codeLabel[2], 1);

    char codeBuf[0x8];
    sprintf(codeBuf, "%X", 0);

    float x = -64.0f;
    for (int i = 0; i < 8; ++i) {
        entity->enterCodeLabel[i]                  = CREATE_ENTITY(TextLabel);
        entity->enterCodeLabel[i]->useRenderMatrix = true;
        entity->enterCodeLabel[i]->fontID          = 1;
        entity->enterCodeLabel[i]->textScale       = 0.175;
        entity->enterCodeLabel[i]->textAlpha       = 0;
        entity->enterCodeLabel[i]->textX           = x;
        entity->enterCodeLabel[i]->textY           = 0;
        entity->enterCodeLabel[i]->textZ           = 16.0;
        entity->enterCodeLabel[i]->state           = 0;

        entity->enterCodeLabel[i]->r = 0xFF;
        entity->enterCodeLabel[i]->g = 0xFF;
        entity->enterCodeLabel[i]->b = 0x00;

        SetStringToFont8(entity->enterCodeLabel[i]->text, codeBuf, entity->enterCodeLabel[i]->fontID);
        entity->enterCodeLabel[i]->alignPtr(entity->enterCodeLabel[i], 1);
        entity->enterCodeLabel[i]->textY -= 20.0f;

        x += 16.0f;
    }
}
void MultiplayerScreen_Main(void *objPtr)
{
    RSDK_THIS(MultiplayerScreen);

    switch (entity->state) {
        case 0: { // fade in
            if (entity->arrowAlpha < 0x100)
                entity->arrowAlpha += 8;

            entity->scale = fminf(entity->scale + ((1.05 - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrix2, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix2);
            SetRenderMatrix(&entity->renderMatrix);

            memcpy(&entity->label->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 3; ++i) memcpy(&entity->codeLabel[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 3; ++i) memcpy(&entity->buttons[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 8; ++i) memcpy(&entity->enterCodeLabel[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                entity->timer = 0.0;
                entity->state = 1;
            }
            break;
        }
        case 1: { // main screen
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->renderMatrix);

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.left) {
                        PlaySfx(21, 0);
                        entity->selectedButton--;
                        if (entity->selectedButton < 0)
                            entity->selectedButton = 1;
                    }
                    else if (keyPress.right) {
                        PlaySfx(21, 0);
                        entity->selectedButton++;
                        if (entity->selectedButton >= 2)
                            entity->selectedButton = 0;
                    }

                    entity->buttons[0]->state                      = 0;
                    entity->buttons[1]->state                      = 0;
                    entity->buttons[entity->selectedButton]->state = 1;

                    if (keyPress.start || keyPress.A) {
                        PlaySfx(22, 0);
                        entity->buttons[entity->selectedButton]->state = 2;
                        entity->state                                  = 2;
                    }
                    else if (keyPress.B) {
                        PlaySfx(23, 0);
                        entity->backPressed = false;
                        entity->state       = 3;
                    }
                }
            }
            else {
                if (touches > 0) {
                    float x = -64.0f;
                    for (int i = 0; i < 2; ++i) {
                        bool valid =
                            CheckTouchRect(x, -64.0f, ((64.0 * entity->buttons[i]->scale) + entity->buttons[i]->textWidth) * 0.75, 12.0) >= 0;
                        entity->buttons[i]->state = valid;

                        x += 128.0f;
                    }
                    entity->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (entity->state == 1) {
                        if (keyDown.left) {
                            entity->selectedButton = 1;
                            usePhysicalControls    = true;
                        }
                        if (keyDown.right) {
                            entity->selectedButton = 0;
                            usePhysicalControls    = true;
                        }
                    }
                }
                else {
                    for (int i = 0; i < 2; ++i) {
                        if (entity->buttons[i]->state == 1) {
                            PlaySfx(22, 0);
                            entity->buttons[i]->state = 2;
                            entity->selectedButton    = i;
                            entity->state             = 2;
                            break;
                        }
                    }

                    if (keyPress.B || entity->backPressed) {
                        PlaySfx(23, 0);
                        entity->backPressed           = false;
                        entity->state                 = 3;
                        NativeEntity_FadeScreen *fade = CREATE_ENTITY(FadeScreen); // fade back to menu
                        fade->fadeSpeed               = 1.0;
                        fade->timeLimit               = 0.5;
                        fade->state                   = 5;
                    }
                    else {
                        if (entity->state == 1) {
                            if (keyDown.left) {
                                entity->selectedButton = 1;
                                usePhysicalControls    = true;
                            }
                            if (keyDown.right) {
                                entity->selectedButton = 0;
                                usePhysicalControls    = true;
                            }
                        }
                    }
                }
            }
            break;
        }
        case 2: { // action
            CheckKeyDown(&keyDown);
            SetRenderMatrix(&entity->renderMatrix);

            if (entity->buttons[entity->selectedButton]->state) {
                entity->state = 1;
                switch (entity->selectedButton) {
                    default: break;
                    case 0: /*Host*/
                        entity->state         = 4;
                        entity->nextState     = 5;
                        entity->nextStateDraw = 1;
                        break;
                    case 1: /*Join*/
                        entity->state         = 4;
                        entity->nextState     = 6;
                        entity->nextStateDraw = 2;
                        break;
                    case 2: { /*Join Room*/
                        if (entity->buttons[2]->state == 2) { ///hhhhhhack
                            setRoomCode(entity->roomCode);
                            CodedData send;
                            send.header                 = 0;
                            send.data.multiData.data[1] = strlen(networkGame);
                            StrCopy((char *)&send.data.multiData.data[2], networkGame);

                            sendCodedData(send);
                        }
                        entity->state = 7;

                        NativeEntity_FadeScreen *fade = CREATE_ENTITY(FadeScreen);
                        fade->state                   = 4;
                        fade->fadeSpeed               = 1.0f;
                        fade->timeLimit               = 0.5f;
                        break;
                    }
                }
            }
            break;
        }
        case 7:
        case 3: { // fade out
            if (entity->arrowAlpha > 0)
                entity->arrowAlpha -= 8;

            if (entity->timer < 0.2)
                entity->scale = fmaxf(entity->scale + ((1.5f - entity->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);
            else
                entity->scale = fmaxf(entity->scale + ((-1.0f - entity->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrix2, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix2);
            SetRenderMatrix(&entity->renderMatrix);

            memcpy(&entity->label->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 3; ++i) memcpy(&entity->codeLabel[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 3; ++i) memcpy(&entity->buttons[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 8; ++i) memcpy(&entity->enterCodeLabel[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                RemoveNativeObject(entity->label);
                for (int i = 0; i < 3; ++i) RemoveNativeObject(entity->codeLabel[i]);
                for (int i = 0; i < 8; ++i) RemoveNativeObject(entity->enterCodeLabel[i]);
                for (int i = 0; i < 3; ++i) RemoveNativeObject(entity->buttons[i]);
                RemoveNativeObject(entity->bg);
                RemoveNativeObject(entity);
                if (entity->state == 3)
                    Engine.gameMode = ENGINE_RESETGAME;
                return;
            }
            break;
        }
        case 4: { // panel flip
            if (entity->arrowAlphaLR > 0)
                entity->arrowAlphaLR -= 0x20;

            if (entity->flipDir) {
                entity->rotationY += (10.0 * Engine.deltaTime);
                if (entity->rotationY > (M_PI * 2)) {
                    entity->state     = entity->nextState;
                    entity->rotationY = 0.0;
                }
                else if (entity->rotationY > M_PI) {
                    entity->stateDraw     = entity->nextStateDraw;
                    entity->nextStateDraw = -1;
                }
            }
            else {
                entity->rotationY -= (10.0 * Engine.deltaTime);
                if (entity->rotationY < -(M_PI * 2)) {
                    entity->state     = entity->nextState;
                    entity->rotationY = 0.0;
                }
                else if (entity->rotationY < -M_PI) {
                    entity->stateDraw     = entity->nextStateDraw;
                    entity->nextStateDraw = -1;
                }
            }
            NewRenderState();
            matrixRotateYF(&entity->renderMatrix, entity->rotationY);
            matrixTranslateXYZF(&entity->matrix2, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix2);
            SetRenderMatrix(&entity->renderMatrix);

            for (int i = 0; i < 3; ++i) memcpy(&entity->codeLabel[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 3; ++i) memcpy(&entity->buttons[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 8; ++i) memcpy(&entity->enterCodeLabel[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            break;
        }
        case 5: { // host screen
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->renderMatrix);

            if (!entity->roomCode) {
                int code = getRoomCode();
                if (code) {
                    char buffer[0x30];
                    int code = getRoomCode();
                    sprintf(buffer, "%X", code);
                    SetStringToFont8(entity->codeLabel[1]->text, buffer, entity->codeLabel[1]->fontID);
                    entity->codeLabel[1]->alignPtr(entity->codeLabel[1], 1);
                    entity->roomCode = code;
                }
            }
            else {
                // listen.
                if (Engine.gameMode != ENGINE_WAIT2PVS) {
                    entity->buttons[2]->state = 1; //HAck
                    entity->selectedButton    = 2;
                    entity->state             = 2;
                }
            }

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.B) {
                        entity->dialog = CREATE_ENTITY(DialogPanel);
                        SetStringToFont8(entity->dialog->text, "ARE YOU SURE YOU WANT TO EXIT?\rTHIS WILL CLOSE THE ROOM", 2);
                        entity->state = 8;
                        PlaySfx(40, 0);
                    }
                }
            }
            else {
                if (touches > 0) {
                    entity->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                }
                else {
                    if (keyPress.B || entity->backPressed) {
                        entity->dialog = CREATE_ENTITY(DialogPanel);
                        SetStringToFont8(entity->dialog->text, "ARE YOU SURE YOU WANT TO EXIT?\rTHIS WILL CLOSE THE ROOM", 2);
                        entity->state = 8;
                        PlaySfx(40, 0);
                    }
                }
            }
            break;
        }
        case 6: { // join screen
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->renderMatrix);

            if (entity->arrowAlphaLR < 0x100)
                entity->arrowAlphaLR += 0x20;

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.left) {
                        PlaySfx(21, 0);
                        entity->selectedButton--;
                        if (entity->selectedButton < 2)
                            entity->selectedButton = 10;
                    }
                    else if (keyPress.right) {
                        PlaySfx(21, 0);
                        entity->selectedButton++;
                        if (entity->selectedButton > 10)
                            entity->selectedButton = 2;
                    }

                    if (entity->selectedButton != 2 && (keyPress.up || keyPress.down)) {
                        union {
                            int val;
                            byte bytes[4];
                        } u;
                        u.val         = entity->roomCode;
                        int n         = 7 - (entity->selectedButton - 3);
                        int nybbles[] = { u.bytes[n >> 1] & 0xF, ((u.bytes[n >> 1] & 0xF0) >> 4) & 0xF };

                        byte val = nybbles[n & 1];
                        if (keyPress.up) {
                            PlaySfx(21, 0);
                            nybbles[n & 1] = (nybbles[n & 1] + 1) & 0xF;
                        }
                        else if (keyPress.down) {
                            PlaySfx(21, 0);
                            nybbles[n & 1] = (nybbles[n & 1] - 1) & 0xF;
                        }

                        u.bytes[n >> 1]  = (nybbles[1] << 4) | (nybbles[0] & 0xF);
                        entity->roomCode = u.val;

                        for (int i = 0; i < 8; i += 2) {
                            int n         = 7 - i;
                            int nybbles[] = { u.bytes[n >> 1] & 0xF, ((u.bytes[n >> 1] & 0xF0) >> 4) & 0xF };

                            entity->enterCodeLabel[i + 0]->textAlpha = 0x100;
                            entity->enterCodeLabel[i + 1]->textAlpha = 0x100;

                            entity->enterCodeLabel[i + 0]->useColours = false;
                            entity->enterCodeLabel[i + 1]->useColours = false;

                            char codeBuf[0x10];
                            sprintf(codeBuf, "%X", nybbles[1]);
                            SetStringToFont8(entity->enterCodeLabel[i + 0]->text, codeBuf, entity->enterCodeLabel[i + 0]->fontID);
                            entity->enterCodeLabel[i + 0]->alignPtr(entity->enterCodeLabel[i + 0], 1);

                            sprintf(codeBuf, "%X", nybbles[0]);
                            SetStringToFont8(entity->enterCodeLabel[i + 1]->text, codeBuf, entity->enterCodeLabel[i + 1]->fontID);
                            entity->enterCodeLabel[i + 1]->alignPtr(entity->enterCodeLabel[i + 1], 1);
                        }

                        entity->enterCodeLabel[n]->useColours = true;
                    }

                    for (int i = 0; i < 8; ++i) entity->enterCodeLabel[i]->useColours = false;
                    entity->buttons[2]->state = 0;

                    if (entity->selectedButton == 2)
                        entity->buttons[2]->state = 1;
                    else
                        entity->enterCodeLabel[entity->selectedButton - 3]->useColours = true;

                    if (keyPress.start || keyPress.A) {
                        PlaySfx(22, 0);
                        entity->buttons[2]->state = 2;
                        entity->selectedButton    = 2;
                        entity->state             = 2;
                    }
                    else if (keyPress.B) {
                        PlaySfx(23, 0);
                        entity->state         = 4;
                        entity->nextState     = 1;
                        entity->nextStateDraw = 0;
                    }
                }
            }
            else {
                if (touches > 0) {
                    float x       = -58.0f;
                    for (int i = 0; i < 8; ++i) {
                        if (CheckTouchRect(x, 16.0f, 16.0f, 16.0) >= 0)
                            entity->touchedUpID = i;
                        if (CheckTouchRect(x, -16.0f, 16.0f, 16.0) >= 0)
                            entity->touchedDownID = i;
                        x += 16;
                    }

                    for (int i = 0; i < 8; ++i) entity->enterCodeLabel[i]->useColours = false;

                    int id = entity->touchedDownID;
                    if (entity->touchedUpID >= 0)
                        id = entity->touchedUpID;
                    if (id >= 0)
                        entity->enterCodeLabel[id]->useColours = true;

                    entity->buttons[2]->state =
                        CheckTouchRect(0, -64.0f, ((64.0 * entity->buttons[2]->scale) + entity->buttons[2]->textWidth) * 0.75, 12.0) >= 0;
                    entity->leftPressed  = CheckTouchRect(-124.0, 0.0, 32.0, 32.0) >= 0;
                    entity->rightPressed = CheckTouchRect(124.0, 0.0, 32.0, 32.0) >= 0;

                    entity->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (keyDown.left) {
                        entity->selectedButton = 3;
                        usePhysicalControls    = true;
                    }
                    if (keyDown.right) {
                        entity->selectedButton = 10;
                        usePhysicalControls    = true;
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

                        byte val = nybbles[n & 1];
                        if (entity->touchedUpID >= 0) {
                            PlaySfx(21, 0);
                            nybbles[n & 1] = (nybbles[n & 1] + 1) & 0xF;
                        }
                        else if (entity->touchedDownID >= 0) {
                            PlaySfx(21, 0);
                            nybbles[n & 1] = (nybbles[n & 1] - 1) & 0xF;
                        }

                        u.bytes[n >> 1]  = (nybbles[1] << 4) | (nybbles[0] & 0xF);
                        entity->roomCode = u.val;

                        for (int i = 0; i < 8; i += 2) {
                            int n         = 7 - i;
                            int nybbles[] = { u.bytes[n >> 1] & 0xF, ((u.bytes[n >> 1] & 0xF0) >> 4) & 0xF };

                            entity->enterCodeLabel[i + 0]->textAlpha = 0x100;
                            entity->enterCodeLabel[i + 1]->textAlpha = 0x100;

                            entity->enterCodeLabel[i + 0]->useColours = false;
                            entity->enterCodeLabel[i + 1]->useColours = false;

                            char codeBuf[0x10];
                            sprintf(codeBuf, "%X", nybbles[1]);
                            SetStringToFont8(entity->enterCodeLabel[i + 0]->text, codeBuf, entity->enterCodeLabel[i + 0]->fontID);
                            entity->enterCodeLabel[i + 0]->alignPtr(entity->enterCodeLabel[i + 0], 1);

                            sprintf(codeBuf, "%X", nybbles[0]);
                            SetStringToFont8(entity->enterCodeLabel[i + 1]->text, codeBuf, entity->enterCodeLabel[i + 1]->fontID);
                            entity->enterCodeLabel[i + 1]->alignPtr(entity->enterCodeLabel[i + 1], 1);
                        }

                        entity->enterCodeLabel[id]->useColours = true;

                        entity->touchedUpID   = -1;
                        entity->touchedDownID = -1;
                    }

                    if (entity->buttons[2]->state == 1) {
                        PlaySfx(22, 0);
                        entity->buttons[2]->state = 2;
                        entity->selectedButton    = 2;
                        entity->state             = 2;
                    }
                    else if (entity->leftPressed) {
                        PlaySfx(21, 0);
                        entity->leftPressed = false;
                        entity->selectedButton--;
                        if (entity->selectedButton < 3)
                            entity->selectedButton = 10;
                    }
                    else if (entity->rightPressed) {
                        PlaySfx(21, 0);
                        entity->rightPressed = false;
                        entity->selectedButton++;
                        if (entity->selectedButton > 10)
                            entity->selectedButton = 3;
                    }

                    if (keyPress.B || entity->backPressed) {
                        PlaySfx(23, 0);
                        entity->backPressed   = false;
                        entity->state         = 4;
                        entity->nextState     = 1;
                        entity->nextStateDraw = 0;
                    }
                    else {
                        if (keyDown.left) {
                            entity->leftPressed    = false;
                            entity->selectedButton = 3;
                            usePhysicalControls    = true;
                        }
                        if (keyDown.right) {
                            entity->rightPressed   = false;
                            entity->selectedButton = 10;
                            usePhysicalControls    = true;
                        }
                    }
                }
            }
            break;
        }
        case 8: {
            if (entity->dialog->selection == 2 || entity->dialog->selection == 3) {
                entity->state = 5;
            }
            else if (entity->dialog->selection == 1) {
                PlaySfx(23, 0);
                entity->backPressed   = false;
                entity->state         = 4;
                entity->nextState     = 1;
                entity->nextStateDraw = 0;
                disconnectNetwork();
            }
            break;
        }
    }

    switch (entity->stateDraw) {
        default: break;
        case 0: // main screen
            for (int i = 0; i < 2; ++i) entity->buttons[i]->alpha = 0x100;
            entity->buttons[2]->alpha = 0;

            for (int i = 0; i < 3; ++i) entity->codeLabel[i]->textAlpha = 0;
            for (int i = 0; i < 8; ++i) entity->enterCodeLabel[i]->textAlpha = 0;

            entity->arrowAlphaLR   = 0;
            entity->selectedButton = 0;
            break;
        case 1: { // host screen
            for (int i = 0; i < 3; ++i) entity->buttons[i]->alpha = 0;
            for (int i = 0; i < 3; ++i) entity->codeLabel[i]->textAlpha = 0x100;
            for (int i = 0; i < 8; ++i) entity->enterCodeLabel[i]->textAlpha = 0;
            entity->arrowAlphaLR   = 0;
            entity->selectedButton = 0;

            entity->roomCode = 0;
            SetStringToFont8(entity->codeLabel[1]->text, "FETCHING...", entity->codeLabel[1]->fontID);
            entity->codeLabel[1]->alignPtr(entity->codeLabel[1], 1);

            CodedData send;
            send.header = 0;
            // send over a preferred roomcode style
            if (!vsGameLength)
                vsGameLength = 4;
            if (!vsItemMode)
                vsItemMode = 1;
            send.data.multiData.type    = 0x00000FF0;
            send.data.multiData.data[0] = (vsGameLength << 4) | (vsItemMode << 8);
            send.data.multiData.data[1] = strlen(networkGame);
            StrCopy((char *)&send.data.multiData.data[2], networkGame);

            sendCodedData(send);
            break;
        }
        case 2: { // join screen
            for (int i = 0; i < 2; ++i) entity->buttons[i]->alpha = 0;
            entity->buttons[2]->alpha = 0x100;
            entity->selectedButton    = 3;

            for (int i = 0; i < 3; ++i) entity->codeLabel[i]->textAlpha = 0;

            entity->roomCode = 0;
            for (int i = 0; i < 8; ++i) {
                entity->enterCodeLabel[i]->textAlpha  = 0x100;
                entity->enterCodeLabel[i]->useColours = false;

                char codeBuf[0x10];
                sprintf(codeBuf, "%X", 0);
                SetStringToFont8(entity->enterCodeLabel[i]->text, codeBuf, entity->enterCodeLabel[i]->fontID);
                entity->enterCodeLabel[i]->alignPtr(entity->enterCodeLabel[i], 1);
            }
            entity->enterCodeLabel[0]->useColours = true;

            break;
        }
    }
    entity->stateDraw = -1;

    SetRenderBlendMode(RENDER_BLEND_ALPHA);
    RenderMesh(entity->meshPanel, 0, false);
    SetRenderBlendMode(RENDER_BLEND_ALPHA);
    NewRenderState();
    SetRenderMatrix(NULL);

    if (entity->backPressed)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, entity->arrowAlpha, entity->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, entity->arrowAlpha, entity->textureArrows);

    if (entity->leftPressed)
        RenderImageFlipH(-124.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, entity->arrowAlphaLR, entity->textureArrows);
    else
        RenderImageFlipH(-124.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, entity->arrowAlphaLR, entity->textureArrows);

    if (entity->rightPressed)
        RenderImage(124.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 128.0, entity->arrowAlphaLR, entity->textureArrows);
    else
        RenderImage(124.0, 0.0, 160.0, 0.2, 0.3, 64.0, 64.0, 128.0, 128.0, 0.0, 0.0, entity->arrowAlphaLR, entity->textureArrows);
}
#endif
