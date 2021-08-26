#include "RetroEngine.hpp"

void FadeScreen_Create(void *objPtr)
{
    RSDK_THIS(FadeScreen);
    entity->timer     = 0.0;
    entity->timeLimit = 1.5;
    entity->fadeSpeed = 2.0;
    entity->state     = 2;
    Engine.nativeMenuFadeIn = true;
}
void FadeScreen_Main(void *objPtr)
{
    RSDK_THIS(FadeScreen);

    SetRenderBlendMode(RENDER_BLEND_ALPHA);
    entity->timer += entity->fadeSpeed * Engine.deltaTime;
    switch (entity->state) {
        case 0:
            entity->fadeA = ((entity->timeLimit - entity->timer) * 256.0f);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, entity->fadeR, entity->fadeG, entity->fadeB,
                       entity->fadeA);
            if (entity->timer > entity->timeLimit) {
                RemoveNativeObject(entity);
                Engine.nativeMenuFadeIn = false;
                SetMusicTrack("MainMenu.ogg", 0, 1, 106596);
                PlayMusic(0, 0);
            }
            break;
        case 1:
            entity->fadeA = entity->timer * 256.0;
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, entity->fadeR, entity->fadeG, entity->fadeB,
                       entity->fadeA);
            if (entity->timer > entity->timeLimit)
                RemoveNativeObject(entity);
            break;
        case 2:
            SetMusicVolume(bgmVolume - 2);

            entity->fadeA = 256.0f * entity->timer;
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, entity->fadeR, entity->fadeG, entity->fadeB,
                       entity->fadeA);
            if (entity->timer > entity->timeLimit) {
                ClearNativeObjects();
                CREATE_ENTITY(RetroGameLoop);
                if (Engine.gameDeviceType == RETRO_MOBILE)
                    CREATE_ENTITY(VirtualDPad);
            }
            break;
    }
}
