#ifndef NATIVE_TIMEATTACKBUTTON_H
#define NATIVE_TIMEATTACKBUTTON_H

struct NativeEntity_TimeAttackButton : NativeEntityBase {
    int field_10;
    byte visible;
    int field_18;
    int field_1C;
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
    MatrixF matrix2;
    NativeEntity_TextLabel *labelPtr;
    MeshAnimator animator;
};

void TimeAttackButton_Create(void *objPtr);
void TimeAttackButton_Main(void *objPtr);

#endif // !NATIVE_TIMEATTACKBUTTON_H
