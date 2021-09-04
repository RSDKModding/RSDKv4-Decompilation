#ifndef NATIVE_LEADERBOARDSBUTTON_H
#define NATIVE_LEADERBOARDSBUTTON_H

struct NativeEntity_LeaderboardsButton : NativeEntityBase {
    int field_10;
    byte visible;
    int field_18;
    int field_1C;
    float x;
    float y;
    float z;
    MeshInfo *meshLeaderboards;
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

void LeaderboardsButton_Create(void *objPtr);
void LeaderboardsButton_Main(void *objPtr);

#endif // !NATIVE_LEADERBOARDSBUTTON_H
