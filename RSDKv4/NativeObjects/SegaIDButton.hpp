#ifndef NATIVE_SEGAIDBUTTON_H
#define NATIVE_SEGAIDBUTTON_H

struct NativeEntity_SegaIDButton : NativeEntityBase {
    float x;
    float y;
    float z;
    float texX;
    int state;
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
