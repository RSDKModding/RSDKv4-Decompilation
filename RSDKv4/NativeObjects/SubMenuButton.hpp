#ifndef NATIVE_SUBMENUBUTTON_H
#define NATIVE_SUBMENUBUTTON_H

struct NativeEntity_SubMenuButton : NativeEntityBase {
    float float10;
    float float14;
    float float18;
    int state;
    float float20;
    float float24;
    float float28;
    float float2C;
    float float30;
    int dword34;
    byte r;
    byte g;
    byte b;
    ushort text[64];
    MeshInfo *meshButton;
    MeshInfo *meshButtonH;
    byte setNewState;
    MatrixF matrix;
    MatrixF renderMatrix;
    byte byte148;
    byte flags;
    byte textureSymbols;
    byte useMeshH;
};

void SubMenuButton_Create(void *objPtr);
void SubMenuButton_Main(void *objPtr);

#endif // !NATIVE_SUBMENUBUTTON_H
