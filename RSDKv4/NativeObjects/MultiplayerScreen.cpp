#include "RetroEngine.hpp"

#if !RETRO_USE_ORIGINAL_CODE
void MultiplayerScreen_Create(void *objPtr)
{
    RSDK_THIS(MultiplayerScreen);

    entity->label            = CREATE_ENTITY(TextLabel);
    entity->label->useMatrix = true;
    entity->label->fontID    = 0;
    entity->label->textScale = 0.2;
    entity->label->textAlpha = 256;
    entity->label->textX     = -144.0;
    entity->label->textY     = 100.0;
    entity->label->textZ     = 16.0;
    entity->label->alignment = 0;
    SetStringToFont8(entity->label->text, "MULTIPLAYER", 0);

    entity->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", -1);
    SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 0xC0);
    entity->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);

    entity->buttons[0]                  = CREATE_ENTITY(PushButton);
    entity->buttons[0]->useRenderMatrix = true;
    entity->buttons[0]->x               = -64.0;
    entity->buttons[0]->y               = -56.0;
    entity->buttons[0]->z               = 0.0;
    entity->buttons[0]->scale           = 0.175;
    entity->buttons[0]->blue            = 0x00A048;
    entity->buttons[0]->blue2           = 0x00C060;
    SetStringToFont8(entity->buttons[0]->text, "HOST", 1);

    entity->buttons[1]                  = CREATE_ENTITY(PushButton);
    entity->buttons[1]->useRenderMatrix = true;
    entity->buttons[1]->x               = 64.0;
    entity->buttons[1]->y               = -56.0;
    entity->buttons[1]->z               = 0.0;
    entity->buttons[1]->scale           = 0.175;
    entity->buttons[1]->blue            = 0x00A048;
    entity->buttons[1]->blue2           = 0x00C060;
    SetStringToFont8(entity->buttons[1]->text, "JOIN", 1);
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
            for (int i = 0; i < 2; ++i) memcpy(&entity->buttons[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                entity->timer   = 0.0;
                entity->state      = 1;
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
                        entity->state      = 3;
                    }
                }
            }
            else {
                if (touches > 0) {
                    float x = -64.0f;
                    for (int i = 0; i < 2; ++i) {
                        bool valid = CheckTouchRect(x, -64.0f, ((64.0 * entity->buttons[i]->scale) + entity->buttons[i]->textWidth) * 0.75, 12.0) >= 0;
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
                        entity->backPressed = false;
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
                    case 1:/*Join*/ 
                        entity->state         = 4;
                        entity->nextState     = 6;
                        entity->nextStateDraw = 2;
                        break;
                }
            }
            break;
        }
        case 3: { // fade out
            if (entity->arrowAlpha > 0x0)
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
            for (int i = 0; i < 2; ++i) memcpy(&entity->buttons[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                RemoveNativeObject(entity->label);
                for (int i = 0; i < 2; ++i) RemoveNativeObject(entity->buttons[i]);
                RemoveNativeObject(entity->bg);
                RemoveNativeObject(entity);
                Engine.gameMode = ENGINE_RESETGAME;
                return;
            }
            break;
        }
        case 4: { //panel flip
            if (entity->flipDir) {
                entity->rotationY += (10.0 * Engine.deltaTime);
                if (entity->rotationY > (M_PI * 2)) {
                    entity->state     = entity->nextState;
                    entity->rotationY = 0.0;
                }
                else if (entity->rotationY > M_PI) {
                    entity->stateDraw = entity->nextStateDraw;
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
            for (int i = 0; i < 2; ++i) memcpy(&entity->buttons[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            break;
        }
        case 5: { //host screen
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->renderMatrix);

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {

                    if (keyPress.start || keyPress.A) {
                        PlaySfx(22, 0);
                        //AWOOOGA (do your thang)
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
                            //entity->selectedButton = 1;
                            //usePhysicalControls    = true;
                        }
                        if (keyDown.right) {
                            //entity->selectedButton = 0;
                            //usePhysicalControls    = true;
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
                        entity->backPressed    = false;
                        entity->state         = 4;
                        entity->nextState     = 1;
                        entity->nextStateDraw = 0;
                    }
                    else {
                        if (entity->state == 1) {
                            if (keyDown.left) {
                                //entity->selectedButton = 1;
                                //usePhysicalControls    = true;
                            }
                            if (keyDown.right) {
                                //entity->selectedButton = 0;
                                //usePhysicalControls    = true;
                            }
                        }
                    }
                }
            }
            break;
        }
        case 6: { //join screen
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->renderMatrix);

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {

                    if (keyPress.start || keyPress.A) {
                        PlaySfx(22, 0);
                        // AWOOOGA
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
                            // entity->selectedButton = 1;
                            // usePhysicalControls    = true;
                        }
                        if (keyDown.right) {
                            // entity->selectedButton = 0;
                            // usePhysicalControls    = true;
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
                        entity->backPressed    = false;
                        entity->state         = 4;
                        entity->nextState     = 1;
                        entity->nextStateDraw = 0;
                    }
                    else {
                        if (entity->state == 1) {
                            if (keyDown.left) {
                                // entity->selectedButton = 1;
                                // usePhysicalControls    = true;
                            }
                            if (keyDown.right) {
                                // entity->selectedButton = 0;
                                // usePhysicalControls    = true;
                            }
                        }
                    }
                }
            }
            break;
        }
    }

    switch (entity->stateDraw) {
        default: break;
        case 0: //main screen
            for (int i = 0; i < 2; ++i) entity->buttons[i]->alpha = 0x100;
            break;
        case 1: // host screen
            for (int i = 0; i < 2; ++i) entity->buttons[i]->alpha = 0;
            break;
        case 2: // join screen
            for (int i = 0; i < 2; ++i) entity->buttons[i]->alpha = 0;
            break;
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
}
#endif
