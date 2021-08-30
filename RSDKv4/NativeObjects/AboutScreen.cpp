#include "RetroEngine.hpp"

void AboutScreen_Create(void *objPtr)
{
    RSDK_THIS(AboutScreen);

    entity->label          = CREATE_ENTITY(TextLabel);
    entity->label->useRenderMatrix = true;
    entity->label->fontID          = 0;
    entity->label->textScale       = 0.2;
    entity->label->textAlpha       = 256;
    entity->label->textX           = -144.0;
    entity->label->textY           = 100.0;
    entity->label->textZ           = 16.0;
    entity->label->state       = 0;
    SetStringToFont(entity->label->text, strAbout, 0);

    char title[0x40];
    for (int i = 0; i < StrLength(Engine.gameWindowText); ++i) title[i] = toupper(Engine.gameWindowText[i]);
    SetStringToFont8(entity->gameTitle, title, 1);
    SetStringToFont(entity->versionNameText, strVersionName, 1);
    SetStringToFont8(entity->versionText, Engine.gameVersion, 1);

    entity->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", 255);
    SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 192);
    entity->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);

    int package = 0;
    switch (Engine.globalBoxRegion) {
        case 0:
            package         = LoadTexture("Data/Game/Models/Package_JP.png", 2);
            entity->meshBox = LoadMesh("Data/Game/Models/JPBox.bin", package);
            break;
        case 1:
            package         = LoadTexture("Data/Game/Models/Package_US.png", 2);
            entity->meshBox = LoadMesh("Data/Game/Models/Box.bin", package);
            break;
        case 2:
            package         = LoadTexture("Data/Game/Models/Package_EU.png", 2);
            entity->meshBox = LoadMesh("Data/Game/Models/Box.bin", package);
            break;
        default: break;
    }

    SetMeshAnimation(entity->meshBox, &entity->animator, 16, 16, 0.0);
    AnimateMesh(entity->meshBox, &entity->animator);

    entity->buttons[0]                   = CREATE_ENTITY(PushButton);
    entity->buttons[0]->useRenderMatrix  = true;
    entity->buttons[0]->x                = 64.0;
    entity->buttons[0]->y                = -24.0;
    entity->buttons[0]->z                = 0.0;
    entity->buttons[0]->scale            = 0.175;
    entity->buttons[0]->bgColour         = 0x00A048;
    entity->buttons[0]->bgColourSelected = 0x00C060;
    SetStringToFont(entity->buttons[0]->text, strPrivacy, 1);

    entity->buttons[1]                   = CREATE_ENTITY(PushButton);
    entity->buttons[1]->useRenderMatrix  = true;
    entity->buttons[1]->x                = 64.0;
    entity->buttons[1]->y                = -56.0;
    entity->buttons[1]->z                = 0.0;
    entity->buttons[1]->scale            = 0.175;
    entity->buttons[1]->bgColour         = 0x00A048;
    entity->buttons[1]->bgColourSelected = 0x00C060;
    SetStringToFont(entity->buttons[1]->text, strTerms, 1);
}
void AboutScreen_Main(void *objPtr)
{
    RSDK_THIS(AboutScreen);
    NativeEntity_OptionsMenu *optionsMenu = (NativeEntity_OptionsMenu *)entity->optionsMenu;

    switch (entity->state) {
        case 0: { // fade in
            if (entity->arrowAlpha < 0x100)
                entity->arrowAlpha += 8;

            float maxWidth = 0;
            for (int i = 0; i < 2; ++i) {
                if (entity->buttons[i]->textWidth > maxWidth)
                    maxWidth = entity->buttons[i]->textWidth;
            }
            for (int i = 0; i < 2; ++i) {
                entity->buttons[i]->textWidth = maxWidth;
            }

            entity->scale = fminf(entity->scale + ((1.05 - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrix2, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix2);
            SetRenderMatrix(&entity->renderMatrix);

            memcpy(&entity->label->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 2; ++i) memcpy(&entity->buttons[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            entity->field_18 += Engine.deltaTime;
            if (entity->field_18 > 0.5) {
                entity->arrowAlpha = 256;
                entity->field_18   = 0.0;
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
                    if (keyPress.up) {
                        PlaySfx(21, 0);
                        entity->selectedButton--;
                        if (entity->selectedButton < 0)
                            entity->selectedButton = 1;
                    }
                    else if (keyPress.down) {
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
                        entity->touchValid = false;
                        entity->state      = 3;
                    }
                }
            }
            else {
                if (touches > 0) {
                    float y = -32.0f;
                    for (int i = 0; i < 2; ++i) {
                        bool valid = CheckTouchRect(64.0, y, ((64.0 * entity->buttons[i]->scale) + entity->buttons[i]->textWidth) * 0.75, 12.0) >= 0;
                        entity->buttons[i]->state = valid;

                        y -= 32.0f;
                    }
                    entity->touchValid = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (entity->state == 1) {
                        if (keyDown.up) {
                            entity->selectedButton = 1;
                            usePhysicalControls    = true;
                        }
                        if (keyDown.down) {
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

                    if (keyPress.B || entity->touchValid) {
                        PlaySfx(23, 0);
                        entity->touchValid = false;
                        entity->state      = 3;
                    }
                    else {
                        if (entity->state == 1) {
                            if (keyDown.up) {
                                entity->selectedButton = 1;
                                usePhysicalControls    = true;
                            }
                            if (keyDown.down) {
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
                switch (entity->selectedButton) {
                    default: break;
                    case 0: ShowWebsite(0); break;
                    case 1: ShowWebsite(1); break;
                }
                entity->state = 1;
            }
            break;
        }
        case 3: { // fade out
            if (entity->arrowAlpha > 0)
                entity->arrowAlpha -= 8;

            if (entity->field_18 < 0.2)
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

            entity->field_18 += Engine.deltaTime;
            if (entity->field_18 > 0.5) {
                optionsMenu->state = 7;
                RemoveNativeObject(entity->label);
                for (int i = 0; i < 2; ++i) RemoveNativeObject(entity->buttons[i]);
                RemoveNativeObject(entity);
                return;
            }
            break;
        }
    }

    RenderMesh(entity->meshPanel, 0, false);
    RenderText(entity->gameTitle, 1, 24.0, 56.0, 0.0, 0.125, 255);
    RenderText(entity->versionNameText, 1, 24.0, 32.0, 0.0, 0.125, 255);
    RenderText(entity->versionText, 1, 24.0, 8.0, 0.0, 0.125, 255);
    NewRenderState();
    
    entity->rotationY -= Engine.deltaTime;
    if (entity->rotationY < -(M_PI * 2))
        entity->rotationY += (M_PI * 2);

    matrixScaleXYZF(&entity->matrix3, 0.6 * entity->scale, 0.6 * entity->scale, 0.6 * entity->scale);
    matrixRotateYF(&entity->matrix2, entity->rotationY);
    matrixMultiplyF(&entity->matrix3, &entity->matrix2);
    matrixTranslateXYZF(&entity->matrix2, -56.0, -8.0, 160.0);
    matrixMultiplyF(&entity->matrix3, &entity->matrix2);
    SetRenderMatrix(&entity->matrix3);
    RenderMesh(entity->meshBox, 1, true);
    SetRenderMatrix(NULL);

    if (entity->touchValid)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, entity->arrowAlpha, entity->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, entity->arrowAlpha, entity->textureArrows);
}
