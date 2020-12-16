#include "RetroEngine.hpp"

int stageListCount[STAGELIST_MAX];
char stageListNames[STAGELIST_MAX][0x20] = {
    "Presentation Stages",
    "Regular Stages",
    "Bonus Stages",
    "Special Stages",
};
SceneInfo stageList[STAGELIST_MAX][0x100];

int stageMode = STAGEMODE_LOAD;

int cameraTarget   = -1;
int cameraStyle    = 0;
int cameraEnabled  = 0;
int cameraAdjustY  = 0;
int xScrollOffset  = 0;
int yScrollOffset  = 0;
int cameraXPos     = 0;
int cameraYPos     = 0;
int cameraShift    = 0;
int cameraLockedY  = 0;
int cameraShakeX   = 0;
int cameraShakeY   = 0;
int cameraLag      = 0;
int cameraLagStyle = 0;

int xBoundary1    = 0;
int newXBoundary1 = 0;
int yBoundary1    = 0;
int newYBoundary1 = 0;
int xBoundary2    = 0;
int yBoundary2    = 0;
int waterLevel    = 0;
int waterDrawPos  = 0;
int newXBoundary2 = 0;
int newYBoundary2 = 0;

int SCREEN_SCROLL_LEFT  = SCREEN_CENTERX - 8;
int SCREEN_SCROLL_RIGHT = SCREEN_CENTERX + 8;

int lastYSize = -1;
int lastXSize = -1;

bool pauseEnabled     = true;
bool timeEnabled      = true;
bool debugMode        = false;
int frameCounter        = 0;
int stageMilliseconds = 0;
int stageSeconds      = 0;
int stageMinutes      = 0;

// Category and Scene IDs
int activeStageList   = 0;
int stageListPosition = 0;
char currentStageFolder[0x100];
int actID = 0;

char titleCardText[0x100];
byte titleCardWord2 = 0;

byte activeTileLayers[4];
byte tLayerMidPoint;
TileLayer stageLayouts[LAYER_COUNT];

int bgDeformationData0[DEFORM_COUNT];
int bgDeformationData1[DEFORM_COUNT];
int bgDeformationData2[DEFORM_COUNT];
int bgDeformationData3[DEFORM_COUNT];

LineScroll hParallax;
LineScroll vParallax;

Tiles128x128 tiles128x128;
CollisionMasks collisionMasks[2];

byte tilesetGFXData[TILESET_SIZE];

ushort tile3DFloorBuffer[0x13334];

void InitFirstStage(void)
{
    xScrollOffset = 0;
    yScrollOffset = 0;
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    fadeMode     = 0;
    ClearGraphicsData();
    ClearAnimationData();
    activePalette = fullPalette[0];
    stageMode         = STAGEMODE_LOAD;
    Engine.gameMode   = ENGINE_MAINGAME;
    //activeStageList   = 0;
    //stageListPosition = 0;
    activeStageList   = Engine.startList;
    stageListPosition = Engine.startStage;
}

void ProcessStage(void)
{
    switch (stageMode) {
        case STAGEMODE_LOAD: // Startup
            fadeMode = 0;
            SetActivePalette(0, 0, 256);

            cameraEnabled = 1;
            cameraTarget  = -1;
            cameraShift   = 0;
            cameraStyle   = 0;
            cameraXPos    = 0;
            cameraYPos    = 0;
            cameraLockedY = 0;
            cameraAdjustY = 0;
            xScrollOffset = 0;
            yScrollOffset = 0;
            cameraShakeX  = 0;
            cameraShakeY  = 0;

            vertexCount       = 0;
            faceCount         = 0;
            pauseEnabled      = false;
            timeEnabled       = false;
            frameCounter      = 0;
            stageMilliseconds = 0;
            stageSeconds      = 0;
            stageMinutes      = 0;
            ResetBackgroundSettings();
            LoadStageFiles();
            stageMode = STAGEMODE_NORMAL;

            break;
        case STAGEMODE_NORMAL:
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown, 0xFF);
            CheckKeyPress(&keyPress, 0xFF);
            if (pauseEnabled && keyPress.start) {
                stageMode = STAGEMODE_PAUSED;
                PauseSound();
            }

            if (timeEnabled) {
                if (++frameCounter == 60) {
                    frameCounter = 0;
                    if (++stageSeconds > 59) {
                        stageSeconds = 0;
                        if (++stageMinutes > 59)
                            stageMinutes = 0;
                    }
                }
                stageMilliseconds = 100 * frameCounter / 60;
            }

            ProcessObjects();
            if (cameraTarget > -1) {
                if (cameraEnabled == 1) {
                    switch (cameraStyle) {
                        case 0: SetPlayerScreenPosition(&objectEntityList[cameraTarget]); break;
                        case 1:
                        case 2:
                        case 3: SetPlayerScreenPositionCDStyle(&objectEntityList[cameraTarget]); break;
                        case 4: SetPlayerHLockedScreenPosition(&objectEntityList[cameraTarget]); break;
                        default: break;
                    }
                }
                else {
                    SetPlayerLockedScreenPosition(&objectEntityList[cameraTarget]);
                }
            }

            ProcessParallaxAutoScroll();
            DrawStageGFX();
            break;
        case STAGEMODE_UNKNOWN:
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown, 0xFF);
            CheckKeyPress(&keyPress, 0xFF);

            if (pauseEnabled && keyPress.start) {
                stageMode = STAGEMODE_STEPOVER;
                ResumeSound();
            }

            ProcessPausedObjects();
            DrawObjectList(0);
            DrawObjectList(1);
            DrawObjectList(2);
            DrawObjectList(3);
            DrawObjectList(4);
            DrawObjectList(5);
            DrawObjectList(6);
            break;
        case STAGEMODE_FROZEN:
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown, 0xFF);
            CheckKeyPress(&keyPress, 0xFF);
            if (pauseEnabled && keyPress.start) {
                stageMode = STAGEMODE_PAUSED;
                PauseSound();
            }

            ProcessFrozenObjects();
            if (cameraTarget > -1) {
                if (cameraEnabled == 1) {
                    switch (cameraStyle) {
                        case 0: SetPlayerScreenPosition(&objectEntityList[cameraTarget]); break;
                        case 1:
                        case 2:
                        case 3: SetPlayerScreenPositionCDStyle(&objectEntityList[cameraTarget]); break;
                        case 4: SetPlayerHLockedScreenPosition(&objectEntityList[cameraTarget]); break;
                        default: break;
                    }
                }
                else {
                    SetPlayerLockedScreenPosition(&objectEntityList[cameraTarget]);
                }
            }

            DrawStageGFX();
            break;
        case STAGEMODE_2P:
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown, 0xFF);
            CheckKeyPress(&keyPress, 0xFF);
            if (pauseEnabled && keyPress.start) {
                stageMode = STAGEMODE_2P_PAUSED;
                PauseSound();
            }

            if (timeEnabled) {
                if (++frameCounter == 60) {
                    frameCounter = 0;
                    if (++stageSeconds > 59) {
                        stageSeconds = 0;
                        if (++stageMinutes > 59)
                            stageMinutes = 0;
                    }
                }
                stageMilliseconds = 100 * frameCounter / 60;
            }

            Process2PObjects();
            if (cameraTarget > -1) {
                if (cameraEnabled == 1) {
                    switch (cameraStyle) {
                        case 0: SetPlayerScreenPosition(&objectEntityList[cameraTarget]); break;
                        case 1:
                        case 2:
                        case 3: SetPlayerScreenPositionCDStyle(&objectEntityList[cameraTarget]); break;
                        case 4: SetPlayerHLockedScreenPosition(&objectEntityList[cameraTarget]); break;
                        default: break;
                    }
                }
                else {
                    SetPlayerLockedScreenPosition(&objectEntityList[cameraTarget]);
                }
            }

            ProcessParallaxAutoScroll();
            DrawStageGFX();
            break;
        case STAGEMODE_PAUSED: // Paused
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown, 0xFF);
            CheckKeyPress(&keyPress, 0xFF);

            if (keyPress.C) {
                keyPress.C = false;

                if (timeEnabled) {
                    if (++frameCounter == 60) {
                        frameCounter = 0;
                        if (++stageSeconds > 59) {
                            stageSeconds = 0;
                            if (++stageMinutes > 59)
                                stageMinutes = 0;
                        }
                    }
                    stageMilliseconds = 100 * frameCounter / 60;
                }

                ProcessObjects();
                if (cameraTarget > -1) {
                    if (cameraEnabled == 1) {
                        switch (cameraStyle) {
                            case 0: SetPlayerScreenPosition(&objectEntityList[cameraTarget]); break;
                            case 1:
                            case 2:
                            case 3: SetPlayerScreenPositionCDStyle(&objectEntityList[cameraTarget]); break;
                            case 4: SetPlayerHLockedScreenPosition(&objectEntityList[cameraTarget]); break;
                            default: break;
                        }
                    }
                    else {
                        SetPlayerLockedScreenPosition(&objectEntityList[cameraTarget]);
                    }
                }
            }

            if (pauseEnabled && keyPress.start) {
                stageMode = STAGEMODE_NORMAL;
                ResumeSound();
            }
            break;
        case STAGEMODE_STEPOVER:
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown, 0xFF);
            CheckKeyPress(&keyPress, 0xFF);

            if (keyPress.C) {
                keyPress.C = false;
                ProcessPausedObjects();
                DrawObjectList(0);
                DrawObjectList(1);
                DrawObjectList(2);
                DrawObjectList(3);
                DrawObjectList(4);
                DrawObjectList(5);
                DrawObjectList(6);
            }

            if (pauseEnabled && keyPress.start) {
                stageMode = STAGEMODE_2P;
                ResumeSound();
            }
            break;
        case STAGEMODE_FROZEN_PAUSED:
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown, 0xFF);
            CheckKeyPress(&keyPress, 0xFF);
            if (pauseEnabled && keyPress.start) {
                stageMode = STAGEMODE_FROZEN;
                PauseSound();
            }

            ProcessFrozenObjects();
            if (cameraTarget > -1) {
                if (cameraEnabled == 1) {
                    switch (cameraStyle) {
                        case 0: SetPlayerScreenPosition(&objectEntityList[cameraTarget]); break;
                        case 1:
                        case 2:
                        case 3: SetPlayerScreenPositionCDStyle(&objectEntityList[cameraTarget]); break;
                        case 4: SetPlayerHLockedScreenPosition(&objectEntityList[cameraTarget]); break;
                        default: break;
                    }
                }
                else {
                    SetPlayerLockedScreenPosition(&objectEntityList[cameraTarget]);
                }
            }

            DrawStageGFX();
            break;
        case STAGEMODE_2P_PAUSED:
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown, 0xFF);
            CheckKeyPress(&keyPress, 0xFF);
            if (pauseEnabled && keyPress.start) {
                stageMode = STAGEMODE_2P;
                PauseSound();
            }

            if (timeEnabled) {
                if (++frameCounter == 60) {
                    frameCounter = 0;
                    if (++stageSeconds > 59) {
                        stageSeconds = 0;
                        if (++stageMinutes > 59)
                            stageMinutes = 0;
                    }
                }
                stageMilliseconds = 100 * frameCounter / 60;
            }

            Process2PObjects();
            if (cameraTarget > -1) {
                if (cameraEnabled == 1) {
                    switch (cameraStyle) {
                        case 0: SetPlayerScreenPosition(&objectEntityList[cameraTarget]); break;
                        case 1:
                        case 2:
                        case 3: SetPlayerScreenPositionCDStyle(&objectEntityList[cameraTarget]); break;
                        case 4: SetPlayerHLockedScreenPosition(&objectEntityList[cameraTarget]); break;
                        default: break;
                    }
                }
                else {
                    SetPlayerLockedScreenPosition(&objectEntityList[cameraTarget]);
                }
            }

            ProcessParallaxAutoScroll();
            DrawStageGFX();
            break;
    }
}

void ProcessParallaxAutoScroll()
{
    for (int i = 0; i < hParallax.entryCount; ++i) hParallax.scrollPos[i] += hParallax.scrollSpeed[i];
    for (int i = 0; i < vParallax.entryCount; ++i) vParallax.scrollPos[i] += vParallax.scrollSpeed[i];
}

void LoadStageFiles(void)
{
    StopAllSfx();
    FileInfo infoStore;
    FileInfo info;
    int fileBuffer  = 0;
    int fileBuffer2 = 0;
    int scriptID    = 1;
    char strBuffer[0x100];

    if (!CheckCurrentStageFolder(stageListPosition)) {
#if RSDK_DEBUG
        printf("Loading Scene %s - %s\n", stageListNames[activeStageList], stageList[activeStageList][stageListPosition].name);
#endif
        ReleaseStageSfx();
        ClearScriptData();
        for (int i = SPRITESHEETS_MAX; i > 0; i--) RemoveGraphicsFile((char *)"", i - 1);

        bool loadGlobals = false;
        if (LoadStageFile("StageConfig.bin", stageListPosition, &info)) {
            FileRead(&loadGlobals, 1);
            CloseFile();
        }
        if (loadGlobals && LoadFile("Data/Game/GameConfig.bin", &info)) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
            
            byte buf[3];
            for (int c = 0; c < 0x60; ++c) {
                FileRead(buf, 3);
                SetPaletteEntry(c, buf[0], buf[1], buf[2]);
            }

            int globalObjectCount = 0;
            FileRead(&globalObjectCount, 1);
            for (int i = 0; i < globalObjectCount; ++i) {
                FileRead(&fileBuffer2, 1);
                FileRead(strBuffer, fileBuffer2);
                strBuffer[fileBuffer2] = 0;
                SetObjectTypeName(strBuffer, i + scriptID);
            }

            if (Engine.usingBytecode) {
                GetFileInfo(&infoStore);
                CloseFile();
                LoadBytecode(4, scriptID);
                scriptID += globalObjectCount;
                SetFileInfo(&infoStore);
            }
            else {
                for (int i = 0; i < globalObjectCount; ++i) {
                    FileRead(&fileBuffer2, 1);
                    FileRead(strBuffer, fileBuffer2);
                    strBuffer[fileBuffer2] = 0;
                    GetFileInfo(&infoStore);
                    CloseFile();
                    ParseScriptFile(strBuffer, scriptID++);
                    SetFileInfo(&infoStore);
                    if (Engine.gameMode == ENGINE_SCRIPTERROR)
                        return;
                }
            }
            CloseFile();
        }

        if (LoadStageFile("StageConfig.bin", stageListPosition, &info)) {
            FileRead(&fileBuffer, 1); // Load Globals

            byte clr[3];
            for (int i = 0x60; i < 0x80; ++i) {
                FileRead(&clr, 3);
                SetPaletteEntry(i, clr[0], clr[1], clr[2]);
            }

            FileRead(&stageSFXCount, 1);
            for (int i = 0; i < stageSFXCount; ++i) {
                FileRead(&fileBuffer2, 1);
                FileRead(strBuffer, fileBuffer2);
                strBuffer[fileBuffer2] = 0;

                SetSfxName(strBuffer, i + globalSFXCount);
            }
            for (int i = 0; i < stageSFXCount; ++i) {
                FileRead(&fileBuffer2, 1);
                FileRead(strBuffer, fileBuffer2);
                strBuffer[fileBuffer2] = 0;
                GetFileInfo(&infoStore);
                CloseFile();
                LoadSfx(strBuffer, globalSFXCount + i);
                SetFileInfo(&infoStore);
            }

            int stageObjectCount = 0;
            FileRead(&stageObjectCount, 1);
            for (int i = 0; i < stageObjectCount; ++i) {
                FileRead(&fileBuffer2, 1);
                FileRead(strBuffer, fileBuffer2);
                strBuffer[fileBuffer2] = 0;
                SetObjectTypeName(strBuffer, scriptID + i);
            }

            if (Engine.usingBytecode) {
                for (int i = 0; i < stageObjectCount; ++i) {
                    FileRead(&fileBuffer2, 1);
                    FileRead(strBuffer, fileBuffer2);
                    strBuffer[fileBuffer2] = 0;
                }
                GetFileInfo(&infoStore);
                CloseFile();
                LoadBytecode(activeStageList, scriptID);
                SetFileInfo(&infoStore);
            }
            else {
                for (int i = 0; i < stageObjectCount; ++i) {
                    FileRead(&fileBuffer2, 1);
                    FileRead(strBuffer, fileBuffer2);
                    strBuffer[fileBuffer2] = 0;
                    GetFileInfo(&infoStore);
                    CloseFile();
                    ParseScriptFile(strBuffer, scriptID + i);
                    SetFileInfo(&infoStore);
                    if (Engine.gameMode == ENGINE_SCRIPTERROR)
                        return;
                }
            }
            CloseFile();
        }

        LoadStageGIFFile(stageListPosition);
        LoadStageCollisions();
        LoadStageBackground();
    }
#if RSDK_DEBUG
    else {
        printf("Reloading Scene %s - %s\n", stageListNames[activeStageList], stageList[activeStageList][stageListPosition].name);
    }
#endif
    LoadStageChunks();
    for (int i = 0; i < TRACK_COUNT; ++i) SetMusicTrack((char *)"", i, 0, 0);

    
  memset(objectEntityList, 0, ENTITY_COUNT * sizeof(Entity));
    for (int i = 0; i < ENTITY_COUNT; ++i) {
        objectEntityList[i].drawOrder      = 3;
        objectEntityList[i].scale          = 512;
        //objectEntityList[i].flailing[0]          = 1;
        //objectEntityList[i].flailing[1]          = 1;
        //objectEntityList[i].flailing[2]          = 1;
    }
    LoadActLayout();
    Init3DFloorBuffer(0);
    ProcessStartupObjects();
}
int LoadActFile(const char *ext, int stageID, FileInfo *info)
{
    char dest[0x40];

    StrCopy(dest, "Data/Stages/");
    StrAdd(dest, stageList[activeStageList][stageID].folder);
    StrAdd(dest, "/Act");
    StrAdd(dest, stageList[activeStageList][stageID].id);
    StrAdd(dest, ext);

    ConvertStringToInteger(stageList[activeStageList][stageID].id, &actID);

    return LoadFile(dest, info);
}
int LoadStageFile(const char *filePath, int stageID, FileInfo *info)
{
    char dest[0x40];

    StrCopy(dest, "Data/Stages/");
    StrAdd(dest, stageList[activeStageList][stageID].folder);
    StrAdd(dest, "/");
    StrAdd(dest, filePath);
    return LoadFile(dest, info);
}
void LoadActLayout()
{
    FileInfo info;
    if (LoadActFile(".bin", stageListPosition, &info)) {
        byte fileBuffer[4];

        int length = 0;
        FileRead(&length, 1);
        titleCardWord2 = (byte)length;
        for (int i = 0; i < length; i++) {
            FileRead(&titleCardText[i], 1);
            if (titleCardText[i] == '-')
                titleCardWord2 = (byte)(i + 1);
        }
        titleCardText[length] = '\0';

        // READ TILELAYER
        FileRead(activeTileLayers, 4);
        FileRead(&tLayerMidPoint, 1);

        FileRead(&stageLayouts[0].width, 1);
        FileRead(fileBuffer, 1); //Unused

        FileRead(&stageLayouts[0].height, 1);
        FileRead(fileBuffer, 1); // Unused
        xBoundary1    = 0;
        newXBoundary1 = 0;
        yBoundary1    = 0;
        newYBoundary1 = 0;
        xBoundary2    = stageLayouts[0].width << 7;
        yBoundary2    = stageLayouts[0].height << 7;
        waterLevel    = yBoundary2 + 128;
        newXBoundary2 = stageLayouts[0].width << 7;
        newYBoundary2 = stageLayouts[0].height << 7;

        memset(stageLayouts[0].tiles, 0, TILELAYER_CHUNK_MAX * sizeof(ushort));
        byte *lineScrollPtr = stageLayouts[0].lineScroll;
        memset(stageLayouts[0].lineScroll, 0, 0x7FFF);

        for (int y = 0; y < stageLayouts[0].height; ++y) {
            ushort *tiles = &stageLayouts[0].tiles[(y * 0x100)];
            for (int x = 0; x < stageLayouts[0].width; ++x) {
                FileRead(&fileBuffer[0], 1);
                tiles[x] = fileBuffer[0];
                FileRead(&fileBuffer[0], 1);
                tiles[x] += fileBuffer[0] << 8;
            }
        }

        // READ OBJECTS
        FileRead(&fileBuffer[0], 1);
        int ObjectCount = fileBuffer[0];
        FileRead(&fileBuffer[0], 1);
        ObjectCount += fileBuffer[0] << 8;

        Entity *object = &objectEntityList[32];
        for (int i = 0; i < ObjectCount; ++i) {
            FileRead(fileBuffer, 2);
            ushort attribs = (fileBuffer[1] << 8) + fileBuffer[0];

            FileRead(fileBuffer, 1);
            object->type = fileBuffer[0];

            FileRead(fileBuffer, 1);
            object->propertyValue = fileBuffer[0];

            FileRead(&fileBuffer, 4);
            object->XPos = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];

            FileRead(&fileBuffer, 4);
            object->YPos = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];

            if (attribs & 0x1) {
                FileRead(&fileBuffer, 4);
                object->state = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }
            if (attribs & 0x2) {
                FileRead(fileBuffer, 1);
                object->direction = fileBuffer[0];
            }
            if (attribs & 0x4) {
                FileRead(&fileBuffer, 4);
                object->scale = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }
            if (attribs & 0x8) {
                FileRead(&fileBuffer, 4);
                object->rotation = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }
            if (attribs & 0x10) {
                FileRead(fileBuffer, 1);
                object->drawOrder = fileBuffer[0];
            }
            if (attribs & 0x20) {
                FileRead(fileBuffer, 1);
                object->priority = fileBuffer[0];
            }
            if (attribs & 0x40) {
                FileRead(fileBuffer, 1);
                object->alpha = fileBuffer[0];
            }
            if (attribs & 0x80) {
                FileRead(fileBuffer, 1);
                object->animation = fileBuffer[0];
            }
            if (attribs & 0x100) {
                FileRead(&fileBuffer, 4);
                object->animationSpeed = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }
            if (attribs & 0x200) {
                FileRead(fileBuffer, 1);
                object->frame = fileBuffer[0];
            }
            if (attribs & 0x400) {
                FileRead(fileBuffer, 1);
                object->inkEffect = fileBuffer[0];
            }
            if (attribs & 0x800) {
                FileRead(&fileBuffer, 4);
                object->values[0] = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }
            if (attribs & 0x1000) {
                FileRead(&fileBuffer, 4);
                object->values[1] = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }
            if (attribs & 0x2000) {
                FileRead(&fileBuffer, 4);
                object->values[2] = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }
            if (attribs & 0x4000) {
                FileRead(&fileBuffer, 4);
                object->values[3] = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }

            ++object;
        }
    }
    stageLayouts[0].type = LAYER_HSCROLL;
    
}
void LoadStageBackground()
{
    for (int i = 0; i < LAYER_COUNT; ++i) {
        stageLayouts[i].type               = LAYER_NOSCROLL;
        stageLayouts[i].deformationOffset  = 0;
        stageLayouts[i].deformationOffsetW = 0;
    }
    for (int i = 0; i < PARALLAX_COUNT; ++i) {
        hParallax.scrollPos[i] = 0;
        vParallax.scrollPos[i] = 0;
    }

    FileInfo info;
    if (LoadStageFile("Backgrounds.bin", stageListPosition, &info)) {
        int fileBuffer = 0;
        int layerCount = 0;
        FileRead(&layerCount, 1);
        FileRead(&hParallax.entryCount, 1);
        for (int i = 0; i < hParallax.entryCount; ++i) {
            FileRead(&fileBuffer, 1);
            hParallax.parallaxFactor[i] = fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            hParallax.parallaxFactor[i] += fileBuffer;

            FileRead(&fileBuffer, 1);
            hParallax.scrollSpeed[i] = fileBuffer << 10;

            hParallax.scrollPos[i] = 0;

            FileRead(&hParallax.deform[i], 1);
        }

        FileRead(&vParallax.entryCount, 1);
        for (int i = 0; i < vParallax.entryCount; ++i) {
            FileRead(&fileBuffer, 1);
            vParallax.parallaxFactor[i] = fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            vParallax.parallaxFactor[i] += fileBuffer;

            FileRead(&fileBuffer, 1);
            vParallax.scrollSpeed[i] = fileBuffer << 10;

            vParallax.scrollPos[i] = 0;

            FileRead(&vParallax.deform[i], 1);
        }

        for (int i = 1; i < layerCount + 1; ++i) {
            FileRead(&fileBuffer, 1);
            stageLayouts[i].width = fileBuffer;
            FileRead(&fileBuffer, 1); // Unused (???)
            FileRead(&fileBuffer, 1);
            stageLayouts[i].height = fileBuffer;
            FileRead(&fileBuffer, 1); //Unused (???)
            FileRead(&fileBuffer, 1);
            stageLayouts[i].type = fileBuffer;
            FileRead(&fileBuffer, 1);
            stageLayouts[i].parallaxFactor = fileBuffer;
            FileRead(&fileBuffer, 1);
            stageLayouts[i].parallaxFactor += fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            stageLayouts[i].scrollSpeed = fileBuffer << 10;
            stageLayouts[i].scrollPos   = 0;
            
            memset(stageLayouts[i].tiles, 0, TILELAYER_CHUNK_MAX * sizeof(ushort));
            byte *lineScrollPtr = stageLayouts[i].lineScroll;
            memset(stageLayouts[i].lineScroll, 0, 0x7FFF);

            // Read Line Scroll
            byte buf[3];
            while (true) {
                FileRead(&buf[0], 1);
                if (buf[0] == 0xFF) {
                    FileRead(&buf[1], 1);
                    if (buf[1] == 0xFF) {
                        break;
                    }
                    else {
                        FileRead(&buf[2], 1);
                        int val = buf[1];
                        int cnt = buf[2] - 1;
                        for (int c = 0; c < cnt; ++c) *lineScrollPtr++ = val;
                    }
                }
                else {
                    *lineScrollPtr++ = buf[0];
                }
            }

            // Read Layout
            for (int y = 0; y < stageLayouts[i].height; ++y) {
                ushort *chunks = &stageLayouts[i].tiles[y * 0x100];
                for (int x = 0; x < stageLayouts[i].width; ++x) {
                    FileRead(&fileBuffer, 1);
                    *chunks = fileBuffer;
                    FileRead(&fileBuffer, 1);
                    *chunks += fileBuffer << 8;
                    ++chunks;
                }
            }
        }

        CloseFile();
    }
}
void LoadStageChunks()
{
    FileInfo info;
    byte entry[3];

    if (LoadStageFile("128x128Tiles.bin", stageListPosition, &info)) {
        for (int i = 0; i < CHUNKTILE_COUNT; ++i) {
            FileRead(&entry, 3);
            entry[0] -= (byte)((entry[0] >> 6) << 6);

            tiles128x128.visualPlane[i] = (byte)(entry[0] >> 4);
            entry[0] -= 16 * (entry[0] >> 4);

            tiles128x128.direction[i] = (byte)(entry[0] >> 2);
            entry[0] -= 4 * (entry[0] >> 2);

            tiles128x128.tileIndex[i]  = entry[1] + (entry[0] << 8);
            tiles128x128.gfxDataPos[i] = tiles128x128.tileIndex[i] << 8;

            tiles128x128.collisionFlags[0][i] = entry[2] >> 4;
            tiles128x128.collisionFlags[1][i] = entry[2] - ((entry[2] >> 4) << 4);
        }
        CloseFile();
    }
}
void LoadStageCollisions()
{
    FileInfo info;
    if (LoadStageFile("CollisionMasks.bin", stageListPosition, &info)) {

        int fileBuffer = 0;
        int tileIndex  = 0;
        for (int t = 0; t < 1024; ++t) {
            for (int p = 0; p < 2; ++p) {
                FileRead(&fileBuffer, 1);
                bool isCeiling             = fileBuffer >> 4;
                collisionMasks[p].flags[t] = fileBuffer & 0xF;
                FileRead(&fileBuffer, 1);
                collisionMasks[p].angles[t] = fileBuffer;
                FileRead(&fileBuffer, 1);
                collisionMasks[p].angles[t] += fileBuffer << 8;
                FileRead(&fileBuffer, 1);
                collisionMasks[p].angles[t] += fileBuffer << 16;
                FileRead(&fileBuffer, 1);
                collisionMasks[p].angles[t] += fileBuffer << 24;

                if (isCeiling) // Ceiling Tile
                {
                    for (int c = 0; c < TILE_SIZE; c += 2) {
                        FileRead(&fileBuffer, 1);
                        collisionMasks[p].roofMasks[c + tileIndex]     = fileBuffer >> 4;
                        collisionMasks[p].roofMasks[c + tileIndex + 1] = fileBuffer & 0xF;
                    }

                    // Has Collision (Pt 1)
                    FileRead(&fileBuffer, 1);
                    int id = 1;
                    for (int c = 0; c < TILE_SIZE / 2; ++c) {
                        if (fileBuffer & id) {
                            collisionMasks[p].floorMasks[c + tileIndex + 8] = 0;
                        }
                        else {
                            collisionMasks[p].floorMasks[c + tileIndex + 8] = 0x40;
                            collisionMasks[p].roofMasks[c + tileIndex + 8]  = -0x40;
                        }
                        id <<= 1;
                    }

                    // Has Collision (Pt 2)
                    FileRead(&fileBuffer, 1);
                    id = 1;
                    for (int c = 0; c < TILE_SIZE / 2; ++c) {
                        if (fileBuffer & id) {
                            collisionMasks[p].floorMasks[c + tileIndex] = 0;
                        }
                        else {
                            collisionMasks[p].floorMasks[c + tileIndex] = 0x40;
                            collisionMasks[p].roofMasks[c + tileIndex]  = -0x40;
                        }
                        id <<= 1;
                    }

                    // LWall rotations
                    for (int c = 0; c < TILE_SIZE; ++c) {
                        int h = 0;
                        while (h > -1) {
                            if (h == TILE_SIZE) {
                                collisionMasks[p].lWallMasks[c + tileIndex] = 0x40;
                                h                                           = -1;
                            }
                            else if (c > collisionMasks[p].roofMasks[h + tileIndex]) {
                                ++h;
                            }
                            else {
                                collisionMasks[p].lWallMasks[c + tileIndex] = h;
                                h                                           = -1;
                            }
                        }
                    }

                    // RWall rotations
                    for (int c = 0; c < TILE_SIZE; ++c) {
                        int h = TILE_SIZE - 1;
                        while (h < TILE_SIZE) {
                            if (h == -1) {
                                collisionMasks[p].rWallMasks[c + tileIndex] = -0x40;
                                h                                           = TILE_SIZE;
                            }
                            else if (c > collisionMasks[p].roofMasks[h + tileIndex]) {
                                --h;
                            }
                            else {
                                collisionMasks[p].rWallMasks[c + tileIndex] = h;
                                h                                           = TILE_SIZE;
                            }
                        }
                    }
                }
                else // Regular Tile
                {
                    for (int c = 0; c < TILE_SIZE; c += 2) {
                        FileRead(&fileBuffer, 1);
                        collisionMasks[p].floorMasks[c + tileIndex]     = fileBuffer >> 4;
                        collisionMasks[p].floorMasks[c + tileIndex + 1] = fileBuffer & 0xF;
                    }
                    FileRead(&fileBuffer, 1);
                    int id = 1;
                    for (int c = 0; c < TILE_SIZE / 2; ++c) // HasCollision
                    {
                        if (fileBuffer & id) {
                            collisionMasks[p].roofMasks[c + tileIndex + 8] = 0xF;
                        }
                        else {
                            collisionMasks[p].floorMasks[c + tileIndex + 8] = 0x40;
                            collisionMasks[p].roofMasks[c + tileIndex + 8]  = -0x40;
                        }
                        id <<= 1;
                    }

                    FileRead(&fileBuffer, 1);
                    id = 1;
                    for (int c = 0; c < TILE_SIZE / 2; ++c) // HasCollision (pt 2)
                    {
                        if (fileBuffer & id) {
                            collisionMasks[p].roofMasks[c + tileIndex] = 0xF;
                        }
                        else {
                            collisionMasks[p].floorMasks[c + tileIndex] = 0x40;
                            collisionMasks[p].roofMasks[c + tileIndex]  = -0x40;
                        }
                        id <<= 1;
                    }

                    //LWall rotations
                    for (int c = 0; c < TILE_SIZE; ++c) {
                        char h = 0;
                        while (h > -1) {
                            if (h == TILE_SIZE) {
                                collisionMasks[p].lWallMasks[c + tileIndex] = 0x40;
                                h                                           = -1;
                            }
                            else if (c < collisionMasks[p].floorMasks[h + tileIndex]) {
                                ++h;
                            }
                            else {
                                collisionMasks[p].lWallMasks[c + tileIndex] = h;
                                h                                           = -1;
                            }
                        }
                    }

                    //RWall rotations
                    for (int c = 0; c < TILE_SIZE; ++c) {
                        char h = TILE_SIZE - 1;
                        while (h < TILE_SIZE) {
                            if (h == -1) {
                                collisionMasks[p].rWallMasks[c + tileIndex] = -0x40;
                                h                                           = TILE_SIZE;
                            }
                            else if (c < collisionMasks[p].floorMasks[h + tileIndex]) {
                                --h;
                            }
                            else {
                                collisionMasks[p].rWallMasks[c + tileIndex] = h;
                                h                                           = TILE_SIZE;
                            }
                        }
                    }
                }
            }
            tileIndex += 16;
        }
        CloseFile();
    }
}
void LoadStageGIFFile(int stageID)
{
    FileInfo info;
    if (LoadStageFile("16x16Tiles.gif", stageID, &info)) {
        int fileBuffer = 0;

        SetFilePosition(6); // GIF89a
        FileRead(&fileBuffer, 1);
        int width = fileBuffer;
        FileRead(&fileBuffer, 1);
        width += (fileBuffer << 8);
        FileRead(&fileBuffer, 1);
        int height = fileBuffer;
        FileRead(&fileBuffer, 1);
        height += (fileBuffer << 8);

        FileRead(&fileBuffer, 1); // Palette Size (thrown away) :/
        FileRead(&fileBuffer, 1); // BG Colour index (thrown away)
        FileRead(&fileBuffer, 1); // idk actually (still thrown away)

        byte clr[3];
        for (int c = 0; c < 0x80; ++c) FileRead(clr, 3);
        for (int c = 0x80; c < 0x100; ++c) {
            FileRead(clr, 3);
            activePalette32[c].r = clr[0];
            activePalette32[c].g = clr[1];
            activePalette32[c].b = clr[2];
            activePalette[c]     = (clr[2] >> 3) | 32 * (clr[1] >> 2) | ((ushort)(clr[0] >> 3) << 11);
        }

        FileRead(&fileBuffer, 1);
        while (fileBuffer != ',') FileRead(&fileBuffer, 1); // gif image start identifier

        FileRead(&fileBuffer, 2);
        FileRead(&fileBuffer, 2);
        FileRead(&fileBuffer, 2);
        FileRead(&fileBuffer, 2);
        FileRead(&fileBuffer, 1);
        bool interlaced = (fileBuffer & 0x40) >> 6;
        if ((unsigned int)fileBuffer >> 7 == 1) {
            int c = 128;
            do {
                ++c;
                FileRead(&fileBuffer, 3);
            } while (c != 256);
        }

        ReadGifPictureData(width, height, interlaced, tilesetGFXData, 0);

        byte transparent = tilesetGFXData[0];
        for (int i = 0; i < 0x40000; ++i) {
            if (tilesetGFXData[i] == transparent)
                tilesetGFXData[i] = 0;
        }

        CloseFile();
    }
}

void ResetBackgroundSettings()
{
    for (int i = 0; i < LAYER_COUNT; ++i) {
        stageLayouts[i].deformationOffset  = 0;
        stageLayouts[i].deformationOffsetW = 0;
        stageLayouts[i].scrollPos          = 0;
    }

    for (int i = 0; i < PARALLAX_COUNT; ++i) {
        hParallax.scrollPos[i] = 0;
        vParallax.scrollPos[i] = 0;
    }

    for (int i = 0; i < DEFORM_COUNT; ++i) {
        bgDeformationData0[i] = 0;
        bgDeformationData1[i] = 0;
        bgDeformationData2[i] = 0;
        bgDeformationData3[i] = 0;
    }
}
void SetLayerDeformation(int deformID, int deformationA, int deformationB, int deformType, int deformOffset, int deformCount)
{
    int *deformPtr = nullptr;
    switch (deformID) {
        case DEFORM_FG: deformPtr = bgDeformationData0; break;
        case DEFORM_FG_WATER: deformPtr = bgDeformationData1; break;
        case DEFORM_BG: deformPtr = bgDeformationData2; break;
        case DEFORM_BG_WATER: deformPtr = bgDeformationData3; break;
        default: break;
    }

    if (deformType == 1) {
        int* d = &deformPtr[deformOffset];
        for (int i = 0; i < deformCount; ++i) {
            *d = deformationB * sinVal512[(i << 9) / deformationA & 0x1FF] >> 9;
            ++d;
        }
    }
    else {
        for (int i = 0; i < 0x200 * 0x100; i += 0x200) {
            *deformPtr = deformationB * sinVal512[i / deformationA & 0x1FF] >> 9;
            if (*deformPtr >= deformationB)
                *deformPtr = deformationB - 1;
            ++deformPtr;
        }
    }

    switch (deformID) {
        case 0:
            for (int i = DEFORM_STORE; i < DEFORM_COUNT; ++i) bgDeformationData0[i] = bgDeformationData0[i - DEFORM_STORE];
            break;
        case 1:
            for (int i = DEFORM_STORE; i < DEFORM_COUNT; ++i) bgDeformationData1[i] = bgDeformationData1[i - DEFORM_STORE];
            break;
        case 2:
            for (int i = DEFORM_STORE; i < DEFORM_COUNT; ++i) bgDeformationData2[i] = bgDeformationData2[i - DEFORM_STORE];
            break;
        case 3:
            for (int i = DEFORM_STORE; i < DEFORM_COUNT; ++i) bgDeformationData3[i] = bgDeformationData3[i - DEFORM_STORE];
            break;
        default: break;
    }
}

void SetPlayerScreenPosition(Entity *Player)
{
    /*int playerXPos = Player->XPos >> 16;
    int playerYPos = Player->YPos >> 16;
    if (newYBoundary1 > yBoundary1) {
        if (yScrollOffset <= newYBoundary1)
            yBoundary1 = yScrollOffset;
        else
            yBoundary1 = newYBoundary1;
    }
    if (newYBoundary1 < yBoundary1) {
        if (yScrollOffset <= yBoundary1)
            --yBoundary1;
        else
            yBoundary1 = newYBoundary1;
    }
    if (newYBoundary2 < yBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= yBoundary2 || yScrollOffset + SCREEN_YSIZE <= newYBoundary2)
            --yBoundary2;
        else
            yBoundary2 = yScrollOffset + SCREEN_YSIZE;
    }
    if (newYBoundary2 > yBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= yBoundary2)
            ++yBoundary2;
        else
            yBoundary2 = newYBoundary2;
    }
    if (newXBoundary1 > xBoundary1) {
        if (xScrollOffset <= newXBoundary1)
            xBoundary1 = xScrollOffset;
        else
            xBoundary1 = newXBoundary1;
    }
    if (newXBoundary1 < xBoundary1) {
        if (xScrollOffset <= xBoundary1) {
            --xBoundary1;
            if (Player->XVelocity < 0) {
                xBoundary1 += Player->XVelocity >> 16;
                if (xBoundary1 < newXBoundary1)
                    xBoundary1 = newXBoundary1;
            }
        }
        else {
            xBoundary1 = newXBoundary1;
        }
    }
    if (newXBoundary2 < xBoundary2) {
        if (SCREEN_XSIZE + xScrollOffset >= xBoundary2)
            xBoundary2 = SCREEN_XSIZE + xScrollOffset;
        else
            xBoundary2 = newXBoundary2;
    }
    if (newXBoundary2 > xBoundary2) {
        if (SCREEN_XSIZE + xScrollOffset >= xBoundary2) {
            ++xBoundary2;
            if (Player->XVelocity > 0) {
                xBoundary2 += Player->XVelocity >> 16;
                if (xBoundary2 > newXBoundary2)
                    xBoundary2 = newXBoundary2;
            }
        }
        else {
            xBoundary2 = newXBoundary2;
        }
    }
    int xscrollA     = xScrollA;
    int xscrollB     = xScrollB;
    int scrollAmount = playerXPos - (SCREEN_CENTERX + xScrollA);
    if (abs(playerXPos - (SCREEN_CENTERX + xScrollA)) >= 25) {
        if (scrollAmount <= 0)
            xscrollA -= 16;
        else
            xscrollA += 16;
        xscrollB = SCREEN_XSIZE + xscrollA;
    }
    else {
        if (playerXPos > SCREEN_SCROLL_RIGHT + xscrollA) {
            xscrollA = playerXPos - SCREEN_SCROLL_RIGHT;
            xscrollB = SCREEN_XSIZE + playerXPos - SCREEN_SCROLL_RIGHT;
        }
        if (playerXPos < SCREEN_SCROLL_LEFT + xscrollA) {
            xscrollA = playerXPos - SCREEN_SCROLL_LEFT;
            xscrollB = SCREEN_XSIZE + playerXPos - SCREEN_SCROLL_LEFT;
        }
    }
    if (xscrollA < xBoundary1) {
        xscrollA = xBoundary1;
        xscrollB = SCREEN_XSIZE + xBoundary1;
    }
    if (xscrollB > xBoundary2) {
        xscrollB = xBoundary2;
        xscrollA = xBoundary2 - SCREEN_XSIZE;
    }

    xScrollA = xscrollA;
    xScrollB = xscrollB;
    if (playerXPos <= SCREEN_CENTERX + xscrollA) {
        Player->screenXPos = cameraShakeX + playerXPos - xscrollA;
        xScrollOffset      = xscrollA - cameraShakeX;
    }
    else {
        xScrollOffset      = cameraShakeX + playerXPos - SCREEN_CENTERX;
        Player->screenXPos = SCREEN_CENTERX - cameraShakeX;
        if (playerXPos > xscrollB - SCREEN_CENTERX) {
            Player->screenXPos = cameraShakeX + SCREEN_CENTERX + playerXPos - (xscrollB - SCREEN_CENTERX);
            xScrollOffset      = xscrollB - SCREEN_XSIZE - cameraShakeX;
        }
    }

    int yscrollA     = yScrollA;
    int yscrollB     = yScrollB;
    int adjustYPos   = cameraAdjustY + playerYPos;
    int adjustAmount = Player->lookPos + adjustYPos - (yscrollA + SCREEN_SCROLL_UP);
    if (Player->trackScroll) {
        yScrollMove = 32;
    }
    else {
        if (yScrollMove == 32) {
            yScrollMove = 2 * ((SCREEN_SCROLL_UP - Player->screenYPos - Player->lookPos) >> 1);
            if (yScrollMove > 32)
                yScrollMove = 32;
            if (yScrollMove < -32)
                yScrollMove = -32;
        }
        if (yScrollMove > 0)
            yScrollMove -= 6;
        yScrollMove += yScrollMove < 0 ? 6 : 0;
    }

    if (abs(adjustAmount) >= abs(yScrollMove) + 17) {
        if (adjustAmount <= 0)
            yscrollA -= 16;
        else
            yscrollA += 16;
        yscrollB = yscrollA + SCREEN_YSIZE;
    }
    else if (yScrollMove == 32) {
        if (Player->lookPos + adjustYPos > yscrollA + yScrollMove + SCREEN_SCROLL_UP) {
            yscrollA = Player->lookPos + adjustYPos - (yScrollMove + SCREEN_SCROLL_UP);
            yscrollB = yscrollA + SCREEN_YSIZE;
        }
        if (Player->lookPos + adjustYPos < yscrollA + SCREEN_SCROLL_UP - yScrollMove) {
            yscrollA = Player->lookPos + adjustYPos - (SCREEN_SCROLL_UP - yScrollMove);
            yscrollB = yscrollA + SCREEN_YSIZE;
        }
    }
    else {
        yscrollA = Player->lookPos + adjustYPos + yScrollMove - SCREEN_SCROLL_UP;
        yscrollB = yscrollA + SCREEN_YSIZE;
    }
    if (yscrollA < yBoundary1) {
        yscrollA = yBoundary1;
        yscrollB = yBoundary1 + SCREEN_YSIZE;
    }
    if (yscrollB > yBoundary2) {
        yscrollB = yBoundary2;
        yscrollA = yBoundary2 - SCREEN_YSIZE;
    }
    yScrollA = yscrollA;
    yScrollB = yscrollB;
    if (Player->lookPos + adjustYPos <= yScrollA + SCREEN_SCROLL_UP) {
        Player->screenYPos = adjustYPos - yScrollA - cameraShakeY;
        yScrollOffset      = cameraShakeY + yScrollA;
    }
    else {
        yScrollOffset      = cameraShakeY + adjustYPos + Player->lookPos - SCREEN_SCROLL_UP;
        Player->screenYPos = SCREEN_SCROLL_UP - Player->lookPos - cameraShakeY;
        if (Player->lookPos + adjustYPos > yScrollB - SCREEN_SCROLL_DOWN) {
            Player->screenYPos = adjustYPos - (yScrollB - SCREEN_SCROLL_DOWN) + cameraShakeY + SCREEN_SCROLL_UP;
            yScrollOffset      = yScrollB - SCREEN_YSIZE - cameraShakeY;
        }
    }

    Player->screenYPos -= cameraAdjustY;
    if (cameraShakeX) {
        if (cameraShakeX <= 0) {
            cameraShakeX = -cameraShakeX--;
        }
        else {
            cameraShakeX = -cameraShakeX;
        }
    }

    if (!cameraShakeY)
        return;
    if (cameraShakeY <= 0) {
        cameraShakeY = -cameraShakeY--;
    }
    else {
        cameraShakeY = -cameraShakeY;
    }*/
}
void SetPlayerScreenPositionCDStyle(Entity *Player)
{
    /*int playerXPos = Player->XPos >> 16;
    int playerYPos = Player->YPos >> 16;
    if (newYBoundary1 > yBoundary1) {
        if (yScrollOffset <= newYBoundary1)
            yBoundary1 = yScrollOffset;
        else
            yBoundary1 = newYBoundary1;
    }
    if (newYBoundary1 < yBoundary1) {
        if (yScrollOffset <= yBoundary1)
            --yBoundary1;
        else
            yBoundary1 = newYBoundary1;
    }
    if (newYBoundary2 < yBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= yBoundary2 || yScrollOffset + SCREEN_YSIZE <= newYBoundary2)
            --yBoundary2;
        else
            yBoundary2 = yScrollOffset + SCREEN_YSIZE;
    }
    if (newYBoundary2 > yBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= yBoundary2)
            ++yBoundary2;
        else
            yBoundary2 = newYBoundary2;
    }
    if (newXBoundary1 > xBoundary1) {
        if (xScrollOffset <= newXBoundary1)
            xBoundary1 = xScrollOffset;
        else
            xBoundary1 = newXBoundary1;
    }
    if (newXBoundary1 < xBoundary1) {
        if (xScrollOffset <= xBoundary1) {
            --xBoundary1;
            if (Player->XVelocity < 0) {
                xBoundary1 += Player->XVelocity >> 16;
                if (xBoundary1 < newXBoundary1)
                    xBoundary1 = newXBoundary1;
            }
        }
        else {
            xBoundary1 = newXBoundary1;
        }
    }
    if (newXBoundary2 < xBoundary2) {
        if (SCREEN_XSIZE + xScrollOffset >= xBoundary2)
            xBoundary2 = SCREEN_XSIZE + xScrollOffset;
        else
            xBoundary2 = newXBoundary2;
    }
    if (newXBoundary2 > xBoundary2) {
        if (SCREEN_XSIZE + xScrollOffset >= xBoundary2) {
            ++xBoundary2;
            if (Player->XVelocity > 0) {
                xBoundary2 += Player->XVelocity >> 16;
                if (xBoundary2 > newXBoundary2)
                    xBoundary2 = newXBoundary2;
            }
        }
        else {
            xBoundary2 = newXBoundary2;
        }
    }
    if (!Player->gravity) {
        if (Player->boundEntity->direction) {
            if (cameraStyle == 3 || Player->speed < -0x5F5C2)
                cameraLagStyle = 2;
            else
                cameraLagStyle = 0;
        }
        else {
            cameraLagStyle = (cameraStyle == 2 || Player->speed > 0x5F5C2) != 0;
        }
    }
    if (cameraLagStyle) {
        if (cameraLagStyle == 1) {
            if (cameraLag > -64)
                cameraLag -= 2;
        }
        else if (cameraLagStyle == 2 && cameraLag < 64) {
            cameraLag += 2;
        }
    }
    else {
        cameraLag += cameraLag < 0 ? 2 : 0;
        if (cameraLag > 0)
            cameraLag -= 2;
    }
    if (playerXPos <= cameraLag + SCREEN_CENTERX + xBoundary1) {
        Player->screenXPos = cameraShakeX + playerXPos - xBoundary1;
        xScrollOffset      = xBoundary1 - cameraShakeX;
    }
    else {
        xScrollOffset      = cameraShakeX + playerXPos - SCREEN_CENTERX - cameraLag;
        Player->screenXPos = cameraLag + SCREEN_CENTERX - cameraShakeX;
        if (playerXPos - cameraLag > xBoundary2 - SCREEN_CENTERX) {
            Player->screenXPos = cameraShakeX + SCREEN_CENTERX + playerXPos - (xBoundary2 - SCREEN_CENTERX);
            xScrollOffset      = xBoundary2 - SCREEN_XSIZE - cameraShakeX;
        }
    }
    xScrollA         = xScrollOffset;
    xScrollB         = SCREEN_XSIZE + xScrollOffset;
    int yscrollA     = yScrollA;
    int yscrollB     = yScrollB;
    int adjustY      = cameraAdjustY + playerYPos;
    int adjustOffset = Player->lookPos + adjustY - (yScrollA + SCREEN_SCROLL_UP);
    if (Player->trackScroll == 1) {
        yScrollMove = 32;
    }
    else {
        if (yScrollMove == 32) {
            yScrollMove = 2 * ((SCREEN_SCROLL_UP - Player->screenYPos - Player->lookPos) >> 1);
            if (yScrollMove > 32)
                yScrollMove = 32;
            if (yScrollMove < -32)
                yScrollMove = -32;
        }
        if (yScrollMove > 0)
            yScrollMove -= 6;
        yScrollMove += yScrollMove < 0 ? 6 : 0;
    }

    int absAdjust = abs(adjustOffset);
    if (absAdjust >= abs(yScrollMove) + 17) {
        if (adjustOffset <= 0)
            yscrollA -= 16;
        else
            yscrollA += 16;
        yscrollB = yscrollA + SCREEN_YSIZE;
    }
    else if (yScrollMove == 32) {
        if (Player->lookPos + adjustY > yscrollA + yScrollMove + SCREEN_SCROLL_UP) {
            yscrollA = Player->lookPos + adjustY - (yScrollMove + SCREEN_SCROLL_UP);
            yscrollB = yscrollA + SCREEN_YSIZE;
        }
        if (Player->lookPos + adjustY < yscrollA + SCREEN_SCROLL_UP - yScrollMove) {
            yscrollA = Player->lookPos + adjustY - (SCREEN_SCROLL_UP - yScrollMove);
            yscrollB = yscrollA + SCREEN_YSIZE;
        }
    }
    else {
        yscrollA = Player->lookPos + adjustY + yScrollMove - SCREEN_SCROLL_UP;
        yscrollB = yscrollA + SCREEN_YSIZE;
    }
    if (yscrollA < yBoundary1) {
        yscrollA = yBoundary1;
        yscrollB = yBoundary1 + SCREEN_YSIZE;
    }
    if (yscrollB > yBoundary2) {
        yscrollB = yBoundary2;
        yscrollA = yBoundary2 - SCREEN_YSIZE;
    }
    yScrollA = yscrollA;
    yScrollB = yscrollB;
    if (Player->lookPos + adjustY <= yscrollA + SCREEN_SCROLL_UP) {
        Player->screenYPos = adjustY - yscrollA - cameraShakeY;
        yScrollOffset      = cameraShakeY + yscrollA;
    }
    else {
        yScrollOffset      = cameraShakeY + adjustY + Player->lookPos - SCREEN_SCROLL_UP;
        Player->screenYPos = SCREEN_SCROLL_UP - Player->lookPos - cameraShakeY;
        if (Player->lookPos + adjustY > yscrollB - SCREEN_SCROLL_DOWN) {
            Player->screenYPos = adjustY - (yscrollB - SCREEN_SCROLL_DOWN) + cameraShakeY + SCREEN_SCROLL_UP;
            yScrollOffset      = yscrollB - SCREEN_YSIZE - cameraShakeY;
        }
    }

    Player->screenYPos -= cameraAdjustY;
    if (cameraShakeX) {
        if (cameraShakeX <= 0) {
            cameraShakeX = -cameraShakeX--;
        }
        else {
            cameraShakeX = -cameraShakeX;
        }
    }
    if (!cameraShakeY)
        return;
    if (cameraShakeY <= 0) {
        cameraShakeY = -cameraShakeY--;
    }
    else {
        cameraShakeY = -cameraShakeY;
    }*/
}
void SetPlayerHLockedScreenPosition(Entity *Player)
{
    /*int playerXPos = Player->XPos >> 16;
    int playerYPos = Player->YPos >> 16;
    if (newYBoundary1 > yBoundary1) {
        if (yScrollOffset <= newYBoundary1)
            yBoundary1 = yScrollOffset;
        else
            yBoundary1 = newYBoundary1;
    }
    if (newYBoundary1 < yBoundary1) {
        if (yScrollOffset <= yBoundary1)
            --yBoundary1;
        else
            yBoundary1 = newYBoundary1;
    }
    if (newYBoundary2 < yBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= yBoundary2 || yScrollOffset + SCREEN_YSIZE <= newYBoundary2)
            --yBoundary2;
        else
            yBoundary2 = yScrollOffset + SCREEN_YSIZE;
    }
    if (newYBoundary2 > yBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= yBoundary2)
            ++yBoundary2;
        else
            yBoundary2 = newYBoundary2;
    }

    int xscrollA = xScrollA;
    int xscrollB = xScrollB;
    if (playerXPos <= SCREEN_CENTERX + xScrollA) {
        Player->screenXPos = cameraShakeX + playerXPos - xScrollA;
        xScrollOffset      = xscrollA - cameraShakeX;
    }
    else {
        xScrollOffset      = cameraShakeX + playerXPos - SCREEN_CENTERX;
        Player->screenXPos = SCREEN_CENTERX - cameraShakeX;
        if (playerXPos > xscrollB - SCREEN_CENTERX) {
            Player->screenXPos = cameraShakeX + SCREEN_CENTERX + playerXPos - (xscrollB - SCREEN_CENTERX);
            xScrollOffset      = xscrollB - SCREEN_XSIZE - cameraShakeX;
        }
    }

    int yscrollA   = yScrollA;
    int yscrollB   = yScrollB;
    int adjustY    = cameraAdjustY + playerYPos;
    int lookOffset = Player->lookPos + adjustY - (yScrollA + SCREEN_SCROLL_UP);
    if (Player->trackScroll == 1) {
        yScrollMove = 32;
    }
    else {
        if (yScrollMove == 32) {
            yScrollMove = 2 * ((SCREEN_SCROLL_UP - Player->screenYPos - Player->lookPos) >> 1);
            if (yScrollMove > 32)
                yScrollMove = 32;
            if (yScrollMove < -32)
                yScrollMove = -32;
        }
        if (yScrollMove > 0)
            yScrollMove -= 6;
        yScrollMove += yScrollMove < 0 ? 6 : 0;
    }

    int absLook = abs(lookOffset);
    if (absLook >= abs(yScrollMove) + 17) {
        if (lookOffset <= 0)
            yscrollA -= 16;
        else
            yscrollA += 16;
        yscrollB = yscrollA + SCREEN_YSIZE;
    }
    else if (yScrollMove == 32) {
        if (Player->lookPos + adjustY > yscrollA + yScrollMove + SCREEN_SCROLL_UP) {
            yscrollA = Player->lookPos + adjustY - (yScrollMove + SCREEN_SCROLL_UP);
            yscrollB = yscrollA + SCREEN_YSIZE;
        }
        if (Player->lookPos + adjustY < yscrollA + SCREEN_SCROLL_UP - yScrollMove) {
            yscrollA = Player->lookPos + adjustY - (SCREEN_SCROLL_UP - yScrollMove);
            yscrollB = yscrollA + SCREEN_YSIZE;
        }
    }
    else {
        yscrollA = Player->lookPos + adjustY + yScrollMove - SCREEN_SCROLL_UP;
        yscrollB = yscrollA + SCREEN_YSIZE;
    }
    if (yscrollA < yBoundary1) {
        yscrollA = yBoundary1;
        yscrollB = yBoundary1 + SCREEN_YSIZE;
    }
    if (yscrollB > yBoundary2) {
        yscrollB = yBoundary2;
        yscrollA = yBoundary2 - SCREEN_YSIZE;
    }
    yScrollA = yscrollA;
    yScrollB = yscrollB;
    if (Player->lookPos + adjustY <= yscrollA + SCREEN_SCROLL_UP) {
        Player->screenYPos = adjustY - yscrollA - cameraShakeY;
        yScrollOffset      = cameraShakeY + yscrollA;
    }
    else {
        yScrollOffset      = cameraShakeY + adjustY + Player->lookPos - SCREEN_SCROLL_UP;
        Player->screenYPos = SCREEN_SCROLL_UP - Player->lookPos - cameraShakeY;
        if (Player->lookPos + adjustY > yscrollB - SCREEN_SCROLL_DOWN) {
            Player->screenYPos = adjustY - (yscrollB - SCREEN_SCROLL_DOWN) + cameraShakeY + SCREEN_SCROLL_UP;
            yScrollOffset      = yscrollB - SCREEN_YSIZE - cameraShakeY;
        }
    }

    Player->screenYPos -= cameraAdjustY;
    if (cameraShakeX) {
        if (cameraShakeX <= 0) {
            cameraShakeX = -cameraShakeX--;
        }
        else {
            cameraShakeX = -cameraShakeX;
        }
    }
    if (!cameraShakeY)
        return;
    if (cameraShakeY <= 0) {
        cameraShakeY = -cameraShakeY--;
    }
    else {
        cameraShakeY = -cameraShakeY;
    }*/
}
void SetPlayerLockedScreenPosition(Entity *Player)
{
    /*int playerXPos = Player->XPos >> 16;
    int playerYPos = Player->YPos >> 16;
    int xscrollA   = xScrollA;
    int xscrollB   = xScrollB;
    if (playerXPos <= SCREEN_CENTERX + xScrollA) {
        Player->screenXPos = cameraShakeX + playerXPos - xScrollA;
        xScrollOffset      = xscrollA - cameraShakeX;
    }
    else {
        xScrollOffset      = cameraShakeX + playerXPos - SCREEN_CENTERX;
        Player->screenXPos = SCREEN_CENTERX - cameraShakeX;
        if (playerXPos > xscrollB - SCREEN_CENTERX) {
            Player->screenXPos = cameraShakeX + SCREEN_CENTERX + playerXPos - (xscrollB - SCREEN_CENTERX);
            xScrollOffset      = xscrollB - SCREEN_XSIZE - cameraShakeX;
        }
    }

    int yscrollA     = yScrollA;
    int yscrollB     = yScrollB;
    int adjustY      = cameraAdjustY + playerYPos;
    int adjustOffset = Player->lookPos + adjustY - (yScrollA + SCREEN_SCROLL_UP);
    if (Player->lookPos + adjustY <= yScrollA + SCREEN_SCROLL_UP) {
        Player->screenYPos = adjustY - yScrollA - cameraShakeY;
        yScrollOffset      = cameraShakeY + yscrollA;
    }
    else {
        yScrollOffset      = cameraShakeY + adjustY + Player->lookPos - SCREEN_SCROLL_UP;
        Player->screenYPos = SCREEN_SCROLL_UP - Player->lookPos - cameraShakeY;
        if (Player->lookPos + adjustY > yscrollB - SCREEN_SCROLL_DOWN) {
            Player->screenYPos = adjustY - (yscrollB - SCREEN_SCROLL_DOWN) + cameraShakeY + SCREEN_SCROLL_UP;
            yScrollOffset      = yscrollB - SCREEN_YSIZE - cameraShakeY;
        }
    }

    Player->screenYPos -= cameraAdjustY;
    if (cameraShakeX) {
        if (cameraShakeX <= 0) {
            cameraShakeX = -cameraShakeX--;
        }
        else {
            cameraShakeX = -cameraShakeX;
        }
    }
    if (!cameraShakeY)
        return;
    if (cameraShakeY <= 0) {
        cameraShakeY = -cameraShakeY--;
    }
    else {
        cameraShakeY = -cameraShakeY;
    }*/
}