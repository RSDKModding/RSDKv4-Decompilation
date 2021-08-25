#include "RetroEngine.hpp"

void MenuControl_Create(void *objPtr)
{
    NativeEntity_MenuControl *entity = (NativeEntity_MenuControl *)objPtr;
    SetMusicTrack("MainMenu.ogg", 0, 1, 106596);
    PlayMusic(0, 0);
    CreateNativeObject(MenuBG_Create, MenuBG_Main);
}
void MenuControl_Main(void *objPtr) { NativeEntity_MenuControl *entity = (NativeEntity_MenuControl *)objPtr; }
