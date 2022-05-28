#ifndef NATIVE_CREDITTEXT_H
#define NATIVE_CREDITTEXT_H

enum CreditTextStates { CREDITTEXT_STATE_DISPLAY, CREDITTEXT_STATE_SETUP = 3, CREDITTEXT_STATE_IMAGE };
enum CreditTextTypes { CREDITS_TYPE_TEXT1, CREDITS_TYPE_TEXT2, CREDITS_TYPE_TEXT3, CREDITS_TYPE_LOGO };

struct NativeEntity_CreditText : NativeEntityBase {
    float textX;
    float textY;
    float textZ;
    float alignOffset;
    float scale;
    int alpha;
    int fontID;
    ushort text[64];
    CreditTextStates state;
    byte useRenderMatrix;
    MatrixF renderMatrix;
    uint color;
    byte textureID;
};

void CreditText_Create(void *objPtr);
void CreditText_Main(void *objPtr);

#endif // !NATIVE_CREDITTEXT_H
