#include "RetroEngine.hpp"

void SubMenuButton_Create(void *objPtr)
{
    RSDK_THIS(SubMenuButton);
    entity->matZ           = 160.0;
    entity->alpha          = 0xFF;
    entity->state          = SUBMENUBUTTON_STATE_IDLE;
    entity->matXOff        = 0.0;
    entity->r              = 0xFF;
    entity->g              = 0xFF;
    entity->b              = 0xFF;
    entity->textureSymbols = LoadTexture("Data/Game/Menu/Symbols.png", TEXFMT_RGBA4444);
    entity->meshButton     = LoadMesh("Data/Game/Models/Button.bin", 255);
    entity->meshButtonH    = LoadMesh("Data/Game/Models/ButtonH.bin", 255);
    SetMeshVertexColors(entity->meshButton, 0, 0, 0, 0xC0);
    SetMeshVertexColors(entity->meshButtonH, 0xA0, 0, 0, 0xC0);
}
void SubMenuButton_Main(void *objPtr)
{
    RSDK_THIS(SubMenuButton);
    if (entity->useMatrix) {
        NewRenderState();
        matrixTranslateXYZF(&entity->renderMatrix, entity->matX - entity->matXOff, entity->matY, entity->matZ);
        matrixMultiplyF(&entity->renderMatrix, &entity->matrix);
        SetRenderMatrix(&entity->renderMatrix);
    }
    SetRenderVertexColor(entity->r, entity->g, entity->b);

    switch (entity->state) {
        case SUBMENUBUTTON_STATE_IDLE: {
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderMesh(entity->meshButton, MESH_COLOURS, false);
            RenderText(entity->text, FONT_LABEL, -80.0, entity->textY, 0, entity->scale, entity->alpha);
            break;
        }
        case SUBMENUBUTTON_STATE_FLASHING1: {
            entity->flashTimer += Engine.deltaTime;
            if (entity->flashTimer > 1.0)
                entity->flashTimer -= 1.0;

            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderMesh(entity->meshButton, MESH_COLOURS, false);
            if (entity->flashTimer > 0.5)
                RenderText(entity->text, FONT_LABEL, -80.0, entity->textY, 0, entity->scale, entity->alpha);
            break;
        }
        case SUBMENUBUTTON_STATE_FLASHING2: {
            entity->flashTimer += Engine.deltaTime;
            if (entity->flashTimer > 0.1)
                entity->flashTimer -= 0.1;
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderMesh(entity->meshButton, MESH_COLOURS, false);
            if (entity->flashTimer > 0.05)
                RenderText(entity->text, FONT_LABEL, -80.0, entity->textY, 0, entity->scale, entity->alpha);

            entity->afterFlashTimer += Engine.deltaTime;
            if (entity->afterFlashTimer > 0.5) {
                entity->afterFlashTimer = 0.0;
                entity->state           = SUBMENUBUTTON_STATE_IDLE;
            }
            break;
        }
        case SUBMENUBUTTON_STATE_SAVEBUTTON_UNSELECTED: {
            entity->flashTimer += Engine.deltaTime;
            if (entity->flashTimer > 0.1)
                entity->flashTimer -= 0.1;

            entity->afterFlashTimer += Engine.deltaTime;
            if (entity->afterFlashTimer > 0.5) {
                entity->flashTimer      = 0.0;
                entity->afterFlashTimer = 0.0;
                entity->state           = SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED;
            }
            // FallThrough
        }
        case SUBMENUBUTTON_STATE_SAVEBUTTON_SELECTED: {
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            if (entity->useMeshH)
                RenderMesh(entity->meshButtonH, MESH_COLOURS, false);
            else
                RenderMesh(entity->meshButton, MESH_COLOURS, false);
            if (entity->flashTimer < 0.05)
                RenderText(entity->text, FONT_LABEL, -64.0, entity->textY, 0, entity->scale, entity->alpha);

            switch (entity->symbol) {
                case 0: RenderImage(-76.0, 0.0, 0.0, 0.3, 0.35, 28.0, 43.0, 56.0, 86.0, 0.0, 170.0, 255, entity->textureSymbols); break;
                case 1: RenderImage(-76.0, 0.0, 0.0, 0.3, 0.35, 34.0, 43.0, 68.0, 86.0, 58.0, 170.0, 255, entity->textureSymbols); break;
                case 2: RenderImage(-76.0, 0.0, 0.0, 0.3, 0.35, 29.0, 43.0, 58.0, 86.0, 130.0, 170.0, 255, entity->textureSymbols); break;
                case 3:
                    RenderImage(-76.0, 0.0, 0.0, 0.3, 0.35, 34.0, 43.0, 68.0, 86.0, 58.0, 170.0, 255, entity->textureSymbols);
                    RenderImage(-84.0, 0.0, 0.0, 0.3, 0.35, 28.0, 43.0, 56.0, 86.0, 0.0, 170.0, 255, entity->textureSymbols);
                    break;
            }

            uint emeraldColours[] = {
                0x8080FF, 0xFFFF00, 0xFF60C0, 0xA0FF00, 0xA0FF00, 0xFF4060, 0xFFFFFF,
            };

            float x = -60.0f;
            for (int i = 0; i < 7; ++i) {
                if (entity->flags & (1 << i)) {
                    SetRenderVertexColor((emeraldColours[i] >> 16) & 0xFF, (emeraldColours[i] >> 8) & 0xFF, emeraldColours[i] & 0xFF);
                    RenderImage(x, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 188.0, 0.0, 255, entity->textureSymbols);
                }
                else {
                    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
                    RenderImage(x, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 133.0, 0.0, 255, entity->textureSymbols);
                }

                x += 16.0f;
            }
            break;
        }
        default: break;
    }

    SetRenderVertexColor(255, 255, 255);
    if (entity->useMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}
