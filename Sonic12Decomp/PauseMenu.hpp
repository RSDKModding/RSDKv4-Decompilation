#ifndef NATIVE_PAUSEMENU_H
#define NATIVE_PAUSEMENU_H

struct NativeEntity_PauseMenu : NativeEntityBase {
    byte state;
    int timer;
    int barPos;
    byte selectedOption;
    TextMenu *menu;

    int lastSurfaceNo;
};

void PauseMenu_Create(void *objPtr);
void PauseMenu_Main(void *objPtr);

#endif // NATIVE_PAUSEMENU_H