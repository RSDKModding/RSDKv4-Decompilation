#ifndef NATIVE_ZONEBUTTON_H
#define NATIVE_ZONEBUTTON_H

struct NativeEntity_ZoneButton : NativeEntityBase {
    float x;
    float y;
    float z;
    float texX;
    float texY;
    float angle;
    float float28;
    byte flag;
    float textWidth;
    int state;
    float float38;
    float float3C;
    uint colourWhite2;
    uint colourYellow2;
    uint colourWhite;
    uint colourYellow;
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
