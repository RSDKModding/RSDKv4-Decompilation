#include "RetroEngine.hpp"

// Native Objects
int nativeEntityPos;

int activeEntityList[NATIVEENTITY_COUNT];
int objectRemoveFlag[NATIVEENTITY_COUNT];
NativeEntity *nativeEntityList[NATIVEENTITY_COUNT];
NativeEntity objectEntityBank[NATIVEENTITY_COUNT];
int nativeEntityCount = 0;

int nativeEntityCountBackup = 0;
int backupEntityList[NATIVEENTITY_COUNT];
NativeEntity objectEntityBackup[NATIVEENTITY_COUNT];

int nativeEntityCountBackupS = 0;
int backupEntityListS[NATIVEENTITY_COUNT];
NativeEntity objectEntityBackupS[NATIVEENTITY_COUNT];

// Game Objects
int objectEntityPos = 0;
int curObjectType   = 0;
Entity objectEntityList[ENTITY_COUNT];
int processObjectFlag[ENTITY_COUNT];
TypeGroupList objectTypeGroupList[TYPEGROUP_COUNT];

char typeNames[OBJECT_COUNT][0x40];

int OBJECT_BORDER_X1 = 0x80;
int OBJECT_BORDER_X2 = SCREEN_XSIZE + 0x80;
int OBJECT_BORDER_X3 = 0x20;
int OBJECT_BORDER_X4 = SCREEN_XSIZE + 0x20;

const int OBJECT_BORDER_Y1 = 0x100;
const int OBJECT_BORDER_Y2 = SCREEN_YSIZE + 0x100;
const int OBJECT_BORDER_Y3 = 0x80;
const int OBJECT_BORDER_Y4 = SCREEN_YSIZE + 0x80;

int playerListPos = 0;

void ProcessStartupObjects()
{
    scriptFrameCount = 0;
    ClearAnimationData();
    scriptEng.arrayPosition[8] = TEMPENTITY_START;
    OBJECT_BORDER_X1           = 0x80;
    OBJECT_BORDER_X3           = 0x20;
    OBJECT_BORDER_X2           = SCREEN_XSIZE + 0x80;
    OBJECT_BORDER_X4           = SCREEN_XSIZE + 0x20;
    Entity *entity             = &objectEntityList[TEMPENTITY_START];

    memset(foreachStack, -1, FORSTACK_COUNT * sizeof(int));
    memset(jumpTableStack, 0, JUMPSTACK_COUNT * sizeof(int));

    int flagStore  = GetGlobalVariableByName("options.stageSelectFlag");
    int flagStore2 = GetGlobalVariableByName("options.saveSlot");
    SetGlobalVariableByName("options.stageSelectFlag", 1); // temp, to allow game opts

    for (int i = 0; i < OBJECT_COUNT; ++i) {
        ObjectScript *scriptInfo    = &objectScriptList[i];
        objectEntityPos             = TEMPENTITY_START;
        curObjectType               = i;
        scriptInfo->frameListOffset = scriptFrameCount;
        scriptInfo->spriteSheetID   = 0;
        entity->type                = i;

        // Man this is so hacky, I hope there's a better way to do this
        if (StrComp("StageSetup", typeNames[i])) {
            SetGlobalVariableByName("options.saveSlot", 0);
        }

        if (scriptData[scriptInfo->eventStartup.scriptCodePtr] > 0)
            ProcessScript(scriptInfo->eventStartup.scriptCodePtr, scriptInfo->eventStartup.jumpTablePtr, EVENT_SETUP);
        scriptInfo->frameCount = scriptFrameCount - scriptInfo->frameListOffset;

        if (StrComp("StageSetup", typeNames[i])) {
            SetGlobalVariableByName("options.saveSlot", flagStore2);
        }
    }
    entity->type  = 0;
    curObjectType = 0;

    // Temp(?): forces game options to load on non-no save slots
    SetGlobalVariableByName("options.stageSelectFlag", flagStore);
    if (GetGlobalVariableByName("options.gameMode") == 1) {
        if (Engine.gameType == GAME_SONIC1) {
            SetGlobalVariableByName("options.spindash", saveRAM[0x101]);
            SetGlobalVariableByName("options.speedCap", saveRAM[0x102]);
            SetGlobalVariableByName("options.airSpeedCap", saveRAM[0x103]);
            SetGlobalVariableByName("options.spikeBehavior", saveRAM[0x104]);
            SetGlobalVariableByName("options.shieldType", saveRAM[0x105]);
            SetGlobalVariableByName("options.superStates", saveRAM[0x106]);
        }
        else {
            SetGlobalVariableByName("options.airSpeedCap", saveRAM[0x101]);
            SetGlobalVariableByName("options.tailsFlight", saveRAM[0x102]);
            SetGlobalVariableByName("options.superTails", saveRAM[0x103]);
            SetGlobalVariableByName("options.spikeBehavior", saveRAM[0x104]);
            SetGlobalVariableByName("options.shieldType", saveRAM[0x105]);
        }
    }
}

void ProcessObjects()
{
    for (int i = 0; i < DRAWLAYER_COUNT; ++i) drawListEntries[i].listSize = 0;

    for (objectEntityPos = 0; objectEntityPos < ENTITY_COUNT; ++objectEntityPos) {
        processObjectFlag[objectEntityPos] = false;
        int x = 0, y = 0;
        Entity *entity = &objectEntityList[objectEntityPos];
        x              = entity->XPos >> 16;
        y              = entity->YPos >> 16;

        switch (entity->priority) {
            case PRIORITY_ACTIVE_BOUNDS:
                processObjectFlag[objectEntityPos] = x > xScrollOffset - OBJECT_BORDER_X1 && x < xScrollOffset + OBJECT_BORDER_X2
                                                     && y > yScrollOffset - OBJECT_BORDER_Y1 && y < yScrollOffset + OBJECT_BORDER_Y2;
                break;
            case PRIORITY_ACTIVE:
            case PRIORITY_ACTIVE_PAUSED:
            case PRIORITY_ACTIVE2: processObjectFlag[objectEntityPos] = true; break;
            case PRIORITY_ACTIVE_XBOUNDS:
                processObjectFlag[objectEntityPos] = x > xScrollOffset - OBJECT_BORDER_X1 && x < OBJECT_BORDER_X2 + xScrollOffset;
                break;
            case PRIORITY_ACTIVE_XBOUNDS_REMOVE:
                processObjectFlag[objectEntityPos] = x > xScrollOffset - OBJECT_BORDER_X1 && x < xScrollOffset + OBJECT_BORDER_X2;
                if (!processObjectFlag[objectEntityPos]) {
                    processObjectFlag[objectEntityPos] = false;
                    entity->type                       = OBJ_TYPE_BLANKOBJECT;
                }
                break;
            case PRIORITY_INACTIVE: processObjectFlag[objectEntityPos] = false; break;
            case PRIORITY_ACTIVE_BOUNDS_SMALL:
                processObjectFlag[objectEntityPos] = x > xScrollOffset - OBJECT_BORDER_X3 && x < OBJECT_BORDER_X4 + xScrollOffset
                                                     && y > yScrollOffset - OBJECT_BORDER_Y3 && y < yScrollOffset + OBJECT_BORDER_Y4;
                break;
            default: break;
        }

        if (processObjectFlag[objectEntityPos] && entity->type > OBJ_TYPE_BLANKOBJECT) {
            ObjectScript *scriptInfo = &objectScriptList[entity->type];
            if (scriptData[scriptInfo->eventMain.scriptCodePtr] > 0)
                ProcessScript(scriptInfo->eventMain.scriptCodePtr, scriptInfo->eventMain.jumpTablePtr, EVENT_MAIN);

            if (entity->drawOrder < DRAWLAYER_COUNT && entity->drawOrder >= 0)
                drawListEntries[entity->drawOrder].entityRefs[drawListEntries[entity->drawOrder].listSize++] = objectEntityPos;
        }
    }

    for (int i = 0; i < TYPEGROUP_COUNT; ++i) objectTypeGroupList[i].listSize = 0;

    for (objectEntityPos = 0; objectEntityPos < ENTITY_COUNT; ++objectEntityPos) {
        Entity *entity = &objectEntityList[objectEntityPos];
        if (processObjectFlag[objectEntityPos] && entity->objectInteractions) {
            if (entity->typeGroup < OBJECT_COUNT) {
                TypeGroupList *list                = &objectTypeGroupList[objectEntityList[objectEntityPos].type];
                list->entityRefs[list->listSize++] = objectEntityPos;
            }
            else {
                TypeGroupList *list                = &objectTypeGroupList[objectEntityList[objectEntityPos].typeGroup];
                list->entityRefs[list->listSize++] = objectEntityPos;
            }
        }
    }
}
void ProcessPausedObjects()
{
    for (int i = 0; i < DRAWLAYER_COUNT; ++i) drawListEntries[i].listSize = 0;

    for (objectEntityPos = 0; objectEntityPos < ENTITY_COUNT; ++objectEntityPos) {
        Entity *entity = &objectEntityList[objectEntityPos];

        if (entity->priority == PRIORITY_ACTIVE_PAUSED && entity->type > OBJ_TYPE_BLANKOBJECT) {
            ObjectScript *scriptInfo = &objectScriptList[entity->type];
            if (scriptData[scriptInfo->eventMain.scriptCodePtr] > 0)
                ProcessScript(scriptInfo->eventMain.scriptCodePtr, scriptInfo->eventMain.jumpTablePtr, EVENT_MAIN);

            if (entity->drawOrder < DRAWLAYER_COUNT && entity->drawOrder >= 0)
                drawListEntries[entity->drawOrder].entityRefs[drawListEntries[entity->drawOrder].listSize++] = objectEntityPos;
        }
    }
}
void ProcessFrozenObjects()
{
    for (int i = 0; i < DRAWLAYER_COUNT; ++i) drawListEntries[i].listSize = 0;

    for (objectEntityPos = 0; objectEntityPos < ENTITY_COUNT; ++objectEntityPos) {
        processObjectFlag[objectEntityPos] = false;
        int x = 0, y = 0;
        Entity *entity = &objectEntityList[objectEntityPos];
        x              = entity->XPos >> 16;
        y              = entity->YPos >> 16;

        switch (entity->priority) {
            case PRIORITY_ACTIVE_BOUNDS:
                processObjectFlag[objectEntityPos] = x > xScrollOffset - OBJECT_BORDER_X1 && x < xScrollOffset + OBJECT_BORDER_X2
                                                     && y > yScrollOffset - OBJECT_BORDER_Y1 && y < yScrollOffset + OBJECT_BORDER_Y2;
                break;
            case PRIORITY_ACTIVE:
            case PRIORITY_ACTIVE_PAUSED:
            case PRIORITY_ACTIVE2: processObjectFlag[objectEntityPos] = true; break;
            case PRIORITY_ACTIVE_XBOUNDS:
                processObjectFlag[objectEntityPos] = x > xScrollOffset - OBJECT_BORDER_X1 && x < OBJECT_BORDER_X2 + xScrollOffset;
                break;
            case PRIORITY_ACTIVE_XBOUNDS_REMOVE:
                processObjectFlag[objectEntityPos] = x > xScrollOffset - OBJECT_BORDER_X1 && x < xScrollOffset + OBJECT_BORDER_X2;
                if (!processObjectFlag[objectEntityPos]) {
                    processObjectFlag[objectEntityPos] = false;
                    entity->type                       = OBJ_TYPE_BLANKOBJECT;
                }
                break;
            case PRIORITY_INACTIVE: processObjectFlag[objectEntityPos] = false; break;
            case PRIORITY_ACTIVE_BOUNDS_SMALL:
                processObjectFlag[objectEntityPos] = x > xScrollOffset - OBJECT_BORDER_X3 && x < OBJECT_BORDER_X4 + xScrollOffset
                                                     && y > yScrollOffset - OBJECT_BORDER_Y3 && y < yScrollOffset + OBJECT_BORDER_Y4;
                break;
            default: break;
        }

        if (entity->type > OBJ_TYPE_BLANKOBJECT) {
            ObjectScript *scriptInfo = &objectScriptList[entity->type];
            if (scriptData[scriptInfo->eventMain.scriptCodePtr] > 0 && entity->priority == PRIORITY_ACTIVE_PAUSED)
                ProcessScript(scriptInfo->eventMain.scriptCodePtr, scriptInfo->eventMain.jumpTablePtr, EVENT_MAIN);

            if (entity->drawOrder < DRAWLAYER_COUNT && entity->drawOrder >= 0)
                drawListEntries[entity->drawOrder].entityRefs[drawListEntries[entity->drawOrder].listSize++] = objectEntityPos;
        }
    }

    for (int i = 0; i < TYPEGROUP_COUNT; ++i) objectTypeGroupList[i].listSize = 0;

    for (objectEntityPos = 0; objectEntityPos < ENTITY_COUNT; ++objectEntityPos) {
        Entity *entity = &objectEntityList[objectEntityPos];
        if (processObjectFlag[objectEntityPos] && entity->objectInteractions) {
            if (entity->typeGroup < OBJECT_COUNT) {
                TypeGroupList *list                = &objectTypeGroupList[objectEntityList[objectEntityPos].type];
                list->entityRefs[list->listSize++] = objectEntityPos;
            }
            else {
                TypeGroupList *list                = &objectTypeGroupList[objectEntityList[objectEntityPos].typeGroup];
                list->entityRefs[list->listSize++] = objectEntityPos;
            }
        }
    }
}
void Process2PObjects()
{
    for (int i = 0; i < DRAWLAYER_COUNT; ++i) drawListEntries[i].listSize = 0;

    Entity *entityP1 = &objectEntityList[0];
    int XPosP1       = entityP1->XPos;
    int YPosP1       = entityP1->YPos;
    Entity *entityP2 = &objectEntityList[1];
    int XPosP2       = entityP2->XPos;
    int YPosP2       = entityP2->YPos;

    for (objectEntityPos = 0; objectEntityPos < ENTITY_COUNT; ++objectEntityPos) {
        processObjectFlag[objectEntityPos] = false;
        int x = 0, y = 0;

        Entity *entity = &objectEntityList[objectEntityPos];
        x              = entity->XPos;
        y              = entity->YPos;
        switch (entity->priority) {
            case PRIORITY_ACTIVE_BOUNDS:
                processObjectFlag[objectEntityPos] =
                    x > XPosP1 - (0x200 << 16) && x < XPosP1 + (0x200 << 16) && y > YPosP1 - (0x180 << 16) && y < YPosP1 + (0x180 << 16);
                if (!processObjectFlag[objectEntityPos]) {
                    processObjectFlag[objectEntityPos] =
                        x > XPosP2 - (0x200 << 16) && x < XPosP2 + (0x200 << 16) && y > YPosP2 - (0x180 << 16) && y < YPosP2 + (0x180 << 16);
                }
                break;
            case PRIORITY_ACTIVE:
            case PRIORITY_ACTIVE_PAUSED:
            case PRIORITY_ACTIVE2: processObjectFlag[objectEntityPos] = true; break;
            case PRIORITY_ACTIVE_XBOUNDS:
                processObjectFlag[objectEntityPos] = x > XPosP1 - (0x200 << 16) && x < XPosP1 + (0x200 << 16);
                if (!processObjectFlag[objectEntityPos]) {
                    processObjectFlag[objectEntityPos] = x > XPosP2 - (0x200 << 16) && x < XPosP2 + (0x200 << 16);
                }
                break;
            case PRIORITY_ACTIVE_XBOUNDS_REMOVE:
                processObjectFlag[objectEntityPos] = x > XPosP1 - (0x200 << 16) && x < XPosP1 + (0x200 << 16);
                if (!processObjectFlag[objectEntityPos]) {
                    processObjectFlag[objectEntityPos] = x > XPosP2 - (0x200 << 16) && x < XPosP2 + (0x200 << 16);
                }

                if (!processObjectFlag[objectEntityPos]) {
                    entity->type = OBJ_TYPE_BLANKOBJECT;
                }
                break;
            case PRIORITY_INACTIVE: processObjectFlag[objectEntityPos] = false; break;
            case PRIORITY_ACTIVE_BOUNDS_SMALL:
                processObjectFlag[objectEntityPos] =
                    x > XPosP1 - (0x180 << 16) && x < XPosP1 + (0x180 << 16) && y > YPosP1 - (0x100 << 16) && y < YPosP1 + (0x100 << 16);
                if (!processObjectFlag[objectEntityPos]) {
                    processObjectFlag[objectEntityPos] =
                        x > XPosP2 - (0x180 << 16) && x < XPosP2 + (0x180 << 16) && y > YPosP2 - (0x100 << 16) && y < YPosP2 + (0x100 << 16);
                }
                break;
            default: break;
        }

        if (processObjectFlag[objectEntityPos] && entity->type > OBJ_TYPE_BLANKOBJECT) {
            ObjectScript *scriptInfo = &objectScriptList[entity->type];
            if (scriptData[scriptInfo->eventMain.scriptCodePtr] > 0)
                ProcessScript(scriptInfo->eventMain.scriptCodePtr, scriptInfo->eventMain.jumpTablePtr, EVENT_MAIN);

            if (entity->drawOrder < DRAWLAYER_COUNT && entity->drawOrder >= 0)
                drawListEntries[entity->drawOrder].entityRefs[drawListEntries[entity->drawOrder].listSize++] = objectEntityPos;
        }
    }

    for (int i = 0; i < TYPEGROUP_COUNT; ++i) objectTypeGroupList[i].listSize = 0;

    for (objectEntityPos = 0; objectEntityPos < ENTITY_COUNT; ++objectEntityPos) {
        Entity *entity = &objectEntityList[objectEntityPos];
        if (processObjectFlag[objectEntityPos] && entity->objectInteractions) {
            if (entity->typeGroup < OBJECT_COUNT) {
                TypeGroupList *list                = &objectTypeGroupList[objectEntityList[objectEntityPos].type];
                list->entityRefs[list->listSize++] = objectEntityPos;
            }
            else {
                TypeGroupList *list                = &objectTypeGroupList[objectEntityList[objectEntityPos].typeGroup];
                list->entityRefs[list->listSize++] = objectEntityPos;
            }
        }
    }
}

void SetObjectTypeName(const char *objectName, int objectID)
{
    int objNameID  = 0;
    int typeNameID = 0;
    while (objectName[objNameID]) {
        if (objectName[objNameID] != ' ')
            typeNames[objectID][typeNameID++] = objectName[objNameID];
        ++objNameID;
    }
    typeNames[objectID][typeNameID] = 0;
    printLog("Set Object (%d) name to: %s", objectID, objectName);
}

void ProcessPlayerControl(Entity *player)
{
    if (!player->controlMode) {
        player->up   = keyDown.up;
        player->down = keyDown.down;
        if (!keyDown.left || !keyDown.right) {
            player->left  = keyDown.left;
            player->right = keyDown.right;
        }
        else {
            player->left  = false;
            player->right = false;
        }
        player->jumpHold  = keyDown.C | keyDown.B | keyDown.A;
        player->jumpPress = keyPress.C | keyPress.B | keyPress.A;
    }
}

void InitNativeObjectSystem()
{
    InitLocalizedStrings();

    nativeEntityCount = 0;
    memset(activeEntityList, 0, NATIVEENTITY_COUNT * sizeof(int));
    memset(objectRemoveFlag, 0, NATIVEENTITY_COUNT * sizeof(int));
    memset(nativeEntityList, 0, NATIVEENTITY_COUNT * sizeof(NativeEntityBase *));
    memset(objectEntityBank, 0, NATIVEENTITY_COUNT * sizeof(NativeEntityBase));

    nativeEntityCountBackup = 0;
    memset(backupEntityList, 0, NATIVEENTITY_COUNT * sizeof(int));
    memset(objectEntityBackup, 0, NATIVEENTITY_COUNT * sizeof(NativeEntityBase));

    nativeEntityCountBackupS = 0;
    memset(backupEntityListS, 0, NATIVEENTITY_COUNT * sizeof(int));
    memset(objectEntityBackupS, 0, NATIVEENTITY_COUNT * sizeof(NativeEntityBase));

    ReadSaveRAMData();
    if (!saveRAM[32]) // if new save
    {
        saveRAM[32] = 1; // Not new save
        saveRAM[33] = MAX_VOLUME;
        saveRAM[34] = MAX_VOLUME;
        saveRAM[35] = 1;
        saveRAM[36] = 0; // Box-Region
        saveRAM[37] = 64;
        saveRAM[38] = 160;
        saveRAM[39] = 56;
        saveRAM[40] = 184;
        saveRAM[41] = -56;
        saveRAM[42] = 188;
        saveRAM[43] = 0;
        saveRAM[44] = 0;
        saveRAM[45] = 0;
        WriteSaveRAMData();
    }
    saveRAM[33] = bgmVolume;
    saveRAM[34] = sfxVolume;

    if (!saveRAM[33])
        musicEnabled = 0;

    // if (!saveRAM[39])
    //    _mm_storeu_si128((__m128i *)&saveRAM[39], _mm_load_si128((const __m128i *)&xmmword_8C670));
    // globalBoxRegion[0] = saveRAM[36];
    SetGameVolumes(saveRAM[33], saveRAM[34]);
    // CreateNativeObject(SegaSplash_Create, SegaSplash_Main);
    CreateNativeObject(RetroGameLoop_Create, RetroGameLoop_Main);
}
NativeEntity *CreateNativeObject(void (*objCreate)(void *objPtr), void (*objMain)(void *objPtr))
{
    if (!nativeEntityCount) {
        NativeEntity *entity = objectEntityBank;
        memset(objectEntityBank, 0, sizeof(NativeEntityBase));
        entity->createPtr   = objCreate;
        entity->mainPtr     = objMain;
        activeEntityList[0] = 0;
        nativeEntityCount++;
        if (entity->createPtr)
            entity->createPtr(entity);
        return entity;
    }
    else if (nativeEntityCount >= 0xFF) {
        // TODO
        return NULL;
    }
    else {
        NativeEntity *entity = objectEntityBank;
        int slot             = 0;
        while (entity->mainPtr) {
            ++entity;
            ++slot;
            if (slot >= NATIVEENTITY_COUNT)
                return entity;
        }
        memset(entity, 0, sizeof(NativeEntity));
        entity->slotID                        = slot;
        entity->objectID                      = nativeEntityCount;
        entity->createPtr                     = objCreate;
        entity->mainPtr                       = objMain;
        activeEntityList[nativeEntityCount++] = slot;
        if (entity->createPtr)
            entity->createPtr(entity);
        return entity;
    }
}
void RemoveNativeObject(NativeEntityBase *entity)
{
    for (int o = 0; o < nativeEntityCount; ++o) {
        if (objectEntityBank[o].createPtr == entity->createPtr && objectEntityBank[o].mainPtr == entity->mainPtr
            && objectEntityBank[o].slotID == entity->slotID && objectEntityBank[o].objectID == entity->objectID) {
            objectEntityBank[o].createPtr = nullptr;
            objectEntityBank[o].mainPtr   = nullptr;
            objectEntityBank[o].slotID    = 0;
            objectEntityBank[o].objectID  = 0;

            nativeEntityCount--;
            for (int i = o; i < nativeEntityCount; ++i) {
                objectEntityBank[i].createPtr = objectEntityBank[i + 1].createPtr;
                objectEntityBank[i].mainPtr   = objectEntityBank[i + 1].mainPtr;
                objectEntityBank[i].slotID    = objectEntityBank[i + 1].slotID;
                objectEntityBank[i].objectID  = objectEntityBank[i + 1].objectID;
            }
        }
    }

    /*if (nativeEntityCount <= 0) {
        objectRemoveFlag[entity->slotID] = 1;
    }
    else {
        memset(objectRemoveFlag, 0, nativeEntityCount);
        int slotStore                    = 0;
        objectRemoveFlag[entity->slotID] = 1;
        int curSlot                      = 0;
        do {
            if (!objectRemoveFlag[curSlot]) {
                if (curSlot != slotStore) {
                    int store                   = activeEntityList[curSlot];
                    objectRemoveFlag[slotStore] = 0;
                    activeEntityList[slotStore] = store;
                }
                ++slotStore;
            }
            ++curSlot;
        } while (curSlot != nativeEntityCount);
        nativeEntityCount = curSlot - 1;
    }*/
}
void ProcessNativeObjects()
{
    // ResetRenderStates();
    for (nativeEntityPos = 0; nativeEntityPos < nativeEntityCount; ++nativeEntityPos) {
        NativeEntity *entity = &objectEntityBank[activeEntityList[nativeEntityPos]];
        entity->mainPtr(entity);
    }
    // RenderScene();
}