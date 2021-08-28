#ifndef PALETTE_H
#define PALETTE_H

#define PALETTE_COUNT (0x8)
#define PALETTE_SIZE  (0x100)

struct Colour {
    byte r;
    byte g;
    byte b;
    byte a;
};

struct PaletteEntry {
    byte r;
    byte g;
    byte b;
};

// Palettes (as RGB565 Colours)
extern PaletteEntry fullPalette32[PALETTE_COUNT][PALETTE_SIZE];
extern ushort fullPalette[PALETTE_COUNT][PALETTE_SIZE];
extern ushort *activePalette; // Pointers to the 256 colour set thats active
extern PaletteEntry *activePalette32;

extern byte gfxLineBuffer[SCREEN_YSIZE]; // Pointers to active palette
extern int GFX_LINESIZE;
extern int GFX_LINESIZE_MINUSONE;
extern int GFX_LINESIZE_DOUBLE;
extern int GFX_FRAMEBUFFERSIZE;
extern int GFX_FBUFFERMINUSONE;

extern uint gfxPalette16to32[0x10000];

extern int fadeMode;
extern byte fadeA;
extern byte fadeR;
extern byte fadeG;
extern byte fadeB;

extern int paletteMode;

#if RETRO_HARDWARE_RENDER
extern int texPaletteNum;
#endif

#define RGB888_TO_RGB5551(r, g, b) (2 * ((b) >> 3) | ((g) >> 3 << 6) | ((r) >> 3 << 11) | 0) // used in mobile vers
#define RGB888_TO_RGB565(r, g, b)  ((b) >> 3) | (((g) >> 2) << 5) | (((r) >> 3) << 11)       // used in pc vers

#if RETRO_SOFTWARE_RENDER
#define PACK_RGB888(r, g, b) RGB888_TO_RGB565(r, g, b)
#elif RETRO_HARDWARE_RENDER || RETRO_USING_OPENGL
#define PACK_RGB888(r, g, b) RGB888_TO_RGB5551(r, g, b)
#endif

void LoadPalette(const char *filePath, int paletteID, int startPaletteIndex, int startIndex, int endIndex);

inline void SetActivePalette(byte newActivePal, int startLine, int endLine)
{
#if RETRO_SOFTWARE_RENDER
    if (newActivePal < PALETTE_COUNT)
        for (int l = startLine; l < endLine && l < SCREEN_YSIZE; l++) gfxLineBuffer[l] = newActivePal;

    activePalette   = fullPalette[gfxLineBuffer[0]];
    activePalette32 = fullPalette32[gfxLineBuffer[0]];
#endif

#if RETRO_HARDWARE_RENDER
    if (newActivePal < PALETTE_COUNT)
        texPaletteNum = newActivePal;
#endif
}

inline void SetPaletteEntry(byte paletteIndex, byte index, byte r, byte g, byte b)
{
    if (paletteIndex != 0xFF) {
        fullPalette[paletteIndex][index]     = PACK_RGB888(r, g, b);
        fullPalette32[paletteIndex][index].r = r;
        fullPalette32[paletteIndex][index].g = g;
        fullPalette32[paletteIndex][index].b = b;
#if RETRO_HARDWARE_RENDER
        if (index)
            fullPalette[paletteIndex][index] |= 1;
#endif
    }
    else {
        activePalette[index]     = PACK_RGB888(r, g, b);
        activePalette32[index].r = r;
        activePalette32[index].g = g;
        activePalette32[index].b = b;
#if RETRO_HARDWARE_RENDER
        if (index)
            activePalette[index] |= 1;
#endif
    }
}

inline void SetPaletteEntryPacked(byte paletteIndex, byte index, uint colour)
{
    fullPalette[paletteIndex][index] = PACK_RGB888((byte)(colour >> 16), (byte)(colour >> 8), (byte)(colour >> 0));
#if RETRO_HARDWARE_RENDER
    if (index)
        fullPalette[paletteIndex][index] |= 1;
#endif
    fullPalette32[paletteIndex][index].r = (byte)(colour >> 16);
    fullPalette32[paletteIndex][index].g = (byte)(colour >> 8);
    fullPalette32[paletteIndex][index].b = (byte)(colour >> 0);
}

inline uint GetPaletteEntryPacked(byte paletteIndex, byte index)
{
    PaletteEntry clr = fullPalette32[paletteIndex][index];
    return (clr.r << 16) | (clr.g << 8) | (clr.b);
}

inline void CopyPalette(byte sourcePalette, byte srcPaletteStart, byte destinationPalette, byte destPaletteStart, ushort count)
{
    if (sourcePalette < PALETTE_COUNT && destinationPalette < PALETTE_COUNT) {
        for (int i = 0; i < count; ++i) {
            fullPalette[destinationPalette][destPaletteStart + i]   = fullPalette[sourcePalette][srcPaletteStart + i];
            fullPalette32[destinationPalette][destPaletteStart + i] = fullPalette32[sourcePalette][srcPaletteStart + i];
        }
    }
}

inline void RotatePalette(int palID, byte startIndex, byte endIndex, bool right)
{
    if (right) {
        ushort startClr         = fullPalette[palID][endIndex];
        PaletteEntry startClr32 = fullPalette32[palID][endIndex];
        for (int i = endIndex; i > startIndex; --i) {
            fullPalette[palID][i]   = fullPalette[palID][i - 1];
            fullPalette32[palID][i] = fullPalette32[palID][i - 1];
        }
        fullPalette[palID][startIndex]   = startClr;
        fullPalette32[palID][startIndex] = startClr32;
    }
    else {
        ushort startClr         = fullPalette[palID][startIndex];
        PaletteEntry startClr32 = fullPalette32[palID][startIndex];
        for (int i = startIndex; i < endIndex; ++i) {
            fullPalette[palID][i]   = fullPalette[palID][i + 1];
            fullPalette32[palID][i] = fullPalette32[palID][i + 1];
        }
        fullPalette[palID][endIndex]   = startClr;
        fullPalette32[palID][endIndex] = startClr32;
    }
}

inline void SetFade(byte R, byte G, byte B, ushort A)
{
    fadeMode = 1;
    fadeR    = R;
    fadeG    = G;
    fadeB    = B;
    fadeA    = A > 0xFF ? 0xFF : A;
}
void SetPaletteFade(byte destPaletteID, byte srcPaletteA, byte srcPaletteB, ushort blendAmount, int startIndex, int endIndex);

#endif // !PALETTE_H