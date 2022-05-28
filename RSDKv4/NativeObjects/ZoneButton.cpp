#include "RetroEngine.hpp"

void ZoneButton_Create(void *objPtr)
{
    RSDK_THIS(ZoneButton);
    self->z                  = 160.0;
    self->state              = ZONEBUTTON_STATE_UNSELECTED;
    self->textureIntro       = LoadTexture("Data/Game/Menu/Intro.png", TEXFMT_RGBA4444);
    self->textureSymbols     = LoadTexture("Data/Game/Menu/Symbols.png", TEXFMT_RGBA4444);
    self->bgColor           = 0xFFFFFF;
    self->bgColorSelected   = 0xFFFF00;
    self->textColor         = 0xFFFFFF;
    self->textSelectedColor = 0xFFFF00;
    self->angleSpeed         = 1.0;
}
void ZoneButton_Main(void *objPtr)
{
    RSDK_THIS(ZoneButton);
    NewRenderState();
    if (self->useRenderMatrix)
        SetRenderMatrix(&self->renderMatrix);
    SetRenderBlendMode(RENDER_BLEND_ALPHA);

    self->angle += self->angleSpeed * Engine.deltaTime;
    if (self->angle > M_PI_2)
        self->angle -= M_PI_2;
    if (self->angle < -M_PI_2)
        self->angle += M_PI_2;

    switch (self->state) {
        case ZONEBUTTON_STATE_UNSELECTED:
            SetRenderVertexColor((self->bgColor >> 16) & 0xFF, (self->bgColor >> 8) & 0xFF, self->bgColor & 0xFF);
            RenderRect(self->x - 64.0, self->y + 64.0, self->z, 128.0, 96.0, (self->bgColor >> 16) & 0xFF, (self->bgColor >> 8) & 0xFF,
                       self->bgColor & 0xFF, 255);

            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            RenderImage(self->x, self->y + 16.0, self->z, 0.9, 0.9, 64.0, 48.0, 128.0, 96.0,
                        (self->texX - 64.0) + (cosf(self->angle) * 24.0), (self->texY - 48.0) + (sinf(self->angle) * 24.0), 255,
                        self->textureIntro);

            SetRenderVertexColor((self->textColor >> 16) & 0xFF, (self->textColor >> 8) & 0xFF, self->textColor & 0xFF);
            RenderText(self->zoneText, FONT_TEXT, self->x - self->textWidth, self->y - 48.0, self->z, 0.25, 255);

            if (self->unlocked)
                RenderText(self->timeText, FONT_TEXT, self->x - 56.0, self->y - 24.0, self->z, 0.25, 255);
            else
                RenderImage(self->x - 32.0, self->y - 6.0, self->z, 0.5, 0.5, 24.0, 32.0, 48.0, 64.0, 205.0, 68.0, 255, self->textureSymbols);
            break;

        case ZONEBUTTON_STATE_SELECTED:
            SetRenderVertexColor((self->bgColor >> 16) & 0xFF, (self->bgColor >> 8) & 0xFF, self->bgColor & 0xFF);
            RenderRect(self->x - 64.0, self->y + 64.0, self->z, 128.0, 96.0, (self->bgColorSelected >> 16) & 0xFF,
                       (self->bgColorSelected >> 8) & 0xFF, self->bgColorSelected & 0xFF, 255);

            SetRenderVertexColor(255, 255, 255);
            RenderImage(self->x, self->y + 16.0, self->z, 0.9, 0.9, 64.0, 48.0, 128.0, 96.0,
                        (self->texX - 64.0) + (cosf(self->angle) * 24.0), (self->texY - 48.0) + (sinf(self->angle) * 24.0), 255,
                        self->textureIntro);

            SetRenderVertexColor((self->textSelectedColor >> 16) & 0xFF, (self->textSelectedColor >> 8) & 0xFF,
                                 self->textSelectedColor & 0xFF);
            RenderText(self->zoneText, FONT_TEXT, self->x - self->textWidth, self->y - 48.0, self->z, 0.25, 255);

            if (self->unlocked)
                RenderText(self->timeText, FONT_TEXT, self->x - 56.0, self->y - 24.0, self->z, 0.25, 255);
            else
                RenderImage(self->x - 32.0, self->y - 6.0, self->z, 0.5, 0.5, 24.0, 32.0, 48.0, 64.0, 205.0, 68.0, 255, self->textureSymbols);
            break;

        case ZONEBUTTON_STATE_FLASHING:
            self->flashTimer += Engine.deltaTime;
            if (self->flashTimer > 0.1)
                self->flashTimer -= 0.1;

            uint color = self->bgColor;
            if (self->flashTimer > 0.05)
                color = self->bgColorSelected;

            SetRenderVertexColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
            RenderRect(self->x - 64.0, self->y + 64.0, self->z, 128.0, 96.0, (self->bgColorSelected >> 16) & 0xFF,
                       (self->bgColorSelected >> 8) & 0xFF, self->bgColorSelected & 0xFF, 255);

            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            RenderImage(self->x, self->y + 16.0, self->z, 0.9, 0.9, 64.0, 48.0, 128.0, 96.0,
                        (self->texX - 64.0) + (cosf(self->angle) * 24.0), (self->texY - 48.0) + (sinf(self->angle) * 24.0), 255,
                        self->textureIntro);

            color = self->textColor;
            if (self->flashTimer > 0.05)
                color = self->textSelectedColor;

            SetRenderVertexColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
            RenderText(self->zoneText, FONT_TEXT, self->x - self->textWidth, self->y - 48.0, self->z, 0.25, 255);
            RenderText(self->timeText, FONT_TEXT, self->x - 56.0, self->y - 24.0, self->z, 0.25, 255);

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                self->timer = 0.0;
                self->state   = ZONEBUTTON_STATE_UNSELECTED;
            }
            break;
    }

    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    if (self->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}
