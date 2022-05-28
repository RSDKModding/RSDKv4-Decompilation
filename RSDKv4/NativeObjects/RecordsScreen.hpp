#ifndef NATIVE_RECORDSSCREEN_H
#define NATIVE_RECORDSSCREEN_H

enum RecordsScreenStates {
    RECORDSSCREEN_STATE_SETUP = -1,
    RECORDSSCREEN_STATE_ENTER,
    RECORDSSCREEN_STATE_MAIN,
    RECORDSSCREEN_STATE_FLIP,
    RECORDSSCREEN_STATE_FINISHFLIP,
    RECORDSSCREEN_STATE_EXIT,
    RECORDSSCREEN_STATE_LOADSTAGE,
    RECORDSSCREEN_STATE_SHOWRESULTS,
    RECORDSSCREEN_STATE_EXITRESULTS
};

enum RecordsScreenButtons { RECORDSSCREEN_BUTTON_PLAY, RECORDSSCREEN_BUTTON_NEXTACT };

struct NativeEntity_RecordsScreen : NativeEntityBase {
    RecordsScreenStates state;
    int zoneID;
    int actID;
    int actCount;
    int taResultID;
    int rank;
    float flashTimer;
    float timer;
    float rotationY;
    float scale;
    NativeEntityBase *timeAttack;
    NativeEntity_TextLabel *labelPtr;
    MeshInfo *meshPanel;
    MatrixF renderMatrix;
    MatrixF matrixTemp;
    byte textureArrows;
    byte textureTimeAttack;
    float timeAttackU;
    float timeAttackV;
    int recordOffset;
    float recordTextWidth;
    ushort textRecords[16];
    int unused1;
    int unused2;
    int unused3;
    int unused4;
    int unused5;
    int unused6;
    int unused7;
    int unused8;
    ushort rank1st[16];
    ushort rank2nd[16];
    ushort rank3rd[16];
    NativeEntity_PushButton *buttons[2];
    int selectedButton;
    byte backPressed;
    byte prevActPressed;
    byte nextActPressed;
    byte unused12;
    int buttonAlpha;
    float lastTouchX;
    byte selectionEnabled;
    byte flipRight;
};

void RecordsScreen_Create(void *objPtr);
void RecordsScreen_Main(void *objPtr);

#endif // !NATIVE_RECORDSSCREEN_H
