#include "RetroEngine.hpp"

void ZoneButton_Create(void *objPtr)
{
    RSDK_THIS(ZoneButton);
    entity->z                  = 160.0;
    entity->state              = ZONEBUTTON_STATE_UNSELECTED;
    entity->textureIntro       = LoadTexture("Data/Game/Menu/Intro.png", TEXFMT_RGBA4444);
    entity->textureSymbols     = LoadTexture("Data/Game/Menu/Symbols.png", TEXFMT_RGBA4444);
    entity->bgColour           = 0xFFFFFF;
    entity->bgColourSelected   = 0xFFFF00;
    entity->textColour         = 0xFFFFFF;
    entity->textSelectedColour = 0xFFFF00;
    entity->angleSpeed         = 1.0;
}
void ZoneButton_Main(void *objPtr)
{
    RSDK_THIS(ZoneButton);
    NewRenderState();
    if (entity->useRenderMatrix)
        SetRenderMatrix(&entity->renderMatrix);
    SetRenderBlendMode(RENDER_BLEND_ALPHA);

    entity->angle += entity->angleSpeed * Engine.deltaTime;
    if (entity->angle > M_PI_2)
        entity->angle -= M_PI_2;
    if (entity->angle < -M_PI_2)
        entity->angle += M_PI_2;

    switch (entity->state) {
        case ZONEBUTTON_STATE_UNSELECTED:
            SetRenderVertexColor((entity->bgColour >> 16) & 0xFF, (entity->bgColour >> 8) & 0xFF, entity->bgColour & 0xFF);
            RenderRect(entity->x - 64.0, entity->y + 64.0, entity->z, 128.0, 96.0, (entity->bgColour >> 16) & 0xFF, (entity->bgColour >> 8) & 0xFF,
                       entity->bgColour & 0xFF, 255);

            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            RenderImage(entity->x, entity->y + 16.0, entity->z, 0.9, 0.9, 64.0, 48.0, 128.0, 96.0,
                        (entity->texX - 64.0) + (cosf(entity->angle) * 24.0), (entity->texY - 48.0) + (sinf(entity->angle) * 24.0), 255,
                        entity->textureIntro);

            SetRenderVertexColor((entity->textColour >> 16) & 0xFF, (entity->textColour >> 8) & 0xFF, entity->textColour & 0xFF);
            RenderText(entity->zoneText, FONT_TEXT, entity->x - entity->textWidth, entity->y - 48.0, entity->z, 0.25, 255);

            if (entity->unlocked)
                RenderText(entity->timeText, FONT_TEXT, entity->x - 56.0, entity->y - 24.0, entity->z, 0.25, 255);
            else
                RenderImage(entity->x - 32.0, entity->y - 6.0, entity->z, 0.5, 0.5, 24.0, 32.0, 48.0, 64.0, 205.0, 68.0, 255, entity->textureSymbols);
            break;
        case ZONEBUTTON_STATE_SELECTED:
            SetRenderVertexColor((entity->bgColour >> 16) & 0xFF, (entity->bgColour >> 8) & 0xFF, entity->bgColour & 0xFF);
            RenderRect(entity->x - 64.0, entity->y + 64.0, entity->z, 128.0, 96.0, (entity->bgColourSelected >> 16) & 0xFF,
                       (entity->bgColourSelected >> 8) & 0xFF, entity->bgColourSelected & 0xFF, 255);

            SetRenderVertexColor(255, 255, 255);
            RenderImage(entity->x, entity->y + 16.0, entity->z, 0.9, 0.9, 64.0, 48.0, 128.0, 96.0,
                        (entity->texX - 64.0) + (cosf(entity->angle) * 24.0), (entity->texY - 48.0) + (sinf(entity->angle) * 24.0), 255,
                        entity->textureIntro);

            SetRenderVertexColor((entity->textSelectedColour >> 16) & 0xFF, (entity->textSelectedColour >> 8) & 0xFF,
                                 entity->textSelectedColour & 0xFF);
            RenderText(entity->zoneText, FONT_TEXT, entity->x - entity->textWidth, entity->y - 48.0, entity->z, 0.25, 255);

            if (entity->unlocked)
                RenderText(entity->timeText, FONT_TEXT, entity->x - 56.0, entity->y - 24.0, entity->z, 0.25, 255);
            else
                RenderImage(entity->x - 32.0, entity->y - 6.0, entity->z, 0.5, 0.5, 24.0, 32.0, 48.0, 64.0, 205.0, 68.0, 255, entity->textureSymbols);
            break;
        case ZONEBUTTON_STATE_FLASHING:
            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.1)
                entity->timer -= 0.1;

            uint colour = entity->bgColour;
            if (entity->timer > 0.05)
                colour = entity->bgColourSelected;

            SetRenderVertexColor((colour >> 16) & 0xFF, (colour >> 8) & 0xFF, colour & 0xFF);
            RenderRect(entity->x - 64.0, entity->y + 64.0, entity->z, 128.0, 96.0, (entity->bgColourSelected >> 16) & 0xFF,
                       (entity->bgColourSelected >> 8) & 0xFF, entity->bgColourSelected & 0xFF, 255);

            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            RenderImage(entity->x, entity->y + 16.0, entity->z, 0.9, 0.9, 64.0, 48.0, 128.0, 96.0,
                        (entity->texX - 64.0) + (cosf(entity->angle) * 24.0), (entity->texY - 48.0) + (sinf(entity->angle) * 24.0), 255,
                        entity->textureIntro);

            colour = entity->textColour;
            if (entity->timer > 0.05)
                colour = entity->textSelectedColour;
            SetRenderVertexColor((colour >> 16) & 0xFF, (colour >> 8) & 0xFF, colour & 0xFF);
            RenderText(entity->zoneText, FONT_TEXT, entity->x - entity->textWidth, entity->y - 48.0, entity->z, 0.25, 255);
            RenderText(entity->timeText, FONT_TEXT, entity->x - 56.0, entity->y - 24.0, entity->z, 0.25, 255);

            entity->float38 += Engine.deltaTime;
            if (entity->float38 > 0.5) {
                entity->float38 = 0.0;
                entity->state   = ZONEBUTTON_STATE_UNSELECTED;
            }
            break;
    }

    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    if (entity->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}
