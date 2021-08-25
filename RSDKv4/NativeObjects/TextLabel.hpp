#ifndef NATIVE_TEXTLABEL_H
#define NATIVE_TEXTLABEL_H

struct NativeEntity_TextLabel : NativeEntityBase {
    float textX;
    float textY;
    float textZ;
    float textWidth;
    float timer;
    float textScale;
    int textAlpha;
    int fontID;
    ushort text[64];
    int alignment;
    byte byteB4;
    MatrixF renderMatrix;
    void (*alignPtr)(NativeEntity_TextLabel *, int);
};

void TextLabel_Create(void *objPtr);
void TextLabel_Main(void *objPtr);

void TextLabel_Align(NativeEntity_TextLabel *label, int align);

#endif // !NATIVE_TEXTLABEL_H
