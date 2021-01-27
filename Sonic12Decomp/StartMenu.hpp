#ifndef NATIVE_STARTMENU_H
#define NATIVE_STARTMENU_H

enum StartMenuStates { STSTATES_INVALID, STSTATES_TITLE, STSTATES_MAIN, STSTATES_SAVE };
struct NativeEntity_StartMenu : NativeEntityBase {
    void (*state)(NativeEntity_StartMenu *);
    bool changingStates;

    bool transitioning = false;
    int transTimer     = 0; // trans rights

    int timers[10]; // LOL i really do not wanna do what i did with pause menu
    int positions[10];

    int offset = 0;

    int selected;
};

void StartMenu_Create(void *objPtr);
void StartMenu_Main(void *objPtr);

#endif // !NATIVE_RETROGAMELOOP_H
