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

struct NativeEntity_PauseMenu : NativeEntityBase {
    int state;
    float timer;
    float float18;
    NativeEntity_RetroGameLoop *retroGameLoop;
    NativeEntity_SettingsScreen *settingsScreen;
    NativeEntity_TextLabel *label;
    float renderRot;
    float renderRotMax;
    float rotInc;
    MatrixF matrix2;
    MatrixF matrix;
    NativeEntity_SubMenuButton *buttons[PMB_COUNT];
    int buttonSelected;
    float buttonRot[PMB_COUNT];
    float rotMax[PMB_COUNT];
    float buttonRotY[PMB_COUNT];
    NativeEntity_DialogPanel *dialog;
    int dwordFC;
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