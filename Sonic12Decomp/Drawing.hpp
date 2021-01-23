#ifndef DRAWING_H
#define DRAWING_H

#define SPRITESHEETS_MAX (16)
#define SURFACE_MAX      (24)
#define GFXDATA_MAX      (0x200000)

#define BLENDTABLE_YSIZE (0x100)
#define BLENDTABLE_XSIZE (0x20)
#define BLENDTABLE_SIZE  (BLENDTABLE_XSIZE * BLENDTABLE_YSIZE)
#define TINTTABLE_SIZE (0x1000)

#define DRAWLAYER_COUNT (0x7)

enum FlipFlags { FLIP_NONE, FLIP_X, FLIP_Y, FLIP_XY };
enum InkFlags { INK_NONE, INK_BLEND, INK_ALPHA, INK_ADD, INK_SUB };
enum DrawFXFlags { FX_SCALE, FX_ROTATE, FX_ROTOZOOM, FX_INK, FX_TINT, FX_FLIP };

struct DrawListEntry
{
    int entityRefs[ENTITY_COUNT];
    int listSize;
};

struct GFXSurface
{
    char fileName[0x40];
    int height;
    int width;
    int widthShift;
    int depth;
    int dataPosition;
};

extern short blendLookupTable[BLENDTABLE_SIZE];
extern short subtractLookupTable[BLENDTABLE_SIZE];
extern short tintLookupTable[TINTTABLE_SIZE];

extern int SCREEN_XSIZE;
extern int SCREEN_CENTERX;

extern DrawListEntry drawListEntries[DRAWLAYER_COUNT];

extern int gfxDataPosition;
extern GFXSurface gfxSurface[SURFACE_MAX];
extern byte graphicData[GFXDATA_MAX];

int InitRenderDevice();
void RenderRenderDevice();
void ReleaseRenderDevice();

void GenerateBlendLookupTable();

inline void ClearGraphicsData()
{
    for (int i = 0; i < SURFACE_MAX; ++i) MEM_ZERO(gfxSurface[i]);
    gfxDataPosition = 0;
}
void ClearScreen(byte index);

void SetScreenSize(int width, int height);

// Layer Drawing
void DrawObjectList(int layer);
void DrawStageGFX();

// TileLayer Drawing
void DrawHLineScrollLayer(int layerID);
void DrawVLineScrollLayer(int layerID);
void Draw3DFloorLayer(int layerID);
void Draw3DSkyLayer(int layerID);

// Shape Drawing
void DrawRectangle(int XPos, int YPos, int width, int height, int R, int G, int B, int A);
void SetFadeHQ(int R, int G, int B, int A);
void DrawTintRectangle(int XPos, int YPos, int width, int height);
void DrawScaledTintMask(int direction, int XPos, int YPos, int pivotX, int pivotY, int scaleX,
                        int scaleY, int width, int height, int sprX, int sprY, int sheetID);

// Sprite Drawing
void DrawSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int sheetID);
void DrawSpriteFlipped(int XPos, int YPos, int width, int height, int sprX, int sprY, int direction,
                       int sheetID);
void DrawSpriteScaled(int direction, int XPos, int YPos, int pivotX, int pivotY, int scaleX, int scaleY,
                      int width, int height, int sprX, int sprY, int sheetID);
void DrawSpriteRotated(int direction, int XPos, int YPos, int pivotX, int pivotY, int sprX, int sprY,
                       int width, int height, int rotation, int sheetID);
void DrawSpriteRotozoom(int direction, int XPos, int YPos, int pivotX, int pivotY, int sprX, int sprY,
                        int width, int height, int rotation, int scale, int sheetID);

void DrawBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int sheetID);
void DrawAlphaBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int alpha,
                            int sheetID);
void DrawAdditiveBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int alpha,
                               int sheetID);
void DrawSubtractiveBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY,
                                  int alpha, int sheetID);

void DrawObjectAnimation(void *objScr, void *ent, int XPos, int YPos);

void DrawFace(void *v, uint colour);
void DrawFadedFace(void *v, uint colour, uint fogColour, int alpha);
void DrawTexturedFace(void *v, byte sheetID);
void DrawTexturedFace2(void *v, byte sheetID);

void DrawBitmapText(void *menu, int XPos, int YPos, int scale, int spacing, int rowStart, int rowCount);

void DrawTextMenu(void *menu, int XPos, int YPos);
void DrawTextMenuEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight);
void DrawStageTextEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight);
void DrawBlendedTextMenuEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight);

#endif // !DRAWING_H
