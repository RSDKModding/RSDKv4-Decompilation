#include "RetroEngine.hpp"

void SubMenuButton_Create(void *objPtr)
{
    RSDK_THIS(SubMenuButton);
    entity->matZ           = 160.0;
    entity->alpha          = 255;
    entity->state          = 0;
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
        case 0:
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderMesh(entity->meshButton, MESH_COLOURS, false);
            RenderText(entity->text, FONT_LABEL, -80.0, entity->textY, 0, entity->scale, entity->alpha);
            break;
        case 1:
            entity->flashTimer += Engine.deltaTime;
            if (entity->flashTimer > 1.0)
                entity->flashTimer -= 1.0;

            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderMesh(entity->meshButton, MESH_COLOURS, false);
            if (entity->flashTimer > 0.5)
                RenderText(entity->text, FONT_LABEL, -80.0, entity->textY, 0, entity->scale, entity->alpha);
            break;
        case 2:
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
                entity->state           = 0;
            }
            break;
        case 3:
            entity->flashTimer += Engine.deltaTime;
            if (entity->flashTimer > 0.1)
                entity->flashTimer -= 0.1;

            entity->afterFlashTimer += Engine.deltaTime;
            if (entity->afterFlashTimer > 0.5) {
                entity->flashTimer      = 0.0;
                entity->afterFlashTimer = 0.0;
                entity->state           = 4;
            }
            // FallThrough
        case 4:
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

            if ((entity->flags & 1) != 0) {
                SetRenderVertexColor(128, 128, 255);
                RenderImage(-60.0, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 188.0, 0.0, 255, entity->textureSymbols);
            }
            else {
                SetRenderVertexColor(255, 255, 255);
                RenderImage(-60.0, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 133.0, 0.0, 255, entity->textureSymbols);
            }
            if ((entity->flags & 2) != 0) {
                SetRenderVertexColor(255, 255, 0);
                RenderImage(-44.0, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 188.0, 0.0, 255, entity->textureSymbols);
            }
            else {
                SetRenderVertexColor(255, 255, 255);
                RenderImage(-44.0, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 133.0, 0.0, 255, entity->textureSymbols);
            }
            if ((entity->flags & 4) != 0) {
                SetRenderVertexColor(255, 96, 192);
                RenderImage(-28.0, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 188.0, 0.0, 255, entity->textureSymbols);
            }
            else {
                SetRenderVertexColor(255, 255, 255);
                RenderImage(-28.0, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 133.0, 0.0, 255, entity->textureSymbols);
            }
            if ((entity->flags & 8) != 0) {
                SetRenderVertexColor(160, 255, 0);
                RenderImage(-12.0, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 188.0, 0.0, 255, entity->textureSymbols);
            }
            else {
                SetRenderVertexColor(255, 255, 255);
                RenderImage(-12.0, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 133.0, 0.0, 255, entity->textureSymbols);
            }
            if ((entity->flags & 0x10) != 0) {
                SetRenderVertexColor(255, 64, 96);
                RenderImage(4.0, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 188.0, 0.0, 255, entity->textureSymbols);
            }
            else {
                SetRenderVertexColor(255, 255, 255);
                RenderImage(4.0, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 133.0, 0.0, 255, entity->textureSymbols);
            }
            if ((entity->flags & 0x20) != 0) {
                SetRenderVertexColor(255, 255, 255);
                RenderImage(20.0, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 188.0, 0.0, 255, entity->textureSymbols);
            }
            else {
                SetRenderVertexColor(255, 255, 255);
                RenderImage(20.0, -6.0, 0.0, 0.125, 0.125, 28.0, 35.0, 56.0, 70.0, 133.0, 0.0, 255, entity->textureSymbols);
            }
            break;
        default: break;
    }

    SetRenderVertexColor(255, 255, 255);
    if (entity->useMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}
