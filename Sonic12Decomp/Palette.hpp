#ifndef PALETTE_H
#define PALETTE_H

#define PALETTE_COUNT (0x8)
#define PALETTE_SIZE  (0x100)

struct PaletteEntry {
    byte r;
    byte g;
    byte b;
};

// Palettes (as RGB565 Colours)
extern PaletteEntry fullPalette32[PALETTE_COUNT][PALETTE_SIZE];
extern ushort fullPalette[PALETTE_COUNT][PALETTE_SIZE];
extern ushort *activePalette; // Ptr to the 256 colour set thats active
extern PaletteEntry *activePalette32;

extern PaletteEntry colourIndexes[PALETTE_SIZE];

extern byte gfxLineBuffer[SCREEN_YSIZE]; // Pointers to active palette

extern int fadeMode;
extern byte fadeA;
extern byte fadeR;
extern byte fadeG;
extern byte fadeB;

extern int paletteMode;

void LoadPalette(const char *filePath, int paletteID, int startPaletteIndex, int startIndex,
                 int endIndex);

inline void SetActivePalette(byte newActivePal, int startLine, int endLine)
{
    if (newActivePal < PALETTE_COUNT)
        while (startLine++ < endLine) gfxLineBuffer[startLine % SCREEN_YSIZE] = newActivePal;

    activePalette   = fullPalette[gfxLineBuffer[0]];
    activePalette32 = fullPalette32[gfxLineBuffer[0]];
}

inline void SetPaletteEntry(byte paletteIndex, byte index, byte r, byte g, byte b)
{
    if (paletteIndex != 0xFF) {
        fullPalette[paletteIndex][index]     = ((int)b >> 3) | 32 * ((int)g >> 2) | ((ushort)((int)r >> 3) << 11);
        fullPalette32[paletteIndex][index].r = r;
        fullPalette32[paletteIndex][index].g = g;
        fullPalette32[paletteIndex][index].b = b;
    }
    else {
        activePalette[index]     = ((int)b >> 3) | 32 * ((int)g >> 2) | ((ushort)((int)r >> 3) << 11);
        activePalette32[index].r = r;
        activePalette32[index].g = g;
        activePalette32[index].b = b;
    }
}

inline void SetPaletteEntryPacked(byte paletteIndex, byte index, uint colour)
{
    fullPalette[paletteIndex][index]     = ((byte)(colour >> 0) >> 3) | 32 * ((byte)(colour >> 8) >> 2) | ((ushort)((byte)(colour >> 16) >> 3) << 11);
    fullPalette32[paletteIndex][index].r = (byte)(colour >> 16);
    fullPalette32[paletteIndex][index].g = (byte)(colour >> 8);
    fullPalette32[paletteIndex][index].b = (byte)(colour >> 0);
}

inline uint GetPaletteEntryPacked(byte paletteIndex, byte index)
{
    PaletteEntry clr = fullPalette32[paletteIndex][index];
    return (clr.r << 16) | (clr.g << 8) | (clr.b);
}

inline void CopyPalette(byte sourcePalette, byte srcPaletteStart, byte destinationPalette, byte destPaletteStart, byte count)
{
    if (sourcePalette < PALETTE_COUNT && destinationPalette < PALETTE_COUNT) {
        for (int i = 0; i < count; ++i) {
            fullPalette[destinationPalette][srcPaletteStart + i]   = fullPalette[sourcePalette][destPaletteStart + i];
            fullPalette32[destinationPalette][srcPaletteStart + i] = fullPalette32[sourcePalette][destPaletteStart + i];
        }
    }
}

inline void RotatePalette(int palID, byte startIndex, byte endIndex, bool right)
{
    if (right) {
        ushort startClr         = fullPalette[palID][endIndex];
        PaletteEntry startClr32 = fullPalette32[palID][startIndex];
        for (int i = endIndex; i > startIndex; --i) {
            fullPalette[palID][i] = fullPalette[palID][i - 1];
            fullPalette32[palID][i] = fullPalette32[palID][i - 1];
        }
        fullPalette[palID][startIndex] = startClr;
        fullPalette32[palID][endIndex] = startClr32;
    }
    else {
        ushort startClr = fullPalette[palID][startIndex];
        PaletteEntry startClr32 = fullPalette32[palID][startIndex];
        for (int i = startIndex; i < endIndex; ++i) {
            fullPalette[palID][i] = fullPalette[palID][i + 1];
            fullPalette32[palID][i] = fullPalette32[palID][i + 1];
        }
        fullPalette[palID][endIndex] = startClr;
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
void SetLimitedFade(byte destPaletteID, byte srcPaletteA, byte srcPaletteB, ushort blendAmount, int startIndex, int endIndex);

#endif // !PALETTE_H
