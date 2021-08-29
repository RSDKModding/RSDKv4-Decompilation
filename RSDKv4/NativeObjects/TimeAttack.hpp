#ifndef NATIVE_TIMEATTACK_H
#define NATIVE_TIMEATTACK_H

struct NativeEntity_TimeAttack : NativeEntityBase {
    int state;
    float timer;
    int field_18;
    NativeEntity_MenuControl *menuControl;
    NativeEntity_TextLabel *labelPtr;
    float float24;
    float float28;
    float float2C;
    NativeEntity_RecordsScreen *recordsScreen;
    NativeEntity_SubMenuButton *button;
    int totalTime;
    float float3C;
    float float40;
    float float44;
    MatrixF matrix2;
    MatrixF matrix1;
    MatrixF matrix3;
    NativeEntity_ZoneButton *zoneButtons[16];
    int zoneID;
    int dword12C;
    float translateY;
    float float134;
    int textureArrows;
    int pagePrevPressed;
    int pageNextPressed;
    int pagePrevAlpha;
    int pageNextAlpha;
    float float144;
    int byte148;
    sbyte pageID;
};

extern int timeAttack_ZoneCount;
extern int timeAttack_ActCount;

void TimeAttack_Create(void *objPtr);
void TimeAttack_Main(void *objPtr);

#endif // !NATIVE_TIMEATTACK_H
