#ifndef NATIVE_RECORDSSCREEN_H
#define NATIVE_RECORDSSCREEN_H

struct NativeEntity_RecordsScreen : NativeEntityBase {
    int state;
    int zoneID;
    int actID;
    int actCount;
    int taResultID;
    int rank;
    float field_28;
    float field_2C;
    float rotationY;
    float scale;
    NativeEntityBase *timeAttack;
    NativeEntity_TextLabel *labelPtr;
    MeshInfo *meshPanel;
    MatrixF renderMatrix;
    MatrixF matrix2;
    byte textureArrows;
    byte textureTimeAttack;
    float timeAttackU;
    float timeAttackV;
    int recordOffset;
    float field_D4;
    ushort textRecords[16];
    int field_F8;
    int field_FC;
    int field_100;
    int field_104;
    int field_108;
    int field_10C;
    int field_110;
    int field_114;
    ushort rank1st[16];
    ushort rank2nd[16];
    ushort rank3rd[16];
    NativeEntity_PushButton *buttons[2];
    int selectedButton;
    byte backPressed;
    byte prevActPressed;
    byte nextActPressed;
    byte field_187;
    int buttonAlpha;
    float field_18C;
    byte field_190;
    byte field_191;
};

void RecordsScreen_Create(void *objPtr);
void RecordsScreen_Main(void *objPtr);

#endif // !NATIVE_RECORDSSCREEN_H
