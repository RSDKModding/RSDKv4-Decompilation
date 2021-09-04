#include "RetroEngine.hpp"

void PushButton_Create(void *objPtr)
{
    RSDK_THIS(PushButton);
    entity->z                  = 160.0f;
    entity->alpha              = 255;
    entity->scale              = 0.15f;
    entity->state              = PUSHBUTTON_STATE_SCALED;
    entity->symbolsTex         = LoadTexture("Data/Game/Menu/Symbols.png", TEXFMT_RGBA4444);
    entity->bgColour           = 0xFF0000;
    entity->bgColourSelected   = 0xFF4000;
    entity->textColour         = 0xFFFFFF;
    entity->textColourSelected = 0xFFFF00;
}
void PushButton_Main(void *objPtr)
{
    RSDK_THIS(PushButton);
    NewRenderState();
    if (entity->useRenderMatrix)
        SetRenderMatrix(&entity->renderMatrix);
    SetRenderBlendMode(RENDER_BLEND_ALPHA);

    switch (entity->state) {
        case PUSHBUTTON_STATE_UNSELECTED: {
            SetRenderVertexColor((entity->bgColour >> 16) & 0xFF, (entity->bgColour >> 8) & 0xFF, entity->bgColour & 0xFF);
            RenderImage(entity->x - entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 64.0, 64.0, 64.0, 128.0, 0.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x, entity->y, entity->z, entity->textWidth + entity->textWidth, entity->scale, 0.5, 64.0, 1.0, 128.0, 63.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x + entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 0.0, 64.0, 64.0, 128.0, 64.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            SetRenderVertexColor(entity->textColour >> 16, (entity->textColour >> 8) & 0xFF, entity->textColour & 0xFF);
            RenderText(entity->text, FONT_LABEL, entity->x - entity->xOff, entity->y - entity->yOff, entity->z, entity->textScale, entity->alpha);
            break;
        }
        case PUSHBUTTON_STATE_SELECTED: {
            if (usePhysicalControls) {
                SetRenderVertexColor(0x00, 0x00, 0x00);
                RenderImage(entity->x - entity->textWidth, entity->y, entity->z, 1.1 * entity->scale, 1.1 * entity->scale, 64.0, 64.0, 64.0, 128.0,
                            0.0, 0.0, entity->alpha, entity->symbolsTex);
                RenderImage(entity->x, entity->y, entity->z, entity->textWidth + entity->textWidth, 1.1 * entity->scale, 0.5, 64.0, 1.0, 128.0, 63.0,
                            0.0, entity->alpha, entity->symbolsTex);
                RenderImage(entity->x + entity->textWidth, entity->y, entity->z, 1.1 * entity->scale, 1.1 * entity->scale, 0.0, 64.0, 64.0, 128.0,
                            64.0, 0.0, entity->alpha, entity->symbolsTex);
            }
            SetRenderVertexColor((entity->bgColourSelected >> 16) & 0xFF, (entity->bgColourSelected >> 8) & 0xFF, entity->bgColourSelected & 0xFF);
            RenderImage(entity->x - entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 64.0, 64.0, 64.0, 128.0, 0.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x, entity->y, entity->z, entity->textWidth + entity->textWidth, entity->scale, 0.5, 64.0, 1.0, 128.0, 63.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x + entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 0.0, 64.0, 64.0, 128.0, 64.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            SetRenderVertexColor(entity->textColourSelected >> 16, (entity->textColourSelected >> 8) & 0xFF, entity->textColourSelected & 0xFF);
            RenderText(entity->text, FONT_LABEL, entity->x - entity->xOff, entity->y - entity->yOff, entity->z, entity->textScale, entity->alpha);
            break;
        }
        case PUSHBUTTON_STATE_FLASHING: {
            entity->flashTimer += Engine.deltaTime;
            if (entity->flashTimer > 0.1)
                entity->flashTimer -= 0.1;
            SetRenderVertexColor((entity->bgColourSelected >> 16) & 0xFF, (entity->bgColourSelected >> 8) & 0xFF, entity->bgColourSelected & 0xFF);
            RenderImage(entity->x - entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 64.0, 64.0, 64.0, 128.0, 0.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x, entity->y, entity->z, entity->textWidth + entity->textWidth, entity->scale, 0.5, 64.0, 1.0, 128.0, 63.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x + entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 0.0, 64.0, 64.0, 128.0, 64.0, 0.0,
                        entity->alpha, entity->symbolsTex);

            int colour = entity->flashTimer > 0.05f ? entity->textColourSelected : entity->textColour;
            SetRenderVertexColor((colour >> 16) & 0xFF, (colour >> 8) & 0xFF, colour & 0xFF);
            RenderText(entity->text, FONT_LABEL, entity->x - entity->xOff, entity->y - entity->yOff, entity->z, entity->textScale, entity->alpha);
            entity->stateTimer += Engine.deltaTime;
            if (entity->stateTimer > 0.5) {
                entity->stateTimer = 0.0;
                entity->state      = 0;
            }
            break;
        }
        case PUSHBUTTON_STATE_SCALED: {
            entity->state     = 0;
            entity->xOff      = GetTextWidth(entity->text, FONT_LABEL, entity->scale) * 0.375;
            entity->textWidth = GetTextWidth(entity->text, FONT_LABEL, entity->scale) * 0.375;
            entity->yOff      = 0.75 * entity->scale * 32.0;
            entity->textScale = 0.75 * entity->scale;

            SetRenderVertexColor(entity->bgColour >> 16, (entity->bgColour >> 8) & 0xFF, entity->bgColour & 0xFF);
            RenderImage(entity->x - entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 64.0, 64.0, 64.0, 128.0, 0.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x, entity->y, entity->z, entity->textWidth + entity->textWidth, entity->scale, 0.5, 64.0, 1.0, 128.0, 63.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            RenderImage(entity->x + entity->textWidth, entity->y, entity->z, entity->scale, entity->scale, 0.0, 64.0, 64.0, 128.0, 64.0, 0.0,
                        entity->alpha, entity->symbolsTex);
            SetRenderVertexColor((entity->textColour >> 16) & 0xFF, (entity->textColour >> 8) & 0xFF, entity->textColour & 0xFF);
            RenderText(entity->text, FONT_LABEL, entity->x - entity->xOff, entity->y - entity->yOff, entity->z, entity->textScale, entity->alpha);
            break;
        }
    }

    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    if (entity->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}
