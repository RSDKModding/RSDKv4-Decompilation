#include "RetroEngine.hpp"

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; ++i) {
#if !RETRO_USE_ORIGINAL_CODE
        if (StrComp(argv[i], "UsingCWD"))
            usingCWD = true;
#endif
    }

#if !RETRO_USE_ORIGINAL_CODE && RETRO_USING_SDL2
    SDL_SetHint(SDL_HINT_WINRT_HANDLE_BACK_BUTTON, "1");
#endif
    Engine.Init();
#if !RETRO_USE_ORIGINAL_CODE
#if RETRO_USING_SDL2
    controllerInit(0);
#endif
#endif
    Engine.Run();

    return 0;
}

#if RETRO_PLATFORM == RETRO_UWP
int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) { return SDL_WinRTRunApp(main, NULL); }
#endif
