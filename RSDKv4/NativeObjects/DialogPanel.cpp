#include "RetroEngine.hpp"

void DialogPanel_Create(void *objPtr)
{
    RSDK_THIS(DialogPanel);
    entity->panelMesh = LoadMesh("Data/Game/Models/Panel.bin", 255);
    SetMeshVertexColors(entity->panelMesh, 0x28, 0x5C, 0xB0, 0xFF);
    entity->buttonCount = 2;
}
void DialogPanel_Main(void *objPtr) { RSDK_THIS(DialogPanel); }

void DialogPanel_CheckTouch(void *objPtr) { RSDK_THIS(DialogPanel); }