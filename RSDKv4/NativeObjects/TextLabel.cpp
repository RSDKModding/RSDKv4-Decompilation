#include "RetroEngine.hpp"

void TextLabel_Create(void *objPtr)
{
    RSDK_THIS(TextLabel);
    self->z        = 160.0;
    self->alpha    = 0xFF;
    self->state    = TEXTLABEL_STATE_IDLE;
    self->alignPtr = TextLabel_Align;
}
void TextLabel_Main(void *objPtr)
{
    RSDK_THIS(TextLabel);

    if (self->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(&self->renderMatrix);
    }

#if !RETRO_USE_ORIGINAL_CODE
    if (self->useColors)
        SetRenderVertexColor(self->r, self->g, self->b);
#endif

    switch (self->state) {
        default: break;
        case TEXTLABEL_STATE_NONE: break;
        case TEXTLABEL_STATE_IDLE:
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderText(self->text, self->fontID, self->x - self->alignOffset, self->y, self->z, self->scale, self->alpha);
            break;
        case TEXTLABEL_STATE_BLINK:
            self->timer += Engine.deltaTime;
            if (self->timer > 1.0f)
                self->timer -= 1.0f;

            if (self->timer > 0.5) {
                SetRenderBlendMode(RENDER_BLEND_ALPHA);
                RenderText(self->text, self->fontID, self->x - self->alignOffset, self->y, self->z, self->scale, self->alpha);
            }
            break;
        case TEXTLABEL_STATE_BLINK_FAST:
            self->timer += Engine.deltaTime;
            if (self->timer > 0.1f)
                self->timer -= 0.1f;

            if (self->timer > 0.05) {
                SetRenderBlendMode(RENDER_BLEND_ALPHA);
                RenderText(self->text, self->fontID, self->x - self->alignOffset, self->y, self->z, self->scale, self->alpha);
            }
            break;
    }

#if !RETRO_USE_ORIGINAL_CODE
    if (self->useColors)
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);
#endif

    if (self->useRenderMatrix) {
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