#ifndef NATIVE_ABOUTSCREEN_H
#define NATIVE_ABOUTSCREEN_H

enum AboutMenuButtons { ABOUT_BTN_PRIVACY, ABOUT_BTN_TERMS, ABOUT_BTN_COUNT };
enum AboutMenuStates { ABOUT_STATE_ENTER, ABOUT_STATE_MAIN, ABOUT_STATE_ACTION, ABOUT_STATE_EXIT };

struct NativeEntity_AboutScreen : NativeEntityBase {
    int state;
    int unused;
    float timer;
    float rotationY;
    float scale;
    NativeEntity_OptionsMenu *optionsMenu;
    NativeEntity_TextLabel *label;
    MeshInfo *meshPanel;
    MeshInfo *meshBox;
    MeshAnimator animator;
    MatrixF renderMatrix;
    MatrixF matrixTemp;
    MatrixF renderMatrix2;
    byte textureArrows;
    byte backPressed;
    int arrowAlpha;
    ushort gameTitle[16];
    ushort versionNameText[16];
    ushort versionText[16];
    NativeEntity_PushButton *buttons[ABOUT_BTN_COUNT];
    int selectedButton;
};

void AboutScreen_Create(void *objPtr);
void AboutScreen_Main(void *objPtr);

#endif // !NATIVE_ABOUTSCREEN_H
