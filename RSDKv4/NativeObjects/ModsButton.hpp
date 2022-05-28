#ifndef NATIVE_MODSBUTTON_H
#define NATIVE_MODSBUTTON_H

#if RETRO_USE_MOD_LOADER
struct NativeEntity_ModsButton : NativeEntityBase {
    int unused1;
    byte visible;
    int unused2;
    int unused3;
    float x;
    float y;
    float z;
    MeshInfo *meshMods;
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

void ModsButton_Create(void *objPtr);
void ModsButton_Main(void *objPtr);
#endif

#endif // !NATIVE_MODSBUTTON_H
