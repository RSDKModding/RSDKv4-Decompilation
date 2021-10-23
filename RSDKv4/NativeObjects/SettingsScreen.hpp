#ifndef NATIVE_SETTINGSSCREEN_H
#define NATIVE_SETTINGSSCREEN_H

struct NativeEntity_OptionsMenu;

enum SettingsScreenStates {
    SETTINGSSCREEN_STATE_ENTER,
    SETTINGSSCREEN_STATE_MAIN,
    SETTINGSSCREEN_STATE_ENTERCTRLS,
    SETTINGSSCREEN_STATE_FLIP_CTRLSTOUCH,
    SETTINGSSCREEN_STATE_FINISHFLIP_CTRLSTOUCH,
    SETTINGSSCREEN_STATE_CTRLS_TOUCH,
    SETTINGSSCREEN_STATE_EXIT,
    SETTINGSSCREEN_STATE_FLIP_CTRLS,
    SETTINGSSCREEN_STATE_FINISHFLIP_CTRLS,
    SETTINGSSCREEN_STATE_CTRLS,
};

enum SettingsScreenDrawStates {
    SETTINGSSCREEN_STATEDRAW_MAIN,
    SETTINGSSCREEN_STATEDRAW_CONFIGDPAD,
    SETTINGSSCREEN_STATEDRAW_CONTROLLER,
};

enum SettingsScreenButtons {
    SETTINGSSCREEN_BTN_MUSUP,
    SETTINGSSCREEN_BTN_MUSDOWN,
    SETTINGSSCREEN_BTN_SFXUP,
    SETTINGSSCREEN_BTN_SFXDOWN,
    SETTINGSSCREEN_BTN_SDON,
    SETTINGSSCREEN_BTN_SDOFF,
    SETTINGSSCREEN_BTN_JP,
    SETTINGSSCREEN_BTN_US,
    SETTINGSSCREEN_BTN_EU,
    SETTINGSSCREEN_BTN_CTRLS,
    SETTINGSSCREEN_BTN_COUNT,
};

enum SettingsScreenSelections {
    SETTINGSSCREEN_SEL_NONE,
    SETTINGSSCREEN_SEL_MUSVOL,
    SETTINGSSCREEN_SEL_SFXVOL,
    SETTINGSSCREEN_SEL_SPINDASH,
    SETTINGSSCREEN_SEL_REGION,
    SETTINGSSCREEN_SEL_CONTROLS,
};

enum SettingScreenControlModes {
    CTRLS_PHYSICAL,
    CTRLS_TOUCH,
    CTRLS_MOGA,
    CTRLS_MOGAPRO,
};

struct NativeEntity_SettingsScreen : NativeEntityBase {
    SettingsScreenStates state;
    SettingsScreenDrawStates stateDraw;
    float timer;
    float buttonRotY;
    float buttonMatScale;
    NativeEntity_OptionsMenu *optionsMenu;
    byte isPauseMenu;
    NativeEntity_TextLabel *label;
    MeshInfo *panelMesh;
    MatrixF buttonMatrix;
    MatrixF tempMatrix;
    byte arrowsTex;
    byte controllerTex;
    byte backPressed;
    int alpha;
    ushort sfxText[32];
    ushort musicText[32];
    ushort spindashText[32];
    ushort boxArtText[24];
    NativeEntity_PushButton *buttons[SETTINGSSCREEN_BTN_COUNT];
    int selected;
    NativeEntity_VirtualDPad *virtualDPad;
};

void SettingsScreen_Create(void *objPtr);
void SettingsScreen_Main(void *objPtr);

#endif // !NATIVE_SETTINGSSCREEN_H
