#ifndef NATIVE_LEADERBOARDSBUTTON_H
#define NATIVE_LEADERBOARDSBUTTON_H

struct NativeEntity_LeaderboardsButton : NativeEntityBase {
    int unused1;
    byte visible;
    int unused2;
    int unused3;
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
