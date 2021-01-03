#include "RetroEngine.hpp"

// Palettes (as RGB888 Colours)
PaletteEntry fullPalette32[PALETTE_COUNT][PALETTE_SIZE];
PaletteEntry *activePalette32 = fullPalette32[0];

// Palettes (as RGB565 Colours)
ushort fullPalette[PALETTE_COUNT][PALETTE_SIZE];
ushort *activePalette = fullPalette[0]; // Ptr to the 256 colour set thats active

PaletteEntry colourIndexes[PALETTE_SIZE];

byte gfxLineBuffer[SCREEN_YSIZE]; // Pointers to active palette

int fadeMode = 0;
byte fadeA   = 0;
byte fadeR   = 0;
byte fadeG   = 0;
byte fadeB   = 0;

void LoadPalette(const char *filePath, int paletteID, int startPaletteIndex, int startIndex, int endIndex)
{
    FileInfo info;
    char fullPath[0x80];

    StrCopy(fullPath, "Data/Palettes/");
    StrAdd(fullPath, filePath);

    if (LoadFile(fullPath, &info)) {
        SetFilePosition(3 * startIndex);
        if (paletteID >= PALETTE_COUNT || paletteID < 0)
            paletteID = 0;

        byte colour[3];
        if (paletteID) {
            for (int i = startIndex; i < endIndex; ++i) {
                FileRead(&colour, 3);
                SetPaletteEntry(paletteID, startPaletteIndex++, colour[0], colour[1], colour[2]);
            }
        }
        else {
            for (int i = startIndex; i < endIndex; ++i) {
                FileRead(&colour, 3);
                SetPaletteEntry(-1, startPaletteIndex++, colour[0], colour[1], colour[2]);
            }
        }
        CloseFile();
    }
}

void SetLimitedFade(byte destPaletteID, byte srcPaletteA, byte srcPaletteB, ushort blendAmount, int startIndex,
                    int endIndex)
{
    if (destPaletteID >= PALETTE_COUNT || srcPaletteA >= PALETTE_COUNT || srcPaletteB >= PALETTE_COUNT)
        return;

    if (blendAmount >= 0) {
        if (blendAmount > 0xFF)
            blendAmount = 0xFF;
    }
    else {
        blendAmount = 0;
    }
    if (++endIndex + 1 > 0x100)
        endIndex = 0x100;

    if (startIndex < endIndex) {
        int trueAlpha = 0xFF - blendAmount;
        ushort *dst   = &fullPalette[destPaletteID][startIndex];
        ushort *srcA  = &fullPalette[srcPaletteA][startIndex];
        ushort *srcB  = &fullPalette[srcPaletteB][startIndex];
        int length    = endIndex - startIndex;
        do {
            uint v11  = *srcA;
            byte v12  = v11 & 0xFF;
            uint v13  = (v11 >> 3) & -4;
            ushort sB = *srcB;
            v11       = sB & 0xFF;
            uint v15  = (sB >> 3) & -4;
            ++srcB;
            uint v18 = blendAmount * (v12 & 0xF8) + trueAlpha * (v12 & 0xF8);
            *dst     = colourIndexes[v18 & 0xFF].r | colourIndexes[(blendAmount * v15 + trueAlpha * v13) >> 8].g
                   | colourIndexes[(blendAmount * (*srcB << 3) + trueAlpha * (*srcA << 3)) >> 8].b;
            ++srcA;
            ++dst;
        } while (length--);
    }
}