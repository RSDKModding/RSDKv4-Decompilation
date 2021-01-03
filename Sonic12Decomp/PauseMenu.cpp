#include "RetroEngine.hpp"

void PauseMenu_Create(void *objPtr)
{
    NativeEntity_PauseMenu *pauseMenu = (NativeEntity_PauseMenu *)objPtr;
    pauseMenu->selectedOption         = 0;
}
void PauseMenu_Main(void *objPtr)
{
    NativeEntity_PauseMenu *pauseMenu = (NativeEntity_PauseMenu *)objPtr;

    if (keyPress.A || keyPress.start) {
        switch (pauseMenu->selectedOption) {
            case 0: {
                Engine.gameMode = ENGINE_EXITPAUSE;
                break;
            }
            case 1: {
                stageMode = STAGEMODE_LOAD;
                break;
            }
            case 2: {
                initStartMenu(0);
                break;
            }
        }
        // RemoveNativeObject(pauseMenu);
    }
}