#include "RetroEngine.hpp"

InputData keyPress = InputData();
InputData keyDown  = InputData();

bool anyPress = false;

int touchDown[8];
int touchX[8];
int touchY[8];
int touchID[8];
int touches = 0;

InputButton inputDevice[9];
int inputType = 0;

int LSTICK_DEADZONE   = 20000;
int RSTICK_DEADZONE   = 20000;
int LTRIGGER_DEADZONE = 20000;
int RTRIGGER_DEADZONE = 20000;

#if RETRO_USING_SDL
SDL_GameController *controller;
#endif

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

#if RETRO_USING_SDL
bool getControllerButton(byte buttonID)
{
    if (SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)buttonID)) {
        return true;
    }
    else {
        switch (buttonID) {
            default: break;
            case SDL_CONTROLLER_BUTTON_DPAD_UP:
                return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) < -LSTICK_DEADZONE;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) > LSTICK_DEADZONE;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) < -LSTICK_DEADZONE;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) > LSTICK_DEADZONE;
        }
    }

    switch (buttonID) {
        default: break;
        case SDL_CONTROLLER_BUTTON_ZL:
            return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > LTRIGGER_DEADZONE;
        case SDL_CONTROLLER_BUTTON_ZR:
            return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > RTRIGGER_DEADZONE;
        case SDL_CONTROLLER_BUTTON_LSTICK_UP:
            return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) < -LSTICK_DEADZONE;
        case SDL_CONTROLLER_BUTTON_LSTICK_DOWN:
            return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) > LSTICK_DEADZONE;
        case SDL_CONTROLLER_BUTTON_LSTICK_LEFT:
            return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) < -LSTICK_DEADZONE;
        case SDL_CONTROLLER_BUTTON_LSTICK_RIGHT:
            return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) > LSTICK_DEADZONE;
        case SDL_CONTROLLER_BUTTON_RSTICK_UP:
            return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY) < -RSTICK_DEADZONE;
        case SDL_CONTROLLER_BUTTON_RSTICK_DOWN:
            return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY) > RSTICK_DEADZONE;
        case SDL_CONTROLLER_BUTTON_RSTICK_LEFT:
            return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) < -RSTICK_DEADZONE;
        case SDL_CONTROLLER_BUTTON_RSTICK_RIGHT:
            return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) > RSTICK_DEADZONE;
    }

    return false;
}
#endif

void ProcessInput()
{
#if RETRO_USING_SDL
    int length           = 0;
    const byte *keyState = SDL_GetKeyboardState(&length);

    if (inputType == 0) {
        for (int i = 0; i < 8; i++) {
            if (keyState[inputDevice[i].keyMappings]) {
                inputDevice[i].setHeld();
                inputDevice[8].setHeld();
                continue;
            }
            else if (inputDevice[i].hold)
                inputDevice[i].setReleased();
        }
    }
    else if (inputType == 1) {
        for (int i = 0; i < 8; i++) {
            if (getControllerButton(inputDevice[i].contMappings)) {
                inputDevice[i].setHeld();
                inputDevice[8].setHeld();
                continue;
            }
            else if (inputDevice[i].hold)
                inputDevice[i].setReleased();
        }
    }

    if (keyState[inputDevice[0].keyMappings] || keyState[inputDevice[1].keyMappings] || keyState[inputDevice[2].keyMappings]
        || keyState[inputDevice[3].keyMappings] || keyState[inputDevice[4].keyMappings] || keyState[inputDevice[5].keyMappings]
        || keyState[inputDevice[6].keyMappings] || keyState[inputDevice[7].keyMappings]) {
        inputType = 0;
    }
    else if (inputType == 0)
        inputDevice[8].setReleased();

    if (getControllerButton(SDL_CONTROLLER_BUTTON_A) || getControllerButton(SDL_CONTROLLER_BUTTON_B) || getControllerButton(SDL_CONTROLLER_BUTTON_X)
        || getControllerButton(SDL_CONTROLLER_BUTTON_Y) || getControllerButton(SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
        || getControllerButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) || getControllerButton(SDL_CONTROLLER_BUTTON_ZL)
        || getControllerButton(SDL_CONTROLLER_BUTTON_ZR) || getControllerButton(SDL_CONTROLLER_BUTTON_DPAD_UP)
        || getControllerButton(SDL_CONTROLLER_BUTTON_DPAD_DOWN) || getControllerButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT)
        || getControllerButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) || getControllerButton(SDL_CONTROLLER_BUTTON_LSTICK_UP)
        || getControllerButton(SDL_CONTROLLER_BUTTON_LSTICK_DOWN) || getControllerButton(SDL_CONTROLLER_BUTTON_LSTICK_LEFT)
        || getControllerButton(SDL_CONTROLLER_BUTTON_LSTICK_RIGHT) || getControllerButton(SDL_CONTROLLER_BUTTON_RSTICK_UP)
        || getControllerButton(SDL_CONTROLLER_BUTTON_RSTICK_DOWN) || getControllerButton(SDL_CONTROLLER_BUTTON_RSTICK_LEFT)
        || getControllerButton(SDL_CONTROLLER_BUTTON_RSTICK_RIGHT) || getControllerButton(SDL_CONTROLLER_BUTTON_START)) {
        inputType = 1;
    }
    else if (inputType == 1)
        inputDevice[8].setReleased();
#endif
}

void CheckKeyPress(InputData *input, byte flags)
{
    if (flags & 0x1)
        input->up = inputDevice[0].press;
    if (flags & 0x2)
        input->down = inputDevice[1].press;
    if (flags & 0x4)
        input->left = inputDevice[2].press;
    if (flags & 0x8)
        input->right = inputDevice[3].press;
    if (flags & 0x10)
        input->A = inputDevice[4].press;
    if (flags & 0x20)
        input->B = inputDevice[5].press;
    if (flags & 0x40)
        input->C = inputDevice[6].press;
    if (flags & 0x80)
        input->start = inputDevice[7].press;
    if (flags & 0x80)
        anyPress = inputDevice[8].press;
}

void CheckKeyDown(InputData *input, byte flags)
{
    if (flags & 0x1)
        input->up = inputDevice[0].hold;
    if (flags & 0x2)
        input->down = inputDevice[1].hold;
    if (flags & 0x4)
        input->left = inputDevice[2].hold;
    if (flags & 0x8)
        input->right = inputDevice[3].hold;
    if (flags & 0x10)
        input->A = inputDevice[4].hold;
    if (flags & 0x20)
        input->B = inputDevice[5].hold;
    if (flags & 0x40)
        input->C = inputDevice[6].hold;
    if (flags & 0x80)
        input->start = inputDevice[7].hold;
    //if (flags & 0x80)
    //   anyHold = inputDevice[8].hold;
}