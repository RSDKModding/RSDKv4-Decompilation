#ifndef NATIVE_ACHIEVEMENTSMENU_H
#define NATIVE_ACHIEVEMENTSMENU_H

#if !RETRO_USE_ORIGINAL_CODE
struct NativeEntity_AchievementsMenu : NativeEntityBase {
    NativeEntity_TextLabel *label;
    NativeEntity_AchievementDisplay *displays[5];
    MeshInfo *meshPanel;
    MatrixF renderMatrix;
    MatrixF matrixTemp;
    float scale;
    byte textureArrows;
    byte backPressed;
    int arrowAlpha;
    int state;
    float timer;
};

void AchievementsMenu_Create(void *objPtr);
void AchievementsMenu_Main(void *objPtr);
#endif

#endif // !NATIVE_ACHIEVEMENTSMENU_H
