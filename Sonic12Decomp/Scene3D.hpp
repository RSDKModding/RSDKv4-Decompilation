#ifndef DRAWING3D_H
#define DRAWING3D_H

#define VERTEXBUFFER_SIZE (0x1000)
#define FACEBUFFER_SIZE (0x400)

enum FaceFlags {
    FACE_FLAG_TEXTURED_3D = 0,
    FACE_FLAG_TEXTURED_2D = 1,
    FACE_FLAG_COLOURED_3D = 2,
    FACE_FLAG_COLOURED_2D = 3,
    FACE_FLAG_FADED       = 4,
    FACE_FLAG_TEXTURED_C  = 5,
    FACE_FLAG_TEXTURED_D  = 6,
    FACE_FLAG_3DSPRITE    = 7
};

enum MatrixTypes {
    MAT_WORLD = 0,
    MAT_VIEW = 1,
    MAT_TEMP = 2,
};

struct Matrix {
    int values[4][4];
};

struct Vertex {
    int x;
    int y;
    int z;
    int u;
    int v;
};

struct Face {
    int a;
    int b;
    int c;
    int d;
    int colour;
    int flags;
};

struct DrawListEntry3D {
    int faceID;
    int depth;
};

extern int vertexCount;
extern int faceCount;

extern Matrix matFinal;
extern Matrix matWorld;
extern Matrix matView;
extern Matrix matTemp;

extern Face faceBuffer[FACEBUFFER_SIZE];
extern Vertex vertexBuffer[VERTEXBUFFER_SIZE];
extern Vertex vertexBufferT[VERTEXBUFFER_SIZE];

extern DrawListEntry3D drawList3D[FACEBUFFER_SIZE];

extern int projectionX;
extern int projectionY;
extern int fogColour;
extern int fogStrength;

extern int faceLineStart[SCREEN_YSIZE];
extern int faceLineEnd[SCREEN_YSIZE];
extern int faceLineStartU[SCREEN_YSIZE];
extern int faceLineEndU[SCREEN_YSIZE];
extern int faceLineStartV[SCREEN_YSIZE];
extern int faceLineEndV[SCREEN_YSIZE];

void setIdentityMatrix(Matrix *matrix);
void matrixMultiply(Matrix *matrixA, Matrix *matrixB);
void matrixTranslateXYZ(Matrix *Matrix, int XPos, int YPos, int ZPos);
void matrixScaleXYZ(Matrix *matrix, int scaleX, int scaleY, int scaleZ);
void matrixRotateX(Matrix *matrix, int rotationX);
void matrixRotateY(Matrix *matrix, int rotationY);
void matrixRotateZ(Matrix *matrix, int rotationZ);
void matrixRotateXYZ(Matrix *matrix, int rotationX, int rotationY, int rotationZ);
void matrixInverse(Matrix *matrix);
void transformVertexBuffer();
void transformVerticies(Matrix *matrix, int startIndex, int endIndex);
void sort3DDrawList();
void draw3DScene(int spriteSheetID);

void processScanEdge(Vertex *vertA, Vertex *vertB);
void processScanEdgeUV(Vertex *vertA, Vertex *vertB);

#endif // !DRAWING3D_H
