#ifndef NATIVE_PUSHBUTTON_H
#define NATIVE_PUSHBUTTON_H

struct NativeEntity_PushButton : NativeEntityBase {
    float x;
    float y;
    float z;
    int state;
    float textWidth;
    float xOff;
    float yOff;
    float stateTimer;
    float flashTimer;
    float scale;
    float textScale;
    int alpha;
    int white;
    int cyan;
    int blue;
    int blue2;
    byte symbolsTex;
    ushort text[64];
    byte useRenderMatrix;
    MatrixF renderMatrix;
};

void PushButton_Create(void *objPtr);
void PushButton_Main(void *objPtr);

#endif // !NATIVE_PUSHBUTTON_H
