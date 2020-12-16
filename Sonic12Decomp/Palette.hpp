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
    if (newActivePal < PALETTE_COUNT) {
        byte *LineBuffer = &gfxLineBuffer[startLine];
        while (++startLine < endLine) *LineBuffer++ = newActivePal;
    }
    activePalette   = fullPalette[gfxLineBuffer[0]];
    activePalette32 = fullPalette32[gfxLineBuffer[0]];
}

inline void SetPaletteEntry(byte paletteIndex, byte r, byte g, byte b)
{
    activePalette[paletteIndex]     = ((int)b >> 3) | 32 * ((int)g >> 2) | ((ushort)((int)r >> 3) << 11);
    activePalette32[paletteIndex].r = r;
    activePalette32[paletteIndex].g = g;
    activePalette32[paletteIndex].b = b;
}

inline void SetPaletteEntryPacked(byte paletteIndex, byte index, uint colour)
{
    fullPalette[paletteIndex][index] = ((byte)(colour >> 0) >> 3) | 32 * ((byte)(colour >> 8) >> 2) | ((ushort)((byte)(colour >> 16) >> 3) << 11);
    //activePalette32[paletteIndex].r = r;
    //activePalette32[paletteIndex].g = g;
    //activePalette32[paletteIndex].b = b;
}

inline uint GetPaletteEntryPacked(byte paletteIndex, byte index) { return fullPalette[paletteIndex][index]; }

inline void CopyPalette(byte sourcePalette, byte srcPaletteStart, byte destinationPalette, byte destPaletteStart, byte count)
{
    if (sourcePalette < PALETTE_COUNT && destinationPalette < PALETTE_COUNT)
        for (int i = 0; i < count; ++i) fullPalette[destinationPalette][srcPaletteStart + i] = fullPalette[sourcePalette][destPaletteStart + i];
}

inline void RotatePalette(int palID, byte startIndex, byte endIndex, bool right)
{
    if (right) {
        ushort startClr = fullPalette[palID][endIndex];
        for (int i = endIndex; i > startIndex; --i) activePalette[i] = activePalette[i - 1];
        fullPalette[palID][startIndex] = startClr;
    }
    else {
        ushort startClr = fullPalette[palID][startIndex];
        for (int i = startIndex; i < endIndex; ++i) activePalette[i] = activePalette[i + 1];
        fullPalette[palID][endIndex] = startClr;
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
void SetLimitedFade(uint paletteIndex, byte srcPaletteA, int srcPaletteB, ushort blendAmount, int startIndex, int endIndex);

#endif // !PALETTE_H
