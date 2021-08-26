#ifndef NATIVE_DIALOGPANEL_H
#define NATIVE_DIALOGPANEL_H

struct NativeEntity_DialogPanel : NativeEntityBase {
    int state;
    int buttonCount;
    float stateTimer;
    int field_1C; //??
    float buttonScale;
    MeshInfo *panelMesh;
    MatrixF buttonMatrix;
    MatrixF buttonMult;
    NativeEntity_PushButton *buttons[2];
    int buttonSelected;
    int selection;
    ushort text[128];
    float textX;
    float textY;
    float textScale;
};

void DialogPanel_Create(void *objPtr);
void DialogPanel_Main(void *objPtr);

#endif // !NATIVE_DIALOGPANEL_H
