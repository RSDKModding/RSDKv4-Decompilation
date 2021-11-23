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
    float field_1C;
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
    float field_CC;
    float field_D0;
    float field_D4;
    float field_D8;
    float lastTouchX;
    byte field_E0;
    byte flipRight;
    ushort pageIDText[8];
};

void InstructionsScreen_Create(void *objPtr);
void InstructionsScreen_Main(void *objPtr);

#endif // !NATIVE_INSTRUCTIONSSCREEN_H
