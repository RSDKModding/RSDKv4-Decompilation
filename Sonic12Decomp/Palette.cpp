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
            for (int i = startIndex; i < endIndex; i++) {
                FileRead(&colour, 3);
                fullPalette[paletteID][startPaletteIndex]     = (ushort)((colour[2] >> 3) | 32 * (colour[1] >> 2) | ((colour[0] >> 3) << 11));
                fullPalette32[paletteID][startPaletteIndex].r   = colour[0];
                fullPalette32[paletteID][startPaletteIndex].g   = colour[1];
                fullPalette32[paletteID][startPaletteIndex++].b = colour[2];
            }
        }
        else {
            for (int i = startIndex; i < endIndex; i++) {
                FileRead(&colour, 3);
                activePalette[startPaletteIndex]       = (ushort)((colour[2] >> 3) | 32 * (colour[1] >> 2) | ((colour[0] >> 3) << 11));
                activePalette32[startPaletteIndex].r   = colour[0];
                activePalette32[startPaletteIndex].g   = colour[1];
                activePalette32[startPaletteIndex++].b = colour[2];
            }
        }
        CloseFile();
    }
}

void SetLimitedFade(uint paletteIndex, byte srcPaletteA, int srcPaletteB, ushort blendAmount, int startIndex,
                    int endIndex)
{
    if (paletteIndex >= PALETTE_COUNT)
        return;

}