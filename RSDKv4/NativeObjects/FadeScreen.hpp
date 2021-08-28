#ifndef NATIVE_FADESCREEN_H
#define NATIVE_FADESCREEN_H

struct NativeEntity_FadeScreen : NativeEntityBase {
    int state;
    float timer;
    float fadeSpeed;
    float timeLimit;
    byte fadeR;
    byte fadeG;
    byte fadeB;
    int fadeA;
};

void FadeScreen_Create(void *objPtr);
void FadeScreen_Main(void *objPtr);

#endif // !NATIVE_FADESCREEN_H
