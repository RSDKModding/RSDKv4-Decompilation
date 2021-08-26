#include "RetroEngine.hpp"

void VirtualDPad_Create(void *objPtr)
{
    RSDK_THIS(VirtualDPad);
    SaveGame *saveGame = (SaveGame *)saveRAM;

    float screenXCenter = SCREEN_CENTERX;
    float screenYCenter = SCREEN_CENTERY;
    entity->moveX       = (float)saveGame->vDPadX_Move - screenXCenter;
    entity->moveY       = -((float)saveGame->vDPadY_Move - screenYCenter);
    entity->jumpX       = saveGame->vDPadX_Jump + screenXCenter;
    entity->pauseY      = 104.0;
    entity->jumpY       = -(float)(saveGame->vDPadY_Jump - screenYCenter);
    entity->pauseX      = screenXCenter - 76.0f;
    entity->pauseX_S    = screenXCenter - 52.0f;
    entity->dword5C     = -1;
    entity->dword60     = -1;

    float dpadSize              = saveGame->vDPadSize * (1 / 256.0f);
    entity->moveSize            = dpadSize;
    entity->jumpSize            = dpadSize;
    entity->unknownSize         = dpadSize * 0.85000002;
    entity->useTouchControls    = GetGlobalVariableID("options.touchControls");
    entity->usePhysicalControls = GetGlobalVariableID("options.physicalControls");
    entity->isVSMode            = GetGlobalVariableID("options.vsMode");
    entity->textureID           = LoadTexture("Data/Game/Menu/VirtualDPad.png", 3);
}
void VirtualDPad_Main(void *objPtr)
{
    RSDK_THIS(VirtualDPad);
    SaveGame *saveGame = (SaveGame *)saveRAM;

    if (globalVariables[entity->useTouchControls] && (globalVariables[entity->usePhysicalControls] || entity->field_70 == 1)) {
        if (entity->alpha < saveGame->vDPadOpacity) {
            entity->alpha += 4;
            if (entity->pauseAlpha <= 254) {
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
        SetRenderBlendMode(1);
        RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, 128.0, 128.0, 256.0, 256.0, 0.0, 0.0, entity->alpha,
                    entity->textureID);

        if (entity->alpha != saveGame->vDPadOpacity) {
            entity->field_24 = 0.0;
            entity->field_28 = 0.0;
        }
        else if (keyDown.up) {
            RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, 128.0, 128.0, 256.0, 120.0, 256.0, 256.0,
                        entity->alpha, entity->textureID);

            entity->field_24 = 0.0;
            entity->field_28 = 20.0;
        }
        else if (keyDown.down) {
            RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, 128.0, -8.0, 256.0, 120.0, 256.0, 392.0,
                        entity->alpha, entity->textureID);

            entity->field_24 = 0.0;
            entity->field_28 = -20.0;
        }
        else if (keyDown.left) {
            RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, 128.0, 128.0, 120.0, 256.0, 256.0, 256.0,
                        entity->alpha, entity->textureID);

            entity->field_24 = 20.0;
            entity->field_28 = 0.0;
        }
        else if (keyDown.right) {
            RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, -8.0, 128.0, 120.0, 256.0, 392.0, 256.0,
                        entity->alpha, entity->textureID);

            entity->field_24 = -20.0;
            entity->field_28 = 0.0;
        }
        else {
            entity->field_24 = 0.0;
            entity->field_28 = 0.0;
        }

        entity->float1C += (entity->field_24 - entity->float1C) * 0.25f;
        entity->float20 += (entity->field_28 - entity->float20) * 0.25f;
        RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, entity->float1C + 84.0, entity->float20 + 84.0, 168.0,
                    168.0, 16.0, 328.0, entity->alpha, entity->textureID);
        RenderImage(entity->jumpX, entity->jumpY, 160.0, entity->unknownSize, entity->unknownSize, 128.0, 128.0, 256.0, 256.0, 256.0, 0.0,
                    entity->alpha, entity->textureID);

        float size = 0.0f;
        if (entity->alpha == saveGame->vDPadOpacity && (keyDown.C || keyDown.A || keyDown.B))
            size = entity->unknownSize;
        else
            size = entity->jumpSize;
        RenderImage(entity->jumpX, entity->jumpY, 160.0, size, size, 84.0, 83.0, 168.0, 168.0, 16.0, 328.0, entity->alpha, entity->textureID);

        if (Engine.gameMode == ENGINE_MAINGAME) {
            if (!globalVariables[entity->isVSMode]) {
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