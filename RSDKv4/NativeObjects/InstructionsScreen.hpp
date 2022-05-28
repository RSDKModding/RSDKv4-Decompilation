#ifndef NATIVE_INSTRUCTIONSSCREEN_H
#define NATIVE_INSTRUCTIONSSCREEN_H

enum InstructionsScreenStates {
    INSTRUCTIONSCREEN_STATE_ENTER,
    INSTRUCTIONSCREEN_STATE_MAIN,
    INSTRUCTIONSCREEN_STATE_FLIP,
    INSTRUCTIONSCREEN_STATE_FINISHFLIP,
    INSTRUCTIONSCREEN_STATE_EXIT
};
enum InstructionsScreenInputStates {
    INSTRUCTIONSCREEN_STATEINPUT_CHECKPRESS,
    INSTRUCTIONSCREEN_STATEINPUT_HANDLEMOVEMENT,
    INSTRUCTIONSCREEN_STATEINPUT_HANDLESCROLL,
    INSTRUCTIONSCREEN_STATEINPUT_HANDLESWIPE
};

struct NativeEntity_InstructionsScreen : NativeEntityBase {
    InstructionsScreenStates state;
    int pageID;
    int shownPage;
    float timer;
    float rotationY;
    float scale;
    NativeEntity_OptionsMenu *optionsMenu;
    NativeEntity_TextLabel *labelPtr;
    MeshInfo *meshPanel;
    MatrixF renderMatrix;
    MatrixF matrixTemp;
    byte textureArrows;
    byte textureHelp;
    byte textureDPad;
    byte touchedBack;
    byte touchedPrev;
    byte touchedNext;
    int arrowAlpha;
    InstructionsScreenInputStates stateInput;
    float textY;
    float textHeight;
    float lastTouchY;
    float textVelocity;
    float lastSwipeDistanceV;
    float swipeDistanceV;
    float lastTouchX;
    byte selectionEnabled;
    byte flipRight;
    ushort pageIDText[8];
};

void InstructionsScreen_Create(void *objPtr);
void InstructionsScreen_Main(void *objPtr);

#endif // !NATIVE_INSTRUCTIONSSCREEN_H
