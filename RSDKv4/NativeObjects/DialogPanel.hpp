#ifndef NATIVE_DIALOGPANEL_H
#define NATIVE_DIALOGPANEL_H

enum DialogPanelTypes {
    DLGTYPE_OK    = 1,
    DLGTYPE_YESNO = 2,
};

enum DialogPanelSelections {
    DLG_YES = 1,
    DLG_NO,
    DLG_OK,
};

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
    float scale;
};

void DialogPanel_Create(void *objPtr);
void DialogPanel_Main(void *objPtr);

#endif // !NATIVE_DIALOGPANEL_H
