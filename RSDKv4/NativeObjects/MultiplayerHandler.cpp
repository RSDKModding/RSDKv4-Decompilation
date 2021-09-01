#include "RetroEngine.hpp"

void MultiplayerHandler_Create(void *objPtr)
{
    RSDK_THIS(MultiplayerHandler);

    entity->pingLabel         = CREATE_ENTITY(TextLabel);
    entity->pingLabel->fontID = 2;
    entity->pingLabel->textX  = SCREEN_XSIZE_F - 48.0f;
    entity->pingLabel->textY  = SCREEN_YSIZE_F - 16.0f;
    entity->pingLabel->alignPtr(entity->pingLabel, 0);
}
void MultiplayerHandler_Main(void *objPtr)
{
    RSDK_THIS(MultiplayerHandler);
    char buf[0x30];
    if (lastPing < 200000000.0f) {
        sprintf(buf, "Ping: %.1fms", lastPing * 1000);
    }
    else
        buf[0] = 0;
    SetStringToFont8(entity->pingLabel->text, buf, 2);
}