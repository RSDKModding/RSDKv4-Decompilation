#include "RetroEngine.hpp"

InputData keyPress = InputData();
InputData keyDown  = InputData();

bool anyPress = false;

int touchDown[8];
int touchX[8];
int touchY[8];
int touchID[8];
int touches = 0;

InputButton inputDevice[INPUT_MAX];
int inputType = 0;

int LSTICK_DEADZONE   = 20000;
int RSTICK_DEADZONE   = 20000;
int LTRIGGER_DEADZONE = 20000;
int RTRIGGER_DEADZONE = 20000;

#if RETRO_USING_SDL2
SDL_GameController *controller = nullptr;
#endif

#if RETRO_USING_SDL1
byte keyState[SDLK_LAST];

SDL_Joystick *controller = nullptr;
#endif

#if RETRO_USING_SDL2
bool getControllerButton(byte buttonID)
{
    if (SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)buttonID)) {
        return true;
    }
    else {
        switch (buttonID) {
            default: break;
            case SDL_CONTROLLER_BUTTON_DPAD_UP: return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) < -LSTICK_DEADZONE;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) > LSTICK_DEADZONE;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) < -LSTICK_DEADZONE;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) > LSTICK_DEADZONE;
        }
    }

    switch (buttonID) {
        default: break;
        case SDL_CONTROLLER_BUTTON_ZL: return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > LTRIGGER_DEADZONE;
        case SDL_CONTROLLER_BUTTON_ZR: return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > RTRIGGER_DEADZONE;
        case SDL_CONTROLLER_BUTTON_LSTICK_UP: return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) < -LSTICK_DEADZONE;
        case SDL_CONTROLLER_BUTTON_LSTICK_DOWN: return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) > LSTICK_DEADZONE;
        case SDL_CONTROLLER_BUTTON_LSTICK_LEFT: return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) < -LSTICK_DEADZONE;
        case SDL_CONTROLLER_BUTTON_LSTICK_RIGHT: return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) > LSTICK_DEADZONE;
        case SDL_CONTROLLER_BUTTON_RSTICK_UP: return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY) < -RSTICK_DEADZONE;
        case SDL_CONTROLLER_BUTTON_RSTICK_DOWN: return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY) > RSTICK_DEADZONE;
        case SDL_CONTROLLER_BUTTON_RSTICK_LEFT: return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) < -RSTICK_DEADZONE;
        case SDL_CONTROLLER_BUTTON_RSTICK_RIGHT: return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) > RSTICK_DEADZONE;
    }

    return false;
}
#endif

void ProcessInput()
{
#if RETRO_USING_SDL2
    int length           = 0;
    const byte *keyState = SDL_GetKeyboardState(&length);

    if (inputType == 0) {
        for (int i = 0; i < INPUT_MAX; i++) {
            if (keyState[inputDevice[i].keyMappings]) {
                inputDevice[i].setHeld();
                inputDevice[INPUT_ANY].setHeld();
                continue;
            }
            else if (inputDevice[i].hold)
                inputDevice[i].setReleased();
        }
    }
    else if (inputType == 1) {
        for (int i = 0; i < INPUT_MAX; i++) {
            if (getControllerButton(inputDevice[i].contMappings)) {
                inputDevice[i].setHeld();
                inputDevice[INPUT_ANY].setHeld();
                continue;
            }
            else if (inputDevice[i].hold)
                inputDevice[i].setReleased();
        }
    }

    bool isPressed = false;
    for (int i = 0; i < INPUT_MAX; i++) {
        if (keyState[inputDevice[i].keyMappings]) {
            isPressed = true;
            break;
        }
    }
    if (isPressed)
        inputType = 0;
    else if (inputType == 0)
        inputDevice[INPUT_ANY].setReleased();

    isPressed = false;
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
        if (getControllerButton(i)) {
            isPressed = true;
            break;
        }
    }
    if (isPressed)
        inputType = 1;
    else if (inputType == 1)
        inputDevice[INPUT_ANY].setReleased();
#elif RETRO_USING_SDL1
    if (SDL_NumJoysticks() > 0) {
        controller = SDL_JoystickOpen(0);

        // There's a problem opening the joystick
        if (controller == NULL) {
            // Uh oh
        }
        else {
            inputType = 1;
        }
    }
    else {
        if (controller) {
            // Close the joystick
            SDL_JoystickClose(controller);
        }
        controller = nullptr;
        inputType  = 0;
    }

    if (inputType == 0) {
        for (int i = 0; i < INPUT_MAX; i++) {
            if (keyState[inputDevice[i].keyMappings]) {
                inputDevice[i].setHeld();
                inputDevice[INPUT_ANY].setHeld();
                continue;
            }
            else if (inputDevice[i].hold)
                inputDevice[i].setReleased();
        }
    }
    else if (inputType == 1 && controller) {
        for (int i = 0; i < INPUT_MAX; i++) {
            if (SDL_JoystickGetButton(controller, inputDevice[i].contMappings)) {
                inputDevice[i].setHeld();
                inputDevice[INPUT_ANY].setHeld();
                continue;
            }
            else if (inputDevice[i].hold)
                inputDevice[i].setReleased();
        }
    }

    bool isPressed = false;
    for (int i = 0; i < INPUT_MAX; i++) {
        if (keyState[inputDevice[i].keyMappings]) {
            isPressed = true;
            break;
        }
    }
    if (isPressed)
        inputType = 0;
    else if (inputType == 0)
        inputDevice[INPUT_ANY].setReleased();

    int buttonCnt = 0;
    if (controller)
        buttonCnt = SDL_JoystickNumButtons(controller);
    bool flag = false;
    for (int i = 0; i < buttonCnt; ++i) {
        flag      = true;
        inputType = 1;
    }
    if (!flag && inputType == 1) {
        inputDevice[INPUT_ANY].setReleased();
    }
#endif
}

void CheckKeyPress(InputData *input)
{
    input->up     = inputDevice[INPUT_UP].press;
    input->down   = inputDevice[INPUT_DOWN].press;
    input->left   = inputDevice[INPUT_LEFT].press;
    input->right  = inputDevice[INPUT_RIGHT].press;
    input->A      = inputDevice[INPUT_BUTTONA].press;
    input->B      = inputDevice[INPUT_BUTTONB].press;
    input->C      = inputDevice[INPUT_BUTTONC].press;
    input->X      = inputDevice[INPUT_BUTTONX].press;
    input->Y      = inputDevice[INPUT_BUTTONY].press;
    input->Z      = inputDevice[INPUT_BUTTONZ].press;
    input->L      = inputDevice[INPUT_BUTTONL].press;
    input->R      = inputDevice[INPUT_BUTTONR].press;
    input->start  = inputDevice[INPUT_START].press;
    input->select = inputDevice[INPUT_SELECT].press;
    anyPress      = inputDevice[INPUT_ANY].press;
}

void CheckKeyDown(InputData *input)
{
    input->up     = inputDevice[INPUT_UP].hold;
    input->down   = inputDevice[INPUT_DOWN].hold;
    input->left   = inputDevice[INPUT_LEFT].hold;
    input->right  = inputDevice[INPUT_RIGHT].hold;
    input->A      = inputDevice[INPUT_BUTTONA].hold;
    input->B      = inputDevice[INPUT_BUTTONB].hold;
    input->C      = inputDevice[INPUT_BUTTONC].hold;
    input->X      = inputDevice[INPUT_BUTTONX].hold;
    input->Y      = inputDevice[INPUT_BUTTONY].hold;
    input->Z      = inputDevice[INPUT_BUTTONZ].hold;
    input->L      = inputDevice[INPUT_BUTTONL].hold;
    input->R      = inputDevice[INPUT_BUTTONR].hold;
    input->start  = inputDevice[INPUT_START].hold;
    input->select = inputDevice[INPUT_SELECT].hold;
    // if (flags & 0x80)
    //   anyHold = inputDevice[8].hold;
}