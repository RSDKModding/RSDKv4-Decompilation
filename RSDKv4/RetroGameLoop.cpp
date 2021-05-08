#include "RetroEngine.hpp"

void RetroGameLoop_Create(void *objPtr)
{
    NativeEntity_RetroGameLoop *entity = (NativeEntity_RetroGameLoop *)objPtr;
    entity->pauseMenu                  = nullptr;
}
void RetroGameLoop_Main(void *objPtr)
{
    NativeEntity_RetroGameLoop *entity = (NativeEntity_RetroGameLoop *)objPtr;

    switch (Engine.gameMode) {
        case ENGINE_DEVMENU:
#if RETRO_HARDWARE_RENDER
            gfxIndexSize        = 0;
            gfxVertexSize       = 0;
            gfxIndexSizeOpaque  = 0;
            gfxVertexSizeOpaque = 0;
#endif
#if !RETRO_USE_ORIGINAL_CODE
            if (entity->pauseMenu && nativeEntityCount > 1) // dumb fix but yknow how it is
                RemoveNativeObject(entity->pauseMenu);
            entity->pauseMenu = nullptr;
#endif

            processStageSelect();
            break;
        case ENGINE_MAINGAME:
#if RETRO_HARDWARE_RENDER
            gfxIndexSize        = 0;
            gfxVertexSize       = 0;
            gfxIndexSizeOpaque  = 0;
            gfxVertexSizeOpaque = 0;
            vertexSize3D        = 0;
            indexSize3D         = 0;
            render3DEnabled     = false;
#endif
            ProcessStage();
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
#if !RETRO_USE_ORIGINAL_CODE
            if (nativeEntityCount > 1) {
                Engine.gameMode = ENGINE_MAINGAME;
                StopSFXByName("MenuBack");
                break;
            }
#endif
            PauseSound();
            // ClearNativeObjects();
            Engine.gameMode = ENGINE_WAIT; // temp (maybe?) so pause menu renders on top
                                           // CreateNativeObject(MenuBG_Create, MenuBG_Main); // temp until/if nativeObjs are fully complete
#if !RETRO_USE_ORIGINAL_CODE
            entity->pauseMenu = (NativeEntity_PauseMenu *)CreateNativeObject(PauseMenu_Create, PauseMenu_Main);
#endif
            break;
        case ENGINE_EXITPAUSE:
            Engine.gameMode = ENGINE_MAINGAME;
            ResumeSound();
            // if (entity->pauseMenu)
            //    RemoveNativeObject(entity->pauseMenu);
            // entity->pauseMenu = nullptr;
            break;
        case ENGINE_ENDGAME:
            ClearScreen(1);
            // RestoreNativeObjects();
            Engine.LoadGameConfig("Data/Game/GameConfig.bin");
            activeStageList   = 0;
            stageListPosition = 0;
#if !RETRO_USE_ORIGINAL_CODE
            initStartMenu(0);
#endif
            break;
        case ENGINE_RESETGAME: // Also called when 2P VS disconnects
            ClearScreen(1);
            // RestoreNativeObjects();
#if !RETRO_USE_ORIGINAL_CODE
            initStartMenu(1);
#endif
            break;
#if !RETRO_USE_ORIGINAL_CODE
        case ENGINE_STARTMENU: processStartMenu(); break;
        case ENGINE_CONNECT2PVS:
            // connect screen goes here
            break;
#endif
        default:
            printLog("GameMode '%d' Called", Engine.gameMode);
            activeStageList   = 0;
            stageListPosition = 0;
            stageMode         = STAGEMODE_LOAD;
            Engine.gameMode   = ENGINE_MAINGAME;
            break;
    }
}
