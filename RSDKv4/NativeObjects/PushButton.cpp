#include "RetroEngine.hpp"

void PushButton_Create(void *objPtr)
{
    RSDK_THIS(PushButton);
    entity->z          = 160.0f;
    entity->alpha      = 255;
    entity->scale      = 0.15f;
    entity->state      = 3;
    entity->symbolsTex = LoadTexture("Data/Game/Menu/Symbols.png", TEXFMT_RGBA4444);
    entity->blue       = 0xFF0000;
    entity->blue2      = 0xFF4000;
    entity->white      = 0xFFFFFF;
    entity->cyan       = 0xFFFF00;
}
void PushButton_Main(void *objPtr)
{
    RSDK_THIS(PushButton);
    NewRenderState();
    if (entity->useRenderMatrix)
        SetRenderMatrix(&entity->renderMatrix);
    SetRenderBlendMode(RENDER_BLEND_ALPHA);

    switch (entity->state) {
        case 0: {
            SetRenderVertexColor((entity->blue >> 16) & 0xFF, (entity->blue >> 8) & 0xFF, entity->blue & 0xFF);
            RenderImage(entity->x - entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 64.0, 64.0, 64.0, 128.0, 0.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x, entity->y, entity->z, entity->textWidth + entity->textWidth, entity->scale, 0.5, 64.0, 1.0, 128.0, 63.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x + entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 0.0, 64.0, 64.0, 128.0, 64.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            SetRenderVertexColor(entity->white >> 16, (entity->white >> 8) & 0xFF, entity->white & 0xFF);
            RenderText(entity->text, 1, entity->x - entity->xOff, entity->y - entity->yOff, entity->z, entity->textScale, entity->alpha);
            break;
        }
        case 1: {
            if (usePhysicalControls) {
                SetRenderVertexColor(0x00, 0x00, 0x00);
                RenderImage(entity->x - entity->textWidth, entity->y, entity->z, 1.1 * entity->scale, 1.1 * entity->scale, 64.0, 64.0, 64.0, 128.0,
                            0.0, 0.0, entity->alpha, entity->symbolsTex);
                RenderImage(entity->x, entity->y, entity->z, entity->textWidth + entity->textWidth, 1.1 * entity->scale, 0.5, 64.0, 1.0, 128.0, 63.0,
                            0.0, entity->alpha, entity->symbolsTex);
                RenderImage(entity->x + entity->textWidth, entity->y, entity->z, 1.1 * entity->scale, 1.1 * entity->scale, 0.0, 64.0, 64.0, 128.0,
                            64.0, 0.0, entity->alpha, entity->symbolsTex);
            }
            SetRenderVertexColor((entity->blue2 >> 16) & 0xFF, (entity->blue2 >> 8) & 0xFF, entity->blue2 & 0xFF);
            RenderImage(entity->x - entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 64.0, 64.0, 64.0, 128.0, 0.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x, entity->y, entity->z, entity->textWidth + entity->textWidth, entity->scale, 0.5, 64.0, 1.0, 128.0, 63.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x + entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 0.0, 64.0, 64.0, 128.0, 64.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            SetRenderVertexColor(entity->cyan >> 16, (entity->cyan >> 8) & 0xFF, entity->cyan & 0xFF);
            RenderText(entity->text, 1, entity->x - entity->xOff, entity->y - entity->yOff, entity->z, entity->textScale, entity->alpha);
            break;
        }
        case 2: {
            entity->flashTimer += Engine.deltaTime;
            if (entity->flashTimer > 0.1)
                entity->flashTimer -= 0.1;
            SetRenderVertexColor((entity->blue2 >> 16) & 0xFF, (entity->blue2 >> 8) & 0xFF, entity->blue2 & 0xFF);
            RenderImage(entity->x - entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 64.0, 64.0, 64.0, 128.0, 0.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x, entity->y, entity->z, entity->textWidth + entity->textWidth, entity->scale, 0.5, 64.0, 1.0, 128.0, 63.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x + entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 0.0, 64.0, 64.0, 128.0, 64.0, 0.0,
                        entity->alpha, entity->symbolsTex);

            int color = entity->flashTimer > 0.05f ? entity->cyan : entity->white;
            SetRenderVertexColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
            RenderText(entity->text, 1, entity->x - entity->xOff, entity->y - entity->yOff, entity->z, entity->textScale, entity->alpha);
            entity->stateTimer += Engine.deltaTime;
            if (entity->stateTimer > 0.5) {
                entity->stateTimer = 0.0;
                entity->state      = 0;
            }
            break;
        }
        case 3: {
            entity->state     = 0;
            entity->xOff      = GetTextWidth(entity->text, 1, entity->scale) * 0.375;
            entity->textWidth = GetTextWidth(entity->text, 1, entity->scale) * 0.375;
            entity->yOff      = 0.75 * entity->scale * 32.0;
            entity->textScale = 0.75 * entity->scale;

            SetRenderVertexColor(entity->blue >> 16, (entity->blue >> 8) & 0xFF, entity->blue & 0xFF);
            RenderImage(entity->x - entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 64.0, 64.0, 64.0, 128.0, 0.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x, entity->y, entity->z, entity->textWidth + entity->textWidth, entity->scale, 0.5, 64.0, 1.0, 128.0, 63.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x + entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 0.0, 64.0, 64.0, 128.0, 64.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            SetRenderVertexColor((entity->white >> 16) & 0xFF, (entity->white >> 8) & 0xFF, entity->white & 0xFF);
            RenderText(entity->text, 1, entity->x - entity->xOff, entity->y - entity->yOff, entity->z, entity->textScale, entity->alpha);
            break;
        }
    }

    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    if (entity->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}
