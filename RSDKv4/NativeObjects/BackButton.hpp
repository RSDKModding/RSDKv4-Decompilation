#ifndef NATIVE_BACKBUTTON_H
#define NATIVE_BACKBUTTON_H

struct NativeEntity_BackButton : NativeEntityBase {
    int field_10;
    byte visible;
    int field_18;
    int field_1C;
    float x;
    float y;
    float z;
    MeshInfo *meshBack;
    float angle;
    float scale;
    byte textureCircle;
    byte r;
    byte g;
    byte b;
    MatrixF renderMatrix;
    MatrixF matrixTemp;
};

void BackButton_Create(void *objPtr);
void BackButton_Main(void *objPtr);

#endif // !NATIVE_BACKBUTTON_H
