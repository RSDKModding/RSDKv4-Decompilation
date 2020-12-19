#include "RetroEngine.hpp"

void RetroGameLoop_Create(void *objPtr) {
    //NativeEntity_RetroGameLoop *entity = (NativeEntity_RetroGameLoop *)objPtr;
}
void RetroGameLoop_Main(void *objPtr)
{
    NativeEntity_RetroGameLoop *entity = (NativeEntity_RetroGameLoop *)objPtr;

    switch (Engine.gameMode) {
        case ENGINE_DEVMENU: processStageSelect(); break;
        case ENGINE_MAINGAME: ProcessStage(); break;
        case ENGINE_INITDEVMENU:
            Engine.LoadGameConfig("Data/Game/GameConfig.bin");
            initDevMenu();
            ResetCurrentStageFolder();
            break;
        case ENGINE_WAIT: break;
        case ENGINE_SCRIPTERROR:
            Engine.LoadGameConfig("Data/Game/GameConfig.bin");
            initErrorMessage();
            ResetCurrentStageFolder();
            break;
        case ENGINE_INITPAUSE:
            Engine.gameMode = ENGINE_MAINGAME;
            stageMode    = STAGEMODE_UNKNOWN; // temp until/if nativeObjs are done
            pauseEnabled = true;
            PauseSound();
            ClearNativeObjects();
            // CreateNativeObject(MenuBG_Create, MenuBG_Main);
            // CreateNativeObject(PauseMenu_Create, PauseMenu_Main);
            break;
        case ENGINE_EXITPAUSE:
            ClearScreen(1);
            Engine.gameMode = ENGINE_MAINGAME;
            stageMode    = STAGEMODE_NORMAL;
            pauseEnabled = false;
            break;
        case ENGINE_7:
            ClearScreen(1);
            RestoreNativeObjects();
            Engine.LoadGameConfig("Data/Game/GameConfig.bin");
#if RSDK_DEBUG
            printLog("GameMode 7 Called"); // End Game?
#endif
            activeStageList   = 0;
            stageListPosition = 0;
            stageMode         = STAGEMODE_LOAD;
            Engine.gameMode   = ENGINE_MAINGAME;
            break;
        case ENGINE_8: ClearScreen(1); RestoreNativeObjects();
#if RSDK_DEBUG
            printLog("GameMode 8 Called");
#endif
            activeStageList   = 0;
            stageListPosition = 0;
            stageMode         = STAGEMODE_LOAD;
            Engine.gameMode   = ENGINE_MAINGAME;
            break;
        default:
#if RSDK_DEBUG
            printLog("GameMode '%d' Called", Engine.gameMode);
#endif
            activeStageList   = 0;
            stageListPosition = 0;
            stageMode         = STAGEMODE_LOAD;
            Engine.gameMode   = ENGINE_MAINGAME;
            break;
    }
}