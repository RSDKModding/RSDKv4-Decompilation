#ifndef NATIVE_SAVESELECT_H
#define NATIVE_SAVESELECT_H

enum SaveSel_PlayerIDs {
    SAVESEL_SONIC = 1,
    SAVESEL_ST,
    SAVESEL_TAILS,
    SAVESEL_KNUX,
};

struct NativeEntity_SaveSelect : NativeEntityBase {
    int state;
    float field_14;
    int field_18;
    NativeEntity_MenuControl *menuControl;
    void *playerSelect;
    NativeEntity_TextLabel *labelPtr;
    float deleteRotateY;
    float field_2C;
    float field_30;
    MatrixF matrix1;
    NativeEntity_SubMenuButton *saveButtons[5];
    NativeEntity_PushButton *delButton;
    NativeEntity_DialogPanel *dialog;
    byte field_90;
    int selectedSave;
    float rotateY[5];
    float field_AC[5];
    float field_C0[5];
};

void SaveSelect_Create(void *objPtr);
void SaveSelect_Main(void *objPtr);

#endif // !NATIVE_SAVESELECT_H
