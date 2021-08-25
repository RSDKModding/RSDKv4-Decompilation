#ifndef NATIVE_TITLESCREEN_H
#define NATIVE_TITLESCREEN_H

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
    float translateX;
    float field_4C;
    float field_50;
    float rotationZ;
    float matrixY;
    float matrixZ;
    MatrixF matrix1;
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
