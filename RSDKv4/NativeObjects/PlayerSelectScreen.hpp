#ifndef NATIVE_PLAYERSELECTSCREEN_H
#define NATIVE_PLAYERSELECTSCREEN_H

struct NativeEntity_PlayerSelectScreen : NativeEntityBase {
    int state;
    float field_14;
    float field_18;
    int field_1C;
    float scale;
    NativeEntity_SaveSelect *saveSel;
    NativeEntity_TextLabel *labelPtr;
    MeshInfo *meshPanel;
    MatrixF matrix1;
    MatrixF matrix2;
    byte textureArrows;
    byte texturePlayerSel;
    byte touchValid;
    int alpha;
    ushort textSonic[8];
    ushort textTails[24];
    ushort textKnux[16];
    float sonicX;
    float tailsX;
    float knuxX;
    int playerID;
    byte flag;
    int field_129;
};

void PlayerSelectScreen_Create(void *objPtr);
void PlayerSelectScreen_Main(void *objPtr);

#endif // !NATIVE_PLAYERSELECTSCREEN_H
