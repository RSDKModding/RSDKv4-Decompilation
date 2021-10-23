#ifndef COLLISION_H
#define COLLISION_H

enum CollisionSidess {
    CSIDE_FLOOR = 0,
    CSIDE_LWALL = 1,
    CSIDE_RWALL = 2,
    CSIDE_ROOF  = 3,
};

enum CollisionModes {
    CMODE_FLOOR = 0,
    CMODE_LWALL = 1,
    CMODE_ROOF  = 2,
    CMODE_RWALL = 3,
};

enum CollisionSolidity {
    SOLID_ALL        = 0,
    SOLID_TOP        = 1,
    SOLID_LRB        = 2,
    SOLID_NONE       = 3,
    SOLID_TOP_NOGRIP = 4,
};

enum ObjectCollisionTypes {
    C_TOUCH    = 0,
    C_BOX      = 1,
    C_BOX2     = 2,
    C_PLATFORM = 3,
};

struct CollisionSensor {
    int xpos;
    int ypos;
    int angle;
    bool collided;
};

#if !RETRO_USE_ORIGINAL_CODE
#define DEBUG_HITBOX_MAX (0x400)

struct DebugHitboxInfo {
    byte type;
    byte collision;
    short left;
    short top;
    short right;
    short bottom;
    int xpos;
    int ypos;
    Entity *entity;
};

enum DebugHitboxTypes { H_TYPE_TOUCH, H_TYPE_BOX, H_TYPE_PLAT };

extern bool showHitboxes;
extern int debugHitboxCount;
extern DebugHitboxInfo debugHitboxList[DEBUG_HITBOX_MAX];

int addDebugHitbox(byte type, Entity *entity, int left, int top, int right, int bottom);
#endif

extern int collisionLeft;
extern int collisionTop;
extern int collisionRight;
extern int collisionBottom;

extern int collisionTolerance;

extern CollisionSensor sensors[7];

void FindFloorPosition(Entity *player, CollisionSensor *sensor, int startYPos);
void FindLWallPosition(Entity *player, CollisionSensor *sensor, int startXPos);
void FindRoofPosition(Entity *player, CollisionSensor *sensor, int startYPos);
void FindRWallPosition(Entity *player, CollisionSensor *sensor, int startXPos);

void FloorCollision(Entity *player, CollisionSensor *sensor);
void LWallCollision(Entity *player, CollisionSensor *sensor);
void RoofCollision(Entity *player, CollisionSensor *sensor);
void RWallCollision(Entity *player, CollisionSensor *sensor);

void SetPathGripSensors(Entity *player);
void ProcessPathGrip(Entity *player);
void ProcessAirCollision(Entity *player);

void ProcessTileCollisions(Entity *player);

void TouchCollision(Entity *thisEntity, int thisLeft, int thisTop, int thisRight, int thisBottom, Entity *otherEntity, int otherLeft, int otherTop,
                    int otherRight, int otherBottom);
void BoxCollision(Entity *thisEntity, int thisLeft, int thisTop, int thisRight, int thisBottom, Entity *otherEntity, int otherLeft, int otherTop,
                  int otherRight, int otherBottom); // Standard
void BoxCollision2(Entity *thisEntity, int thisLeft, int thisTop, int thisRight, int thisBottom, Entity *otherEntity, int otherLeft, int otherTop,
                   int otherRight, int otherBottom); // Updated (?)
void PlatformCollision(Entity *thisEntity, int thisLeft, int thisTop, int thisRight, int thisBottom, Entity *otherEntity, int otherLeft, int otherTop,
                       int otherRight, int otherBottom);

void ObjectFloorCollision(int xOffset, int yOffset, int cPath);
void ObjectLWallCollision(int xOffset, int yOffset, int cPath);
void ObjectRoofCollision(int xOffset, int yOffset, int cPath);
void ObjectRWallCollision(int xOffset, int yOffset, int cPath);

void ObjectFloorGrip(int xOffset, int yOffset, int cPath);
void ObjectLWallGrip(int xOffset, int yOffset, int cPath);
void ObjectRoofGrip(int xOffset, int yOffset, int cPath);
void ObjectRWallGrip(int xOffset, int yOffset, int cPath);

#endif // !COLLISION_H
