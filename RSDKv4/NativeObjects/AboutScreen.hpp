#ifndef NATIVE_ABOUTSCREEN_H
#define NATIVE_ABOUTSCREEN_H

struct NativeEntity_AboutScreen : NativeEntityBase {
    int state;
    int field_14;
    float field_18;
    float rotationY;
    float scale;
    NativeEntity_OptionsMenu *optionsMenu;
    NativeEntity_TextLabel *label;
    MeshInfo *meshPanel;
    MeshInfo *meshBox;
    MeshAnimator animator;
    MatrixF renderMatrix;
    MatrixF matrix2;
    MatrixF matrix3;
    byte textureArrows;
    byte touchValid;
    int arrowAlpha;
    ushort gameTitle[16];
    ushort versionNameText[16];
    ushort versionText[16];
    NativeEntity_PushButton *buttons[2];
    int selectedButton;
};

void AboutScreen_Create(void *objPtr);
void AboutScreen_Main(void *objPtr);

#endif // !NATIVE_ABOUTSCREEN_H
