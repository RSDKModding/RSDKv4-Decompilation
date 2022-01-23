#ifndef NATIVE_MULTIPLAYERSCREEN_H
#define NATIVE_MULTIPLAYERSCREEN_H

#if !RETRO_USE_ORIGINAL_CODE && RETRO_USE_NETWORKING
enum MultiplayerScreenStates {
    MULTIPLAYERSCREEN_STATE_ENTER,
    MULTIPLAYERSCREEN_STATE_MAIN,
    MULTIPLAYERSCREEN_STATE_ACTION,
    MULTIPLAYERSCREEN_STATE_EXIT,
    MULTIPLAYERSCREEN_STATE_FLIP,
    MULTIPLAYERSCREEN_STATE_HOSTSCR,
    MULTIPLAYERSCREEN_STATE_JOINSCR,
    MULTIPLAYERSCREEN_STATE_STARTGAME,
    MULTIPLAYERSCREEN_STATE_DIALOGWAIT,
};
enum MultiplayerScreenDrawStates {
    MULTIPLAYERSCREEN_STATEDRAW_NONE = -1,
    MULTIPLAYERSCREEN_STATEDRAW_MAIN,
    MULTIPLAYERSCREEN_STATEDRAW_HOST,
    MULTIPLAYERSCREEN_STATEDRAW_JOIN
};

enum MultiplayerScreenButton {
    MULTIPLAYERSCREEN_BUTTON_HOST,
    MULTIPLAYERSCREEN_BUTTON_JOIN,
    MULTIPLAYERSCREEN_BUTTON_COPY,
    MULTIPLAYERSCREEN_BUTTON_JOINROOM,
    MULTIPLAYERSCREEN_BUTTON_PASTE,
    MULTIPLAYERSCREEN_BUTTON_COUNT,
};

struct NativeEntity_MultiplayerScreen : NativeEntityBase {
    MultiplayerScreenStates state;
    MultiplayerScreenStates nextState;
    MultiplayerScreenDrawStates stateDraw;
    MultiplayerScreenDrawStates nextStateDraw;
    float timer;
    float scale;
    NativeEntity_MenuBG *bg;
    NativeEntity_TextLabel *label;
    NativeEntity_DialogPanel *dialog;
    MeshInfo *meshPanel;
    MeshAnimator animator;
    MatrixF renderMatrix;
    MatrixF matrixTemp;
    MatrixF matrix3;
    byte textureArrows;
    byte backPressed;
    int arrowAlpha;
    sbyte touchedUpID;
    sbyte touchedDownID;
    NativeEntity_PushButton *buttons[MULTIPLAYERSCREEN_BUTTON_COUNT];
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
void MultiplayerScreen_Destroy(void *objPtr);
#endif

#endif // !NATIVE_MULTIPLAYERSCREEN_H
