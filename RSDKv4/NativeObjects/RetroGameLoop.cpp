#include "RetroEngine.hpp"

void InitPauseMenu()
{
    PauseSound();
    ClearNativeObjects();
    CREATE_ENTITY(MenuBG);
    CREATE_ENTITY(PauseMenu);
}

void RetroGameLoop_Create(void *objPtr) { mixFiltersOnJekyll = Engine.useHighResAssets; }
void RetroGameLoop_Main(void *objPtr)
{
    switch (Engine.gameMode) {
        case ENGINE_DEVMENU:
#if RETRO_HARDWARE_RENDER
            gfxIndexSize        = 0;
            gfxVertexSize       = 0;
            gfxIndexSizeOpaque  = 0;
            gfxVertexSizeOpaque = 0;
#endif

            ProcessStageSelect();
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
            InitDevMenu();
            ResetCurrentStageFolder();
            break;

        case ENGINE_WAIT: break;

        case ENGINE_SCRIPTERROR:
            Engine.LoadGameConfig("Data/Game/GameConfig.bin");
            InitErrorMessage();
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
#if !RETRO_USE_ORIGINAL_CODE
            if (skipStartMenu) {
                activeStageList   = 0;
                stageListPosition = 0;
                stageMode         = STAGEMODE_LOAD;
                Engine.gameMode   = ENGINE_MAINGAME;
            }
            else {
                RestoreNativeObjects();
                Engine.LoadGameConfig("Data/Game/GameConfig.bin");
                activeStageList   = 0;
                stageListPosition = 0;
            }
#else
            RestoreNativeObjects();
            Engine.LoadGameConfig("Data/Game/GameConfig.bin");
            activeStageList   = 0;
            stageListPosition = 0;
#endif
            break;

        case ENGINE_RESETGAME: // Also called when 2P VS disconnects
            ClearScreen(1);
            TransferRetroBuffer();
#if !RETRO_USE_ORIGINAL_CODE
            if (skipStartMenu) {
                activeStageList   = 0;
                stageListPosition = 0;
                stageMode         = STAGEMODE_LOAD;
                Engine.gameMode   = ENGINE_MAINGAME;
            }
            else
                RestoreNativeObjects();
#else
            RestoreNativeObjects();
#endif
            break;

#if !RETRO_USE_ORIGINAL_CODE && RETRO_USE_NETWORKING
        case ENGINE_CONNECT2PVS: {
            CREATE_ENTITY(MultiplayerScreen)->bg = CREATE_ENTITY(MenuBG);
            NativeEntity_FadeScreen *fade        = CREATE_ENTITY(FadeScreen);
            fade->state                          = FADESCREEN_STATE_FADEIN;
            fade->delay                          = 1.5;
            fade->fadeSpeed                      = 1.0;
            Engine.gameMode                      = ENGINE_WAIT2PVS;
            break;
        }
        case ENGINE_WAIT2PVS:
            // wait for vs response
            if (dcError)
                CREATE_ENTITY(MultiplayerHandler);
            break;

#endif
#if RETRO_USE_MOD_LOADER
        case ENGINE_INITMODMENU:
            Engine.LoadGameConfig("Data/Game/GameConfig.bin");
            InitDevMenu();

            ResetCurrentStageFolder();

            SetupTextMenu(&gameMenu[0], 0);
            AddTextMenuEntry(&gameMenu[0], "MOD LIST");
            SetupTextMenu(&gameMenu[1], 0);
            InitMods(); // reload mods

            char buffer[0x100];
            for (int m = 0; m < modList.size(); ++m) {
                StrCopy(buffer, modList[m].name.c_str());
                StrAdd(buffer, ": ");
                StrAdd(buffer, modList[m].active ? "  Active" : "Inactive");
                AddTextMenuEntry(&gameMenu[1], buffer);
            }

            gameMenu[1].alignment      = MENU_ALIGN_RIGHT;
            gameMenu[1].selectionCount = 3;
            gameMenu[1].selection1     = 0;
            if (gameMenu[1].rowCount > 18)
                gameMenu[1].visibleRowCount = 18;
            else
                gameMenu[1].visibleRowCount = 0;

            gameMenu[0].alignment        = MENU_ALIGN_CENTER;
            gameMenu[0].selectionCount   = 1;
            gameMenu[1].timer            = 0;
            gameMenu[1].visibleRowOffset = 0;
            stageMode                    = DEVMENU_MODMENU;
            break;
#endif
        default:
            PrintLog("GameMode '%d' Called", Engine.gameMode);
            activeStageList   = 0;
            stageListPosition = 0;
            stageMode         = STAGEMODE_LOAD;
            Engine.gameMode   = ENGINE_MAINGAME;
            break;
    }
}
