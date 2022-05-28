#ifndef NATIVE_PUSHBUTTON_H
#define NATIVE_PUSHBUTTON_H

enum PushButtonStates { PUSHBUTTON_STATE_UNSELECTED, PUSHBUTTON_STATE_SELECTED, PUSHBUTTON_STATE_FLASHING, PUSHBUTTON_STATE_SCALED };

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
    int textColor;
    int textColorSelected;
    int bgColor;
    int bgColorSelected;
    byte symbolsTex;
    ushort text[64];
    byte useRenderMatrix;
    MatrixF renderMatrix;
};

void PushButton_Create(void *objPtr);
void PushButton_Main(void *objPtr);

#endif // !NATIVE_PUSHBUTTON_H
