#ifndef NATIVE_LEADERBOARDSBUTTON_H
#define NATIVE_LEADERBOARDSBUTTON_H

struct NativeEntity_LeaderboardsButton : NativeEntityBase {
    int field_10;
    byte field_14;
    int field_18;
    int field_1C;
    float translateX;
    float translateY;
    float translateZ;
    MeshInfo *meshLeaderboards;
    float angle;
    float scale;
    byte textureCircle;
    byte r;
    byte g;
    byte b;
    MatrixF renderMatrix;
    MatrixF matrix2;
    NativeEntity_TextLabel *labelPtr;
};

void LeaderboardsButton_Create(void *objPtr);
void LeaderboardsButton_Main(void *objPtr);

#endif // !NATIVE_LEADERBOARDSBUTTON_H
