#include "RetroEngine.hpp"

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; ++i) {
        if (StrComp(argv[i], "UsingCWD"))
            usingCWD = true;
    }

    Engine.Init();
    controllerInit(0);
    Engine.Run();

    return 0;
}
