#include "RetroEngine.hpp"

#if !RETRO_USE_ORIGINAL_CODE
void AchievementDisplay_Create(void *objPtr)
{
    RSDK_THIS(AchievementDisplay);

    self->z     = 160.0;
    self->alpha = 0xFF;
    self->state = ACHDISP_STATE_LOCKED;
}
void AchievementDisplay_Main(void *objPtr)
{
    RSDK_THIS(AchievementDisplay);

    if (self->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(&self->renderMatrix);
    }

    float width  = GetTextWidth(self->achievementText, self->fontID, self->scale);
    float height = GetTextHeight(self->achievementText, self->fontID, self->scale) + fontList[self->fontID].lineHeight * self->scale;

    switch (self->state) {
        case ACHDISP_STATE_LOCKED: // locked
            SetRenderBlendMode(RENDER_BLEND_ALPHA);

            // RenderRectClipped(self->x, self->y + 16.0f, self->z, 224.0f, 80.0f, 0x00, 0x00, 0x00, 0xC0);

            RenderRectClipped(self->x, self->y + 16.0f, self->z, 4.0f, 80.0f, 0xFF, 0x00, 0x00, 0xC0);
            RenderRectClipped(self->x + 220.0f, self->y + 16.0f, self->z, 4.0f, 80.0f, 0xFF, 0x00, 0x00, 0xC0);
            RenderRectClipped(self->x + 4.0f, self->y + 16.0f, self->z, 216.0f, 4.0f, 0xFF, 0x00, 0x00, 0xC0);
            RenderRectClipped(self->x + 4.0f, self->y - 60.0f, self->z, 216.0f, 4.0f, 0xFF, 0x00, 0x00, 0xC0);

            RenderTextClipped(self->achievementText, self->fontID, self->x + (224.0f - width) / 2, self->y, self->z, self->scale, self->alpha);
            RenderTextClipped(self->descriptionText, self->fontID, self->x + 8.0f, self->y - height, self->z, self->scale * 0.75, self->alpha);
            break;

        case ACHDISP_STATE_UNLOCKED: // unlocked
            SetRenderBlendMode(RENDER_BLEND_ALPHA);

            // RenderRectClipped(self->x, self->y + 16.0f, self->z, 224.0f, 80.0f, 0x00, 0x00, 0x00, 0xC0);

            RenderRectClipped(self->x, self->y + 16.0f, self->z, 4.0f, 80.0f, 0x00, 0xFF, 0x00, 0xC0);
            RenderRectClipped(self->x + 220.0f, self->y + 16.0f, self->z, 4.0f, 80.0f, 0x00, 0xFF, 0x00, 0xC0);
            RenderRectClipped(self->x + 4.0f, self->y + 16.0f, self->z, 216.0f, 4.0f, 0x00, 0xFF, 0x00, 0xC0);
            RenderRectClipped(self->x + 4.0f, self->y - 60.0f, self->z, 216.0f, 4.0f, 0x00, 0xFF, 0x00, 0xC0);

            RenderTextClipped(self->achievementText, self->fontID, self->x + (224.0f - width) / 2, self->y, self->z, self->scale, self->alpha);
            RenderTextClipped(self->descriptionText, self->fontID, self->x + 8.0f, self->y - height, self->z, self->scale * 0.75, self->alpha);
            break;
    }

    if (self->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}
#endif
