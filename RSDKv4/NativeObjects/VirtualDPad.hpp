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
    int field_54;
    int field_58;
    byte dword5C;
    byte dword60;
    int useTouchControls;
    int usePhysicalControls;
    int isVSMode;
    byte field_70;
};

void VirtualDPad_Create(void *objPtr);
void VirtualDPad_Main(void *objPtr);

#endif //!NATIVE_VIRTUALDPAD_H