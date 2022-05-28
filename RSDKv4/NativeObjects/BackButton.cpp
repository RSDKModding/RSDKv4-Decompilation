#include "RetroEngine.hpp"

void BackButton_Create(void *objPtr)
{
    RSDK_THIS(BackButton);
    self->textureCircle = LoadTexture("Data/Game/Menu/Circle.png", TEXFMT_RGBA4444);

    int texture    = LoadTexture("Data/Game/Menu/Intro.png", TEXFMT_RGBA4444);
    self->meshBack = LoadMesh("Data/Game/Models/BackArrow.bin", texture);
    self->x        = 0.0;
    self->y        = 16.0;
    self->z        = 160.0;
    self->r        = 0xFF;
    self->g        = 0xFF;
    self->b        = 0x00;
}
void BackButton_Main(void *objPtr)
{
    RSDK_THIS(BackButton);

    if (self->visible) {
        if (self->scale < 0.2) {
            self->scale += ((0.25 - self->scale) / ((60.0 * Engine.deltaTime) * 16.0));
            if (self->scale > 0.2)
                self->scale = 0.2;
        }
        SetRenderBlendMode(RENDER_BLEND_ALPHA);
        SetRenderVertexColor(self->r, self->g, self->b);
        RenderImage(self->x, self->y, self->z, self->scale, self->scale, 256.0, 256.0, 512.0, 512.0, 0.0, 0.0, 255, self->textureCircle);
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);
        SetRenderBlendMode(RENDER_BLEND_NONE);

        self->angle -= (Engine.deltaTime + Engine.deltaTime);
        if (self->angle < -M_PI_2)
            self->angle += M_PI_2;

        NewRenderState();
        MatrixScaleXYZF(&self->renderMatrix, (cosf(self->angle) * 0.35) + 1.25, (sinf(self->angle) * 0.35) + 1.25, 1.0);
        MatrixTranslateXYZF(&self->matrixTemp, self->x, self->y, self->z - 8.0);
        MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
        SetRenderMatrix(&self->renderMatrix);
        RenderMesh(self->meshBack, MESH_NORMALS, true);
        SetRenderMatrix(NULL);
    }
}
