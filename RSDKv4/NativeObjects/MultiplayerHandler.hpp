
#ifndef NATIVE_MULTIPLAYERHANDLER_H
#define NATIVE_MULTIPLAYERHANDLER_H

#if !RETRO_USE_ORIGINAL_CODE
struct NativeEntity_MultiplayerHandler : NativeEntityBase {
    int state;
    NativeEntity_TextLabel *pingLabel;
    NativeEntity_DialogPanel *errorPanel;
};

void MultiplayerHandler_Create(void *objPtr);
void MultiplayerHandler_Main(void *objPtr);
#endif

#endif // !NATIVE_MULTIPLAYERHANDLER_H