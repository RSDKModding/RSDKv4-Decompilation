#ifndef NATIVE_SEGASPLASH_H
#define NATIVE_SEGASPLASH_H

enum SegaSplashStates { SEGAPLASH_STATE_ENTER, SEGAPLASH_STATE_EXIT, SEGAPLASH_STATE_SPAWNCWSPLASH };

struct NativeEntity_SegaSplash : NativeEntityBase {
    SegaSplashStates state;
    float rectAlpha;
    byte textureID;
};

void SegaSplash_Create(void *objPtr);
void SegaSplash_Main(void *objPtr);

#endif // !NATIVE_SEGASPLASH_H
