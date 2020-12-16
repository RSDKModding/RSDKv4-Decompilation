#ifndef DEBUG_H
#define DEBUG_H


enum DevMenuMenus {
    DEVMENU_MAIN,
    DEVMENU_PLAYERSEL,
    DEVMENU_STAGELISTSEL,
    DEVMENU_STAGESEL,
    DEVMENU_SCRIPTERROR,
};

void initDevMenu();
void initErrorMessage();
void processStageSelect();

#endif //!DEBUG_H
