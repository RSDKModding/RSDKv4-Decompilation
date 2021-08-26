#ifndef NATIVE_PAUSEMENU_H
#define NATIVE_PAUSEMENU_H

struct NativeEntity_PauseMenu : NativeEntityBase {
    uint state;
    float float14;
    char field_18;
    char field_19;
    char field_1A;
    char field_1B;
    NativeEntity_RetroGameLoop *retroGameLoop;
    int dword20;
    NativeEntity_TextLabel *label;
    float renderRot;
    float float2C;
    float float30;
    MatrixF matrix2;
    MatrixF matrix;
    NativeEntity_SubMenuButton *buttons[4];
    uint buttonID;
    float buttonRot[4];
    float floatD8;
    float floatDC;
    float floatE0;
    float floatE4;
    int dwordE8;
    int dwordEC;
    int dwordF0;
    int dwordF4;
    int dwordF8;
    int dwordFC;
    float dword100;
    float matrixX;
    float float108;
    float matrixY;
    float matrixZ;
    float rotationY;
    float float118;
    byte textureCircle;
    byte textureDPad;
    byte field_11E;
    char field_11F;
    float float120;
    float float124;
    float pfunc128;
    int dword12C;
    byte byte130;
    byte byte131;
};

void PauseMenu_Create(void *objPtr);
void PauseMenu_Main(void *objPtr);

void PauseMenu_Render(void *objPtr);

#endif // NATIVE_PAUSEMENU_H