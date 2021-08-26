#ifndef NATIVE_TIMEATTACKBUTTON_H
#define NATIVE_TIMEATTACKBUTTON_H

struct NativeEntity_TimeAttackButton : NativeEntityBase {
    int field_10;
    byte field_14;
    int field_18;
    int field_1C;
    float translateX;
    float translateY;
    float translateZ;
    MeshInfo *meshTimeAttack;
    float angle;
    float scale;
    byte textureCircle;
    byte r;
    byte g;
    byte b;
    MatrixF renderMatrix;
    MatrixF matrix2;
    NativeEntity_TextLabel *labelPtr;
    MeshAnimator animator;
};

void TimeAttackButton_Create(void *objPtr);
void TimeAttackButton_Main(void *objPtr);

#endif // !NATIVE_TIMEATTACKBUTTON_H
