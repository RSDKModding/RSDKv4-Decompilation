#ifndef NATIVE_SEGAIDBUTTON_H
#define NATIVE_SEGAIDBUTTON_H

enum SegaIDButtonStates { SEGAIDBUTTON_STATE_IDLE, SEGAIDBUTTON_STATE_PRESSED };

struct NativeEntity_SegaIDButton : NativeEntityBase {
    float x;
    float y;
    float z;
    float texX;
    SegaIDButtonStates state;
    int alpha;
    int field_28;
    int field_2C;
    byte textureID;
    byte useRenderMatrix;
    MatrixF renderMatrix;
};

void SegaIDButton_Create(void *objPtr);
void SegaIDButton_Main(void *objPtr);

#endif // !NATIVE_SEGAIDBUTTON_H
