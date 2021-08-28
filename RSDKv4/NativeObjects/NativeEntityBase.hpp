#ifndef NATIVE_NENTITY_H
#define NATIVE_NENTITY_H

struct NativeEntity_NEntity : NativeEntityBase {
};

void NEntity_Create(void *objPtr);
void NEntity_Main(void *objPtr);

#endif // !NATIVE_NENTITY_H
