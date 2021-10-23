#include "RetroEngine.hpp"

// Native Objects
int nativeEntityPos;

int activeEntityList[NATIVEENTITY_COUNT];
byte objectRemoveFlag[NATIVEENTITY_COUNT];
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
Entity objectEntityList[ENTITY_COUNT * 2]; //"regular" list & "storage" list
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

    memset(foreachStack, -1, sizeof(foreachStack));
    memset(jumpTableStack, 0, sizeof(jumpTableStack));

    for (int i = 0; i < OBJECT_COUNT; ++i) {
        ObjectScript *scriptInfo    = &objectScriptList[i];
        objectEntityPos             = TEMPENTITY_START;
        curObjectType               = i;
        scriptInfo->frameListOffset = scriptFrameCount;
        scriptInfo->spriteSheetID   = 0;
        entity->type                = i;

        if (scriptData[scriptInfo->eventStartup.scriptCodePtr] > 0)
            ProcessScript(scriptInfo->eventStartup.scriptCodePtr, scriptInfo->eventStartup.jumpTablePtr, EVENT_SETUP);
        scriptInfo->frameCount = scriptFrameCount - scriptInfo->frameListOffset;
    }
    entity->type  = 0;
    curObjectType = 0;
}

void ProcessObjects()
{
    for (int i = 0; i < DRAWLAYER_COUNT; ++i) drawListEntries[i].listSize = 0;

    for (objectEntityPos = 0; objectEntityPos < ENTITY_COUNT; ++objectEntityPos) {
        processObjectFlag[objectEntityPos] = false;
        int x = 0, y = 0;
        Entity *entity = &objectEntityList[objectEntityPos];
        x              = entity->xpos >> 16;
        y              = entity->ypos >> 16;

        switch (entity->priority) {
            case PRIORITY_ACTIVE_BOUNDS:
                processObjectFlag[objectEntityPos] = x > xScrollOffset - OBJECT_BORDER_X1 && x < xScrollOffset + OBJECT_BORDER_X2
                                                     && y > yScrollOffset - OBJECT_BORDER_Y1 && y < yScrollOffset + OBJECT_BORDER_Y2;
                break;
            case PRIORITY_ACTIVE:
            case PRIORITY_ACTIVE_PAUSED:
            case PRIORITY_ACTIVE_2P_UNKNOWN: processObjectFlag[objectEntityPos] = true; break;
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
            // Custom Group
            if (entity->groupID >= OBJECT_COUNT) {
                TypeGroupList *listCustom                      = &objectTypeGroupList[objectEntityList[objectEntityPos].groupID];
                listCustom->entityRefs[listCustom->listSize++] = objectEntityPos;
            }
            // Type-Specific list
            TypeGroupList *listType                    = &objectTypeGroupList[objectEntityList[objectEntityPos].type];
            listType->entityRefs[listType->listSize++] = objectEntityPos;

            // All Entities list
            TypeGroupList *listAll                   = &objectTypeGroupList[GROUP_ALL];
            listAll->entityRefs[listAll->listSize++] = objectEntityPos;
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
        x              = entity->xpos >> 16;
        y              = entity->ypos >> 16;

        switch (entity->priority) {
            case PRIORITY_ACTIVE_BOUNDS:
                processObjectFlag[objectEntityPos] = x > xScrollOffset - OBJECT_BORDER_X1 && x < xScrollOffset + OBJECT_BORDER_X2
                                                     && y > yScrollOffset - OBJECT_BORDER_Y1 && y < yScrollOffset + OBJECT_BORDER_Y2;
                break;
            case PRIORITY_ACTIVE:
            case PRIORITY_ACTIVE_PAUSED:
            case PRIORITY_ACTIVE_2P_UNKNOWN: processObjectFlag[objectEntityPos] = true; break;
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
            // Custom Group
            if (entity->groupID >= OBJECT_COUNT) {
                TypeGroupList *listCustom                      = &objectTypeGroupList[objectEntityList[objectEntityPos].groupID];
                listCustom->entityRefs[listCustom->listSize++] = objectEntityPos;
            }
            // Type-Specific list
            TypeGroupList *listType                    = &objectTypeGroupList[objectEntityList[objectEntityPos].type];
            listType->entityRefs[listType->listSize++] = objectEntityPos;

            // All Entities list
            TypeGroupList *listAll                   = &objectTypeGroupList[GROUP_ALL];
            listAll->entityRefs[listAll->listSize++] = objectEntityPos;
        }
    }
}
void Process2PObjects()
{
    for (int i = 0; i < DRAWLAYER_COUNT; ++i) drawListEntries[i].listSize = 0;

    Entity *entityP1 = &objectEntityList[0];
    int XPosP1       = entityP1->xpos;
    int YPosP1       = entityP1->ypos;
    Entity *entityP2 = &objectEntityList[1];
    int XPosP2       = entityP2->xpos;
    int YPosP2       = entityP2->ypos;

    int boundX1 = -(0x200 << 16);
    int boundX2 = (0x200 << 16);
    int boundX3 = -(0x180 << 16);
    int boundX4 = (0x180 << 16);

    int boundY1 = -(0x180 << 16);
    int boundY2 = (0x180 << 16);
    int boundY3 = -(0x100 << 16);
    int boundY4 = (0x100 << 16);

    for (objectEntityPos = 0; objectEntityPos < ENTITY_COUNT; ++objectEntityPos) {
        processObjectFlag[objectEntityPos] = false;
        int x = 0, y = 0;

        Entity *entity = &objectEntityList[objectEntityPos];
        x              = entity->xpos;
        y              = entity->ypos;
        switch (entity->priority) {
            case PRIORITY_ACTIVE_BOUNDS:
                processObjectFlag[objectEntityPos] = x > XPosP1 + boundX1 && x < XPosP1 + boundX2 && y > YPosP1 + boundY1 && y < YPosP1 + boundY2;
                if (!processObjectFlag[objectEntityPos]) {
                    processObjectFlag[objectEntityPos] = x > XPosP2 + boundX1 && x < XPosP2 + boundX2 && y > YPosP2 + boundY1 && y < YPosP2 + boundY2;
                }
                break;
            case PRIORITY_ACTIVE:
            case PRIORITY_ACTIVE_PAUSED:
            case PRIORITY_ACTIVE_2P_UNKNOWN: processObjectFlag[objectEntityPos] = true; break;
            case PRIORITY_ACTIVE_XBOUNDS:
                processObjectFlag[objectEntityPos] = x > XPosP1 + boundX1 && x < XPosP1 + boundX2;
                if (!processObjectFlag[objectEntityPos]) {
                    processObjectFlag[objectEntityPos] = x > XPosP2 + boundX1 && x < XPosP2 + boundX2;
                }
                break;
            case PRIORITY_ACTIVE_XBOUNDS_REMOVE:
                processObjectFlag[objectEntityPos] = x > XPosP1 + boundX1 && x < XPosP1 + boundX2;
                if (!processObjectFlag[objectEntityPos]) {
                    processObjectFlag[objectEntityPos] = x > XPosP2 + boundX1 && x < XPosP2 + boundX2;
                }

                if (!processObjectFlag[objectEntityPos])
                    entity->type = OBJ_TYPE_BLANKOBJECT;
                break;
            case PRIORITY_INACTIVE: processObjectFlag[objectEntityPos] = false; break;
            case PRIORITY_ACTIVE_BOUNDS_SMALL:
                processObjectFlag[objectEntityPos] = x > XPosP1 + boundX3 && x < XPosP1 + boundX4 && y > YPosP1 + boundY3 && y < YPosP1 + boundY4;
                if (!processObjectFlag[objectEntityPos]) {
                    processObjectFlag[objectEntityPos] = x > XPosP2 + boundX3 && x < XPosP2 + boundX4 && y > YPosP2 + boundY3 && y < YPosP2 + boundY4;
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
            // Custom Group
            if (entity->groupID >= OBJECT_COUNT) {
                TypeGroupList *listCustom                      = &objectTypeGroupList[objectEntityList[objectEntityPos].groupID];
                listCustom->entityRefs[listCustom->listSize++] = objectEntityPos;
            }
            // Type-Specific list
            TypeGroupList *listType                    = &objectTypeGroupList[objectEntityList[objectEntityPos].type];
            listType->entityRefs[listType->listSize++] = objectEntityPos;

            // All Entities list
            TypeGroupList *listAll                   = &objectTypeGroupList[GROUP_ALL];
            listAll->entityRefs[listAll->listSize++] = objectEntityPos;
        }
    }
}

void SetObjectTypeName(const char *objectName, int objectID)
{
    int objPos  = 0;
    int typePos = 0;
    while (objectName[objPos]) {
        if (objectName[objPos] != ' ')
            typeNames[objectID][typePos++] = objectName[objPos];
        ++objPos;
    }
    typeNames[objectID][typePos] = 0;
    printLog("Set Object (%d) name to: %s", objectID, objectName);
}

void ProcessObjectControl(Entity *player)
{
    if (!player->controlMode) {
        player->up   = inputDown.up;
        player->down = inputDown.down;
        if (!inputDown.left || !inputDown.right) {
            player->left  = inputDown.left;
            player->right = inputDown.right;
        }
        else {
            player->left  = false;
            player->right = false;
        }
        player->jumpHold  = inputDown.C || inputDown.B || inputDown.A;
        player->jumpPress = inputPress.C || inputPress.B || inputPress.A;
    }
}

void InitNativeObjectSystem()
{
    InitLocalizedStrings();

    nativeEntityCount = 0;
    memset(activeEntityList, 0, sizeof(activeEntityList));
    memset(objectRemoveFlag, 0, sizeof(objectRemoveFlag));
    memset(objectEntityBank, 0, sizeof(objectEntityBank));

    nativeEntityCountBackup = 0;
    memset(backupEntityList, 0, sizeof(backupEntityList));
    memset(objectEntityBackup, 0, sizeof(objectEntityBackup));

    nativeEntityCountBackupS = 0;
    memset(backupEntityListS, 0, sizeof(backupEntityListS));
    memset(objectEntityBackupS, 0, sizeof(objectEntityBackupS));

    ReadSaveRAMData();

    SaveGame *saveGame = (SaveGame *)saveRAM;
    if (!saveGame->saveInitialized) {
        saveGame->saveInitialized = true;
        saveGame->musVolume       = MAX_VOLUME;
        saveGame->sfxVolume       = MAX_VOLUME;
        saveGame->spindashEnabled = true;
        saveGame->boxRegion       = 0;
        saveGame->vDPadSize       = 64;
        saveGame->vDPadOpacity    = 160;
        saveGame->vDPadX_Move     = 56;
        saveGame->vDPadY_Move     = 184;
        saveGame->vDPadX_Jump     = -56;
        saveGame->vDPadY_Jump     = 188;
        saveGame->tailsUnlocked   = Engine.gameType != GAME_SONIC1;
        saveGame->knuxUnlocked    = Engine.gameType != GAME_SONIC1;
        saveGame->unlockedActs    = 0;
        WriteSaveRAMData();
    }
    saveGame->musVolume = bgmVolume;
    saveGame->sfxVolume = sfxVolume;

    if (!saveGame->musVolume)
        musicEnabled = false;

    if (!saveGame->vDPadX_Move) {
        saveGame->vDPadX_Move = 60;
        saveGame->vDPadY_Move = 176;
        saveGame->vDPadX_Jump = -56;
        saveGame->vDPadY_Jump = 180;
    }

    Engine.globalBoxRegion = saveGame->boxRegion;
    SetGameVolumes(saveGame->musVolume, saveGame->sfxVolume);
    if (skipStartMenu) {
        CREATE_ENTITY(RetroGameLoop);
        if (Engine.gameDeviceType == RETRO_MOBILE)
            CREATE_ENTITY(VirtualDPad);
    }
    else
        CREATE_ENTITY(SegaSplash);
}
NativeEntity *CreateNativeObject(void (*create)(void *objPtr), void (*main)(void *objPtr))
{
    if (!nativeEntityCount) {
        memset(objectEntityBank, 0, sizeof(objectEntityBank));
        NativeEntity *entity = &objectEntityBank[0];
        entity->createPtr    = create;
        entity->mainPtr      = main;
        activeEntityList[0]  = 0;
        nativeEntityCount++;
        if (entity->createPtr)
            entity->createPtr(entity);
        return entity;
    }
    else if (nativeEntityCount >= NATIVEENTITY_COUNT) {
        // TODO
        return NULL;
    }
    else {
        int slot = 0;
        for (; slot < NATIVEENTITY_COUNT; ++slot) {
            if (!objectEntityBank[slot].mainPtr)
                break;
        }
        NativeEntity *entity = &objectEntityBank[slot];
        memset(entity, 0, sizeof(NativeEntity));
        entity->slotID                        = slot;
        entity->objectID                      = nativeEntityCount;
        entity->createPtr                     = create;
        entity->mainPtr                       = main;
        activeEntityList[nativeEntityCount++] = slot;
        if (entity->createPtr)
            entity->createPtr(entity);
        return entity;
    }
}
void RemoveNativeObject(NativeEntityBase *entity)
{
    if (nativeEntityCount <= 0) {
        objectRemoveFlag[entity->slotID] = true;
    }
    else {
        memset(objectRemoveFlag, 0, nativeEntityCount);
        int slotStore                    = 0;
        objectRemoveFlag[entity->slotID] = true;
        int s                            = 0;
        for (; s < nativeEntityCount; ++s) {
            if (!objectRemoveFlag[s]) {
                if (s != slotStore) {
                    int store                   = activeEntityList[s];
                    objectRemoveFlag[slotStore] = false;
                    activeEntityList[slotStore] = store;
                }
                ++slotStore;
            }
        }
        nativeEntityCount = s - 1;
    }
}
void ResetNativeObject(NativeEntityBase *obj, void (*create)(void *objPtr), void (*main)(void *objPtr))
{
    int slotID = obj->slotID;
    int objID  = obj->objectID;
    memset(&objectEntityBank[slotID], 0, sizeof(NativeEntity));
    obj->slotID    = slotID;
    obj->mainPtr   = main;
    obj->createPtr = create;
    obj->objectID  = objID;
    if (create)
        create(obj);
}
void ProcessNativeObjects()
{
    ResetRenderStates();
    for (nativeEntityPos = 0; nativeEntityPos < nativeEntityCount; ++nativeEntityPos) {
        NativeEntity *entity = &objectEntityBank[activeEntityList[nativeEntityPos]];
        entity->mainPtr(entity);
    }
    RenderScene();
}

void RestoreNativeObjects()
{
    memcpy(activeEntityList, backupEntityList, sizeof(activeEntityList));
    memcpy(objectEntityBank, objectEntityBackup, sizeof(objectEntityBank));
#if !RETRO_USE_ORIGINAL_CODE
    if (!nativeEntityCountBackup) {
        CREATE_ENTITY(SegaSplash);
        nativeEntityCountBackup = 1;
    }
#endif
    nativeEntityCount = nativeEntityCountBackup;

    CREATE_ENTITY(FadeScreen)->state = FADESCREEN_STATE_MENUFADEIN;
}

void RestoreNativeObjectsNoFade()
{
    memcpy(activeEntityList, backupEntityList, sizeof(activeEntityList));
    memcpy(objectEntityBank, objectEntityBackup, sizeof(objectEntityBank));
#if !RETRO_USE_ORIGINAL_CODE
    if (!nativeEntityCountBackup) {
        CREATE_ENTITY(SegaSplash);
        nativeEntityCountBackup = 1;
    }
#endif
    nativeEntityCount = nativeEntityCountBackup;
}
void RestoreNativeObjectsSettings()
{
    memcpy(activeEntityList, backupEntityListS, sizeof(activeEntityList));
    memcpy(objectEntityBank, objectEntityBackupS, sizeof(objectEntityBank));
#if !RETRO_USE_ORIGINAL_CODE
    if (!nativeEntityCountBackupS) {
        CREATE_ENTITY(SegaSplash);
        nativeEntityCountBackupS = 1;
    }
#endif
    nativeEntityCount = nativeEntityCountBackupS;
}
