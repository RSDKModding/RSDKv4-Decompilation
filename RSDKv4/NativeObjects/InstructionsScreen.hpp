#ifndef NATIVE_INSTRUCTIONSSCREEN_H
#define NATIVE_INSTRUCTIONSSCREEN_H

struct NativeEntity_InstructionsScreen : NativeEntityBase {
    int state;
    int pageID;
    int shownPage;
    float field_1C;
    float rotationY;
    float scale;
    NativeEntity_OptionsMenu *optionsMenu;
    NativeEntity_TextLabel *labelPtr;
    MeshInfo *meshPanel;
    MatrixF renderMatrix;
    MatrixF matrix2;
    byte textureArrows;
    byte textureHelp;
    byte textureDPad;
    byte touchedBack;
    byte touchedPrev;
    byte touchedNext;
    int arrowAlpha;
    int stateInput;
    float textY;
    float textHeight;
    float field_CC;
    float field_D0;
    float field_D4;
    float field_D8;
    float lastTouchX;
    byte field_E0;
    byte field_E1;
    ushort pageIDText[8];
};

void InstructionsScreen_Create(void *objPtr);
void InstructionsScreen_Main(void *objPtr);

#endif // !NATIVE_INSTRUCTIONSSCREEN_H
