#ifndef NATIVE_OPTIONSBUTTON_H
#define NATIVE_OPTIONSBUTTON_H

struct NativeEntity_OptionsButton : NativeEntityBase {
    int unused1;
    byte visible;
    int unused2;
    int unused3;
    float x;
    float y;
    float z;
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
