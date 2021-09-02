#ifndef NATIVE_TITLESCREEN_H
#define NATIVE_TITLESCREEN_H

enum FontIDs { FONT_HEADING, FONT_LABEL, FONT_TEXT };
enum RegionIDs { REGION_JP, REGION_US, REGION_EU };

struct NativeEntity_TitleScreen : NativeEntityBase {
    int state;
    float introRectAlpha;
    NativeEntity_TextLabel *labelPtr;
    MeshInfo *introMesh;
    MeshInfo *boxMesh;
    MeshInfo *cartMesh;
    MeshAnimator meshAnimator;
    float field_38;
    float field_3C;
    float meshScale;
    float rotationY;
    float x;
    float field_4C;
    float field_50;
    float rotationZ;
    float matrixY;
    float matrixZ;
    MatrixF renderMatrix;
    MatrixF matrix2;
    MatrixF matrix3;
    byte logoTextureID;
    byte introTextureID;
    int logoAlpha;
    int introAlpha;
    int field_12C;
    byte field_130;
};

void TitleScreen_Create(void *objPtr);
void TitleScreen_Main(void *objPtr);

#endif // !NATIVE_TITLESCREEN_H
