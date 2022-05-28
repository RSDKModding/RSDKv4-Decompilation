#include "RetroEngine.hpp"

void CreditText_Create(void *objPtr)
{
    RSDK_THIS(CreditText);
    self->alpha     = 255;
    self->state     = CREDITTEXT_STATE_SETUP;
    self->textureID = LoadTexture("Data/Game/Menu/DevLogos.png", TEXFMT_RGBA8888);
}
void CreditText_Main(void *objPtr)
{
    RSDK_THIS(CreditText);

    if (self->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(&self->renderMatrix);
    }

    switch (self->state) {
        case CREDITTEXT_STATE_SETUP:
            self->state       = CREDITTEXT_STATE_DISPLAY;
            self->alignOffset = GetTextWidth(self->text, self->fontID, self->scale) * 0.5;
            // Fallthrough
        default:
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            SetRenderVertexColor((self->color >> 16) & 0xFF, (self->color >> 8) & 0xFF, self->color & 0xFF);
            RenderTextClipped(self->text, self->fontID, self->textX - self->alignOffset, self->textY, self->textZ, self->scale,
                              self->alpha);
            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            break;

        case CREDITTEXT_STATE_IMAGE:
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImageClipped(-64.0, self->textY, self->textZ, 0.35, 0.35, 157.0, 77.0, 314.0, 154.0, 2.0, 2.0, 255, self->textureID);
            RenderImageClipped(64.0, self->textY, self->textZ, 0.35, 0.35, 92.0, 92.0, 184.0, 184.0, 320.0, 2.0, 255, self->textureID);
            break;
    }

    if (self->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}
