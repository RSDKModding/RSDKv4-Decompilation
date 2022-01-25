#include "RetroEngine.hpp"

#if !RETRO_USE_ORIGINAL_CODE && RETRO_USE_NETWORKING
void MultiplayerHandler_Create(void *objPtr)
{
    RSDK_THIS(MultiplayerHandler);

    entity->pingLabel         = CREATE_ENTITY(TextLabel);
    entity->pingLabel->fontID = FONT_TEXT;
    entity->pingLabel->x      = SCREEN_XSIZE_F / 2 - 64.0f;
    entity->pingLabel->y      = -(SCREEN_YSIZE_F / 2) + 4.0f;
    entity->pingLabel->alignPtr(entity->pingLabel, ALIGN_LEFT);
    entity->pingLabel->scale      = 0.175;
    entity->pingLabel->useColours = true;
    entity->pingLabel->r          = 0xFF;

    entity->state = 1;
}
void MultiplayerHandler_Main(void *objPtr)
{
    RSDK_THIS(MultiplayerHandler);
    char buf[0x30];
    if (dcError && entity->state != 3)
        entity->state = 2;

    switch (entity->state) {
        case 0:
            if (!(activeStageList & 1)) {
                entity->state = 1;
                break;
            }
            entity->timer += Engine.deltaTime;
            if (entity->pingLabel->alignOffset) {
                entity->pingLabel->alignPtr(entity->pingLabel, ALIGN_LEFT);
                entity->pingLabel->x -= 28.0f;
            }
            if (entity->timer >= 0.25f && !waitingForPing) {
                waitingForPing = true;
                entity->timer  = 0;
                if (lastPing < 800.0f) {
                    sprintf(buf, "Ping: %.1fms", lastPing);
                    entity->pingLabel->g = 0xFF;
                    entity->pingLabel->b = 0xFF;
                }
                else if (lastPing < 2000.0f) {
                    entity->pingLabel->g = 0xFF;
                    entity->pingLabel->b = 0x30;
                    sprintf(buf, "Ping: %.2fs", lastPing / 1000);
                }
                else {
                    entity->pingLabel->g = 0x30;
                    entity->pingLabel->b = 0x30;
                    sprintf(buf, "Ping: %.0fs", entity->timer);
                }
                SetStringToFont8(entity->pingLabel->text, buf, FONT_TEXT);
            }
            else if (entity->timer >= 10.0f) {
                disconnectNetwork();
                dcError       = 2; // timeout
                entity->state = 2;
                vsPlaying     = false;
            }
            else if (entity->timer >= 5.0f && waitingForPing) {
                entity->pingLabel->g = 0xCF * (fmod(entity->timer, .5) >= .25) + 0x30;
                entity->pingLabel->b = 0xCF * (fmod(entity->timer, .5) >= .25) + 0x30;
                entity->pingLabel->alignPtr(entity->pingLabel, ALIGN_CENTER);
                entity->pingLabel->x += 28.0f;
                sprintf(buf, " !! %.2fs !!", 10 - entity->timer);
                SetStringToFont8(entity->pingLabel->text, buf, FONT_TEXT);
            }
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderRect(SCREEN_XSIZE_F / 2 - 68, -(SCREEN_YSIZE_F / 2) + 16, 160, 68, 16, 0, 0, 0, 0x80);
            // FallThrough
        case 1:
            // wait for regular
            if (!vsPlaying) {
                if (dcError) {
                    entity->state = 2;
                    break;
                }
                RemoveNativeObject(entity->pingLabel);
                RemoveNativeObject(entity->errorPanel);
                RemoveNativeObject(entity);
                break;
            }
            else if (activeStageList & 1)
                entity->state = 0;
            break;
        case 2: {
            // display error
            StopMusic(true);
            RemoveNativeObject(entity->pingLabel);
            RemoveNativeObjectType(RetroGameLoop_Create, RetroGameLoop_Main);
            RemoveNativeObjectType(VirtualDPad_Create, VirtualDPad_Main);
            RemoveNativeObjectType(FadeScreen_Create, FadeScreen_Main);

            entity->fadeError               = CREATE_ENTITY(FadeScreen);
            entity->fadeError->state        = FADESCREEN_STATE_FADEOUT;
            entity->errorPanel              = CREATE_ENTITY(DialogPanel);
            entity->errorPanel->buttonCount = DLGTYPE_OK;
            char *set                       = NULL;
            switch (dcError) {
                case 1: set = (char *)"The other player has disconnected.\rReturning to title screen."; break;
                case 2: set = (char *)"Connection timed out.\rReturning to title screen."; break;
                case 3: set = (char *)"This room is full.\rReturning to title screen.";
                // fallthrough
                case 5:
                    if (!set)
                        set = (char *)"This room doesn't exist.\rReturning to title screen.";
                    entity->fadeError->timer = entity->fadeError->delay;
                    for (int i = 0; i < nativeEntityCount; ++i) {
                        if (objectEntityBank[activeEntityList[i]].mainPtr == MultiplayerScreen_Main) {
                            MultiplayerScreen_Destroy(&objectEntityBank[i]);
                            break;
                        }
                    }
                    break;
                case 4: set = (char *)"Couldn't connect after 10 retries.\rReturning to title screen."; break;
            }
            SetStringToFont8(entity->errorPanel->text, set, FONT_TEXT);
            entity->state = 3;
            dcError       = 0;
        }
            // FallThrough
        case 3:
            RenderRetroBuffer(256, 160);
            if (entity->errorPanel->state == DIALOGPANEL_STATE_EXIT)
                entity->errorPanel->state = DIALOGPANEL_STATE_IDLE;
            if (entity->fadeError->timer >= entity->fadeError->delay) {
                RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, 255);
            }
            if (!entity->fade) {
                if (entity->errorPanel->selection) {
                    entity->fade        = CREATE_ENTITY(FadeScreen);
                    entity->fade->state = FADESCREEN_STATE_FADEOUT;
                }
            }
            else if (entity->fade->timer > entity->fade->delay) {
                ClearNativeObjects();
                RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, 255);
                Engine.nativeMenuFadeIn = false;
				if (skipStartMenu) {
					ClearGraphicsData();
					ClearAnimationData();
					activeStageList   = 0;
					stageMode         = STAGEMODE_LOAD;
					Engine.gameMode   = ENGINE_MAINGAME;
					stageListPosition = 0;
					CREATE_ENTITY(RetroGameLoop);
					if (Engine.gameDeviceType == RETRO_MOBILE)
						CREATE_ENTITY(VirtualDPad);
				} else {
					DrawRectangle(0, 0, SCREEN_XSIZE, SCREEN_YSIZE, 0x00, 0x00, 0x00, 0xFF); // This is probably hacky don't tell rdc or i'm going to jail
					CREATE_ENTITY(SegaSplash);
				}
            }
            break;
    }
}
#endif
