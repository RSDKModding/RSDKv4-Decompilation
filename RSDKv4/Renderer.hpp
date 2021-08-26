#ifndef RENDERER_H
#define RENDERER_H

#define DRAWFACE_LIMIT    (0x1000)
#define DRAWVERTEX_LIMIT  (DRAWFACE_LIMIT * 4)
#define DRAWINDEX_LIMIT   (DRAWFACE_LIMIT * 6)
#define TEXTURE_LIMIT (0x80)
#define MESH_LIMIT (0x40)
#define RENDERSTATE_LIMIT (0x100)

enum RenderBlendModes {
    RENDER_BLEND_NONE,
    RENDER_BLEND_ALPHA,
    RENDER_BLEND_ALPHA2,
    RENDER_BLEND_ALPHA3,
};

struct DrawVertex {
    float vertX;
    float vertY;
    float vertZ;
    float normalX;
    float normalY;
    float normalZ;
    float texCoordX;
    float texCoordY;
    byte r;
    byte g;
    byte b;
    byte a;
};

struct MatrixF {
    float values[4][4];
};

struct TextureInfo {
    char fileName[64];
    int width;
    int height;
    float widthN;
    float heightN;
    int format;
    uint id;
};

struct MeshVertex {
    float vertX;
    float vertY;
    float vertZ;
    float normalX;
    float normalY;
    float normalZ;
};

struct MeshInfo {
    char fileName[64];
    DrawVertex *vertices;
    ushort *indices;
    ushort vertexCount;
    ushort indexCount;
    MeshVertex *frames;
    ushort frameCount;
    byte textureID;
};

struct MeshAnimator {
    float animationSpeed;
    float animationTimer;
    ushort frameID;
    ushort loopIndex;
    ushort frameCount;
    byte loopAnimation;
    byte animationFinished;
};

struct RenderState {
    MatrixF *renderMatrix;
    DrawVertex *vertPtr;
    ushort *indexPtr;
    ushort indexCount;
    int id;
    byte blendMode;
    byte useTexture;
    byte useColours;
    byte depthTest;
    byte useNormals;
    byte useFilter;
};

extern float retroVertexList[40];
extern float screenBufferVertexList[40];

extern int vertexListSize;
extern DrawVertex drawVertexList[DRAWVERTEX_LIMIT];
extern ushort drawIndexList[DRAWINDEX_LIMIT];

extern byte vertexR;
extern byte vertexG;
extern byte vertexB;

extern TextureInfo textureList[TEXTURE_LIMIT];
extern int textureCount;

extern MeshInfo meshList[MESH_LIMIT];
extern int meshCount;

extern int renderStateCount;
extern RenderState renderStateList[RENDERSTATE_LIMIT];
extern RenderState currentRenderState;

//Matricies
void setIdentityMatrixF(MatrixF *matrixA);
void matrixMultiplyF(MatrixF *matrixA, MatrixF *matrixB);
void matrixTranslateXYZF(MatrixF *Matrix, float XPos, float YPos, float ZPos);
void matrixScaleXYZF(MatrixF *matrix, float scaleX, float scaleY, float scaleZ);
void matrixRotateXF(MatrixF *matrix, float rotationX);
void matrixRotateYF(MatrixF *matrix, float rotationY);
void matrixRotateZF(MatrixF *matrix, float rotationZ);
void matrixRotateXYZF(MatrixF *matrix, float rotationX, float rotationY, float rotationZ);

//Render States
void ResetRenderStates();
void SetRenderBlendMode(byte mode);
void SetRenderVertexColor(byte r, byte g, byte b);
void SetPerspectiveMatrix(float w, float h, float near, float far);
void SetupDrawIndexList();
void SetRenderMatrix(MatrixF *matrix);
void NewRenderState();
void RenderScene();

// Textures
int LoadTexture(const char *filePath, int format);
void ClearTextures();

// Meshes
MeshInfo *LoadMesh(const char *filePath, byte textureID);
void SetMeshVertexColors(MeshInfo *mesh, byte r, byte g, byte b, byte a);
void ClearMeshData();
void SetMeshAnimation(MeshInfo *mesh, MeshAnimator *animator, ushort frameID, ushort frameCount, float speed);
void AnimateMesh(MeshInfo *mesh, MeshAnimator *animator);

//Rendering
void TransferRetroBuffer();
void RenderRetroBuffer(int alpha, float z);
void RenderImage(float x, float y, float z, float scaleX, float scaleY, float pivotX, float pivotY, float sprW, float sprH, float sprX, float sprY,
                 int alpha, byte texture);
void RenderText(ushort *text, int fontID, float x, float y, int z, float scale, int alpha);
void RenderRect(float x, float y, float z, float w, float h, byte r, byte g, byte b, int alpha);
void RenderMesh(MeshInfo *mesh, byte a2, byte a3);

#endif // !RENDERER_H