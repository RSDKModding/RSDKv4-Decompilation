#include "RetroEngine.hpp"

void ZoneButton_Create(void *objPtr)
{
    RSDK_THIS(ZoneButton);
    entity->textZ          = 160.0;
    entity->state          = 0;
    entity->textureIntro   = LoadTexture("Data/Game/Menu/Intro.png", TEXFMT_RGBA4444);
    entity->textureSymbols = LoadTexture("Data/Game/Menu/Symbols.png", TEXFMT_RGBA4444);
    entity->colourWhite    = 0xFFFFFF;
    entity->colourYellow   = 0xFFFF00;
    entity->colourWhite2   = 0xFFFFFF;
    entity->colourYellow2  = 0xFFFF00;
    entity->float28        = 1.0;
}
void ZoneButton_Main(void *objPtr)
{
    RSDK_THIS(ZoneButton);
    NewRenderState();
    if (entity->useRenderMatrix)
        SetRenderMatrix(&entity->renderMatrix);
    SetRenderBlendMode(RENDER_BLEND_ALPHA);

    entity->angle += entity->float28 * Engine.deltaTime;
    if (entity->angle > (M_PI * 2))
        entity->angle -= (M_PI * 2);
    if (entity->angle < -(M_PI * 2))
        entity->angle += (M_PI * 2);

    switch (entity->state) {
        case 0:
            SetRenderVertexColor((entity->colourWhite >> 16) & 0xFF, (entity->colourWhite >> 8) & 0xFF, entity->colourWhite & 0xFF);
            RenderRect(entity->textX - 64.0, entity->textY + 64.0, entity->textZ, 128.0, 96.0, (entity->colourWhite >> 16) & 0xFF,
                       (entity->colourWhite >> 8) & 0xFF, entity->colourWhite & 0xFF, 255);
            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            RenderImage(entity->textX, entity->textY + 16.0, entity->textZ, 0.9, 0.9, 64.0, 48.0, 128.0, 96.0,
                        (entity->texX - 64.0) + (cosf(entity->angle) * 24.0), (entity->texY - 48.0) + (sinf(entity->angle) * 24.0), 255,
                        entity->textureIntro);

            SetRenderVertexColor((entity->colourWhite2 >> 16) & 0xFF, (entity->colourWhite2 >> 8) & 0xFF, entity->colourWhite2 & 0xFF);
            RenderText(entity->zoneText, 2, entity->textX - entity->textWidth, entity->textY - 48.0, entity->textZ, 0.25, 255);
            if (entity->flag)
                RenderText(entity->timeText, 2, entity->textX - 56.0, entity->textY - 24.0, entity->textZ, 0.25, 255);
            else
                RenderImage(entity->textX - 32.0, entity->textY - 6.0, entity->textZ, 0.5, 0.5, 24.0, 32.0, 48.0, 64.0, 205.0, 68.0, 255,
                            entity->textureSymbols);
            break;
        case 1:
            SetRenderVertexColor((entity->colourWhite >> 16) & 0xFF, (entity->colourWhite >> 8) & 0xFF, entity->colourWhite & 0xFF);
            RenderRect(entity->textX - 64.0, entity->textY + 64.0, entity->textZ, 128.0, 96.0, (entity->colourYellow >> 16) & 0xFF,
                       (entity->colourYellow >> 8) & 0xFF, entity->colourYellow & 0xFF, 255);
            SetRenderVertexColor(255, 255, 255);
            RenderImage(entity->textX, entity->textY + 16.0, entity->textZ, 0.9, 0.9, 64.0, 48.0, 128.0, 96.0,
                        (entity->texX - 64.0) + (cosf(entity->angle) * 24.0), (entity->texY - 48.0) + (sinf(entity->angle) * 24.0), 255,
                        entity->textureIntro);
            SetRenderVertexColor((entity->colourYellow2 >> 16) & 0xFF, (entity->colourYellow2 >> 8) & 0xFF, entity->colourYellow2 & 0xFF);
            RenderText(entity->zoneText, 2, entity->textX - entity->textWidth, entity->textY - 48.0, entity->textZ, 0.25, 255);
            if (entity->flag)
                RenderText(entity->timeText, 2, entity->textX - 56.0, entity->textY - 24.0, entity->textZ, 0.25, 255);
            else
                RenderImage(entity->textX - 32.0, entity->textY - 6.0, entity->textZ, 0.5, 0.5, 24.0, 32.0, 48.0, 64.0, 205.0, 68.0, 255,
                            entity->textureSymbols);
            break;
        case 2:
            entity->float3C += Engine.deltaTime;
            if (entity->float3C > 0.1)
                entity->float3C -= 0.1;

            uint colour = entity->colourWhite;
            if (entity->float3C > 0.05)
                colour = entity->colourYellow;

            SetRenderVertexColor((colour >> 16) & 0xFF, (colour >> 8) & 0xFF, colour & 0xFF);
            RenderRect(entity->textX - 64.0, entity->textY + 64.0, entity->textZ, 128.0, 96.0, (entity->colourYellow >> 16) & 0xFF,
                       (entity->colourYellow >> 8) & 0xFF, entity->colourYellow & 0xFF, 255);
            SetRenderVertexColor(0xFF, 0xFF, 0xFF);
            RenderImage(entity->textX, entity->textY + 16.0, entity->textZ, 0.9, 0.9, 64.0, 48.0, 128.0, 96.0,
                        (entity->texX - 64.0) + (cosf(entity->angle) * 24.0), (entity->texY - 48.0) + (sinf(entity->angle) * 24.0), 255,
                        entity->textureIntro);

            colour = entity->colourYellow2;
            if (entity->float3C <= 0.05)
                colour = entity->colourWhite2;
            SetRenderVertexColor((colour >> 16) & 0xFF, (colour >> 8) & 0xFF, colour & 0xFF);
            RenderText(entity->zoneText, 2, entity->textX - entity->textWidth, entity->textY - 48.0, entity->textZ, 0.25, 255);
            RenderText(entity->timeText, 2, entity->textX - 56.0, entity->textY - 24.0, entity->textZ, 0.25, 255);

            entity->float38 += Engine.deltaTime;
            if (entity->float38 > 0.5) {
                entity->float38 = 0.0;
                entity->state   = 0;
            }
            break;
    }

    SetRenderVertexColor(0xFF, 0xFF, 0xFF);
    if (entity->useRenderMatrix) {
        NewRenderState();
        SetRenderMatrix(NULL);
    }
}
