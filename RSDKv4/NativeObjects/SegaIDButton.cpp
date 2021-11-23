#include "RetroEngine.hpp"

void SegaIDButton_Create(void *objPtr)
{
    RSDK_THIS(SegaIDButton);
    entity->z         = 160.0;
    entity->state     = SEGAIDBUTTON_STATE_IDLE;
    entity->textureID = LoadTexture("Data/Game/Menu/SegaID.png", TEXFMT_RGBA8888);
}
void SegaIDButton_Main(void *objPtr)
{
    RSDK_THIS(SegaIDButton);
    if (entity->useRenderMatrix)
        SetRenderMatrix(&entity->renderMatrix);
    SetRenderBlendMode(RENDER_BLEND_ALPHA);

    switch (entity->state) {
        case SEGAIDBUTTON_STATE_IDLE:
            RenderImage(entity->x, entity->y, entity->z, 0.25, 0.25, 64.0, 64.0, 128.0, 128.0, entity->texX, 0.0, entity->alpha, entity->textureID);
            break;
        case SEGAIDBUTTON_STATE_PRESSED:
            RenderImage(entity->x, entity->y, entity->z, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, entity->texX, 0.0, entity->alpha, entity->textureID);
            break;
    }
    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    NewRenderState();
    if (entity->useRenderMatrix)
        SetRenderMatrix(NULL);
}
