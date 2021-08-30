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
    int lookPosX;
    int lookPosY;
    ushort typeGroup;
    byte type;
    byte propertyValue;
    byte priority;
    sbyte drawOrder;
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
extern byte objectRemoveFlag[NATIVEENTITY_COUNT];
extern NativeEntity objectEntityBank[NATIVEENTITY_COUNT];
extern int nativeEntityCount;

extern int nativeEntityCountBackup;
extern int backupEntityList[NATIVEENTITY_COUNT];
extern NativeEntity objectEntityBackup[NATIVEENTITY_COUNT];

extern int nativeEntityCountBackupS;
extern int backupEntityListS[NATIVEENTITY_COUNT];
extern NativeEntity objectEntityBackupS[NATIVEENTITY_COUNT];

//Game Objects
extern int objectEntityPos;
extern int curObjectType;
extern Entity objectEntityList[ENTITY_COUNT * 2];
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

void SetObjectTypeName(const char *objectName, int objectID);

extern int playerListPos;

void ProcessPlayerControl(Entity *player);

void InitNativeObjectSystem();
NativeEntity *CreateNativeObject(void (*objCreate)(void *objPtr), void (*objMain)(void *objPtr));
void RemoveNativeObject(NativeEntityBase *NativeEntry);
void ResetNativeObject(NativeEntityBase *obj, void (*objCreate)(void *objPtr), void (*objMain)(void *objPtr));
void ProcessNativeObjects();
inline void BackupNativeObjects() {
    memcpy(backupEntityList, activeEntityList, sizeof(activeEntityList));
    memcpy(objectEntityBackup, objectEntityBank, sizeof(objectEntityBank));
    nativeEntityCountBackup = nativeEntityCount;
}
inline void BackupNativeObjectsSettings() {
    memcpy(backupEntityListS, activeEntityList, sizeof(int) * NATIVEENTITY_COUNT);
    memcpy(objectEntityBackupS, objectEntityBank, sizeof(objectEntityBank));
    nativeEntityCountBackupS = nativeEntityCount;
}
void RestoreNativeObjects();
inline void RestoreNativeObjectsNoFade()
{
    memcpy(activeEntityList, backupEntityList, sizeof(activeEntityList));
    memcpy(objectEntityBank, objectEntityBackup, sizeof(objectEntityBank));
    nativeEntityCount = nativeEntityCountBackup;
}
inline void RestoreNativeObjectsSettings()
{
    memcpy(activeEntityList, backupEntityListS, sizeof(activeEntityList));
    memcpy(objectEntityBank, objectEntityBackupS, sizeof(objectEntityBank));
    nativeEntityCount = nativeEntityCountBackupS;
}
inline NativeEntity *GetNativeObject(uint objID)
{
    if (objID >= NATIVEENTITY_COUNT)
        return nullptr;
    else
        return &objectEntityBank[objID];
}

//Custom, used for cleaning purposes
inline void RemoveNativeObjectType(void (*objCreate)(void *objPtr), void (*objMain)(void *objPtr))
{
    for (int i = nativeEntityCount - 1; i >= 0; --i) {
        if (objectEntityBank[i].createPtr == objCreate && objectEntityBank[i].mainPtr == objMain) {
            RemoveNativeObject((NativeEntityBase *)&objectEntityBank[i]);
        }
    }
}
inline void ClearNativeObjects() {
    nativeEntityCount = 0;
    memset(objectEntityBank, 0, sizeof(NativeEntity) * NATIVEENTITY_COUNT);
}

#endif // !OBJECT_H
