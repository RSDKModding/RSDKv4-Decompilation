#include "RetroEngine.hpp"

int vertexCount = 0;
int faceCount   = 0;

Matrix matFinal = Matrix();
Matrix matWorld = Matrix();
Matrix matView  = Matrix();
Matrix matTemp  = Matrix();

Face faceBuffer[FACEBUFFER_SIZE];
Vertex vertexBuffer[VERTEXBUFFER_SIZE];
Vertex vertexBufferT[VERTEXBUFFER_SIZE];

DrawListEntry3D drawList3D[FACEBUFFER_SIZE];

int projectionX = 136;
int projectionY = 160;
int fogColour   = 0;
int fogStrength = 0;

int faceLineStart[SCREEN_YSIZE];
int faceLineEnd[SCREEN_YSIZE];
int faceLineStartU[SCREEN_YSIZE];
int faceLineEndU[SCREEN_YSIZE];
int faceLineStartV[SCREEN_YSIZE];
int faceLineEndV[SCREEN_YSIZE];

void setIdentityMatrix(Matrix *matrix)
{
    matrix->values[0][0] = 0x100;
    matrix->values[0][1] = 0;
    matrix->values[0][2] = 0;
    matrix->values[0][3] = 0;
    matrix->values[1][0] = 0;
    matrix->values[1][1] = 0x100;
    matrix->values[1][2] = 0;
    matrix->values[1][3] = 0;
    matrix->values[2][0] = 0;
    matrix->values[2][1] = 0;
    matrix->values[2][2] = 0x100;
    matrix->values[2][3] = 0;
    matrix->values[3][0] = 0;
    matrix->values[3][0] = 0;
    matrix->values[3][2] = 0;
    matrix->values[3][3] = 0x100;
}
void matrixMultiply(Matrix *matrixA, Matrix *matrixB)
{
    int output[16];

    for (int i = 0; i < 0x10; ++i) {
        uint RowB = i & 3;
        uint RowA = i & 0xC;
        output[i] = (matrixA->values[0][RowA + 3] * matrixB->values[3][RowB] >> 8) + (matrixA->values[0][RowA + 2] * matrixB->values[2][RowB] >> 8)
                    + (matrixA->values[0][RowA + 1] * matrixB->values[1][RowB] >> 8) + (matrixA->values[0][RowA] * matrixB->values[0][RowB] >> 8);
    }

    for (int i = 0; i < 0x10; ++i) matrixA->values[i / 4][i % 4] = output[i];
}
void matrixTranslateXYZ(Matrix *matrix, int XPos, int YPos, int ZPos)
{
    matrix->values[0][0] = 0x100;
    matrix->values[0][1] = 0;
    matrix->values[0][2] = 0;
    matrix->values[0][3] = 0;
    matrix->values[1][0] = 0;
    matrix->values[1][1] = 0x100;
    matrix->values[1][2] = 0;
    matrix->values[1][3] = 0;
    matrix->values[2][0] = 0;
    matrix->values[2][1] = 0;
    matrix->values[2][2] = 0x100;
    matrix->values[2][3] = 0;
    matrix->values[3][0] = XPos;
    matrix->values[3][1] = YPos;
    matrix->values[3][2] = ZPos;
    matrix->values[3][3] = 0x100;
}
void matrixScaleXYZ(Matrix *matrix, int scaleX, int scaleY, int scaleZ)
{
    matrix->values[0][0] = scaleX;
    matrix->values[0][1] = 0;
    matrix->values[0][2] = 0;
    matrix->values[0][3] = 0;
    matrix->values[1][0] = 0;
    matrix->values[1][1] = scaleY;
    matrix->values[1][2] = 0;
    matrix->values[1][3] = 0;
    matrix->values[2][0] = 0;
    matrix->values[2][1] = 0;
    matrix->values[2][2] = scaleZ;
    matrix->values[2][3] = 0;
    matrix->values[3][0] = 0;
    matrix->values[3][1] = 0;
    matrix->values[3][2] = 0;
    matrix->values[3][3] = 0x100;
}
void matrixRotateX(Matrix *matrix, int rotationX)
{
    if (rotationX < 0)
        rotationX = 0x200 - rotationX;
    rotationX &= 0x1FF;
    int sine             = sinVal512[rotationX] >> 1;
    int cosine           = cosVal512[rotationX] >> 1;
    matrix->values[0][0] = 0x100;
    matrix->values[0][1] = 0;
    matrix->values[0][2] = 0;
    matrix->values[0][3] = 0;
    matrix->values[1][0] = 0;
    matrix->values[1][1] = cosine;
    matrix->values[1][2] = sine;
    matrix->values[1][3] = 0;
    matrix->values[2][0] = 0;
    matrix->values[2][1] = -sine;
    matrix->values[2][2] = cosine;
    matrix->values[2][3] = 0;
    matrix->values[3][0] = 0;
    matrix->values[3][1] = 0;
    matrix->values[3][2] = 0;
    matrix->values[3][3] = 0x100;
}
void matrixRotateY(Matrix *matrix, int rotationY)
{
    if (rotationY < 0)
        rotationY = 0x200 - rotationY;
    rotationY &= 0x1FF;
    int sine             = sinVal512[rotationY] >> 1;
    int cosine           = cosVal512[rotationY] >> 1;
    matrix->values[0][0] = cosine;
    matrix->values[0][1] = 0;
    matrix->values[0][2] = sine;
    matrix->values[0][3] = 0;
    matrix->values[1][0] = 0;
    matrix->values[1][1] = 0x100;
    matrix->values[1][2] = 0;
    matrix->values[1][3] = 0;
    matrix->values[2][0] = -sine;
    matrix->values[2][1] = 0;
    matrix->values[2][2] = cosine;
    matrix->values[2][3] = 0;
    matrix->values[3][0] = 0;
    matrix->values[3][1] = 0;
    matrix->values[3][2] = 0;
    matrix->values[3][3] = 0x100;
}
void matrixRotateZ(Matrix *matrix, int rotationZ)
{
    if (rotationZ < 0)
        rotationZ = 0x200 - rotationZ;
    rotationZ &= 0x1FF;
    int sine            = sinVal512[rotationZ] >> 1;
    int cosine          = cosVal512[rotationZ] >> 1;
    matrix->values[0][0] = cosine;
    matrix->values[0][1] = 0;
    matrix->values[0][2] = sine;
    matrix->values[0][3] = 0;
    matrix->values[1][0] = 0;
    matrix->values[1][1] = 0x100;
    matrix->values[1][2] = 0;
    matrix->values[1][3] = 0;
    matrix->values[2][0] = -sine;
    matrix->values[2][1] = 0;
    matrix->values[2][2] = cosine;
    matrix->values[2][3] = 0;
    matrix->values[3][0] = 0;
    matrix->values[3][1] = 0;
    matrix->values[3][2] = 0;
    matrix->values[3][3] = 0x100;
}
void matrixRotateXYZ(Matrix *matrix, int rotationX, int rotationY, int rotationZ)
{
    if (rotationX < 0)
        rotationX = 0x200 - rotationX;
    rotationX &= 0x1FF;
    if (rotationY < 0)
        rotationY = 0x200 - rotationY;
    rotationY &= 0x1FF;
    if (rotationZ < 0)
        rotationZ = 0x200 - rotationZ;
    rotationZ &= 0x1FF;
    int sineX   = sinVal512[rotationX] >> 1;
    int cosineX = cosVal512[rotationX] >> 1;
    int sineY   = sinVal512[rotationY] >> 1;
    int cosineY = cosVal512[rotationY] >> 1;
    int sineZ   = sinVal512[rotationZ] >> 1;
    int cosineZ = cosVal512[rotationZ] >> 1;

    matrix->values[0][0] = (sineZ * (sineY * sineX >> 8) >> 8) + (cosineZ * cosineY >> 8);
    matrix->values[0][1] = (sineZ * cosineY >> 8) - (cosineZ * (sineY * sineX >> 8) >> 8);
    matrix->values[0][2] = sineY * cosineX >> 8;
    matrix->values[0][3] = 0;
    matrix->values[1][0] = sineZ * -cosineX >> 8;
    matrix->values[1][1] = cosineZ * cosineX >> 8;
    matrix->values[1][2] = sineX;
    matrix->values[1][3] = 0;
    matrix->values[2][0] = (sineZ * (cosineY * sineX >> 8) >> 8) - (cosineZ * sineY >> 8);
    matrix->values[2][1] = (sineZ * -sineY >> 8) - (cosineZ * (cosineY * sineX >> 8) >> 8);
    matrix->values[2][2] = cosineY * cosineX >> 8;
    matrix->values[2][3] = 0;
    matrix->values[3][0] = 0;
    matrix->values[3][1] = 0;
    matrix->values[3][2] = 0;
    matrix->values[3][3] = 0x100;
}
void matrixInverse(Matrix* matrix) {

}
void transformVertexBuffer()
{
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            matFinal.values[y][x] = matWorld.values[y][x];
        }
    }
    matrixMultiply(&matFinal, &matView);

    if (vertexCount <= 0)
        return;

    int inVertexID  = 0;
    int outVertexID = 0;
    do {
        int vx       = vertexBuffer[inVertexID].x;
        int vy       = vertexBuffer[inVertexID].y;
        int vz       = vertexBuffer[inVertexID].z;
        Vertex *vert = &vertexBufferT[inVertexID++];

        vert->x = (vx * matFinal.values[0][0] >> 8) + (vy * matFinal.values[1][0] >> 8) + (vz * matFinal.values[2][0] >> 8) + matFinal.values[3][0];
        vert->y = (vx * matFinal.values[0][1] >> 8) + (vy * matFinal.values[1][1] >> 8) + (vz * matFinal.values[2][1] >> 8) + matFinal.values[3][1];
        vert->z = (vx * matFinal.values[0][2] >> 8) + (vy * matFinal.values[1][2] >> 8) + (vz * matFinal.values[2][2] >> 8) + matFinal.values[3][2];
        if (vert->z < 1 && vert->z > 0)
            vert->z = 1;
    } while (++outVertexID != vertexCount);
}
void transformVerticies(Matrix *matrix, int startIndex, int endIndex)
{
    if (startIndex > endIndex)
        return;

    int inVertexID  = 0;
    int outVertexID = 0;
    do {
        int vx       = vertexBuffer[inVertexID].x;
        int vy       = vertexBuffer[inVertexID].y;
        int vz       = vertexBuffer[inVertexID].z;
        Vertex *vert = &vertexBuffer[inVertexID++];
        vert->x      = (vx * matrix->values[0][0] >> 8) + (vy * matrix->values[1][0] >> 8) + (vz * matrix->values[2][0] >> 8) + matrix->values[3][0];
        vert->y      = (vx * matrix->values[0][1] >> 8) + (vy * matrix->values[1][1] >> 8) + (vz * matrix->values[2][1] >> 8) + matrix->values[3][1];
        vert->z      = (vx * matrix->values[0][2] >> 8) + (vy * matrix->values[1][2] >> 8) + (vz * matrix->values[2][2] >> 8) + matrix->values[3][2];
    } while (++startIndex < endIndex);
}
void sort3DDrawList()
{
    for (int i = 0; i < faceCount; ++i) {
        drawList3D[i].depth = (vertexBufferT[faceBuffer[i].d].z + vertexBufferT[faceBuffer[i].c].z + vertexBufferT[faceBuffer[i].b].z
                               + vertexBufferT[faceBuffer[i].a].z)
                              >> 2;
        drawList3D[i].faceID = i;
    }

    for (int i = 0; i < faceCount; ++i) {
        for (int j = faceCount - 1; j > i; --j) {
            if (drawList3D[j].depth > drawList3D[j - 1].depth) {
                int faceID               = drawList3D[j].faceID;
                int depth                = drawList3D[j].depth;
                drawList3D[j].faceID     = drawList3D[j - 1].faceID;
                drawList3D[j].depth      = drawList3D[j - 1].depth;
                drawList3D[j - 1].faceID = faceID;
                drawList3D[j - 1].depth  = depth;
            }
        }
    }
}
void draw3DScene(int spriteSheetID)
{
    Vertex quad[4];
    for (int i = 0; i < faceCount; ++i) {
        Face *face = &faceBuffer[drawList3D[i].faceID];
        memset(quad, 0, 4 * sizeof(Vertex));
        switch (face->flags) {
            default: break;
            case FACE_FLAG_TEXTURED_3D:
                if (vertexBufferT[face->a].z > 0x100 && vertexBufferT[face->b].z > 0x100 && vertexBufferT[face->c].z > 0x100
                    && vertexBufferT[face->d].z > 0x100) {
                    quad[0].x = SCREEN_CENTERX + projectionX * vertexBufferT[face->a].x / vertexBufferT[face->a].z;
                    quad[0].y = SCREEN_CENTERY - projectionY * vertexBufferT[face->a].y / vertexBufferT[face->a].z;
                    quad[1].x = SCREEN_CENTERX + projectionX * vertexBufferT[face->b].x / vertexBufferT[face->b].z;
                    quad[1].y = SCREEN_CENTERY - projectionY * vertexBufferT[face->b].y / vertexBufferT[face->b].z;
                    quad[2].x = SCREEN_CENTERX + projectionX * vertexBufferT[face->c].x / vertexBufferT[face->c].z;
                    quad[2].y = SCREEN_CENTERY - projectionY * vertexBufferT[face->c].y / vertexBufferT[face->c].z;
                    quad[3].x = SCREEN_CENTERX + projectionX * vertexBufferT[face->d].x / vertexBufferT[face->d].z;
                    quad[3].y = SCREEN_CENTERY - projectionY * vertexBufferT[face->d].y / vertexBufferT[face->d].z;
                    quad[0].u = vertexBuffer[face->a].u;
                    quad[0].v = vertexBuffer[face->a].v;
                    quad[1].u = vertexBuffer[face->b].u;
                    quad[1].v = vertexBuffer[face->b].v;
                    quad[2].u = vertexBuffer[face->c].u;
                    quad[2].v = vertexBuffer[face->c].v;
                    quad[3].u = vertexBuffer[face->d].u;
                    quad[3].v = vertexBuffer[face->d].v;
                    DrawTexturedFace(quad, spriteSheetID);
                }
                break;
            case FACE_FLAG_TEXTURED_2D:
                quad[0].x = vertexBuffer[face->a].x;
                quad[0].y = vertexBuffer[face->a].y;
                quad[1].x = vertexBuffer[face->b].x;
                quad[1].y = vertexBuffer[face->b].y;
                quad[2].x = vertexBuffer[face->c].x;
                quad[2].y = vertexBuffer[face->c].y;
                quad[3].x = vertexBuffer[face->d].x;
                quad[3].y = vertexBuffer[face->d].y;
                quad[0].u = vertexBuffer[face->a].u;
                quad[0].v = vertexBuffer[face->a].v;
                quad[1].u = vertexBuffer[face->b].u;
                quad[1].v = vertexBuffer[face->b].v;
                quad[2].u = vertexBuffer[face->c].u;
                quad[2].v = vertexBuffer[face->c].v;
                quad[3].u = vertexBuffer[face->d].u;
                quad[3].v = vertexBuffer[face->d].v;
                DrawTexturedFace(quad, spriteSheetID);
                break;
            case FACE_FLAG_COLOURED_3D:
                if (vertexBufferT[face->a].z > 0x100 && vertexBufferT[face->b].z > 0x100 && vertexBufferT[face->c].z > 0x100
                    && vertexBufferT[face->d].z > 0x100) {
                    quad[0].x = SCREEN_CENTERX + projectionX * vertexBufferT[face->a].x / vertexBufferT[face->a].z;
                    quad[0].y = SCREEN_CENTERY - projectionY * vertexBufferT[face->a].y / vertexBufferT[face->a].z;
                    quad[1].x = SCREEN_CENTERX + projectionX * vertexBufferT[face->b].x / vertexBufferT[face->b].z;
                    quad[1].y = SCREEN_CENTERY - projectionY * vertexBufferT[face->b].y / vertexBufferT[face->b].z;
                    quad[2].x = SCREEN_CENTERX + projectionX * vertexBufferT[face->c].x / vertexBufferT[face->c].z;
                    quad[2].y = SCREEN_CENTERY - projectionY * vertexBufferT[face->c].y / vertexBufferT[face->c].z;
                    quad[3].x = SCREEN_CENTERX + projectionX * vertexBufferT[face->d].x / vertexBufferT[face->d].z;
                    quad[3].y = SCREEN_CENTERY - projectionY * vertexBufferT[face->d].y / vertexBufferT[face->d].z;
                    DrawFace(quad, face->colour);
                }
                break;
            case FACE_FLAG_COLOURED_2D:
                quad[0].x = vertexBuffer[face->a].x;
                quad[0].y = vertexBuffer[face->a].y;
                quad[1].x = vertexBuffer[face->b].x;
                quad[1].y = vertexBuffer[face->b].y;
                quad[2].x = vertexBuffer[face->c].x;
                quad[2].y = vertexBuffer[face->c].y;
                quad[3].x = vertexBuffer[face->d].x;
                quad[3].y = vertexBuffer[face->d].y;
                DrawFace(quad, face->colour);
                break;
            case FACE_FLAG_FADED: break;
            case FACE_FLAG_TEXTURED_C: break;
            case FACE_FLAG_TEXTURED_D: break;
            case FACE_FLAG_3DSPRITE: break;
        }
    }
}

void processScanEdge(Vertex *vertA, Vertex *vertB)
{
    int bottom, top;
    int fullX, fullY;
    int trueX, yDifference;

    if (vertA->y == vertB->y)
        return;
    if (vertA->y >= vertB->y) {
        top    = vertB->y;
        bottom = vertA->y + 1;
    }
    else {
        top    = vertA->y;
        bottom = vertB->y + 1;
    }
    if (top > SCREEN_YSIZE - 1 || bottom < 0)
        return;
    if (bottom > 240)
        bottom = 240;
    fullX       = vertA->x << 16;
    yDifference = vertB->y - vertA->y;
    fullY       = ((vertB->x - vertA->x) << 16) / yDifference;
    if (top < 0) {
        fullX -= top * fullY;
        top = 0;
    }
    for (int i = top; i < bottom; ++i) {
        trueX = fullX >> 16;
        if (fullX >> 16 < faceLineStart[i])
            faceLineStart[i] = trueX;
        if (trueX > faceLineEnd[i])
            faceLineEnd[i] = trueX;
        fullX += fullY;
    }
}
void processScanEdgeUV(Vertex *vertA, Vertex *vertB)
{
    int bottom, top;
    int fullX, fullY, fullU, fullV;
    int trueX, trueU, trueV, yDifference;

    if (vertA->y == vertB->y)
        return;
    if (vertA->y >= vertB->y) {
        top    = vertB->y;
        bottom = vertA->y + 1;
    }
    else {
        top    = vertA->y;
        bottom = vertB->y + 1;
    }
    if (top > SCREEN_YSIZE - 1 || bottom < 0)
        return;
    if (bottom > SCREEN_YSIZE)
        bottom = SCREEN_YSIZE;
    fullX      = vertA->x << 16;
    fullU      = vertA->u << 16;
    fullV      = vertA->v << 16;
    int finalX = ((vertB->x - vertA->x) << 16) / (vertB->y - vertA->y);
    if (vertA->u == vertB->u)
        trueU = 0;
    else
        trueU = ((vertB->u - vertA->u) << 16) / (vertB->y - vertA->y);

    if (vertA->v == vertB->v) {
        trueV = 0;
    }
    else {
        yDifference = vertB->y - vertA->y;
        trueV       = ((vertB->v - vertA->v) << 16) / yDifference;
    }
    if (top < 0) {
        fullX -= top * finalX;
        fullU -= top * trueU;
        fullV -= top * trueV;
        top = 0;
    }
    for (int i = top; i < bottom; ++i) {
        trueX = fullX >> 16;
        if (fullX >> 16 < faceLineStart[i]) {
            faceLineStart[i]  = trueX;
            faceLineStartU[i] = fullU;
            faceLineStartV[i] = fullV;
        }
        if (trueX > faceLineEnd[i]) {
            faceLineEnd[i]  = trueX;
            faceLineEndU[i] = fullU;
            faceLineEndV[i] = fullV;
        }
        fullX += finalX;
        fullU += trueU;
        fullV += trueV;
    }
}