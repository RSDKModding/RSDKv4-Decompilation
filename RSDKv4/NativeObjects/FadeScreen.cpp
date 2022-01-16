#include "RetroEngine.hpp"

void FadeScreen_Create(void *objPtr)
{
    RSDK_THIS(FadeScreen);
    entity->timer           = 0.0;
    entity->delay           = 1.5;
    entity->fadeSpeed       = 2.0;
    entity->state           = FADESCREEN_STATE_GAMEFADEOUT;
    Engine.nativeMenuFadeIn = true;
}
void FadeScreen_Main(void *objPtr)
{
    RSDK_THIS(FadeScreen);

#if !RETRO_USE_ORIGINAL_CODE
    MatrixF render, temp;
    NewRenderState();
    matrixScaleXYZF(&render, Engine.windowScale, Engine.windowScale, 1.0);
    matrixTranslateXYZF(&temp, 0.0, 0.0, 160.0);
    matrixMultiplyF(&render, &temp);
    SetRenderMatrix(&render);
#endif

    SetRenderBlendMode(RENDER_BLEND_ALPHA);
    entity->timer += entity->fadeSpeed * Engine.deltaTime;
    switch (entity->state) {
        case FADESCREEN_STATE_MENUFADEIN:
            entity->fadeA = ((entity->delay - entity->timer) * 256.0f);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, entity->fadeR, entity->fadeG, entity->fadeB,
                       entity->fadeA);
            if (entity->timer > entity->delay) {
                RemoveNativeObject(entity);
                Engine.nativeMenuFadeIn = false;
                SetMusicTrack("MainMenu.ogg", 0, true, 106596);
                PlayMusic(0, 0);
            }
            break;
        case FADESCREEN_STATE_FADEOUT:
            entity->fadeA = entity->timer * 256.0;
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, entity->fadeR, entity->fadeG, entity->fadeB,
                       entity->fadeA);
            if (entity->timer > entity->delay)
                RemoveNativeObject(entity);
            break;
        case FADESCREEN_STATE_GAMEFADEOUT:
            SetMusicVolume(masterVolume - 2);

            entity->fadeA = 256.0 * entity->timer;
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, entity->fadeR, entity->fadeG, entity->fadeB,
                       entity->fadeA);
            if (entity->timer > entity->delay) {
                ClearNativeObjects();
                CREATE_ENTITY(RetroGameLoop);
                if (Engine.gameDeviceType == RETRO_MOBILE)
                    CREATE_ENTITY(VirtualDPad);
            }
            break;
#if !RETRO_USE_ORIGINAL_CODE
        case FADESCREEN_STATE_FADEIN:
            entity->fadeA = ((entity->delay - entity->timer) * 256.0f);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, entity->fadeR, entity->fadeG, entity->fadeB,
                       entity->fadeA);
            if (entity->timer > entity->delay) {
                RemoveNativeObject(entity);
                Engine.nativeMenuFadeIn = false;
            }
            break;
#endif
    }
}
