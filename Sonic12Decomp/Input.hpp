#ifndef INPUT_H
#define INPUT_H

enum InputButtons {
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_BUTTONA,
    INPUT_BUTTONB,
    INPUT_BUTTONC,
    INPUT_BUTTONX,
    INPUT_BUTTONY,
    INPUT_BUTTONZ,
    INPUT_BUTTONL,
    INPUT_BUTTONR,
    INPUT_START,
    INPUT_SELECT,
    INPUT_ANY,
    INPUT_MAX,
};

struct InputData {
    bool up;
    bool down;
    bool left;
    bool right;
    bool A;
    bool B;
    bool C;
    bool X;
    bool Y;
    bool Z;
    bool L;
    bool R;
    bool start;
    bool select;
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

extern InputButton inputDevice[INPUT_MAX];
extern int inputType;

extern int LSTICK_DEADZONE;
extern int RSTICK_DEADZONE;
extern int LTRIGGER_DEADZONE;
extern int RTRIGGER_DEADZONE;

#if RETRO_USING_SDL2
extern SDL_GameController *controller;

// Easier this way
enum ExtraSDLButtons {
    SDL_CONTROLLER_BUTTON_ZL = SDL_CONTROLLER_BUTTON_MAX + 1,
    SDL_CONTROLLER_BUTTON_ZR,
    SDL_CONTROLLER_BUTTON_LSTICK_UP,
    SDL_CONTROLLER_BUTTON_LSTICK_DOWN,
    SDL_CONTROLLER_BUTTON_LSTICK_LEFT,
    SDL_CONTROLLER_BUTTON_LSTICK_RIGHT,
    SDL_CONTROLLER_BUTTON_RSTICK_UP,
    SDL_CONTROLLER_BUTTON_RSTICK_DOWN,
    SDL_CONTROLLER_BUTTON_RSTICK_LEFT,
    SDL_CONTROLLER_BUTTON_RSTICK_RIGHT,
    SDL_CONTROLLER_BUTTON_MAX_EXTRA,
};

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

#if RETRO_USING_SDL1
extern byte keyState[SDLK_LAST];

extern SDL_Joystick *controller;
#endif

void ProcessInput();

void CheckKeyPress(InputData *input);
void CheckKeyDown(InputData *input);

#endif // !INPUT_H
