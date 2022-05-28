#ifndef RENDERER_H
#define RENDERER_H

#define DRAWFACE_COUNT    (0x1000)
#define DRAWVERTEX_COUNT  (DRAWFACE_COUNT * 4)
#define DRAWINDEX_COUNT   (DRAWFACE_COUNT * 6)
#define TEXTURE_COUNT     (0x80)
#define MESH_COUNT        (0x40)
#define RENDERSTATE_COUNT (0x100)

enum RenderBlendModes {
    RENDER_BLEND_NONE,
    RENDER_BLEND_ALPHA,
    RENDER_BLEND_ALPHA2,
    RENDER_BLEND_ALPHA3,
};

enum TextureFormats {
    TEXFMT_NONE,
    TEXFMT_RGBA4444,
    TEXFMT_RGBA5551,
    TEXFMT_RGBA8888,
    TEXFMT_RETROBUFFER,
};

enum MeshRenderTypes {
    MESH_COLORS,
    MESH_NORMALS,
    MESH_COLORS_NORMALS,
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
    byte useColors;
    byte depthTest;
    byte useNormals;
    byte useFilter;
};

extern float retroVertexList[40];
extern float screenBufferVertexList[40];

extern int vertexListSize;
extern DrawVertex drawVertexList[DRAWVERTEX_COUNT];
extern ushort drawIndexList[DRAWINDEX_COUNT];

extern byte vertexR;
extern byte vertexG;
extern byte vertexB;

extern TextureInfo textureList[TEXTURE_COUNT];
extern MeshInfo meshList[MESH_COUNT];

extern int renderStateCount;
extern RenderState renderStateList[RENDERSTATE_COUNT];
extern RenderState currentRenderState;

// Matricies
void SetIdentityMatrixF(MatrixF *matrix);
void MatrixMultiplyF(MatrixF *matrixA, MatrixF *matrixB);
void MatrixTranslateXYZF(MatrixF *Matrix, float x, float y, float z);
void MatrixScaleXYZF(MatrixF *matrix, float scaleX, float scaleY, float scaleZ);
void MatrixRotateXF(MatrixF *matrix, float rotationX);
void MatrixRotateYF(MatrixF *matrix, float rotationY);
void MatrixRotateZF(MatrixF *matrix, float rotationZ);
void MatrixRotateXYZF(MatrixF *matrix, float rotationX, float rotationY, float rotationZ);
void MatrixInvertF(MatrixF *dstMatrix, MatrixF *matrix);

// Render States
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
void ReplaceTexture(const char *filePath, int textureID);
void ClearTextures(bool keepBuffer);

// Meshes
MeshInfo *LoadMesh(const char *filePath, byte textureID);
void SetMeshVertexColors(MeshInfo *mesh, byte r, byte g, byte b, byte a);
void ClearMeshData();
void SetMeshAnimation(MeshInfo *mesh, MeshAnimator *animator, ushort frameID, ushort frameCount, float speed);
void AnimateMesh(MeshInfo *mesh, MeshAnimator *animator);

// Rendering
void TransferRetroBuffer();
void RenderRetroBuffer(int alpha, float z);
void RenderImage(float x, float y, float z, float scaleX, float scaleY, float pivotX, float pivotY, float sprW, float sprH, float sprX, float sprY,
                 int alpha, byte texture);
void RenderImageClipped(float x, float y, float z, float scaleX, float scaleY, float pivotX, float pivotY, float sprW, float sprH, float sprX,
                        float sprY, int alpha, byte texture);
void RenderImageFlipH(float x, float y, float z, float scaleX, float scaleY, float pivotX, float pivotY, float sprW, float sprH, float sprX,
                      float sprY, int alpha, byte texture);
void RenderText(ushort *text, int fontID, float x, float y, int z, float scale, int alpha);
void RenderTextClipped(ushort *text, int fontID, float x, float y, int z, float scale, int alpha);
void RenderRect(float x, float y, float z, float w, float h, byte r, byte g, byte b, int alpha);
#if !RETRO_USE_ORIGINAL_CODE
void RenderRectClipped(float x, float y, float z, float w, float h, byte r, byte g, byte b, int alpha);
#endif
void RenderMesh(MeshInfo *mesh, byte type, byte depthTest);

#endif // !RENDERER_H