#ifndef NATIVE_MENUBG_H
#define NATIVE_MENUBG_H

struct NativeEntity_MenuBG : NativeEntityBase {
    int isFading;
    float fadeTimer;
    float circle1Rot;
    float circle2Rot;
    float Ztrans1;
    float Ztrans2;
    float ZtransRender;
    MeshInfo *bgCircle1;
    MeshInfo *bgCircle2;
    MeshInfo *bgLines;
    MeshAnimator animator;
    byte textureID;
    byte fadeR;
    byte fadeG;
    byte fadeB;
    int alpha;
    MatrixF renderMatrix;
    MatrixF matrixTemp;
    MatrixF circle1;
    MatrixF circle2;
};

void MenuBG_Create(void *objPtr);
void MenuBG_Main(void *objPtr);

#endif // !NATIVE_NENTITY_H
