#include "RetroEngine.hpp"

void MenuControl_Create(void *objPtr)
{
    RSDK_THIS(MenuControl);
    SetMusicTrack("MainMenu.ogg", 0, 1, 106596);
    PlayMusic(0, 0);
    CreateNativeObject(MenuBG_Create, MenuBG_Main);
    CreateNativeObject(RetroGameLoop_Create, RetroGameLoop_Main);
}
void MenuControl_Main(void *objPtr) { RSDK_THIS(MenuControl); }
