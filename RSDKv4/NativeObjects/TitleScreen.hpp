#ifndef NATIVE_TITLESCREEN_H
#define NATIVE_TITLESCREEN_H

enum FontIDs { FONT_HEADING, FONT_LABEL, FONT_TEXT };
enum RegionIDs { REGION_JP, REGION_US, REGION_EU };

enum TitleScreenStates {
    TITLESCREEN_STATE_SETUP,
    TITLESCREEN_STATE_ENTERINTRO,
    TITLESCREEN_STATE_INTRO,
    TITLESCREEN_STATE_ENTERBOX,
    TITLESCREEN_STATE_TITLE,
    TITLESCREEN_STATE_EXITTITLE,
    TITLESCREEN_STATE_EXIT
};

struct NativeEntity_TitleScreen : NativeEntityBase {
    TitleScreenStates state;
    float introRectAlpha;
    NativeEntity_TextLabel *labelPtr;
    MeshInfo *introMesh;
    MeshInfo *boxMesh;
    MeshInfo *cartMesh;
    MeshAnimator meshAnimator;
    float rectY;
    float rectYVelocity;
    float meshScale;
    float rotationY;
    float x;
    float y;
    float yVelocity;
    float rotationZ;
    float matrixY;
    float matrixZ;
    MatrixF renderMatrix;
    MatrixF renderMatrix2;
    MatrixF matrixTemp;
    byte logoTextureID;
    byte introTextureID;
    int logoAlpha;
    int skipButtonAlpha;
    int fadeRectAlpha;
    byte selectionDisabled;
};

void TitleScreen_Create(void *objPtr);
void TitleScreen_Main(void *objPtr);

#endif // !NATIVE_TITLESCREEN_H
