#ifndef NATIVE_VSBUTTON_H
#define NATIVE_VSBUTTON_H

struct NativeEntity_VSButton : NativeEntityBase {
    int field_10;
    byte field_14;
    int field_18;
    int field_1C;
    float translateX;
    float translateY;
    float translateZ;
    MeshInfo *meshVS;
    float angle;
    float scale;
    byte textureCircle;
    byte r;
    byte g;
    byte b;
    MatrixF renderMatrix;
    MatrixF matrix2;
    NativeEntity_TextLabel *labelPtr;
};

void VSButton_Create(void *objPtr);
void VSButton_Main(void *objPtr);

#endif // !NATIVE_VSBUTTON_H
