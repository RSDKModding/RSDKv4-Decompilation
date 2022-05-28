#include "RetroEngine.hpp"

void StaffCredits_Create(void *objPtr)
{
    RSDK_THIS(StaffCredits);

    self->labelPtr                  = CREATE_ENTITY(TextLabel);
    self->labelPtr->useRenderMatrix = true;
    self->labelPtr->fontID          = FONT_HEADING;
    self->labelPtr->scale           = 0.2;
    self->labelPtr->alpha           = 0x100;
    self->labelPtr->x               = -144.0;
    self->labelPtr->y               = 100.0;
    self->labelPtr->z               = 16.0;
    self->labelPtr->state           = TEXTLABEL_STATE_IDLE;
    SetStringToFont(self->labelPtr->text, strStaffCredits, FONT_HEADING);
    self->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", 255);
    SetMeshVertexColors(self->meshPanel, 0, 0, 0, 0xC0);
    self->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);
    self->creditsTextID = 0;

    float offY = -128.0;
    for (int i = 0; i < StaffCredits_CreditsCount; ++i) {
        NativeEntity_CreditText *creditText = CREATE_ENTITY(CreditText);
        self->creditText[i]                 = creditText;

        switch (creditsType[self->creditsTextID]) {
            case CREDITS_TYPE_TEXT1:
                creditText->fontID = FONT_LABEL;
                creditText->color  = 0xFFFFFF;
                creditText->scale  = 0.125;
                break;
            case CREDITS_TYPE_TEXT2:
                creditText->fontID = FONT_TEXT;
                creditText->color  = 0xFF8000;
                creditText->scale  = 0.25;
                break;
            case CREDITS_TYPE_TEXT3:
                creditText->fontID = FONT_TEXT;
                creditText->color  = 0xFFFFFF;
                creditText->scale  = 0.25;
                break;
            case CREDITS_TYPE_LOGO:
                creditText->fontID = FONT_TEXT;
                creditText->state  = CREDITTEXT_STATE_IMAGE;
                break;
            default: break;
        }

        offY -= creditsAdvanceY[self->creditsTextID];
        SetStringToFont(creditText->text, strCreditsList[self->creditsTextID], creditText->fontID);
        creditText->textX           = 0.0;
        creditText->useRenderMatrix = true;
        creditText->textZ           = 8.0;
        creditText->textY           = offY;
        ++self->creditsTextID;
    }

    self->latestTextID = -1;
}
void StaffCredits_Main(void *objPtr)
{
    RSDK_THIS(StaffCredits);
    NativeEntity_OptionsMenu *optionsMenu = (NativeEntity_OptionsMenu *)self->optionsMenu;

    switch (self->state) {
        case STAFFCREDITS_STATE_ENTER:
            if (self->alpha < 0x100)
                self->alpha += 8;

            self->scale = fminf(self->scale + ((1.05 - self->scale) / ((60.0 * Engine.deltaTime) * 8.0)), 1.0f);

            NewRenderState();
            MatrixScaleXYZF(&self->renderMatrix, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrix2, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrix2);
            SetRenderMatrix(&self->renderMatrix);

            memcpy(&self->labelPtr->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < StaffCredits_CreditsCount; ++i) memcpy(&self->creditText[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                self->alpha = 256;
                self->timer = 0.0;
                self->state = STAFFCREDITS_STATE_SCROLL;
            }
            break;
        case STAFFCREDITS_STATE_SCROLL:
            CheckKeyDown(&inputDown);
            CheckKeyPress(&inputPress);
            SetRenderMatrix(&self->renderMatrix);
            if (touches <= 0) {
                if (self->useRenderMatrix) {
                    PlaySfxByName("Menu Back", false);
                    self->useRenderMatrix = false;
                    self->state           = STAFFCREDITS_STATE_EXIT;
                }
            }
            else {
                self->useRenderMatrix = CheckTouchRect(128.0, -92.0, 32.0, 32.0) >= 0;
            }
            if (inputPress.B) {
                PlaySfxByName("Menu Back", false);
                self->useRenderMatrix = false;
                self->state           = STAFFCREDITS_STATE_EXIT;
            }
            break;
        case STAFFCREDITS_STATE_EXIT:
            if (self->alpha > 0)
                self->alpha -= 8;

            if (self->timer < 0.2)
                self->scale = fmaxf(self->scale + ((1.5f - self->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);
            else
                self->scale = fmaxf(self->scale + ((-1.0f - self->scale) / ((Engine.deltaTime * 60.0) * 8.0)), 0.0);

            NewRenderState();
            MatrixScaleXYZF(&self->renderMatrix, self->scale, self->scale, 1.0);
            MatrixTranslateXYZF(&self->matrix2, 0.0, -8.0, 160.0);
            MatrixMultiplyF(&self->renderMatrix, &self->matrix2);
            SetRenderMatrix(&self->renderMatrix);

            memcpy(&self->labelPtr->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            for (int i = 0; i < StaffCredits_CreditsCount; ++i) {
                memcpy(&self->creditText[i]->renderMatrix, &self->renderMatrix, sizeof(MatrixF));
            }

            self->timer += Engine.deltaTime;
            if (self->timer > 0.5) {
                optionsMenu->state = OPTIONSMENU_STATE_EXITSUBMENU;
                for (int i = 0; i < StaffCredits_CreditsCount; ++i) RemoveNativeObject(self->creditText[i]);
                RemoveNativeObject(self->labelPtr);
                RemoveNativeObject(self);
                return;
            }
            break;
    }

    for (int i = 0; i < StaffCredits_CreditsCount; ++i) {
        NativeEntity_CreditText *creditText = self->creditText[i];

        creditText->textY += 0.75;
        if (touches > 0 || inputDown.A || inputDown.C) {
            creditText->textY += 0.75;
        }

        if (creditText->textY > SCREEN_CENTERY_F) {
            creditText->textY = self->creditText[self->latestTextID % StaffCredits_CreditsCount]->textY - creditsAdvanceY[self->creditsTextID];

            switch (creditsType[self->creditsTextID]) {
                case CREDITS_TYPE_TEXT1:
                    creditText->fontID = FONT_LABEL;
                    creditText->color  = 0xFFFFFF;
                    creditText->scale  = 0.125;
                    creditText->state  = CREDITTEXT_STATE_SETUP;
                    break;
                case CREDITS_TYPE_TEXT2:
                    creditText->fontID = FONT_TEXT;
                    creditText->color  = 0xFF8000;
                    creditText->scale  = 0.25;
                    creditText->state  = CREDITTEXT_STATE_SETUP;
                    break;
                case CREDITS_TYPE_TEXT3:
                    creditText->fontID = FONT_TEXT;
                    creditText->color  = 0xFFFFFF;
                    creditText->scale  = 0.25;
                    creditText->state  = CREDITTEXT_STATE_SETUP;
                    break;
                case CREDITS_TYPE_LOGO:
                    creditText->fontID = FONT_TEXT;
                    creditText->state  = CREDITTEXT_STATE_IMAGE;
                    break;
                default: break;
            }
            SetStringToFont(creditText->text, strCreditsList[self->creditsTextID], creditText->fontID);
            self->latestTextID++;
            self->creditsTextID++;
            if (self->creditsTextID >= creditsListSize)
                self->creditsTextID = 0;
        }
    }
    RenderMesh(self->meshPanel, MESH_COLORS, false);
    NewRenderState();
    SetRenderMatrix(NULL);

    if (self->useRenderMatrix)
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 128.0, self->alpha, self->textureArrows);
    else
        RenderImage(128.0, -92.0, 160.0, 0.3, 0.3, 64.0, 64.0, 128.0, 128.0, 128.0, 0.0, self->alpha, self->textureArrows);
}
