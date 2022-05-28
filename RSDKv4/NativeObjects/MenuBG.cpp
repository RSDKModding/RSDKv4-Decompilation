#include "RetroEngine.hpp"

void MenuBG_Create(void *objPtr)
{
    RSDK_THIS(MenuBG);
    self->textureID = LoadTexture("Data/Game/Menu/BG1.png", TEXFMT_RGBA5551);
    self->bgCircle1 = LoadMesh("Data/Game/Models/BGCircle1.bin", -1);
    self->bgCircle2 = LoadMesh("Data/Game/Models/BGCircle2.bin", -1);
    self->bgLines   = LoadMesh("Data/Game/Models/BGLines.bin", -1);
    SetMeshAnimation(self->bgLines, &self->animator, 0, 40, 0.0);
    self->animator.loopAnimation = true;
    self->fadeR                  = -96;
    self->fadeG                  = -64;
    self->fadeB                  = -8;
    self->isFading               = true;
    SetMeshVertexColors(self->bgCircle1, 0xE0, 0xD0, 0xC0, 0xFF);
    SetMeshVertexColors(self->bgCircle2, 0xE0, 0xD0, 0xC0, 0xFF);
    SetMeshVertexColors(self->bgLines, 0xE0, 0, 0, 0xFF);
    self->Ztrans1      = -32.0;
    self->Ztrans2      = -64.0;
    self->ZtransRender = -128.0;
}
void MenuBG_Main(void *objPtr)
{
    RSDK_THIS(MenuBG);
    if (self->isFading) {
        SetRenderBlendMode(RENDER_BLEND_NONE);
        self->fadeTimer += Engine.deltaTime;
        if (self->fadeTimer > 1.5)
            self->isFading = false;

        if (self->fadeR < 0xF8)
            self->fadeR += 8;

        if (self->fadeG < 0xF8)
            self->fadeG += 8;

        if (self->fadeB < 0xF8)
            self->fadeB += 8;

        if (self->alpha < 0x100)
            self->alpha += 0x10;

        self->Ztrans1      = ((160.0 - self->Ztrans1) / (16.0 * Engine.deltaTime * 60.0)) + self->Ztrans1;
        self->Ztrans2      = ((160.0 - self->Ztrans2) / (18.0 * Engine.deltaTime * 60.0)) + self->Ztrans2;
        self->ZtransRender = ((160.0 - self->ZtransRender) / (Engine.deltaTime * 60.0 * 20.0)) + self->ZtransRender;
        RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, self->fadeR, self->fadeG, self->fadeB, 255);
        SetRenderBlendMode(RENDER_BLEND_ALPHA);
        SetRenderVertexColor(224, 208, 192);
        RenderImage(-64.0, 0.0, 160.0, 0.45, 0.45, 256.0, 256.0, 512.0, 512.0, 0.0, 0.0, self->alpha, self->textureID);
        SetRenderVertexColor(255, 255, 255);
        SetRenderBlendMode(RENDER_BLEND_NONE);
        self->circle1Rot = self->circle1Rot - Engine.deltaTime;
        if (self->circle1Rot < -M_PI_2)
            self->circle1Rot += M_PI_2;
        self->circle2Rot += Engine.deltaTime;
        if (self->circle2Rot > M_PI_2)
            self->circle2Rot -= M_PI_2;
        NewRenderState();
        MatrixRotateZF(&self->circle1, self->circle1Rot);
        MatrixTranslateXYZF(&self->matrixTemp, 120.0, 94.0, self->Ztrans1);
        MatrixMultiplyF(&self->circle1, &self->matrixTemp);
        SetRenderMatrix(&self->circle1);
        RenderMesh(self->bgCircle1, MESH_COLORS, false);

        MatrixRotateZF(&self->circle2, self->circle2Rot);
        MatrixTranslateXYZF(&self->matrixTemp, 4.0, 150.0, self->Ztrans2);
        MatrixMultiplyF(&self->circle2, &self->matrixTemp);
        SetRenderMatrix(&self->circle2);
        RenderMesh(self->bgCircle2, MESH_COLORS, false);

        MatrixTranslateXYZF(&self->renderMatrix, 0.0, 0.0, self->ZtransRender);
    }
    else {
        SetRenderBlendMode(RENDER_BLEND_NONE);
        RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 248, 248, 248, 255);
        SetRenderBlendMode(RENDER_BLEND_ALPHA);
        SetRenderVertexColor(0xE0, 0xD0, 0xC0);
        RenderImage(-64.0, 0.0, 160.0, 0.45, 0.45, 256.0, 256.0, 512.0, 512.0, 0.0, 0.0, 255, self->textureID);
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);
        SetRenderBlendMode(RENDER_BLEND_NONE);
        self->circle1Rot -= Engine.deltaTime;
        if (self->circle1Rot < -M_PI_2)
            self->circle1Rot += M_PI_2;
        self->circle2Rot += Engine.deltaTime;
        if (self->circle2Rot > M_PI_2)
            self->circle2Rot -= M_PI_2;
        NewRenderState();
        MatrixRotateZF(&self->circle1, self->circle1Rot);
        MatrixTranslateXYZF(&self->matrixTemp, 120.0, 94.0, 160.0);
        MatrixMultiplyF(&self->circle1, &self->matrixTemp);
        SetRenderMatrix(&self->circle1);
        RenderMesh(self->bgCircle1, MESH_COLORS, false);

        MatrixRotateZF(&self->circle2, self->circle2Rot);
        MatrixTranslateXYZF(&self->matrixTemp, 4.0, 150.0, 160.0);
        MatrixMultiplyF(&self->circle2, &self->matrixTemp);
        SetRenderMatrix(&self->circle2);
        RenderMesh(self->bgCircle2, MESH_COLORS, false);

        MatrixTranslateXYZF(&self->renderMatrix, 0.0, 0.0, 160.0);
    }
    SetRenderMatrix(&self->renderMatrix);
    self->animator.animationSpeed = 8.0 * Engine.deltaTime;
    AnimateMesh(self->bgLines, &self->animator);
    RenderMesh(self->bgLines, MESH_COLORS, false);
    SetRenderMatrix(NULL);
}
