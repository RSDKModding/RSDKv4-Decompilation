
#ifndef NATIVE_MULTIPLAYERHANDLER_H
#define NATIVE_MULTIPLAYERHANDLER_H

#if !RETRO_USE_ORIGINAL_CODE
struct NativeEntity_MultiplayerHandler : NativeEntityBase {
    int state;
    float timer;
    NativeEntity_TextLabel *pingLabel;
    NativeEntity_DialogPanel *errorPanel;
    NativeEntity_FadeScreen *fade;
};

void MultiplayerHandler_Create(void *objPtr);
void MultiplayerHandler_Main(void *objPtr);
#endif

#endif // !NATIVE_MULTIPLAYERHANDLER_H