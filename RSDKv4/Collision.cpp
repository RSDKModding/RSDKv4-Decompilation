#include "RetroEngine.hpp"

int collisionLeft   = 0;
int collisionTop    = 0;
int collisionRight  = 0;
int collisionBottom = 0;

int collisionTolerance = 0;

CollisionSensor sensors[7];

inline Hitbox *getHitbox(Entity *entity)
{
    AnimationFile *thisAnim = objectScriptList[entity->type].animFile;
    return &hitboxList[thisAnim->hitboxListOffset
                       + animFrames[animationList[thisAnim->aniListOffset + entity->animation].frameListOffset + entity->frame].hitboxID];
}

void FindFloorPosition(Entity *player, CollisionSensor *sensor, int startY)
{
    int c     = 0;
    int angle = sensor->angle;
    int tsm1  = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = sensor->XPos >> 16;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = (sensor->YPos >> 16) - TILE_SIZE + i;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile += tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] != SOLID_LRB
                    && tiles128x128.collisionFlags[player->collisionPlane][tile] != SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (XPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].floorMasks[c] >= 0x40)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF;
                            break;
                        }
                        case FLIP_X: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].floorMasks[c] >= 0x40)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF);
                            break;
                        }
                        case FLIP_Y: {
                            c = (XPos & 15) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].roofMasks[c] <= -0x40)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = (byte)(0x180 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24));
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].roofMasks[c] <= -0x40)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle = 0x100 - (byte)(0x180 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24));
                            break;
                        }
                    }
                }

                if (sensor->collided) {
                    if (sensor->angle < 0)
                        sensor->angle += 0x100;

                    if (sensor->angle >= 0x100)
                        sensor->angle -= 0x100;

                    if ((abs(sensor->angle - angle) > 0x20) && (abs(sensor->angle - 0x100 - angle) > 0x20)
                        && (abs(sensor->angle + 0x100 - angle) > 0x20)) {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                        sensor->angle    = angle;
                        i                = TILE_SIZE * 3;
                    }
                    else if (sensor->YPos - startY > collisionTolerance || sensor->YPos - startY < -collisionTolerance) {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                    }
                }
            }
        }
    }
}
void FindLWallPosition(Entity *player, CollisionSensor *sensor, int startX)
{
    int c     = 0;
    int angle = sensor->angle;
    int tsm1  = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = (sensor->XPos >> 16) - TILE_SIZE + i;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = sensor->YPos >> 16;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile      = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile          = tile + tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] < SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].lWallMasks[c] >= 0x40)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            sensor->angle    = ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF00) >> 8);
                            break;
                        }
                        case FLIP_X: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].rWallMasks[c] <= -0x40)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF0000) >> 16);
                            break;
                        }
                        case FLIP_Y: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].lWallMasks[c] >= 0x40)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            sensor->angle    = (byte)(0x180 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF00) >> 8));
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].rWallMasks[c] <= -0x40)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - (byte)(0x180 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF0000) >> 16));
                            break;
                        }
                    }
                }
                if (sensor->collided) {
                    if (sensor->angle < 0)
                        sensor->angle += 0x100;

                    if (sensor->angle >= 0x100)
                        sensor->angle -= 0x100;

                    if (abs(angle - sensor->angle) > 0x20) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                        sensor->angle    = angle;
                        i                = TILE_SIZE * 3;
                    }
                    else if (sensor->XPos - startX > collisionTolerance || sensor->XPos - startX < -collisionTolerance) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                    }
                }
            }
        }
    }
}
void FindRoofPosition(Entity *player, CollisionSensor *sensor, int startY)
{
    int c     = 0;
    int angle = sensor->angle;
    int tsm1  = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = sensor->XPos >> 16;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = (sensor->YPos >> 16) + TILE_SIZE - i;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile      = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile          = tile + tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] < SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (XPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].roofMasks[c] <= -0x40)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24;
                            break;
                        }
                        case FLIP_X: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].roofMasks[c] <= -0x40)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24);
                            break;
                        }
                        case FLIP_Y: {
                            c = (XPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].floorMasks[c] >= 0x40)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = (byte)(0x180 - (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF));
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].floorMasks[c] >= 0x40)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - (byte)(0x180 - (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF));
                            break;
                        }
                    }
                }

                if (sensor->collided) {
                    if (sensor->angle < 0)
                        sensor->angle += 0x100;

                    if (sensor->angle >= 0x100)
                        sensor->angle -= 0x100;

                    if (abs(sensor->angle - angle) <= 0x20) {
                        if (sensor->YPos - startY > collisionTolerance || sensor->YPos - startY < -collisionTolerance) {
                            sensor->YPos     = startY << 16;
                            sensor->collided = false;
                        }
                    }
                    else {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                        sensor->angle    = angle;
                        i                = TILE_SIZE * 3;
                    }
                }
            }
        }
    }
}
void FindRWallPosition(Entity *player, CollisionSensor *sensor, int startX)
{
    int c;
    int angle = sensor->angle;
    int tsm1  = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = (sensor->XPos >> 16) + TILE_SIZE - i;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = sensor->YPos >> 16;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile      = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile          = tile + tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] < SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].rWallMasks[c] <= -0x40)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            sensor->angle    = (byte)((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF0000) >> 16);
                            break;
                        }
                        case FLIP_X: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].lWallMasks[c] >= 0x40)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF00) >> 8);
                            break;
                        }
                        case FLIP_Y: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].rWallMasks[c] <= -0x40)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            sensor->angle    = (byte)(0x180 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF0000) >> 16));
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].lWallMasks[c] >= 0x40)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - (byte)(0x180 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF00) >> 8));
                            break;
                        }
                    }
                }
                if (sensor->collided) {
                    if (sensor->angle < 0)
                        sensor->angle += 0x100;

                    if (sensor->angle >= 0x100)
                        sensor->angle -= 0x100;

                    if (abs(sensor->angle - angle) > 0x20) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                        sensor->angle    = angle;
                        i                = TILE_SIZE * 3;
                    }
                    else if (sensor->XPos - startX > collisionTolerance || sensor->XPos - startX < -collisionTolerance) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                    }
                }
            }
        }
    }
}

void FloorCollision(Entity *player, CollisionSensor *sensor)
{
    int c;
    int startY = sensor->YPos >> 16;
    int tsm1   = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = sensor->XPos >> 16;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = (sensor->YPos >> 16) - TILE_SIZE + i;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile += tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] != SOLID_LRB
                    && tiles128x128.collisionFlags[player->collisionPlane][tile] != SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) <= collisionMasks[player->collisionPlane].floorMasks[c] - TILE_SIZE + i
                                || collisionMasks[player->collisionPlane].floorMasks[c] >= tsm1)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF;
                            break;
                        }
                        case FLIP_X: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) <= collisionMasks[player->collisionPlane].floorMasks[c] - TILE_SIZE + i
                                || collisionMasks[player->collisionPlane].floorMasks[c] >= tsm1)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF);
                            break;
                        }
                        case FLIP_Y: {
                            c = (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) <= tsm1 - collisionMasks[player->collisionPlane].roofMasks[c] - TILE_SIZE + i)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            byte cAngle      = (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24;
                            sensor->angle    = (byte)(0x180 - cAngle);
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) <= tsm1 - collisionMasks[player->collisionPlane].roofMasks[c] - TILE_SIZE + i)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle = 0x100 - (byte)(0x180 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24));
                            break;
                        }
                    }
                }

                if (sensor->collided) {
                    if (sensor->angle < 0)
                        sensor->angle += 0x100;

                    if (sensor->angle >= 0x100)
                        sensor->angle -= 0x100;

                    if (sensor->YPos - startY > (TILE_SIZE - 2)) {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                    }
                    else if (sensor->YPos - startY < -(TILE_SIZE + 1)) {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                    }
                }
            }
        }
    }
}
void LWallCollision(Entity *player, CollisionSensor *sensor)
{
    int c;
    int startX = sensor->XPos >> 16;
    int tsm1   = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = (sensor->XPos >> 16) - TILE_SIZE + i;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = sensor->YPos >> 16;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile += tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] != SOLID_TOP
                    && tiles128x128.collisionFlags[player->collisionPlane][tile] < SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) <= collisionMasks[player->collisionPlane].lWallMasks[c] - TILE_SIZE + i)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                        case FLIP_X: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) <= tsm1 - collisionMasks[player->collisionPlane].rWallMasks[c] - TILE_SIZE + i)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                        case FLIP_Y: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) <= collisionMasks[player->collisionPlane].lWallMasks[c] - TILE_SIZE + i)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) <= tsm1 - collisionMasks[player->collisionPlane].rWallMasks[c] - TILE_SIZE + i)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                    }
                }

                if (sensor->collided) {
                    if (sensor->XPos - startX > tsm1) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                    }
                    else if (sensor->XPos - startX < -tsm1) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                    }
                }
            }
        }
    }
}
void RoofCollision(Entity *player, CollisionSensor *sensor)
{
    int c;
    int startY = sensor->YPos >> 16;
    int tsm1   = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = sensor->XPos >> 16;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = (sensor->YPos >> 16) + TILE_SIZE - i;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile += tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] != SOLID_TOP
                    && tiles128x128.collisionFlags[player->collisionPlane][tile] < SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) >= collisionMasks[player->collisionPlane].roofMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24);
                            break;
                        }
                        case FLIP_X: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) >= collisionMasks[player->collisionPlane].roofMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24);
                            break;
                        }
                        case FLIP_Y: {
                            c = (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) >= tsm1 - collisionMasks[player->collisionPlane].floorMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x180 - (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF);
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) >= tsm1 - collisionMasks[player->collisionPlane].floorMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - (byte)(0x180 - (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF));
                            break;
                        }
                    }
                }

                if (sensor->collided) {
                    if (sensor->angle < 0)
                        sensor->angle += 0x100;

                    if (sensor->angle >= 0x100)
                        sensor->angle -= 0x100;

                    if (sensor->YPos - startY > (tsm1 - 1)) {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                    }
                    else if (sensor->YPos - startY < -(tsm1 - 1)) {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                    }
                }
            }
        }
    }
}
void RWallCollision(Entity *player, CollisionSensor *sensor)
{
    int c;
    int startX = sensor->XPos >> 16;
    int tsm1   = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = (sensor->XPos >> 16) + TILE_SIZE - i;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = sensor->YPos >> 16;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile += tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] != SOLID_TOP
                    && tiles128x128.collisionFlags[player->collisionPlane][tile] < SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) >= collisionMasks[player->collisionPlane].rWallMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                        case FLIP_X: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) >= tsm1 - collisionMasks[player->collisionPlane].lWallMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                        case FLIP_Y: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) >= collisionMasks[player->collisionPlane].rWallMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) >= tsm1 - collisionMasks[player->collisionPlane].lWallMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                    }
                }

                if (sensor->collided) {
                    if (sensor->XPos - startX > tsm1) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                    }
                    else if (sensor->XPos - startX < -tsm1) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                    }
                }
            }
        }
    }
}

void ProcessAirCollision(Entity *entity)
{
    Hitbox *playerHitbox = getHitbox(entity);
    collisionLeft        = playerHitbox->left[0];
    collisionTop         = playerHitbox->top[0];
    collisionRight       = playerHitbox->right[0];
    collisionBottom      = playerHitbox->bottom[0];

    byte movingDown  = 0;
    byte movingUp    = 0;
    byte movingLeft  = 0;
    byte movingRight = 0;

    if (entity->XVelocity < 0) {
        movingRight = 0;
    }
    else {
        movingRight         = 1;
        sensors[0].YPos     = entity->YPos + 0x40000;
        sensors[0].collided = false;
        sensors[0].XPos     = entity->XPos + (collisionRight << 16);
    }
    if (entity->XVelocity > 0) {
        movingLeft = 0;
    }
    else {
        movingLeft          = 1;
        sensors[1].YPos     = entity->YPos + 0x40000;
        sensors[1].collided = false;
        sensors[1].XPos     = entity->XPos + ((collisionLeft - 1) << 16);
    }
    sensors[2].XPos     = entity->XPos + (playerHitbox->left[1] << 16);
    sensors[3].XPos     = entity->XPos + (playerHitbox->right[1] << 16);
    sensors[2].collided = false;
    sensors[3].collided = false;
    sensors[4].XPos     = sensors[2].XPos;
    sensors[5].XPos     = sensors[3].XPos;
    sensors[4].collided = false;
    sensors[5].collided = false;
    if (entity->YVelocity < 0) {
        movingDown = 0;
    }
    else {
        movingDown      = 1;
        sensors[2].YPos = entity->YPos + (collisionBottom << 16);
        sensors[3].YPos = entity->YPos + (collisionBottom << 16);
    }

    if (abs(entity->XVelocity) > 0x10000 || entity->YVelocity < 0) {
        movingUp        = 1;
        sensors[4].YPos = entity->YPos + ((collisionTop - 1) << 16);
        sensors[5].YPos = entity->YPos + ((collisionTop - 1) << 16);
    }

    int cnt         = (abs(entity->XVelocity) <= abs(entity->YVelocity) ? (abs(entity->YVelocity) >> 19) + 1 : (abs(entity->XVelocity) >> 19) + 1);
    int XVel        = entity->XVelocity / cnt;
    int YVel        = entity->YVelocity / cnt;
    int XVel2       = entity->XVelocity - XVel * (cnt - 1);
    int YVel2       = entity->YVelocity - YVel * (cnt - 1);
    while (cnt > 0) {
        if (cnt < 2) {
            XVel = XVel2;
            YVel = YVel2;
        }
        cnt--;

        if (movingRight == 1) {
            sensors[0].XPos += XVel;
            sensors[0].YPos += YVel;
            LWallCollision(entity, &sensors[0]);
            if (sensors[0].collided) {
                movingRight = 2;
            }
            else if (entity->XVelocity < 0x20000) {
                sensors[0].YPos -= 0x80000;
                LWallCollision(entity, &sensors[0]);
                if (sensors[0].collided)
                    movingRight = 2;
                sensors[0].YPos += 0x80000;
            }
        }

        if (movingLeft == 1) {
            sensors[1].XPos += XVel;
            sensors[1].YPos += YVel;
            RWallCollision(entity, &sensors[1]);
            if (sensors[1].collided) {
                movingLeft = 2;
            }
            else if (entity->XVelocity > -0x20000) {
                sensors[1].YPos -= 0x80000;
                RWallCollision(entity, &sensors[1]);
                if (sensors[1].collided)
                    movingLeft = 2;
                sensors[1].YPos += 0x80000;
            }
        }

        if (movingRight == 2) {
            entity->XVelocity = 0;
            entity->speed     = 0;
            entity->XPos      = (sensors[0].XPos - collisionRight) << 16;
            sensors[2].XPos   = entity->XPos + ((collisionLeft + 1) << 16);
            sensors[3].XPos   = entity->XPos + ((collisionRight - 2) << 16);
            sensors[4].XPos   = sensors[2].XPos;
            sensors[5].XPos   = sensors[3].XPos;
            XVel              = 0;
            XVel2             = 0;
            movingRight       = 3;
        }

        if (movingLeft == 2) {
            entity->XVelocity = 0;
            entity->speed     = 0;
            entity->XPos      = (sensors[1].XPos - collisionLeft + 1) << 16;
            sensors[2].XPos   = entity->XPos + ((collisionLeft + 1) << 16);
            sensors[3].XPos   = entity->XPos + ((collisionRight - 2) << 16);
            sensors[4].XPos   = sensors[2].XPos;
            sensors[5].XPos   = sensors[3].XPos;
            XVel              = 0;
            XVel2             = 0;
            movingLeft        = 3;
        }

        if (movingDown == 1) {
            for (int i = 2; i < 4; i++) {
                if (!sensors[i].collided) {
                    sensors[i].XPos += XVel;
                    sensors[i].YPos += YVel;
                    FloorCollision(entity, &sensors[i]);
                }
            }
            if (sensors[2].collided || sensors[3].collided) {
                movingDown = 2;
                cnt        = 0;
            }
        }

        if (movingUp == 1) {
            for (int i = 4; i < 6; i++) {
                if (!sensors[i].collided) {
                    sensors[i].XPos += XVel;
                    sensors[i].YPos += YVel;
                    RoofCollision(entity, &sensors[i]);
                }
            }
            if (sensors[4].collided || sensors[5].collided) {
                movingUp = 2;
                cnt      = 0;
            }
        }
    }

    if (movingRight < 2 && movingLeft < 2)
        entity->XPos = entity->XPos + entity->XVelocity;

    if (movingUp < 2 && movingDown < 2) {
        entity->YPos = entity->YPos + entity->YVelocity;
        return;
    }

    if (movingDown == 2) {
        entity->gravity = 0;
        if (sensors[2].collided && sensors[3].collided) {
            if (sensors[2].YPos >= sensors[3].YPos) {
                entity->YPos  = (sensors[3].YPos - collisionBottom) << 16;
                entity->angle = sensors[3].angle;
            }
            else {
                entity->YPos  = (sensors[2].YPos - collisionBottom) << 16;
                entity->angle = sensors[2].angle;
            }
        }
        else if (sensors[2].collided == 1) {
            entity->YPos  = (sensors[2].YPos - collisionBottom) << 16;
            entity->angle = sensors[2].angle;
        }
        else if (sensors[3].collided == 1) {
            entity->YPos  = (sensors[3].YPos - collisionBottom) << 16;
            entity->angle = sensors[3].angle;
        }
        if (entity->angle > 0xA0 && entity->angle < 0xE0 && entity->collisionMode != CMODE_LWALL) {
            entity->collisionMode = CMODE_LWALL;
            entity->XPos -= 0x40000;
        }
        if (entity->angle > 0x20 && entity->angle < 0x60 && entity->collisionMode != CMODE_RWALL) {
            entity->collisionMode = CMODE_RWALL;
            entity->XPos += 0x40000;
        }
        if (entity->angle < 0x20 || entity->angle > 0xE0) {
            entity->controlLock = 0;
        }
        entity->rotation = entity->angle << 1;

        int speed = 0;
        if (entity->down) {
            if (entity->angle < 128) {
                if (entity->angle < 16) {
                    speed = entity->XVelocity;
                }
                else if (entity->angle >= 32) {
                    speed = (abs(entity->XVelocity) <= abs(entity->YVelocity) ? entity->YVelocity + entity->YVelocity / 12 : entity->XVelocity);
                }
                else {
                    speed = (abs(entity->XVelocity) <= abs(entity->YVelocity >> 1) ? (entity->YVelocity + entity->YVelocity / 12) >> 1
                                                                                   : entity->XVelocity);
                }
            }
            else if (entity->angle > 240) {
                speed = entity->XVelocity;
            }
            else if (entity->angle <= 224) {
                speed = (abs(entity->XVelocity) <= abs(entity->YVelocity) ? -(entity->YVelocity + entity->YVelocity / 12) : entity->XVelocity);
            }
            else {
                speed = (abs(entity->XVelocity) <= abs(entity->YVelocity >> 1) ? -((entity->YVelocity + entity->YVelocity / 12) >> 1)
                                                                               : entity->XVelocity);
            }
        }
        else if (entity->angle < 0x80) {
            if (entity->angle < 0x10) {
                speed = entity->XVelocity;
            }
            else if (entity->angle >= 0x20) {
                speed = (abs(entity->XVelocity) <= abs(entity->YVelocity) ? entity->YVelocity : entity->XVelocity);
            }
            else {
                speed = (abs(entity->XVelocity) <= abs(entity->YVelocity >> 1) ? entity->YVelocity >> 1 : entity->XVelocity);
            }
        }
        else if (entity->angle > 0xF0) {
            speed = entity->XVelocity;
        }
        else if (entity->angle <= 0xE0) {
            speed = (abs(entity->XVelocity) <= abs(entity->YVelocity) ? -entity->YVelocity : entity->XVelocity);
        }
        else {
            speed = (abs(entity->XVelocity) <= abs(entity->YVelocity >> 1) ? -(entity->YVelocity >> 1) : entity->XVelocity);
        }

        if (speed < -0x180000)
            speed = -0x180000;
        if (speed > 0x180000)
            speed = 0x180000;
        entity->speed         = speed;
        entity->YVelocity     = 0;
        scriptEng.checkResult = 1;
    }

    if (movingUp == 2) {
        int sensorAngle = 0;
        if (sensors[4].collided && sensors[5].collided) {
            if (sensors[4].YPos <= sensors[5].YPos) {
                entity->YPos = (sensors[5].YPos - collisionTop + 1) << 16;
                sensorAngle  = sensors[5].angle;
            }
            else {
                entity->YPos = (sensors[4].YPos - collisionTop + 1) << 16;
                sensorAngle  = sensors[4].angle;
            }
        }
        else if (sensors[4].collided) {
            entity->YPos = (sensors[4].YPos - collisionTop + 1) << 16;
            sensorAngle  = sensors[4].angle;
        }
        else if (sensors[5].collided) {
            entity->YPos = (sensors[5].YPos - collisionTop + 1) << 16;
            sensorAngle  = sensors[5].angle;
        }
        sensorAngle &= 0xFF;

        int angle = ArcTanLookup(entity->XVelocity, entity->YVelocity);
        if (sensorAngle > 0x40 && sensorAngle < 0x62 && angle > 0xA0 && angle < 0xC2) {
            entity->gravity       = 0;
            entity->angle         = sensorAngle;
            entity->rotation      = entity->angle << 1;
            entity->collisionMode = CMODE_RWALL;
            entity->XPos += 0x40000;
            entity->YPos -= 0x20000;
            if (entity->angle <= 0x60)
                entity->speed = entity->YVelocity;
            else
                entity->speed = entity->YVelocity >> 1;
        }
        if (sensorAngle > 0x9E && sensorAngle < 0xC0 && angle > 0xBE && angle < 0xE0) {
            entity->gravity       = 0;
            entity->angle         = sensorAngle;
            entity->rotation      = entity->angle << 1;
            entity->collisionMode = CMODE_LWALL;
            entity->XPos -= 0x40000;
            entity->YPos -= 0x20000;
            if (entity->angle >= 0xA0)
                entity->speed = -entity->YVelocity;
            else
                entity->speed = -entity->YVelocity >> 1;
        }
        if (entity->YVelocity < 0)
            entity->YVelocity = 0;
        scriptEng.checkResult = 2;
    }
}
void ProcessPathGrip(Entity *entity)
{
    int cosValue256;
    int sinValue256;
    sensors[4].XPos = entity->XPos;
    sensors[4].YPos = entity->YPos;
    for (int i = 0; i < 7; ++i) {
        sensors[i].angle    = entity->angle;
        sensors[i].collided = false;
    }
    SetPathGripSensors(entity);
    int absSpeed  = abs(entity->speed);
    int checkDist = absSpeed >> 18;
    absSpeed &= 0x3FFFF;
    byte cMode = entity->collisionMode;

    while (checkDist > -1) {
        if (checkDist >= 1) {
            cosValue256 = cosVal256[entity->angle] << 10;
            sinValue256 = sinVal256[entity->angle] << 10;
            checkDist--;
        }
        else {
            cosValue256 = absSpeed * cosVal256[entity->angle] >> 8;
            sinValue256 = absSpeed * sinVal256[entity->angle] >> 8;
            checkDist   = -1;
        }

        if (entity->speed < 0) {
            cosValue256 = -cosValue256;
            sinValue256 = -sinValue256;
        }

        sensors[0].collided = false;
        sensors[1].collided = false;
        sensors[2].collided = false;
        sensors[5].collided = false;
        sensors[6].collided = false;
        sensors[4].XPos += cosValue256;
        sensors[4].YPos += sinValue256;
        int tileDistance = -1;

        switch (entity->collisionMode) {
            case CMODE_FLOOR: {
                sensors[3].XPos += cosValue256;
                sensors[3].YPos += sinValue256;

                if (entity->speed > 0) {
                    LWallCollision(entity, &sensors[3]);
                    if (sensors[3].collided) {
                        sensors[2].XPos = (sensors[3].XPos - 2) << 16;
                    }
                }

                if (entity->speed < 0) {
                    RWallCollision(entity, &sensors[3]);
                    if (sensors[3].collided) {
                        sensors[0].XPos = (sensors[3].XPos + 2) << 16;
                    }
                }

                if (sensors[3].collided) {
                    cosValue256 = 0;
                    checkDist   = -1;
                }

                for (int i = 0; i < 3; i++) {
                    sensors[i].XPos += cosValue256;
                    sensors[i].YPos += sinValue256;
                    FindFloorPosition(entity, &sensors[i], sensors[i].YPos >> 16);
                }

                for (int i = 5; i < 7; i++) {
                    sensors[i].XPos += cosValue256;
                    sensors[i].YPos += sinValue256;
                    FindFloorPosition(entity, &sensors[i], sensors[i].YPos >> 16);
                }

                tileDistance = -1;
                for (int i = 0; i < 3; i++) {
                    if (tileDistance > -1) {
                        if (sensors[i].collided) {
                            if (sensors[i].YPos < sensors[tileDistance].YPos)
                                tileDistance = i;

                            if (sensors[i].YPos == sensors[tileDistance].YPos && (sensors[i].angle < 0x08 || sensors[i].angle > 0xF8))
                                tileDistance = i;
                        }
                    }
                    else if (sensors[i].collided)
                        tileDistance = i;
                }

                if (tileDistance <= -1) {
                    checkDist = -1;
                }
                else {
                    sensors[0].YPos  = sensors[tileDistance].YPos << 16;
                    sensors[0].angle = sensors[tileDistance].angle;
                    sensors[1].YPos  = sensors[0].YPos;
                    sensors[1].angle = sensors[0].angle;
                    sensors[2].YPos  = sensors[0].YPos;
                    sensors[2].angle = sensors[0].angle;
                    sensors[3].YPos  = sensors[0].YPos - 0x40000;
                    sensors[3].angle = sensors[0].angle;
                    sensors[4].XPos  = sensors[1].XPos;
                    sensors[4].YPos  = sensors[0].YPos - (collisionBottom << 16);
                }

                if (sensors[0].angle < 0xDE && sensors[0].angle > 0x80)
                    entity->collisionMode = CMODE_LWALL;
                if (sensors[0].angle > 0x22 && sensors[0].angle < 0x80)
                    entity->collisionMode = CMODE_RWALL;
                break;
            }
            case CMODE_LWALL: {
                sensors[3].XPos += cosValue256;
                sensors[3].YPos += sinValue256;

                if (entity->speed > 0)
                    RoofCollision(entity, &sensors[3]);

                if (entity->speed < 0)
                    FloorCollision(entity, &sensors[3]);

                if (sensors[3].collided) {
                    sinValue256 = 0;
                    checkDist   = -1;
                }
                for (int i = 0; i < 3; i++) {
                    sensors[i].XPos += cosValue256;
                    sensors[i].YPos += sinValue256;
                    FindLWallPosition(entity, &sensors[i], sensors[i].XPos >> 16);
                }

                tileDistance = -1;
                for (int i = 0; i < 3; i++) {
                    if (tileDistance > -1) {
                        if (sensors[i].XPos < sensors[tileDistance].XPos && sensors[i].collided) {
                            tileDistance = i;
                        }
                    }
                    else if (sensors[i].collided) {
                        tileDistance = i;
                    }
                }

                if (tileDistance <= -1) {
                    checkDist = -1;
                }
                else {
                    sensors[0].XPos  = sensors[tileDistance].XPos << 16;
                    sensors[0].angle = sensors[tileDistance].angle;
                    sensors[1].XPos  = sensors[0].XPos;
                    sensors[1].angle = sensors[0].angle;
                    sensors[2].XPos  = sensors[0].XPos;
                    sensors[2].angle = sensors[0].angle;
                    sensors[4].YPos  = sensors[1].YPos;
                    sensors[4].XPos  = sensors[1].XPos - (collisionRight << 16);
                }

                if (sensors[0].angle > 0xE2)
                    entity->collisionMode = CMODE_FLOOR;
                if (sensors[0].angle < 0x9E)
                    entity->collisionMode = CMODE_ROOF;
                break;
                break;
            }
            case CMODE_ROOF: {
                sensors[3].XPos += cosValue256;
                sensors[3].YPos += sinValue256;

                if (entity->speed > 0)
                    RWallCollision(entity, &sensors[3]);

                if (entity->speed < 0)
                    LWallCollision(entity, &sensors[3]);

                if (sensors[3].collided) {
                    cosValue256 = 0;
                    checkDist   = -1;
                }
                for (int i = 0; i < 3; i++) {
                    sensors[i].XPos += cosValue256;
                    sensors[i].YPos += sinValue256;
                    FindRoofPosition(entity, &sensors[i], sensors[i].YPos >> 16);
                }

                tileDistance = -1;
                for (int i = 0; i < 3; i++) {
                    if (tileDistance > -1) {
                        if (sensors[i].YPos > sensors[tileDistance].YPos && sensors[i].collided) {
                            tileDistance = i;
                        }
                    }
                    else if (sensors[i].collided) {
                        tileDistance = i;
                    }
                }

                if (tileDistance <= -1) {
                    checkDist = -1;
                }
                else {
                    sensors[0].YPos  = sensors[tileDistance].YPos << 16;
                    sensors[0].angle = sensors[tileDistance].angle;
                    sensors[1].YPos  = sensors[0].YPos;
                    sensors[1].angle = sensors[0].angle;
                    sensors[2].YPos  = sensors[0].YPos;
                    sensors[2].angle = sensors[0].angle;
                    sensors[3].YPos  = sensors[0].YPos + 0x40000;
                    sensors[3].angle = sensors[0].angle;
                    sensors[4].XPos  = sensors[1].XPos;
                    sensors[4].YPos  = sensors[0].YPos - ((collisionTop - 1) << 16);
                }

                if (sensors[0].angle > 0xA2)
                    entity->collisionMode = CMODE_LWALL;
                if (sensors[0].angle < 0x5E)
                    entity->collisionMode = CMODE_RWALL;
                break;
            }
            case CMODE_RWALL: {
                sensors[3].XPos += cosValue256;
                sensors[3].YPos += sinValue256;

                if (entity->speed > 0)
                    FloorCollision(entity, &sensors[3]);

                if (entity->speed < 0)
                    RoofCollision(entity, &sensors[3]);

                if (sensors[3].collided) {
                    sinValue256 = 0;
                    checkDist   = -1;
                }
                for (int i = 0; i < 3; i++) {
                    sensors[i].XPos += cosValue256;
                    sensors[i].YPos += sinValue256;
                    FindRWallPosition(entity, &sensors[i], sensors[i].XPos >> 16);
                }

                tileDistance = -1;
                for (int i = 0; i < 3; i++) {
                    if (tileDistance > -1) {
                        if (sensors[i].XPos > sensors[tileDistance].XPos && sensors[i].collided) {
                            tileDistance = i;
                        }
                    }
                    else if (sensors[i].collided) {
                        tileDistance = i;
                    }
                }

                if (tileDistance <= -1) {
                    checkDist = -1;
                }
                else {
                    sensors[0].XPos  = sensors[tileDistance].XPos << 16;
                    sensors[0].angle = sensors[tileDistance].angle;
                    sensors[1].XPos  = sensors[0].XPos;
                    sensors[1].angle = sensors[0].angle;
                    sensors[2].XPos  = sensors[0].XPos;
                    sensors[2].angle = sensors[0].angle;
                    sensors[4].YPos  = sensors[1].YPos;
                    sensors[4].XPos  = sensors[1].XPos - ((collisionLeft - 1) << 16);
                }

                if (sensors[0].angle < 0x1E)
                    entity->collisionMode = CMODE_FLOOR;
                if (sensors[0].angle > 0x62)
                    entity->collisionMode = CMODE_ROOF;
                break;
            }
        }
        if (tileDistance != -1)
            entity->angle = sensors[0].angle;

        if (!sensors[3].collided)
            SetPathGripSensors(entity);
        else
            checkDist = -2;
    }

    switch (cMode) {
        case CMODE_FLOOR: {
            if (sensors[0].collided || sensors[1].collided || sensors[2].collided) {
                entity->angle       = sensors[0].angle;
                entity->rotation    = entity->angle << 1;
                entity->flailing[0] = sensors[0].collided;
                entity->flailing[1] = sensors[1].collided;
                entity->flailing[2] = sensors[2].collided;
                entity->flailing[3] = sensors[5].collided;
                entity->flailing[4] = sensors[6].collided;
                if (!sensors[3].collided) {
                    entity->pushing = 0;
                    entity->XPos    = sensors[4].XPos;
                }
                else {
                    if (entity->speed > 0)
                        entity->XPos = (sensors[3].XPos - collisionRight) << 16;

                    if (entity->speed < 0)
                        entity->XPos = (sensors[3].XPos - collisionLeft + 1) << 16;

                    entity->speed = 0;
                    if ((entity->left || entity->right) && entity->pushing < 2)
                        entity->pushing++;
                }
                entity->YPos = sensors[4].YPos;
                return;
            }
            entity->gravity       = 1;
            entity->collisionMode = CMODE_FLOOR;
            entity->XVelocity     = cosVal256[entity->angle] * entity->speed >> 8;
            entity->YVelocity     = sinVal256[entity->angle] * entity->speed >> 8;
            if (entity->YVelocity < -0x100000)
                entity->YVelocity = -0x100000;

            if (entity->YVelocity > 0x100000)
                entity->YVelocity = 0x100000;

            entity->speed = entity->XVelocity;
            entity->angle = 0;
            if (!sensors[3].collided) {
                entity->pushing = 0;
                entity->XPos += entity->XVelocity;
            }
            else {
                if (entity->speed > 0)
                    entity->XPos = (sensors[3].XPos - collisionRight) << 16;
                if (entity->speed < 0)
                    entity->XPos = (sensors[3].XPos - collisionLeft + 1) << 16;

                entity->speed = 0;
                if ((entity->left || entity->right) && entity->pushing < 2)
                    entity->pushing++;
            }
            entity->YPos += entity->YVelocity;
            return;
        }
        case CMODE_LWALL: {
            if (!sensors[0].collided && !sensors[1].collided && !sensors[2].collided) {
                entity->gravity       = 1;
                entity->collisionMode = CMODE_FLOOR;
                entity->XVelocity     = cosVal256[entity->angle] * entity->speed >> 8;
                entity->YVelocity     = sinVal256[entity->angle] * entity->speed >> 8;
                if (entity->YVelocity < -1048576) {
                    entity->YVelocity = -1048576;
                }
                if (entity->YVelocity > 0x100000) {
                    entity->YVelocity = 0x100000;
                }
                entity->speed = entity->XVelocity;
                entity->angle = 0;
            }
            else if (entity->speed >= 0x28000 || entity->speed <= -0x28000 || entity->controlLock != 0) {
                entity->angle    = sensors[0].angle;
                entity->rotation = entity->angle << 1;
            }
            else {
                entity->gravity       = 1;
                entity->angle         = 0;
                entity->collisionMode = CMODE_FLOOR;
                entity->speed         = entity->XVelocity;
                entity->controlLock   = 30;
            }
            if (!sensors[3].collided) {
                entity->YPos = sensors[4].YPos;
            }
            else {
                if (entity->speed > 0)
                    entity->YPos = (sensors[3].YPos - collisionTop) << 16;

                if (entity->speed < 0)
                    entity->YPos = (sensors[3].YPos - collisionBottom) << 16;

                entity->speed = 0;
            }
            entity->XPos = sensors[4].XPos;
            return;
        }
        case CMODE_ROOF: {
            if (!sensors[0].collided && !sensors[1].collided && !sensors[2].collided) {
                entity->gravity       = 1;
                entity->collisionMode = CMODE_FLOOR;
                entity->XVelocity     = cosVal256[entity->angle] * entity->speed >> 8;
                entity->YVelocity     = sinVal256[entity->angle] * entity->speed >> 8;
                entity->flailing[0]   = 0;
                entity->flailing[1]   = 0;
                entity->flailing[2]   = 0;
                if (entity->YVelocity < -0x100000)
                    entity->YVelocity = -0x100000;

                if (entity->YVelocity > 0x100000)
                    entity->YVelocity = 0x100000;

                entity->angle = 0;
                entity->speed = entity->XVelocity;
                if (!sensors[3].collided) {
                    entity->XPos = entity->XPos + entity->XVelocity;
                }
                else {
                    if (entity->speed > 0)
                        entity->XPos = (sensors[3].XPos - collisionRight) << 16;

                    if (entity->speed < 0)
                        entity->XPos = (sensors[3].XPos - collisionLeft + 1) << 16;

                    entity->speed = 0;
                }
            }
            else if (entity->speed <= -0x28000 || entity->speed >= 0x28000) {
                entity->angle    = sensors[0].angle;
                entity->rotation = entity->angle << 1;
                if (!sensors[3].collided) {
                    entity->XPos = sensors[4].XPos;
                }
                else {
                    if (entity->speed < 0)
                        entity->XPos = (sensors[3].XPos - collisionRight) << 16;

                    if (entity->speed > 0)
                        entity->XPos = (sensors[3].XPos - collisionLeft + 1) << 16;
                    entity->speed = 0;
                }
            }
            else {
                entity->gravity       = 1;
                entity->angle         = 0;
                entity->collisionMode = CMODE_FLOOR;
                entity->speed         = entity->XVelocity;
                entity->flailing[0]   = 0;
                entity->flailing[1]   = 0;
                entity->flailing[2]   = 0;
                if (!sensors[3].collided) {
                    entity->XPos = entity->XPos + entity->XVelocity;
                }
                else {
                    if (entity->speed > 0)
                        entity->XPos = (sensors[3].XPos - collisionRight) << 16;

                    if (entity->speed < 0)
                        entity->XPos = (sensors[3].XPos - collisionLeft + 1) << 16;
                    entity->speed = 0;
                }
            }
            entity->YPos = sensors[4].YPos;
            return;
        }
        case CMODE_RWALL: {
            if (!sensors[0].collided && !sensors[1].collided && !sensors[2].collided) {
                entity->gravity       = 1;
                entity->collisionMode = CMODE_FLOOR;
                entity->XVelocity     = cosVal256[entity->angle] * entity->speed >> 8;
                entity->YVelocity     = sinVal256[entity->angle] * entity->speed >> 8;
                if (entity->YVelocity < -0x100000)
                    entity->YVelocity = -0x100000;

                if (entity->YVelocity > 0x100000)
                    entity->YVelocity = 0x100000;

                entity->speed = entity->XVelocity;
                entity->angle = 0;
            }
            else if (entity->speed <= -0x28000 || entity->speed >= 0x28000 || entity->controlLock != 0) {
                entity->angle    = sensors[0].angle;
                entity->rotation = entity->angle << 1;
            }
            else {
                entity->gravity       = 1;
                entity->angle         = 0;
                entity->collisionMode = CMODE_FLOOR;
                entity->speed         = entity->XVelocity;
                entity->controlLock   = 30;
            }
            if (!sensors[3].collided) {
                entity->YPos = sensors[4].YPos;
            }
            else {
                if (entity->speed > 0)
                    entity->YPos = (sensors[3].YPos - collisionBottom) << 16;

                if (entity->speed < 0)
                    entity->YPos = (sensors[3].YPos - collisionTop + 1) << 16;

                entity->speed = 0;
            }
            entity->XPos = sensors[4].XPos;
            return;
        }
        default: return;
    }
}

void SetPathGripSensors(Entity *player)
{
    Hitbox *playerHitbox = getHitbox(player);

    switch (player->collisionMode) {
        case CMODE_FLOOR: {
            collisionLeft   = playerHitbox->left[0];
            collisionTop    = playerHitbox->top[0];
            collisionRight  = playerHitbox->right[0];
            collisionBottom = playerHitbox->bottom[0];
            sensors[0].YPos = sensors[4].YPos + (collisionBottom << 16);
            sensors[1].YPos = sensors[0].YPos;
            sensors[2].YPos = sensors[0].YPos;
            sensors[3].YPos = sensors[4].YPos + 0x40000;
            sensors[5].YPos = sensors[0].YPos;
            sensors[6].YPos = sensors[0].YPos;

            sensors[0].XPos = sensors[4].XPos + ((playerHitbox->left[1] - 1) << 16);
            sensors[1].XPos = sensors[4].XPos;
            sensors[2].XPos = sensors[4].XPos + (playerHitbox->right[1] << 16);
            sensors[5].XPos = sensors[4].XPos + (playerHitbox->left[1] << 15);
            sensors[6].XPos = sensors[4].XPos + (playerHitbox->right[1] << 15);
            if (player->speed > 0) {
                sensors[3].XPos = sensors[4].XPos + ((collisionRight + 1) << 16);
            }
            else {
                sensors[3].XPos = sensors[4].XPos + ((collisionLeft - 1) << 16);
            }
            return;
        }
        case CMODE_LWALL: {
            collisionLeft   = playerHitbox->left[2];
            collisionTop    = playerHitbox->top[2];
            collisionRight  = playerHitbox->right[2];
            collisionBottom = playerHitbox->bottom[2];
            sensors[0].XPos = sensors[4].XPos + (collisionRight << 16);
            sensors[1].XPos = sensors[0].XPos;
            sensors[2].XPos = sensors[0].XPos;
            sensors[3].XPos = sensors[4].XPos + 0x40000;
            sensors[0].YPos = sensors[4].YPos + ((playerHitbox->top[3] - 1) << 16);
            sensors[1].YPos = sensors[4].YPos;
            sensors[2].YPos = sensors[4].YPos + (playerHitbox->bottom[3] << 16);
            if (player->speed > 0) {
                sensors[3].YPos = sensors[4].YPos + (collisionTop << 16);
            }
            else {
                sensors[3].YPos = sensors[4].YPos + ((collisionBottom - 1) << 16);
            }
            return;
        }
        case CMODE_ROOF: {
            collisionLeft   = playerHitbox->left[4];
            collisionTop    = playerHitbox->top[4];
            collisionRight  = playerHitbox->right[4];
            collisionBottom = playerHitbox->bottom[4];
            sensors[0].YPos = sensors[4].YPos + ((collisionTop - 1) << 16);
            sensors[1].YPos = sensors[0].YPos;
            sensors[2].YPos = sensors[0].YPos;
            sensors[3].YPos = sensors[4].YPos - 0x40000;
            sensors[0].XPos = sensors[4].XPos + ((playerHitbox->left[5] - 1) << 16);
            sensors[1].XPos = sensors[4].XPos;
            sensors[2].XPos = sensors[4].XPos + (playerHitbox->right[5] << 16);
            if (player->speed < 0) {
                sensors[3].XPos = sensors[4].XPos + ((collisionRight + 1) << 16);
            }
            else {
                sensors[3].XPos = sensors[4].XPos + ((collisionLeft - 1) << 16);
            }
            return;
        }
        case CMODE_RWALL: {
            collisionLeft   = playerHitbox->left[6];
            collisionTop    = playerHitbox->top[6];
            collisionRight  = playerHitbox->right[6];
            collisionBottom = playerHitbox->bottom[6];
            sensors[0].XPos = sensors[4].XPos + ((collisionLeft - 1) << 16);
            sensors[1].XPos = sensors[0].XPos;
            sensors[2].XPos = sensors[0].XPos;
            sensors[3].XPos = sensors[4].XPos - 0x40000;
            sensors[0].YPos = sensors[4].YPos + ((playerHitbox->top[7] - 1) << 16);
            sensors[1].YPos = sensors[4].YPos;
            sensors[2].YPos = sensors[4].YPos + (playerHitbox->bottom[7] << 16);
            if (player->speed > 0) {
                sensors[3].YPos = sensors[4].YPos + (collisionBottom << 16);
            }
            else {
                sensors[3].YPos = sensors[4].YPos + ((collisionTop - 1) << 16);
            }
            return;
        }
        default: return;
    }
}

void ProcessPlayerTileCollisions(Entity *player)
{
    player->flailing[0]   = 0;
    player->flailing[1]   = 0;
    player->flailing[2]   = 0;
    player->flailing[3]   = 0;
    player->flailing[4]   = 0;
    scriptEng.checkResult = false;

    collisionTolerance = 15;
    if (player->speed < 0x60000)
        collisionTolerance = (sbyte)player->angle == 0 ? 8 : 15;

    if (player->gravity == 1)
        ProcessAirCollision(player);
    else
        ProcessPathGrip(player);
}

void ObjectFloorCollision(int xOffset, int yOffset, int cPath)
{
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectEntityPos];
    int c                 = 0;
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    if (XPos > 0 && XPos < stageLayouts[0].width << 7 && YPos > 0 && YPos < stageLayouts[0].height << 7) {
        int chunkX    = XPos >> 7;
        int tileX     = (XPos & 0x7F) >> 4;
        int chunkY    = YPos >> 7;
        int tileY     = (YPos & 0x7F) >> 4;
        int chunk     = (stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6) + tileX + (tileY << 3);
        int tileIndex = tiles128x128.tileIndex[chunk];
        if (tiles128x128.collisionFlags[cPath][chunk] != SOLID_LRB && tiles128x128.collisionFlags[cPath][chunk] != SOLID_NONE) {
            switch (tiles128x128.direction[chunk]) {
                case 0: {
                    c = (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) <= collisionMasks[cPath].floorMasks[c]) {
                        break;
                    }
                    YPos                  = collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 1: {
                    c = 15 - (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) <= collisionMasks[cPath].floorMasks[c]) {
                        break;
                    }
                    YPos                  = collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 2: {
                    c = (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) <= 15 - collisionMasks[cPath].roofMasks[c]) {
                        break;
                    }
                    YPos                  = 15 - collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 3: {
                    c = 15 - (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) <= 15 - collisionMasks[cPath].roofMasks[c]) {
                        break;
                    }
                    YPos                  = 15 - collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
            }
        }
        if (scriptEng.checkResult) {
            entity->YPos = (YPos - yOffset) << 16;
        }
    }
}
void ObjectLWallCollision(int xOffset, int yOffset, int cPath)
{
    int c;
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectEntityPos];
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    if (XPos > 0 && XPos < stageLayouts[0].width << 7 && YPos > 0 && YPos < stageLayouts[0].height << 7) {
        int chunkX    = XPos >> 7;
        int tileX     = (XPos & 0x7F) >> 4;
        int chunkY    = YPos >> 7;
        int tileY     = (YPos & 0x7F) >> 4;
        int chunk     = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
        chunk         = chunk + tileX + (tileY << 3);
        int tileIndex = tiles128x128.tileIndex[chunk];
        if (tiles128x128.collisionFlags[cPath][chunk] != SOLID_TOP && tiles128x128.collisionFlags[cPath][chunk] < SOLID_NONE) {
            switch (tiles128x128.direction[chunk]) {
                case 0: {
                    c = (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) <= collisionMasks[cPath].lWallMasks[c]) {
                        break;
                    }
                    XPos                  = collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 1: {
                    c = (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) <= 15 - collisionMasks[cPath].rWallMasks[c]) {
                        break;
                    }
                    XPos                  = 15 - collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 2: {
                    c = 15 - (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) <= collisionMasks[cPath].lWallMasks[c]) {
                        break;
                    }
                    XPos                  = collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 3: {
                    c = 15 - (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) <= 15 - collisionMasks[cPath].rWallMasks[c]) {
                        break;
                    }
                    XPos                  = 15 - collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
            }
        }
        if (scriptEng.checkResult) {
            entity->XPos = (XPos - xOffset) << 16;
        }
    }
}
void ObjectRoofCollision(int xOffset, int yOffset, int cPath)
{
    int c;
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectEntityPos];
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    if (XPos > 0 && XPos < stageLayouts[0].width << 7 && YPos > 0 && YPos < stageLayouts[0].height << 7) {
        int chunkX    = XPos >> 7;
        int tileX     = (XPos & 0x7F) >> 4;
        int chunkY    = YPos >> 7;
        int tileY     = (YPos & 0x7F) >> 4;
        int chunk     = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
        chunk         = chunk + tileX + (tileY << 3);
        int tileIndex = tiles128x128.tileIndex[chunk];
        if (tiles128x128.collisionFlags[cPath][chunk] != SOLID_TOP && tiles128x128.collisionFlags[cPath][chunk] < SOLID_NONE) {
            switch (tiles128x128.direction[chunk]) {
                case 0: {
                    c = (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) >= collisionMasks[cPath].roofMasks[c]) {
                        break;
                    }
                    YPos                  = collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 1: {
                    c = 15 - (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) >= collisionMasks[cPath].roofMasks[c]) {
                        break;
                    }
                    YPos                  = collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 2: {
                    c = (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) >= 15 - collisionMasks[cPath].floorMasks[c]) {
                        break;
                    }
                    YPos                  = 15 - collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 3: {
                    c = 15 - (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) >= 15 - collisionMasks[cPath].floorMasks[c]) {
                        break;
                    }
                    YPos                  = 15 - collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
            }
        }
        if (scriptEng.checkResult) {
            entity->YPos = (YPos - yOffset) << 16;
        }
    }
}
void ObjectRWallCollision(int xOffset, int yOffset, int cPath)
{
    int c;
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectEntityPos];
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    if (XPos > 0 && XPos < stageLayouts[0].width << 7 && YPos > 0 && YPos < stageLayouts[0].height << 7) {
        int chunkX    = XPos >> 7;
        int tileX     = (XPos & 0x7F) >> 4;
        int chunkY    = YPos >> 7;
        int tileY     = (YPos & 0x7F) >> 4;
        int chunk     = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
        chunk         = chunk + tileX + (tileY << 3);
        int tileIndex = tiles128x128.tileIndex[chunk];
        if (tiles128x128.collisionFlags[cPath][chunk] != SOLID_TOP && tiles128x128.collisionFlags[cPath][chunk] < SOLID_NONE) {
            switch (tiles128x128.direction[chunk]) {
                case 0: {
                    c = (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) >= collisionMasks[cPath].rWallMasks[c]) {
                        break;
                    }
                    XPos                  = collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 1: {
                    c = (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) >= 15 - collisionMasks[cPath].lWallMasks[c]) {
                        break;
                    }
                    XPos                  = 15 - collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 2: {
                    c = 15 - (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) >= collisionMasks[cPath].rWallMasks[c]) {
                        break;
                    }
                    XPos                  = collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 3: {
                    c = 15 - (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) >= 15 - collisionMasks[cPath].lWallMasks[c]) {
                        break;
                    }
                    XPos                  = 15 - collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
            }
        }
        if (scriptEng.checkResult) {
            entity->XPos = (XPos - xOffset) << 16;
        }
    }
}

void ObjectFloorGrip(int xOffset, int yOffset, int cPath)
{
    int c;
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectEntityPos];
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    int chunkX            = YPos;
    YPos                  = YPos - 16;
    for (int i = 3; i > 0; i--) {
        if (XPos > 0 && XPos < stageLayouts[0].width << 7 && YPos > 0 && YPos < stageLayouts[0].height << 7 && !scriptEng.checkResult) {
            int chunkX    = XPos >> 7;
            int tileX     = (XPos & 0x7F) >> 4;
            int chunkY    = YPos >> 7;
            int tileY     = (YPos & 0x7F) >> 4;
            int chunk     = (stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6) + tileX + (tileY << 3);
            int tileIndex = tiles128x128.tileIndex[chunk];
            if (tiles128x128.collisionFlags[cPath][chunk] != SOLID_LRB && tiles128x128.collisionFlags[cPath][chunk] != SOLID_NONE) {
                switch (tiles128x128.direction[chunk]) {
                    case 0: {
                        c = (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].floorMasks[c] >= 64) {
                            break;
                        }
                        entity->YPos          = collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 1: {
                        c = 15 - (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].floorMasks[c] >= 64) {
                            break;
                        }
                        entity->YPos          = collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 2: {
                        c = (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].roofMasks[c] <= -64) {
                            break;
                        }
                        entity->YPos          = 15 - collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 3: {
                        c = 15 - (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].roofMasks[c] <= -64) {
                            break;
                        }
                        entity->YPos          = 15 - collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                }
            }
        }
        YPos += 16;
    }
    if (scriptEng.checkResult) {
        if (abs(entity->YPos - chunkX) < 16) {
            entity->YPos = (entity->YPos - yOffset) << 16;
            return;
        }
        entity->YPos          = (chunkX - yOffset) << 16;
        scriptEng.checkResult = false;
    }
}
void ObjectLWallGrip(int xOffset, int yOffset, int cPath)
{
    int c;
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectEntityPos];
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    int startX            = XPos;
    XPos                  = XPos - 16;
    for (int i = 3; i > 0; i--) {
        if (XPos > 0 && XPos < stageLayouts[0].width << 7 && YPos > 0 && YPos < stageLayouts[0].height << 7 && !scriptEng.checkResult) {
            int chunkX    = XPos >> 7;
            int tileX     = (XPos & 0x7F) >> 4;
            int chunkY    = YPos >> 7;
            int tileY     = (YPos & 0x7F) >> 4;
            int chunk     = (stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6) + tileX + (tileY << 3);
            int tileIndex = tiles128x128.tileIndex[chunk];
            if (tiles128x128.collisionFlags[cPath][chunk] < SOLID_NONE) {
                switch (tiles128x128.direction[chunk]) {
                    case 0: {
                        c = (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].lWallMasks[c] >= 64) {
                            break;
                        }
                        entity->XPos          = collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 1: {
                        c = (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].rWallMasks[c] <= -64) {
                            break;
                        }
                        entity->XPos          = 15 - collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 2: {
                        c = 15 - (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].lWallMasks[c] >= 64) {
                            break;
                        }
                        entity->XPos          = collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 3: {
                        c = 15 - (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].rWallMasks[c] <= -64) {
                            break;
                        }
                        entity->XPos          = 15 - collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                }
            }
        }
        XPos += 16;
    }
    if (scriptEng.checkResult) {
        if (abs(entity->XPos - startX) < 16) {
            entity->XPos = (entity->XPos - xOffset) << 16;
            return;
        }
        entity->XPos          = (startX - xOffset) << 16;
        scriptEng.checkResult = false;
    }
}
void ObjectRoofGrip(int xOffset, int yOffset, int cPath)
{
    int c;
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectEntityPos];
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    int startY            = YPos;
    YPos                  = YPos + 16;
    for (int i = 3; i > 0; i--) {
        if (XPos > 0 && XPos < stageLayouts[0].width << 7 && YPos > 0 && YPos < stageLayouts[0].height << 7 && !scriptEng.checkResult) {
            int chunkX    = XPos >> 7;
            int tileX     = (XPos & 0x7F) >> 4;
            int chunkY    = YPos >> 7;
            int tileY     = (YPos & 0x7F) >> 4;
            int chunk     = (stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6) + tileX + (tileY << 3);
            int tileIndex = tiles128x128.tileIndex[chunk];
            if (tiles128x128.collisionFlags[cPath][chunk] < SOLID_NONE) {
                switch (tiles128x128.direction[chunk]) {
                    case 0: {
                        c = (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].roofMasks[c] <= -64) {
                            break;
                        }
                        entity->YPos          = collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 1: {
                        c = 15 - (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].roofMasks[c] <= -64) {
                            break;
                        }
                        entity->YPos          = collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 2: {
                        c = (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].floorMasks[c] >= 64) {
                            break;
                        }
                        entity->YPos          = 15 - collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 3: {
                        c = 15 - (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].floorMasks[c] >= 64) {
                            break;
                        }
                        entity->YPos          = 15 - collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                }
            }
        }
        YPos -= 16;
    }
    if (scriptEng.checkResult) {
        if (abs(entity->YPos - startY) < 16) {
            entity->YPos = (entity->YPos - yOffset) << 16;
            return;
        }
        entity->YPos          = (startY - yOffset) << 16;
        scriptEng.checkResult = false;
    }
}
void ObjectRWallGrip(int xOffset, int yOffset, int cPath)
{
    int c;
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectEntityPos];
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    int startX            = XPos;
    XPos                  = XPos + 16;
    for (int i = 3; i > 0; i--) {
        if (XPos > 0 && XPos < stageLayouts[0].width << 7 && YPos > 0 && YPos < stageLayouts[0].height << 7 && !scriptEng.checkResult) {
            int chunkX    = XPos >> 7;
            int tileX     = (XPos & 0x7F) >> 4;
            int chunkY    = YPos >> 7;
            int tileY     = (YPos & 0x7F) >> 4;
            int chunk     = (stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6) + tileX + (tileY << 3);
            int tileIndex = tiles128x128.tileIndex[chunk];
            if (tiles128x128.collisionFlags[cPath][chunk] < SOLID_NONE) {
                switch (tiles128x128.direction[chunk]) {
                    case 0: {
                        c = (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].rWallMasks[c] <= -64) {
                            break;
                        }
                        entity->XPos          = collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 1: {
                        c = (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].lWallMasks[c] >= 64) {
                            break;
                        }
                        entity->XPos          = 15 - collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 2: {
                        c = 15 - (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].rWallMasks[c] <= -64) {
                            break;
                        }
                        entity->XPos          = collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 3: {
                        c = 15 - (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].lWallMasks[c] >= 64) {
                            break;
                        }
                        entity->XPos          = 15 - collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                }
            }
        }
        XPos -= 16;
    }
    if (scriptEng.checkResult) {
        if (abs(entity->XPos - startX) < 16) {
            entity->XPos = (entity->XPos - xOffset) << 16;
            return;
        }
        entity->XPos          = (startX - xOffset) << 16;
        scriptEng.checkResult = false;
    }
}

void TouchCollision(Entity *thisEntity, int thisLeft, int thisTop, int thisRight, int thisBottom, Entity *otherEntity, int otherLeft, int otherTop,
                    int otherRight, int otherBottom)
{
    Hitbox *thisHitbox  = getHitbox(thisEntity);
    Hitbox *otherHitbox = getHitbox(otherEntity);

    if (thisLeft == 0x10000)
        thisLeft = thisHitbox->left[0];

    if (thisTop == 0x10000)
        thisTop = thisHitbox->top[0];

    if (thisRight == 0x10000)
        thisRight = thisHitbox->right[0];

    if (thisBottom == 0x10000)
        thisBottom = thisHitbox->bottom[0];

    thisLeft += thisEntity->XPos >> 16;
    thisTop += thisEntity->YPos >> 16;
    thisRight += thisEntity->XPos >> 16;
    thisBottom += thisEntity->YPos >> 16;

    if (otherLeft == 0x10000)
        otherLeft = otherHitbox->left[0];

    if (otherTop == 0x10000)
        otherTop = otherHitbox->top[0];

    if (otherRight == 0x10000)
        otherRight = otherHitbox->right[0];

    if (otherBottom == 0x10000)
        otherBottom = otherHitbox->bottom[0];

    otherLeft += otherEntity->XPos >> 16;
    otherTop += otherEntity->YPos >> 16;
    otherRight += otherEntity->XPos >> 16;
    otherBottom += otherEntity->YPos >> 16;

    scriptEng.checkResult = otherRight > thisLeft && otherLeft < thisRight && otherBottom > thisTop && otherTop < thisBottom;
}
void BoxCollision(Entity *thisEntity, int thisLeft, int thisTop, int thisRight, int thisBottom, Entity *otherEntity, int otherLeft, int otherTop,
                  int otherRight, int otherBottom)
{
    Hitbox *thisHitbox  = getHitbox(thisEntity);
    Hitbox *otherHitbox = getHitbox(otherEntity);

    if (thisLeft == 0x10000)
        thisLeft = thisHitbox->left[0];

    if (thisTop == 0x10000)
        thisTop = thisHitbox->top[0];

    if (thisRight == 0x10000)
        thisRight = thisHitbox->right[0];

    if (thisBottom == 0x10000)
        thisBottom = thisHitbox->bottom[0];

    thisLeft += thisEntity->XPos >> 16;
    thisTop += thisEntity->YPos >> 16;
    thisRight += thisEntity->XPos >> 16;
    thisBottom += thisEntity->YPos >> 16;

    thisLeft <<= 16;
    thisTop <<= 16;
    thisRight <<= 16;
    thisBottom <<= 16;

    if (otherLeft == 0x10000)
        otherLeft = otherHitbox->left[0];

    if (otherTop == 0x10000)
        otherTop = otherHitbox->top[0];

    if (otherRight == 0x10000)
        otherRight = otherHitbox->right[0];

    if (otherBottom == 0x10000)
        otherBottom = otherHitbox->bottom[0];

    otherLeft <<= 16;
    otherTop <<= 16;
    otherRight <<= 16;
    otherBottom <<= 16;

    scriptEng.checkResult = 0;

    int rx = otherEntity->XPos >> 16 << 16;
    int ry = otherEntity->YPos >> 16 << 16;

    int xDif = otherEntity->XPos - thisRight;
    if (thisEntity->XPos > otherEntity->XPos)
        xDif = thisLeft - otherEntity->XPos;
    int yDif = thisTop - otherEntity->YPos;
    if (thisEntity->YPos <= otherEntity->YPos)
        yDif = otherEntity->YPos - thisBottom;

    if (xDif <= yDif && abs(otherEntity->XVelocity) >> 1 <= abs(otherEntity->YVelocity)) {
        sensors[0].collided = false;
        sensors[1].collided = false;
        sensors[2].collided = false;
        sensors[3].collided = false;
        sensors[4].collided = false;
        sensors[0].XPos     = rx + otherLeft + 0x20000;
        sensors[1].XPos     = rx;
        sensors[2].XPos     = rx + otherRight - 0x20000;
        sensors[3].XPos     = (sensors[0].XPos + rx) >> 1;
        sensors[4].XPos     = (sensors[2].XPos + rx) >> 1;

        sensors[0].YPos = ry + otherBottom;

        if (otherEntity->YVelocity >= 0) {
            for (int i = 0; i < 5; ++i) {
                if (thisLeft < sensors[i].XPos && thisRight > sensors[i].XPos && thisTop <= sensors[0].YPos
                    && thisTop > otherEntity->YPos - otherEntity->YVelocity) {
                    sensors[i].collided      = true;
                    otherEntity->flailing[i] = true;
                }
            }
        }

        if (sensors[0].collided || sensors[1].collided || sensors[2].collided) {
            if (!otherEntity->gravity && (otherEntity->collisionMode == CMODE_RWALL || otherEntity->collisionMode == CMODE_LWALL)) {
                otherEntity->XVelocity = 0;
                otherEntity->speed     = 0;
            }
            otherEntity->YPos        = thisTop - otherBottom;
            otherEntity->gravity     = 0;
            otherEntity->YVelocity   = 0;
            otherEntity->angle       = 0;
            otherEntity->rotation    = 0;
            otherEntity->controlLock = 0;
            scriptEng.checkResult    = 1;
        }
        else {
            sensors[0].collided = false;
            sensors[1].collided = false;
            sensors[0].XPos     = rx + otherLeft + 0x20000;
            sensors[1].XPos     = rx + otherRight - 0x20000;

            sensors[0].YPos = ry + otherTop;

            for (int i = 0; i < 2; ++i) {
                if (thisLeft < sensors[1].XPos && thisRight > sensors[0].XPos && thisBottom > sensors[0].YPos
                    && thisBottom < otherEntity->YPos - otherEntity->YVelocity) {
                    sensors[i].collided = true;
                }
            }

            if (sensors[1].collided || sensors[0].collided) {
                if (otherEntity->gravity == 1)
                    otherEntity->YPos = thisBottom - otherTop;

                if (otherEntity->YVelocity <= 0)
                    otherEntity->YVelocity = 0;
                scriptEng.checkResult = 4;
            }
            else {
                sensors[0].collided = false;
                sensors[1].collided = false;
                sensors[0].XPos     = rx + otherRight;

                sensors[0].YPos = ry + otherTop + 0x20000;
                sensors[1].YPos = ry + otherBottom - 0x20000;
                for (int i = 0; i < 2; ++i) {
                    if (thisLeft <= sensors[0].XPos && thisLeft > otherEntity->XPos - otherEntity->XVelocity && thisTop < sensors[1].YPos
                        && thisBottom > sensors[0].YPos) {
                        sensors[i].collided = true;
                    }
                }

                if (sensors[1].collided || sensors[0].collided) {
                    otherEntity->XPos = thisLeft - otherRight;
                    if (otherEntity->XVelocity > 0) {
                        if (!otherEntity->direction)
                            otherEntity->pushing = 2;

                        otherEntity->XVelocity = 0;
                        if (otherEntity->collisionMode || !otherEntity->left)
                            otherEntity->speed = 0;
                        else
                            otherEntity->speed = -0x8000;
                    }
                    scriptEng.checkResult = 2;
                }
                else {
                    sensors[0].collided = false;
                    sensors[1].collided = false;
                    sensors[0].XPos     = rx + otherLeft;

                    sensors[0].YPos     = ry + otherTop + 0x20000;
                    sensors[1].YPos     = ry + otherBottom - 0x20000;
                    for (int i = 0; i < 2; ++i) {
                        if (thisRight > sensors[0].XPos && thisRight < otherEntity->XPos - otherEntity->XVelocity && thisTop < sensors[1].YPos
                            && thisBottom > sensors[0].YPos) {
                            sensors[i].collided = true;
                        }
                    }

                    if (sensors[1].collided || sensors[0].collided) {
                        otherEntity->XPos = thisRight - otherLeft;
                        if (otherEntity->XVelocity < 0) {
                            if (otherEntity->direction == FLIP_X)
                                otherEntity->pushing = 2;

                            if (otherEntity->XVelocity < -0x10000)
                                otherEntity->XPos += 0x8000;

                            otherEntity->XVelocity = 0;
                            if (otherEntity->collisionMode || !otherEntity->right)
                                otherEntity->speed = 0;
                            else
                                otherEntity->speed = 0x8000;
                        }
                        scriptEng.checkResult = 3;
                    }
                }
            }
        }
    }
    else {
        sensors[0].collided = false;
        sensors[1].collided = false;
        sensors[0].XPos     = rx + otherRight;

        sensors[0].YPos     = ry + otherTop + 0x20000;
        sensors[1].YPos     = ry + otherBottom - 0x20000;
        for (int i = 0; i < 2; ++i) {
            if (thisLeft <= sensors[0].XPos && thisLeft > otherEntity->XPos - otherEntity->XVelocity && thisTop < sensors[1].YPos
                && thisBottom > sensors[0].YPos) {
                sensors[i].collided = true;
            }
        }
        if (sensors[1].collided || sensors[0].collided) {
            otherEntity->XPos = thisLeft - otherRight;
            if (otherEntity->XVelocity > 0) {
                if (!otherEntity->direction)
                    otherEntity->pushing = 2;

                otherEntity->XVelocity = 0;
                if (otherEntity->collisionMode || !otherEntity->left)
                    otherEntity->speed = 0;
                else
                    otherEntity->speed = -0x8000;
            }
            scriptEng.checkResult = 2;
        }
        else {
            sensors[0].collided = false;
            sensors[1].collided = false;
            sensors[0].XPos     = rx + otherLeft;

            sensors[0].YPos = ry + otherTop + 0x20000;
            sensors[1].YPos = ry + otherBottom - 0x20000;
            for (int i = 0; i < 2; ++i) {
                if (thisRight > sensors[0].XPos && thisRight < otherEntity->XPos - otherEntity->XVelocity && thisTop < sensors[1].YPos
                    && thisBottom > sensors[0].YPos) {
                    sensors[i].collided = true;
                }
            }

            if (sensors[0].collided || sensors[1].collided) {
                otherEntity->XPos = thisRight - otherLeft;
                if (otherEntity->XVelocity < 0) {
                    if (otherEntity->direction == FLIP_X)
                        otherEntity->pushing = 2;

                    if (otherEntity->XVelocity < -0x10000)
                        otherEntity->XPos += 0x8000;

                    otherEntity->XVelocity = 0;
                    if (otherEntity->collisionMode || !otherEntity->right)
                        otherEntity->speed = 0;
                    else
                        otherEntity->speed = 0x8000;
                }
                scriptEng.checkResult = 3;
            }
            else {
                sensors[0].collided = false;
                sensors[1].collided = false;
                sensors[2].collided = false;
                sensors[3].collided = false;
                sensors[4].collided = false;
                sensors[0].XPos     = rx + otherLeft + 0x20000;
                sensors[1].XPos     = rx;
                sensors[2].XPos     = rx + otherRight - 0x20000;
                sensors[3].XPos     = (sensors[0].XPos + rx) >> 1;
                sensors[4].XPos     = (sensors[2].XPos + rx) >> 1;

                sensors[0].YPos = ry + otherBottom;
                if (otherEntity->YVelocity >= 0) {
                    for (int i = 0; i < 5; ++i) {
                        if (thisLeft < sensors[i].XPos && thisRight > sensors[i].XPos && thisTop <= sensors[0].YPos
                            && thisTop > otherEntity->YPos - otherEntity->YVelocity) {
                            sensors[i].collided      = true;
                            otherEntity->flailing[i] = true;
                        }
                    }
                }
                if (sensors[2].collided || sensors[1].collided || sensors[0].collided) {
                    if (!otherEntity->gravity && (otherEntity->collisionMode == CMODE_RWALL || otherEntity->collisionMode == CMODE_LWALL)) {
                        otherEntity->XVelocity = 0;
                        otherEntity->speed     = 0;
                    }
                    otherEntity->YPos        = thisTop - otherBottom;
                    otherEntity->gravity     = 0;
                    otherEntity->YVelocity   = 0;
                    otherEntity->angle       = 0;
                    otherEntity->rotation    = 0;
                    otherEntity->controlLock = 0;
                    scriptEng.checkResult    = 1;
                }
                else {
                    sensors[0].collided = false;
                    sensors[1].collided = false;
                    sensors[0].XPos     = rx + otherLeft + 0x20000;
                    sensors[1].XPos     = rx + otherRight - 0x20000;
                    sensors[0].YPos     = ry + otherTop;

                    for (int i = 0; i < 2; ++i) {
                        if (thisLeft < sensors[1].XPos && thisRight > sensors[0].XPos && thisBottom > sensors[0].YPos
                            && thisBottom < otherEntity->YPos - otherEntity->YVelocity) {
                            sensors[i].collided = true;
                        }
                    }

                    if (sensors[1].collided || sensors[0].collided) {
                        if (otherEntity->gravity == 1)
                            otherEntity->YPos = thisBottom - otherTop;

                        if (otherEntity->YVelocity <= 0)
                            otherEntity->YVelocity = 0;
                        scriptEng.checkResult = 4;
                    }
                }
            }
        }
    }
}
void BoxCollision2(Entity *thisEntity, int thisLeft, int thisTop, int thisRight, int thisBottom, Entity *otherEntity, int otherLeft, int otherTop,
                   int otherRight, int otherBottom)
{
    Hitbox *thisHitbox  = getHitbox(thisEntity);
    Hitbox *otherHitbox = getHitbox(otherEntity);

    if (thisLeft == 0x10000)
        thisLeft = thisHitbox->left[0];

    if (thisTop == 0x10000)
        thisTop = thisHitbox->top[0];

    if (thisRight == 0x10000)
        thisRight = thisHitbox->right[0];

    if (thisBottom == 0x10000)
        thisBottom = thisHitbox->bottom[0];

    thisLeft += thisEntity->XPos >> 16;
    thisTop += thisEntity->YPos >> 16;
    thisRight += thisEntity->XPos >> 16;
    thisBottom += thisEntity->YPos >> 16;

    thisLeft <<= 16;
    thisTop <<= 16;
    thisRight <<= 16;
    thisBottom <<= 16;

    if (otherLeft == 0x10000)
        otherLeft = otherHitbox->left[0];

    if (otherTop == 0x10000)
        otherTop = otherHitbox->top[0];

    if (otherRight == 0x10000)
        otherRight = otherHitbox->right[0];

    if (otherBottom == 0x10000)
        otherBottom = otherHitbox->bottom[0];

    otherLeft <<= 16;
    otherTop <<= 16;
    otherRight <<= 16;
    otherBottom <<= 16;

    scriptEng.checkResult = 0;

    int rx = otherEntity->XPos >> 16 << 16;
    int ry = otherEntity->YPos >> 16 << 16;

    int xDif = thisLeft - rx;
    if (thisEntity->XPos <= rx)
        xDif = rx - thisRight;
    int yDif = thisTop - ry;
    if (thisEntity->YPos <= ry)
        yDif = ry - thisBottom;

    if (xDif <= yDif) {
        sensors[0].collided = false;
        sensors[1].collided = false;
        sensors[2].collided = false;
        sensors[0].XPos     = rx + otherLeft + 0x20000;
        sensors[1].XPos     = rx;
        sensors[2].XPos     = rx + otherRight - 0x20000;

        sensors[0].YPos = ry + otherBottom;

        if (otherEntity->YVelocity >= 0) {
            for (int i = 0; i < 3; ++i) {
                if (thisLeft < sensors[i].XPos && thisRight > sensors[i].XPos && thisTop <= sensors[0].YPos && thisEntity->YPos > sensors[0].YPos) {
                    sensors[i].collided      = true;
                    otherEntity->flailing[i] = true;
                }
            }
        }

        if (sensors[0].collided || sensors[1].collided || sensors[2].collided) {
            if (!otherEntity->gravity && (otherEntity->collisionMode == CMODE_RWALL || otherEntity->collisionMode == CMODE_LWALL)) {
                otherEntity->XVelocity = 0;
                otherEntity->speed     = 0;
            }
            otherEntity->YPos        = thisTop - otherBottom;
            otherEntity->gravity     = 0;
            otherEntity->YVelocity   = 0;
            otherEntity->angle       = 0;
            otherEntity->rotation    = 0;
            otherEntity->controlLock = 0;
            scriptEng.checkResult    = 1;
        }
        else {
            sensors[0].collided = false;
            sensors[1].collided = false;
            sensors[0].XPos     = rx + otherLeft + 0x20000;
            sensors[1].XPos     = rx + otherRight - 0x20000;

            sensors[0].YPos = ry + otherTop;

            for (int i = 0; i < 2; ++i) {
                if (thisLeft < sensors[1].XPos && thisRight > sensors[0].XPos && thisBottom > sensors[0].YPos && thisEntity->YPos < sensors[0].YPos) {
                    sensors[i].collided = true;
                }
            }

            if (sensors[1].collided || sensors[0].collided) {
                if (!otherEntity->gravity && (otherEntity->collisionMode == CMODE_RWALL || otherEntity->collisionMode == CMODE_LWALL)) {
                    otherEntity->XVelocity = 0;
                    otherEntity->speed     = 0;
                }

                otherEntity->YPos = thisBottom - otherTop;
                if (otherEntity->YVelocity < 0)
                    otherEntity->YVelocity = 0;
                scriptEng.checkResult = 4;
            }
            else {
                sensors[0].collided = false;
                sensors[1].collided = false;
                sensors[0].XPos     = rx + otherRight;

                sensors[0].YPos = ry + otherTop + 0x20000;
                sensors[1].YPos = ry + otherBottom - 0x20000;
                for (int i = 0; i < 2; ++i) {
                    if (thisLeft <= sensors[0].XPos && thisEntity->XPos > sensors[0].XPos && thisTop < sensors[1].YPos
                        && thisBottom > sensors[0].YPos) {
                        sensors[i].collided = true;
                    }
                }

                if (sensors[1].collided || sensors[0].collided) {
                    otherEntity->XPos = thisLeft - otherRight;
                    if (otherEntity->XVelocity > 0) {
                        if (!otherEntity->direction)
                            otherEntity->pushing = 2;

                        otherEntity->XVelocity = 0;
                        otherEntity->speed     = 0;
                    }
                    scriptEng.checkResult = 2;
                }
                else {
                    sensors[0].collided = false;
                    sensors[1].collided = false;
                    sensors[0].XPos     = rx + otherLeft;

                    sensors[0].YPos = ry + otherTop + 0x20000;
                    sensors[1].YPos = ry + otherBottom - 0x20000;
                    for (int i = 0; i < 2; ++i) {
                        if (thisRight > sensors[0].XPos && thisEntity->XPos < sensors[0].XPos && thisTop < sensors[1].YPos
                            && thisBottom > sensors[0].YPos) {
                            sensors[i].collided = true;
                        }
                    }

                    if (sensors[1].collided || sensors[0].collided) {
                        otherEntity->XPos = thisRight - otherLeft;
                        if (otherEntity->XVelocity < 0) {
                            if (otherEntity->direction == FLIP_X)
                                otherEntity->pushing = 2;

                            if (otherEntity->XVelocity < -0x10000)
                                otherEntity->XPos += 0x8000;

                            otherEntity->XVelocity = 0;
                            otherEntity->speed     = 0;
                        }
                        scriptEng.checkResult = 3;
                    }
                }
            }
        }
    }
    else {
        sensors[0].collided = false;
        sensors[1].collided = false;
        sensors[0].XPos     = rx + otherRight;

        sensors[0].YPos = ry + otherTop + 0x20000;
        sensors[1].YPos = ry + otherBottom - 0x20000;
        for (int i = 0; i < 2; ++i) {
            if (thisLeft <= sensors[0].XPos && thisEntity->XPos > sensors[0].XPos && thisTop < sensors[1].YPos
                && thisBottom > sensors[0].YPos) {
                sensors[i].collided = true;
            }
        }
        if (sensors[1].collided || sensors[0].collided) {
            otherEntity->XPos = thisLeft - otherRight;
            if (otherEntity->XVelocity > 0) {
                if (!otherEntity->direction)
                    otherEntity->pushing = 2;

                otherEntity->XVelocity = 0;
                otherEntity->speed     = 0;
            }
            scriptEng.checkResult = 2;
        }
        else {
            sensors[0].collided = false;
            sensors[1].collided = false;
            sensors[0].XPos     = rx + otherLeft;

            sensors[0].YPos = ry + otherTop + 0x20000;
            sensors[1].YPos = ry + otherBottom - 0x20000;
            for (int i = 0; i < 2; ++i) {
                if (thisRight > sensors[0].XPos && thisEntity->XPos < sensors[0].XPos && thisTop < sensors[1].YPos
                    && thisBottom > sensors[0].YPos) {
                    sensors[i].collided = true;
                }
            }

            if (sensors[0].collided || sensors[1].collided) {
                otherEntity->XPos = thisRight - otherLeft;
                if (otherEntity->XVelocity < 0) {
                    if (otherEntity->direction == FLIP_X)
                        otherEntity->pushing = 2;

                    if (otherEntity->XVelocity < -0x10000)
                        otherEntity->XPos += 0x8000;

                    otherEntity->XVelocity = 0;
                    otherEntity->speed     = 0;
                }
                scriptEng.checkResult = 3;
            }
            else {
                sensors[0].collided = false;
                sensors[1].collided = false;
                sensors[2].collided = false;
                sensors[0].XPos     = rx + otherLeft + 0x20000;
                sensors[1].XPos     = rx;
                sensors[2].XPos     = rx + otherRight - 0x20000;

                sensors[0].YPos = ry + otherBottom;
                if (otherEntity->YVelocity >= 0) {
                    for (int i = 0; i < 3; ++i) {
                        if (thisLeft < sensors[i].XPos && thisRight > sensors[i].XPos && thisTop <= sensors[0].YPos && thisEntity->YPos > sensors[0].YPos) {
                            sensors[i].collided      = true;
                            otherEntity->flailing[i] = true;
                        }
                    }
                }

                if (sensors[0].collided || sensors[1].collided || sensors[2].collided) {
                    if (!otherEntity->gravity && (otherEntity->collisionMode == CMODE_RWALL || otherEntity->collisionMode == CMODE_LWALL)) {
                        otherEntity->XVelocity = 0;
                        otherEntity->speed     = 0;
                    }
                    otherEntity->YPos        = thisTop - otherBottom;
                    otherEntity->gravity     = 0;
                    otherEntity->YVelocity   = 0;
                    otherEntity->angle       = 0;
                    otherEntity->rotation    = 0;
                    otherEntity->controlLock = 0;
                    scriptEng.checkResult    = 1;
                }
                else {
                    sensors[0].collided = false;
                    sensors[1].collided = false;
                    sensors[0].XPos     = rx + otherLeft + 0x20000;
                    sensors[1].XPos     = rx + otherRight - 0x20000;

                    sensors[0].YPos     = ry + otherTop;

                    for (int i = 0; i < 2; ++i) {
                        if (thisLeft < sensors[1].XPos && thisRight > sensors[0].XPos && thisBottom > sensors[0].YPos
                            && thisEntity->YPos < sensors[0].YPos) {
                            sensors[i].collided = true;
                        }
                    }

                    if (sensors[1].collided || sensors[0].collided) {
                        if (!otherEntity->gravity && (otherEntity->collisionMode == CMODE_RWALL || otherEntity->collisionMode == CMODE_LWALL)) {
                            otherEntity->XVelocity = 0;
                            otherEntity->speed     = 0;
                        }

                        otherEntity->YPos = thisBottom - otherTop;

                        if (otherEntity->YVelocity < 0)
                            otherEntity->YVelocity = 0;
                        scriptEng.checkResult = 4;
                    }
                }
            }
        }
    }

}
void PlatformCollision(Entity *thisEntity, int thisLeft, int thisTop, int thisRight, int thisBottom, Entity *otherEntity, int otherLeft, int otherTop,
                       int otherRight, int otherBottom)
{
    scriptEng.checkResult = false;

    Hitbox *thisHitbox  = getHitbox(thisEntity);
    Hitbox *otherHitbox = getHitbox(otherEntity);

    if (thisLeft == 0x10000)
        thisLeft = thisHitbox->left[0];

    if (thisTop == 0x10000)
        thisTop = thisHitbox->top[0];

    if (thisRight == 0x10000)
        thisRight = thisHitbox->right[0];

    if (thisBottom == 0x10000)
        thisBottom = thisHitbox->bottom[0];
    thisLeft += thisEntity->XPos >> 16;
    thisTop += thisEntity->YPos >> 16;
    thisRight += thisEntity->XPos >> 16;
    thisBottom += thisEntity->YPos >> 16;

    thisLeft <<= 16;
    thisTop <<= 16;
    thisRight <<= 16;
    thisBottom <<= 16;

    if (otherLeft == 0x10000)
        otherLeft = otherHitbox->left[0];

    if (otherTop == 0x10000)
        otherTop = otherHitbox->top[0];

    if (otherRight == 0x10000)
        otherRight = otherHitbox->right[0];

    if (otherBottom == 0x10000)
        otherBottom = otherHitbox->bottom[0];

    sensors[0].collided = false;
    sensors[1].collided = false;
    sensors[2].collided = false;

    int rx = otherEntity->XPos >> 16 << 16;
    int ry = otherEntity->YPos >> 16 << 16;

    sensors[0].XPos = rx + (otherLeft << 16);
    sensors[1].XPos = rx;
    sensors[2].XPos = rx + (otherRight << 16);
    sensors[3].XPos = (rx + sensors[0].XPos) >> 1;
    sensors[4].XPos = (sensors[2].XPos + rx) >> 1;

    sensors[0].YPos = (otherBottom << 16) + ry;

    for (int i = 0; i < 5; ++i) {
        if (thisLeft < sensors[i].XPos && thisRight > sensors[i].XPos && thisTop - 1 <= sensors[0].YPos && thisBottom > sensors[0].YPos
            && otherEntity->YVelocity >= 0) {
            sensors[i].collided      = true;
            otherEntity->flailing[i] = true;
        }
    }

    if (sensors[0].collided || sensors[1].collided || sensors[2].collided) {
        if (!otherEntity->gravity && (otherEntity->collisionMode == CMODE_RWALL || otherEntity->collisionMode == CMODE_LWALL)) {
            otherEntity->XVelocity = 0;
            otherEntity->speed     = 0;
        }
        otherEntity->YPos        = thisTop - (otherBottom << 16);
        otherEntity->gravity     = 0;
        otherEntity->YVelocity   = 0;
        otherEntity->angle       = 0;
        otherEntity->rotation    = 0;
        otherEntity->controlLock = 0;
        scriptEng.checkResult    = 1;
    }
}
