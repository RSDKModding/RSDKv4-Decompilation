#ifndef NATIVE_SETTINGSSCREEN_H
#define NATIVE_SETTINGSSCREEN_H

struct NativeEntity_OptionsMenu;

struct NativeEntity_SettingsScreen : NativeEntityBase {
    int state;
    int subState;
    float timer;
    float buttonRotY;
    float buttonMatScale;
    NativeEntity_OptionsMenu *optionsMenu;
    byte controlStyle;
    NativeEntity_TextLabel *label;
    MeshInfo *panelMesh;
    MatrixF buttonMatrix;
    MatrixF tempMatrix;
    byte arrowsTex;
    byte controllerTex;
    byte touched;
    int alpha;
    ushort sfxText[32];
    ushort musicText[32];
    ushort spindashText[32];
    ushort boxArtText[24];
    NativeEntity_PushButton *buttons[10];
    int selected;
    NativeEntity_VirtualDPad *virtualDPad;
};

void SettingsScreen_Create(void *objPtr);
void SettingsScreen_Main(void *objPtr);

#endif // !NATIVE_SETTINGSSCREEN_H
