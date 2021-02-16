#ifndef SPRITE_H
#define SPRITE_H

#if !RETRO_USE_ORIGINAL_CODE
#define TEXTURE_MAX (0x20)

struct texInfo {
    char fileName[0x40];
    int width;
    int height;
    void *tex;
};

extern int textureCount;
extern texInfo textureList[TEXTURE_MAX];
#endif

int AddGraphicsFile(const char *filePath);
void RemoveGraphicsFile(const char *filePath, int sheetID);

int LoadBMPFile(const char *filePath, byte sheetID);
int LoadGIFFile(const char *filePath, byte sheetID);
int LoadPVRFile(const char *filePath, byte sheetID);

void ReadGifPictureData(int width, int height, bool interlaced, byte *gfxData, int offset);

#if !RETRO_USE_ORIGINAL_CODE
int LoadTexture(const char *filePath, byte dMode);
void ClearTextures();
#endif

#endif // !SPRITE_H
