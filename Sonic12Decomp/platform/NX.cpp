#ifdef __SWITCH__
#include <switch.h>
#include "nx.h"

extern "C" {
    void userAppInit(void);
    void userAppExit(void);
}

void userAppInit()
{
    smInitialize();
    fsInitialize();
    hidInitialize();
    romfsInit();

#ifdef NX_APPLICATION_ID
    accountInitialize(AccountServiceType_Application);
    mountSaveData();
#endif
}

void userAppExit() 
{
#ifdef NX_APPLICATION_ID
    unmountSaveData();
#endif

    romfsExit();
    hidExit();
}

AccountUid currentUser()
{
    AccountUid uid;

    uid.uid[0] = 0;
    uid.uid[1] = 0;

    if (accountGetPreselectedUser(&uid)) {
    }

    return uid;
}

int mountSaveData()
{
#ifdef NX_APPLICATION_ID
    FsFileSystem fileSystem;

    if (fsOpen_SaveData(&fileSystem, NX_APPLICATION_ID, currentUser())) {
        return -1;
    }

    if (fsdevMountDevice("save", fileSystem) == -1) {
        fsdevUnmountDevice("save");
        return -2;
    }
#endif
    return 0;
}

int unmountSaveData()
{
#ifdef NX_APPLICATION_ID
    if (fsdevUnmountDevice("save") == -1) {
        return -1;
    }
#endif
    return 0;
}

int commitSave()
{
#ifdef NX_APPLICATION_ID
    return fsdevCommitDevice("save");
#else
    return 0;
#endif
}


#endif
