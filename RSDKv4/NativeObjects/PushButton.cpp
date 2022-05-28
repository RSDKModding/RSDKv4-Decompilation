#include "RetroEngine.hpp"

void PushButton_Create(void *objPtr)
{
    RSDK_THIS(PushButton);
    self->z                 = 160.0f;
    self->alpha             = 255;
    self->scale             = 0.15f;
    self->state             = PUSHBUTTON_STATE_SCALED;
    self->symbolsTex        = LoadTexture("Data/Game/Menu/Symbols.png", TEXFMT_RGBA4444);
    self->bgColor           = 0xFF0000;
    self->bgColorSelected   = 0xFF4000;
    self->textColor         = 0xFFFFFF;
    self->textColorSelected = 0xFFFF00;
}
void PushButton_Main(void *objPtr)
{
    RSDK_THIS(PushButton);
    NewRenderState();
    if (self->useRenderMatrix)
        SetRenderMatrix(&self->renderMatrix);
    SetRenderBlendMode(RENDER_BLEND_ALPHA);

    switch (self->state) {
        case PUSHBUTTON_STATE_UNSELECTED: {
            SetRenderVertexColor((self->bgColor >> 16) & 0xFF, (self->bgColor >> 8) & 0xFF, self->bgColor & 0xFF);
            RenderImage(self->x - self->textWidth, self->y, self->z, self->scale, self->scale, 64.0, 64.0, 64.0, 128.0, 0.0, 0.0, self->alpha,
                        self->symbolsTex);
            RenderImage(self->x, self->y, self->z, self->textWidth + self->textWidth, self->scale, 0.5, 64.0, 1.0, 128.0, 63.0, 0.0, self->alpha,
                        self->symbolsTex);
            RenderImage(self->x + self->textWidth, self->y, self->z, self->scale, self->scale, 0.0, 64.0, 64.0, 128.0, 64.0, 0.0, self->alpha,
                        self->symbolsTex);
            SetRenderVertexColor(self->textColor >> 16, (self->textColor >> 8) & 0xFF, self->textColor & 0xFF);
            RenderText(self->text, FONT_LABEL, self->x - self->xOff, self->y - self->yOff, self->z, self->textScale, self->alpha);
            break;
        }
        case PUSHBUTTON_STATE_SELECTED: {
            if (usePhysicalControls) {
                SetRenderVertexColor(0x00, 0x00, 0x00);
                RenderImage(self->x - self->textWidth, self->y, self->z, 1.1 * self->scale, 1.1 * self->scale, 64.0, 64.0, 64.0, 128.0, 0.0, 0.0,
                            self->alpha, self->symbolsTex);
                RenderImage(self->x, self->y, self->z, self->textWidth + self->textWidth, 1.1 * self->scale, 0.5, 64.0, 1.0, 128.0, 63.0, 0.0,
                            self->alpha, self->symbolsTex);
                RenderImage(self->x + self->textWidth, self->y, self->z, 1.1 * self->scale, 1.1 * self->scale, 0.0, 64.0, 64.0, 128.0, 64.0, 0.0,
                            self->alpha, self->symbolsTex);
            }
            SetRenderVertexColor((self->bgColorSelected >> 16) & 0xFF, (self->bgColorSelected >> 8) & 0xFF, self->bgColorSelected & 0xFF);
            RenderImage(self->x - self->textWidth, self->y, self->z, self->scale, self->scale, 64.0, 64.0, 64.0, 128.0, 0.0, 0.0, self->alpha,
                        self->symbolsTex);
            RenderImage(self->x, self->y, self->z, self->textWidth + self->textWidth, self->scale, 0.5, 64.0, 1.0, 128.0, 63.0, 0.0, self->alpha,
                        self->symbolsTex);
            RenderImage(self->x + self->textWidth, self->y, self->z, self->scale, self->scale, 0.0, 64.0, 64.0, 128.0, 64.0, 0.0, self->alpha,
                        self->symbolsTex);
            SetRenderVertexColor(self->textColorSelected >> 16, (self->textColorSelected >> 8) & 0xFF, self->textColorSelected & 0xFF);
            RenderText(self->text, FONT_LABEL, self->x - self->xOff, self->y - self->yOff, self->z, self->textScale, self->alpha);
            break;
        }
        case PUSHBUTTON_STATE_FLASHING: {
            self->flashTimer += Engine.deltaTime;
            if (self->flashTimer > 0.1)
                self->flashTimer -= 0.1;
            SetRenderVertexColor((self->bgColorSelected >> 16) & 0xFF, (self->bgColorSelected >> 8) & 0xFF, self->bgColorSelected & 0xFF);
            RenderImage(self->x - self->textWidth, self->y, self->z, self->scale, self->scale, 64.0, 64.0, 64.0, 128.0, 0.0, 0.0, self->alpha,
                        self->symbolsTex);
            RenderImage(self->x, self->y, self->z, self->textWidth + self->textWidth, self->scale, 0.5, 64.0, 1.0, 128.0, 63.0, 0.0, self->alpha,
                        self->symbolsTex);
            RenderImage(self->x + self->textWidth, self->y, self->z, self->scale, self->scale, 0.0, 64.0, 64.0, 128.0, 64.0, 0.0, self->alpha,
                        self->symbolsTex);

            int color = self->flashTimer > 0.05f ? self->textColorSelected : self->textColor;
            SetRenderVertexColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
            RenderText(self->text, FONT_LABEL, self->x - self->xOff, self->y - self->yOff, self->z, self->textScale, self->alpha);
            self->stateTimer += Engine.deltaTime;
            if (self->stateTimer > 0.5) {
                self->stateTimer = 0.0;
                self->state      = PUSHBUTTON_STATE_UNSELECTED;
            }
            break;
        }
        case PUSHBUTTON_STATE_SCALED: {
            self->state     = 0;
            self->xOff      = GetTextWidth(self->text, FONT_LABEL, self->scale) * 0.375;
            self->textWidth = GetTextWidth(self->text, FONT_LABEL, self->scale) * 0.375;
            self->yOff      = 0.75 * self->scale * 32.0;
            self->textScale = 0.75 * self->scale;

            SetRenderVertexColor(self->bgColor >> 16, (self->bgColor >> 8) & 0xFF, self->bgColor & 0xFF);
            RenderImage(self->x - self->textWidth, self->y, self->z, self->scale, self->scale, 64.0, 64.0, 64.0, 128.0, 0.0, 0.0, self->alpha,
                        self->symbolsTex);
            RenderImage(self->x, self->y, self->z, self->textWidth + self->textWidth, self->scale, 0.5, 64.0, 1.0, 128.0, 63.0, 0.0, self->alpha,
                        self->symbolsTex);
            RenderImage(self->x + self->textWidth, self->y, self->z, self->scale, self->scale, 0.0, 64.0, 64.0, 128.0, 64.0, 0.0, self->alpha,
                        self->symbolsTex);
            SetRenderVertexColor((self->textColor >> 16) & 0xFF, (self->textColor >> 8) & 0xFF, self->textColor & 0xFF);
            RenderText(self->text, FONT_LABEL, self->x - self->xOff, self->y - self->yOff, self->z, self->textScale, self->alpha);
            break;
        }
    }

    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    if (self->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}
