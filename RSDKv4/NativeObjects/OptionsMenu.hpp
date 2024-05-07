#ifndef NATIVE_OPTIONSMENU_H
#define NATIVE_OPTIONSMENU_H

enum OptionsMenuStates {
    OPTIONSMENU_STATE_SETUP,
    OPTIONSMENU_STATE_ENTER,
    OPTIONSMENU_STATE_MAIN,
    OPTIONSMENU_STATE_EXIT,
    OPTIONSMENU_STATE_ACTION,
    OPTIONSMENU_STATE_ENTERSUBMENU,
    OPTIONSMENU_STATE_SUBMENU,
    OPTIONSMENU_STATE_EXITSUBMENU
};

enum OptionsMenuButtons {
    OPTIONSMENU_BUTTON_INSTRUCTIONS,
    OPTIONSMENU_BUTTON_SETTINGS,
    OPTIONSMENU_BUTTON_ABOUT,
    OPTIONSMENU_BUTTON_CREDITS,
    OPTIONSMENU_BUTTON_COUNT,
};

struct NativeEntity_OptionsMenu : NativeEntityBase {
    OptionsMenuStates state;
    float timer;
    int unused1; // Used for checking if the dev menu should be entered in the decomp
    NativeEntity_MenuControl *menuControl;
    NativeEntity_InstructionsScreen *instructionsScreen;
    NativeEntity_SettingsScreen *settingsScreen;
    NativeEntity_AboutScreen *aboutScreen;
    NativeEntity_StaffCredits *staffCredits;
    NativeEntity_TextLabel *labelPtr;
    float labelRotateY;
    float targetLabelRotateY;
    float labelRotateYVelocity;
    MatrixF matrix1;
    NativeEntity_SubMenuButton *buttons[OPTIONSMENU_BUTTON_COUNT];
    float buttonRotateY[OPTIONSMENU_BUTTON_COUNT];
    int selectedButton;
    float targetButtonRotateY[OPTIONSMENU_BUTTON_COUNT];
    float buttonRotateYVelocity[OPTIONSMENU_BUTTON_COUNT];
};

void OptionsMenu_Create(void *objPtr);
void OptionsMenu_Main(void *objPtr);

#endif // !NATIVE_OPTIONSMENU_H
