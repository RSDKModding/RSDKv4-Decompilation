#ifndef NATIVE_TEXTLABEL_H
#define NATIVE_TEXTLABEL_H

struct NativeEntity_TextLabel : NativeEntityBase {
    float x;
    float y;
    float z;
    float textWidth;
    float timer;
    float scale;
    int alpha;
    int fontID;
    ushort text[64];
    int state;
    byte useRenderMatrix;
    MatrixF renderMatrix;
    void (*alignPtr)(NativeEntity_TextLabel *, int);

    byte useColours;
    byte r;
    byte g;
    byte b;
};

void TextLabel_Create(void *objPtr);
void TextLabel_Main(void *objPtr);

void TextLabel_Align(NativeEntity_TextLabel *label, int align);

#endif // !NATIVE_TEXTLABEL_H
