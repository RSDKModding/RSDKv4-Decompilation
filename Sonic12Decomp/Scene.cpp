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

int curXBoundary1 = 0;
int newXBoundary1 = 0;
int curYBoundary1 = 0;
int newYBoundary1 = 0;
int curXBoundary2 = 0;
int curYBoundary2 = 0;
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
int frameCounter      = 0;
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
bool drawStageGFXHQ = false;

void InitFirstStage(void)
{
    xScrollOffset = 0;
    yScrollOffset = 0;
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    fadeMode = 0;
    ClearGraphicsData();
    ClearAnimationData();
    activePalette     = fullPalette[0];
    activePalette32   = fullPalette32[0];
    stageMode         = STAGEMODE_LOAD;
    Engine.gameMode   = ENGINE_MAINGAME;
    activeStageList   = Engine.startList;
    stageListPosition = Engine.startStage;
}

void InitStartingStage(int list, int stage, int player)
{
    xScrollOffset = 0;
    yScrollOffset = 0;
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    fadeMode      = 0;
    playerListPos = player;
    ClearGraphicsData();
    ClearAnimationData();
    ResetCurrentStageFolder();
    activeStageList   = list;
    activePalette     = fullPalette[0];
    activePalette32   = fullPalette32[0];
    stageMode         = STAGEMODE_LOAD;
    Engine.gameMode   = ENGINE_MAINGAME;
    stageListPosition = stage;
}

void ProcessStage(void)
{
    int updateMax = 0;
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
            Engine.frameCount = 0;
            stageMilliseconds = 0;
            stageSeconds      = 0;
            stageMinutes      = 0;
            stageMode         = STAGEMODE_NORMAL;

            setIdentityMatrix(&matTemp);
            setIdentityMatrix(&matWorld);
            setIdentityMatrix(&matView);

            ResetBackgroundSettings();
            LoadStageFiles();
            break;
        case STAGEMODE_NORMAL:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            if (pauseEnabled && keyPress.start) {
                // stageMode = STAGEMODE_STEPOVER;
                // PauseSound();
            }

            if (timeEnabled) {
                if (++frameCounter == Engine.refreshRate) {
                    frameCounter = 0;
                    if (++stageSeconds > 59) {
                        stageSeconds = 0;
                        if (++stageMinutes > 59)
                            stageMinutes = 0;
                    }
                }
                stageMilliseconds = 100 * frameCounter / Engine.refreshRate;
            }

            updateMax = 1;
            /*updateMax = Engine.renderFrameIndex;
            if (Engine.refreshRate >= Engine.targetRefreshRate) {
                updateMax = 0;
                if (Engine.frameCount % Engine.skipFrameIndex < Engine.renderFrameIndex)
                    updateMax = 1;
            }*/

            // Update
            for (int i = 0; i < updateMax; ++i) {
                ProcessObjects();
            }

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
        case STAGEMODE_PAUSED:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);

            if (pauseEnabled && keyPress.start) {
                // stageMode = STAGEMODE_PAUSED_STEPOVER;
                // stageMode = STAGEMODE_NORMAL;
                // ResumeSound();
            }

            updateMax = 1;
            /*updateMax = Engine.renderFrameIndex;
            if (Engine.refreshRate >= Engine.targetRefreshRate) {
                updateMax = 0;
                if (Engine.frameCount % Engine.skipFrameIndex < Engine.renderFrameIndex)
                    updateMax = 1;
            }*/

            // Update
            for (int i = 0; i < updateMax; ++i) {
                ProcessPausedObjects();
            }

            DrawObjectList(0);
            DrawObjectList(1);
            DrawObjectList(2);
            DrawObjectList(3);
            DrawObjectList(4);
            DrawObjectList(5);
            DrawObjectList(6);
            break;
        case STAGEMODE_FROZEN:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            if (pauseEnabled && keyPress.start) {
                stageMode = STAGEMODE_STEPOVER;
                PauseSound();
            }

            updateMax = 1;
            /*updateMax = Engine.renderFrameIndex;
            if (Engine.refreshRate >= Engine.targetRefreshRate) {
                updateMax = 0;
                if (Engine.frameCount % Engine.skipFrameIndex < Engine.renderFrameIndex)
                    updateMax = 1;
            }*/

            // Update
            for (int i = 0; i < updateMax; ++i) {
                ProcessFrozenObjects();
            }

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
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            if (pauseEnabled && keyPress.start) {
                stageMode = STAGEMODE_2P_PAUSED;
                PauseSound();
            }

            if (timeEnabled) {
                if (++frameCounter == Engine.refreshRate) {
                    frameCounter = 0;
                    if (++stageSeconds > 59) {
                        stageSeconds = 0;
                        if (++stageMinutes > 59)
                            stageMinutes = 0;
                    }
                }
                stageMilliseconds = 100 * frameCounter / Engine.refreshRate;
            }

            updateMax = 1;
            /*updateMax = Engine.renderFrameIndex;
            if (Engine.refreshRate >= Engine.targetRefreshRate) {
                updateMax = 0;
                if (Engine.frameCount % Engine.skipFrameIndex < Engine.renderFrameIndex)
                    updateMax = 1;
            }*/

            // Update
            for (int i = 0; i < updateMax; ++i) {
                Process2PObjects();
            }

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
        case STAGEMODE_STEPOVER:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);

            if (keyPress.C) {
                keyPress.C = false;

                if (timeEnabled) {
                    if (++frameCounter == Engine.refreshRate) {
                        frameCounter = 0;
                        if (++stageSeconds > 59) {
                            stageSeconds = 0;
                            if (++stageMinutes > 59)
                                stageMinutes = 0;
                        }
                    }
                    stageMilliseconds = 100 * frameCounter / Engine.refreshRate;
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
        case STAGEMODE_PAUSED_STEPOVER:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);

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
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            if (pauseEnabled && keyPress.start) {
                stageMode = STAGEMODE_FROZEN;
                PauseSound();
            }

            updateMax = 1;
            /*updateMax = Engine.renderFrameIndex;
            if (Engine.refreshRate >= Engine.targetRefreshRate) {
                updateMax = 0;
                if (Engine.frameCount % Engine.skipFrameIndex < Engine.renderFrameIndex)
                    updateMax = 1;
            }*/

            // Update
            for (int i = 0; i < updateMax; ++i) {
                ProcessFrozenObjects();
            }

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
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode--;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown);
            CheckKeyPress(&keyPress);
            if (pauseEnabled && keyPress.start) {
                stageMode = STAGEMODE_2P;
                PauseSound();
            }

            if (timeEnabled) {
                if (++frameCounter == Engine.refreshRate) {
                    frameCounter = 0;
                    if (++stageSeconds > 59) {
                        stageSeconds = 0;
                        if (++stageMinutes > 59)
                            stageMinutes = 0;
                    }
                }
                stageMilliseconds = 100 * frameCounter / Engine.refreshRate;
            }

            updateMax = 1;
            /*updateMax = Engine.renderFrameIndex;
            if (Engine.refreshRate >= Engine.targetRefreshRate) {
                updateMax = 0;
                if (Engine.frameCount % Engine.skipFrameIndex < Engine.renderFrameIndex)
                    updateMax = 1;
            }*/

            // Update
            for (int i = 0; i < updateMax; ++i) {
                Process2PObjects();
            }

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
    Engine.frameCount++;
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
    byte fileBuffer = 0;
    byte fileBuffer2 = 0;
    int scriptID    = 1;
    char strBuffer[0x100];

    if (!CheckCurrentStageFolder(stageListPosition)) {
        printLog("Loading Scene %s - %s", stageListNames[activeStageList], stageList[activeStageList][stageListPosition].name);
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
                SetPaletteEntry(-1, c, buf[0], buf[1], buf[2]);
            }

            byte globalObjectCount = 0;
            FileRead(&globalObjectCount, 1);
            for (byte i = 0; i < globalObjectCount; ++i) {
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
                for (byte i = 0; i < globalObjectCount; ++i) {
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
                SetPaletteEntry(-1, i, clr[0], clr[1], clr[2]);
            }

            FileRead(&fileBuffer, 1);
            stageSFXCount = fileBuffer;
            for (byte i = 0; i < stageSFXCount; ++i) {
                FileRead(&fileBuffer2, 1);
                FileRead(strBuffer, fileBuffer2);
                strBuffer[fileBuffer2] = 0;

                SetSfxName(strBuffer, i + globalSFXCount);
            }
            for (byte i = 0; i < stageSFXCount; ++i) {
                FileRead(&fileBuffer2, 1);
                FileRead(strBuffer, fileBuffer2);
                strBuffer[fileBuffer2] = 0;
                GetFileInfo(&infoStore);
                CloseFile();
                LoadSfx(strBuffer, globalSFXCount + i);
                SetFileInfo(&infoStore);
            }

            byte stageObjectCount = 0;
            FileRead(&stageObjectCount, 1);
            for (byte i = 0; i < stageObjectCount; ++i) {
                FileRead(&fileBuffer2, 1);
                FileRead(strBuffer, fileBuffer2);
                strBuffer[fileBuffer2] = 0;
                SetObjectTypeName(strBuffer, scriptID + i);
            }

            if (Engine.usingBytecode) {
                for (byte i = 0; i < stageObjectCount; ++i) {
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
                for (byte i = 0; i < stageObjectCount; ++i) {
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
    else {
        printLog("Reloading Scene %s - %s", stageListNames[activeStageList], stageList[activeStageList][stageListPosition].name);
    }
    LoadStageChunks();
    for (int i = 0; i < TRACK_COUNT; ++i) SetMusicTrack("", i, false, 0);

    memset(objectEntityList, 0, ENTITY_COUNT * sizeof(Entity));
    for (int i = 0; i < ENTITY_COUNT; ++i) {
        objectEntityList[i].drawOrder          = 3;
        objectEntityList[i].scale              = 512;
        objectEntityList[i].objectInteractions = true;
        objectEntityList[i].visible            = true;
        objectEntityList[i].tileCollisions     = true;
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
    for (int a = 0; a < 4; ++a) activeTileLayers[a] = 9; // disables missing scenes from rendering

    if (LoadActFile(".bin", stageListPosition, &info)) {
        byte fileBuffer[4];

        byte length = 0;
        FileRead(&length, 1);
        titleCardWord2 = (byte)length;
        for (int i = 0; i < length; ++i) {
            FileRead(&titleCardText[i], 1);
            if (titleCardText[i] == '-')
                titleCardWord2 = (byte)(i + 1);
        }
        titleCardText[length] = '\0';

        // READ TILELAYER
        FileRead(activeTileLayers, 4);
        FileRead(&tLayerMidPoint, 1);

        FileRead(&stageLayouts[0].width, 1);
        FileRead(fileBuffer, 1); // Unused

        FileRead(&stageLayouts[0].height, 1);
        FileRead(fileBuffer, 1); // Unused
        curXBoundary1 = 0;
        newXBoundary1 = 0;
        curYBoundary1 = 0;
        newYBoundary1 = 0;
        curXBoundary2 = stageLayouts[0].width << 7;
        curYBoundary2 = stageLayouts[0].height << 7;
        waterLevel    = curYBoundary2 + 128;
        newXBoundary2 = stageLayouts[0].width << 7;
        newYBoundary2 = stageLayouts[0].height << 7;

        memset(stageLayouts[0].tiles, 0, TILELAYER_CHUNK_MAX * sizeof(ushort));
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
        FileRead(&fileBuffer[0], 2);
        int objectCount = fileBuffer[0] + (fileBuffer[1] << 8);

        Entity *object = &objectEntityList[32];
        for (int i = 0; i < objectCount; ++i) {
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
    CloseFile();
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
        byte fileBuffer = 0;
        byte layerCount = 0;
        FileRead(&layerCount, 1);
        FileRead(&hParallax.entryCount, 1);
        for (byte i = 0; i < hParallax.entryCount; ++i) {
            FileRead(&fileBuffer, 1);
            hParallax.parallaxFactor[i] = fileBuffer;
            FileRead(&fileBuffer, 1);
            hParallax.parallaxFactor[i] += fileBuffer << 8;

            FileRead(&fileBuffer, 1);
            hParallax.scrollSpeed[i] = fileBuffer << 10;

            hParallax.scrollPos[i] = 0;

            FileRead(&hParallax.deform[i], 1);
        }

        FileRead(&vParallax.entryCount, 1);
        for (byte i = 0; i < vParallax.entryCount; ++i) {
            FileRead(&fileBuffer, 1);
            vParallax.parallaxFactor[i] = fileBuffer;
            FileRead(&fileBuffer, 1);
            vParallax.parallaxFactor[i] += fileBuffer << 8;

            FileRead(&fileBuffer, 1);
            vParallax.scrollSpeed[i] = fileBuffer << 10;

            vParallax.scrollPos[i] = 0;

            FileRead(&vParallax.deform[i], 1);
        }

        for (byte i = 1; i < layerCount + 1; ++i) {
            FileRead(&fileBuffer, 1);
            stageLayouts[i].width = fileBuffer;
            FileRead(&fileBuffer, 1); // Unused (???)
            FileRead(&fileBuffer, 1);
            stageLayouts[i].height = fileBuffer;
            FileRead(&fileBuffer, 1); // Unused (???)
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

        byte fileBuffer = 0;
        int tileIndex  = 0;
        for (int t = 0; t < TILE_COUNT; ++t) {
            for (int p = 0; p < CPATH_COUNT; ++p) {
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

                    // LWall rotations
                    for (int c = 0; c < TILE_SIZE; ++c) {
                        int h = 0;
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

                    // RWall rotations
                    for (int c = 0; c < TILE_SIZE; ++c) {
                        int h = TILE_SIZE - 1;
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
        byte fileBuffer = 0;

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
            SetPaletteEntry(-1, c, clr[0], clr[1], clr[2]);
        }

        FileRead(&fileBuffer, 1);
        while (fileBuffer != ',') FileRead(&fileBuffer, 1); // gif image start identifier

        ushort fileBuffer2 = 0;
        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer2, 2);
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
        for (int i = 0; i < TILESET_SIZE; ++i) {
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
        int *d = &deformPtr[deformOffset];
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

void SetPlayerScreenPosition(Entity *target)
{
    int targetX = target->XPos >> 16;
    int targetY = cameraAdjustY + (target->YPos >> 16);
    if (newYBoundary1 > curYBoundary1) {
        if (newYBoundary1 >= yScrollOffset)
            curYBoundary1 = yScrollOffset;
        else
            curYBoundary1 = newYBoundary1;
    }
    if (newYBoundary1 < curYBoundary1) {
        if (curYBoundary1 >= yScrollOffset)
            --curYBoundary1;
        else
            curYBoundary1 = newYBoundary1;
    }
    if (newYBoundary2 < curYBoundary2) {
        if (curYBoundary2 <= yScrollOffset + SCREEN_YSIZE || newYBoundary2 >= yScrollOffset + SCREEN_YSIZE)
            --curYBoundary2;
        else
            curYBoundary2 = yScrollOffset + SCREEN_YSIZE;
    }
    if (newYBoundary2 > curYBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= curYBoundary2) {
            ++curYBoundary2;
            if (target->YVelocity > 0) {
                int buf = curYBoundary2 + (target->YVelocity >> 16);
                if (newYBoundary2 < buf) {
                    curYBoundary2 = newYBoundary2;
                }
                else {
                    curYBoundary2 += target->YVelocity >> 16;
                }
            }
        }
        else
            curYBoundary2 = newYBoundary2;
    }
    if (newXBoundary1 > curXBoundary1) {
        if (xScrollOffset <= newXBoundary1)
            curXBoundary1 = xScrollOffset;
        else
            curXBoundary1 = newXBoundary1;
    }
    if (newXBoundary1 < curXBoundary1) {
        if (xScrollOffset <= curXBoundary1) {
            --curXBoundary1;
            if (target->XVelocity < 0) {
                curXBoundary1 += target->XVelocity >> 16;
                if (curXBoundary1 < newXBoundary1)
                    curXBoundary1 = newXBoundary1;
            }
        }
        else {
            curXBoundary1 = newXBoundary1;
        }
    }
    if (newXBoundary2 < curXBoundary2) {
        if (newXBoundary2 > SCREEN_XSIZE + xScrollOffset)
            curXBoundary2 = newXBoundary2;
        else
            curXBoundary2 = SCREEN_XSIZE + xScrollOffset;
    }
    if (newXBoundary2 > curXBoundary2) {
        if (SCREEN_XSIZE + xScrollOffset >= curXBoundary2) {
            ++curXBoundary2;
            if (target->XVelocity > 0) {
                curXBoundary2 += target->XVelocity >> 16;
                if (curXBoundary2 > newXBoundary2)
                    curXBoundary2 = newXBoundary2;
            }
        }
        else {
            curXBoundary2 = newXBoundary2;
        }
    }

    int xPosDif = targetX - cameraXPos;
    if (targetX > cameraXPos) {
        xPosDif -= 8;
        if (xPosDif >= 0) {
            if (xPosDif >= 17)
                xPosDif = 16;
        }
        else {
            xPosDif = 0;
        }
    }
    else {
        xPosDif += 8;
        if (xPosDif > 0) {
            xPosDif = 0;
        }
        else if (xPosDif <= -17) {
            xPosDif = -16;
        }
    }

    int centeredXBound1 = cameraXPos + xPosDif;
    cameraXPos          = centeredXBound1;
    if (centeredXBound1 < SCREEN_CENTERX + curXBoundary1) {
        cameraXPos      = SCREEN_CENTERX + curXBoundary1;
        centeredXBound1 = SCREEN_CENTERX + curXBoundary1;
    }

    int centeredXBound2 = curXBoundary2 - SCREEN_CENTERX;
    if (centeredXBound2 < centeredXBound1) {
        cameraXPos      = centeredXBound2;
        centeredXBound1 = centeredXBound2;
    }

    int yPosDif = 0;
    if (target->trackScroll) {
        if (targetY <= cameraYPos) {
            yPosDif = (targetY - cameraYPos) + 32;
            if (yPosDif <= 0) {
                if (yPosDif <= -17)
                    yPosDif = -16;
            }
            else
                yPosDif = 0;
        }
        else {
            yPosDif = (targetY - cameraYPos) - 32;
            if (yPosDif >= 0) {
                if (yPosDif >= 17)
                    yPosDif = 16;
            }
            else
                yPosDif = 0;
        }
        cameraLockedY = false;
    }
    else if (targetY <= cameraYPos) {
        yPosDif = targetY - cameraYPos;
        if (targetY - cameraYPos <= 0) {
            if (yPosDif >= -32 && abs(target->YVelocity) <= 0x60000) {
                if (yPosDif < -6) {
                    yPosDif = -6;
                }
            }
            else if (yPosDif < -16) {
                yPosDif = -16;
            }
        }
        else {
            yPosDif       = 0;
            cameraLockedY = true;
        }
    }
    else {
        yPosDif = targetY - cameraYPos;
        if (targetY - cameraYPos < 0) {
            yPosDif       = 0;
            cameraLockedY = true;
        }
        else if (yPosDif > 32 || abs(target->YVelocity) > 0x60000) {
            if (yPosDif > 16) {
                yPosDif = 16;
            }
            else {
                cameraLockedY = true;
            }
        }
        else {
            if (yPosDif <= 6) {
                cameraLockedY = true;
            }
            else {
                yPosDif = 6;
            }
        }
    }

    int newCamY = cameraYPos + yPosDif;
    if (newCamY <= curYBoundary1 + (SCREEN_SCROLL_UP - 1))
        newCamY = curYBoundary1 + SCREEN_SCROLL_UP;
    cameraYPos = newCamY;
    if (curYBoundary2 - (SCREEN_SCROLL_DOWN - 1) <= newCamY) {
        cameraYPos = curYBoundary2 - SCREEN_SCROLL_DOWN;
    }

    xScrollOffset = cameraShakeX + centeredXBound1 - SCREEN_CENTERX;

    int pos = cameraYPos + target->lookPos - SCREEN_SCROLL_UP;
    if (pos < curYBoundary1) {
        yScrollOffset = curYBoundary1;
    }
    else {
        yScrollOffset = cameraYPos + target->lookPos - SCREEN_SCROLL_UP;
    }

    int y = curYBoundary2 - SCREEN_YSIZE;
    if (curYBoundary2 - (SCREEN_YSIZE - 1) > yScrollOffset)
        y = yScrollOffset;
    yScrollOffset = cameraShakeY + y;

    if (cameraShakeX) {
        if (cameraShakeX <= 0) {
            cameraShakeX = ~cameraShakeX;
        }
        else {
            cameraShakeX = -cameraShakeX;
        }
    }

    if (cameraShakeY) {
        if (cameraShakeY <= 0) {
            cameraShakeY = ~cameraShakeY;
        }
        else {
            cameraShakeY = -cameraShakeY;
        }
    }
}
void SetPlayerScreenPositionCDStyle(Entity *target)
{
    int targetX = target->XPos >> 16;
    int targetY = cameraAdjustY + (target->YPos >> 16);
    if (newYBoundary1 > curYBoundary1) {
        if (newYBoundary1 >= yScrollOffset)
            curYBoundary1 = yScrollOffset;
        else
            curYBoundary1 = newYBoundary1;
    }
    if (newYBoundary1 < curYBoundary1) {
        if (curYBoundary1 >= yScrollOffset)
            --curYBoundary1;
        else
            curYBoundary1 = newYBoundary1;
    }
    if (newYBoundary2 < curYBoundary2) {
        if (curYBoundary2 <= yScrollOffset + SCREEN_YSIZE || newYBoundary2 >= yScrollOffset + SCREEN_YSIZE)
            --curYBoundary2;
        else
            curYBoundary2 = yScrollOffset + SCREEN_YSIZE;
    }
    if (newYBoundary2 > curYBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= curYBoundary2) {
            ++curYBoundary2;
            if (target->YVelocity > 0) {
                int buf = curYBoundary2 + (target->YVelocity >> 16);
                if (newYBoundary2 < buf) {
                    curYBoundary2 = newYBoundary2;
                }
                else {
                    curYBoundary2 += target->YVelocity >> 16;
                }
            }
        }
        else
            curYBoundary2 = newYBoundary2;
    }
    if (newXBoundary1 > curXBoundary1) {
        if (xScrollOffset <= newXBoundary1)
            curXBoundary1 = xScrollOffset;
        else
            curXBoundary1 = newXBoundary1;
    }
    if (newXBoundary1 < curXBoundary1) {
        if (xScrollOffset <= curXBoundary1) {
            --curXBoundary1;
            if (target->XVelocity < 0) {
                curXBoundary1 += target->XVelocity >> 16;
                if (curXBoundary1 < newXBoundary1)
                    curXBoundary1 = newXBoundary1;
            }
        }
        else {
            curXBoundary1 = newXBoundary1;
        }
    }
    if (newXBoundary2 < curXBoundary2) {
        if (newXBoundary2 > SCREEN_XSIZE + xScrollOffset)
            curXBoundary2 = newXBoundary2;
        else
            curXBoundary2 = SCREEN_XSIZE + xScrollOffset;
    }
    if (newXBoundary2 > curXBoundary2) {
        if (SCREEN_XSIZE + xScrollOffset >= curXBoundary2) {
            ++curXBoundary2;
            if (target->XVelocity > 0) {
                curXBoundary2 += target->XVelocity >> 16;
                if (curXBoundary2 > newXBoundary2)
                    curXBoundary2 = newXBoundary2;
            }
        }
        else {
            curXBoundary2 = newXBoundary2;
        }
    }

    if (!target->gravity) {
        if (target->direction) {
            if (cameraStyle == 3 || target->speed < -0x5F5C2) {
                cameraShift = 2;
                if (target->camOffsetX <= 63) {
                    target->camOffsetX += 2;
                }
            }
            else {
                cameraShift = 0;
                if (target->camOffsetX < 0) {
                    target->camOffsetX += 2;
                }

                if (target->camOffsetX > 0) {
                    target->camOffsetX -= 2;
                }
            }
        }
        else {
            if (cameraStyle == 2 || target->speed > 0x5F5C2) {
                cameraShift = 1;
                if (target->camOffsetX >= -63) {
                    target->camOffsetX -= 2;
                }
            }
            else {
                cameraShift = 0;
                if (target->camOffsetX < 0) {
                    target->camOffsetX += 2;
                }

                if (target->camOffsetX > 0) {
                    target->camOffsetX -= 2;
                }
            }
        }
    }
    else {
        if (cameraShift == 1) {
            if (target->camOffsetX >= -63) {
                target->camOffsetX -= 2;
            }
        }
        else if (cameraShift < 1) {
            if (target->camOffsetX < 0) {
                target->camOffsetX += 2;
            }
            if (target->camOffsetX > 0) {
                target->camOffsetX -= 2;
            }
        }
        else if (cameraShift == 2) {
            if (target->camOffsetX <= 63) {
                target->camOffsetX += 2;
            }
        }
    }
    cameraXPos = targetX - target->camOffsetX;

    if (!target->trackScroll) {
        if (cameraLockedY) {
            cameraYPos = targetY;
            if (cameraYPos < curYBoundary1 + SCREEN_SCROLL_UP) {
                cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
            }
        }
        else if (targetY > cameraYPos) {
            int dif = targetY - cameraYPos;
            if (targetY - cameraYPos < 0) {
                cameraLockedY = true;
                if (cameraYPos < curYBoundary1 + SCREEN_SCROLL_UP) {
                    cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                }
            }
            else {
                if (dif > 32 || abs(target->YVelocity) > 0x60000) {
                    if (dif > 16) {
                        dif = 16;
                        if (cameraYPos + dif >= curYBoundary1 + SCREEN_SCROLL_UP) {
                            cameraYPos += dif;
                        }
                        else {
                            cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                        }
                    }
                    else {
                        cameraLockedY = true;
                        if (cameraYPos + dif >= curYBoundary1 + SCREEN_SCROLL_UP) {
                            cameraYPos += dif;
                        }
                        else {
                            cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                        }
                    }
                }
                else if (dif > 6) {
                    dif = 6;
                    if (cameraYPos + dif >= curYBoundary1 + SCREEN_SCROLL_UP) {
                        cameraYPos += dif;
                    }
                    else {
                        cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                    }
                }
                else {
                    cameraLockedY = true;
                    if (cameraYPos + dif >= curYBoundary1 + SCREEN_SCROLL_UP) {
                        cameraYPos += dif;
                    }
                    else {
                        cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                    }
                }
            }
        }
        else {
            int dif = targetY - cameraYPos;
            if (targetY - cameraYPos <= 0) {
                if (dif < -32 || abs(target->YVelocity) > 0x60000) {
                    if (dif < -16) {
                        dif = -16;
                        if (cameraYPos + dif >= curYBoundary1 + SCREEN_SCROLL_UP) {
                            cameraYPos += dif;
                        }
                        else {
                            cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                        }
                    }
                    else {
                        cameraLockedY = true;
                        if (cameraYPos + dif >= curYBoundary1 + SCREEN_SCROLL_UP) {
                            cameraYPos += dif;
                        }
                        else {
                            cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                        }
                    }
                }
                else if (dif < -6) {
                    dif = -6;
                    if (cameraYPos + dif >= curYBoundary1 + SCREEN_SCROLL_UP) {
                        cameraYPos += dif;
                    }
                    else {
                        cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                    }
                }
            }
            else {
                dif = 0;
                if (abs(target->YVelocity) > 0x60000) {
                    cameraLockedY = true;
                    if (cameraYPos + dif >= curYBoundary1 + SCREEN_SCROLL_UP) {
                        cameraYPos += dif;
                    }
                    else {
                        cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                    }
                }
                else {
                    cameraLockedY = true;
                    if (cameraYPos + dif >= curYBoundary1 + SCREEN_SCROLL_UP) {
                        cameraYPos += dif;
                    }
                    else {
                        cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                    }
                }
            }
        }
    }
    else {
        int dif  = targetY - cameraYPos;
        int difY = 0;
        if (targetY > cameraYPos) {
            difY = dif - 32;
            if (difY >= 0) {
                if (difY >= 17)
                    difY = 16;

                cameraLockedY = false;
                if (cameraYPos + difY >= curYBoundary1 + SCREEN_SCROLL_UP) {
                    cameraYPos += difY;
                }
                else {
                    cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                }
            }
            else {
                cameraLockedY = false;
                if (cameraYPos < curYBoundary1 + SCREEN_SCROLL_UP) {
                    cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                }
            }
        }
        else {
            difY = dif + 32;
            if (difY > 0) {
                difY = 0;

                cameraLockedY = false;
                if (cameraYPos < curYBoundary1 + SCREEN_SCROLL_UP) {
                    cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                }
            }
            else if (difY <= -17) {
                difY = -16;

                cameraLockedY = false;
                if (cameraYPos + difY >= curYBoundary1 + SCREEN_SCROLL_UP) {
                    cameraYPos += difY;
                }
                else {
                    cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                }
            }
            else {
                cameraLockedY = false;
                if (cameraYPos + difY >= curYBoundary1 + SCREEN_SCROLL_UP) {
                    cameraYPos += difY;
                }
                else {
                    cameraYPos = curYBoundary1 + SCREEN_SCROLL_UP;
                }
            }
        }
    }

    if (cameraYPos >= curYBoundary2 - SCREEN_SCROLL_DOWN - 1) {
        cameraYPos = curYBoundary2 - SCREEN_SCROLL_DOWN;
    }

    xScrollOffset = cameraXPos - SCREEN_CENTERX;
    yScrollOffset = target->lookPos + cameraYPos - SCREEN_SCROLL_UP;

    int x = curXBoundary1;
    if (x <= xScrollOffset)
        x = xScrollOffset;
    else
        xScrollOffset = x;

    if (x > curXBoundary2 - SCREEN_XSIZE) {
        x             = curXBoundary2 - SCREEN_XSIZE;
        xScrollOffset = curXBoundary2 - SCREEN_XSIZE;
    }

    int y = curYBoundary1;
    if (yScrollOffset >= y)
        y = yScrollOffset;
    else
        yScrollOffset = y;

    if (curYBoundary2 - SCREEN_YSIZE - 1 <= y)
        y = curYBoundary2 - SCREEN_YSIZE;

    xScrollOffset = cameraShakeX + x;
    yScrollOffset = cameraShakeY + y;

    if (cameraShakeX) {
        if (cameraShakeX <= 0) {
            cameraShakeX = ~cameraShakeX;
        }
        else {
            cameraShakeX = -cameraShakeX;
        }
    }

    if (cameraShakeY) {
        if (cameraShakeY <= 0) {
            cameraShakeY = ~cameraShakeY;
        }
        else {
            cameraShakeY = -cameraShakeY;
        }
    }
}
void SetPlayerHLockedScreenPosition(Entity *target)
{
    int targetY = cameraAdjustY + (target->YPos >> 16);

    if (newYBoundary1 <= curYBoundary1) {
        if (curYBoundary1 > yScrollOffset)
            --curYBoundary1;
        else
            curYBoundary1 = newYBoundary1;
    }
    else {
        if (newYBoundary1 >= yScrollOffset)
            curYBoundary1 = yScrollOffset;
        else
            curYBoundary1 = newYBoundary1;
    }

    if (newYBoundary2 < curYBoundary2) {
        if (curYBoundary2 <= yScrollOffset + SCREEN_YSIZE || newYBoundary2 >= yScrollOffset + SCREEN_YSIZE)
            --curYBoundary2;
        else
            curYBoundary2 = yScrollOffset + SCREEN_YSIZE;
    }
    if (newYBoundary2 > curYBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= curYBoundary2) {
            ++curYBoundary2;
            if (target->YVelocity > 0) {
                if (newYBoundary2 < curYBoundary2 + (target->YVelocity >> 16)) {
                    curYBoundary2 = newYBoundary2;
                }
                else {
                    curYBoundary2 += target->YVelocity >> 16;
                }
            }
        }
        else
            curYBoundary2 = newYBoundary2;
    }

    if (newXBoundary1 > curXBoundary1) {
        if (xScrollOffset <= newXBoundary1)
            curXBoundary1 = xScrollOffset;
        else
            curXBoundary1 = newXBoundary1;
    }
    if (newXBoundary1 < curXBoundary1) {
        if (xScrollOffset <= curXBoundary1) {
            --curXBoundary1;
            if (target->XVelocity < 0) {
                curXBoundary1 += target->XVelocity >> 16;
                if (curXBoundary1 < newXBoundary1)
                    curXBoundary1 = newXBoundary1;
            }
        }
        else {
            curXBoundary1 = newXBoundary1;
        }
    }
    if (newXBoundary2 < curXBoundary2) {
        if (newXBoundary2 > SCREEN_XSIZE + xScrollOffset)
            curXBoundary2 = newXBoundary2;
        else
            curXBoundary2 = SCREEN_XSIZE + xScrollOffset;
    }
    if (newXBoundary2 > curXBoundary2) {
        if (SCREEN_XSIZE + xScrollOffset >= curXBoundary2) {
            ++curXBoundary2;
            if (target->XVelocity > 0) {
                curXBoundary2 += target->XVelocity >> 16;
                if (curXBoundary2 > newXBoundary2)
                    curXBoundary2 = newXBoundary2;
            }
        }
        else {
            curXBoundary2 = newXBoundary2;
        }
    }

    int camScroll = 0;
    if (target->trackScroll) {
        if (targetY <= cameraYPos) {
            camScroll = targetY - cameraYPos + 32;
            if (camScroll <= 0) {
                if (camScroll <= -17)
                    camScroll = -16;
            }
            else
                camScroll = 0;
        }
        else {
            camScroll = targetY - cameraYPos - 32;
            if (camScroll >= 0) {
                if (camScroll >= 17)
                    camScroll = 16;
            }
            else
                camScroll = 0;
        }
        cameraLockedY = false;
    }
    else if (cameraLockedY) {
        camScroll = 0;
    }
    else if (targetY > cameraYPos) {
        camScroll = targetY - cameraYPos;
        if (camScroll >= 0) {
            if (camScroll > 32 || abs(target->YVelocity) > 0x60000) {
                if (camScroll > 16) {
                    camScroll = 16;
                }
                else {
                    cameraLockedY = true;
                }
            }
            else if (camScroll > 6) {
                camScroll = 6;
            }
            else {
                cameraLockedY = true;
            }
        }
        else {
            camScroll     = 0;
            cameraLockedY = true;
        }
    }
    else {
        camScroll = targetY - cameraYPos;
        if (camScroll > 0) {
            camScroll     = 0;
            cameraLockedY = true;
        }
        else if (camScroll < -32 || abs(target->YVelocity) > 0x60000) {
            if (camScroll < -16) {
                camScroll = -16;
            }
            else {
                cameraLockedY = true;
            }
        }
        else {
            if (camScroll >= -6)
                cameraLockedY = true;
            else
                camScroll = -6;
        }
    }

    int newCamY = cameraYPos + camScroll;
    if (newCamY <= curYBoundary1 + (SCREEN_SCROLL_UP - 1))
        newCamY = curYBoundary1 + SCREEN_SCROLL_UP;
    cameraYPos = newCamY;
    if (curYBoundary2 - (SCREEN_SCROLL_DOWN - 1) <= newCamY) {
        newCamY    = curYBoundary2 - SCREEN_SCROLL_DOWN;
        cameraYPos = curYBoundary2 - SCREEN_SCROLL_DOWN;
    }

    xScrollOffset = cameraShakeX + cameraXPos - SCREEN_CENTERX;

    int pos = newCamY + target->lookPos - SCREEN_SCROLL_UP;
    if (pos < curYBoundary1) {
        yScrollOffset = curYBoundary1;
    }
    else {
        yScrollOffset = newCamY + target->lookPos - SCREEN_SCROLL_UP;
    }
    int y1 = curYBoundary2 - (SCREEN_YSIZE - 1);
    int y2 = curYBoundary2 - SCREEN_YSIZE;
    if (y1 > yScrollOffset)
        y2 = yScrollOffset;
    yScrollOffset = cameraShakeY + y2;

    if (cameraShakeX) {
        if (cameraShakeX <= 0) {
            cameraShakeX = ~cameraShakeX;
        }
        else {
            cameraShakeX = -cameraShakeX;
        }
    }

    if (cameraShakeY) {
        if (cameraShakeY <= 0) {
            cameraShakeY = ~cameraShakeY;
        }
        else {
            cameraShakeY = -cameraShakeY;
        }
    }
}
void SetPlayerLockedScreenPosition(Entity *target)
{
    if (newYBoundary1 > curYBoundary1) {
        if (yScrollOffset <= newYBoundary1)
            curYBoundary1 = yScrollOffset;
        else
            curYBoundary1 = newYBoundary1;
    }
    if (newYBoundary1 < curYBoundary1) {
        if (yScrollOffset <= curYBoundary1)
            --curYBoundary1;
        else
            curYBoundary1 = newYBoundary1;
    }
    if (newYBoundary2 < curYBoundary2) {
        if (curYBoundary2 <= yScrollOffset + SCREEN_YSIZE || newYBoundary2 >= yScrollOffset + SCREEN_YSIZE)
            --curYBoundary2;
        else
            curYBoundary2 = yScrollOffset + SCREEN_YSIZE;
    }
    if (newYBoundary2 > curYBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= curYBoundary2)
            ++curYBoundary2;
        else
            curYBoundary2 = newYBoundary2;
    }
    if (newXBoundary1 > curXBoundary1) {
        if (xScrollOffset <= newXBoundary1)
            curXBoundary1 = xScrollOffset;
        else
            curXBoundary1 = newXBoundary1;
    }
    if (newXBoundary1 < curXBoundary1) {
        if (xScrollOffset <= curXBoundary1) {
            --curXBoundary1;
            if (target->XVelocity < 0) {
                curXBoundary1 += target->XVelocity >> 16;
                if (curXBoundary1 < newXBoundary1)
                    curXBoundary1 = newXBoundary1;
            }
        }
        else {
            curXBoundary1 = newXBoundary1;
        }
    }
    if (newXBoundary2 < curXBoundary2) {
        if (newXBoundary2 > SCREEN_XSIZE + xScrollOffset)
            curXBoundary2 = newXBoundary2;
        else
            curXBoundary2 = SCREEN_XSIZE + xScrollOffset;
    }
    if (newXBoundary2 > curXBoundary2) {
        if (SCREEN_XSIZE + xScrollOffset >= curXBoundary2) {
            ++curXBoundary2;
            if (target->XVelocity > 0) {
                curXBoundary2 += target->XVelocity >> 16;
                if (curXBoundary2 > newXBoundary2)
                    curXBoundary2 = newXBoundary2;
            }
        }
        else {
            curXBoundary2 = newXBoundary2;
        }
    }

    if (cameraShakeX) {
        if (cameraShakeX <= 0) {
            cameraShakeX = ~cameraShakeX;
        }
        else {
            cameraShakeX = -cameraShakeX;
        }
    }

    if (cameraShakeY) {
        if (cameraShakeY <= 0) {
            cameraShakeY = ~cameraShakeY;
        }
        else {
            cameraShakeY = -cameraShakeY;
        }
    }
}
// Completely unused in both Sonic 1 & Sonic 2
void SetPlayerScreenPositionFixed(Entity *target)
{
    int targetX = target->XPos >> 16;
    int targetY = cameraAdjustY + (target->YPos >> 16);
    if (newYBoundary1 > curYBoundary1) {
        if (newYBoundary1 >= yScrollOffset)
            curYBoundary1 = yScrollOffset;
        else
            curYBoundary1 = newYBoundary1;
    }
    if (newYBoundary1 < curYBoundary1) {
        if (curYBoundary1 >= yScrollOffset)
            --curYBoundary1;
        else
            curYBoundary1 = newYBoundary1;
    }
    if (newYBoundary2 < curYBoundary2) {
        if (curYBoundary2 <= yScrollOffset + SCREEN_YSIZE || newYBoundary2 >= yScrollOffset + SCREEN_YSIZE)
            --curYBoundary2;
        else
            curYBoundary2 = yScrollOffset + SCREEN_YSIZE;
    }
    if (newYBoundary2 > curYBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= curYBoundary2) {
            ++curYBoundary2;
            if (target->YVelocity > 0) {
                int buf = curYBoundary2 + (target->YVelocity >> 16);
                if (newYBoundary2 < buf) {
                    curYBoundary2 = newYBoundary2;
                }
                else {
                    curYBoundary2 += target->YVelocity >> 16;
                }
            }
        }
        else
            curYBoundary2 = newYBoundary2;
    }
    if (newXBoundary1 > curXBoundary1) {
        if (xScrollOffset <= newXBoundary1)
            curXBoundary1 = xScrollOffset;
        else
            curXBoundary1 = newXBoundary1;
    }
    if (newXBoundary1 < curXBoundary1) {
        if (xScrollOffset <= curXBoundary1) {
            --curXBoundary1;
            if (target->XVelocity < 0) {
                curXBoundary1 += target->XVelocity >> 16;
                if (curXBoundary1 < newXBoundary1)
                    curXBoundary1 = newXBoundary1;
            }
        }
        else {
            curXBoundary1 = newXBoundary1;
        }
    }
    if (newXBoundary2 < curXBoundary2) {
        if (newXBoundary2 > SCREEN_XSIZE + xScrollOffset)
            curXBoundary2 = newXBoundary2;
        else
            curXBoundary2 = SCREEN_XSIZE + xScrollOffset;
    }
    if (newXBoundary2 > curXBoundary2) {
        if (SCREEN_XSIZE + xScrollOffset >= curXBoundary2) {
            ++curXBoundary2;
            if (target->XVelocity > 0) {
                curXBoundary2 += target->XVelocity >> 16;
                if (curXBoundary2 > newXBoundary2)
                    curXBoundary2 = newXBoundary2;
            }
        }
        else {
            curXBoundary2 = newXBoundary2;
        }
    }

    cameraXPos = targetX;
    if (targetX < SCREEN_CENTERX + curXBoundary1) {
        targetX    = SCREEN_CENTERX + curXBoundary1;
        cameraXPos = SCREEN_CENTERX + curXBoundary1;
    }
    int boundX2 = curXBoundary2 - SCREEN_CENTERX;
    if (boundX2 < targetX) {
        targetX    = boundX2;
        cameraXPos = boundX2;
    }

    if (targetY <= curYBoundary1 + 119) {
        targetY    = curYBoundary1 + 120;
        cameraYPos = curYBoundary1 + 120;
    }
    else {
        cameraYPos = targetY;
    }
    if (curYBoundary2 - 119 <= targetY) {
        targetY    = curYBoundary2 - 120;
        cameraYPos = curYBoundary2 - 120;
    }

    xScrollOffset = cameraShakeX + targetX - SCREEN_CENTERX;
    int camY      = targetY + target->lookPos - SCREEN_CENTERY;
    if (curYBoundary1 > camY) {
        yScrollOffset = curYBoundary1;
    }
    else {
        yScrollOffset = targetY + target->lookPos - SCREEN_CENTERY;
    }

    int newCamY = curYBoundary2 - SCREEN_YSIZE;
    if (curYBoundary2 - (SCREEN_YSIZE - 1) > yScrollOffset)
        newCamY = yScrollOffset;
    yScrollOffset = cameraShakeY + newCamY;

    if (cameraShakeX) {
        if (cameraShakeX <= 0) {
            cameraShakeX = ~cameraShakeX;
        }
        else {
            cameraShakeX = -cameraShakeX;
        }
    }

    if (cameraShakeY) {
        if (cameraShakeY <= 0) {
            cameraShakeY = ~cameraShakeY;
        }
        else {
            cameraShakeY = -cameraShakeY;
        }
    }
}
