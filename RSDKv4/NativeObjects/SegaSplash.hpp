#ifndef NATIVE_SEGASPLASH_H
#define NATIVE_SEGASPLASH_H

struct NativeEntity_SegaSplash : NativeEntityBase {
    int state;
    float rectAlpha;
    byte textureID;
};

void SegaSplash_Create(void *objPtr);
void SegaSplash_Main(void *objPtr);

#endif // !NATIVE_SEGASPLASH_H
