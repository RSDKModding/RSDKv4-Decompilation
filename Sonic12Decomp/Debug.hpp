#ifndef DEBUG_H
#define DEBUG_H

extern bool endLine;
inline void printLog(const char *msg, ...)
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
#if RETRO_PLATFORM == RETRO_OSX
        if (!usingCWD)
            sprintf(pathBuffer, "%s/log.txt", getResourcesPath());
        else
            sprintf(pathBuffer, "log.txt");
#else
        sprintf(pathBuffer, BASE_PATH "log.txt");
#endif
        FileIO *file = fOpen(pathBuffer, "a");
        if (file) {
            fWrite(&buffer, 1, StrLength(buffer), file);
            fClose(file);
            RETRO_FILE_COMMIT_FUNC();
        }
    }
#endif
}

inline void printLog(const ushort *msg)
{
#ifndef RETRO_DISABLE_LOG
    if (engineDebugMode) {
        int mPos = 0;
        while (msg[mPos]) {
            printf("%lc", (wint_t)msg[mPos]);
            mPos++;
        }
        if (endLine)
            printf("\n");

        char pathBuffer[0x100];
#if RETRO_PLATFORM == RETRO_OSX
        if (!usingCWD)
            sprintf(pathBuffer, "%s/log.txt", getResourcesPath());
        else
            sprintf(pathBuffer, "log.txt");
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
            RETRO_FILE_COMMIT_FUNC();
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
};

enum StartMenuMenus {
    STARTMENU_MAIN = 5,
    STARTMENU_SAVESEL,
    STARTMENU_PLAYERSEL,
    STARTMENU_GAMEOPTS,
    STARTMENU_TASTAGESEL,
    STARTMENU_TACONFIRMSEL,
    STARTMENU_ACHIEVEMENTS,
    STARTMENU_LEADERBOARDS,
};

void initDevMenu();
void initErrorMessage();
void processStageSelect();

// added due to lack of normal main menu
void initStartMenu(int mode);
void processStartMenu(); 
void setTextMenu(int mode);

#endif //!DEBUG_H
