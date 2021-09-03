#include "RetroEngine.hpp"

void TextLabel_Create(void *objPtr)
{
    RSDK_THIS(TextLabel);
    entity->z        = 160.0;
    entity->alpha    = 255;
    entity->state    = 0;
    entity->alignPtr = TextLabel_Align;
}
void TextLabel_Main(void *objPtr)
{
    RSDK_THIS(TextLabel);

    if (entity->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(&entity->renderMatrix);
    }

    if (entity->useColours)
        SetRenderVertexColor(entity->r, entity->g, entity->b);

    switch (entity->state) {
        case 0:
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderText(entity->text, entity->fontID, entity->x - entity->textWidth, entity->y, entity->z, entity->scale, entity->alpha);
            break;
        case 1:
            entity->timer += Engine.deltaTime;
            if (entity->timer > 1.0f)
                entity->timer -= 1.0f;

            if (entity->timer > 0.5) {
                SetRenderBlendMode(RENDER_BLEND_ALPHA);
                RenderText(entity->text, entity->fontID, entity->x - entity->textWidth, entity->y, entity->z, entity->scale, entity->alpha);
            }
            break;
        case 2:
            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.1f)
                entity->timer -= 0.1f;

            if (entity->timer > 0.05) {
                SetRenderBlendMode(RENDER_BLEND_ALPHA);
                RenderText(entity->text, entity->fontID, entity->x - entity->textWidth, entity->y, entity->z, entity->scale, entity->alpha);
            }
            break;
    }

    if (entity->useColours)
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);

    if (entity->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}

void TextLabel_Align(NativeEntity_TextLabel *label, int align)
{
    switch (align) {
        default:
        case 0: label->textWidth = 0.0; break;
        case 1: label->textWidth = GetTextWidth(label->text, label->fontID, label->scale) * 0.5; break;
        case 2: label->textWidth = GetTextWidth(label->text, label->fontID, label->scale); break;
    }
}