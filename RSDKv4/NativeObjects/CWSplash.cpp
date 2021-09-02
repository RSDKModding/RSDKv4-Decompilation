#include "RetroEngine.hpp"

void CWSplash_Create(void *objPtr)
{
    RSDK_THIS(SegaSplash);
    entity->state     = 0;
    entity->rectAlpha = 320.0;
    entity->textureID = LoadTexture("Data/Game/Menu/CWLogo.png", TEXFMT_RGBA8888);
}
void CWSplash_Main(void *objPtr)
{
    RSDK_THIS(SegaSplash);

    switch (entity->state) {
        case 0:
            entity->rectAlpha -= 300.0 * Engine.deltaTime;
            if (entity->rectAlpha < -320.0)
                entity->state = 1;
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0xFF, 0x90, 0x00, 0xFF);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImage(0.0, 0.0, 160.0, 0.25, 0.25, 512.0, 256.0, 1024.0, 512.0, 0.0, 0.0, 255, entity->textureID);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, entity->rectAlpha);
            break;
        case 1:
            entity->rectAlpha += 300.0 * Engine.deltaTime;
            if (entity->rectAlpha > 512.0)
                entity->state = 2;
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0xFF, 0x90, 0x00, 0xFF);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImage(0.0, 0.0, 160.0, 0.25, 0.25, 512.0, 256.0, 1024.0, 512.0, 0.0, 0.0, 255, entity->textureID);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, entity->rectAlpha);
            break;
        case 2: ResetNativeObject(entity, TitleScreen_Create, TitleScreen_Main); break;
    }
}
