#include "RetroEngine.hpp"

void PauseMenu_Create(void *objPtr)
{
    NativeEntity_PauseMenu *pauseMenu = (NativeEntity_PauseMenu *)objPtr;
    pauseMenu->selectedOption         = 0;
}
void PauseMenu_Main(void *objPtr)
{
    CheckKeyDown(&keyDown, 0xFF);
    CheckKeyPress(&keyPress, 0xFF);

    NativeEntity_PauseMenu *pauseMenu = (NativeEntity_PauseMenu *)objPtr;

    if (keyPress.up) {
        if (pauseMenu->selectedOption - 1 < 0) {
            if (!Engine.devMenu)
                pauseMenu->selectedOption = 3;
            else
                pauseMenu->selectedOption = 4;
        }
        --pauseMenu->selectedOption;
        for (int s = 0; s < globalSFXCount; ++s) {
            if (StrComp(sfxNames[s], "MenuMove")) {
                PlaySfx(s, 0);
            }
        }
    }
    else if (keyPress.down) {
        if (!Engine.devMenu)
            pauseMenu->selectedOption = ++pauseMenu->selectedOption % 3;
        else
            pauseMenu->selectedOption = ++pauseMenu->selectedOption % 4;
        for (int s = 0; s < globalSFXCount; ++s) {
            if (StrComp(sfxNames[s], "MenuMove")) {
                PlaySfx(s, 0);
            }
        }
    }

    if (keyPress.A || keyPress.start) {
        switch (pauseMenu->selectedOption) {
            case 0: {
                Engine.gameMode = ENGINE_EXITPAUSE;
                break;
            }
            case 1: {
                Engine.gameMode = ENGINE_EXITPAUSE;
                stageMode = STAGEMODE_LOAD;
                break;
            }
            case 2: {
                initStartMenu(0);
                break;
            }
            case 3: {
                Engine.gameMode = ENGINE_DEVMENU;
                initDevMenu();
                break;
            }
        }
        for (int s = 0; s < globalSFXCount; ++s) {
            if (StrComp(sfxNames[s], "MenuSelect")) {
                PlaySfx(s, 0);
            }
        }
        RemoveNativeObject(pauseMenu);
        CreateNativeObject(RetroGameLoop_Create, RetroGameLoop_Main);
    }
    else if (keyPress.B) {
        Engine.gameMode = ENGINE_EXITPAUSE;
        for (int s = 0; s < globalSFXCount; ++s) {
            if (StrComp(sfxNames[s], "MenuBack")) {
                PlaySfx(s, 0);
            }
        }
        RemoveNativeObject(pauseMenu);
    }
}