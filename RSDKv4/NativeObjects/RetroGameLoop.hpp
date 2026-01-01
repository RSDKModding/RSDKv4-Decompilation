#ifndef NATIVE_RETROGAMELOOP_H
#define NATIVE_RETROGAMELOOP_H

struct NativeEntity_RetroGameLoop : NativeEntityBase {
    // Nothin lol
};

void RetroGameLoop_Create(void *objPtr);
void RetroGameLoop_Main(void *objPtr);

#if RETRO_USE_V6
extern int tempGlobalVar; //i couldn't think of a better name, and it changes between global variables too
extern const char *checkpointName; // this is fine ig

void eventPauseMenuVisible(bool paused, int state); // not sure about the name "state", best i could come up with based on the output
void showPauseScreenJava(); //as much as im gonna get without decompiling the classes files in the apk
#endif

#endif // !NATIVE_RETROGAMELOOP_H
