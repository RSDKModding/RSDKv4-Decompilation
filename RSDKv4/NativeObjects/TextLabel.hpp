#ifndef NATIVE_TEXTLABEL_H
#define NATIVE_TEXTLABEL_H

enum TextLabelStates { TEXTLABEL_STATE_NONE = -1, TEXTLABEL_STATE_IDLE, TEXTLABEL_STATE_BLINK, TEXTLABEL_STATE_BLINK_FAST };

struct NativeEntity_TextLabel : NativeEntityBase {
    float x;
    float y;
    float z;
    float alignOffset;
    float timer;
    float scale;
    int alpha;
    int fontID;
    ushort text[64];
    TextLabelStates state;
    byte useRenderMatrix;
    MatrixF renderMatrix;
    void (*alignPtr)(NativeEntity_TextLabel *, TextAlignments);

#if !RETRO_USE_ORIGINAL_CODE
    byte useColors;
    byte r;
    byte g;
    byte b;
#endif
};

void TextLabel_Create(void *objPtr);
void TextLabel_Main(void *objPtr);

void TextLabel_Align(NativeEntity_TextLabel *label, TextAlignments align);

#endif // !NATIVE_TEXTLABEL_H
