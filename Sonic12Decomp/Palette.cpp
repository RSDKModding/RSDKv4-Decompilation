#include "RetroEngine.hpp"

// Palettes (as RGB888 Colours)
PaletteEntry fullPalette32[PALETTE_COUNT][PALETTE_SIZE];
PaletteEntry *activePalette32 = fullPalette32[0];

// Palettes (as RGB565 Colours)
ushort fullPalette[PALETTE_COUNT][PALETTE_SIZE];
ushort *activePalette = fullPalette[0]; // Ptr to the 256 colour set thats active

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

void SetPaletteFade(byte destPaletteID, byte srcPaletteA, byte srcPaletteB, ushort blendAmount, int startIndex,
                    int endIndex)
{
    if (destPaletteID >= PALETTE_COUNT || srcPaletteA >= PALETTE_COUNT || srcPaletteB >= PALETTE_COUNT)
        return;

    if (blendAmount > 0xFF) {
        blendAmount = 0xFF;
    }
    else {
        blendAmount = 0;
    }
    if (endIndex > 0x100)
        endIndex = 0x100;

    if (startIndex < endIndex) {
        uint blendA = 0xFF - blendAmount;
        ushort *dst   = &fullPalette[destPaletteID][startIndex];
        ushort *srcA  = &fullPalette[srcPaletteA][startIndex];
        ushort *srcB  = &fullPalette[srcPaletteB][startIndex];
        for (int l = startIndex; l < endIndex; ++l) {
            ushort result = (((blendAmount * (4 * (byte)*srcB & 0xF8) + blendA * (4 * (byte)*srcA & 0xF8)) >> 10) & 0x3E
                                     | (blendAmount * (ushort)((*srcB & 0xF800) >> 8) + blendA * (ushort)((*srcA & 0xF800) >> 8)) & 0xF800
                                     | ((blendA * ((*srcA & 0x7C0) >> 3) + blendAmount * ((*srcB & 0x7C0) >> 3)) >> 5) & 0x7C0 | 1);
            *dst           = result;

            /*byte a1   = *srcA;
            ushort a2 = (*srcA >> 3) & 0xFFFC;
            byte b1   = *srcB;
            ushort b2 = (*srcB >> 3) & 0xFFFC;
            ushort r  = (blendAmount * (b1 & 0xFFF8) + blendA * (a1 & 0xFFF8)) >> 8;
            ushort g  = (blendAmount * b2 + blendA * a2) >> 8;
            ushort b  = (blendAmount * (*srcB << 3) + blendA * (*srcA << 3)) >> 8;
            ushort val = r | g | b;
            //*dst      = RGB888_TO_RGB565((blendAmount * (b1 & 0xFFF8) + blendA * (a1 & 0xFFF8)) >> 8, (blendAmount * b2 + blendA * a2) >> 8,
            //                        (blendAmount * (*srcB << 3) + blendA * (*srcA << 3)) >> 8);*/
            ++srcA;
            ++srcB;
            ++dst;
        }
    }
}