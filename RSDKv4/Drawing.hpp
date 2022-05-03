#ifndef DRAWING_H
#define DRAWING_H

#define SURFACE_MAX (24)
#define GFXDATA_MAX (0x2000 * 0x2000)

#define DRAWLAYER_COUNT (7)

enum FlipFlags { FLIP_NONE, FLIP_X, FLIP_Y, FLIP_XY };
enum InkFlags { INK_NONE, INK_BLEND, INK_ALPHA, INK_ADD, INK_SUB };
enum DrawFXFlags { FX_SCALE, FX_ROTATE, FX_ROTOZOOM, FX_INK, FX_TINT, FX_FLIP };

struct DrawListEntry {
    int entityRefs[ENTITY_COUNT];
    int listSize;
};

struct GFXSurface {
    char fileName[0x40];
    int height;
    int width;
#if RETRO_SOFTWARE_RENDER
    int widthShift;
#endif
#if RETRO_HARDWARE_RENDER
    int texStartX;
    int texStartY;
#endif
    int depth;
    int dataPosition;
};

struct DisplaySettings {
    byte field_0;
    int offsetX;
    int width;
    int height;
    int field_10;
    int field_14;
    int field_18;
    int maxWidth;
    byte field_20;
    int field_24;
    int field_28;
    int field_2C;
    int field_30;
    int field_34;
    int field_38;
    int field_3C;
};

extern ushort blendLookupTable[0x20 * 0x100];
extern ushort subtractLookupTable[0x20 * 0x100];
extern ushort tintLookupTable[0x10000];

extern int SCREEN_XSIZE_CONFIG;
extern int SCREEN_XSIZE;
extern int SCREEN_CENTERX;

extern bool windowCreated;

extern float SCREEN_XSIZE_F;
extern float SCREEN_CENTERX_F;

extern float SCREEN_YSIZE_F;
extern float SCREEN_CENTERY_F;

extern int touchWidth;
extern int touchHeight;
extern float touchWidthF;
extern float touchHeightF;

extern DrawListEntry drawListEntries[DRAWLAYER_COUNT];

extern int gfxDataPosition;
extern GFXSurface gfxSurface[SURFACE_MAX];
extern byte graphicData[GFXDATA_MAX];

extern DisplaySettings displaySettings;
extern bool convertTo32Bit;
extern bool mixFiltersOnJekyll;

#if RETRO_USING_OPENGL
extern GLint defaultFramebuffer;
extern GLuint framebufferHiRes;
extern GLuint renderbufferHiRes;
#endif

#if RETRO_HARDWARE_RENDER
#define INDEX_LIMIT      (0xC000)
#define VERTEX_LIMIT     (0x2000)
#define VERTEX3D_LIMIT   (0x1904)
#define TEXBUFFER_SIZE   (0x100000)
#define TILEUV_SIZE      (0x1000)
#define TEXTURE_LIMIT    (6)
#define TEXTURE_DATASIZE (1024 * 1024 * 2)
#define TEXTURE_SIZE     (1024)

struct DrawVertex {
    short x;
    short y;
    short u;
    short v;

    Colour colour;
};

struct DrawVertex3D {
    float x;
    float y;
    float z;
    short u;
    short v;

    Colour colour;
};

extern DrawVertex gfxPolyList[VERTEX_LIMIT];
extern short gfxPolyListIndex[INDEX_LIMIT];
extern ushort gfxVertexSize;
extern ushort gfxVertexSizeOpaque;
extern ushort gfxIndexSize;
extern ushort gfxIndexSizeOpaque;

extern DrawVertex3D polyList3D[VERTEX3D_LIMIT];

extern ushort vertexSize3D;
extern ushort indexSize3D;
extern float tileUVArray[TILEUV_SIZE];
extern float floor3DXPos;
extern float floor3DYPos;
extern float floor3DZPos;
extern float floor3DAngle;
extern bool render3DEnabled;
extern bool hq3DFloorEnabled;

extern ushort texBuffer[TEXBUFFER_SIZE];
extern byte texBufferMode;

extern int orthWidth;
extern int viewWidth;
extern int viewHeight;
extern float viewAspect;
extern int bufferWidth;
extern int bufferHeight;
extern int virtualX;
extern int virtualY;
extern int virtualWidth;
extern int virtualHeight;

#if RETRO_USING_OPENGL
extern GLuint gfxTextureID[TEXTURE_LIMIT];
extern GLuint framebufferId;
extern GLuint fbTextureId;
#endif

#endif

int InitRenderDevice();
void FlipScreen();
void ReleaseRenderDevice();

void GenerateBlendLookupTable();

inline void ClearGraphicsData()
{
    for (int i = 0; i < SURFACE_MAX; ++i) MEM_ZERO(gfxSurface[i]);
    gfxDataPosition = 0;
}
void ClearScreen(byte index);
void SetScreenDimensions(int width, int height);
void SetScreenSize(int width, int lineSize);

#if RETRO_SOFTWARE_RENDER
void CopyFrameOverlay2x();
#endif

#if RETRO_HARDWARE_RENDER
inline bool CheckSurfaceSize(int size)
{
    for (int cnt = 2; cnt < 2048; cnt <<= 1) {
        if (cnt == size)
            return true;
    }
    return false;
}

void UpdateHardwareTextures();
void SetScreenDimensions(int width, int height, int scale);
void ScaleViewport(int width, int height);
void CalcPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);

void SetupPolygonLists();
void UpdateTextureBufferWithTiles();
void UpdateTextureBufferWithSortedSprites();
void UpdateTextureBufferWithSprites();

#endif
void setupViewport();
void setFullScreen(bool fs);

// Layer Drawing
void DrawObjectList(int layer);
void DrawStageGFX();
#if !RETRO_USE_ORIGINAL_CODE
void DrawDebugOverlays();
#endif

// TileLayer Drawing
void DrawHLineScrollLayer(int layerID);
void DrawVLineScrollLayer(int layerID);
void Draw3DFloorLayer(int layerID);
void Draw3DSkyLayer(int layerID);

// Shape Drawing
void DrawRectangle(int XPos, int YPos, int width, int height, int R, int G, int B, int A);
void SetFadeHQ(int R, int G, int B, int A);
void DrawTintRectangle(int XPos, int YPos, int width, int height);
void DrawScaledTintMask(int direction, int XPos, int YPos, int pivotX, int pivotY, int scaleX, int scaleY, int width, int height, int sprX, int sprY,
                        int sheetID);

// Sprite Drawing
void DrawSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int sheetID);
#if RETRO_REV00
void DrawSpriteClipped(int XPos, int YPos, int width, int height, int sprX, int sprY, int sheetID, int clipY);
#endif
void DrawSpriteFlipped(int XPos, int YPos, int width, int height, int sprX, int sprY, int direction, int sheetID);
void DrawSpriteScaled(int direction, int XPos, int YPos, int pivotX, int pivotY, int scaleX, int scaleY, int width, int height, int sprX, int sprY,
                      int sheetID);
#if RETRO_REV00 || RETRO_REV01
void DrawScaledChar(int direction, int XPos, int YPos, int pivotX, int pivotY, int scaleX, int scaleY, int width, int height, int sprX, int sprY,
                    int sheetID);
#endif
void DrawSpriteRotated(int direction, int XPos, int YPos, int pivotX, int pivotY, int sprX, int sprY, int width, int height, int rotation,
                       int sheetID);
void DrawSpriteRotozoom(int direction, int XPos, int YPos, int pivotX, int pivotY, int sprX, int sprY, int width, int height, int rotation, int scale,
                        int sheetID);

void DrawBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int sheetID);
void DrawAlphaBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int alpha, int sheetID);
void DrawAdditiveBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int alpha, int sheetID);
void DrawSubtractiveBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int alpha, int sheetID);

void DrawObjectAnimation(void *objScr, void *ent, int XPos, int YPos);

void DrawFace(void *v, uint colour);
void DrawFadedFace(void *v, uint colour, uint fogColour, int alpha);
void DrawTexturedFace(void *v, byte sheetID);
void DrawTexturedFaceBlended(void *v, byte sheetID);

#if RETRO_REV00 || RETRO_REV01
void DrawBitmapText(void *menu, int XPos, int YPos, int scale, int spacing, int rowStart, int rowCount);
#endif

void DrawTextMenu(void *menu, int XPos, int YPos);
void DrawTextMenuEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight);
void DrawStageTextEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight);
void DrawBlendedTextMenuEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight);

#endif // !DRAWING_H
