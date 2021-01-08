#include "RetroEngine.hpp"

void RetroGameLoop_Create(void *objPtr) {
    //NativeEntity_RetroGameLoop *entity = (NativeEntity_RetroGameLoop *)objPtr;
}
void RetroGameLoop_Main(void *objPtr)
{
    //NativeEntity_RetroGameLoop *entity = (NativeEntity_RetroGameLoop *)objPtr;

    switch (Engine.gameMode) {
        case ENGINE_DEVMENU: processStageSelect(); break;
        case ENGINE_MAINGAME:
            if (Engine.finishedStartMenu)
                ProcessStage();
            else
                processStartMenu();
            break;
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
            //Engine.gameMode = ENGINE_MAINGAME;
            //stageMode    = STAGEMODE_PAUSED; // temp until/if nativeObjs are fully complete
            PauseSound();
            ClearNativeObjects();
            // CreateNativeObject(MenuBG_Create, MenuBG_Main); // temp until/if nativeObjs are fully complete
            CreateNativeObject(PauseMenu_Create, PauseMenu_Main);
            break;
        case ENGINE_EXITPAUSE:
            Engine.gameMode = ENGINE_MAINGAME;
            ResumeSound();
            break;
        case ENGINE_ENDGAME:
            ClearScreen(1);
            //RestoreNativeObjects();
            Engine.LoadGameConfig("Data/Game/GameConfig.bin");
            activeStageList   = 0;
            stageListPosition = 0;
            initStartMenu(0);
            break;
        case ENGINE_FINISHTA: //Also called when 2P VS disconnects
            ClearScreen(1); 
            //RestoreNativeObjects();
            initStartMenu(1);
            break;
        case ENGINE_CONNECT2PVS: 
            //connect screen goes here
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
