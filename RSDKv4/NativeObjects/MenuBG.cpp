#include "RetroEngine.hpp"

void MenuBG_Create(void *objPtr)
{
    RSDK_THIS(MenuBG);
    entity->textureID = LoadTexture("Data/Game/Menu/BG1.png", 1);
    entity->bgCircle1 = LoadMesh("Data/Game/Models/BGCircle1.bin", 255);
    entity->bgCircle2 = LoadMesh("Data/Game/Models/BGCircle2.bin", 255);
    entity->bgLines   = LoadMesh("Data/Game/Models/BGLines.bin", 255);
    SetMeshAnimation(entity->bgLines, &entity->animator, 0, 40u, 0.0);
    entity->animator.loopAnimation = 1;
    entity->fadeR          = -96;
    entity->fadeG          = -64;
    entity->fadeB          = -8;
    entity->isFading       = 1;
    SetMeshVertexColors(entity->bgCircle1, 0xE0, 0xD0, 0xC0, 0xFF);
    SetMeshVertexColors(entity->bgCircle2, 0xE0, 0xD0, 0xC0, 0xFF);
    SetMeshVertexColors(entity->bgLines, 0xE0, 0, 0, 0xFF);
    entity->Ztrans1      = -32.0;
    entity->Ztrans2      = -64.0;
    entity->ZtransRender = -128.0;
}
void MenuBG_Main(void *objPtr)
{
    RSDK_THIS(MenuBG);
    if (entity->isFading) {
        if (entity->isFading != 1)
            return;
        SetRenderBlendMode(0);
        entity->fadeTimer += Engine.deltaTime;
        if (entity->fadeTimer > 1.5)
            entity->isFading = false;
        fadeR = entity->fadeR;
        if (fadeR <= 0xF7u) {
            fadeR += 8;
            entity->fadeR = fadeR;
        }
        fadeG = entity->fadeG;
        if (fadeG <= 0xF7u) {
            fadeG += 8;
            entity->fadeG = fadeG;
        }
        fadeB = entity->fadeB;
        if (fadeB <= 0xF7u) {
            fadeB += 8;
            entity->fadeB = fadeB;
        }
        if (entity->alpha <= 0xFF)
            entity->alpha += 0x10;
        entity->Ztrans1      = ((160.0 - entity->Ztrans1) / (16.0 * Engine.deltaTime * 60.0)) + entity->Ztrans1;
        entity->Ztrans2      = ((160.0 - entity->Ztrans2) / (18.0 * Engine.deltaTime * 60.0)) + entity->Ztrans2;
        entity->ZtransRender = ((160.0 - entity->ZtransRender) / (Engine.deltaTime * 60.0 * 20.0)) + entity->ZtransRender;
        RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, fadeR, fadeG, fadeB, 255);
        SetRenderBlendMode(1);
        SetRenderVertexColor(224, 208, 192);
        RenderImage(-64.0, 0.0, 160.0, 0.44999999, 0.44999999, 256.0, 256.0, 512.0, 512.0, 0.0, 0.0, entity->alpha, entity->textureID);
        SetRenderVertexColor(255, 255, 255);
        SetRenderBlendMode(0);
        entity->circle1Rot = entity->circle1Rot - Engine.deltaTime;
        if (entity->circle1Rot < M_PI * -2)
            entity->circle1Rot += M_PI * 2;
        entity->circle2Rot += Engine.deltaTime;
        if (entity->circle2Rot > M_PI * 2)
            entity->circle2Rot += M_PI * -2;
        NewRenderState();
        matrixRotateZF(&entity->circle1, entity->circle1Rot);
        matrixTranslateXYZF(&entity->matrix2, 120.0, 94.0, entity->Ztrans1);
        matrixMultiplyF(&entity->circle1, &entity->matrix2);
        SetRenderMatrix(&entity->circle1);
        RenderMesh(entity->bgCircle1, 0, 0);
        matrixRotateZF(&entity->circle2, entity->circle2Rot);
        matrixTranslateXYZF(&entity->matrix2, 4.0, 150.0, entity->Ztrans2);
        matrixMultiplyF(&entity->circle2, &entity->matrix2);
        SetRenderMatrix(&entity->circle2);
        RenderMesh(entity->bgCircle2, 0, 0);
        matrixTranslateXYZF(&entity->renderMatrix, 0.0, 0.0, entity->ZtransRender);
    }
    else {
        SetRenderBlendMode(0);
        RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 248, 248, 248, 255);
        SetRenderBlendMode(1);
        SetRenderVertexColor(0xE0, 0xD0, 0xC0);
        RenderImage(-64.0, 0.0, 160.0, 0.44999999, 0.44999999, 256.0, 256.0, 512.0, 512.0, 0.0, 0.0, 255, entity->textureID);
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);
        SetRenderBlendMode(0);
        entity->circle1Rot = entity->circle1Rot - Engine.deltaTime;
        if (entity->circle1Rot < M_PI * -2)
            entity->circle1Rot += M_PI * 2;
        entity->circle2Rot += Engine.deltaTime;
        if (entity->circle2Rot > M_PI * 2)
            entity->circle2Rot += M_PI * -2;
        NewRenderState();
        matrixRotateZF(&entity->circle1, entity->circle1Rot);
        matrixTranslateXYZF(&entity->matrix2, 120.0, 94.0, 160.0);
        matrixMultiplyF(&entity->circle1, &entity->matrix2);
        SetRenderMatrix(&entity->circle1);
        RenderMesh(entity->bgCircle1, 0, 0);
        matrixRotateZF(&entity->circle2, entity->circle2Rot);
        matrixTranslateXYZF(&entity->matrix2, 4.0, 150.0, 160.0);
        matrixMultiplyF(&entity->circle2, &entity->matrix2);
        SetRenderMatrix(&entity->circle2);
        RenderMesh(entity->bgCircle2, 0, 0);
        matrixTranslateXYZF(&entity->renderMatrix, 0.0, 0.0, 160.0);
    }
    SetRenderMatrix(&entity->renderMatrix);
    entity->animator.animationSpeed = 8.0 * Engine.deltaTime;
    AnimateMesh(entity->bgLines, &entity->animator);
    RenderMesh(entity->bgLines, 0, 0);
    SetRenderMatrix(0);
}
