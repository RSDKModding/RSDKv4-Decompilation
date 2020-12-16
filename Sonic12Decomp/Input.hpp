#ifndef INPUT_H
#define INPUT_H

struct InputData {
    bool up;
    bool down;
    bool left;
    bool right;
    bool A;
    bool B;
    bool C;
    bool start;
};

struct InputButton {
    bool press, hold;
    int keyMappings, contMappings;

    inline void setHeld()
    {
        press = !hold;
        hold  = true;
    }
    inline void setReleased()
    {
        press = false;
        hold  = false;
    }

    inline bool down() { return (press || hold); }
};

extern InputData keyPress;
extern InputData keyDown;

extern bool anyPress;

extern int touchDown[8];
extern int touchX[8];
extern int touchY[8];
extern int touchID[8];
extern int touches;

extern InputButton inputDevice[9];
extern int inputType;

extern int LSTICK_DEADZONE;
extern int RSTICK_DEADZONE;
extern int LTRIGGER_DEADZONE;
extern int RTRIGGER_DEADZONE;

#if RETRO_USING_SDL
extern SDL_GameController *controller;

inline void controllerInit(byte controllerID)
{
    inputType  = 1;
    controller = SDL_GameControllerOpen(controllerID);
};

inline void controllerClose(byte controllerID)
{
    if (controllerID >= 2)
        return;
    inputType = 0;
}
#endif

void ProcessInput();

void CheckKeyPress(InputData *input, byte Flags);
void CheckKeyDown(InputData *input, byte Flags);

#endif // !INPUT_H
