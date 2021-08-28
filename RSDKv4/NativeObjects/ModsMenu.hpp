#ifndef NATIVE_MODSMENU_H
#define NATIVE_MODSMENU_H

#if RETRO_USE_MOD_LOADER
struct NativeEntity_ModsMenu : NativeEntityBase {
};

void ModsMenu_Create(void *objPtr);
void ModsMenu_Main(void *objPtr);
#endif

#endif // !NATIVE_MODSMENU_H
