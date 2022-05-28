#ifndef NATIVE_ZONEBUTTON_H
#define NATIVE_ZONEBUTTON_H

enum ZoneButtonStates { ZONEBUTTON_STATE_UNSELECTED, ZONEBUTTON_STATE_SELECTED, ZONEBUTTON_STATE_FLASHING };

struct NativeEntity_ZoneButton : NativeEntityBase {
    float x;
    float y;
    float z;
    float texX;
    float texY;
    float angle;
    float angleSpeed;
    byte unlocked;
    float textWidth;
    ZoneButtonStates state;
    float timer;
    float flashTimer;
    uint textColor;
    uint textSelectedColor;
    uint bgColor;
    uint bgColorSelected;
    byte textureIntro;
    byte textureSymbols;
    ushort zoneText[32];
    ushort timeText[32];
    byte useRenderMatrix;
    MatrixF renderMatrix;
};

void ZoneButton_Create(void *objPtr);
void ZoneButton_Main(void *objPtr);

#endif // !NATIVE_ZONEBUTTON_H
