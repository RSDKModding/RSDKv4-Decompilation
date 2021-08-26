#ifndef NATIVE_BACKBUTTON_H
#define NATIVE_BACKBUTTON_H

struct NativeEntity_BackButton : NativeEntityBase {
    int field_10;
    byte field_14;
    int field_18;
    int field_1C;
    float translateX;
    float translateY;
    float translateZ;
    MeshInfo *meshBack;
    float angle;
    float scale;
    byte textureCircle;
    byte r;
    byte g;
    byte b;
    MatrixF renderMatrix;
    MatrixF matrix2;
};

void BackButton_Create(void *objPtr);
void BackButton_Main(void *objPtr);

#endif // !NATIVE_BACKBUTTON_H
