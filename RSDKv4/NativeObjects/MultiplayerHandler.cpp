#include "RetroEngine.hpp"

void MultiplayerHandler_Create(void *objPtr)
{
    RSDK_THIS(MultiplayerHandler);

    entity->pingLabel         = CREATE_ENTITY(TextLabel);
    entity->pingLabel->fontID = FONT_TEXT;
    entity->pingLabel->x      = SCREEN_XSIZE_F / 2 - 64.0f;
    entity->pingLabel->y      = -(SCREEN_YSIZE_F / 2) + 4.0f;
    entity->pingLabel->alignPtr(entity->pingLabel, 0);
    entity->pingLabel->scale      = 0.175;
    entity->pingLabel->useColours = true;
    entity->pingLabel->r          = 0xFF;

    entity->state = 1;
}
void MultiplayerHandler_Main(void *objPtr)
{
    RSDK_THIS(MultiplayerHandler);
    char buf[0x30];
    switch (entity->state) {
        case 0:
            if (activeStageList != STAGELIST_REGULAR) {
                entity->state = 1;
                break;
            }
            entity->timer += Engine.deltaTime;
            if (entity->pingLabel->textWidth) {
                entity->pingLabel->alignPtr(entity->pingLabel, 0);
                entity->pingLabel->x -= 28.0f;
            }
            if (entity->timer >= 1.0f && !waitingForPing) {
                waitingForPing = true;
                entity->timer  = 0;
                if (lastPing < 0.8f) {
                    sprintf(buf, "Ping: %.1fms", lastPing);
                    entity->pingLabel->g = 0xFF;
                    entity->pingLabel->b = 0xFF;
                }
                else if (lastPing < 2.0f) {
                    entity->pingLabel->g = 0xFF;
                    entity->pingLabel->b = 0x30;
                    sprintf(buf, "Ping: %.2fs", lastPing);
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
                entity->pingLabel->alignPtr(entity->pingLabel, 1);
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
            else if (activeStageList == STAGELIST_REGULAR)
                entity->state = 0;
            break;
        case 2:
            // display error
            StopMusic(true);
            RemoveNativeObject(entity->pingLabel);
            RemoveNativeObjectType(RetroGameLoop_Create, RetroGameLoop_Main);
            RemoveNativeObjectType(VirtualDPad_Create, VirtualDPad_Main);
            // entity->errorPanel              = CREATE_ENTITY(DialogPanel);
            // entity->errorPanel->buttonCount = DLGTYPE_OK;
            char *set;
            switch (dcError) {
                case 1: set = (char *)"The other player has disconnected. Returning to title screen."; break;
                case 2: set = (char *)"Connection timed out. Returning to title screen."; break;
                default: set = (char *)"You shouldn't get this message! If you do, message me."; break;
            }
            // SetStringToFont8(entity->errorPanel->text, set, FONT_TEXT);
            entity->state = 3;
            // FallThrough
        case 3:
            RenderRetroBuffer(64, 160);
            if (!entity->fade) {
                // if (false && entity->errorPanel->selection) {
                entity->fade        = CREATE_ENTITY(FadeScreen);
                entity->fade->state = 1;
                //}
            }
            else if (entity->fade->timer > entity->fade->timeLimit) {
                ClearNativeObjects();
                CREATE_ENTITY(SegaSplash);
                RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, 0);
            }
            break;
    }
}