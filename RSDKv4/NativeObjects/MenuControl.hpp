#ifndef NATIVE_MENUCONTROL_H
#define NATIVE_MENUCONTROL_H

enum MenuButtonIDs {
    BUTTON_STARTGAME = 1,
    BUTTON_TIMEATTACK,
    BUTTON_MULTIPLAYER,
    BUTTON_ACHIEVEMENTS,
    BUTTON_LEADERBOARDS,
    BUTTON_OPTIONS,
#if RETRO_USE_MOD_LOADER
    BUTTON_MODS,
#endif
};

struct NativeEntity_MenuControl : NativeEntityBase {
    int state;
    float timer;
    float float18;
    float float1C;
    float float20;
    float float24;
    float float28;
    float float2C;
    float float30;
    int buttonCount;
    NativeEntity_AchievementsButton *buttons[8];
    NativeEntity_BackButton *backButton;
    char buttonFlags[8];
    byte buttonID;
    int stateInput;
    float field_6C;
    float field_70;
    float field_74;
    float field_78;
    float touchX2;
    NativeEntity_SegaIDButton *segaIDButton;
    int field_84;
    NativeEntity_DialogPanel *dialog;
    int dialogTimer;
};

void MenuControl_Create(void *objPtr);
void MenuControl_Main(void *objPtr);

#endif // !NATIVE_NENTITY_H
