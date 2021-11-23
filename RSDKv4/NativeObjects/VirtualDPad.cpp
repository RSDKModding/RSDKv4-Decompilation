#include "RetroEngine.hpp"

void VirtualDPad_Create(void *objPtr)
{
    RSDK_THIS(VirtualDPad);
    SaveGame *saveGame = (SaveGame *)saveRAM;

    float screenXCenter = SCREEN_CENTERX;
    float screenYCenter = SCREEN_CENTERY;
    entity->moveX       = saveGame->vDPadX_Move - screenXCenter;
    entity->moveY       = -(saveGame->vDPadY_Move - screenYCenter);
    entity->jumpX       = saveGame->vDPadX_Jump + screenXCenter;
    entity->pauseY      = 104.0f;
    entity->jumpY       = -(saveGame->vDPadY_Jump - screenYCenter);
    entity->pauseX      = screenXCenter - 76.0f;
    entity->pauseX_S    = screenXCenter - 52.0f;
    entity->moveFinger  = -1;
    entity->jumpFinger  = -1;

    float dpadSize              = saveGame->vDPadSize * (1 / 256.0f);
    entity->moveSize            = dpadSize;
    entity->jumpSize            = dpadSize;
    entity->pressedSize         = dpadSize * 0.85;
    entity->useTouchControls    = GetGlobalVariableID("options.touchControls");
    entity->usePhysicalControls = GetGlobalVariableID("options.physicalControls");
    entity->vsMode              = GetGlobalVariableID("options.vsMode");
    entity->textureID           = LoadTexture("Data/Game/Menu/VirtualDPad.png", TEXFMT_RGBA8888);
}
void VirtualDPad_Main(void *objPtr)
{
    RSDK_THIS(VirtualDPad);
    SaveGame *saveGame = (SaveGame *)saveRAM;

    if (globalVariables[entity->useTouchControls] && (!globalVariables[entity->usePhysicalControls] || entity->editMode)) {
        if (entity->alpha < saveGame->vDPadOpacity) {
            entity->alpha += 4;
            if (entity->pauseAlpha < 0xFF) {
                entity->pauseAlpha = (entity->alpha << 8) / saveGame->vDPadOpacity;
            }
        }
    }
    else {
        if (entity->alpha > 0) {
            entity->alpha -= 4;
            entity->pauseAlpha = (entity->alpha << 8) / saveGame->vDPadOpacity;
        }
    }

    if (entity->alpha > 0) {
        SetRenderBlendMode(RENDER_BLEND_ALPHA);
        RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, 128.0, 128.0, 256.0, 256.0, 0.0, 0.0, entity->alpha,
                    entity->textureID);

        if (entity->alpha != saveGame->vDPadOpacity) {
            entity->offsetX = 0.0;
            entity->offsetY = 0.0;
        }
        else if (inputDown.up) {
            RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, 128.0, 128.0, 256.0, 120.0, 256.0, 256.0,
                        entity->alpha, entity->textureID);

            entity->offsetX = 0.0;
            entity->offsetY = 20.0;
        }
        else if (inputDown.down) {
            RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, 128.0, -8.0, 256.0, 120.0, 256.0, 392.0,
                        entity->alpha, entity->textureID);

            entity->offsetX = 0.0;
            entity->offsetY = -20.0;
        }
        else if (inputDown.left) {
            RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, 128.0, 128.0, 120.0, 256.0, 256.0, 256.0,
                        entity->alpha, entity->textureID);

            entity->offsetX = 20.0;
            entity->offsetY = 0.0;
        }
        else if (inputDown.right) {
            RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, -8.0, 128.0, 120.0, 256.0, 392.0, 256.0,
                        entity->alpha, entity->textureID);

            entity->offsetX = -20.0;
            entity->offsetY = 0.0;
        }
        else {
            entity->offsetX = 0.0;
            entity->offsetY = 0.0;
        }

        entity->pivotX += (entity->offsetX - entity->pivotX) * 0.25f;
        entity->pivotY += (entity->offsetY - entity->pivotY) * 0.25f;
        RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, entity->pivotX + 84.0, entity->pivotY + 84.0, 168.0,
                    168.0, 16.0, 328.0, entity->alpha, entity->textureID);
        RenderImage(entity->jumpX, entity->jumpY, 160.0, entity->pressedSize, entity->pressedSize, 128.0, 128.0, 256.0, 256.0, 256.0, 0.0,
                    entity->alpha, entity->textureID);

        float size = 0.0f;
        if (entity->alpha == saveGame->vDPadOpacity && (inputDown.C || inputDown.A || inputDown.B))
            size = entity->pressedSize;
        else
            size = entity->jumpSize;
        RenderImage(entity->jumpX, entity->jumpY, 160.0, size, size, 84.0, 83.0, 168.0, 168.0, 16.0, 328.0, entity->alpha, entity->textureID);

        if (Engine.gameMode == ENGINE_MAINGAME) {
            if (!globalVariables[entity->vsMode]) {
                if (activeStageList == STAGELIST_SPECIAL)
                    RenderImage(entity->pauseX_S, entity->pauseY, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, entity->pauseAlpha,
                                entity->textureID);
                else
                    RenderImage(entity->pauseX, entity->pauseY, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, entity->pauseAlpha,
                                entity->textureID);
            }
        }
    }
}