#include "RetroEngine.hpp"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"

void main_loop()
{
	Engine.Run();
}
#endif


int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; ++i) {
        if (StrComp(argv[i], "UsingCWD"))
            usingCWD = true;
    }
    
    SDL_SetHint(SDL_HINT_WINRT_HANDLE_BACK_BUTTON, "1");
    Engine.Init();
#if RETRO_USING_SDL2
    controllerInit(0);
#endif

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
#endif

#ifndef __EMSCRIPTEN__
    Engine.Run();
#endif

    return 0;
}

#if RETRO_PLATFORM == RETRO_UWP
int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) { return SDL_WinRTRunApp(main, NULL); }
#endif
