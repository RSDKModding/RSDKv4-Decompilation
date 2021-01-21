#include <windows.h>
#include <unknwn.h>
#include <restrictederrorinfo.h>
#include <hstring.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>

#include <SDL_main.h>
#include "RetroEngine.hpp"


#if RETRO_USING_SDL
int SDL_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    for (int i = 0; i < argc; ++i) {
        if (StrComp(argv[i], "UsingCWD"))
            usingCWD = true;
    }

    Engine.Init();

#ifdef __SWITCH__
    // swap A, B to correct positions
    SDL_GameControllerAddMapping("53776974636820436F6E74726F6C6C65,Switch Controller,"
                                 "a:b0,b:b1,back:b11,"
                                 "dpdown:b15,dpleft:b12,dpright:b14,dpup:b13,"
                                 "leftshoulder:b6,leftstick:b4,lefttrigger:b8,leftx:a0,lefty:a1,"
                                 "rightshoulder:b7,rightstick:b5,righttrigger:b9,rightx:a2,righty:a3,"
                                 "start:b10,x:b3,y:b2");
#endif

#ifdef RETRO_DISABLE_CONTROLLER_HOTSWAP
    controllerInit(0);
#endif

    Engine.Run();

    return 0;
}

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) { return SDL_WinRTRunApp(SDL_main, NULL); }