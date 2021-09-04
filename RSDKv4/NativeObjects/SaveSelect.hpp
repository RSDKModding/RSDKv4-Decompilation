#ifndef NATIVE_SAVESELECT_H
#define NATIVE_SAVESELECT_H

enum SaveSelectPlayerIDs {
    SAVESEL_NONE,
    SAVESEL_SONIC,
    SAVESEL_ST,
    SAVESEL_TAILS,
    SAVESEL_KNUX,
};

enum SaveSelectStates {
    SAVESELECT_STATE_SETUP,
    SAVESELECT_STATE_ENTER,
    SAVESELECT_STATE_MAIN,
    SAVESELECT_STATE_EXIT,
    SAVESELECT_STATE_LOADSAVE,
    SAVESELECT_STATE_ENTERSUBMENU,
    SAVESELECT_STATE_SUBMENU,
    SAVESELECT_STATE_EXITSUBMENU,
    SAVESELECT_STATE_MAIN2,
    SAVESELECT_STATE_DELSETUP,
    SAVESELECT_STATE_DIALOGWAIT,
};

enum SaveSelectButtons {
    SAVESELECT_BUTTON_NOSAVE,
    SAVESELECT_BUTTON_SAVE1,
    SAVESELECT_BUTTON_SAVE2,
    SAVESELECT_BUTTON_SAVE3,
    SAVESELECT_BUTTON_SAVE4,
    SAVESELECT_BUTTON_COUNT,
};

struct NativeEntity_SaveSelect : NativeEntityBase {
    SaveSelectStates state;
    float timer;
    int field_18;
    NativeEntity_MenuControl *menuControl;
    void *playerSelect;
    NativeEntity_TextLabel *labelPtr;
    float deleteRotateY;
    float field_2C;
    float field_30;
    MatrixF matrix1;
    NativeEntity_SubMenuButton *saveButtons[SAVESELECT_BUTTON_COUNT];
    NativeEntity_PushButton *delButton;
    NativeEntity_DialogPanel *dialog;
    byte deleteEnabled;
    int selectedButton;
    float rotateY[SAVESELECT_BUTTON_COUNT];
    float field_AC[SAVESELECT_BUTTON_COUNT];
    float field_C0[SAVESELECT_BUTTON_COUNT];
};

void SaveSelect_Create(void *objPtr);
void SaveSelect_Main(void *objPtr);

#endif // !NATIVE_SAVESELECT_H
