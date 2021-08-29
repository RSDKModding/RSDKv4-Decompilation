#include "RetroEngine.hpp"

void InitPauseMenu()
{
    PauseSound();
    ClearNativeObjects();
    CREATE_ENTITY(MenuBG);
    CREATE_ENTITY(PauseMenu);
}

void RetroGameLoop_Create(void *objPtr)
{
    RSDK_THIS(RetroGameLoop);
    mixFiltersOnJekyll = Engine.useHighResAssets;
}
void RetroGameLoop_Main(void *objPtr)
{
    RSDK_THIS(RetroGameLoop);

    switch (Engine.gameMode) {
        case ENGINE_DEVMENU:
#if RETRO_HARDWARE_RENDER
            gfxIndexSize        = 0;
            gfxVertexSize       = 0;
            gfxIndexSizeOpaque  = 0;
            gfxVertexSizeOpaque = 0;
#endif

            processStageSelect();
            TransferRetroBuffer();
            RenderRetroBuffer(64, 160.0);
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
            TransferRetroBuffer();
            RenderRetroBuffer(64, 160.0);
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
            mixFiltersOnJekyll = false;
            InitPauseMenu();
            break;
        case ENGINE_EXITPAUSE:
            Engine.gameMode = ENGINE_MAINGAME;
            ResumeSound();
            TransferRetroBuffer();
            break;
        case ENGINE_ENDGAME:
            ClearScreen(1);
            TransferRetroBuffer();
            RestoreNativeObjects();
            Engine.LoadGameConfig("Data/Game/GameConfig.bin");
            activeStageList   = 0;
            stageListPosition = 0;
            break;
        case ENGINE_RESETGAME: // Also called when 2P VS disconnects
            ClearScreen(1);
            TransferRetroBuffer();
            RestoreNativeObjects();
            break;
#if !RETRO_USE_ORIGINAL_CODE
        case ENGINE_CONNECT2PVS:
            // connect screen goes here
            break;
#if RETRO_USE_MOD_LOADER
        case ENGINE_INITMODMENU:
            Engine.LoadGameConfig("Data/Game/GameConfig.bin");
            initDevMenu();

            ResetCurrentStageFolder();

            SetupTextMenu(&gameMenu[0], 0);
            AddTextMenuEntry(&gameMenu[0], "MOD LIST");
            SetupTextMenu(&gameMenu[1], 0);
            initMods(); // reload mods

            char buffer[0x100];
            for (int m = 0; m < modList.size(); ++m) {
                StrCopy(buffer, modList[m].name.c_str());
                StrAdd(buffer, ": ");
                StrAdd(buffer, modList[m].active ? "  Active" : "Inactive");
                AddTextMenuEntry(&gameMenu[1], buffer);
            }

            gameMenu[1].alignment      = 1;
            gameMenu[1].selectionCount = 3;
            gameMenu[1].selection1     = 0;
            if (gameMenu[1].rowCount > 18)
                gameMenu[1].visibleRowCount = 18;
            else
                gameMenu[1].visibleRowCount = 0;

            gameMenu[0].alignment        = 2;
            gameMenu[0].selectionCount   = 1;
            gameMenu[1].timer            = 0;
            gameMenu[1].visibleRowOffset = 0;
            stageMode                    = DEVMENU_MODMENU;
            break;
#endif
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
