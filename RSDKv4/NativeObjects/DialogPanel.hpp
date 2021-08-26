#ifndef NATIVE_DIALOGPANEL_H
#define NATIVE_DIALOGPANEL_H

struct NativeEntity_DialogPanel : NativeEntityBase {
    int state;
    int buttonCount;
    float stateTimer;
    byte gap1C[4]; //??
    float buttonScale;
    MeshInfo *panelMesh;
    MatrixF buttonMatrix;
    Matrix buttonMult;
    NativeEntity_PushButton *buttons[2];
    int buttonPressed;
    int dwordB4;
    ushort text[128];
    float textX;
    float textY;
    float textScale;
};

void DialogPanel_Create(void *objPtr);
void DialogPanel_Main(void *objPtr);

void DialogPanel_CheckTouch(void *objPtr);

#endif // !NATIVE_DIALOGPANEL_H
