#ifndef NATIVE_VSBUTTON_H
#define NATIVE_VSBUTTON_H

struct NativeEntity_MultiplayerButton : NativeEntityBase {
    int field_10;
    byte visible;
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

void MultiplayerButton_Create(void *objPtr);
void MultiplayerButton_Main(void *objPtr);

#endif // !NATIVE_VSBUTTON_H
