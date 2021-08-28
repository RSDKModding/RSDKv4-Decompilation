#include "RetroEngine.hpp"

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; ++i) {
        if (StrComp(argv[i], "UsingCWD"))
            usingCWD = true;
    }
    
#if RETRO_USING_SDL1 || RETRO_USING_SDL2
    SDL_SetHint(SDL_HINT_WINRT_HANDLE_BACK_BUTTON, "1");
#endif

    Engine.Init();
    Engine.Run();

    return 0;
}

#if RETRO_PLATFORM == RETRO_UWP
int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) { return SDL_WinRTRunApp(main, NULL); }
#endif
