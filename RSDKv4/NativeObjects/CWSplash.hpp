#ifndef NATIVE_CWSPLASH_H
#define NATIVE_CWSPLASH_H

struct NativeEntity_CWSplash : NativeEntityBase {
    int state;
    float rectAlpha;
    char textureID;
};

void CWSplash_Create(void *objPtr);
void CWSplash_Main(void *objPtr);

#endif // !NATIVE_CWSPLASH_H
