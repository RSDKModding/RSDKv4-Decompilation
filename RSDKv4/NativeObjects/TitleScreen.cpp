#include "RetroEngine.hpp"

void TitleScreen_Create(void *objPtr)
{
    RSDK_THIS(TitleScreen);
    int heading  = 0;
    int labelTex = 0;
    int textTex  = 0;

    self->state          = TITLESCREEN_STATE_SETUP;
    self->introRectAlpha = 320.0;
    self->logoTextureID  = LoadTexture("Data/Game/Menu/SonicLogo.png", TEXFMT_RGBA8888);

    // code has been moved to SegaSplash_Create due to the possibility of opening the dev menu before this loads :(
#if RETRO_USE_ORIGINAL_CODE
    ResetBitmapFonts();
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
#endif

    self->labelPtr         = CREATE_ENTITY(TextLabel);
    self->labelPtr->fontID = FONT_HEADING;
    self->labelPtr->scale  = 0.15;

    switch (Engine.language) {
        case RETRO_ES: self->labelPtr->scale = 0.125; break;

        case RETRO_JP: self->labelPtr->scale = 0.1;
#if RETRO_USE_ORIGINAL_CODE
            heading = LoadTexture("Data/Game/Menu/Heading_JA@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Heading_JA.fnt", FONT_HEADING, heading);

            labelTex = LoadTexture("Data/Game/Menu/Label_JA@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Label_JA.fnt", FONT_LABEL, labelTex);

            textTex = LoadTexture("Data/Game/Menu/Text_JA@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Text_JA.fnt", FONT_TEXT, textTex);
#endif
            break;

        case RETRO_RU: self->labelPtr->scale = 0.08;
#if RETRO_USE_ORIGINAL_CODE
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
#endif
            break;

        case RETRO_KO:
#if RETRO_USE_ORIGINAL_CODE
            heading = LoadTexture("Data/Game/Menu/Heading_KO@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Heading_KO.fnt", FONT_HEADING, heading);

            labelTex = LoadTexture("Data/Game/Menu/Label_KO@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Label_KO.fnt", FONT_LABEL, labelTex);

            textTex = LoadTexture("Data/Game/Menu/Text_KO.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Text_KO.fnt", FONT_TEXT, textTex);
#endif
            break;

        case RETRO_ZH:
#if RETRO_USE_ORIGINAL_CODE
            heading = LoadTexture("Data/Game/Menu/Heading_ZH@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Heading_ZH.fnt", FONT_HEADING, heading);

            labelTex = LoadTexture("Data/Game/Menu/Label_ZH@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Label_ZH.fnt", FONT_LABEL, labelTex);

            textTex = LoadTexture("Data/Game/Menu/Text_ZH@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Text_ZH.fnt", FONT_TEXT, textTex);
#endif
            break;

        case RETRO_ZS:
#if RETRO_USE_ORIGINAL_CODE
            heading = LoadTexture("Data/Game/Menu/Heading_ZHS@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Heading_ZHS.fnt", FONT_HEADING, heading);
            labelTex = LoadTexture("Data/Game/Menu/Label_ZHS@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Label_ZHS.fnt", FONT_LABEL, labelTex);
            textTex = LoadTexture("Data/Game/Menu/Text_ZHS@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Text_ZHS.fnt", FONT_TEXT, textTex);
#endif
            break;

        default: break;
    }
    self->labelPtr->alpha = 0;
    self->labelPtr->state = TEXTLABEL_STATE_NONE;

    if (Engine.gameDeviceType == RETRO_MOBILE)
        SetStringToFont(self->labelPtr->text, strTouchToStart, FONT_HEADING);
    else
        SetStringToFont(self->labelPtr->text, strPressStart, FONT_HEADING);

    self->labelPtr->alignPtr(self->labelPtr, ALIGN_CENTER);

    self->labelPtr->x    = 64.0;
    self->labelPtr->y    = -96.0;
    self->introTextureID = LoadTexture("Data/Game/Menu/Intro.png", TEXFMT_RGBA5551);

    int package = 0;
    switch (Engine.globalBoxRegion) {
        case REGION_JP:
            package         = LoadTexture("Data/Game/Models/Package_JP.png", TEXFMT_RGBA5551);
            self->introMesh = LoadMesh("Data/Game/Models/Intro.bin", self->introTextureID);
            self->boxMesh   = LoadMesh("Data/Game/Models/JPBox.bin", package);
            self->cartMesh  = LoadMesh("Data/Game/Models/JPCartridge.bin", package);
            break;
        case REGION_US:
            package         = LoadTexture("Data/Game/Models/Package_US.png", TEXFMT_RGBA5551);
            self->introMesh = LoadMesh("Data/Game/Models/Intro.bin", self->introTextureID);
            self->boxMesh   = LoadMesh("Data/Game/Models/Box.bin", package);
            self->cartMesh  = LoadMesh("Data/Game/Models/Cartridge.bin", package);
            break;
        case REGION_EU:
            package         = LoadTexture("Data/Game/Models/Package_EU.png", TEXFMT_RGBA5551);
            self->introMesh = LoadMesh("Data/Game/Models/Intro.bin", self->introTextureID);
            self->boxMesh   = LoadMesh("Data/Game/Models/Box.bin", package);
            self->cartMesh  = LoadMesh("Data/Game/Models/Cartridge.bin", package);
            break;
    }

    SetMeshAnimation(self->boxMesh, &self->meshAnimator, 16, 16, 0.0);
    AnimateMesh(self->boxMesh, &self->meshAnimator);
    SetMeshAnimation(self->introMesh, &self->meshAnimator, 0, 36, 0.09);
    self->rectY     = 160.0;
    self->meshScale = 0.0;
    self->rotationY = 0.0;
    SetMusicTrack("MenuIntro.ogg", 0, false, 0);
    SetMusicTrack("MainMenu.ogg", 1, true, 106596);
    LoadTexture("Data/Game/Menu/Circle.png", TEXFMT_RGBA4444);
    LoadTexture("Data/Game/Menu/BG1.png", TEXFMT_RGBA4444);
    LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);
    if (Engine.gameDeviceType == RETRO_MOBILE)
        LoadTexture("Data/Game/Menu/VirtualDPad.png", TEXFMT_RGBA8888);
    else
        LoadTexture("Data/Game/Menu/Generic.png", TEXFMT_RGBA8888);
    LoadTexture("Data/Game/Menu/PlayerSelect.png", TEXFMT_RGBA8888);
    LoadTexture("Data/Game/Menu/SegaID.png", TEXFMT_RGBA8888);
}
void TitleScreen_Main(void *objPtr)
{
    RSDK_THIS(TitleScreen);

    switch (self->state) {
        case TITLESCREEN_STATE_SETUP: {
            PlayMusic(0, 0);
            self->state = TITLESCREEN_STATE_ENTERINTRO;
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, self->introRectAlpha);
            break;
        }

        case TITLESCREEN_STATE_ENTERINTRO: {
            SetRenderBlendMode(RENDER_BLEND_NONE);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 255, 255, 255, 255);
            self->meshAnimator.animationSpeed = 6.0 * Engine.deltaTime;
            AnimateMesh(self->introMesh, &self->meshAnimator);
            RenderMesh(self->introMesh, MESH_COLORS, true);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);

            if (Engine.gameDeviceType == RETRO_MOBILE && self->skipButtonAlpha < 0x100 && self->introRectAlpha < 0.0) {
                self->skipButtonAlpha += 8;
            }
            RenderImage(SCREEN_CENTERX_F - 32.0, 104.0, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 704.0, 544.0, self->skipButtonAlpha,
                        self->introTextureID);
            self->introRectAlpha -= (300.0 * Engine.deltaTime);
            if (self->introRectAlpha < -320.0)
                self->state = TITLESCREEN_STATE_INTRO;
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, self->introRectAlpha);
            CheckKeyDown(&inputDown);
            CheckKeyPress(&inputPress);

            if (CheckTouchRect(SCREEN_CENTERX_F - 32.0, 104.0, 20.0, 20.0) >= 0 || (inputPress.start || inputPress.A)) {
                self->state                   = TITLESCREEN_STATE_TITLE;
                self->x                       = -96.0;
                self->meshScale               = 1.0;
                self->rectY                   = -48.0;
                self->fadeRectAlpha           = 256;
                self->logoAlpha               = 256;
                self->selectionDisabled       = 1;
                NativeEntity_TextLabel *label = self->labelPtr;
                label->alpha                  = 256;
                label->state                  = TEXTLABEL_STATE_BLINK;
            }
            break;
        }

        case TITLESCREEN_STATE_INTRO: {
            CheckKeyDown(&inputDown);
            CheckKeyPress(&inputPress);
            SetRenderBlendMode(RENDER_BLEND_NONE);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 255, 255, 255, 255);
            self->meshAnimator.animationSpeed = 6.0 * Engine.deltaTime;
            AnimateMesh(self->introMesh, &self->meshAnimator);
            RenderMesh(self->introMesh, MESH_COLORS, true);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImage(SCREEN_CENTERX_F - 32.0, 104.0, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 704.0, 544.0, self->skipButtonAlpha,
                        self->introTextureID);
            if (self->meshAnimator.frameID > 26)
                self->state = TITLESCREEN_STATE_ENTERBOX;

            if (CheckTouchRect(SCREEN_CENTERX_F - 32.0, 104.0, 20.0, 20.0) >= 0 || (inputPress.start || inputPress.A)) {
                self->state             = TITLESCREEN_STATE_TITLE;
                self->x                 = -96.0;
                self->meshScale         = 1.0;
                self->rectY             = -48.0;
                self->fadeRectAlpha     = 0x100;
                self->logoAlpha         = 0x100;
                self->selectionDisabled = true;

                NativeEntity_TextLabel *label = self->labelPtr;
                label->alpha                  = 256;
                label->state                  = TEXTLABEL_STATE_BLINK;
            }
            break;
        }

        case TITLESCREEN_STATE_ENTERBOX: {
            SetRenderBlendMode(RENDER_BLEND_NONE);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 255, 255, 255, 255);

            float y = 0;
            if (self->rectY > -48.0) {
                self->rectY -= (300.0 * Engine.deltaTime);
                if (self->rectY >= -48.0) {
                    y = self->rectY + 240.0;
                }
                else {
                    self->rectY = -48.0;
                    y           = 192.0;
                }
            }
            else {
                y = self->rectY + 240.0;
            }
            RenderRect(-SCREEN_CENTERX_F, y, 160.0, SCREEN_XSIZE_F, 256.0, 160, 192, 255, 255);
            RenderRect(-SCREEN_CENTERX_F, self->rectY, 160.0, SCREEN_XSIZE_F, 16.0, 0, 0, 0, 255);
            self->meshAnimator.animationSpeed = 6.0 * Engine.deltaTime;
            AnimateMesh(self->introMesh, &self->meshAnimator);
            RenderMesh(self->introMesh, MESH_COLORS, true);

            if (self->meshScale < 1.0) {
                self->meshScale += (0.75 * Engine.deltaTime);
                if (self->meshScale > 1.0)
                    self->meshScale = 1.0;
            }
            else {
                NativeEntity_TextLabel *label = self->labelPtr;
                label->state                  = TEXTLABEL_STATE_BLINK;
                self->state                   = TITLESCREEN_STATE_TITLE;
                self->x                       = 0.0;
            }
            self->rotationY += Engine.deltaTime;
            MatrixScaleXYZF(&self->renderMatrix, self->meshScale, self->meshScale, self->meshScale);
            MatrixRotateYF(&self->matrixTemp, self->rotationY);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, 0.0, 200.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);
            RenderMesh(self->boxMesh, MESH_NORMALS, true);
            SetRenderMatrix(NULL);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImage(SCREEN_CENTERX_F - 32.0, 104.0, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 704.0, 544.0, self->skipButtonAlpha,
                        self->introTextureID);
            break;
        }

        case TITLESCREEN_STATE_TITLE: {
            SetRenderBlendMode(RENDER_BLEND_NONE);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 255, 255, 255, 255);
            RenderRect(-SCREEN_CENTERX_F, self->rectY + 240.0, 160.0, SCREEN_XSIZE_F, 256.0, 160, 192, 255, 255);
            RenderRect(-SCREEN_CENTERX_F, self->rectY, 160.0, SCREEN_XSIZE_F, 16.0, 0, 0, 0, 255);

            self->rotationY += Engine.deltaTime;
            if (self->rotationY > M_PI_2)
                self->rotationY -= M_PI_2;

            if (self->x <= -96.0) {
                if (self->logoAlpha > 255) {
                    CheckKeyDown(&inputDown);
                    CheckKeyPress(&inputPress);
                    if (inputPress.start || touches > 0 || inputPress.A) {
                        if (!self->selectionDisabled) {
                            PlaySfxByName("Menu Select", false);
                            StopMusic(true);
                            self->labelPtr->state = TEXTLABEL_STATE_BLINK_FAST;
                            self->introRectAlpha  = 0.0;
                            self->state           = TITLESCREEN_STATE_EXITTITLE;
                        }
                    }
                    else {
                        self->selectionDisabled = false;
                    }
                }
                else {
                    self->logoAlpha += 8;
                    self->labelPtr->alpha += 8;
                }
            }
            else {
                self->x += ((-97.0 - self->x) / ((Engine.deltaTime * 60.0) * 16.0));
            }

            NewRenderState();
            MatrixScaleXYZF(&self->renderMatrix, self->meshScale, self->meshScale, self->meshScale);
            MatrixRotateYF(&self->matrixTemp, self->rotationY);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            MatrixTranslateXYZF(&self->matrixTemp, self->x, 0.0, 200.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);
            RenderMesh(self->boxMesh, MESH_NORMALS, true);
            SetRenderMatrix(NULL);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);

            if (self->skipButtonAlpha > 0) {
                self->skipButtonAlpha -= 8;
            }
            RenderImage(SCREEN_CENTERX_F - 32.0, 104.0, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 704.0, 544.0, self->skipButtonAlpha,
                        self->introTextureID);
            RenderImage(64.0, 32.0, 160.0, 0.3, 0.3, 256.0, 128.0, 512.0, 256.0, 0.0, 0.0, self->logoAlpha, self->logoTextureID);

            if (self->fadeRectAlpha > 0) {
                self->fadeRectAlpha -= 32;
                RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 255, 255, 255, self->fadeRectAlpha);
            }
            break;
        }

        case TITLESCREEN_STATE_EXITTITLE: {
            SetRenderBlendMode(RENDER_BLEND_NONE);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 255, 255, 255, 255);
            RenderRect(-SCREEN_CENTERX_F, self->rectY + 240.0, 160.0, SCREEN_XSIZE_F, 256.0, 160, 192, 255, 255);
            RenderRect(-SCREEN_CENTERX_F, self->rectY, 160.0, SCREEN_XSIZE_F, 16.0, 0, 0, 0, 255);

            float speed = (60.0 * Engine.deltaTime) * 1.125;
            self->x /= speed;
            self->rotationY /= speed;
            NewRenderState();
            MatrixRotateYF(&self->renderMatrix, self->rotationY);
            MatrixTranslateXYZF(&self->matrixTemp, self->x, 0.0, 200.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);
            RenderMesh(self->boxMesh, MESH_NORMALS, true);
            SetRenderMatrix(NULL);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);

            if (self->logoAlpha > 0)
                self->logoAlpha -= 8;

            self->introRectAlpha += Engine.deltaTime;
            if (self->introRectAlpha > 1.0) {
                self->state                   = TITLESCREEN_STATE_EXIT;
                NativeEntity_TextLabel *label = self->labelPtr;
                RemoveNativeObject(label);
                SetMeshAnimation(self->boxMesh, &self->meshAnimator, 4, 16, 0.0);
                self->meshAnimator.animationTimer = 0.0;
                self->meshAnimator.frameID        = 16;
                self->matrixZ                     = 200.0;
                self->rectYVelocity               = 4.0;
                self->rotationZ                   = DegreesToRad(-90.0);
            }

            RenderImage(64.0, 32.0, 160.0, 0.3, 0.3, 256.0, 128.0, 512.0, 256.0, 0.0, 0.0, self->logoAlpha, self->logoTextureID);
            break;
        }

        case TITLESCREEN_STATE_EXIT: {
            SetRenderBlendMode(RENDER_BLEND_NONE);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 255, 255, 255, 255);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_CENTERY_F - self->rectY, 160, 192, 255, 255);
            RenderRect(-SCREEN_CENTERX_F, self->rectY, 160.0, SCREEN_XSIZE_F, 16.0, 0, 0, 0, 255);
            self->meshAnimator.animationSpeed = -16.0 * Engine.deltaTime;
            AnimateMesh(self->boxMesh, &self->meshAnimator);

            float speed1 = 60.0 * Engine.deltaTime;
            float speed2 = 0.125 * speed1;

            self->rectYVelocity = (self->rectYVelocity - speed2) - speed2;
            self->rectY += (speed1 * (self->rectYVelocity - speed2));
            if (self->meshAnimator.frameID <= 7) {
                if (self->rotationY < 1.0)
                    self->rotationY += Engine.deltaTime;

                self->yVelocity = (self->yVelocity - speed2) - speed2;
                self->y += (speed1 * (self->yVelocity - speed2));

                self->matrixY += ((16.0 - self->matrixY) / (speed1 * 16.0));
                self->matrixZ += ((152.0 - self->matrixZ) / (speed1 * 16.0));
                self->rotationZ += ((0.0 - self->rotationZ) / (speed1 * 22.0));
            }
            NewRenderState();
            MatrixRotateXF(&self->renderMatrix, self->rotationY);
            MatrixTranslateXYZF(&self->matrixTemp, self->x, self->y, 200.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix);
            RenderMesh(self->boxMesh, MESH_NORMALS, true);

            MatrixRotateXYZF(&self->renderMatrix2, 0.0, 0.0, self->rotationZ);
            MatrixTranslateXYZF(&self->matrixTemp, 0.0, self->matrixY, self->matrixZ);
            MatrixMultiplyF(&self->renderMatrix2, &self->matrixTemp);
            SetRenderMatrix(&self->renderMatrix2);
            RenderMesh(self->cartMesh, MESH_NORMALS, true);
            SetRenderMatrix(NULL);

            if (self->y < -360.0) {
                ShowPromoPopup(0, "BootupPromo");
                ResetNativeObject(self, MenuControl_Create, MenuControl_Main);
            }
            break;
        }

        default: break;
    }
}
