#include "RetroEngine.hpp"

int main(int argc, char *argv[])
{
#if RETRO_PLATFORM != RETRO_VITA
    for (int i = 0; i < argc; ++i) {
        if (StrComp(argv[i], "UsingCWD"))
            usingCWD = true;
    }
#endif

    Engine.Init();
    Engine.Run();

    return 0;
}
