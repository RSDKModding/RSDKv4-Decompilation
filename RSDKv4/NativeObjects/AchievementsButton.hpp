#ifndef NATIVE_ACHIEVEMENTSBUTTON_H
#define NATIVE_ACHIEVEMENTSBUTTON_H

struct NativeEntity_AchievementsButton : NativeEntityBase {
    int field_10;
    byte visible;
    int field_18;
    int field_1C;
    float x;
    float y;
    float z;
    MeshInfo *meshAchievements;
    float angle;
    float scale;
    byte textureCircle;
    byte r;
    byte g;
    byte b;
    MatrixF renderMatrix;
    MatrixF matrixTemp;
    NativeEntity_TextLabel *labelPtr;
};

void AchievementsButton_Create(void *objPtr);
void AchievementsButton_Main(void *objPtr);

#endif // !NATIVE_ACHIEVEMENTSBUTTON_H
