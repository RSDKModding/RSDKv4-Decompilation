#ifndef NATIVE_VIRTUALDPAD_H
#define NATIVE_VIRTUALDPAD_H

struct NativeEntity_VirtualDPad : NativeEntityBase {
    byte textureID;
    float moveX;
    float moveY;
    float pivotX;
    float pivotY;
    float offsetX;
    float offsetY;
    float jumpX;
    float jumpY;
    float moveSize;
    float jumpSize;
    float pressedSize;
    int alpha;
    float pauseX;
    float pauseX_S;
    float pauseY;
    int pauseAlpha;
    float relativeX;
    float relativeY;
    sbyte moveFinger;
    sbyte jumpFinger;
    int useTouchControls;
    int usePhysicalControls;
    int vsMode;
    byte editMode;
};

void VirtualDPad_Create(void *objPtr);
void VirtualDPad_Main(void *objPtr);

#endif //! NATIVE_VIRTUALDPAD_H