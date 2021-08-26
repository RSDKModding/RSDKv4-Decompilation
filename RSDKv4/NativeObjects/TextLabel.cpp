#include "RetroEngine.hpp"

void TextLabel_Create(void *objPtr)
{
    RSDK_THIS(TextLabel);
    entity->textZ     = 160.0;
    entity->textAlpha = 255;
    entity->alignment = 0;
    entity->alignPtr  = TextLabel_Align;
}
void TextLabel_Main(void *objPtr)
{
    RSDK_THIS(TextLabel);

    if (entity->byteB4 == 1) {
        NewRenderState();
        SetRenderMatrix(&entity->renderMatrix);
    }

    switch (entity->alignment) {
        case 0:
            SetRenderBlendMode(1);
            RenderText(entity->text, entity->fontID, entity->textX - entity->textWidth, entity->textY, entity->textZ, entity->textScale,
                       entity->textAlpha);
            break;
        case 1:
            entity->timer += Engine.deltaTime;
            if (entity->timer > 1.0f)
                entity->timer -= 1.0f;

            if (entity->timer > 0.5) {
                SetRenderBlendMode(1);
                RenderText(entity->text, entity->fontID, entity->textX - entity->textWidth, entity->textY, entity->textZ, entity->textScale,
                           entity->textAlpha);
            }
            break;
        case 2:
            entity->timer += Engine.deltaTime;
            if (entity->timer > 0.1f)
                entity->timer -= 0.1f;

            if (entity->timer > 0.05) {
                SetRenderBlendMode(1);
                RenderText(entity->text, entity->fontID, entity->textX - entity->textWidth, entity->textY, entity->textZ, entity->textScale,
                           entity->textAlpha);
            }
            break;
    }

    if (entity->byteB4 == 1) {
        NewRenderState();
        SetRenderMatrix(0);
    }
}

void TextLabel_Align(NativeEntity_TextLabel *label, int align)
{
    switch (align) {
        case 1: label->textWidth = GetTextWidth(label->text, label->fontID, label->textScale) * 0.5; break;
        case 2: label->textWidth = GetTextWidth(label->text, label->fontID, label->textScale); break;
        case 0: label->textWidth = 0.0; break;
    }
}