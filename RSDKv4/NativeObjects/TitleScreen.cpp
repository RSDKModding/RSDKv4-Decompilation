#include "RetroEngine.hpp"

void TitleScreen_Create(void *objPtr)
{
    RSDK_THIS(TitleScreen);
    int heading  = 0;
    int labelTex = 0;
    int textTex  = 0;

    entity->state          = 0;
    entity->introRectAlpha = 320.0;
    entity->logoTextureID  = LoadTexture("Data/Game/Menu/SonicLogo.png", 3);
    ResetBitmapFonts();
    if (Engine.useHighResAssets)
        heading = LoadTexture("Data/Game/Menu/Heading_EN.png", 1);
    else
        heading = LoadTexture("Data/Game/Menu/Heading_EN@1x.png", 1);
    LoadBitmapFont("Data/Game/Menu/Heading_EN.fnt", 0, heading);

    if (Engine.useHighResAssets)
        labelTex = LoadTexture("Data/Game/Menu/Label_EN.png", 1);
    else
        labelTex = LoadTexture("Data/Game/Menu/Label_EN@1x.png", 1);
    LoadBitmapFont("Data/Game/Menu/Label_EN.fnt", 1, labelTex);

    textTex = LoadTexture("Data/Game/Menu/Text_EN.png", 1);
    LoadBitmapFont("Data/Game/Menu/Text_EN.fnt", 2, textTex);

    entity->labelPtr            = CREATE_ENTITY(TextLabel);
    entity->labelPtr->fontID    = 0;
    entity->labelPtr->textScale = 0.15;

    switch (Engine.language) {
        case RETRO_ES: entity->labelPtr->textScale = 0.125; break;
        case RETRO_JP:
            entity->labelPtr->textScale = 0.1;
            heading                     = LoadTexture("Data/Game/Menu/Heading_JA@1x.png", 1);
            LoadBitmapFont("Data/Game/Menu/Heading_JA.fnt", 0, heading);

            labelTex = LoadTexture("Data/Game/Menu/Label_JA@1x.png", 1);
            LoadBitmapFont("Data/Game/Menu/Label_JA.fnt", 1, labelTex);

            textTex = LoadTexture("Data/Game/Menu/Text_JA@1x.png", 1);
            LoadBitmapFont("Data/Game/Menu/Text_JA.fnt", 2, textTex);
            break;
        case 7:
            entity->labelPtr->textScale = 0.08;
            if (Engine.useHighResAssets)
                heading = LoadTexture("Data/Game/Menu/Heading_RU.png", 1);
            else
                heading = LoadTexture("Data/Game/Menu/Heading_RU@1x.png", 1);
            LoadBitmapFont("Data/Game/Menu/Heading_RU.fnt", 0, heading);

            if (Engine.useHighResAssets)
                labelTex = LoadTexture("Data/Game/Menu/Label_RU.png", 1);
            else
                labelTex = LoadTexture("Data/Game/Menu/Label_RU@1x.png", 1);
            LoadBitmapFont("Data/Game/Menu/Label_RU.fnt", 1, labelTex);
            break;
        case RETRO_KO:
            heading = LoadTexture("Data/Game/Menu/Heading_KO@1x.png", 1);
            LoadBitmapFont("Data/Game/Menu/Heading_KO.fnt", 0, heading);

            labelTex = LoadTexture("Data/Game/Menu/Label_KO@1x.png", 1);
            LoadBitmapFont("Data/Game/Menu/Label_KO.fnt", 1, labelTex);

            textTex = LoadTexture("Data/Game/Menu/Text_KO.png", 1);
            LoadBitmapFont("Data/Game/Menu/Text_KO.fnt", 2, textTex);
            break;
        case RETRO_ZH:
            heading = LoadTexture("Data/Game/Menu/Heading_ZH@1x.png", 1);
            LoadBitmapFont("Data/Game/Menu/Heading_ZH.fnt", 0, heading);

            labelTex = LoadTexture("Data/Game/Menu/Label_ZH@1x.png", 1);
            LoadBitmapFont("Data/Game/Menu/Label_ZH.fnt", 1, labelTex);

            textTex = LoadTexture("Data/Game/Menu/Text_ZH@1x.png", 1);
            LoadBitmapFont("Data/Game/Menu/Text_ZH.fnt", 2, textTex);
            break;
        case RETRO_ZS:
            heading = LoadTexture("Data/Game/Menu/Heading_ZHS@1x.png", 1);
            LoadBitmapFont("Data/Game/Menu/Heading_ZHS.fnt", 0, heading);
            labelTex = LoadTexture("Data/Game/Menu/Label_ZHS@1x.png", 1);
            LoadBitmapFont("Data/Game/Menu/Label_ZHS.fnt", 1, labelTex);
            textTex = LoadTexture("Data/Game/Menu/Text_ZHS@1x.png", 1);
            LoadBitmapFont("Data/Game/Menu/Text_ZHS.fnt", 2, textTex);
            break;
        default: break;
    }
    entity->labelPtr->textAlpha = 0;
    entity->labelPtr->alignment = -1;

    ushort *str = NULL;
    if (Engine.gameDeviceType == RETRO_MOBILE)
        SetStringToFont(entity->labelPtr->text, strTouchToStart, 0);
    else
        SetStringToFont(entity->labelPtr->text, strPressStart, 0);

    entity->labelPtr->alignPtr(entity->labelPtr, 1);

    entity->labelPtr->textX = 64.0;
    entity->labelPtr->textY = -96.0;
    entity->introTextureID  = LoadTexture("Data/Game/Menu/Intro.png", 2);

    int package = 0;
    switch (Engine.globalBoxRegion) {
        case 0:
            package           = LoadTexture("Data/Game/Models/Package_JP.png", 2);
            entity->introMesh = LoadMesh("Data/Game/Models/Intro.bin", entity->introTextureID);
            entity->boxMesh   = LoadMesh("Data/Game/Models/JPBox.bin", package);
            entity->cartMesh  = LoadMesh("Data/Game/Models/JPCartridge.bin", package);
            break;
        case 1:
            package           = LoadTexture("Data/Game/Models/Package_US.png", 2);
            entity->introMesh = LoadMesh("Data/Game/Models/Intro.bin", entity->introTextureID);
            entity->boxMesh   = LoadMesh("Data/Game/Models/Box.bin", package);
            entity->cartMesh  = LoadMesh("Data/Game/Models/Cartridge.bin", package);
            break;
        case 2:
            package           = LoadTexture("Data/Game/Models/Package_EU.png", 2);
            entity->introMesh = LoadMesh("Data/Game/Models/Intro.bin", entity->introTextureID);
            entity->boxMesh   = LoadMesh("Data/Game/Models/Box.bin", package);
            entity->cartMesh  = LoadMesh("Data/Game/Models/Cartridge.bin", package);
            break;
    }

    SetMeshAnimation(entity->boxMesh, &entity->meshAnimator, 16, 16, 0.0);
    AnimateMesh(entity->boxMesh, &entity->meshAnimator);
    SetMeshAnimation(entity->introMesh, &entity->meshAnimator, 0, 36, 0.09);
    entity->field_38  = 160.0;
    entity->meshScale = 0.0;
    entity->rotationY = 0.0;
    SetMusicTrack("MenuIntro.ogg", 0, 0, 0);
    SetMusicTrack("MainMenu.ogg", 1, 1, 106596);
    LoadTexture("Data/Game/Menu/Circle.png", 1);
    LoadTexture("Data/Game/Menu/BG1.png", 1);
    LoadTexture("Data/Game/Menu/ArrowButtons.png", 1);
    if (Engine.gameDeviceType == RETRO_MOBILE)
        LoadTexture("Data/Game/Menu/VirtualDPad.png", 3);
    else
        LoadTexture("Data/Game/Menu/Generic.png", 3);
    LoadTexture("Data/Game/Menu/PlayerSelect.png", 3);
    LoadTexture("Data/Game/Menu/SegaID.png", 3);
}
void TitleScreen_Main(void *objPtr)
{
    RSDK_THIS(TitleScreen);

    switch (entity->state) {
        case 0: {
            PlayMusic(0, 0);
            entity->state = 1;
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, entity->introRectAlpha);
            break;
        }
        case 1: {
            SetRenderBlendMode(RENDER_BLEND_NONE);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 255, 255, 255, 255);
            entity->meshAnimator.animationSpeed = 6.0 * Engine.deltaTime;
            AnimateMesh(entity->introMesh, &entity->meshAnimator);
            RenderMesh(entity->introMesh, 0, true);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);

            if (Engine.gameDeviceType == RETRO_MOBILE && entity->introAlpha < 0x100 && entity->introRectAlpha < 0.0) {
                entity->introAlpha += 8;
            }
            RenderImage(SCREEN_CENTERX_F - 32.0, 104.0, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 704.0, 544.0, entity->introAlpha,
                        entity->introTextureID);
            entity->introRectAlpha -= (300.0 * Engine.deltaTime);
            if (entity->introRectAlpha < -320.0)
                entity->state = 2;
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, entity->introRectAlpha);
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);

            if (touches > 0 && entity->introRectAlpha < 0.0) {
                if (CheckTouchRect(SCREEN_CENTERX_F - 32.0, 104.0, 20.0, 20.0) >= 0) {
                    entity->state                 = 4;
                    entity->translateX            = -96.0;
                    entity->meshScale             = 1.0;
                    entity->field_38              = -48.0;
                    entity->field_12C             = 256;
                    entity->logoAlpha             = 256;
                    entity->field_130             = 1;
                    NativeEntity_TextLabel *label = entity->labelPtr;
                    label->textAlpha              = 256;
                    label->alignment              = 1;
                }
            }
            else if (keyPress.start || keyPress.A) {
                entity->state                 = 4;
                entity->translateX            = -96.0;
                entity->meshScale             = 1.0;
                entity->field_38              = -48.0;
                entity->field_12C             = 256;
                entity->logoAlpha             = 256;
                entity->field_130             = 1;
                NativeEntity_TextLabel *label = entity->labelPtr;
                label->textAlpha              = 256;
                label->alignment              = 1;
            }
            break;
        }
        case 2: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderBlendMode(RENDER_BLEND_NONE);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 255, 255, 255, 255);
            entity->meshAnimator.animationSpeed = 6.0 * Engine.deltaTime;
            AnimateMesh(entity->introMesh, &entity->meshAnimator);
            RenderMesh(entity->introMesh, 0, true);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImage(SCREEN_CENTERX_F - 32.0, 104.0, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 704.0, 544.0, entity->introAlpha,
                        entity->introTextureID);
            if (entity->meshAnimator.frameID > 26)
                entity->state = 3;

            if (touches > 0) {
                if (CheckTouchRect(SCREEN_CENTERX_F - 32.0, 104.0, 20.0, 20.0) >= 0) {
                    entity->state                 = 4;
                    entity->translateX            = -96.0;
                    entity->meshScale             = 1.0;
                    entity->field_38              = -48.0;
                    entity->field_12C             = 256;
                    entity->logoAlpha             = 256;
                    entity->field_130             = 1;
                    NativeEntity_TextLabel *label = entity->labelPtr;
                    label->textAlpha              = 256;
                    label->alignment              = 1;
                }
            }
            else if (keyPress.start || keyPress.A) {
                entity->state                 = 4;
                entity->translateX            = -96.0;
                entity->meshScale             = 1.0;
                entity->field_38              = -48.0;
                entity->field_12C             = 256;
                entity->logoAlpha             = 256;
                entity->field_130             = 1;
                NativeEntity_TextLabel *label = entity->labelPtr;
                label->textAlpha              = 256;
                label->alignment              = 1;
            }
            break;
        }
        case 3: {
            SetRenderBlendMode(RENDER_BLEND_NONE);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 255, 255, 255, 255);

            float y = 0;
            if (entity->field_38 > -48.0) {
                entity->field_38 -= (300.0 * Engine.deltaTime);
                if (entity->field_38 >= -48.0) {
                    y = entity->field_38 + 240.0;
                }
                else {
                    entity->field_38 = -48.0;
                    y                = 192.0;
                }
            }
            else {
                y = entity->field_38 + 240.0;
            }
            RenderRect(-SCREEN_CENTERX_F, y, 160.0, SCREEN_XSIZE_F, 256.0, 160, 192, 255, 255);
            RenderRect(-SCREEN_CENTERX_F, entity->field_38, 160.0, SCREEN_XSIZE_F, 16.0, 0, 0, 0, 255);
            entity->meshAnimator.animationSpeed = 6.0 * Engine.deltaTime;
            AnimateMesh(entity->introMesh, &entity->meshAnimator);
            RenderMesh(entity->introMesh, 0, true);

            if (entity->meshScale < 1.0) {
                entity->meshScale += (0.75 * Engine.deltaTime);
                if (entity->meshScale > 1.0)
                    entity->meshScale = 1.0;
            }
            else {
                NativeEntity_TextLabel *label = entity->labelPtr;
                label->alignment              = 1;
                entity->state                 = 4;
                entity->translateX            = 0.0;
            }
            entity->rotationY += Engine.deltaTime;
            matrixScaleXYZF(&entity->renderMatrix, entity->meshScale, entity->meshScale, entity->meshScale);
            matrixRotateYF(&entity->matrix3, entity->rotationY);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix3);
            matrixTranslateXYZF(&entity->matrix3, 0.0, 0.0, 200.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix3);
            SetRenderMatrix(&entity->renderMatrix);
            RenderMesh(entity->boxMesh, 1, true);
            SetRenderMatrix(NULL);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImage(SCREEN_CENTERX_F - 32.0, 104.0, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 704.0, 544.0, entity->introAlpha,
                        entity->introTextureID);
            break;
        }
        case 4: {
            SetRenderBlendMode(RENDER_BLEND_NONE);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 255, 255, 255, 255);
            RenderRect(-SCREEN_CENTERX_F, entity->field_38 + 240.0, 160.0, SCREEN_XSIZE_F, 256.0, 160, 192, 255, 255);
            RenderRect(-SCREEN_CENTERX_F, entity->field_38, 160.0, SCREEN_XSIZE_F, 16.0, 0, 0, 0, 255);

            entity->rotationY += Engine.deltaTime;
            if (entity->rotationY > M_PI * 2)
                entity->rotationY -= M_PI * 2;

            if (entity->translateX <= -96.0) {
                if (entity->logoAlpha > 255) {
                    CheckKeyDown(&keyDown);
                    CheckKeyPress(&keyPress);
                    if (keyPress.start || touches > 0 || keyPress.A) {
                        if (!entity->field_130) {
                            PlaySfx(22, 0);
                            StopMusic(true);
                            entity->labelPtr->alignment = 2;
                            entity->introRectAlpha      = 0.0;
                            entity->state               = 5;
                        }
                    }
                    else {
                        entity->field_130 = 0;
                    }
                }
                else {
                    entity->logoAlpha += 8;
                    entity->labelPtr->textAlpha += 8;
                }
            }
            else {
                entity->translateX += ((-97.0 - entity->translateX) / ((Engine.deltaTime * 60.0) * 16.0));
            }
            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->meshScale, entity->meshScale, entity->meshScale);
            matrixRotateYF(&entity->matrix3, entity->rotationY);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix3);
            matrixTranslateXYZF(&entity->matrix3, entity->translateX, 0.0, 200.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix3);
            SetRenderMatrix(&entity->renderMatrix);
            RenderMesh(entity->boxMesh, 1, true);
            SetRenderMatrix(NULL);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);

            if (entity->introAlpha > 0) {
                entity->introAlpha -= 8;
            }
            RenderImage(SCREEN_CENTERX_F - 32.0, 104.0, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 704.0, 544.0, entity->introAlpha,
                        entity->introTextureID);
            RenderImage(64.0, 32.0, 160.0, 0.3, 0.3, 256.0, 128.0, 512.0, 256.0, 0.0, 0.0, entity->logoAlpha, entity->logoTextureID);

            if (entity->field_12C > 0) {
                entity->field_12C -= 32;
                RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 255, 255, 255, entity->field_12C);
            }
            break;
        }
        case 5: {
            SetRenderBlendMode(RENDER_BLEND_NONE);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 255, 255, 255, 255);
            RenderRect(-SCREEN_CENTERX_F, entity->field_38 + 240.0, 160.0, SCREEN_XSIZE_F, 256.0, 160, 192, 255, 255);
            RenderRect(-SCREEN_CENTERX_F, entity->field_38, 160.0, SCREEN_XSIZE_F, 16.0, 0, 0, 0, 255);

            float div = (60.0 * Engine.deltaTime) * 1.125;
            entity->translateX /= div;
            entity->rotationY /= div;
            NewRenderState();
            matrixRotateYF(&entity->renderMatrix, entity->rotationY);
            matrixTranslateXYZF(&entity->matrix3, entity->translateX, 0.0, 200.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix3);
            SetRenderMatrix(&entity->renderMatrix);
            RenderMesh(entity->boxMesh, 1, true);
            SetRenderMatrix(NULL);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);

            if (entity->logoAlpha > 0)
                entity->logoAlpha -= 8;

            entity->introRectAlpha += Engine.deltaTime;
            if (entity->introRectAlpha > 1.0) {
                entity->state                 = 6;
                NativeEntity_TextLabel *label = entity->labelPtr;
                RemoveNativeObject(label);
                SetMeshAnimation(entity->boxMesh, &entity->meshAnimator, 4, 16, 0.0);
                entity->meshAnimator.animationTimer = 0.0;
                entity->meshAnimator.frameID        = 16;
                entity->matrixZ                     = 200.0;
                entity->field_3C                    = 4.0;
                entity->rotationZ                   = DegreesToRad(-90.0);
            }
            RenderImage(64.0, 32.0, 160.0, 0.3, 0.31, 256.0, 128.0, 512.0, 256.0, 0.0, 0.0, entity->logoAlpha, entity->logoTextureID);
            break;
        }
        case 6: {
            SetRenderBlendMode(RENDER_BLEND_NONE);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 255, 255, 255, 255);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_CENTERY_F - entity->field_38, 160, 192, 255, 255);
            RenderRect(-SCREEN_CENTERX_F, entity->field_38, 160.0, SCREEN_XSIZE_F, 16.0, 0, 0, 0, 255);
            entity->meshAnimator.animationSpeed = -16.0 * Engine.deltaTime;
            AnimateMesh(entity->boxMesh, &entity->meshAnimator);

            float val  = 60.0 * Engine.deltaTime;
            float val2 = 0.125 * (60.0 * Engine.deltaTime);

            entity->field_3C = (entity->field_3C - val2) - val2;
            entity->field_38 += (val * (entity->field_3C - val2));
            if (entity->meshAnimator.frameID <= 7) {
                if (entity->rotationY < 1.0)
                    entity->rotationY += Engine.deltaTime;
                entity->field_50 = (entity->field_50 - val2) - val2;
                entity->field_4C += (val * (entity->field_50 - val2));
                entity->matrixY += ((16.0 - entity->matrixY) / (val * 16.0));

                entity->matrixZ += ((152.0 - entity->matrixZ) / (val * 16.0));
                entity->rotationZ += ((0.0 - entity->rotationZ) / (val * 22.0));
            }
            NewRenderState();
            matrixRotateXF(&entity->renderMatrix, entity->rotationY);
            matrixTranslateXYZF(&entity->matrix3, entity->translateX, entity->field_4C, 200.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix3);
            SetRenderMatrix(&entity->renderMatrix);
            RenderMesh(entity->boxMesh, 1, true);

            matrixRotateXYZF(&entity->matrix2, 0.0, 0.0, entity->rotationZ);
            matrixTranslateXYZF(&entity->matrix3, 0.0, entity->matrixY, entity->matrixZ);
            matrixMultiplyF(&entity->matrix2, &entity->matrix3);
            SetRenderMatrix(&entity->matrix2);
            RenderMesh(entity->cartMesh, 1, true);
            SetRenderMatrix(NULL);
            if (entity->field_4C < -360.0) {
                ShowPromoPopup(0, (void *)"BootupPromo");
                ResetNativeObject(entity, MenuControl_Create, MenuControl_Main);
            }
            break;
        }
        default: break;
    }
}
