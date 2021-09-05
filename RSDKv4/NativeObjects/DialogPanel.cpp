#include "RetroEngine.hpp"

void DialogPanel_Create(void *objPtr)
{
    RSDK_THIS(DialogPanel);
    entity->panelMesh = LoadMesh("Data/Game/Models/Panel.bin", -1);
    SetMeshVertexColors(entity->panelMesh, 0x28, 0x5C, 0xB0, 0xFF);
    entity->buttonCount = DLGTYPE_YESNO;
}

void DialogPanel_Main(void *objPtr)
{
    RSDK_THIS(DialogPanel);
    NewRenderState();
    SetRenderBlendMode(RENDER_BLEND_ALPHA);
    switch (entity->state) {
        case DIALOGPANEL_STATE_SETUP: {
            NativeEntity_PushButton *confirmButton = CREATE_ENTITY(PushButton);
            entity->buttons[0]                     = confirmButton;
            if (entity->buttonCount == DLGTYPE_OK) {
                confirmButton->x                = 0.0;
                confirmButton->y                = -40.0;
                confirmButton->z                = 0.0;
                confirmButton->scale            = 0.25;
                confirmButton->bgColour         = 0x00A048;
                confirmButton->bgColourSelected = 0x00C060;
                confirmButton->useRenderMatrix  = true;
                SetStringToFont8(confirmButton->text, " OK ", FONT_LABEL);
            }
            else {
                confirmButton->x                = -48.0;
                confirmButton->y                = -40.0;
                confirmButton->z                = 0.0;
                confirmButton->scale            = 0.25;
                confirmButton->bgColour         = 0x00A048;
                confirmButton->bgColourSelected = 0x00C060;
                confirmButton->useRenderMatrix  = true;
                SetStringToFont(confirmButton->text, strYes, FONT_LABEL);

                NativeEntity_PushButton *noButton = CREATE_ENTITY(PushButton);
                entity->buttons[1]                = noButton;
                noButton->useRenderMatrix         = true;
                noButton->scale                   = 0.25;
                noButton->x                       = 48.0;
                noButton->y                       = -40.0;
                noButton->z                       = 0.0;
                SetStringToFont(noButton->text, strNo, FONT_LABEL);
            }
            entity->scale = 224.0 / (GetTextWidth(entity->text, FONT_TEXT, 1.0) + 1.0);
            if (entity->scale > 0.4)
                entity->scale = 0.4;
            entity->textX = GetTextWidth(entity->text, FONT_TEXT, entity->scale) * -0.5;
            entity->textY = GetTextHeight(entity->text, FONT_TEXT, entity->scale) * 0.5;
            entity->state = DIALOGPANEL_STATE_ENTER;
        }
        // FallThrough
        case DIALOGPANEL_STATE_ENTER: {
            entity->buttonScale += ((0.77 - entity->buttonScale) / ((Engine.deltaTime * 60.0) * 8.0));
            if (entity->buttonScale > 0.75)
                entity->buttonScale = 0.75;
            NewRenderState();
            matrixScaleXYZF(&entity->buttonMatrix, entity->buttonScale, entity->buttonScale, 1.0);
            matrixTranslateXYZF(&entity->buttonMult, 0.0, 0.0, 160.0);
            matrixMultiplyF(&entity->buttonMatrix, &entity->buttonMult);
            SetRenderMatrix(&entity->buttonMatrix);
            for (int i = 0; i < entity->buttonCount; ++i) memcpy(&entity->buttons[i]->renderMatrix, &entity->buttonMatrix, sizeof(MatrixF));

            entity->stateTimer += Engine.deltaTime;
            if (entity->stateTimer > 0.5) {
                entity->state      = DIALOGPANEL_STATE_MAIN;
                entity->stateTimer = 0.0;
            }
            break;
        }
        case DIALOGPANEL_STATE_MAIN: {
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            SetRenderMatrix(&entity->buttonMatrix);
            if (!usePhysicalControls) {
                if (touches < 1) {
                    if (entity->buttons[0]->state == 1) {
                        entity->buttonSelected = 0;
                        entity->state          = DIALOGPANEL_STATE_ACTION;
                        PlaySfxByName("Menu Select", false);
                        entity->buttons[0]->state = 2;
                    }
                    if (entity->buttonCount == DLGTYPE_YESNO && entity->buttons[1]->state == 1) {
                        entity->buttonSelected = 1;
                        entity->state          = DIALOGPANEL_STATE_ACTION;
                        PlaySfxByName("Menu Select", false);
                        entity->buttons[1]->state = 2;
                    }
                }
                else {
                    if (entity->buttonCount == DLGTYPE_OK) {
                        entity->buttons[0]->state =
                            CheckTouchRect(0.0, -30.0, (entity->buttons[0]->textWidth + (entity->buttons[0]->scale * 64.0)) * 0.75, 12.0) >= 0;
                    }
                    else {
                        entity->buttons[0]->state =
                            CheckTouchRect(-36.0, -30.0, (entity->buttons[0]->textWidth + (entity->buttons[0]->scale * 64.0)) * 0.75, 12.0) >= 0;
                        entity->buttons[1]->state =
                            CheckTouchRect(36.0, -30.0, (entity->buttons[1]->textWidth + (entity->buttons[1]->scale * 64.0)) * 0.75, 12.0) >= 0;
                    }
                }
                if (keyDown.left) {
                    usePhysicalControls    = true;
                    entity->buttonSelected = 1;
                }
                else if (keyDown.right) {
                    usePhysicalControls    = true;
                    entity->buttonSelected = 0;
                }
            }
            else if (touches > 0) {
                usePhysicalControls = false;
            }
            else if (entity->buttonCount == DLGTYPE_OK) {
                entity->buttonSelected = 0;
                if (keyPress.start || keyPress.A) {
                    entity->state = DIALOGPANEL_STATE_ACTION;
                    PlaySfxByName("Menu Select", false);
                    entity->buttons[entity->buttonSelected]->state = 2;
                }
            }
            else {
                if (keyPress.left) {
                    PlaySfxByName("Menu Move", false);
                    if (--entity->buttonSelected < 0)
                        entity->buttonSelected = 1;
                }
                if (keyPress.right) {
                    PlaySfxByName("Menu Move", false);
                    if (++entity->buttonSelected > 1)
                        entity->buttonSelected = 0;
                }
                entity->buttons[0]->state                      = 0;
                entity->buttons[1]->state                      = 0;
                entity->buttons[entity->buttonSelected]->state = 1;

                if (keyPress.start || keyPress.A) {
                    entity->state = DIALOGPANEL_STATE_ACTION;
                    PlaySfxByName("Menu Select", false);
                    entity->buttons[entity->buttonSelected]->state = 2;
                }
            }
            if (entity->state == 2 && keyPress.B) {
                PlaySfxByName("Menu Back", false);
                entity->selection = DLG_NO;
                entity->state     = DIALOGPANEL_STATE_EXIT;
            }
            break;
        }
        case DIALOGPANEL_STATE_ACTION:
            SetRenderMatrix(&entity->buttonMatrix);
            if (!entity->buttons[entity->buttonSelected]->state) {
                entity->selection = entity->buttonSelected + 1;
                entity->state     = DIALOGPANEL_STATE_EXIT;
                if (entity->buttonCount == DLGTYPE_OK)
                    entity->selection = DLG_OK;
            }
            break;
        case DIALOGPANEL_STATE_EXIT:
            entity->buttonScale =
                entity->buttonScale + ((((entity->stateTimer < 0.2) ? 1 : -1) - entity->buttonScale) / ((Engine.deltaTime * 60.0) * 8.0));
            if (entity->buttonScale < 0.0)
                entity->buttonScale = 0.0;
            NewRenderState();
            matrixScaleXYZF(&entity->buttonMatrix, entity->buttonScale, entity->buttonScale, 1.0);
            matrixTranslateXYZF(&entity->buttonMult, 0.0, 0.0, 160.0);
            matrixMultiplyF(&entity->buttonMatrix, &entity->buttonMult);
            SetRenderMatrix(&entity->buttonMatrix);
            for (int i = 0; i < entity->buttonCount; ++i) memcpy(&entity->buttons[i]->renderMatrix, &entity->buttonMatrix, sizeof(MatrixF));

            entity->stateTimer += Engine.deltaTime;
            if (entity->stateTimer > 0.5) {
                // TODO: is this conditional?
                for (int i = 0; i < entity->buttonCount; ++i) RemoveNativeObject(entity->buttons[i]);
                RemoveNativeObject(entity);
                return;
            }
            break;
        case DIALOGPANEL_STATE_IDLE: SetRenderMatrix(&entity->buttonMatrix); break;
        default: break;
    }
    RenderMesh(entity->panelMesh, MESH_COLOURS, false);
    RenderText(entity->text, FONT_TEXT, entity->textX, entity->textY, 0.0, entity->scale, 255);
}