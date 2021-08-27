#include "RetroEngine.hpp"

void StaffCredits_Create(void *objPtr)
{
    RSDK_THIS(StaffCredits);

    entity->labelPtr            = CREATE_ENTITY(TextLabel);
    entity->labelPtr->useMatrix = true;
    entity->labelPtr->fontID    = 0;
    entity->labelPtr->textScale = 0.2;
    entity->labelPtr->textAlpha = 256;
    entity->labelPtr->textX     = -144.0;
    entity->labelPtr->textY     = 100.0;
    entity->labelPtr->textZ     = 16.0;
    entity->labelPtr->alignment = 0;
    SetStringToFont(entity->labelPtr->text, strStaffCredits, 0);
    entity->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", 255);
    SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 0xC0);
    entity->textureArrows    = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);
    entity->creditsTextID = 0;

    float offY =  -128.0;
    for (int i = 0; i < 0x10; ++i) {
        NativeEntity_CreditText *creditText = CREATE_ENTITY(CreditText);
        entity->creditText[i]               = creditText;

        switch (creditsType[entity->creditsTextID]) {
            case 0:
                creditText->fontID = 1;
                creditText->colour = 0xFFFFFF;
                creditText->scaleX = 0.125;
                break;
            case 1:
                creditText->fontID = 2;
                creditText->colour = 0xFF8000;
                creditText->scaleX = 0.25;
                break;
            case 2:
                creditText->fontID = 2;
                creditText->colour = 0xFFFFFF;
                creditText->scaleX = 0.25;
                break;
            case 3:
                creditText->fontID = 2;
                creditText->state  = 4;
                break;
            default: break;
        }

        SetStringToFont(creditText->text, strCreditsList[entity->creditsTextID], creditText->fontID);
        offY -= creditsAdvanceY[entity->creditsTextID];
        creditText->textX           = 0.0;
        creditText->useRenderMatrix = true;
        creditText->textZ           = 8.0;
        creditText->textY           = offY - creditsAdvanceY[entity->creditsTextID];
        ++entity->creditsTextID;
    }

    entity->latestTextID = 15;
}
void StaffCredits_Main(void *objPtr)
{
    RSDK_THIS(StaffCredits);
    NativeEntity_OptionsMenu *optionsMenu = (NativeEntity_OptionsMenu *)entity->optionsMenu;

    switch (entity->state) {
        case 0: //fade in
            if (entity->alpha < 0x100)
                entity->alpha += 8;

            entity->scale += ((1.05 - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0));
            if (entity->scale < -8.0)
                entity->scale = -8.0;

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrix2, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix2);
            SetRenderMatrix(&entity->renderMatrix);

            for (int i = 0; i < 16; ++i) {
                memcpy(&entity->creditText[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            }

            entity->field_18 += Engine.deltaTime;
            if (entity->field_18 > 0.5) {
                entity->alpha    = 256;
                entity->field_18 = 0.0;
                entity->state    = 1;
            }
            break;
        case 1: //da credits
            CheckKeyDown(&keyDown);
            CheckKeyDown(&keyPress);
            SetRenderMatrix(&entity->renderMatrix);
            if (touches <= 0) {
                if (entity->useRenderMatrix) {
                    PlaySfx(23, 0);
                    entity->useRenderMatrix = 0;
                    entity->state           = 2;
                }
            }
            else {
                entity->useRenderMatrix = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
            }
            if (keyPress.B) {
                PlaySfx(23, 0);
                entity->useRenderMatrix = false;
                entity->state           = 2;
            }
            break;
        case 2: //fade out
            if (entity->alpha > 0)
                entity->alpha -= 8;

            if (entity->field_14 >= 0.2)
                entity->scale += ((-1.0f - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0));
            else
                entity->scale += ((1.5 - entity->scale) / ((60.0 * Engine.deltaTime) * 8.0));
            if (entity->scale < 0.0)
                entity->scale = 0.0;

            NewRenderState();
            matrixScaleXYZF(&entity->renderMatrix, entity->scale, entity->scale, 1.0);
            matrixTranslateXYZF(&entity->matrix2, 0.0, -8.0, 160.0);
            matrixMultiplyF(&entity->renderMatrix, &entity->matrix2);
            SetRenderMatrix(&entity->renderMatrix);

            for (int i = 0; i < 16; ++i) {
                memcpy(&entity->creditText[i]->renderMatrix, &entity->renderMatrix, sizeof(MatrixF));
            }

            entity->field_18 += Engine.deltaTime;
            if (entity->field_18 > 0.5) {
                optionsMenu->state = 7;
                for (int i = 15; i >= 0; --i) RemoveNativeObject(entity->creditText[i]);
                RemoveNativeObject(entity->labelPtr);
                RemoveNativeObject(entity);
            }
            return;
    }

    
    for (int i = 0; i < 0x10; ++i) {
        NativeEntity_CreditText *creditText = entity->creditText[i];

        if (touches > 0 || keyDown.A || keyDown.C) {
            creditText->textY += 1.5;
        }
        else {
            creditText->textY += 0.75;
        }
        
        if (creditText->textY > SCREEN_CENTERY_F) {
            creditText->textY = entity->creditText[entity->latestTextID]->textY - creditsAdvanceY[entity->creditsTextID];

            switch (creditsType[entity->creditsTextID]) {
                case 0:
                    creditText->fontID = 1;
                    creditText->colour = 0xFFFFFF;
                    creditText->scaleX = 0.125;
                    break;
                case 1:
                    creditText->fontID = 2;
                    creditText->colour = 0xFF8000;
                    creditText->scaleX = 0.25;
                    break;
                case 2:
                    creditText->fontID = 2;
                    creditText->colour = 0xFFFFFF;
                    creditText->scaleX = 0.25;
                    break;
                case 3:
                    creditText->fontID = 2;
                    creditText->state  = 4;
                    break;
                default: break;
            }
            SetStringToFont(creditText->text, strCreditsList[entity->creditsTextID], creditText->fontID);
            entity->latestTextID = (entity->latestTextID + 1) & 0xF;
            entity->creditsTextID++;
            if (entity->creditsTextID >= creditsListSize)
                entity->creditsTextID = 0;
        }
    }
    RenderMesh(entity->meshPanel, 0, false);
    NewRenderState();
    SetRenderMatrix(NULL);

    if (entity->useRenderMatrix)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, entity->alpha, entity->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, entity->alpha, entity->textureArrows);
}
