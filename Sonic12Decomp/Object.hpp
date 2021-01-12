#ifndef OBJECT_H
#define OBJECT_H

#define NATIVEENTITY_COUNT (0x100)

#define ENTITY_COUNT (0x4A0)
#define TEMPENTITY_START (ENTITY_COUNT - 0x80)
#define OBJECT_COUNT (0x100)
#define TYPEGROUP_COUNT (0x103)

struct TypeGroupList {
    int entityRefs[ENTITY_COUNT];
    int listSize;
};

struct Entity {
    int XPos;
    int YPos;
    int XVelocity;
    int YVelocity;
    int speed;
    int values[48];
    int state;
    int angle;
    int scale;
    int rotation;
    int alpha;
    int animationTimer;
    int animationSpeed;
    int camOffsetX;
    int lookPos;
    ushort typeGroup;
    byte type;
    byte propertyValue;
    byte priority;
    byte drawOrder;
    byte direction;
    byte inkEffect;
    byte animation;
    byte prevAnimation;
    byte frame;
    byte collisionMode;
    byte collisionPlane;
    sbyte controlMode;
    byte controlLock;
    byte pushing;
    byte visible;
    byte tileCollisions;
    byte objectInteractions;
    byte gravity;
    byte left;
    byte right;
    byte up;
    byte down;
    byte jumpPress;
    byte jumpHold;
    byte trackScroll;
    byte flailing[5];
};

struct NativeEntityBase {
    void (*createPtr)(void *objPtr);
    void (*mainPtr)(void *objPtr);
    int slotID;
    int objectID;
};

struct NativeEntity {
    void (*createPtr)(void *objPtr);
    void (*mainPtr)(void *objPtr);
    int slotID;
    int objectID;
    byte extra[0x400];
};

enum ObjectTypes {
    OBJ_TYPE_BLANKOBJECT = 0 //0 is always blank obj
};

enum ObjectPriority {
    PRIORITY_ACTIVE_BOUNDS,
    PRIORITY_ACTIVE,
    PRIORITY_ACTIVE_PAUSED,
    PRIORITY_ACTIVE_XBOUNDS,
    PRIORITY_ACTIVE_XBOUNDS_REMOVE,
    PRIORITY_INACTIVE,
    PRIORITY_ACTIVE_BOUNDS_SMALL,
    PRIORITY_ACTIVE2
};

//Native Objects
extern int nativeEntityPos;

extern int activeEntityList[NATIVEENTITY_COUNT];
extern int objectRemoveFlag[NATIVEENTITY_COUNT];
extern NativeEntity objectEntityBank[NATIVEENTITY_COUNT];
extern int nativeEntityCount;

extern int nativeEntityCountBackup;
extern int backupEntityList[NATIVEENTITY_COUNT];
extern NativeEntity objectEntityBackup[NATIVEENTITY_COUNT];

extern int nativeEntityCountBackupS;
extern int backupEntityListS[NATIVEENTITY_COUNT];
extern NativeEntity objectEntityBackupS[NATIVEENTITY_COUNT];

//Game Objects
extern int objectLoop;
extern int curObjectType;
extern Entity objectEntityList[ENTITY_COUNT];
extern int processObjectFlag[ENTITY_COUNT];
extern TypeGroupList objectTypeGroupList[TYPEGROUP_COUNT];

extern char typeNames[OBJECT_COUNT][0x40];

extern int OBJECT_BORDER_X1;
extern int OBJECT_BORDER_X2;
extern int OBJECT_BORDER_X3;
extern int OBJECT_BORDER_X4;
extern const int OBJECT_BORDER_Y1;
extern const int OBJECT_BORDER_Y2;
extern const int OBJECT_BORDER_Y3;
extern const int OBJECT_BORDER_Y4;

void ProcessStartupObjects();
void ProcessObjects();
void ProcessPausedObjects();
void ProcessFrozenObjects();
void Process2PObjects();

#if !RSDK_DEBUG
inline void SetObjectTypeName(const char *objectName, int objectID)
{
    int objNameID  = 0;
    int typeNameID = 0;
    while (objectName[objNameID]) {
        if (objectName[objNameID] != ' ')
            typeNames[objectID][typeNameID++] = objectName[objNameID];
        ++objNameID;
    }
    typeNames[objectID][typeNameID] = 0;
}
#else
void SetObjectTypeName(const char *objectName, int objectID);
#endif

extern int playerListPos;

void ProcessPlayerControl(Entity *player);

void InitNativeObjectSystem();
NativeEntity *CreateNativeObject(void (*objCreate)(void *objPtr), void (*objMain)(void *objPtr));
void RemoveNativeObject(NativeEntityBase *NativeEntry);
void ProcessNativeObjects();
inline void BackupNativeObjects() {
    memcpy(backupEntityList, activeEntityList, sizeof(int) * NATIVEENTITY_COUNT);
    memcpy(objectEntityBackup, objectEntityBank, sizeof(NativeEntity) * NATIVEENTITY_COUNT);
    nativeEntityCountBackup = nativeEntityCount;
}
inline void BackupNativeObjectsSettings() {
    memcpy(backupEntityListS, activeEntityList, sizeof(int) * NATIVEENTITY_COUNT);
    memcpy(objectEntityBackupS, objectEntityBank, sizeof(NativeEntity) * NATIVEENTITY_COUNT);
    nativeEntityCountBackupS = nativeEntityCount;
}
inline void RestoreNativeObjects()
{
    memcpy(activeEntityList, backupEntityList, sizeof(int) * NATIVEENTITY_COUNT);
    nativeEntityCount = nativeEntityCountBackup;
    memcpy(objectEntityBank, objectEntityBackup, sizeof(NativeEntity) * NATIVEENTITY_COUNT);

    //ptr = CreateNativeObject(FadeScreen_Create, FadeScreen_Main);
    //ptr + 16 = 0;
}
inline void RestoreNativeObjectsNoFade()
{
    memcpy(activeEntityList, backupEntityList, sizeof(int) * NATIVEENTITY_COUNT);
    nativeEntityCount = nativeEntityCountBackup;
    memcpy(objectEntityBank, objectEntityBackup, sizeof(NativeEntity) * NATIVEENTITY_COUNT);
}
inline void RestoreNativeObjectsSettings()
{
    memcpy(activeEntityList, backupEntityListS, sizeof(int) * NATIVEENTITY_COUNT);
    nativeEntityCount = nativeEntityCountBackupS;
    memcpy(objectEntityBank, objectEntityBackupS, sizeof(NativeEntity) * NATIVEENTITY_COUNT);
}
inline void GetNativeObject(NativeEntity *obj, void (*newCreate)(void *objPtr), void (*newMain)(void *objPtr))
{
    int slotID = obj->slotID;
    int objID  = obj->objectID;
    memset(&objectEntityBank[slotID], 0, sizeof(NativeEntity));
    obj->slotID   = slotID;
    obj->mainPtr     = newMain;
    obj->createPtr   = newCreate;
    obj->objectID = objID;
    if (obj->createPtr)
        obj->createPtr(obj);
}
inline NativeEntity *GetNativeObject(uint objID)
{
    if (objID > 0xFF)
        return nullptr;
    else
        return &objectEntityBank[objID];
}
inline void ClearNativeObjects() {
    nativeEntityCount = 0;
    memset(objectEntityBank, 0, sizeof(NativeEntity) * NATIVEENTITY_COUNT);
}

#endif // !OBJECT_H
