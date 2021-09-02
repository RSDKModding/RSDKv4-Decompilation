#include "RetroEngine.hpp"

void SegaSplash_Create(void *objPtr)
{
    RSDK_THIS(SegaSplash);
    entity->state     = 0;
    entity->rectAlpha = 320.0;
    entity->textureID = LoadTexture("Data/Game/Menu/CWLogo.png", TEXFMT_RGBA8888);
    if (Engine.useHighResAssets) {
        if (Engine.language == RETRO_JP)
            entity->textureID = LoadTexture("Data/Game/Menu/SegaJP@2x.png", TEXFMT_RGBA5551);
        else
            entity->textureID = LoadTexture("Data/Game/Menu/Sega@2x.png", TEXFMT_RGBA5551);
    }
    else {
        if (Engine.language == RETRO_JP)
            entity->textureID = LoadTexture("Data/Game/Menu/SegaJP.png", TEXFMT_RGBA5551);
        else
            entity->textureID = LoadTexture("Data/Game/Menu/Sega.png", TEXFMT_RGBA5551);
    }
}
void SegaSplash_Main(void *objPtr)
{
    RSDK_THIS(SegaSplash);

    switch (entity->state) {
        case 0:
            entity->rectAlpha -= 300.0 * Engine.deltaTime;
            if (entity->rectAlpha < -320.0)
                entity->state = 1;
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0xFF, 0xFF, 0xFF, 0xFF);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImage(0.0, 0.0, 160.0, 0.4, 0.4, 256.0, 128.0, 512.0, 256.0, 0.0, 0.0, 255, entity->textureID);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, entity->rectAlpha);
            break;
        case 1:
            entity->rectAlpha += 300.0 * Engine.deltaTime;
            if (entity->rectAlpha > 512.0)
                entity->state = 2;
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0xFF, 0xFF, 0xFF, 0xFF);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImage(0.0, 0.0, 160.0, 0.4, 0.4, 256.0, 128.0, 512.0, 256.0, 0.0, 0.0, 255, entity->textureID);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, entity->rectAlpha);
            break;
        case 2: ResetNativeObject(entity, CWSplash_Create, CWSplash_Main); break;
    }
}
