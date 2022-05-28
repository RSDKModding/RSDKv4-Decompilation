#ifndef NATIVE_TIMEATTACKBUTTON_H
#define NATIVE_TIMEATTACKBUTTON_H

struct NativeEntity_TimeAttackButton : NativeEntityBase {
    int unused1;
    byte visible;
    int unused2;
    int unused3;
    float x;
    float y;
    float z;
    MeshInfo *meshTimeAttack;
    float angle;
    float scale;
    byte textureCircle;
    byte r;
    byte g;
    byte b;
    MatrixF renderMatrix;
    MatrixF matrixTemp;
    NativeEntity_TextLabel *labelPtr;
    MeshAnimator animator;
};

void TimeAttackButton_Create(void *objPtr);
void TimeAttackButton_Main(void *objPtr);

#endif // !NATIVE_TIMEATTACKBUTTON_H
