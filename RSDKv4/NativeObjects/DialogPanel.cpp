#include "RetroEngine.hpp"

void DialogPanel_Create(void *objPtr)
{
    RSDK_THIS(DialogPanel);
    self->panelMesh = LoadMesh("Data/Game/Models/Panel.bin", -1);
    SetMeshVertexColors(self->panelMesh, 0x28, 0x5C, 0xB0, 0xFF);
    self->buttonCount = DLGTYPE_YESNO;
}

void DialogPanel_Main(void *objPtr)
{
    RSDK_THIS(DialogPanel);
    NewRenderState();
    SetRenderBlendMode(RENDER_BLEND_ALPHA);

    switch (self->state) {
        case DIALOGPANEL_STATE_SETUP: {
            NativeEntity_PushButton *confirmButton = CREATE_ENTITY(PushButton);
            self->buttons[0]                       = confirmButton;
            if (self->buttonCount == DLGTYPE_OK) {
                confirmButton->x               = 0.0;
                confirmButton->y               = -40.0;
                confirmButton->z               = 0.0;
                confirmButton->scale           = 0.25;
                confirmButton->bgColor         = 0x00A048;
                confirmButton->bgColorSelected = 0x00C060;
                confirmButton->useRenderMatrix = true;
                SetStringToFont8(confirmButton->text, " OK ", FONT_LABEL);
            }
            else {
                confirmButton->x               = -48.0;
                confirmButton->y               = -40.0;
                confirmButton->z               = 0.0;
                confirmButton->scale           = 0.25;
                confirmButton->bgColor         = 0x00A048;
                confirmButton->bgColorSelected = 0x00C060;
                confirmButton->useRenderMatrix = true;
                SetStringToFont(confirmButton->text, strYes, FONT_LABEL);

                NativeEntity_PushButton *noButton = CREATE_ENTITY(PushButton);
                self->buttons[1]                  = noButton;
                noButton->useRenderMatrix         = true;
                noButton->scale                   = 0.25;
                noButton->x                       = 48.0;
                noButton->y                       = -40.0;
                noButton->z                       = 0.0;
                SetStringToFont(noButton->text, strNo, FONT_LABEL);
            }
            self->scale = 224.0 / (GetTextWidth(self->text, FONT_TEXT, 1.0) + 1.0);
            if (self->scale > 0.4)
                self->scale = 0.4;
            self->textX = GetTextWidth(self->text, FONT_TEXT, self->scale) * -0.5;
            self->textY = GetTextHeight(self->text, FONT_TEXT, self->scale) * 0.5;
            self->state = DIALOGPANEL_STATE_ENTER;
        }
        // FallThrough
        case DIALOGPANEL_STATE_ENTER: {
            self->buttonScale += ((0.77 - self->buttonScale) / ((Engine.deltaTime * 60.0) * 8.0));
            if (self->buttonScale > 0.75)
                self->buttonScale = 0.75;
            NewRenderState();
            MatrixScaleXYZF(&self->buttonMatrix, self->buttonScale, self->buttonScale, 1.0);
            MatrixTranslateXYZF(&self->buttonMult, 0.0, 0.0, 160.0);
            MatrixMultiplyF(&self->buttonMatrix, &self->buttonMult);
            SetRenderMatrix(&self->buttonMatrix);
            for (int i = 0; i < self->buttonCount; ++i) memcpy(&self->buttons[i]->renderMatrix, &self->buttonMatrix, sizeof(MatrixF));

            self->stateTimer += Engine.deltaTime;
            if (self->stateTimer > 0.5) {
                self->state      = DIALOGPANEL_STATE_MAIN;
                self->stateTimer = 0.0;
            }
            break;
        }
        case DIALOGPANEL_STATE_MAIN: {
            CheckKeyDown(&inputDown);
            CheckKeyPress(&inputPress);
            SetRenderMatrix(&self->buttonMatrix);
            if (!usePhysicalControls) {
                if (touches < 1) {
                    if (self->buttons[0]->state == 1) {
                        self->buttonSelected = 0;
                        self->state          = DIALOGPANEL_STATE_ACTION;
                        PlaySfxByName("Menu Select", false);
                        self->buttons[0]->state = PUSHBUTTON_STATE_FLASHING;
                    }
                    if (self->buttonCount == DLGTYPE_YESNO && self->buttons[1]->state == 1) {
                        self->buttonSelected = 1;
                        self->state          = DIALOGPANEL_STATE_ACTION;
                        PlaySfxByName("Menu Select", false);
                        self->buttons[1]->state = PUSHBUTTON_STATE_FLASHING;
                    }
                }
                else {
                    if (self->buttonCount == DLGTYPE_OK) {
                        self->buttons[0]->state =
                            CheckTouchRect(0.0, -30.0, (self->buttons[0]->textWidth + (self->buttons[0]->scale * 64.0)) * 0.75, 12.0) >= 0;
                    }
                    else {
                        self->buttons[0]->state =
                            CheckTouchRect(-36.0, -30.0, (self->buttons[0]->textWidth + (self->buttons[0]->scale * 64.0)) * 0.75, 12.0) >= 0;
                        self->buttons[1]->state =
                            CheckTouchRect(36.0, -30.0, (self->buttons[1]->textWidth + (self->buttons[1]->scale * 64.0)) * 0.75, 12.0) >= 0;
                    }
                }
                if (inputDown.left) {
                    usePhysicalControls  = true;
                    self->buttonSelected = 1;
                }
                else if (inputDown.right) {
                    usePhysicalControls  = true;
                    self->buttonSelected = 0;
                }
            }
            else if (touches > 0) {
                usePhysicalControls = false;
            }
            else if (self->buttonCount == DLGTYPE_OK) {
                self->buttonSelected = 0;
                if (inputPress.start || inputPress.A) {
                    self->state = DIALOGPANEL_STATE_ACTION;
                    PlaySfxByName("Menu Select", false);
                    self->buttons[self->buttonSelected]->state = 2;
                }
            }
            else {
                if (inputPress.left) {
                    PlaySfxByName("Menu Move", false);
                    if (--self->buttonSelected < 0)
                        self->buttonSelected = 1;
                }
                if (inputPress.right) {
                    PlaySfxByName("Menu Move", false);
                    if (++self->buttonSelected > 1)
                        self->buttonSelected = 0;
                }
                self->buttons[0]->state                    = 0;
                self->buttons[1]->state                    = 0;
                self->buttons[self->buttonSelected]->state = 1;

                if (inputPress.start || inputPress.A) {
                    self->state = DIALOGPANEL_STATE_ACTION;
                    PlaySfxByName("Menu Select", false);
                    self->buttons[self->buttonSelected]->state = 2;
                }
            }
            if (self->state == 2 && inputPress.B) {
                PlaySfxByName("Menu Back", false);
                self->selection = DLG_NO;
                self->state     = DIALOGPANEL_STATE_EXIT;
            }
            break;
        }
        case DIALOGPANEL_STATE_ACTION:
            SetRenderMatrix(&self->buttonMatrix);
            if (!self->buttons[self->buttonSelected]->state) {
                self->state = DIALOGPANEL_STATE_EXIT;

                self->selection = self->buttonSelected + 1;
                if (self->buttonCount == DLGTYPE_OK)
                    self->selection = DLG_OK;
            }
            break;
        case DIALOGPANEL_STATE_EXIT:
            self->buttonScale = self->buttonScale + ((((self->stateTimer < 0.2) ? 1 : -1) - self->buttonScale) / ((Engine.deltaTime * 60.0) * 8.0));
            if (self->buttonScale < 0.0)
                self->buttonScale = 0.0;
            NewRenderState();
            MatrixScaleXYZF(&self->buttonMatrix, self->buttonScale, self->buttonScale, 1.0);
            MatrixTranslateXYZF(&self->buttonMult, 0.0, 0.0, 160.0);
            MatrixMultiplyF(&self->buttonMatrix, &self->buttonMult);
            SetRenderMatrix(&self->buttonMatrix);
            for (int i = 0; i < self->buttonCount; ++i) memcpy(&self->buttons[i]->renderMatrix, &self->buttonMatrix, sizeof(MatrixF));

            self->stateTimer += Engine.deltaTime;
            if (self->stateTimer > 0.5) {
                for (int i = 0; i < self->buttonCount; ++i) RemoveNativeObject(self->buttons[i]);
                RemoveNativeObject(self);
                return;
            }
            break;
        case DIALOGPANEL_STATE_IDLE: SetRenderMatrix(&self->buttonMatrix); break;
        default: break;
    }
    RenderMesh(self->panelMesh, MESH_COLORS, false);
    RenderText(self->text, FONT_TEXT, self->textX, self->textY, 0.0, self->scale, 255);
}