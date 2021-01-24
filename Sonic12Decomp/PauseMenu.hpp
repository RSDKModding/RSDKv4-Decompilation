#ifndef NATIVE_PAUSEMENU_H
#define NATIVE_PAUSEMENU_H

struct NativeEntity_PauseMenu : NativeEntityBase {
    byte state;
    bool direction;
    int timer;
    int barPos;
    sbyte selectedOption;

    int barTimer;
    int triTimer;
    int slowTimer;
    int revokeTimer;
    int doodleTimer;
    int pressTimer;

    int switchFrame;
    int switch1, switch2;

    int lastSurfaceNo;
};

void PauseMenu_Create(void *objPtr);
void PauseMenu_Main(void *objPtr);

#endif // NATIVE_PAUSEMENU_H