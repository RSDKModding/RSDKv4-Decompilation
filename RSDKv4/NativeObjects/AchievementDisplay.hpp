#ifndef NATIVE_ACHIEVEMENTDISPLAY_H
#define NATIVE_ACHIEVEMENTDISPLAY_H

#if !RETRO_USE_ORIGINAL_CODE
enum AchievementDisplayStates { ACHDISP_STATE_LOCKED, ACHDISP_STATE_UNLOCKED };

struct NativeEntity_AchievementDisplay : NativeEntityBase {
    float x;
    float y;
    float z;
    float textWidth;
    float timer;
    float scale;
    int alpha;
    int fontID;
    ushort achievementText[0x40];
    ushort descriptionText[0x80];
    int state;
    byte useRenderMatrix;
    MatrixF renderMatrix;
};

void AchievementDisplay_Create(void *objPtr);
void AchievementDisplay_Main(void *objPtr);
#endif

#endif // !NATIVE_ACHIEVEMENTDISPLAY_H
