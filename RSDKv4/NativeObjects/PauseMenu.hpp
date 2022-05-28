#ifndef NATIVE_PAUSEMENU_H
#define NATIVE_PAUSEMENU_H

enum PauseMenuButtons {
    PMB_CONTINUE,
    PMB_RESTART,
    PMB_SETTINGS,
    PMB_EXIT,
#if !RETRO_USE_ORIGINAL_CODE
    PMB_DEVMENU,
#endif
    PMB_COUNT
};

enum PauseMenuStates {
    PAUSEMENU_STATE_SETUP,
    PAUSEMENU_STATE_ENTER,
    PAUSEMENU_STATE_MAIN,
    PAUSEMENU_STATE_CONTINUE,
    PAUSEMENU_STATE_ACTION,
    PAUSEMENU_STATE_ENTERSUBMENU,
    PAUSEMENU_STATE_SUBMENU,
    PAUSEMENU_STATE_EXITSUBMENU,
    PAUSEMENU_STATE_RESTART,
    PAUSEMENU_STATE_EXIT,
#if !RETRO_USE_ORIGINAL_CODE
    PAUSEMENU_STATE_DEVMENU,
#endif
};

struct NativeEntity_PauseMenu : NativeEntityBase {
    PauseMenuStates state;
    float timer;
    float unused1;
    NativeEntity_RetroGameLoop *retroGameLoop;
    NativeEntity_SettingsScreen *settingsScreen;
    NativeEntity_TextLabel *label;
    float renderRot;
    float renderRotMax;
    float rotInc;
    MatrixF matrixTemp;
    MatrixF matrix;
    NativeEntity_SubMenuButton *buttons[PMB_COUNT];
    int buttonSelected;
    float buttonRot[PMB_COUNT];
    float rotMax[PMB_COUNT];
    float buttonRotY[PMB_COUNT];
    NativeEntity_DialogPanel *dialog;
    int unused2;
    float buttonX;
    float matrixX;
    float width;
    float matrixY;
    float matrixZ;
    float rotationY;
    float rotYOff;
    byte textureCircle;
    byte textureDPad;
    float dpadX;
    float dpadXSpecial;
    float dpadY;
    int unusedAlpha;
    byte makeSound;
    byte miniPauseDisabled;
#if !RETRO_USE_ORIGINAL_CODE
    NativeEntity_FadeScreen *devMenuFade;
#endif
};

void PauseMenu_Create(void *objPtr);
void PauseMenu_Main(void *objPtr);

// added by me rmg
void PauseMenu_Render(void *objPtr);
void PauseMenu_CheckTouch(void *objPtr);

#endif // NATIVE_PAUSEMENU_H