#ifndef NATIVE_VIRTUALDPADM_H
#define NATIVE_VIRTUALDPADM_H

struct NativeEntity_VirtualDPadM : NativeEntityBase {
    byte textureID;
    float moveX;
    float moveY;
    float jumpX;
    float jumpY;
    float moveSize;
    float jumpSize;
    int alpha;
    float pauseX;
    float pauseX_S;
    float pauseY;
    int pauseAlpha;
};

void VirtualDPadM_Create(void *objPtr);
void VirtualDPadM_Main(void *objPtr);

#endif //! NATIVE_VIRTUALDPAD_H