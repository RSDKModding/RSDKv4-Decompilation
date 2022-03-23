#ifndef NATIVE_FADESCREEN_H
#define NATIVE_FADESCREEN_H

enum FadeScreenStates {
    FADESCREEN_STATE_MENUFADEIN,
    FADESCREEN_STATE_FADEOUT,
    FADESCREEN_STATE_GAMEFADEOUT,
#if !RETRO_USE_ORIGINAL_CODE
    FADESCREEN_STATE_FADEIN
#endif
};

struct NativeEntity_FadeScreen : NativeEntityBase {
    FadeScreenStates state;
    float timer;
    float fadeSpeed;
    float delay;
    byte fadeR;
    byte fadeG;
    byte fadeB;
    int fadeA;
#if !RETRO_USE_ORIGINAL_CODE
    MatrixF render, temp;
#endif
};

void FadeScreen_Create(void *objPtr);
void FadeScreen_Main(void *objPtr);

#endif // !NATIVE_FADESCREEN_H
