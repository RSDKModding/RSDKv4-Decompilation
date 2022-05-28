#ifndef NATIVE_ACHIEVEMENTSBUTTON_H
#define NATIVE_ACHIEVEMENTSBUTTON_H

struct NativeEntity_AchievementsButton : NativeEntityBase {
    int unused1;
    byte visible;
    int unused2;
    int unused3;
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
