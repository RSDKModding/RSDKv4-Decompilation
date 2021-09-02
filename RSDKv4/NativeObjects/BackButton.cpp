#include "RetroEngine.hpp"

void BackButton_Create(void *objPtr)
{
    RSDK_THIS(BackButton);
    entity->textureCircle = LoadTexture("Data/Game/Menu/Circle.png", TEXFMT_RGBA4444);

    int texture      = LoadTexture("Data/Game/Menu/Intro.png", TEXFMT_RGBA4444);
    entity->meshBack = LoadMesh("Data/Game/Models/BackArrow.bin", texture);
    entity->x        = 0.0;
    entity->y        = 16.0;
    entity->z        = 160.0;
    entity->r        = 0xFF;
    entity->g        = 0xFF;
    entity->b        = 0x00;
}
void BackButton_Main(void *objPtr)
{
    RSDK_THIS(BackButton);

    if (entity->visible) {
        if (entity->scale < 0.2) {
            entity->scale += ((0.25 - entity->scale) / ((60.0 * Engine.deltaTime) * 16.0));
            if (entity->scale > 0.2)
                entity->scale = 0.2;
        }
        SetRenderBlendMode(RENDER_BLEND_ALPHA);
        SetRenderVertexColor(entity->r, entity->g, entity->b);
        RenderImage(entity->x, entity->y, entity->z, entity->scale, entity->scale, 256.0, 256.0, 512.0, 512.0, 0.0, 0.0, 255, entity->textureCircle);
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);
        SetRenderBlendMode(RENDER_BLEND_NONE);

        entity->angle -= (Engine.deltaTime + Engine.deltaTime);
        if (entity->angle < -(M_PI * 2))
            entity->angle += (M_PI * 2);

        NewRenderState();
        matrixScaleXYZF(&entity->renderMatrix, (cosf(entity->angle) * 0.35) + 1.25, (sinf(entity->angle) * 0.35) + 1.25, 1.0);
        matrixTranslateXYZF(&entity->matrix2, entity->x, entity->y, entity->z - 8.0);
        matrixMultiplyF(&entity->renderMatrix, &entity->matrix2);
        SetRenderMatrix(&entity->renderMatrix);
        RenderMesh(entity->meshBack, MESH_NORMALS, true);
        SetRenderMatrix(NULL);
    }
}
