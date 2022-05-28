#include "RetroEngine.hpp"

void FadeScreen_Create(void *objPtr)
{
    RSDK_THIS(FadeScreen);
    self->timer             = 0.0;
    self->delay             = 1.5;
    self->fadeSpeed         = 2.0;
    self->state             = FADESCREEN_STATE_GAMEFADEOUT;
    Engine.nativeMenuFadeIn = true;
}
void FadeScreen_Main(void *objPtr)
{
    RSDK_THIS(FadeScreen);

    SetRenderBlendMode(RENDER_BLEND_ALPHA);
    self->timer += self->fadeSpeed * Engine.deltaTime;
    switch (self->state) {
        case FADESCREEN_STATE_MENUFADEIN:
            self->fadeA = ((self->delay - self->timer) * 256.0f);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, self->fadeR, self->fadeG, self->fadeB, self->fadeA);
            if (self->timer > self->delay) {
                RemoveNativeObject(self);
                Engine.nativeMenuFadeIn = false;
                SetMusicTrack("MainMenu.ogg", 0, true, 106596);
                PlayMusic(0, 0);
            }
            break;

        case FADESCREEN_STATE_FADEOUT:
            self->fadeA = self->timer * 256.0;
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, self->fadeR, self->fadeG, self->fadeB,
                       self->fadeA);
            if (self->timer > self->delay)
                RemoveNativeObject(self);
            break;

        case FADESCREEN_STATE_GAMEFADEOUT:
            self->fadeA = self->timer * 256.0;
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, self->fadeR, self->fadeG, self->fadeB,
                       self->fadeA);
            SetMusicVolume(masterVolume - 2);

            if (self->timer > self->delay) {
                ClearNativeObjects();
                CREATE_ENTITY(RetroGameLoop);
                if (Engine.gameDeviceType == RETRO_MOBILE)
                    CREATE_ENTITY(VirtualDPad);
            }
            break;

#if !RETRO_USE_ORIGINAL_CODE
        case FADESCREEN_STATE_FADEIN:
            self->fadeA = ((self->delay - self->timer) * 256.0f);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, self->fadeR, self->fadeG, self->fadeB,
                       self->fadeA);
            if (self->timer > self->delay) {
                RemoveNativeObject(self);
                Engine.nativeMenuFadeIn = false;
            }
            break;
#endif
    }

#if !RETRO_USE_ORIGINAL_CODE
    NewRenderState();
    MatrixScaleXYZF(&self->render, Engine.windowScale, Engine.windowScale, 1.0);
    MatrixTranslateXYZF(&self->temp, 0.0, 0.0, 160.0);
    MatrixMultiplyF(&self->render, &self->temp);
    SetRenderMatrix(&self->render);
#endif
}
