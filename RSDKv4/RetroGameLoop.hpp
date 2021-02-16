#ifndef NATIVE_RETROGAMELOOP_H
#define NATIVE_RETROGAMELOOP_H

struct NativeEntity_RetroGameLoop : NativeEntityBase {
	//Nothin lol
#if !RETRO_USE_ORIGINAL_CODE
    NativeEntity_PauseMenu *pauseMenu;
#endif
};

void RetroGameLoop_Create(void *objPtr);
void RetroGameLoop_Main(void *objPtr);

#endif // !NATIVE_RETROGAMELOOP_H
