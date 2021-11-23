#ifndef NATIVE_CWSPLASH_H
#define NATIVE_CWSPLASH_H

enum CWSplashStates { CWSPLASH_STATE_ENTER, CWSPLASH_STATE_EXIT, CWSPLASH_STATE_SPAWNTITLE };

struct NativeEntity_CWSplash : NativeEntityBase {
    CWSplashStates state;
    float rectAlpha;
    byte textureID;
};

void CWSplash_Create(void *objPtr);
void CWSplash_Main(void *objPtr);

#endif // !NATIVE_CWSPLASH_H
