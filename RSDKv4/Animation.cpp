#include "RetroEngine.hpp"

AnimationFile animationFileList[ANIFILE_COUNT];
int animationFileCount = 0;

SpriteFrame scriptFrames[SPRITEFRAME_COUNT];
int scriptFrameCount = 0;

SpriteFrame animFrames[SPRITEFRAME_COUNT];
int animFrameCount = 0;
SpriteAnimation animationList[ANIMATION_COUNT];
int animationCount = 0;
Hitbox hitboxList[HITBOX_COUNT];
int hitboxCount = 0;

void LoadAnimationFile(char *filePath)
{
    FileInfo info;
    if (LoadFile(filePath, &info)) {
        byte fileBuffer = 0;
        char strBuf[0x21];
        byte sheetIDs[0x18];
        sheetIDs[0] = 0;

        byte sheetCount = 0;
        FileRead(&sheetCount, 1);

        for (int s = 0; s < sheetCount; ++s) {
            FileRead(&fileBuffer, 1);
            if (fileBuffer) {
                int i = 0;
                for (; i < fileBuffer; ++i) FileRead(&strBuf[i], 1);
                strBuf[i] = 0;
                GetFileInfo(&info);
                CloseFile();
                sheetIDs[s] = AddGraphicsFile(strBuf);
                SetFileInfo(&info);
            }
        }

        byte animCount = 0;
        FileRead(&animCount, 1);
        AnimationFile *animFile = &animationFileList[animationFileCount];
        animFile->animCount     = animCount;
        animFile->aniListOffset = animationCount;

        for (int a = 0; a < animCount; ++a) {
            SpriteAnimation *anim = &animationList[animationCount++];
            anim->frameListOffset = animFrameCount;
            FileRead(&fileBuffer, 1);
            FileRead(anim->name, fileBuffer);
            anim->name[fileBuffer] = 0;
            FileRead(&anim->frameCount, 1);
            FileRead(&anim->speed, 1);
            FileRead(&anim->loopPoint, 1);
            FileRead(&anim->rotationStyle, 1);

            for (int j = 0; j < anim->frameCount; ++j) {
                SpriteFrame *frame = &animFrames[animFrameCount++];
                FileRead(&frame->sheetID, 1);
                frame->sheetID = sheetIDs[frame->sheetID];
                FileRead(&frame->hitboxID, 1);
                FileRead(&fileBuffer, 1);
                frame->sprX = fileBuffer;
                FileRead(&fileBuffer, 1);
                frame->sprY = fileBuffer;
                FileRead(&fileBuffer, 1);
                frame->width = fileBuffer;
                FileRead(&fileBuffer, 1);
                frame->height = fileBuffer;

                sbyte buffer = 0;
                FileRead(&buffer, 1);
                frame->pivotX = buffer;
                FileRead(&buffer, 1);
                frame->pivotY = buffer;
            }

            // 90 Degree (Extra rotation Frames) rotation
            if (anim->rotationStyle == ROTSTYLE_STATICFRAMES)
                anim->frameCount >>= 1;
        }

        animFile->hitboxListOffset = hitboxCount;
        FileRead(&fileBuffer, 1);
        for (int i = 0; i < fileBuffer; ++i) {
            Hitbox *hitbox = &hitboxList[hitboxCount++];
            for (int d = 0; d < HITBOX_DIR_COUNT; ++d) {
                FileRead(&hitbox->left[d], 1);
                FileRead(&hitbox->top[d], 1);
                FileRead(&hitbox->right[d], 1);
                FileRead(&hitbox->bottom[d], 1);
            }
        }

        CloseFile();
    }
}
void ClearAnimationData()
{
    for (int f = 0; f < SPRITEFRAME_COUNT; ++f) MEM_ZERO(scriptFrames[f]);
    for (int f = 0; f < SPRITEFRAME_COUNT; ++f) MEM_ZERO(animFrames[f]);
    for (int h = 0; h < HITBOX_COUNT; ++h) MEM_ZERO(hitboxList[h]);
    for (int a = 0; a < ANIMATION_COUNT; ++a) MEM_ZERO(animationList[a]);
    for (int a = 0; a < ANIFILE_COUNT; ++a) MEM_ZERO(animationFileList[a]);

    scriptFrameCount   = 0;
    animFrameCount     = 0;
    animationCount     = 0;
    animationFileCount = 0;
    hitboxCount        = 0;
}

AnimationFile *AddAnimationFile(char *filePath)
{
    char path[0x80];
    StrCopy(path, "Data/Animations/");
    StrAdd(path, filePath);

    for (int a = 0; a < ANIFILE_COUNT; ++a) {
        if (StrLength(animationFileList[a].fileName) <= 0) {
            StrCopy(animationFileList[a].fileName, filePath);
            LoadAnimationFile(path);
            ++animationFileCount;
            return &animationFileList[a];
        }
        if (StrComp(animationFileList[a].fileName, filePath))
            return &animationFileList[a];
    }
    return NULL;
}

void ProcessObjectAnimation(void *objScr, void *ent)
{
    ObjectScript *objectScript = (ObjectScript *)objScr;
    Entity *entity             = (Entity *)ent;
    SpriteAnimation *sprAnim   = &animationList[objectScript->animFile->aniListOffset + entity->animation];

    if (entity->animationSpeed <= 0) {
        entity->animationTimer += sprAnim->speed;
    }
    else {
        if (entity->animationSpeed > 0xF0)
            entity->animationSpeed = 0xF0;
        entity->animationTimer += entity->animationSpeed;
    }

    if (entity->animation != entity->prevAnimation) {
        entity->prevAnimation  = entity->animation;
        entity->frame          = 0;
        entity->animationTimer = 0;
        entity->animationSpeed = 0;
    }

    if (entity->animationTimer >= 0xF0) {
        entity->animationTimer -= 0xF0;
        ++entity->frame;
    }

    if (entity->frame >= sprAnim->frameCount)
        entity->frame = sprAnim->loopPoint;
}