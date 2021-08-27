#include "RetroEngine.hpp"

TextMenu pauseTextMenu;

int CheckTouchRect(int x1, int y1, int x2, int y2)
{
    for (int f = 0; f < touches; ++f) {
        if (touchDown[f] && touchX[f] > x1 && touchX[f] < x2 && touchY[f] > y1 && touchY[f] < y2) {
            return f;
        }
    }
    return -1;
}

void PauseMenu_Create(void *objPtr)
{
    RSDK_THIS(PauseMenu);
    entity->retroGameLoop    = (NativeEntity_RetroGameLoop *)GetNativeObject(0);
    entity->label            = CREATE_ENTITY(TextLabel);
    entity->label->alignment = 0;
    entity->label->textZ     = 0.0;
    entity->label->textScale = 0.2;
    entity->label->textAlpha = 0;
    entity->label->fontID    = 0;
    SetStringToFont(entity->label->text, strPause, 0);
    entity->label->textWidth = 512.0;
    entity->renderRot        = DegreesToRad(22.5);
    matrixRotateYF(&entity->label->renderMatrix, DegreesToRad(22.5));
    matrixTranslateXYZF(&entity->matrix, -128.0, 80.0, 160.0);
    matrixMultiplyF(&entity->label->renderMatrix, &entity->matrix);
    entity->label->useMatrix = 1;
    entity->dword100         = ((SCREEN_CENTERX_F + -160.0) * -0.5) + -128.0;
    for (int i = 0; i < 4; ++i) {
        NativeEntity_SubMenuButton *button = CREATE_ENTITY(SubMenuButton);
        entity->buttons[i]                 = button;
        button->textScale                  = 0.1;
        button->matZ                       = 0.0;
        button->matXOff                    = 512.0;
        button->textY                      = -4.0;
        entity->buttonRot[i]               = DegreesToRad(16.0);
        matrixRotateYF(&button->matrix, DegreesToRad(16.0));
        matrixTranslateXYZF(&entity->matrix, entity->dword100, 48.0 - i * 30, 160.0);
        matrixMultiplyF(&entity->buttons[0]->matrix, &entity->matrix);
        button->symbol    = 1;
        button->useMatrix = 1;
    }
    if (GetGlobalVariableByName("player.lives") <= 1 && GetGlobalVariableByName("options.gameMode") <= 1 || !activeStageList
        || GetGlobalVariableByName("options.attractMode") == 1) {
        entity->buttons[1]->r = 0x80;
        entity->buttons[1]->g = 0x80;
        entity->buttons[1]->b = 0x80;
    }
    SetStringToFont(entity->buttons[0]->text, strContinue, 1);
    SetStringToFont(entity->buttons[1]->text, strRestart, 1);
    SetStringToFont(entity->buttons[2]->text, strSettings, 1);
    SetStringToFont(entity->buttons[3]->text, strExit, 1);
    entity->textureCircle = LoadTexture("Data/Game/Menu/Circle.png", TEXFMT_RGBA4444);
    entity->rotationY     = 0.0;
    entity->float118      = DegreesToRad(-16.0);
    entity->matrixX       = 0.0;
    entity->matrixY       = 0.0;
    entity->matrixZ       = 160.0;
    entity->float108      = (1.75 * SCREEN_CENTERX_F) - ((SCREEN_CENTERX_F - 160) * 2);
    if (Engine.gameDeviceType == RETRO_MOBILE)
        entity->textureDPad = LoadTexture("Data/Game/Menu/VirtualDPad.png", TEXFMT_RGBA8888);
    entity->pfunc128 = 104.0;
    entity->state    = 1;
    entity->byte131  = 1;
    entity->float120 = SCREEN_CENTERX_F - 76.0;
    entity->float124 = SCREEN_CENTERX_F - 52.0;
}
void PauseMenu_Main(void *objPtr) { RSDK_THIS(PauseMenu); }