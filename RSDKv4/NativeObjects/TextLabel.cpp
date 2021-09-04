#include "RetroEngine.hpp"

void TextLabel_Create(void *objPtr)
{
    RSDK_THIS(TextLabel);
    entity->z        = 160.0;
    entity->alpha    = 0xFF;
    entity->state    = TEXTLABEL_STATE_IDLE;
    entity->alignPtr = TextLabel_Align;
}
void TextLabel_Main(void *objPtr)
{
    RSDK_THIS(TextLabel);

    if (entity->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(&entity->renderMatrix);
    }

#if !RETRO_USE_ORIGINAL_CODE
    if (entity->useColours)
        SetRenderVertexColor(entity->r, entity->g, entity->b);
#endif

    switch (entity->state) {
        case TEXTLABEL_STATE_IDLE:
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderText(entity->text, entity->fontID, entity->x - entity->alignOffset, entity->y, entity->z, entity->scale, entity->alpha);
            break;
        case TEXTLABEL_STATE_BLINK:
            entity->timer += Engine.deltaTime;
            if (entity->timer > 1.0f)
                entity->timer -= 1.0f;

            if (entity->timer > 0.5) {
                SetRenderBlendMode(RENDER_BLEND_ALPHA);
                RenderText(entity->text, entity->fontID, entity->x - entity->alignOffset, entity->y, entity->z, entity->scale, entity->alpha);
            }
            break;
        case TEXTLABEL_STATE_BLINK_FAST:
            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.1f)
                entity->timer -= 0.1f;

            if (entity->timer > 0.05) {
                SetRenderBlendMode(RENDER_BLEND_ALPHA);
                RenderText(entity->text, entity->fontID, entity->x - entity->alignOffset, entity->y, entity->z, entity->scale, entity->alpha);
            }
            break;
    }

#if !RETRO_USE_ORIGINAL_CODE
    if (entity->useColours)
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);
#endif

    if (entity->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}

void TextLabel_Align(NativeEntity_TextLabel *label, TextAlignments align)
{
    switch (align) {
        default:
        case ALIGN_LEFT: label->alignOffset = 0.0; break;
        case ALIGN_CENTER: label->alignOffset = GetTextWidth(label->text, label->fontID, label->scale) * 0.5; break;
        case ALIGN_RIGHT: label->alignOffset = GetTextWidth(label->text, label->fontID, label->scale); break;
    }
}