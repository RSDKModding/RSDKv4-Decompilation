#ifndef NATIVE_PLAYERSELECTSCREEN_H
#define NATIVE_PLAYERSELECTSCREEN_H

enum PlayerSelectScreenStates {
    PLAYERSELECTSCREEN_STATE_ENTER,
    PLAYERSELECTSCREEN_STATE_MAIN,
    PLAYERSELECTSCREEN_STATE_ACTION,
    PLAYERSELECTSCREEN_STATE_IDLE,
    PLAYERSELECTSCREEN_STATE_EXIT
};

struct NativeEntity_PlayerSelectScreen : NativeEntityBase {
    PlayerSelectScreenStates state;
    float timer;
    float timer2;
    int unused1;
    float scale;
    NativeEntity_SaveSelect *saveSel;
    NativeEntity_TextLabel *labelPtr;
    MeshInfo *meshPanel;
    MatrixF matrix1;
    MatrixF matrixTemp;
    byte textureArrows;
    byte texturePlayerSel;
    byte backPressed;
    int alpha;
    ushort textSonic[8];
    ushort textTails[24];
    ushort textKnux[16];
    float sonicX;
    float tailsX;
    float knuxX;
    int playerID;
    byte flag;
    int unused2;
};

void PlayerSelectScreen_Create(void *objPtr);
void PlayerSelectScreen_Main(void *objPtr);

#endif // !NATIVE_PLAYERSELECTSCREEN_H
