#ifndef NATIVE_STARTGAMEBUTTON_H
#define NATIVE_STARTGAMEBUTTON_H

struct NativeEntity_StartGameButton : NativeEntityBase {
    int unused1;
    byte visible;
    int unused2;
    int unused3;
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
