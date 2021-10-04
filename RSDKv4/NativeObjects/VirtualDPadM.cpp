#include "RetroEngine.hpp"

void VirtualDPadM_Create(void *objPtr)
{
    RSDK_THIS(VirtualDPadM);
    SaveGame *saveGame = (SaveGame *)saveRAM;

    entity->moveX    = 60.0f - SCREEN_CENTERX;
    entity->moveY    = -64.0;
    entity->jumpX    = SCREEN_CENTERX - 56.0;
    entity->pauseY   = 104.0;
    entity->jumpY    = -68.0;
    entity->pauseX   = SCREEN_CENTERX - 76.0f;
    entity->pauseX_S = SCREEN_CENTERX - 52.0f;

    float dpadSize    = saveGame->vDPadSize * (1 / 256.0f);
    entity->moveSize  = dpadSize;
    entity->jumpSize  = entity->moveSize * 0.75;
    entity->textureID = LoadTexture("Data/Game/Menu/VirtualDPadM.png", 3);
}
void VirtualDPadM_Main(void *objPtr)
{
    RSDK_THIS(VirtualDPadM);
    SaveGame *saveGame   = (SaveGame *)saveRAM;
    int physicalControls = GetGlobalVariableByName("options.physicalControls");

    if (physicalControls) {
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

        if (entity->alpha == saveGame->vDPadOpacity) {
            if (inputDown.up) {
                RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, 25.0, 30.0, 50.0, 55.0, 24.0, 282.0,
                            entity->alpha, entity->textureID);
            }
            else if (inputDown.down) {
                RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, 26.0, 0.0, 52.0, 128.0, 102.0, 384.0,
                            entity->alpha, entity->textureID);
            }
            else if (inputDown.left) {
                RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, 128.0, 29.0, 128.0, 59.0, 0.0, 355.0,
                            entity->alpha, entity->textureID);
            }
            else if (inputDown.right) {
                RenderImage(entity->moveX, entity->moveY, 160.0, entity->moveSize, entity->moveSize, 0.0, 29.0, 128.0, 59.0, 128.0, 355.0,
                            entity->alpha, entity->textureID);
            }
        }

        int alpha  = 0xFF;
        float sprY = 0;
        if (inputDown.A && entity->alpha == saveGame->vDPadOpacity) {
            alpha = 255;
            sprY  = 256.0;
        }
        else {
            alpha = entity->alpha;
            sprY  = 0.0;
        }

        RenderImage(entity->jumpX, entity->jumpY, 160.0, entity->jumpSize, entity->jumpSize, 128.0, 128.0, 256.0, 256.0, 256.0, sprY, entity->alpha,
                    entity->textureID);

        if (Engine.gameMode == ENGINE_MAINGAME) {
            if (activeStageList == STAGELIST_SPECIAL)
                RenderImage(entity->pauseX_S, entity->pauseY, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, entity->pauseAlpha,
                            entity->textureID);
            else
                RenderImage(entity->pauseX, entity->pauseY, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, entity->pauseAlpha,
                            entity->textureID);
        }
    }
}