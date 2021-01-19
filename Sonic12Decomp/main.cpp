#include "RetroEngine.hpp"

int main(int argc, char *argv[])
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
