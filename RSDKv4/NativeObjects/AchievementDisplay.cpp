#include "RetroEngine.hpp"

#if !RETRO_USE_ORIGINAL_CODE
void AchievementDisplay_Create(void *objPtr)
{
    RSDK_THIS(AchievementDisplay);
    entity->z     = 160.0;
    entity->alpha = 0xFF;
    entity->state = 0;
}
void AchievementDisplay_Main(void *objPtr)
{
    RSDK_THIS(AchievementDisplay);

    if (entity->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(&entity->renderMatrix);
    }

    float width  = GetTextWidth(entity->achievementText, entity->fontID, entity->scale);
    float height = GetTextHeight(entity->achievementText, entity->fontID, entity->scale) + fontList[entity->fontID].lineHeight * entity->scale;

    switch (entity->state) {
        case 0: // locked
            SetRenderBlendMode(RENDER_BLEND_ALPHA);

            // RenderRectClipped(entity->x, entity->y + 16.0f, entity->z, 224.0f, 80.0f, 0x00, 0x00, 0x00, 0xC0);

            RenderRectClipped(entity->x, entity->y + 16.0f, entity->z, 4.0f, 80.0f, 0xFF, 0x00, 0x00, 0xC0);
            RenderRectClipped(entity->x + 220.0f, entity->y + 16.0f, entity->z, 4.0f, 80.0f, 0xFF, 0x00, 0x00, 0xC0);
            RenderRectClipped(entity->x + 4.0f, entity->y + 16.0f, entity->z, 216.0f, 4.0f, 0xFF, 0x00, 0x00, 0xC0);
            RenderRectClipped(entity->x + 4.0f, entity->y - 60.0f, entity->z, 216.0f, 4.0f, 0xFF, 0x00, 0x00, 0xC0);

            RenderTextClipped(entity->achievementText, entity->fontID, entity->x + (224.0f - width) / 2, entity->y, entity->z, entity->scale,
                              entity->alpha);
            RenderTextClipped(entity->descriptionText, entity->fontID, entity->x + 8.0f, entity->y - height, entity->z, entity->scale * 0.75,
                              entity->alpha);
            break;
        case 1: // unlocked
            SetRenderBlendMode(RENDER_BLEND_ALPHA);

            // RenderRectClipped(entity->x, entity->y + 16.0f, entity->z, 224.0f, 80.0f, 0x00, 0x00, 0x00, 0xC0);

            RenderRectClipped(entity->x, entity->y + 16.0f, entity->z, 4.0f, 80.0f, 0x00, 0xFF, 0x00, 0xC0);
            RenderRectClipped(entity->x + 220.0f, entity->y + 16.0f, entity->z, 4.0f, 80.0f, 0x00, 0xFF, 0x00, 0xC0);
            RenderRectClipped(entity->x + 4.0f, entity->y + 16.0f, entity->z, 216.0f, 4.0f, 0x00, 0xFF, 0x00, 0xC0);
            RenderRectClipped(entity->x + 4.0f, entity->y - 60.0f, entity->z, 216.0f, 4.0f, 0x00, 0xFF, 0x00, 0xC0);

            RenderTextClipped(entity->achievementText, entity->fontID, entity->x + (224.0f - width) / 2, entity->y, entity->z, entity->scale,
                              entity->alpha);
            RenderTextClipped(entity->descriptionText, entity->fontID, entity->x + 8.0f, entity->y - height, entity->z, entity->scale * 0.75,
                              entity->alpha);
            break;
    }

    if (entity->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}
#endif
