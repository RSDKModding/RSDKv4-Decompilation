#include "RetroEngine.hpp"

void CreditText_Create(void *objPtr)
{
    RSDK_THIS(CreditText);
    entity->alpha     = 255;
    entity->state     = CREDITTEXT_STATE_SETUP;
    entity->textureID = LoadTexture("Data/Game/Menu/DevLogos.png", TEXFMT_RGBA8888);
}
void CreditText_Main(void *objPtr)
{
    RSDK_THIS(CreditText);

    if (entity->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(&entity->renderMatrix);
    }

    switch (entity->state) {
        case CREDITTEXT_STATE_SETUP:
            entity->state       = CREDITTEXT_STATE_DISPLAY;
            entity->alignOffset = GetTextWidth(entity->text, entity->fontID, entity->scale) * 0.5;
            // Fallthrough
        default:
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            SetRenderVertexColor((entity->colour >> 16) & 0xFF, (entity->colour >> 8) & 0xFF, entity->colour & 0xFF);
            RenderTextClipped(entity->text, entity->fontID, entity->textX - entity->alignOffset, entity->textY, entity->textZ, entity->scale,
                              entity->alpha);
            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            break;
        case CREDITTEXT_STATE_IMAGE:
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImageClipped(-64.0, entity->textY, entity->textZ, 0.35, 0.35, 157.0, 77.0, 314.0, 154.0, 2.0, 2.0, 255, entity->textureID);
            RenderImageClipped(64.0, entity->textY, entity->textZ, 0.35, 0.35, 92.0, 92.0, 184.0, 184.0, 320.0, 2.0, 255, entity->textureID);
            break;
    }

    if (entity->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}
