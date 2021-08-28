#ifndef NATIVE_OPTIONSBUTTON_H
#define NATIVE_OPTIONSBUTTON_H

struct NativeEntity_OptionsButton : NativeEntityBase {
    int field_10;
    byte visible;
    int field_18;
    int field_1C;
    float translateX;
    float translateY;
    float translateZ;
    MeshInfo *meshOptions;
    float angle;
    float scale;
    byte textureCircle;
    byte r;
    byte g;
    byte b;
    MatrixF renderMatrix;
    MatrixF matrix2;
    NativeEntity_TextLabel *labelPtr;
};

void OptionsButton_Create(void *objPtr);
void OptionsButton_Main(void *objPtr);

#endif // !NATIVE_OPTIONSBUTTON_H
