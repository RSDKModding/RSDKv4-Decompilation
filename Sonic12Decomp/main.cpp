#include "RetroEngine.hpp"

#if RETRO_PLATFORM == RETRO_NX
#include <switch.h>

extern "C" {
    void userAppInit(void);
    void userAppExit(void);
}

void userAppInit(void)
{
    hidInitialize();
    romfsInit();
}

void userAppExit(void)
{
    romfsExit();
    hidExit();
}
#endif

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; ++i) {
        if (StrComp(argv[i], "UsingCWD"))
            usingCWD = true;
    }

    Engine.Init();

#ifdef RETRO_DISABLE_CONTROLLER_HOTSWAP
    controllerInit(0);
#endif

    Engine.Run();

    return 0;
}
