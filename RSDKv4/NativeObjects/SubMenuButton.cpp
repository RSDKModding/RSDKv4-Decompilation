#include "RetroEngine.hpp"

void SubMenuButton_Create(void *objPtr)
{
    RSDK_THIS(SubMenuButton);
    self->matZ           = 160.0;
    self->alpha          = 0xFF;
    self->state          = SUBMENUBUTTON_STATE_IDLE;
    self->matXOff        = 0.0;
    self->r              = 0xFF;
    self->g              = 0xFF;
    self->b              = 0xFF;
    self->textureSymbols = LoadTexture("Data/Game/Menu/Symbols.png", TEXFMT_RGBA4444);
    self->meshButton     = LoadMesh("Data/Game/Models/Button.bin", 255);
    self->meshButtonH    = LoadMesh("Data/Game/Models/ButtonH.bin", 255);
    SetMeshVertexColors(self->meshButton, 0, 0, 0, 0xC0);
    SetMeshVertexColors(self->meshButtonH, 0xA0, 0, 0, 0xC0);
}
void SubMenuButton_Main(void *objPtr)
{
    RSDK_THIS(SubMenuButton);
    if (self->useMatrix) {
        NewRenderState();
        MatrixTranslateXYZF(&self->renderMatrix, self->matX - self->matXOff, self->matY, self->matZ);
        MatrixMultiplyF(&self->renderMatrix, &self->matrix);
        SetRenderMatrix(&self->renderMatrix);
    }
    SetRenderVertexColor(self->r, self->g, self->b);

    switch (self->state) {
        case SUBMENUBUTTON_STATE_IDLE: {
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderMesh(self->meshButton, MESH_COLORS, false);
            RenderText(self->text, FONT_LABEL, -80.0, self->textY, 0, self->scale, self->alpha);
            break;
        }
        case SUBMENUBUTTON_STATE_FLASHING1: {
            self->flashTimer += Engine.deltaTime;
            if (self->flashTimer > 1.0)
                self->flashTimer -= 1.0;

            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderMesh(self->meshButton, MESH_COLORS, false);
            if (self->flashTimer > 0.5)
                RenderText(self->text, FONT_LABEL, -80.0, self->textY, 0, self->scale, self->alpha);
            break;
        }
        case SUBMENUBUTTON_STATE_FLASHING2: {
            self->flashTimer += Engine.deltaTime;
            if (self->flashTimer > 0.1)
                self->flashTimer -= 0.1;
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderMesh(self->meshButton, MESH_COLORS, false);
            if (self->flashTimer > 0.05)
                RenderText(self->text, FONT_LABEL, -80.0, self->textY, 0, self->scale, self->alpha);

            self->afterFlashTimer += Engine.deltaTime;
            if (self->afterFlashTimer > 0.5) {
                self->afterFlashTimer = 0.0;
                self->state           = SUBMENUBUTTON_STATE_IDLE;
            }
            break;
        }
        case SUBMENUBUTTON_STATE_SAVEBUTTON_UNSELECTED: {
            self->flashTimer += Engine.deltaTime;
            if (self->flashTimer > 0.1)
                self->flashTimer -= 0.1;

            self->afterFlashTimer += Engine.deltaTime;
            if (self->afterFlashTimer > 0.5) {
                self->flashTimer      = 0.0;
                self->afterFlashTimer = 0.0;
                self->state           = SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED;
            }
            // FallThrough
        }
        case SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED: {
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            if (self->useMeshH)
                RenderMesh(self->meshButtonH, MESH_COLORS, false);
            else
                RenderMesh(self->meshButton, MESH_COLORS, false);
            if (self->flashTimer < 0.05)
                RenderText(self->text, FONT_LABEL, -64.0, self->textY, 0, self->scale, self->alpha);

            switch (self->symbol) {
                case 0: RenderImage(-76.0, 0.0, 0.0, 0.3, 0.35, 28.0, 43.0, 56.0, 86.0, 0.0, 170.0, 255, self->textureSymbols); break;
                case 1: RenderImage(-76.0, 0.0, 0.0, 0.3, 0.35, 34.0, 43.0, 68.0, 86.0, 58.0, 170.0, 255, self->textureSymbols); break;
                case 2: RenderImage(-76.0, 0.0, 0.0, 0.3, 0.35, 29.0, 43.0, 58.0, 86.0, 130.0, 170.0, 255, self->textureSymbols); break;
                case 3:
                    RenderImage(-76.0, 0.0, 0.0, 0.3, 0.35, 34.0, 43.0, 68.0, 86.0, 58.0, 170.0, 255, self->textureSymbols);
                    RenderImage(-84.0, 0.0, 0.0, 0.3, 0.35, 28.0, 43.0, 56.0, 86.0, 0.0, 170.0, 255, self->textureSymbols);
                    break;
            }

            uint emeraldColorsS1[] = {
                0x8080FF, 0xFFFF00, 0xFF60C0, 0xA0FF00, 0xFF4060, 0xFFFFFF,
            };
            uint emeraldColorsS2[] = {
                0x00C0FF, 0x8000C0, 0xFF0000, 0xFF60FF, 0xFFC000, 0x60C000, 0xFFFFFF,
            };

            float x = -60.0f;
            for (int i = 0; i < (Engine.gameType == GAME_SONIC1 ? 6 : 7); ++i) {
                if (self->flags & (1 << i)) {
                    if (Engine.gameType == GAME_SONIC1)
                        SetRenderVertexColor((emeraldColorsS1[i] >> 16) & 0xFF, (emeraldColorsS1[i] >> 8) & 0xFF, emeraldColorsS1[i] & 0xFF);
                    else
                        SetRenderVertexColor((emeraldColorsS2[i] >> 16) & 0xFF, (emeraldColorsS2[i] >> 8) & 0xFF, emeraldColorsS2[i] & 0xFF);
                    RenderImage(x, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 188.0, 0.0, 255, self->textureSymbols);
                }
                else {
                    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
                    RenderImage(x, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 133.0, 0.0, 255, self->textureSymbols);
                }

                x += 16.0f;
            }
            break;
        }
        default: break;
    }

    SetRenderVertexColor(255, 255, 255);
    if (self->useMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}
