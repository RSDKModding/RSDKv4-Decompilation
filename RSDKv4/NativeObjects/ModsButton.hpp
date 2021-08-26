#ifndef NATIVE_MODSBUTTON_H
#define NATIVE_MODSBUTTON_H

#if RETRO_USE_MOD_LOADER
struct NativeEntity_ModsButton : NativeEntityBase {
    int field_10;
    byte visible;
    int field_18;
    int field_1C;
    float translateX;
    float translateY;
    float translateZ;
    MeshInfo *meshMods;
    float angle;
    float scale;
    byte textureCircle;
    byte r;
    byte g;
    byte b;
    MatrixF renderMatrix;
    MatrixF matrix2;
    NativeEntity_TextLabel *labelPtr;
    byte prevRegion;
};

void ModsButton_Create(void *objPtr);
void ModsButton_Main(void *objPtr);
#endif

#endif // !NATIVE_MODSBUTTON_H
