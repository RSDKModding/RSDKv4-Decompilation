#include "RetroEngine.hpp"

ushort helpText[0x1000];

void InstructionsScreen_Create(void *objPtr)
{
    RSDK_THIS(InstructionsScreen);

    entity->labelPtr                  = CREATE_ENTITY(TextLabel);
    entity->labelPtr->useMatrix       = true;
    entity->labelPtr->fontID          = 0;
    entity->labelPtr->textScale       = 0.2;
    entity->labelPtr->textAlpha       = 256;
    entity->labelPtr->textX           = -144.0;
    entity->labelPtr->textY           = 100.0;
    entity->labelPtr->textZ           = 16.0;
    entity->labelPtr->alignment       = 0;
    SetStringToFont(entity->labelPtr->text, strInstructions, 0);
    SetStringToFont8(entity->pageIDText, (char*)"1 / 5", 2);
    SetStringToFont(helpText, strHelpText1, 2);

    entity->meshPanel = LoadMesh("Data/Game/Models/Panel.bin", 255);
    SetMeshVertexColors(entity->meshPanel, 0, 0, 0, 0xC0);
    entity->textureArrows = LoadTexture("Data/Game/Menu/ArrowButtons.png", 1);
    entity->touchValid2   = 0;
    entity->textY         = 68.0;
    entity->floatC8       = (GetTextHeight(helpText, 2, 0.14) - 152.0) + 68.0;
    if (Engine.gameDeviceType == RETRO_STANDARD) {
        entity->textureDPad = LoadTexture("Data/Game/Menu/Generic.png", TEXFMT_RGBA8888);
        if (timeAttackTex[0])
            ReplaceTexture("Data/Game/Menu/HelpGraphics.png", timeAttackTex[0]);
        else
            entity->textureHelp = LoadTexture("Data/Game/Menu/HelpGraphics.png", TEXFMT_RGBA5551);
    }
    else {
        entity->textureDPad = LoadTexture("Data/Game/Menu/VirtualDPad.png", TEXFMT_RGBA8888);
        if (timeAttackTex[0])
            ReplaceTexture("Data/Game/Menu/HelpGraphics.png", timeAttackTex[0]);
        else
            entity->textureHelp = LoadTexture("Data/Game/Menu/HelpGraphics.png", TEXFMT_RGBA5551);
    }
}
void InstructionsScreen_Main(void *objPtr) { RSDK_THIS(InstructionsScreen); }
