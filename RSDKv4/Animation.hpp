#ifndef ANIMATION_H
#define ANIMATION_H

#define ANIFILE_COUNT     (0x100)
#define ANIMATION_COUNT   (0x400)
#define SPRITEFRAME_COUNT (0x1000)

#define HITBOX_COUNT     (0x20)
#define HITBOX_DIR_COUNT (0x8)

enum AnimRotationFlags { ROTSTYLE_NONE, ROTSTYLE_FULL, ROTSTYLE_45DEG, ROTSTYLE_STATICFRAMES };

struct AnimationFile {
    char fileName[0x20];
    int animCount;
    int aniListOffset;
    int hitboxListOffset;
};

struct SpriteAnimation {
    char name[16];
    byte frameCount;
    byte speed;
    byte loopPoint;
    byte rotationStyle;
    int frameListOffset;
};

struct SpriteFrame {
    int sprX;
    int sprY;
    int width;
    int height;
    int pivotX;
    int pivotY;
    byte sheetID;
    byte hitboxID;
};

struct Hitbox {
    sbyte left[HITBOX_DIR_COUNT];
    sbyte top[HITBOX_DIR_COUNT];
    sbyte right[HITBOX_DIR_COUNT];
    sbyte bottom[HITBOX_DIR_COUNT];
};

extern AnimationFile animationFileList[ANIFILE_COUNT];
extern int animationFileCount;

extern SpriteFrame scriptFrames[SPRITEFRAME_COUNT];
extern int scriptFrameCount;

extern SpriteFrame animFrames[SPRITEFRAME_COUNT];
extern int animFrameCount;
extern SpriteAnimation animationList[ANIMATION_COUNT];
extern int animationCount;
extern Hitbox hitboxList[HITBOX_COUNT];
extern int hitboxCount;

void LoadAnimationFile(char *filePath);
void ClearAnimationData();

AnimationFile *AddAnimationFile(char *filePath);

inline AnimationFile *GetDefaultAnimationRef() { return &animationFileList[0]; }

void ProcessObjectAnimation(void *objScr, void *ent);

#endif // !ANIMATION_H
