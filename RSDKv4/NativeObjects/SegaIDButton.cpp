#include "RetroEngine.hpp"

void SegaIDButton_Create(void *objPtr)
{
    RSDK_THIS(SegaIDButton);
    entity->translateZ = 160.0;
    entity->state      = 0;
    entity->textureID  = LoadTexture("Data/Game/Menu/SegaID.png", 3);
}
void SegaIDButton_Main(void *objPtr)
{
    RSDK_THIS(SegaIDButton);
    if (entity->useRenderMatrix)
        SetRenderMatrix(&entity->renderMatrix);
    SetRenderBlendMode(RENDER_BLEND_ALPHA);

    switch (entity->state) {
        case 0:
            RenderImage(entity->translateX, entity->translateY, entity->translateZ, 0.25, 0.25, 64.0, 64.0, 128.0, 128.0, entity->texX, 0.0,
                        entity->alpha, entity->textureID);
            break;
        case 1:
            RenderImage(entity->translateX, entity->translateY, entity->translateZ, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, entity->texX, 0.0,
                        entity->alpha, entity->textureID);
            break;
    }
    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    NewRenderState();
    if (entity->useRenderMatrix)
        SetRenderMatrix(NULL);
}
