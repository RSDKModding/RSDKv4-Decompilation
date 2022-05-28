#include "RetroEngine.hpp"

#if !RETRO_USE_ORIGINAL_CODE
void AchievementsMenu_Create(void *objPtr)
{
    RSDK_THIS(AchievementsMenu);

    self->label                  = CREATE_ENTITY(TextLabel);
    self->label->useRenderMatrix = true;
    self->label->fontID          = FONT_HEADING;
    self->label->scale           = 0.2;
    self->label->alpha           = 256;
    self->label->x               = -144.0;
    self->label->y               = 100.0;
    self->label->z               = 16.0;
    self->label->state           = TEXTLABEL_STATE_IDLE;
    SetStringToFont(self->label->text, strAchievements, FONT_HEADING);

    self->scale      = 0;
    self->arrowAlpha = 0;

    self->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", -1);
    SetMeshVertexColors(self->meshPanel, 0, 0, 0, 0xC0);
    self->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);

    char buffer[0x40];
    char buffer2[0x80];

    float y = 24.0f;
    for (int i = 0; i < 5; ++i) {
        self->displays[i]                  = CREATE_ENTITY(AchievementDisplay);
        self->displays[i]->fontID          = FONT_LABEL;
        self->displays[i]->x               = -112.0f;
        self->displays[i]->y               = y;
        self->displays[i]->scale           = 0.11;
        self->displays[i]->state           = achievements[i].status == 100;
        self->displays[i]->useRenderMatrix = true;

        StringUpperCase(buffer, achievements[i].name);
        StringUpperCase(buffer2, achievements[i].desc);
        SetStringToFont8(self->displays[i]->achievementText, buffer, self->displays[i]->fontID);
        SetStringToFont8(self->displays[i]->descriptionText, buffer2, self->displays[i]->fontID);

        y -= 96.0f;
    }
}
void AchievementsMenu_Main(void *objPtr)
{
    RSDK_THIS(AchievementsMenu);

    switch (self->state) {
        case 0: {
            if (self->arrowAlpha < 0x100)
                self->arrowAlpha += 8;

            self->scale = fminf(self->scale + ((1.05 - self->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            MatrixScaleXYZF(&self->renderMatrix, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, 0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);

            memcpy(&self->label->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 5; ++i) memcpy(&self->displays[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                self->arrowAlpha = 0x100;
                self->timer      = 0.0;
                self->state      = 1;
            }
            break;
        }
        case 1: {
            CheckKeyDown(&inputDown);
            CheckKeyPress(&inputPress);
            SetRenderMatrix(&self->renderMatrix);

            if (usePhysicalControls) {
                if (touches > 0) {
                    usePhysicalControls = false;
                }
                else {
                    if (inputPress.up) {
                    }
                    else if (inputPress.down) {
                    }

                    if (inputPress.B) {
                        PlaySfxByName("Menu Back", false);
                        self->backPressed = false;
                        self->state       = ABOUT_STATE_EXIT;
                    }
                }
            }
            else {
                if (touches > 0) {
                    self->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                }
                else {
                    if (inputPress.B || self->backPressed) {
                        PlaySfxByName("Menu Back", false);
                        self->backPressed = false;
                        self->state       = ABOUT_STATE_EXIT;
                    }
                    else {
                        if (self->state == ABOUT_STATE_MAIN) {
                            if (inputDown.up) {
                                usePhysicalControls = true;
                            }
                            if (inputDown.down) {
                                usePhysicalControls = true;
                            }
                        }
                    }
                }
            }
            break;
        }
        case 2: { // fade out
            if (self->arrowAlpha > 0)
                self->arrowAlpha -= 8;

            if (self->timer < 0.2)
                self->scale = fmaxf(self->scale + ((1.5f - self->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);
            else
                self->scale = fmaxf(self->scale + ((-1.0f - self->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);

            NewRenderState();
            MatrixScaleXYZF(&self->renderMatrix, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, 0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);

            memcpy(&self->label->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < 5; ++i) memcpy(&self->displays[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                // mainMenu->state = 4;
                RemoveNativeObject(self->label);
                for (int i = 0; i < 5; ++i) RemoveNativeObject(self->displays[i]);
                RemoveNativeObject(self);
                return;
            }
            break;
        }
    }

    RenderMesh(self->meshPanel, MESH_COLORS, false);
    NewRenderState();
    SetRenderMatrix(NULL);

    if (self->backPressed)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, self->arrowAlpha, self->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, self->arrowAlpha, self->textureArrows);
}
#endif
