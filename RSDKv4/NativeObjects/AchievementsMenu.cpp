#include "RetroEngine.hpp"

#if !RETRO_USE_ORIGINAL_CODE
void AchievementsMenu_Create(void *objPtr)
{
    RSDK_THIS(AchievementsMenu);

    entity->label                  = CREATE_ENTITY(TextLabel);
    entity->label->useRenderMatrix = true;
    entity->label->fontID          = FONT_HEADING;
    entity->label->scale           = 0.2;
    entity->label->alpha           = 256;
    entity->label->x               = -144.0;
    entity->label->y               = 100.0;
    entity->label->z               = 16.0;
    entity->label->state           = TEXTLABEL_STATE_IDLE;
    SetStringToFont(entity->label->text, strAchievements, FONT_HEADING);

    entity->scale      = 0;
    entity->arrowAlpha = 0;

    entity->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", -1);
    SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 0xC0);
    entity->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);

    char buffer[0x40];
    char buffer2[0x80];

    float y = 24.0f;
    for (int i = 0; i < 5; ++i) {
        entity->displays[i]                  = CREATE_ENTITY(AchievementDisplay);
        entity->displays[i]->fontID          = FONT_LABEL;
        entity->displays[i]->x               = -112.0f;
        entity->displays[i]->y               = y;
        entity->displays[i]->scale           = 0.11;
        entity->displays[i]->state           = achievements[i].status == 100;
        entity->displays[i]->useRenderMatrix = true;

        StringUpperCase(buffer, achievements[i].name);
        StringUpperCase(buffer2, achievements[i].desc);
        SetStringToFont8(entity->displays[i]->achievementText, buffer, entity->displays[i]->fontID);
        SetStringToFont8(entity->displays[i]->descriptionText, buffer2, entity->displays[i]->fontID);

        y -= 96.0f;
    }
}
void AchievementsMenu_Main(void *objPtr)
{
    RSDK_THIS(AchievementsMenu);

    switch (entity->state) {
        case 0: {
            if (entity->arrowAlpha < 0x100)
                entity->arrowAlpha += 8;

            entity->scale = fminf(entity->scale + ((1.05 - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, 0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrixTemp);
            SetRenderMatrix(&entity->renderMatrix);

            memcpy(&entity->label->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 5; ++i) memcpy(&entity->displays[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                entity->arrowAlpha = 0x100;
                entity->timer      = 0.0;
                entity->state      = 1;
            }
            break;
        }
        case 1: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->renderMatrix);

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (keyPress.up) {
                    }
                    else if (keyPress.down) {
                    }

                    if (keyPress.B) {
                        PlaySfxByName("Menu Back", false);
                        entity->backPressed = false;
                        entity->state       = ABOUT_STATE_EXIT;
                    }
                }
            }
            else {
                if (touches > 0) {
                    entity->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                }
                else {
                    if (keyPress.B || entity->backPressed) {
                        PlaySfxByName("Menu Back", false);
                        entity->backPressed = false;
                        entity->state       = ABOUT_STATE_EXIT;
                    }
                    else {
                        if (entity->state == ABOUT_STATE_MAIN) {
                            if (keyDown.up) {
                                usePhysicalControls = true;
                            }
                            if (keyDown.down) {
                                usePhysicalControls = true;
                            }
                        }
                    }
                }
            }
            break;
        }
        case 2: { // fade out
            if (entity->arrowAlpha > 0)
                entity->arrowAlpha -= 8;

            if (entity->timer < 0.2)
                entity->scale = fmaxf(entity->scale + ((1.5f - entity->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);
            else
                entity->scale = fmaxf(entity->scale + ((-1.0f - entity->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrixTemp, 0.0, 0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrixTemp);
            SetRenderMatrix(&entity->renderMatrix);

            memcpy(&entity->label->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 5; ++i) memcpy(&entity->displays[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));

            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.5) {
                // mainMenu->state = 4;
                RemoveNativeObject(entity->label);
                for (int i = 0; i < 5; ++i) RemoveNativeObject(entity->displays[i]);
                RemoveNativeObject(entity);
                return;
            }
            break;
        }
    }

    RenderMesh(entity->meshPanel, MESH_COLOURS, false);
    NewRenderState();
    SetRenderMatrix(NULL);

    if (entity->backPressed)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, entity->arrowAlpha, entity->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, entity->arrowAlpha, entity->textureArrows);
}
#endif
