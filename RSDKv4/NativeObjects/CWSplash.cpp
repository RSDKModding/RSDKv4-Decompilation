#include "RetroEngine.hpp"

void CWSplash_Create(void *objPtr)
{
    RSDK_THIS(CWSplash);
    self->state     = CWSPLASH_STATE_ENTER;
    self->rectAlpha = 320.0;
    self->textureID = LoadTexture("Data/Game/Menu/CWLogo.png", TEXFMT_RGBA8888);
}
void CWSplash_Main(void *objPtr)
{
    RSDK_THIS(CWSplash);

    switch (self->state) {
        case CWSPLASH_STATE_ENTER:
            self->rectAlpha -= 300.0 * Engine.deltaTime;
            if (self->rectAlpha < -320.0)
                self->state = CWSPLASH_STATE_EXIT;
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0xFF, 0x90, 0x00, 0xFF);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImage(0.0, 0.0, 160.0, 0.25, 0.25, 512.0, 256.0, 1024.0, 512.0, 0.0, 0.0, 255, self->textureID);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, self->rectAlpha);
            break;
        case CWSPLASH_STATE_EXIT:
            self->rectAlpha += 300.0 * Engine.deltaTime;
            if (self->rectAlpha > 512.0)
                self->state = CWSPLASH_STATE_SPAWNTITLE;
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0xFF, 0x90, 0x00, 0xFF);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImage(0.0, 0.0, 160.0, 0.25, 0.25, 512.0, 256.0, 1024.0, 512.0, 0.0, 0.0, 255, self->textureID);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, self->rectAlpha);
            break;
        case CWSPLASH_STATE_SPAWNTITLE: ResetNativeObject(self, TitleScreen_Create, TitleScreen_Main); break;
    }
}
