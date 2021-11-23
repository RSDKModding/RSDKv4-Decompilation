#ifndef NATIVE_MODINFOBUTTON_H
#define NATIVE_MODINFOBUTTON_H

#if RETRO_USE_MOD_LOADER
struct NativeEntity_ModInfoButton : NativeEntityBase {
};

void ModInfoButton_Create(void *objPtr);
void ModInfoButton_Main(void *objPtr);
#endif

#endif // !NATIVE_MODINFOBUTTON_H
