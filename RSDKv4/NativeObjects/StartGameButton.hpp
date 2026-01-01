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
#if RETRO_USE_V6
void loadCartridgeValue(void *objPtr);
//took 2 parameters, but one is used as a handle for the mesh
//while the other is the region value, so we can just use the region directly
#endif
void StartGameButton_Create(void *objPtr);
void StartGameButton_Main(void *objPtr);

#endif // !NATIVE_STARTGAMEBUTTON_H
