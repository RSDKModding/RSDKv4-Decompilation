#ifndef NATIVE_VIRTUALDPAD_H
#define NATIVE_VIRTUALDPAD_H

struct NativeEntity_VirtualDPad : NativeEntityBase {
    byte textureID;
    float moveX;
    float moveY;
    float float1C;
    float float20;
    float field_24;
    float field_28;
    float jumpX;
    float jumpY;
    float moveSize;
    float jumpSize;
    float unknownSize;
    int alpha;
    float pauseX;
    float pauseX_S;
    float pauseY;
    int pauseAlpha;
    float relativeX;
    float relativeY;
    byte moveFinger;
    byte jumpFinger;
    int useTouchControls;
    int usePhysicalControls;
    int isVSMode;
    byte editMode;
};

void VirtualDPad_Create(void *objPtr);
void VirtualDPad_Main(void *objPtr);

#endif //! NATIVE_VIRTUALDPAD_H