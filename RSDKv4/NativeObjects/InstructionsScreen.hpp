#ifndef NATIVE_INSTRUCTIONSSCREEN_H
#define NATIVE_INSTRUCTIONSSCREEN_H

struct NativeEntity_InstructionsScreen : NativeEntityBase {
    int state;
    int pageID;
    int field_18;
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
    byte touchValid1;
    byte touchValid2;
    byte touchValid3;
    int arrowAlpha;
    int field_C0;
    float textY;
    float floatC8;
    float field_CC;
    float field_D0;
    float field_D4;
    float field_D8;
    float field_DC;
    byte field_E0;
    byte field_E1;
    ushort pageIDText[8];
};

void InstructionsScreen_Create(void *objPtr);
void InstructionsScreen_Main(void *objPtr);

#endif // !NATIVE_INSTRUCTIONSSCREEN_H
