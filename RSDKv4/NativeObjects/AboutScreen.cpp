#include "RetroEngine.hpp"

void AboutScreen_Create(void *objPtr)
{
    RSDK_THIS(AboutScreen);

    entity->label                  = CREATE_ENTITY(TextLabel);
    entity->label->useRenderMatrix = true;
    entity->label->fontID          = FONT_HEADING;
    entity->label->scale           = 0.2;
    entity->label->alpha           = 256;
    entity->label->x               = -144.0;
    entity->label->y               = 100.0;
    entity->label->z               = 16.0;
    entity->label->state           = 0;
    SetStringToFont(entity->label->text, strAbout, FONT_HEADING);

    char title[0x40];
    for (int i = 0; i < StrLength(Engine.gameWindowText); ++i) title[i] = toupper(Engine.gameWindowText[i]);
    SetStringToFont8(entity->gameTitle, title, FONT_LABEL);
    SetStringToFont(entity->versionNameText, strVersionName, FONT_LABEL);
    SetStringToFont8(entity->versionText, Engine.gameVersion, FONT_LABEL);

    entity->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", -1);
    SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 0xC0);
    entity->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);

    int package = 0;
    switch (Engine.globalBoxRegion) {
        case REGION_JP:
            package         = LoadTexture("Data/Game/Models/Package_JP.png", TEXFMT_RGBA5551);
            entity->meshBox = LoadMesh("Data/Game/Models/JPBox.bin", package);
            break;
        case REGION_US:
            package         = LoadTexture("Data/Game/Models/Package_US.png", TEXFMT_RGBA5551);
            entity->meshBox = LoadMesh("Data/Game/Models/Box.bin", package);
            break;
        case REGION_EU:
            package         = LoadTexture("Data/Game/Models/Package_EU.png", TEXFMT_RGBA5551);
            entity->meshBox = LoadMesh("Data/Game/Models/Box.bin", package);
            break;
        default: break;
    }

    SetMeshAnimation(entity->meshBox, &entity->animator, 16, 16, 0.0);
    AnimateMesh(entity->meshBox, &entity->animator);

    float y = -24.0f;
    for (int i = 0; i < ABOUT_BTN_COUNT; ++i) {
        entity->buttons[i]                   = CREATE_ENTITY(PushButton);
        entity->buttons[i]->useRenderMatrix  = true;
        entity->buttons[i]->x                = 64.0;
        entity->buttons[i]->y                = y;
        entity->buttons[i]->z                = 0.0;
        entity->buttons[i]->scale            = 0.175;
        entity->buttons[i]->bgColour         = 0x00A048;
        entity->buttons[i]->bgColourSelected = 0x00C060;

        y -= 32.0f;
    }

    SetStringToFont(entity->buttons[ABOUT_BTN_PRIVACY]->text, strPrivacy, FONT_LABEL);
    SetStringToFont(entity->buttons[ABOUT_BTN_TERMS]->text, strTerms, FONT_LABEL);
}
void AboutScreen_Main(void *objPtr)
{
    RSDK_THIS(AboutScreen);
    NativeEntity_OptionsMenu *optionsMenu = (NativeEntity_OptionsMenu *)entity->optionsMenu;

    switch (entity->state) {
        case ABOUT_STATE_ENTER: {
            if (entity->arrowAlpha < 0x100)
                entity->arrowAlpha += 8;

            float maxWidth = 0;
            for (int i = 0; i < ABOUT_BTN_COUNT; ++i) {
                if (entity->buttons[i]->textWidth > maxWidth)
                    maxWidth = entity->buttons[i]->textWidth;
            }
            for (int i = 0; i < ABOUT_BTN_COUNT; ++i) {
                entity->buttons[i]->textWidth = maxWidth;
            }

            entity->scale = fminf(entity->scale + ((1.05 - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrix2, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix2);
            SetRenderMatrix(&entity->renderMatrix);

            memcpy(&entity->label->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < ABOUT_BTN_COUNT; ++i) memcpy(&entity->buttons[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                entity->arrowAlpha = 256;
                entity->timer      = 0.0;
                entity->state      = ABOUT_STATE_MAIN;
            }
            break;
        }
        case ABOUT_STATE_MAIN: {
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
                        if (entity->selectedButton < 0)
                            entity->selectedButton = 1;
                    }
                    else if (keyPress.down) {
                        PlaySfxByName("Menu Move", false);
                        entity->selectedButton++;
                        if (entity->selectedButton >= 2)
                            entity->selectedButton = 0;
                    }

                    for (int i = 0; i < ABOUT_BTN_COUNT; ++i) entity->buttons[i]->state = 0;
                    entity->buttons[entity->selectedButton]->state = 1;

                    if (keyPress.start || keyPress.A) {
                        PlaySfxByName("Menu Select", false);
                        entity->buttons[entity->selectedButton]->state = 2;
                        entity->state                                  = ABOUT_STATE_ACTION;
                    }
                    else if (keyPress.B) {
                        PlaySfxByName("Menu Back", false);
                        entity->backPressed = false;
                        entity->state       = ABOUT_STATE_EXIT;
                    }
                }
            }
            else {
                if (touches > 0) {
                    float y = -32.0f;
                    for (int i = 0; i < ABOUT_BTN_COUNT; ++i) {
                        bool valid = CheckTouchRect(64.0, y, ((64.0 * entity->buttons[i]->scale) + entity->buttons[i]->textWidth) * 0.75, 12.0) >= 0;
                        entity->buttons[i]->state = valid;

                        y -= 32.0f;
                    }
                    entity->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (entity->state == ABOUT_STATE_MAIN) {
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
                    for (int i = 0; i < ABOUT_BTN_COUNT; ++i) {
                        if (entity->buttons[i]->state == 1) {
                            PlaySfxByName("Menu Select", false);
                            entity->buttons[i]->state = 2;
                            entity->selectedButton    = i;
                            entity->state             = ABOUT_STATE_ACTION;
                            break;
                        }
                    }

                    if (keyPress.B || entity->backPressed) {
                        PlaySfxByName("Menu Back", false);
                        entity->backPressed = false;
                        entity->state       = ABOUT_STATE_EXIT;
                    }
                    else {
                        if (entity->state == ABOUT_STATE_MAIN) {
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
        case ABOUT_STATE_ACTION: {
            CheckKeyDown(&keyDown);
            SetRenderMatrix(&entity->renderMatrix);

            if (entity->buttons[entity->selectedButton]->state) {
                switch (entity->selectedButton) {
                    default: break;
                    case ABOUT_BTN_PRIVACY: ShowWebsite(0); break;
                    case ABOUT_BTN_TERMS: ShowWebsite(1); break;
                }
                entity->state = ABOUT_STATE_MAIN;
            }
            break;
        }
        case ABOUT_STATE_EXIT: {
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
            for (int i = 0; i < ABOUT_BTN_COUNT; ++i) memcpy(&entity->buttons[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                optionsMenu->state = 7;
                RemoveNativeObject(entity->label);
                for (int i = 0; i < ABOUT_BTN_COUNT; ++i) RemoveNativeObject(entity->buttons[i]);
                RemoveNativeObject(entity);
                return;
            }
            break;
        }
    }

    RenderMesh(entity->meshPanel, MESH_COLOURS, false);
    RenderText(entity->gameTitle, FONT_LABEL, 24.0, 56.0, 0.0, 0.125, 255);
    RenderText(entity->versionNameText, FONT_LABEL, 24.0, 32.0, 0.0, 0.125, 255);
    RenderText(entity->versionText, FONT_LABEL, 24.0, 8.0, 0.0, 0.125, 255);
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
    RenderMesh(entity->meshBox, MESH_NORMALS, true);
    SetRenderMatrix(NULL);

    if (entity->backPressed)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, entity->arrowAlpha, entity->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, entity->arrowAlpha, entity->textureArrows);
}
