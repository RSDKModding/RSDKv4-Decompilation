#include "RetroEngine.hpp"

void SegaIDButton_Create(void *objPtr)
{
    RSDK_THIS(SegaIDButton);
    self->z         = 160.0;
    self->state     = SEGAIDBUTTON_STATE_IDLE;
    self->textureID = LoadTexture("Data/Game/Menu/SegaID.png", TEXFMT_RGBA8888);
}
void SegaIDButton_Main(void *objPtr)
{
    RSDK_THIS(SegaIDButton);
    if (self->useRenderMatrix)
        SetRenderMatrix(&self->renderMatrix);
    SetRenderBlendMode(RENDER_BLEND_ALPHA);

    switch (self->state) {
        case SEGAIDBUTTON_STATE_IDLE:
            RenderImage(self->x, self->y, self->z, 0.25, 0.25, 64.0, 64.0, 128.0, 128.0, self->texX, 0.0, self->alpha, self->textureID);
            break;
        case SEGAIDBUTTON_STATE_PRESSED:
            RenderImage(self->x, self->y, self->z, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, self->texX, 0.0, self->alpha, self->textureID);
            break;
    }
    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    NewRenderState();
    if (self->useRenderMatrix)
        SetRenderMatrix(NULL);
}
