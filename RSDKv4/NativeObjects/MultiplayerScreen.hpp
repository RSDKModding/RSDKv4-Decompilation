#ifndef NATIVE_MULTIPLAYERSCREEN_H
#define NATIVE_MULTIPLAYERSCREEN_H

#if !RETRO_USE_ORIGINAL_CODE
struct NativeEntity_MultiplayerScreen : NativeEntityBase {
    int state;
    int nextState;
    int stateDraw;
    int nextStateDraw;
    float timer;
    float scale;
    NativeEntity_MenuBG *bg;
    NativeEntity_TextLabel *label;
    NativeEntity_DialogPanel *dialog;
    MeshInfo *meshPanel;
    MeshAnimator animator;
    MatrixF renderMatrix;
    MatrixF matrix2;
    MatrixF matrix3;
    byte textureArrows;
    byte backPressed;
    int arrowAlpha;
    sbyte touchedUpID;
    sbyte touchedDownID;
    NativeEntity_PushButton *buttons[5];
    int selectedButton;
    byte flipDir;
    float rotationY;
    NativeEntity_TextLabel *codeLabel[3];
    int roomCode;
    NativeEntity_TextLabel *enterCodeLabel[8];
    NativeEntity_TextLabel *enterCodeSlider[2];
};

void MultiplayerScreen_Create(void *objPtr);
void MultiplayerScreen_Main(void *objPtr);
#endif

#endif // !NATIVE_MULTIPLAYERSCREEN_H
