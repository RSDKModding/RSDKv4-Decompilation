#include "RetroEngine.hpp"

void VirtualDPadM_Create(void *objPtr)
{
    RSDK_THIS(VirtualDPadM);
    SaveGame *saveGame = (SaveGame *)saveRAM;

    self->moveX    = 60.0f - SCREEN_CENTERX;
    self->moveY    = -64.0;
    self->jumpX    = SCREEN_CENTERX - 56.0;
    self->pauseY   = 104.0;
    self->jumpY    = -68.0;
    self->pauseX   = SCREEN_CENTERX - 76.0f;
    self->pauseX_S = SCREEN_CENTERX - 52.0f;

    float dpadSize  = saveGame->vDPadSize * (1 / 256.0f);
    self->moveSize  = dpadSize;
    self->jumpSize  = self->moveSize * 0.75;
    self->textureID = LoadTexture("Data/Game/Menu/VirtualDPadM.png", 3);
}
void VirtualDPadM_Main(void *objPtr)
{
    RSDK_THIS(VirtualDPadM);
    SaveGame *saveGame   = (SaveGame *)saveRAM;
    int physicalControls = GetGlobalVariableByName("options.physicalControls");

    if (physicalControls) {
        if (self->alpha < saveGame->vDPadOpacity) {
            self->alpha += 4;
            if (self->pauseAlpha < 0xFF) {
                self->pauseAlpha = (self->alpha << 8) / saveGame->vDPadOpacity;
            }
        }
    }
    else {
        if (self->alpha > 0) {
            self->alpha -= 4;
            self->pauseAlpha = (self->alpha << 8) / saveGame->vDPadOpacity;
        }
    }

    if (self->alpha > 0) {
        SetRenderBlendMode(RENDER_BLEND_ALPHA);
        RenderImage(self->moveX, self->moveY, 160.0, self->moveSize, self->moveSize, 128.0, 128.0, 256.0, 256.0, 0.0, 0.0, self->alpha,
                    self->textureID);

        if (self->alpha == saveGame->vDPadOpacity) {
            if (keyDown.up) {
                RenderImage(self->moveX, self->moveY, 160.0, self->moveSize, self->moveSize, 25.0, 30.0, 50.0, 55.0, 24.0, 282.0, self->alpha,
                            self->textureID);
            }
            else if (keyDown.down) {
                RenderImage(self->moveX, self->moveY, 160.0, self->moveSize, self->moveSize, 26.0, 0.0, 52.0, 128.0, 102.0, 384.0, self->alpha,
                            self->textureID);
            }
            else if (keyDown.left) {
                RenderImage(self->moveX, self->moveY, 160.0, self->moveSize, self->moveSize, 128.0, 29.0, 128.0, 59.0, 0.0, 355.0, self->alpha,
                            self->textureID);
            }
            else if (keyDown.right) {
                RenderImage(self->moveX, self->moveY, 160.0, self->moveSize, self->moveSize, 0.0, 29.0, 128.0, 59.0, 128.0, 355.0, self->alpha,
                            self->textureID);
            }
        }

        int alpha  = 0xFF;
        float sprY = 0;
        if (keyDown.A && self->alpha == saveGame->vDPadOpacity) {
            alpha = 255;
            sprY  = 256.0;
        }
        else {
            alpha = self->alpha;
            sprY  = 0.0;
        }

        RenderImage(self->jumpX, self->jumpY, 160.0, self->jumpSize, self->jumpSize, 128.0, 128.0, 256.0, 256.0, 256.0, sprY, self->alpha,
                    self->textureID);

        if (Engine.gameMode == ENGINE_MAINGAME) {
            if (activeStageList == STAGELIST_SPECIAL)
                RenderImage(self->pauseX_S, self->pauseY, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, self->pauseAlpha, self->textureID);
            else
                RenderImage(self->pauseX, self->pauseY, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, self->pauseAlpha, self->textureID);
        }
    }
}