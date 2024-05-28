#ifndef DRAWING_H
#define DRAWING_H

#define SURFACE_COUNT (24)
#define GFXDATA_SIZE  (0x800 * 0x800)

#if RETRO_REV03
#define DRAWLAYER_COUNT (8)
#else
#define DRAWLAYER_COUNT (7)
#endif

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
    int depth;
    int dataPosition;
};

struct DisplaySettings {
    int offsetX;
    int width;
    int height;
    int unknown1;
    int maxWidth;
    byte unknown2;
};

extern ushort blendLookupTable[0x20 * 0x100];
extern ushort subtractLookupTable[0x20 * 0x100];
extern ushort tintLookupTable[0x10000];

extern int SCREEN_XSIZE_CONFIG;
extern int SCREEN_XSIZE;
extern int SCREEN_CENTERX;

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
extern GFXSurface gfxSurface[SURFACE_COUNT];
extern byte graphicData[GFXDATA_SIZE];

extern DisplaySettings displaySettings;
extern bool convertTo32Bit;
extern bool mixFiltersOnJekyll;

#if RETRO_USING_OPENGL
extern GLint defaultFramebuffer;
extern GLuint framebufferHiRes;
extern GLuint renderbufferHiRes;
#endif

int InitRenderDevice();
void FlipScreen();
void ReleaseRenderDevice(bool refresh = false);

void GenerateBlendLookupTable();

inline void ClearGraphicsData()
{
    for (int i = 0; i < SURFACE_COUNT; ++i) MEM_ZERO(gfxSurface[i]);
    gfxDataPosition = 0;
}
void ClearScreen(byte index);
void SetScreenDimensions(int width, int height);
void SetScreenSize(int width, int lineSize);

#if RETRO_SOFTWARE_RENDER
void CopyFrameOverlay2x();
#endif

void SetupViewport();
void SetFullScreen(bool fs);

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
#if !RETRO_REV02
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

void DrawFace(void *v, uint color);
void DrawFadedFace(void *v, uint color, uint fogColor, int alpha);
void DrawTexturedFace(void *v, byte sheetID);
void DrawTexturedFaceBlended(void *v, byte sheetID);

#if !RETRO_REV02
void DrawBitmapText(void *menu, int XPos, int YPos, int scale, int spacing, int rowStart, int rowCount);
#endif

void DrawTextMenu(void *menu, int XPos, int YPos);
void DrawTextMenuEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight);
void DrawStageTextEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight);
void DrawBlendedTextMenuEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight);

#endif // !DRAWING_H
