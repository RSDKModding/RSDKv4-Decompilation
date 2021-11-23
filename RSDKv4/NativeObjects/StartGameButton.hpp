#ifndef NATIVE_STARTGAMEBUTTON_H
#define NATIVE_STARTGAMEBUTTON_H

struct NativeEntity_StartGameButton : NativeEntityBase {
    int field_10;
    byte visible;
    int field_18;
    int field_1C;
    float x;
    float y;
    float z;
    MeshInfo *meshCart;
    float angle;
    float scale;
    byte textureCircle;
    byte r;
    byte g;
    byte b;
    MatrixF renderMatrix;
    MatrixF matrixTemp;
    NativeEntity_TextLabel *labelPtr;
    byte prevRegion;
};

void StartGameButton_Create(void *objPtr);
void StartGameButton_Main(void *objPtr);

#endif // !NATIVE_STARTGAMEBUTTON_H
