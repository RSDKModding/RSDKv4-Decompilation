#ifndef NATIVE_CREDITTEXT_H
#define NATIVE_CREDITTEXT_H

struct NativeEntity_CreditText : NativeEntityBase {
    float textX;
    float textY;
    float textZ;
    float width;
    float scaleX;
    int textAlpha;
    int fontID;
    ushort text[64];
    int state;
    byte useRenderMatrix;
    MatrixF renderMatrix;
    uint colour;
    byte textureID;
};

void CreditText_Create(void *objPtr);
void CreditText_Main(void *objPtr);

#endif // !NATIVE_CREDITTEXT_H
