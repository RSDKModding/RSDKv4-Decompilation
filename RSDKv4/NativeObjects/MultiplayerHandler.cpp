#include "RetroEngine.hpp"

#if !RETRO_USE_ORIGINAL_CODE && RETRO_USE_NETWORKING
void MultiplayerHandler_Create(void *objPtr)
{
    RSDK_THIS(MultiplayerHandler);

    self->pingLabel         = CREATE_ENTITY(TextLabel);
    self->pingLabel->fontID = FONT_TEXT;
    self->pingLabel->x      = SCREEN_XSIZE_F / 2 - 64.0f;
    self->pingLabel->y      = -(SCREEN_YSIZE_F / 2) + 4.0f;
    self->pingLabel->alignPtr(self->pingLabel, ALIGN_LEFT);
    self->pingLabel->scale      = 0.175;
    self->pingLabel->useColors = true;
    self->pingLabel->r          = 0xFF;

    self->state = 1;
}
void MultiplayerHandler_Main(void *objPtr)
{
    RSDK_THIS(MultiplayerHandler);
    char buf[0x30];
    if (dcError && self->state != 3)
        self->state = 2;

    switch (self->state) {
        case 0:
            if (!(activeStageList & 1)) {
                self->state = 1;
                break;
            }

            self->timer += Engine.deltaTime;
            if (self->pingLabel->alignOffset) {
                self->pingLabel->alignPtr(self->pingLabel, ALIGN_LEFT);
                self->pingLabel->x -= 28.0f;
            }

            if (self->timer >= 0.25f && !waitingForPing) {
                waitingForPing = true;
                self->timer  = 0;
                if (lastPing < 800.0f) {
                    sprintf(buf, "Ping: %.1fms", lastPing);
                    self->pingLabel->g = 0xFF;
                    self->pingLabel->b = 0xFF;
                }
                else if (lastPing < 2000.0f) {
                    self->pingLabel->g = 0xFF;
                    self->pingLabel->b = 0x30;
                    sprintf(buf, "Ping: %.2fs", lastPing / 1000);
                }
                else {
                    self->pingLabel->g = 0x30;
                    self->pingLabel->b = 0x30;
                    sprintf(buf, "Ping: %.0fs", self->timer);
                }
                SetStringToFont8(self->pingLabel->text, buf, FONT_TEXT);
            }
            else if (self->timer >= 10.0f) {
                DisconnectNetwork();
                dcError       = 2; // timeout
                self->state = 2;
                vsPlaying     = false;
            }
            else if (self->timer >= 5.0f && waitingForPing) {
                self->pingLabel->g = 0xCF * (fmod(self->timer, .5) >= .25) + 0x30;
                self->pingLabel->b = 0xCF * (fmod(self->timer, .5) >= .25) + 0x30;
                self->pingLabel->alignPtr(self->pingLabel, ALIGN_CENTER);
                self->pingLabel->x += 28.0f;
                sprintf(buf, " !! %.2fs !!", 10 - self->timer);
                SetStringToFont8(self->pingLabel->text, buf, FONT_TEXT);
            }
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderRect(SCREEN_XSIZE_F / 2 - 68, -(SCREEN_YSIZE_F / 2) + 16, 160, 68, 16, 0, 0, 0, 0x80);
            // FallThrough
        case 1:
            // wait for regular
            if (!vsPlaying) {
                if (dcError) {
                    self->state = 2;
                    break;
                }
                RemoveNativeObject(self->pingLabel);
                RemoveNativeObject(self->errorPanel);
                RemoveNativeObject(self);
                break;
            }
            else if (activeStageList & 1)
                self->state = 0;
            break;

        case 2: {
            // display error
            StopMusic(true);
            RemoveNativeObject(self->pingLabel);
            RemoveNativeObjectType(RetroGameLoop_Create, RetroGameLoop_Main);
            RemoveNativeObjectType(VirtualDPad_Create, VirtualDPad_Main);
            RemoveNativeObjectType(FadeScreen_Create, FadeScreen_Main);

            self->fadeError               = CREATE_ENTITY(FadeScreen);
            self->fadeError->state        = FADESCREEN_STATE_FADEOUT;
            self->errorPanel              = CREATE_ENTITY(DialogPanel);
            self->errorPanel->buttonCount = DLGTYPE_OK;
            char *set                       = NULL;
            switch (dcError) {
                case 1: set = (char *)"The other player has disconnected.\rReturning to title screen."; break;
                case 2: set = (char *)"Connection timed out.\rReturning to title screen."; break;
                case 3: set = (char *)"This room is full.\rReturning to title screen.";
                // fallthrough
                case 5:
                    if (!set)
                        set = (char *)"This room doesn't exist.\rReturning to title screen.";
                    self->fadeError->timer = self->fadeError->delay;
                    for (int i = 0; i < nativeEntityCount; ++i) {
                        if (objectEntityBank[activeEntityList[i]].eventMain == MultiplayerScreen_Main) {
                            MultiplayerScreen_Destroy(&objectEntityBank[i]);
                            break;
                        }
                    }
                    break;
                case 4: set = (char *)"Couldn't connect after 10 retries.\rReturning to title screen."; break;
            }
            SetStringToFont8(self->errorPanel->text, set, FONT_TEXT);
            self->state = 3;
            dcError       = 0;
        }
            // FallThrough
        case 3:
            RenderRetroBuffer(256, 160);
            if (self->errorPanel->state == DIALOGPANEL_STATE_EXIT)
                self->errorPanel->state = DIALOGPANEL_STATE_IDLE;
            if (self->fadeError->timer >= self->fadeError->delay) {
                RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, 255);
            }

            if (!self->fade) {
                if (self->errorPanel->selection) {
                    self->fade        = CREATE_ENTITY(FadeScreen);
                    self->fade->state = FADESCREEN_STATE_FADEOUT;
                }
            }

            else if (self->fade->timer > self->fade->delay) {
                ClearNativeObjects();
                Engine.nativeMenuFadeIn = false;
                if (skipStartMenu) {
                    activeStageList   = 0;
                    stageMode         = STAGEMODE_LOAD;
                    Engine.gameMode   = ENGINE_MAINGAME;
                    stageListPosition = 0;
                    CREATE_ENTITY(RetroGameLoop);
                    if (Engine.gameDeviceType == RETRO_MOBILE)
                        CREATE_ENTITY(VirtualDPad);
                }
                else {
                    RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, 255);
                    CREATE_ENTITY(SegaSplash);
                }
            }
            break;
    }
}
#endif
