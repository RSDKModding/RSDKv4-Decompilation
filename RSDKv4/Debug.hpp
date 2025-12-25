#ifndef DEBUG_H
#define DEBUG_H

#if RETRO_PLATFORM == RETRO_ANDROID
#include <android/log.h>
#endif

extern bool endLine;
void PrintLog(const char *msg, ...);
void PrintLog(const ushort *msg);

enum DevMenuMenus {
    DEVMENU_MAIN,
    DEVMENU_PLAYERSEL,
    DEVMENU_STAGELISTSEL,
    DEVMENU_STAGESEL,
    DEVMENU_SCRIPTERROR,
#if !RETRO_USE_ORIGINAL_CODE
    DEVMENU_MODMENU
#endif
};

void InitDevMenu();
void InitErrorMessage();
void ProcessStageSelect();

// Not in original, but the code was, and its cleaner this way
void SetTextMenu(int mode);

#endif //! DEBUG_H
