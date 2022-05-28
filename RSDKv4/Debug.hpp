#ifndef DEBUG_H
#define DEBUG_H

#if RETRO_PLATFORM == RETRO_ANDROID
#include <android/log.h>
#endif

extern bool endLine;
inline void PrintLog(const char *msg, ...)
{
#ifndef RETRO_DISABLE_LOG
    if (engineDebugMode) {
        char buffer[0x100];

        // make the full string
        va_list args;
        va_start(args, msg);
        vsprintf(buffer, msg, args);
        if (endLine) {
            printf("%s\n", buffer);
            sprintf(buffer, "%s\n", buffer);
        }
        else {
            printf("%s", buffer);
            sprintf(buffer, "%s", buffer);
        }

        char pathBuffer[0x100];
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(pathBuffer, "%s/log.txt", getResourcesPath());
        else
            sprintf(pathBuffer, "log.txt");
#elif RETRO_PLATFORM == RETRO_ANDROID
        sprintf(pathBuffer, "%s/log.txt", gamePath);
        __android_log_print(ANDROID_LOG_INFO, "RSDKv4", "%s", buffer);
#else
        sprintf(pathBuffer, BASE_PATH "log.txt");
#endif
        FileIO *file = fOpen(pathBuffer, "a");
        if (file) {
            fWrite(&buffer, 1, StrLength(buffer), file);
            fClose(file);
        }
    }
#endif
}

inline void PrintLog(const ushort *msg)
{
#ifndef RETRO_DISABLE_LOG
    if (engineDebugMode) {
        int mPos = 0;
        while (msg[mPos]) {
            printf("%lc", (ushort)msg[mPos]);
            mPos++;
        }
        if (endLine)
            printf("\n");

        char pathBuffer[0x100];
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(pathBuffer, "%s/log.txt", getResourcesPath());
        else
            sprintf(pathBuffer, "log.txt");
#elif RETRO_PLATFORM == RETRO_ANDROID
        sprintf(pathBuffer, "%s/log.txt", gamePath);
        __android_log_print(ANDROID_LOG_INFO, "RSDKv4", "%ls", (wchar_t *)msg);
#else
        sprintf(pathBuffer, BASE_PATH "log.txt");
#endif
        mPos         = 0;
        FileIO *file = fOpen(pathBuffer, "a");
        if (file) {
            while (msg[mPos]) {
                fWrite(&msg[mPos], 2, 1, file);
                mPos++;
            }

            ushort el = '\n';
            if (endLine)
                fWrite(&el, 2, 1, file);
            fClose(file);
        }
    }
#endif
}

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
