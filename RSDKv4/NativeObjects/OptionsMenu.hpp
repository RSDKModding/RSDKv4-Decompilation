#ifndef NATIVE_OPTIONSMENU_H
#define NATIVE_OPTIONSMENU_H

struct NativeEntity_OptionsMenu : NativeEntityBase {
    int state;
    float field_14;
    int field_18;
    NativeEntity_MenuControl *menuControl;
    NativeEntity_InstructionsScreen *instructionsScreen;
    NativeEntity_SettingsScreen *settingsScreen;
    NativeEntity_AboutScreen *aboutScreen;
    NativeEntity_StaffCredits *staffCredits;
    NativeEntity_TextLabel *labelPtr;
    float float34;
    float float38;
    int field_3C;
    MatrixF matrix1;
    NativeEntity_SubMenuButton *buttons[4];
    float field_94[4];
    int selectedButton;
    float field_A4[4];
    float field_B4[4];
};

void OptionsMenu_Create(void *objPtr);
void OptionsMenu_Main(void *objPtr);

#endif // !NATIVE_OPTIONSMENU_H
