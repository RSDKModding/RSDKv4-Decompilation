#include "RetroEngine.hpp"
#include <string>

void AboutScreen_Create(void *objPtr)
{
    RSDK_THIS(AboutScreen);

    self->label                  = CREATE_ENTITY(TextLabel);
    self->label->useRenderMatrix = true;
    self->label->fontID          = FONT_HEADING;
    self->label->scale           = 0.2;
    self->label->alpha           = 256;
    self->label->x               = -144.0;
    self->label->y               = 100.0;
    self->label->z               = 16.0;
    self->label->state           = TEXTLABEL_STATE_IDLE;
    SetStringToFont(self->label->text, strAbout, FONT_HEADING);

    char title[0x40];
    for (int i = 0; i < StrLength(Engine.gameWindowText); ++i) title[i] = toupper(Engine.gameWindowText[i]);
    SetStringToFont8(self->gameTitle, title, FONT_LABEL);
    SetStringToFont(self->versionNameText, strVersionName, FONT_LABEL);
    SetStringToFont8(self->versionText, Engine.gameVersion, FONT_LABEL);

    self->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", -1);
    SetMeshVertexColors(self->meshPanel, 0, 0, 0, 0xC0);
    self->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);

    int package = 0;
    switch (Engine.globalBoxRegion) {
        case REGION_JP:
            package       = LoadTexture("Data/Game/Models/Package_JP.png", TEXFMT_RGBA5551);
            self->meshBox = LoadMesh("Data/Game/Models/JPBox.bin", package);
            break;
        case REGION_US:
            package       = LoadTexture("Data/Game/Models/Package_US.png", TEXFMT_RGBA5551);
            self->meshBox = LoadMesh("Data/Game/Models/Box.bin", package);
            break;
        case REGION_EU:
            package       = LoadTexture("Data/Game/Models/Package_EU.png", TEXFMT_RGBA5551);
            self->meshBox = LoadMesh("Data/Game/Models/Box.bin", package);
            break;
        default: break;
    }

    SetMeshAnimation(self->meshBox, &self->animator, 16, 16, 0.0);
    AnimateMesh(self->meshBox, &self->animator);

    float y = -24.0f;
    for (int i = 0; i < ABOUT_BTN_COUNT; ++i) {
        self->buttons[i]                  = CREATE_ENTITY(PushButton);
        self->buttons[i]->useRenderMatrix = true;
        self->buttons[i]->x               = 64.0;
        self->buttons[i]->y               = y;
        self->buttons[i]->z               = 0.0;
        self->buttons[i]->scale           = 0.175;
        self->buttons[i]->bgColor         = 0x00A048;
        self->buttons[i]->bgColorSelected = 0x00C060;

        y -= 32.0f;
    }

    SetStringToFont(self->buttons[ABOUT_BTN_PRIVACY]->text, strPrivacy, FONT_LABEL);
    SetStringToFont(self->buttons[ABOUT_BTN_TERMS]->text, strTerms, FONT_LABEL);
}
void AboutScreen_Main(void *objPtr)
{
    RSDK_THIS(AboutScreen);
    NativeEntity_OptionsMenu *optionsMenu = (NativeEntity_OptionsMenu *)self->optionsMenu;

    switch (self->state) {
        case ABOUT_STATE_ENTER: {
            if (self->arrowAlpha < 0x100)
                self->arrowAlpha += 8;

            float maxWidth = 0;
            for (int i = 0; i < ABOUT_BTN_COUNT; ++i) {
                if (self->buttons[i]->textWidth > maxWidth)
                    maxWidth = self->buttons[i]->textWidth;
            }
            for (int i = 0; i < ABOUT_BTN_COUNT; ++i) {
                self->buttons[i]->textWidth = maxWidth;
            }

            self->scale = fminf(self->scale + ((1.05 - self->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            MatrixScaleXYZF(&self->renderMatrix, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);

            memcpy(&self->label->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < ABOUT_BTN_COUNT; ++i) memcpy(&self->buttons[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                self->arrowAlpha = 256;
                self->timer      = 0.0;
                self->state      = ABOUT_STATE_MAIN;
            }
            break;
        }
        case ABOUT_STATE_MAIN: {
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
                        if (self->selectedButton < 0)
                            self->selectedButton = 1;
                    }
                    else if (inputPress.down) {
                        PlaySfxByName("Menu Move", false);
                        self->selectedButton++;
                        if (self->selectedButton >= 2)
                            self->selectedButton = 0;
                    }

                    for (int i = 0; i < ABOUT_BTN_COUNT; ++i) self->buttons[i]->state = 0;
                    self->buttons[self->selectedButton]->state = 1;

                    if (inputPress.start || inputPress.A) {
                        PlaySfxByName("Menu Select", false);
                        self->buttons[self->selectedButton]->state = 2;
                        self->state                                = ABOUT_STATE_ACTION;
                    }
                    else if (inputPress.B) {
                        PlaySfxByName("Menu Back", false);
                        self->backPressed = false;
                        self->state       = ABOUT_STATE_EXIT;
                    }
                }
            }
            else {
                if (touches > 0) {
                    float y = -32.0f;
                    for (int i = 0; i < ABOUT_BTN_COUNT; ++i) {
                        bool valid = CheckTouchRect(64.0, y, ((64.0 * self->buttons[i]->scale) + self->buttons[i]->textWidth) * 0.75, 12.0) >= 0;
                        self->buttons[i]->state = valid;

                        y -= 32.0f;
                    }
                    self->backPressed = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
                    if (self->state == ABOUT_STATE_MAIN) {
                        if (inputDown.up) {
                            self->selectedButton = 1;
                            usePhysicalControls  = true;
                        }
                        if (inputDown.down) {
                            self->selectedButton = 0;
                            usePhysicalControls  = true;
                        }
                    }
                }
                else {
                    for (int i = 0; i < ABOUT_BTN_COUNT; ++i) {
                        if (self->buttons[i]->state == 1) {
                            PlaySfxByName("Menu Select", false);
                            self->buttons[i]->state = 2;
                            self->selectedButton    = i;
                            self->state             = ABOUT_STATE_ACTION;
                            break;
                        }
                    }

                    if (inputPress.B || self->backPressed) {
                        PlaySfxByName("Menu Back", false);
                        self->backPressed = false;
                        self->state       = ABOUT_STATE_EXIT;
                    }
                    else {
                        if (self->state == ABOUT_STATE_MAIN) {
                            if (inputDown.up) {
                                self->selectedButton = 1;
                                usePhysicalControls  = true;
                            }
                            if (inputDown.down) {
                                self->selectedButton = 0;
                                usePhysicalControls  = true;
                            }
                        }
                    }
                }
            }
            break;
        }
        case ABOUT_STATE_ACTION: {
            CheckKeyDown(&inputDown);
            SetRenderMatrix(&self->renderMatrix);

            if (self->buttons[self->selectedButton]->state) {
                switch (self->selectedButton) {
                    default: break;
                    case ABOUT_BTN_PRIVACY: ShowWebsite(0); break;
                    case ABOUT_BTN_TERMS: ShowWebsite(1); break;
                }
                self->state = ABOUT_STATE_MAIN;
            }
            break;
        }
        case ABOUT_STATE_EXIT: {
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

            memcpy(&self->label->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < ABOUT_BTN_COUNT; ++i) memcpy(&self->buttons[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                optionsMenu->state = OPTIONSMENU_STATE_EXITSUBMENU;
                RemoveNativeObject(self->label);
                for (int i = 0; i < ABOUT_BTN_COUNT; ++i) RemoveNativeObject(self->buttons[i]);
                RemoveNativeObject(self);
                return;
            }
            break;
        }
    }

    RenderMesh(self->meshPanel, MESH_COLORS, false);
    RenderText(self->gameTitle, FONT_LABEL, 24.0, 56.0, 0.0, 0.125, 255);
    RenderText(self->versionNameText, FONT_LABEL, 24.0, 32.0, 0.0, 0.125, 255);
    RenderText(self->versionText, FONT_LABEL, 24.0, 8.0, 0.0, 0.125, 255);
    NewRenderState();

    self->rotationY -= Engine.deltaTime;
    if (self->rotationY < -(M_PI_2))
        self->rotationY += (M_PI_2);

    MatrixScaleXYZF(&self->renderMatrix2, 0.6 * self->scale, 0.6 * self->scale, 0.6 * self->scale);
    MatrixRotateYF(&self->matrixTemp, self->rotationY);
    MatrixMultiplyF(&self->renderMatrix2, &self->matrixTemp);
    MatrixTranslateXYZF(&self->matrixTemp, -56.0, -8.0, 160.0);
    MatrixMultiplyF(&self->renderMatrix2, &self->matrixTemp);
    SetRenderMatrix(&self->renderMatrix2);
    RenderMesh(self->meshBox, MESH_NORMALS, true);
    SetRenderMatrix(NULL);

    if (self->backPressed)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, self->arrowAlpha, self->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, self->arrowAlpha, self->textureArrows);
}
