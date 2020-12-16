#include "RetroEngine.hpp"


// Native Objects
int nativeEntityPos;

int activeEntityList[ACTIVE_NATIVEENTITY_COUNT];
NativeEntity objectEntityBank[NATIVEENTITY_COUNT];
int nativeEntityCount;

int nativeEntityCountBackup;
int backupEntityList[ACTIVE_NATIVEENTITY_COUNT];
NativeEntity objectEntityBackup[NATIVEENTITY_COUNT];

int nativeEntityCountBackupS;
int backupEntityListS[ACTIVE_NATIVEENTITY_COUNT];
NativeEntity objectEntityBackupS[NATIVEENTITY_COUNT];

//Game Objects
int objectLoop    = 0;
int curObjectType = 0;
Entity objectEntityList[ENTITY_COUNT];
int processObjectFlag[ENTITY_COUNT];
TypeGroupList objectTypeGroupList[TYPEGROUP_COUNT];

char typeNames[OBJECT_COUNT][0x40];

int OBJECT_BORDER_X1       = 0x80;
int OBJECT_BORDER_X2       = SCREEN_XSIZE + 0x80;
int OBJECT_BORDER_X3       = 0x20;
int OBJECT_BORDER_X4       = SCREEN_XSIZE + 0x20;

const int OBJECT_BORDER_Y1 = 0x100;
const int OBJECT_BORDER_Y2 = SCREEN_YSIZE + 0x100;
const int OBJECT_BORDER_Y3 = 0x7F;
const int OBJECT_BORDER_Y4 = SCREEN_YSIZE + 0x7F;

int playerListPos          = 0;

void ProcessStartupObjects()
{
    scriptFrameCount           = 0;
    ClearAnimationData();
    scriptEng.arrayPosition[2] = TEMPENTITY_START;
    OBJECT_BORDER_X1           = 0x80;
    OBJECT_BORDER_X3           = 0x20;
    OBJECT_BORDER_X2           = SCREEN_XSIZE + 0x80;
    OBJECT_BORDER_X4           = SCREEN_XSIZE + 0x20;
    Entity *entity             = &objectEntityList[TEMPENTITY_START];
    for (int i = 0; i < OBJECT_COUNT; ++i) {
        ObjectScript *scriptInfo    = &objectScriptList[i];
        objectLoop                  = TEMPENTITY_START;
        curObjectType               = i;
        scriptInfo->frameListOffset = scriptFrameCount;
        scriptInfo->spriteSheetID   = 0;
        entity->type                = i;
        if (scriptData[scriptInfo->subStartup.scriptCodePtr] > 0)
            ProcessScript(scriptInfo->subStartup.scriptCodePtr, scriptInfo->subStartup.jumpTablePtr, SUB_SETUP);
        scriptInfo->frameCount = scriptFrameCount - scriptInfo->frameListOffset;
    }
    entity->type  = 0;
    curObjectType = 0;
}

void ProcessObjects()
{
    for (int i = 0; i < DRAWLAYER_COUNT; ++i) drawListEntries[i].listSize = 0;
    for (int i = 0; i < TYPEGROUP_COUNT; ++i) objectTypeGroupList[i].listSize = 0;

    for (objectLoop = 0; objectLoop < ENTITY_COUNT; ++objectLoop) {
        processObjectFlag[objectLoop] = false;
        int x = 0, y = 0;
        Entity *entity = &objectEntityList[objectLoop];
        switch (entity->priority) {
            case PRIORITY_ACTIVE_BOUNDS:
                x      = entity->XPos >> 16;
                y      = entity->YPos >> 16;
                processObjectFlag[objectLoop] = x > xScrollOffset - OBJECT_BORDER_X1 && x < OBJECT_BORDER_X2 + xScrollOffset
                         && y > yScrollOffset - OBJECT_BORDER_Y1 && y < yScrollOffset + OBJECT_BORDER_Y2;
                break;
            case PRIORITY_ACTIVE: processObjectFlag[objectLoop] = true; break;
            case PRIORITY_ACTIVE_PAUSED: processObjectFlag[objectLoop] = true; break;
            case PRIORITY_ACTIVE_XBOUNDS:
                x      = entity->XPos >> 16;
                processObjectFlag[objectLoop] = x > xScrollOffset - OBJECT_BORDER_X1 && x < OBJECT_BORDER_X2 + xScrollOffset;
                break;
            case PRIORITY_ACTIVE_BOUNDS_REMOVE:
                x = entity->XPos >> 16;
                y = entity->YPos >> 16;
                if (x <= xScrollOffset - OBJECT_BORDER_X1 || x >= OBJECT_BORDER_X2 + xScrollOffset
                    || y <= yScrollOffset - OBJECT_BORDER_Y1 || y >= yScrollOffset + OBJECT_BORDER_Y2) {
                    processObjectFlag[objectLoop] = false;
                    entity->type = OBJ_TYPE_BLANKOBJECT;
                }
                else {
                    processObjectFlag[objectLoop] = true;
                }
                break;
            case PRIORITY_INACTIVE: processObjectFlag[objectLoop] = false; break;
            case PRIORITY_ACTIVE_BOUNDS_SMALL:
                x      = entity->XPos >> 16;
                y      = entity->YPos >> 16;
                processObjectFlag[objectLoop] = x > xScrollOffset - OBJECT_BORDER_X3 && x < OBJECT_BORDER_X4 + xScrollOffset
                                                && y > yScrollOffset - OBJECT_BORDER_Y3 && y < yScrollOffset + OBJECT_BORDER_Y4;
                break;
            case PRIORITY_ACTIVE_XBOUNDS_SMALL:
                x      = entity->XPos >> 16;
                processObjectFlag[objectLoop] = x > xScrollOffset - OBJECT_BORDER_X3 && x < OBJECT_BORDER_X4 + xScrollOffset;
                break;
            default: break;
        }

        if (processObjectFlag[objectLoop] && entity->type > OBJ_TYPE_BLANKOBJECT) {
            ObjectScript *scriptInfo = &objectScriptList[entity->type];
            if (scriptData[scriptInfo->subMain.scriptCodePtr] > 0)
                ProcessScript(scriptInfo->subMain.scriptCodePtr, scriptInfo->subMain.jumpTablePtr, SUB_MAIN);

            if (entity->drawOrder < DRAWLAYER_COUNT)
                drawListEntries[entity->drawOrder].entityRefs[drawListEntries[entity->drawOrder].listSize++] = objectLoop;
        }
    }
    
    for (objectLoop = 0; objectLoop < ENTITY_COUNT; ++objectLoop) {
        Entity *entity = &objectEntityList[objectLoop];
        if (processObjectFlag[objectLoop] && entity->type > OBJ_TYPE_BLANKOBJECT) {
            if (entity->typeGroup < TYPEGROUP_COUNT)
                objectTypeGroupList[entity->typeGroup].entityRefs[objectTypeGroupList[entity->typeGroup].listSize++] = objectLoop;
        }
    }

}

void ProcessPausedObjects()
{
    for (int i = 0; i < DRAWLAYER_COUNT; ++i) drawListEntries[i].listSize = 0;

    for (objectLoop = 0; objectLoop < ENTITY_COUNT; ++objectLoop) {
        Entity *entity = &objectEntityList[objectLoop];

        if (entity->priority == PRIORITY_ACTIVE_PAUSED && entity->type > OBJ_TYPE_BLANKOBJECT) {
            ObjectScript *scriptInfo = &objectScriptList[entity->type];
            if (scriptData[scriptInfo->subMain.scriptCodePtr] > 0)
                ProcessScript(scriptInfo->subMain.scriptCodePtr, scriptInfo->subMain.jumpTablePtr, SUB_MAIN);

            if (entity->drawOrder < DRAWLAYER_COUNT)
                drawListEntries[entity->drawOrder].entityRefs[drawListEntries[entity->drawOrder].listSize++] = objectLoop;
        }
    }
}


void ProcessFrozenObjects() {
    for (int i = 0; i < DRAWLAYER_COUNT; ++i) drawListEntries[i].listSize = 0;
    for (int i = 0; i < TYPEGROUP_COUNT; ++i) objectTypeGroupList[i].listSize = 0;

    for (objectLoop = 0; objectLoop < ENTITY_COUNT; ++objectLoop) {
        processObjectFlag[objectLoop] = false;
        int x = 0, y = 0;
        Entity *entity = &objectEntityList[objectLoop];
        switch (entity->priority) {
            case PRIORITY_ACTIVE_BOUNDS:
                x                             = entity->XPos >> 16;
                y                             = entity->YPos >> 16;
                processObjectFlag[objectLoop] = x > xScrollOffset - OBJECT_BORDER_X1 && x < OBJECT_BORDER_X2 + xScrollOffset
                                                && y > yScrollOffset - OBJECT_BORDER_Y1 && y < yScrollOffset + OBJECT_BORDER_Y2;
                break;
            case PRIORITY_ACTIVE: 
            case PRIORITY_ACTIVE_PAUSED:
            case PRIORITY_ACTIVE_XBOUNDS_SMALL: processObjectFlag[objectLoop] = true; break;
            case PRIORITY_ACTIVE_XBOUNDS:
                x                             = entity->XPos >> 16;
                processObjectFlag[objectLoop] = x > xScrollOffset - OBJECT_BORDER_X1 && x < OBJECT_BORDER_X2 + xScrollOffset;
                break;
            case PRIORITY_ACTIVE_BOUNDS_REMOVE:
                x = entity->XPos >> 16;
                y = entity->YPos >> 16;
                if (x <= xScrollOffset - OBJECT_BORDER_X1 || x >= OBJECT_BORDER_X2 + xScrollOffset || y <= yScrollOffset - OBJECT_BORDER_Y1
                    || y >= yScrollOffset + OBJECT_BORDER_Y2) {
                    processObjectFlag[objectLoop] = false;
                    entity->type                  = OBJ_TYPE_BLANKOBJECT;
                }
                else {
                    processObjectFlag[objectLoop] = true;
                }
                break;
            case PRIORITY_INACTIVE: processObjectFlag[objectLoop] = false; break;
            case PRIORITY_ACTIVE_BOUNDS_SMALL:
                x                             = entity->XPos >> 16;
                y                             = entity->YPos >> 16;
                processObjectFlag[objectLoop] = x > xScrollOffset - OBJECT_BORDER_X3 && x < OBJECT_BORDER_X4 + xScrollOffset
                                                && y > yScrollOffset - OBJECT_BORDER_Y3 && y < yScrollOffset + OBJECT_BORDER_Y4;
                break;
            default: break;
        }

        if (processObjectFlag[objectLoop] && entity->type > OBJ_TYPE_BLANKOBJECT) {
            ObjectScript *scriptInfo = &objectScriptList[entity->type];
            if (scriptData[scriptInfo->subMain.scriptCodePtr] > 0)
                ProcessScript(scriptInfo->subMain.scriptCodePtr, scriptInfo->subMain.jumpTablePtr, SUB_MAIN);

            if (entity->drawOrder < DRAWLAYER_COUNT)
                drawListEntries[entity->drawOrder].entityRefs[drawListEntries[entity->drawOrder].listSize++] = objectLoop;
        }
    }

    for (objectLoop = 0; objectLoop < ENTITY_COUNT; ++objectLoop) {
        Entity *entity = &objectEntityList[objectLoop];
        if (processObjectFlag[objectLoop] && entity->type > OBJ_TYPE_BLANKOBJECT) {
            if (entity->typeGroup < TYPEGROUP_COUNT)
                objectTypeGroupList[entity->typeGroup].entityRefs[objectTypeGroupList[entity->typeGroup].listSize++] = objectLoop;
        }
    }
}
void Process2PObjects() {
    for (int i = 0; i < DRAWLAYER_COUNT; ++i) drawListEntries[i].listSize = 0;
    for (int i = 0; i < TYPEGROUP_COUNT; ++i) objectTypeGroupList[i].listSize = 0;

    for (objectLoop = 0; objectLoop < ENTITY_COUNT; ++objectLoop) {
        processObjectFlag[objectLoop] = false;
        int x = 0, y = 0;
        Entity *entity = &objectEntityList[objectLoop];
        switch (entity->priority) {
            case PRIORITY_ACTIVE_BOUNDS:
                x                             = entity->XPos >> 16;
                y                             = entity->YPos >> 16;
                processObjectFlag[objectLoop] = x > xScrollOffset - OBJECT_BORDER_X1 && x < OBJECT_BORDER_X2 + xScrollOffset
                                                && y > yScrollOffset - OBJECT_BORDER_Y1 && y < yScrollOffset + OBJECT_BORDER_Y2;
                break;
            case PRIORITY_ACTIVE: processObjectFlag[objectLoop] = true; break;
            case PRIORITY_ACTIVE_PAUSED: processObjectFlag[objectLoop] = true; break;
            case PRIORITY_ACTIVE_XBOUNDS:
                x                             = entity->XPos >> 16;
                processObjectFlag[objectLoop] = x > xScrollOffset - OBJECT_BORDER_X1 && x < OBJECT_BORDER_X2 + xScrollOffset;
                break;
            case PRIORITY_ACTIVE_BOUNDS_REMOVE:
                x = entity->XPos >> 16;
                y = entity->YPos >> 16;
                if (x <= xScrollOffset - OBJECT_BORDER_X1 || x >= OBJECT_BORDER_X2 + xScrollOffset || y <= yScrollOffset - OBJECT_BORDER_Y1
                    || y >= yScrollOffset + OBJECT_BORDER_Y2) {
                    processObjectFlag[objectLoop] = false;
                    entity->type                  = OBJ_TYPE_BLANKOBJECT;
                }
                else {
                    processObjectFlag[objectLoop] = true;
                }
                break;
            case PRIORITY_INACTIVE: processObjectFlag[objectLoop] = false; break;
            case PRIORITY_ACTIVE_BOUNDS_SMALL:
                x                             = entity->XPos >> 16;
                y                             = entity->YPos >> 16;
                processObjectFlag[objectLoop] = x > xScrollOffset - OBJECT_BORDER_X3 && x < OBJECT_BORDER_X4 + xScrollOffset
                                                && y > yScrollOffset - OBJECT_BORDER_Y3 && y < yScrollOffset + OBJECT_BORDER_Y4;
                break;
            case PRIORITY_ACTIVE_XBOUNDS_SMALL:
                x                             = entity->XPos >> 16;
                processObjectFlag[objectLoop] = x > xScrollOffset - OBJECT_BORDER_X3 && x < OBJECT_BORDER_X4 + xScrollOffset;
                break;
            default: break;
        }

        if (processObjectFlag[objectLoop] && entity->type > OBJ_TYPE_BLANKOBJECT) {
            ObjectScript *scriptInfo = &objectScriptList[entity->type];
            if (scriptData[scriptInfo->subMain.scriptCodePtr] > 0)
                ProcessScript(scriptInfo->subMain.scriptCodePtr, scriptInfo->subMain.jumpTablePtr, SUB_MAIN);

            if (entity->drawOrder < DRAWLAYER_COUNT)
                drawListEntries[entity->drawOrder].entityRefs[drawListEntries[entity->drawOrder].listSize++] = objectLoop;
        }
    }

    for (objectLoop = 0; objectLoop < ENTITY_COUNT; ++objectLoop) {
        Entity *entity = &objectEntityList[objectLoop];
        if (processObjectFlag[objectLoop] && entity->type > OBJ_TYPE_BLANKOBJECT) {
            if (entity->typeGroup < TYPEGROUP_COUNT)
                objectTypeGroupList[entity->typeGroup].entityRefs[objectTypeGroupList[entity->typeGroup].listSize++] = objectLoop;
        }
    }
}

void ProcessPlayerControl(Entity *Player)
{
    if (Player->controlMode) {
        return;
    }

    Player->up   = keyDown.up;
    Player->down = keyDown.down;
    if (!keyDown.left || !keyDown.right) {
        Player->left  = keyDown.left;
        Player->right = keyDown.right;
    }
    else {
        Player->left  = false;
        Player->right = false;
    }
    Player->jumpHold  = keyDown.C | keyDown.B | keyDown.A;
    Player->jumpPress = keyPress.C | keyPress.B | keyPress.A;
}

void InitNativeObjectSystem() {

}
void CreateNativeObject(void (*create)(void* objPtr), void (*main)(void* objPtr)) {

}
void RemoveNativeObject(NativeEntity* NativeEntry) {

}
void ProcessNativeObjects() {
    //ResetRenderStates();
    nativeEntityPos = 0;
    if (nativeEntityCount > 0) {
        do {
            NativeEntity* entity = &objectEntityBank[activeEntityList[nativeEntityPos]];
            entity->Main(entity);
            ++nativeEntityPos;
        } while (nativeEntityCount > nativeEntityPos);
    }
    //RenderScene()
}