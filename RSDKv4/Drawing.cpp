#include "RetroEngine.hpp"

ushort blendLookupTable[BLENDTABLE_SIZE];
ushort subtractLookupTable[BLENDTABLE_SIZE];
ushort tintLookupTable[TINTTABLE_SIZE];

int SCREEN_XSIZE_CONFIG = 424;
int SCREEN_XSIZE        = 424;
int SCREEN_CENTERX      = 424 / 2;

int SCREEN_XSIZE_F   = 424;
int SCREEN_CENTERX_F = 424 / 2;

int SCREEN_YSIZE_F   = SCREEN_YSIZE;
int SCREEN_CENTERY_F = SCREEN_YSIZE / 2;

int touchWidth  = SCREEN_XSIZE;
int touchHeight = SCREEN_YSIZE;
float touchWidthF  = SCREEN_XSIZE;
float touchHeightF = SCREEN_YSIZE;

DrawListEntry drawListEntries[DRAWLAYER_COUNT];

int gfxDataPosition = 0;
GFXSurface gfxSurface[SURFACE_MAX];
byte graphicData[GFXDATA_MAX];

DisplaySettings displaySettings;
bool convertTo32Bit     = false;
bool mixFiltersOnJekyll = false;

#if RETRO_USING_OPENGL
GLint defaultFramebuffer = -1;
GLuint framebufferHiRes = -1;
GLuint renderbufferHiRes = -1;
#endif

#if RETRO_HARDWARE_RENDER
DrawVertex gfxPolyList[VERTEX_LIMIT];
short gfxPolyListIndex[INDEX_LIMIT];
ushort gfxVertexSize       = 0;
ushort gfxVertexSizeOpaque = 0;
ushort gfxIndexSize        = 0;
ushort gfxIndexSizeOpaque  = 0;

DrawVertex3D polyList3D[VERTEX3D_LIMIT];

ushort vertexSize3D = 0;
ushort indexSize3D  = 0;
float tileUVArray[TILEUV_SIZE];
float floor3DXPos     = 0.0f;
float floor3DYPos     = 0.0f;
float floor3DZPos     = 0.0f;
float floor3DAngle    = 0;
bool render3DEnabled  = false;
bool hq3DFloorEnabled = false;

ushort texBuffer[TEXBUFFER_SIZE];
byte texBufferMode = 0;

int orthWidth     = 0;
int viewWidth     = 0;
int viewHeight    = 0;
float viewAspect  = 0;
int bufferWidth   = 0;
int bufferHeight  = 0;
int virtualX      = 0;
int virtualY      = 0;
int virtualWidth  = 0;
int virtualHeight = 0;

#if RETRO_USING_OPENGL
GLuint gfxTextureID[TEXTURE_LIMIT];
GLuint framebufferId = 0;
GLuint fbTextureId   = 0;

short screenVerts[] = { 0, 0, 6400, 0, 0, SCREEN_YSIZE << 4, 6400, 0, 0, SCREEN_YSIZE << 4, 6400, SCREEN_YSIZE << 4 };
float fbTexVerts[]  = {
    -TEXTURE_SIZE, TEXTURE_SIZE, 0, TEXTURE_SIZE, -TEXTURE_SIZE, 0, 0, TEXTURE_SIZE, -TEXTURE_SIZE, 0, 0, 0,
};
float pureLight[] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };

#endif
#endif

// Alpha blending
#define setPixelAlpha(colour, frameBufferClr, alpha)                                                                                                 \
    ushort *blendPtrB = &blendLookupTable[BLENDTABLE_XSIZE * (0xFF - alpha)];                                                                        \
    ushort *blendPtrA = &blendLookupTable[BLENDTABLE_XSIZE * alpha];                                                                                 \
    frameBufferClr    = (blendPtrB[frameBufferClr & 0x1F] + blendPtrA[colour & 0x1F])                                                                \
                     | ((blendPtrB[(frameBufferClr & 0x7E0) >> 6] + blendPtrA[(colour & 0x7E0) >> 6]) << 6)                                          \
                     | ((blendPtrB[(frameBufferClr & 0xF800) >> 11] + blendPtrA[(colour & 0xF800) >> 11]) << 11)

#if !RETRO_USE_ORIGINAL_CODE
// enable integer scaling, which is a modification of enhanced scaling
bool integerScaling = false;
// allows me to disable it to prevent blur on resolutions that match only on 1 axis
bool disableEnhancedScaling = false;
// enable bilinear scaling, which just disables the fancy upscaling that enhanced scaling does.
bool bilinearScaling = false;
#endif

int InitRenderDevice()
{
    char gameTitle[0x40];

    sprintf(gameTitle, "%s%s", Engine.gameWindowText, Engine.usingDataFile ? "" : " (Using Data Folder)");

#if !RETRO_USE_ORIGINAL_CODE
#if RETRO_USING_SDL2
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_DisableScreenSaver();

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, Engine.vsync ? "1" : "0");

    byte flags = 0;
#if RETRO_USING_OPENGL
    flags |= SDL_WINDOW_OPENGL;
    
#if RETRO_PLATFORM != RETRO_OSX //dude idk either you just gotta trust that this works
#if RETRO_PLATFORM != RETRO_ANDROID
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetSwapInterval(0);
#endif
#endif
#if RETRO_DEVICETYPE == RETRO_MOBILE
    Engine.startFullScreen = true;

    SDL_DisplayMode dm;
    SDL_GetDesktopDisplayMode(0, &dm);
    
    bool landscape = dm.h < dm.w;
    int h = landscape ? dm.w : dm.h;
    int w = landscape ? dm.h : dm.w;

    SCREEN_XSIZE = ((float)SCREEN_YSIZE * h / w);
    if (SCREEN_XSIZE % 2) ++SCREEN_XSIZE;
#endif

    SCREEN_CENTERX = SCREEN_XSIZE / 2;

    Engine.window = SDL_CreateWindow(gameTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_XSIZE * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale, SDL_WINDOW_ALLOW_HIGHDPI | flags);

    if (!Engine.window) {
        printLog("ERROR: failed to create window!");
        return 0;
    }

#if !RETRO_USING_OPENGL
    Engine.renderer = SDL_CreateRenderer(Engine.window, -1, SDL_RENDERER_ACCELERATED);

    if (!Engine.renderer) {
        printLog("ERROR: failed to create renderer!");
        return 0;
    }

    SDL_RenderSetLogicalSize(Engine.renderer, SCREEN_XSIZE, SCREEN_YSIZE);
    SDL_SetRenderDrawBlendMode(Engine.renderer, SDL_BLENDMODE_BLEND);

#if RETRO_SOFTWARE_RENDER
    Engine.screenBuffer = SDL_CreateTexture(Engine.renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, SCREEN_XSIZE, SCREEN_YSIZE);

    if (!Engine.screenBuffer) {
        printLog("ERROR: failed to create screen buffer!\nerror msg: %s", SDL_GetError());
        return 0;
    }

    Engine.screenBuffer2x =
        SDL_CreateTexture(Engine.renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, SCREEN_XSIZE * 2, SCREEN_YSIZE * 2);

    if (!Engine.screenBuffer2x) {
        printLog("ERROR: failed to create screen buffer HQ!\nerror msg: %s", SDL_GetError());
        return 0;
    }
#endif
#endif

    if (Engine.borderless) {
        SDL_RestoreWindow(Engine.window);
        SDL_SetWindowBordered(Engine.window, SDL_FALSE);
    }

    SDL_DisplayMode disp;
    if (SDL_GetDisplayMode(0, 0, &disp) == 0) {
        Engine.screenRefreshRate = disp.refresh_rate;
    }

#endif

#if RETRO_USING_SDL1
    SDL_Init(SDL_INIT_EVERYTHING);

    byte flags = 0;
#if RETRO_USING_OPENGL
    flags |= SDL_OPENGL;
#endif

    Engine.windowSurface = SDL_SetVideoMode(SCREEN_XSIZE * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale, 32, SDL_SWSURFACE | flags);
    if (!Engine.windowSurface) {
        printLog("ERROR: failed to create window!\nerror msg: %s", SDL_GetError());
        return 0;
    }
    // Set the window caption
    SDL_WM_SetCaption(gameTitle, NULL);

    Engine.screenBuffer =
        SDL_CreateRGBSurface(0, SCREEN_XSIZE * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale, 16, 0xF800, 0x7E0, 0x1F, 0x00);

    if (!Engine.screenBuffer) {
        printLog("ERROR: failed to create screen buffer!\nerror msg: %s", SDL_GetError());
        return 0;
    }

    /*Engine.screenBuffer2x = SDL_SetVideoMode(SCREEN_XSIZE * 2, SCREEN_YSIZE * 2, 16, SDL_SWSURFACE | flags);
    if (!Engine.screenBuffer2x) {
        printLog("ERROR: failed to create screen buffer HQ!\nerror msg: %s", SDL_GetError());
        return 0;
    }*/

    if (Engine.startFullScreen) {
        Engine.windowSurface =
            SDL_SetVideoMode(SCREEN_XSIZE * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale, 16, SDL_SWSURFACE | SDL_FULLSCREEN | flags);
        SDL_ShowCursor(SDL_FALSE);
        Engine.isFullScreen = true;
    }

    // TODO: not supported in 1.2?
    if (Engine.borderless) {
        // SDL_RestoreWindow(Engine.window);
        // SDL_SetWindowBordered(Engine.window, SDL_FALSE);
    }

    // SDL_SetWindowPosition(Engine.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    Engine.useHQModes = false; // disabled
    Engine.borderless = false; // disabled
#endif

#if RETRO_USING_OPENGL

    // Init GL
    Engine.glContext = SDL_GL_CreateContext(Engine.window);

#if RETRO_PLATFORM != RETRO_ANDROID && RETRO_PLATFORM != RETRO_OSX
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        printLog("glew init error:");
        printLog((const char *)glewGetErrorString(err));
        return false;
    }
#endif

    displaySettings.field_20 = 0;
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_DITHER);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
#if RETRO_PLATFORM == RETRO_ANDROID
    Engine.windowScale = 1;
#endif
    displaySettings.width = SCREEN_XSIZE_CONFIG * Engine.windowScale;
    displaySettings.height = SCREEN_YSIZE * Engine.windowScale;

    textureList[0].id = -1;
    setupViewport();

    ResetRenderStates();
    SetupDrawIndexList();

    for (int c = 0; c < 0x10000; ++c) {
        int r               = (c & 0b1111100000000000) >> 8;
        int g               = (c & 0b0000011111100000) >> 3;
        int b               = (c & 0b0000000000011111) << 3;
        gfxPalette16to32[c] = (0xFF << 24) | (b << 16) | (g << 8) | (r << 0);
    }

    float lightAmbient[4] = { 2.0, 2.0, 2.0, 1.0 };
    float lightDiffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
    float lightPos[4]     = { 0.0, 0.0, 0.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHT0);

#if RETRO_PLATFORM == RETRO_ANDROID
    Engine.startFullScreen = true;
#endif
#endif


#if RETRO_PLATFORM != RETRO_ANDROID
    SetScreenDimensions(SCREEN_XSIZE_CONFIG * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale);
#else
    SetScreenDimensions(SCREEN_XSIZE, SCREEN_YSIZE);
#endif

#if RETRO_SOFTWARE_RENDER
    Engine.frameBuffer   = new ushort[GFX_LINESIZE * SCREEN_YSIZE];
    Engine.frameBuffer2x = new ushort[GFX_LINESIZE_DOUBLE * (SCREEN_YSIZE * 2)];
    memset(Engine.frameBuffer, 0, (GFX_LINESIZE * SCREEN_YSIZE) * sizeof(ushort));
    memset(Engine.frameBuffer2x, 0, GFX_LINESIZE_DOUBLE * (SCREEN_YSIZE * 2) * sizeof(ushort));
#endif
    Engine.texBuffer = new uint[GFX_LINESIZE * SCREEN_YSIZE];
    memset(Engine.texBuffer, 0, (GFX_LINESIZE * SCREEN_YSIZE) * sizeof(uint));

#endif

    if (Engine.startFullScreen) {
        setFullScreen(true);
    }

    OBJECT_BORDER_X2 = SCREEN_XSIZE + 0x80;
    // OBJECT_BORDER_Y2 = SCREEN_YSIZE + 0x100;
    OBJECT_BORDER_X4 = SCREEN_XSIZE + 0x20;
    // OBJECT_BORDER_Y4 = SCREEN_YSIZE + 0x80;

    InitInputDevices();

    return 1;
}
void FlipScreen()
{
#if !RETRO_USE_ORIGINAL_CODE
    if (Engine.dimTimer < Engine.dimLimit) {
        if (Engine.dimPercent < 1.0) {
            Engine.dimPercent += 0.05;
            if (Engine.dimPercent > 1.0)
                Engine.dimPercent = 1.0;
        }
    }
    else if (Engine.dimPercent > 0.25 && Engine.dimLimit >= 0) {
        Engine.dimPercent *= 0.9;
    }

    float dimAmount = Engine.dimMax * Engine.dimPercent;

#if RETRO_SOFTWARE_RENDER && !RETRO_USING_OPENGL
#if RETRO_USING_SDL2
    SDL_Rect destScreenPos_scaled;
    SDL_Texture *texTarget = NULL;

    switch (Engine.scalingMode) {
        // reset to default if value is invalid.
        default: Engine.scalingMode = RETRO_DEFAULTSCALINGMODE; break;
        case 0: break;                         // nearest
        case 1: integerScaling = true; break;  // integer scaling
        case 2: break;                         // sharp bilinear
        case 3: bilinearScaling = true; break; // regular old bilinear
    }

    SDL_GetWindowSize(Engine.window, &Engine.windowXSize, &Engine.windowYSize);
    float screenxsize = SCREEN_XSIZE;
    float screenysize = SCREEN_YSIZE;

    // check if enhanced scaling is even necessary to be calculated by checking if the screen size is close enough on one axis
    // unfortunately it has to be "close enough" because of floating point precision errors. dang it
    if (Engine.scalingMode == 2) {
        bool cond1 = std::round((Engine.windowXSize / screenxsize) * 24) / 24 == std::floor(Engine.windowXSize / screenxsize);
        bool cond2 = std::round((Engine.windowYSize / screenysize) * 24) / 24 == std::floor(Engine.windowYSize / screenysize);
        if (cond1 || cond2)
            disableEnhancedScaling = true;
    }

    // get 2x resolution if HQ is enabled.
    if (drawStageGFXHQ) {
        screenxsize *= 2;
        screenysize *= 2;
    }

    if (Engine.scalingMode != 0 && !disableEnhancedScaling) {
        // set up integer scaled texture, which is scaled to the largest integer scale of the screen buffer
        // before you make a texture that's larger than the window itself. This texture will then be scaled
        // up to the actual screen size using linear interpolation. This makes even window/screen scales
        // nice and sharp, and uneven scales as sharp as possible without creating wonky pixel scales,
        // creating a nice image.

        // get integer scale
        float scale = 1;
        if (!bilinearScaling) {
            scale =
                std::fminf(std::floor((float)Engine.windowXSize / (float)SCREEN_XSIZE), std::floor((float)Engine.windowYSize / (float)SCREEN_YSIZE));
        }
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"); // set interpolation to linear
        // create texture that's integer scaled.
        texTarget = SDL_CreateTexture(Engine.renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_TARGET, SCREEN_XSIZE * scale, SCREEN_YSIZE * scale);

        // keep aspect
        float aspectScale = std::fminf(Engine.windowYSize / screenysize, Engine.windowXSize / screenxsize);
        if (integerScaling) {
            aspectScale = std::floor(aspectScale);
        }
        float xoffset          = (Engine.windowXSize - (screenxsize * aspectScale)) / 2;
        float yoffset          = (Engine.windowYSize - (screenysize * aspectScale)) / 2;
        destScreenPos_scaled.x = std::round(xoffset);
        destScreenPos_scaled.y = std::round(yoffset);
        destScreenPos_scaled.w = std::round(screenxsize * aspectScale);
        destScreenPos_scaled.h = std::round(screenysize * aspectScale);
        // fill the screen with the texture, making lerp work.
        SDL_RenderSetLogicalSize(Engine.renderer, Engine.windowXSize, Engine.windowYSize);
    }

    int pitch = 0;
    SDL_SetRenderTarget(Engine.renderer, texTarget);

    // Clear the screen. This is needed to keep the
    // pillarboxes in fullscreen from displaying garbage data.
    SDL_RenderClear(Engine.renderer);

    ushort *pixels = NULL;
    if (!drawStageGFXHQ) {
        SDL_LockTexture(Engine.screenBuffer, NULL, (void **)&pixels, &pitch);
        ushort *frameBufferPtr = Engine.frameBuffer;
        for (int y = 0; y < SCREEN_YSIZE; ++y) {
            for (int x = 0; x < SCREEN_XSIZE; ++x) {
                pixels[x] = frameBufferPtr[x];
            }
            frameBufferPtr += GFX_LINESIZE;
            pixels += pitch / sizeof(ushort);
        }
        //memcpy(pixels, Engine.frameBuffer, pitch * SCREEN_YSIZE); //faster but produces issues with odd numbered screen sizes
        SDL_UnlockTexture(Engine.screenBuffer);

        SDL_RenderCopy(Engine.renderer, Engine.screenBuffer, NULL, NULL);
    }
    else {
        int w = 0, h = 0;
        SDL_QueryTexture(Engine.screenBuffer2x, NULL, NULL, &w, &h);
        SDL_LockTexture(Engine.screenBuffer2x, NULL, (void **)&pixels, &pitch);

        ushort *framebufferPtr = Engine.frameBuffer;
        for (int y = 0; y < (SCREEN_YSIZE / 2) + 12; ++y) {
            for (int x = 0; x < GFX_LINESIZE; ++x) {
                *pixels = *framebufferPtr;
                pixels++;
                *pixels = *framebufferPtr;
                pixels++;
                framebufferPtr++;
            }

            framebufferPtr -= GFX_LINESIZE;
            for (int x = 0; x < GFX_LINESIZE; ++x) {
                *pixels = *framebufferPtr;
                pixels++;
                *pixels = *framebufferPtr;
                pixels++;
                framebufferPtr++;
            }
        }

        framebufferPtr = Engine.frameBuffer2x;
        for (int y = 0; y < ((SCREEN_YSIZE / 2) - 12) * 2; ++y) {
            for (int x = 0; x < GFX_LINESIZE; ++x) {
                *pixels = *framebufferPtr;
                framebufferPtr++;
                pixels++;

                *pixels = *framebufferPtr;
                framebufferPtr++;
                pixels++;
            }
        }
        SDL_UnlockTexture(Engine.screenBuffer2x);
        SDL_RenderCopy(Engine.renderer, Engine.screenBuffer2x, NULL, NULL);
    }

    if (Engine.scalingMode != 0 && !disableEnhancedScaling) {
        // set render target back to the screen.
        SDL_SetRenderTarget(Engine.renderer, NULL);
        // clear the screen itself now, for same reason as above
        SDL_RenderClear(Engine.renderer);
        // copy texture to screen with lerp
        SDL_RenderCopy(Engine.renderer, texTarget, NULL, &destScreenPos_scaled);
        // Apply dimming
        SDL_SetRenderDrawColor(Engine.renderer, 0, 0, 0, 0xFF - (dimAmount * 0xFF));
        if (dimAmount < 1.0)
            SDL_RenderFillRect(Engine.renderer, NULL);
        // finally present it
        SDL_RenderPresent(Engine.renderer);
        // reset everything just in case
        SDL_RenderSetLogicalSize(Engine.renderer, SCREEN_XSIZE, SCREEN_YSIZE);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
        // putting some FLEX TAPE� on that memory leak
        SDL_DestroyTexture(texTarget);
    }
    else {
        // Apply dimming
        SDL_SetRenderDrawColor(Engine.renderer, 0, 0, 0, 0xFF - (dimAmount * 0xFF));
        if (dimAmount < 1.0)
            SDL_RenderFillRect(Engine.renderer, NULL);
        // no change here
        SDL_RenderPresent(Engine.renderer);
    }
    SDL_ShowWindow(Engine.window);
#endif

#if RETRO_USING_SDL1
    ushort *px = (ushort *)Engine.screenBuffer->pixels;
    int w      = SCREEN_XSIZE * Engine.windowScale;
    int h      = SCREEN_YSIZE * Engine.windowScale;

    if (Engine.windowScale == 1) {
        ushort *frameBufferPtr = Engine.frameBuffer;
        for (int y = 0; y < SCREEN_YSIZE; ++y) {
            for (int x = 0; x < SCREEN_XSIZE; ++x) {
                pixels[x] = frameBufferPtr[x];
            }
            frameBufferPtr += GFX_LINESIZE;
            px += Engine.screenBuffer->pitch / sizeof(ushort);
        }
        // memcpy(Engine.screenBuffer->pixels, Engine.frameBuffer, Engine.screenBuffer->pitch * SCREEN_YSIZE);
    }
    else {
        // TODO: this better, I really dont know how to use SDL1.2 well lol
        int dx = 0, dy = 0;
        do {
            do {
                int x = (int)(dx * (1.0f / Engine.windowScale));
                int y = (int)(dy * (1.0f / Engine.windowScale));

                px[dx + (dy * w)] = Engine.frameBuffer[x + (y * GFX_LINESIZE)];

                dx++;
            } while (dx < w);
            dy++;
            dx = 0;
        } while (dy < h);
    }

    // Apply image to screen
    SDL_BlitSurface(Engine.screenBuffer, NULL, Engine.windowSurface, NULL);

    // Update Screen
    SDL_Flip(Engine.windowSurface);
#endif

#endif // !RETRO_SOFTWARE_RENDER

#if RETRO_HARDWARE_RENDER
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

    glLoadIdentity();

    glOrtho(0, orthWidth, SCREEN_YSIZE << 4, 0.0, 0.0f, 100.0f);
    if (texPaletteNum >= TEXTURE_LIMIT) {
        glBindTexture(GL_TEXTURE_2D, gfxTextureID[texPaletteNum % TEXTURE_LIMIT]);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, gfxTextureID[texPaletteNum]);
    }
    glEnableClientState(GL_COLOR_ARRAY);

    if (render3DEnabled) {
        // Non Blended rendering
        glVertexPointer(2, GL_SHORT, sizeof(DrawVertex), &gfxPolyList[0].x);
        glTexCoordPointer(2, GL_SHORT, sizeof(DrawVertex), &gfxPolyList[0].u);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(DrawVertex), &gfxPolyList[0].colour);
        glDrawElements(GL_TRIANGLES, gfxIndexSizeOpaque, GL_UNSIGNED_SHORT, gfxPolyListIndex);
        glEnable(GL_BLEND);

        // Init 3D Plane
        glViewport(0, 0, viewWidth, viewHeight);
        glPushMatrix();
        glLoadIdentity();
        CalcPerspective(1.8326f, viewAspect, 0.1f, 1000.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glScalef(1.0f, -1.0f, -1.0f);
        glRotatef(180.0f + floor3DAngle, 0, 1.0f, 0);
        glTranslatef(floor3DXPos, floor3DYPos, floor3DZPos);
        glVertexPointer(3, GL_FLOAT, sizeof(DrawVertex3D), &polyList3D[0].x);
        glTexCoordPointer(2, GL_SHORT, sizeof(DrawVertex3D), &polyList3D[0].u);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(DrawVertex3D), &polyList3D[0].colour);
        glDrawElements(GL_TRIANGLES, indexSize3D, GL_UNSIGNED_SHORT, gfxPolyListIndex);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);

        // Return for blended rendering
        glViewport(0, 0, bufferWidth, bufferHeight);
        glPopMatrix();

        int numBlendedGfx = (int)(gfxIndexSize - gfxIndexSizeOpaque);
        glVertexPointer(2, GL_SHORT, sizeof(DrawVertex), &gfxPolyList[0].x);
        glTexCoordPointer(2, GL_SHORT, sizeof(DrawVertex), &gfxPolyList[0].u);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(DrawVertex), &gfxPolyList[0].colour);
        glDrawElements(GL_TRIANGLES, numBlendedGfx, GL_UNSIGNED_SHORT, &gfxPolyListIndex[gfxIndexSizeOpaque]);
    }
    else {
        glVertexPointer(2, GL_SHORT, sizeof(DrawVertex), &gfxPolyList[0].x);
        glTexCoordPointer(2, GL_SHORT, sizeof(DrawVertex), &gfxPolyList[0].u);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(DrawVertex), &gfxPolyList[0].colour);
        glDrawElements(GL_TRIANGLES, gfxIndexSizeOpaque, GL_UNSIGNED_SHORT, gfxPolyListIndex);

        int blendedGfxCount = gfxIndexSize - gfxIndexSizeOpaque;

        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glVertexPointer(2, GL_SHORT, sizeof(DrawVertex), &gfxPolyList[0].x);
        glTexCoordPointer(2, GL_SHORT, sizeof(DrawVertex), &gfxPolyList[0].u);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(DrawVertex), &gfxPolyList[0].colour);
        glDrawElements(GL_TRIANGLES, blendedGfxCount, GL_UNSIGNED_SHORT, &gfxPolyListIndex[gfxIndexSizeOpaque]);
    }
    glDisableClientState(GL_COLOR_ARRAY);

    // Render the framebuffer now
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(virtualX, virtualY, virtualWidth, virtualHeight);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fbTextureId);
    glVertexPointer(2, GL_SHORT, 0, &screenVerts);
    glTexCoordPointer(2, GL_FLOAT, 0, &fbTexVerts);
    glColorPointer(4, GL_FLOAT, 0, &pureLight);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glViewport(0, 0, bufferWidth, bufferHeight);
#endif

#endif
}
void ReleaseRenderDevice()
{
    ClearMeshData();
    ClearTextures(false);

#if !RETRO_USE_ORIGINAL_CODE
#if RETRO_SOFTWARE_RENDER
    if (Engine.frameBuffer)
        delete[] Engine.frameBuffer;
    if (Engine.frameBuffer2x)
        delete[] Engine.frameBuffer2x;
#if RETRO_USING_SDL2 && !RETRO_USING_OPENGL
    SDL_DestroyTexture(Engine.screenBuffer);
    Engine.screenBuffer = NULL;
#endif
    if (Engine.texBuffer)
        delete[] Engine.texBuffer;

#if RETRO_USING_SDL1
    SDL_FreeSurface(Engine.screenBuffer);
#endif
#endif

#if RETRO_USING_OPENGL
    if (Engine.glContext)
        SDL_GL_DeleteContext(Engine.glContext);
#endif

#if RETRO_USING_SDL2
#if !RETRO_USING_OPENGL
    SDL_DestroyRenderer(Engine.renderer);
#endif
    SDL_DestroyWindow(Engine.window);
#endif
#endif
}

void GenerateBlendLookupTable(void)
{
    int blendTableID = 0;
    for (int y = 0; y < BLENDTABLE_YSIZE; y++) {
        for (int x = 0; x < BLENDTABLE_XSIZE; x++) {
            blendLookupTable[blendTableID]      = y * x >> 8;
            subtractLookupTable[blendTableID++] = y * (0x1F - x) >> 8;
        }
    }

    for (int i = 0; i < TINTTABLE_SIZE; i++) {
        int tintValue = ((i & 0x1F) + ((i & 0x7E0) >> 6) + ((i & 0xF800) >> 11)) / 3 + 6;
        if (tintValue > 31)
            tintValue = 31;
        tintLookupTable[i] = 0x841 * tintValue;
    }
}

void ClearScreen(byte index)
{
#if RETRO_SOFTWARE_RENDER
    ushort colour       = activePalette[index];
    ushort *framebuffer = Engine.frameBuffer;
    int cnt             = GFX_LINESIZE * SCREEN_YSIZE;
    while (cnt--) {
        *framebuffer = colour;
        ++framebuffer;
    }
#endif

#if RETRO_HARDWARE_RENDER
    gfxPolyList[gfxVertexSize].x        = 0.0f;
    gfxPolyList[gfxVertexSize].y        = 0.0f;
    gfxPolyList[gfxVertexSize].colour.r = activePalette32[index].r;
    gfxPolyList[gfxVertexSize].colour.g = activePalette32[index].g;
    gfxPolyList[gfxVertexSize].colour.b = activePalette32[index].b;
    gfxPolyList[gfxVertexSize].colour.a = 0xFF;
    gfxPolyList[gfxVertexSize].u        = 0.0f;
    gfxPolyList[gfxVertexSize].v        = 0.0f;

    gfxVertexSize++;
    gfxPolyList[gfxVertexSize].x        = SCREEN_XSIZE << 4;
    gfxPolyList[gfxVertexSize].y        = 0.0f;
    gfxPolyList[gfxVertexSize].colour.r = activePalette32[index].r;
    gfxPolyList[gfxVertexSize].colour.g = activePalette32[index].g;
    gfxPolyList[gfxVertexSize].colour.b = activePalette32[index].b;
    gfxPolyList[gfxVertexSize].colour.a = 0xFF;
    gfxPolyList[gfxVertexSize].u        = 0.0f;
    gfxPolyList[gfxVertexSize].v        = 0.0f;

    gfxVertexSize++;
    gfxPolyList[gfxVertexSize].x        = 0.0f;
    gfxPolyList[gfxVertexSize].y        = SCREEN_YSIZE << 4;
    gfxPolyList[gfxVertexSize].colour.r = activePalette32[index].r;
    gfxPolyList[gfxVertexSize].colour.g = activePalette32[index].g;
    gfxPolyList[gfxVertexSize].colour.b = activePalette32[index].b;
    gfxPolyList[gfxVertexSize].colour.a = 0xFF;
    gfxPolyList[gfxVertexSize].u        = 0.0f;
    gfxPolyList[gfxVertexSize].v        = 0.0f;

    gfxVertexSize++;
    gfxPolyList[gfxVertexSize].x        = SCREEN_XSIZE << 4;
    gfxPolyList[gfxVertexSize].y        = SCREEN_YSIZE << 4;
    gfxPolyList[gfxVertexSize].colour.r = activePalette32[index].r;
    gfxPolyList[gfxVertexSize].colour.g = activePalette32[index].g;
    gfxPolyList[gfxVertexSize].colour.b = activePalette32[index].b;
    gfxPolyList[gfxVertexSize].colour.a = 0xFF;
    gfxPolyList[gfxVertexSize].u        = 0.0f;
    gfxPolyList[gfxVertexSize].v        = 0.0f;
    gfxVertexSize++;

    gfxIndexSize += 6;
#endif
}

void SetScreenDimensions(int width, int height)
{
    touchWidth               = width;
    touchHeight              = height;
    displaySettings.width    = width;
    displaySettings.height   = height;
    touchWidthF              = width;
    displaySettings.field_10 = 16;
    touchHeightF             = height;
    //displaySettings.maxWidth = 424;
    double aspect    = (((width >> 16) * 65536.0) + width) / (((height >> 16) * 65536.0) + height);
    SCREEN_XSIZE_F   = SCREEN_YSIZE * aspect;
    SCREEN_CENTERX_F = aspect * SCREEN_CENTERY;
    SetPerspectiveMatrix(SCREEN_YSIZE * aspect, SCREEN_YSIZE_F, 0.0, 1000.0);
#if RETRO_USING_OPENGL
    glViewport(0, 0, displaySettings.width, displaySettings.height);
#endif

    Engine.useHighResAssets = displaySettings.height > (SCREEN_YSIZE * 2);
    int displayWidth        = aspect * SCREEN_YSIZE;
    //if (val > displaySettings.maxWidth)
    //    val = displaySettings.maxWidth;
    SetScreenSize(displayWidth, (displayWidth + 9) & -0x10);

    int width2 = 0;
    int wBuf   = GFX_LINESIZE - 1;
    while (wBuf > 0) {
        width2++;
        wBuf >>= 1;
    }
    int height2 = 0;
    int hBuf    = SCREEN_YSIZE - 1;
    while (hBuf > 0) {
        height2++;
        hBuf >>= 1;
    }
    int texWidth = 1 << width2;
    int texHeight = 1 << height2;

    textureList[0].widthN  = 1.0f / texWidth;
    textureList[0].heightN = 1.0f / texHeight;

    float w  = (SCREEN_XSIZE * textureList[0].widthN);
    float w2 = (GFX_LINESIZE * textureList[0].widthN);
    float h  = (SCREEN_YSIZE * textureList[0].heightN);

    retroVertexList[0]     = -SCREEN_CENTERX_F;
    retroVertexList[1]     = SCREEN_CENTERY_F;
    retroVertexList[2]     = 160.0;
    retroVertexList[6]     = 0.0;
    retroVertexList[7]     = 0.0;

    retroVertexList[9]     = SCREEN_CENTERX_F;
    retroVertexList[10]    = SCREEN_CENTERY_F;
    retroVertexList[11]    = 160.0;
    retroVertexList[15]    = w;
    retroVertexList[16]    = 0.0;

    retroVertexList[18]    = -SCREEN_CENTERX_F;
    retroVertexList[19]    = -SCREEN_CENTERY_F;
    retroVertexList[20]    = 160.0;
    retroVertexList[24]    = 0.0;
    retroVertexList[25]    = h;

    retroVertexList[27]    = SCREEN_CENTERX_F;
    retroVertexList[28]    = -SCREEN_CENTERY_F;
    retroVertexList[29]    = 160.0;
    retroVertexList[33]    = w;
    retroVertexList[34]    = h;

    screenBufferVertexList[0]  = -1.0;
    screenBufferVertexList[1]  = 1.0;
    screenBufferVertexList[2]  = 1.0;
    screenBufferVertexList[6]  = 0.0;
    screenBufferVertexList[7]  = h;

    screenBufferVertexList[9]  = 1.0;
    screenBufferVertexList[10] = 1.0;
    screenBufferVertexList[11] = 1.0;
    screenBufferVertexList[15] = w2;
    screenBufferVertexList[16] = h;

    screenBufferVertexList[18] = -1.0;
    screenBufferVertexList[19] = -1.0;
    screenBufferVertexList[20] = 1.0;
    screenBufferVertexList[24] = 0.0;
    screenBufferVertexList[25] = 0.0;

    screenBufferVertexList[27] = 1.0;
    screenBufferVertexList[28] = -1.0;
    screenBufferVertexList[29] = 1.0;
    screenBufferVertexList[33] = w2;
    screenBufferVertexList[34] = 0.0;
}

void SetScreenSize(int width, int lineSize)
{
    SCREEN_XSIZE        = width;
    SCREEN_CENTERX      = width / 2;
    SCREEN_SCROLL_LEFT  = SCREEN_CENTERX - 8;
    SCREEN_SCROLL_RIGHT = SCREEN_CENTERX + 8;
    OBJECT_BORDER_X2    = width + 0x80;
    OBJECT_BORDER_X4    = width + 0x20;

    GFX_LINESIZE          = lineSize;
    GFX_LINESIZE_MINUSONE = lineSize - 1;
    GFX_LINESIZE_DOUBLE   = 2 * lineSize;
    GFX_FRAMEBUFFERSIZE   = SCREEN_YSIZE * lineSize;
    GFX_FBUFFERMINUSONE   = SCREEN_YSIZE * lineSize - 1;
}

#if RETRO_SOFTWARE_RENDER
void CopyFrameOverlay2x()
{
    ushort *frameBuffer   = &Engine.frameBuffer[((SCREEN_YSIZE / 2) + 12) * GFX_LINESIZE];
    ushort *frameBuffer2x = Engine.frameBuffer2x;

    for (int y = 0; y < (SCREEN_YSIZE / 2) - 12; ++y) {
        for (int x = 0; x < GFX_LINESIZE; ++x) {
            if (*frameBuffer == 0xF81F) { // magenta
                frameBuffer2x += 2;
            }
            else {
                *frameBuffer2x = *frameBuffer;
                frameBuffer2x++;
                *frameBuffer2x = *frameBuffer;
                frameBuffer2x++;
            }
            ++frameBuffer;
        }

        frameBuffer -= GFX_LINESIZE;
        for (int x = 0; x < GFX_LINESIZE; ++x) {
            if (*frameBuffer == 0xF81F) { // magenta
                frameBuffer2x += 2;
            }
            else {
                *frameBuffer2x = *frameBuffer;
                frameBuffer2x++;
                *frameBuffer2x = *frameBuffer;
                frameBuffer2x++;
            }
            ++frameBuffer;
        }
    }
}
#endif

#if RETRO_HARDWARE_RENDER
void UpdateHardwareTextures()
{
    SetActivePalette(0, 0, SCREEN_YSIZE);
    UpdateTextureBufferWithTiles();
    UpdateTextureBufferWithSortedSprites();

    glBindTexture(GL_TEXTURE_2D, gfxTextureID[0]);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEXTURE_SIZE, TEXTURE_SIZE, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, texBuffer);

    for (byte b = 1; b < TEXTURE_LIMIT; ++b) {
        SetActivePalette(b, 0, SCREEN_YSIZE);
        UpdateTextureBufferWithTiles();
        UpdateTextureBufferWithSprites();

        glBindTexture(GL_TEXTURE_2D, gfxTextureID[b]);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEXTURE_SIZE, TEXTURE_SIZE, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, texBuffer);
    }
    SetActivePalette(0, 0, SCREEN_YSIZE);
}
void SetScreenDimensions(int width, int height, int scale)
{
    viewWidth = touchWidth = width * scale;
    viewHeight = touchHeight = height * scale;

    // float widthBuf = (float)viewWidth / (float)viewHeight;
    // widthBuf *= (float)SCREEN_YSIZE;
    bufferWidth = width;
    // bufferWidth += 8;
    // bufferWidth = bufferWidth >> 4 << 4;

    width *= scale;
    height *= scale;

    viewAspect = 0.75f;
    if (viewHeight >= SCREEN_YSIZE * 2)
        hq3DFloorEnabled = true;
    else
        hq3DFloorEnabled = false;

    SetScreenSize(bufferWidth, bufferWidth);
    if (viewHeight >= SCREEN_YSIZE * 2) {
        bufferWidth *= 2;
        bufferHeight = SCREEN_YSIZE * 2;
    }
    else {
        bufferHeight = SCREEN_YSIZE;
    }
    orthWidth = SCREEN_XSIZE * 16;

    if (framebufferId > 0) {
        glDeleteFramebuffers(1, &framebufferId);
    }
    if (fbTextureId > 0) {
        glDeleteTextures(1, &fbTextureId);
    }

    // Setup framebuffer texture
    glGenFramebuffers(1, &framebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    glGenTextures(1, &fbTextureId);
    glBindTexture(GL_TEXTURE_2D, fbTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewWidth, viewHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbTextureId, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int newWidth  = width * 8;
    int newHeight = (height * 8) + 4;

    screenVerts[2]  = newWidth;
    screenVerts[6]  = newWidth;
    screenVerts[10] = newWidth;
    screenVerts[5]  = newHeight;
    screenVerts[9]  = newHeight;
    screenVerts[11] = newHeight;
    ScaleViewport(width, height);
}

void ScaleViewport(int width, int height)
{
    virtualWidth  = width;
    virtualHeight = height;
    virtualX      = 0;
    virtualY      = 0;

    float virtualAspect = (float)width / height;
    float realAspect    = (float)viewWidth / viewHeight;
    if (virtualAspect < realAspect) {
        virtualHeight = viewHeight * ((float)width / viewWidth);
        virtualY      = (height - virtualHeight) >> 1;
    }
    else {
        virtualWidth = viewWidth * ((float)height / viewHeight);
        virtualX     = (width - virtualWidth) >> 1;
    }
}

void CalcPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
    float matrix[16];
    float w = 1.0 / tanf(fov * 0.5f);
    float h = 1.0 / (w * aspectRatio);
    float q = (nearPlane + farPlane) / (farPlane - nearPlane);

    matrix[0] = w;
    matrix[1] = 0;
    matrix[2] = 0;
    matrix[3] = 0;

    matrix[4] = 0;
    matrix[5] = h / 2;
    matrix[6] = 0;
    matrix[7] = 0;

    matrix[8]  = 0;
    matrix[9]  = 0;
    matrix[10] = q;
    matrix[11] = 1.0;

    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = (((farPlane * -2.0f) * nearPlane) / (farPlane - nearPlane));
    matrix[15] = 0;

#if RETRO_USING_OPENGL
    glMultMatrixf(matrix);
#endif
}

void SetupPolygonLists()
{
    int vID = 0;
    for (int i = 0; i < VERTEX_LIMIT; i++) {
        gfxPolyListIndex[vID++] = (i << 2) + 0;
        gfxPolyListIndex[vID++] = (i << 2) + 1;
        gfxPolyListIndex[vID++] = (i << 2) + 2;
        gfxPolyListIndex[vID++] = (i << 2) + 1;
        gfxPolyListIndex[vID++] = (i << 2) + 3;
        gfxPolyListIndex[vID++] = (i << 2) + 2;

        gfxPolyList[i].colour.r = 0xFF;
        gfxPolyList[i].colour.g = 0xFF;
        gfxPolyList[i].colour.b = 0xFF;
        gfxPolyList[i].colour.a = 0xFF;
    }

    for (int i = 0; i < VERTEX3D_LIMIT; i++) {
        polyList3D[i].colour.r = 0xFF;
        polyList3D[i].colour.g = 0xFF;
        polyList3D[i].colour.b = 0xFF;
        polyList3D[i].colour.a = 0xFF;
    }
}

void UpdateTextureBufferWithTiles()
{
    int cnt = 0;
    if (texBufferMode == 0) {
        for (int h = 0; h < 512; h += 16) {
            for (int w = 0; w < 512; w += 16) {
                int dataPos = cnt << 8;
                cnt++;
                int bufPos = w + (h << 10);
                for (int y = 0; y < TILE_SIZE; y++) {
                    for (int x = 0; x < TILE_SIZE; x++) {
                        if (tilesetGFXData[dataPos] > 0) {
                            texBuffer[bufPos] = fullPalette[texPaletteNum][tilesetGFXData[dataPos]];
                        }
                        else {
                            texBuffer[bufPos] = 0;
                        }
                        bufPos++;
                        dataPos++;
                    }
                    bufPos += 1008;
                }
            }
        }
    }
    else {
        for (int h = 0; h < 504; h += 18) {
            for (int w = 0; w < 504; w += 18) {
                int dataPos = cnt << 8;
                cnt++;
                if (cnt == 783)
                    cnt = 1023;

                int bufPos = w + (h << 10);
                if (tilesetGFXData[dataPos] > 0) {
                    texBuffer[bufPos] = fullPalette[texPaletteNum][tilesetGFXData[dataPos]];
                }
                else {
                    texBuffer[bufPos] = 0;
                }
                bufPos++;

                for (int l = 0; l < 15; l++) {
                    if (tilesetGFXData[dataPos] > 0) {
                        texBuffer[bufPos] = fullPalette[texPaletteNum][tilesetGFXData[dataPos]];
                    }
                    else {
                        texBuffer[bufPos] = 0;
                    }
                    bufPos++;
                    dataPos++;
                }

                if (tilesetGFXData[dataPos] > 0) {
                    texBuffer[bufPos] = fullPalette[texPaletteNum][tilesetGFXData[dataPos]];
                    bufPos++;
                    texBuffer[bufPos] = fullPalette[texPaletteNum][tilesetGFXData[dataPos]];
                }
                else {
                    texBuffer[bufPos] = 0;
                    bufPos++;
                    texBuffer[bufPos] = 0;
                }
                bufPos++;
                dataPos -= 15;
                bufPos += 1006;

                for (int k = 0; k < 16; k++) {
                    if (tilesetGFXData[dataPos] > 0) {
                        texBuffer[bufPos] = fullPalette[texPaletteNum][tilesetGFXData[dataPos]];
                    }
                    else {
                        texBuffer[bufPos] = 0;
                    }
                    bufPos++;
                    for (int l = 0; l < 15; l++) {
                        if (tilesetGFXData[dataPos] > 0) {
                            texBuffer[bufPos] = fullPalette[texPaletteNum][tilesetGFXData[dataPos]];
                        }
                        else {
                            texBuffer[bufPos] = 0;
                        }
                        bufPos++;
                        dataPos++;
                    }
                    if (tilesetGFXData[dataPos] > 0) {
                        texBuffer[bufPos] = fullPalette[texPaletteNum][tilesetGFXData[dataPos]];
                        bufPos++;
                        texBuffer[bufPos] = fullPalette[texPaletteNum][tilesetGFXData[dataPos]];
                    }
                    else {
                        texBuffer[bufPos] = 0;
                        bufPos++;
                        texBuffer[bufPos] = 0;
                    }
                    bufPos++;
                    dataPos++;
                    bufPos += 1006;
                }
                dataPos -= 16;

                if (tilesetGFXData[dataPos] > 0) {
                    texBuffer[bufPos] = fullPalette[texPaletteNum][tilesetGFXData[dataPos]];
                }
                else {
                    texBuffer[bufPos] = 0;
                }
                bufPos++;

                for (int l = 0; l < 15; l++) {
                    if (tilesetGFXData[dataPos] > 0) {
                        texBuffer[bufPos] = fullPalette[texPaletteNum][tilesetGFXData[dataPos]];
                    }
                    else {
                        texBuffer[bufPos] = 0;
                    }
                    bufPos++;
                    dataPos++;
                }

                if (tilesetGFXData[dataPos] > 0) {
                    texBuffer[bufPos] = fullPalette[texPaletteNum][tilesetGFXData[dataPos]];
                    bufPos++;
                    texBuffer[bufPos] = fullPalette[texPaletteNum][tilesetGFXData[dataPos]];
                }
                else {
                    texBuffer[bufPos] = 0;
                    bufPos++;
                    texBuffer[bufPos] = 0;
                }
                bufPos++;
                bufPos += 1006;
            }
        }
    }

    int bufPos = 0;
    for (int k = 0; k < TILE_SIZE; k++) {
        for (int l = 0; l < TILE_SIZE; l++) {
            texBuffer[bufPos] = PACK_RGB888(0xFF, 0xFF, 0xFF);
            texBuffer[bufPos] |= 1;
            bufPos++;
        }
        bufPos += 1008;
    }
}
void UpdateTextureBufferWithSortedSprites()
{
    byte surfCnt = 0;
    byte surfList[SURFACE_MAX];
    bool flag = true;
    for (int i = 0; i < SURFACE_MAX; i++) gfxSurface[i].texStartX = -1;

    for (int i = 0; i < SURFACE_MAX; i++) {
        int gfxSize  = 0;
        sbyte surfID = -1;
        for (int s = 0; s < SURFACE_MAX; s++) {
            GFXSurface *surface = &gfxSurface[s];
            if (StrLength(surface->fileName) && surface->texStartX == -1) {
                if (CheckSurfaceSize(surface->width) && CheckSurfaceSize(surface->height)) {
                    if (surface->width + surface->height > gfxSize) {
                        gfxSize = surface->width + surface->height;
                        surfID  = s;
                    }
                }
                else {
                    surface->texStartX = 0;
                }
            }
        }

        if (surfID == -1) {
            i = SURFACE_MAX;
        }
        else {
            gfxSurface[surfID].texStartX = 0;
            surfList[surfCnt++]          = surfID;
        }
    }

    for (int i = 0; i < SURFACE_MAX; i++) gfxSurface[i].texStartX = -1;

    for (int i = 0; i < surfCnt; i++) {
        GFXSurface *curSurface = &gfxSurface[surfList[i]];
        curSurface->texStartX  = 0;
        curSurface->texStartY  = 0;
        bool loopFlag          = true;
        while (loopFlag) {
            loopFlag = false;
            if (curSurface->height == TEXTURE_SIZE)
                flag = false;

            if (flag) {
                if (curSurface->texStartX < 512 && curSurface->texStartY < 512) {
                    loopFlag = true;
                    curSurface->texStartX += curSurface->width;
                    if (curSurface->texStartX + curSurface->width > TEXTURE_SIZE) {
                        curSurface->texStartX = 0;
                        curSurface->texStartY += curSurface->height;
                    }
                }
                else {
                    for (int s = 0; s < SURFACE_MAX; s++) {
                        GFXSurface *surface = &gfxSurface[s];
                        if (surface->texStartX > -1 && s != surfList[i] && curSurface->texStartX < surface->texStartX + surface->width
                            && curSurface->texStartX >= surface->texStartX && curSurface->texStartY < surface->texStartY + surface->height) {
                            loopFlag = true;
                            curSurface->texStartX += curSurface->width;
                            if (curSurface->texStartX + curSurface->width > TEXTURE_SIZE) {
                                curSurface->texStartX = 0;
                                curSurface->texStartY += curSurface->height;
                            }
                            s = SURFACE_MAX;
                        }
                    }
                }
            }
            else {
                if (curSurface->width < TEXTURE_SIZE) {
                    if (curSurface->texStartX < 16 && curSurface->texStartY < 16) {
                        loopFlag = true;
                        curSurface->texStartX += curSurface->width;
                        if (curSurface->texStartX + curSurface->width > TEXTURE_SIZE) {
                            curSurface->texStartX = 0;
                            curSurface->texStartY += curSurface->height;
                        }
                    }
                    else {
                        for (int s = 0; s < SURFACE_MAX; s++) {
                            GFXSurface *surface = &gfxSurface[s];
                            if (surface->texStartX > -1 && s != surfList[i] && curSurface->texStartX < surface->texStartX + surface->width
                                && curSurface->texStartX >= surface->texStartX && curSurface->texStartY < surface->texStartY + surface->height) {
                                loopFlag = true;
                                curSurface->texStartX += curSurface->width;
                                if (curSurface->texStartX + curSurface->width > TEXTURE_SIZE) {
                                    curSurface->texStartX = 0;
                                    curSurface->texStartY += curSurface->height;
                                }
                                s = SURFACE_MAX;
                            }
                        }
                    }
                }
            }
        }

        if (curSurface->texStartY + curSurface->height <= TEXTURE_SIZE) {
            int gfXPos  = curSurface->dataPosition;
            int dataPos = curSurface->texStartX + (curSurface->texStartY << 10);
            for (int h = 0; h < curSurface->height; h++) {
                for (int w = 0; w < curSurface->width; w++) {
                    if (graphicData[gfXPos] > 0) {
                        texBuffer[dataPos] = fullPalette[texPaletteNum][graphicData[gfXPos]];
                    }
                    else {
                        texBuffer[dataPos] = 0;
                    }
                    dataPos++;
                    gfXPos++;
                }
                dataPos += TEXTURE_SIZE - curSurface->width;
            }
        }
    }
}
void UpdateTextureBufferWithSprites()
{
    for (int i = 0; i < SURFACE_MAX; ++i) {
        if (gfxSurface[i].texStartY + gfxSurface[i].height <= TEXTURE_SIZE && gfxSurface[i].texStartX > -1) {
            int pos    = gfxSurface[i].dataPosition;
            int teXPos = gfxSurface[i].texStartX + (gfxSurface[i].texStartY << 10);
            for (int j = 0; j < gfxSurface[i].height; j++) {
                for (int k = 0; k < gfxSurface[i].width; k++) {
                    if (graphicData[pos] > 0)
                        texBuffer[teXPos] = fullPalette[texPaletteNum][graphicData[pos]];
                    else
                        texBuffer[teXPos] = 0;

                    teXPos++;
                    pos++;
                }
                teXPos += TEXTURE_SIZE - gfxSurface[i].width;
            }
        }
    }
}
#endif

void setupViewport()
{
    double aspect     = displaySettings.width / (double)displaySettings.height;
    SCREEN_XSIZE_F   = SCREEN_YSIZE * aspect;
    SCREEN_CENTERX_F = aspect * SCREEN_CENTERY;

#if RETRO_USING_OPENGL
    glScalef(320.0f / (SCREEN_YSIZE * aspect), 1.0, 1.0);
#endif

    SetPerspectiveMatrix(90.0, 0.75, 1.0, 5000.0);
#if RETRO_USING_OPENGL
    glViewport(0, 0, displaySettings.width, displaySettings.height);
#endif
    int displayWidth = aspect * SCREEN_YSIZE;
    SetScreenSize(displayWidth, (displayWidth + 9) & -0x10);

    Engine.useHighResAssets = displaySettings.height > (SCREEN_YSIZE * 2);

#if RETRO_USING_OPENGL
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#endif

    int width2 = 0;
    int wBuf   = GFX_LINESIZE - 1;
    while (wBuf > 0) {
        width2++;
        wBuf >>= 1;
    }
    int height2 = 0;
    int hBuf    = SCREEN_YSIZE - 1;
    while (hBuf > 0) {
        height2++;
        hBuf >>= 1;
    }
    int texWidth  = 1 << width2;
    int texHeight = 1 << height2;

    StrCopy(textureList[0].fileName, "RetroBuffer");
    textureList[0].width   = texWidth;
    textureList[0].height  = texHeight;
    textureList[0].format  = TEXFMT_RETROBUFFER;
    textureList[0].widthN  = 1.0f / texWidth;
    textureList[0].heightN = 1.0f / texHeight;

    if (Engine.useHighResAssets) {
#if RETRO_USING_OPENGL
        if (framebufferHiRes != -1)
            glDeleteFramebuffers(1, &framebufferHiRes);
        if (renderbufferHiRes != -1)
            glDeleteTextures(1, &renderbufferHiRes);
        framebufferHiRes  = -1;
        renderbufferHiRes = -1;

        glGenFramebuffers(1, &framebufferHiRes);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferHiRes);
        glGenTextures(1, &renderbufferHiRes);
        glBindTexture(GL_TEXTURE_2D, renderbufferHiRes);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth << 1, texHeight << 1, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderbufferHiRes, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
#endif

        float w                    = (((((GFX_LINESIZE >> 16) * 65536.0) + GFX_LINESIZE) + 0.5) * textureList[0].widthN) - 0.001;
        float h                    = (SCREEN_YSIZE * textureList[0].heightN) - 0.001;
        screenBufferVertexList[0]  = -1.0;
        screenBufferVertexList[1]  = 1.0;
        screenBufferVertexList[2]  = 1.0;
        screenBufferVertexList[6]  = 0.0;
        screenBufferVertexList[9]  = 1.0;
        screenBufferVertexList[10] = 1.0;
        screenBufferVertexList[11] = 1.0;
        screenBufferVertexList[7]  = h;
        screenBufferVertexList[16] = h;
        screenBufferVertexList[18] = -1.0;
        screenBufferVertexList[19] = -1.0;
        screenBufferVertexList[20] = 1.0;
        screenBufferVertexList[24] = 0.0;
        screenBufferVertexList[25] = 0.0;
        screenBufferVertexList[27] = 1.0;
        screenBufferVertexList[28] = -1.0;
        screenBufferVertexList[29] = 1.0;
        screenBufferVertexList[15] = w;
        screenBufferVertexList[34] = 0.0;
        screenBufferVertexList[33] = w;
    }
    else {
#if RETRO_USING_OPENGL
        if (framebufferHiRes != -1)
            glDeleteFramebuffers(1, &framebufferHiRes);
        if (renderbufferHiRes != -1)
            glDeleteTextures(1, &renderbufferHiRes);

        framebufferHiRes  = -1;
        renderbufferHiRes = -1;
#endif
    }

    bool transfer = false;
#if RETRO_USING_OPENGL
    if (textureList[0].id != -1) {
        glDeleteTextures(1, &textureList[0].id);
        transfer = true;
    }
    glGenTextures(1, &textureList[0].id);
    glBindTexture(GL_TEXTURE_2D, textureList[0].id);
#endif

    convertTo32Bit = true;
#if RETRO_USING_OPENGL
    if (displaySettings.height > 720) {
        convertTo32Bit = true;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }
    else if (convertTo32Bit)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, 0);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_LINEAR
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
#endif

    if (transfer && Engine.frameBuffer)
        TransferRetroBuffer();
}

void setFullScreen(bool fs)
{
    if (fs) {
#if RETRO_USING_SDL1
        Engine.windowSurface =
            SDL_SetVideoMode(SCREEN_XSIZE * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale, 16, SDL_SWSURFACE | SDL_FULLSCREEN);
        SDL_ShowCursor(SDL_FALSE);
#elif RETRO_USING_SDL2
        SDL_RestoreWindow(Engine.window);
        SDL_SetWindowFullscreen(Engine.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_ShowCursor(SDL_FALSE);

#if RETRO_USING_OPENGL
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(0, &mode);
        //SetScreenDimensions(mode.w, mode.h);
        setupViewport();
        int w = mode.w;
        int h = mode.h;
        if (mode.h > mode.w) {
            w = mode.h;
            h = mode.w;
        }

        glViewport(0, 0, w, h);
#endif
#endif
    }
    else {
#if RETRO_USING_SDL1 
        Engine.windowSurface =
            SDL_SetVideoMode(SCREEN_XSIZE * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale, 16, SDL_SWSURFACE);
        SDL_ShowCursor(SDL_TRUE);
#elif RETRO_USING_SDL2
        SDL_SetWindowFullscreen(Engine.window, false);
        SDL_ShowCursor(SDL_TRUE);
        SDL_SetWindowSize(Engine.window, SCREEN_XSIZE_CONFIG * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale);
        SDL_SetWindowPosition(Engine.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_RestoreWindow(Engine.window);

        //SetScreenDimensions(SCREEN_XSIZE_CONFIG * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale);
        setupViewport();
#if RETRO_USING_OPENGL
        glViewport(0, 0, SCREEN_XSIZE_CONFIG * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale);
#endif
#endif
    }
    Engine.isFullScreen = fs;
}

void DrawObjectList(int Layer)
{
    int size = drawListEntries[Layer].listSize;
    for (int i = 0; i < size; ++i) {
        objectEntityPos = drawListEntries[Layer].entityRefs[i];
        int type        = objectEntityList[objectEntityPos].type;
        if (type) {
            if (scriptData[objectScriptList[type].eventDraw.scriptCodePtr] > 0)
                ProcessScript(objectScriptList[type].eventDraw.scriptCodePtr, objectScriptList[type].eventDraw.jumpTablePtr, EVENT_DRAW);
        }
    }
}
void DrawStageGFX()
{
    waterDrawPos = waterLevel - yScrollOffset;
#if RETRO_HARDWARE_RENDER
    gfxVertexSize = 0;
    gfxIndexSize  = 0;

    if (waterDrawPos < -TILE_SIZE)
        waterDrawPos = -TILE_SIZE;
    if (waterDrawPos >= SCREEN_YSIZE)
        waterDrawPos = SCREEN_YSIZE + TILE_SIZE;
#endif

#if RETRO_SOFTWARE_RENDER
    if (waterDrawPos < 0)
        waterDrawPos = 0;

    if (waterDrawPos > SCREEN_YSIZE)
        waterDrawPos = SCREEN_YSIZE;
#endif

    if (tLayerMidPoint < 3) {
        DrawObjectList(0);
        if (activeTileLayers[0] < LAYER_COUNT) {
            switch (stageLayouts[activeTileLayers[0]].type) {
                case LAYER_HSCROLL: DrawHLineScrollLayer(0); break;
                case LAYER_VSCROLL: DrawVLineScrollLayer(0); break;
                case LAYER_3DFLOOR:
#if RETRO_SOFTWARE_RENDER
                    drawStageGFXHQ = false;
#endif
                    Draw3DFloorLayer(0);
                    break;
                case LAYER_3DSKY:
#if RETRO_SOFTWARE_RENDER
#if !RETRO_USE_ORIGINAL_CODE
                    if (Engine.useHQModes)
#endif
                        drawStageGFXHQ = true;
                    Draw3DSkyLayer(0);
#endif
#if RETRO_HARDWARE_RENDER
                    Draw3DFloorLayer(0);
#endif
                    break;
                default: break;
            }
        }
#if RETRO_HARDWARE_RENDER
        gfxIndexSizeOpaque  = gfxIndexSize;
        gfxVertexSizeOpaque = gfxVertexSize;
#endif

        DrawObjectList(1);
        if (activeTileLayers[1] < LAYER_COUNT) {
            switch (stageLayouts[activeTileLayers[1]].type) {
                case LAYER_HSCROLL: DrawHLineScrollLayer(1); break;
                case LAYER_VSCROLL: DrawVLineScrollLayer(1); break;
                case LAYER_3DFLOOR:
#if RETRO_SOFTWARE_RENDER
                    drawStageGFXHQ = false;
#endif
                    Draw3DFloorLayer(1);
                    break;
                case LAYER_3DSKY:
#if RETRO_SOFTWARE_RENDER
#if !RETRO_USE_ORIGINAL_CODE
                    if (Engine.useHQModes)
#endif
                        drawStageGFXHQ = true;
                    Draw3DSkyLayer(1);
#endif
#if RETRO_HARDWARE_RENDER
                    Draw3DFloorLayer(1);
#endif
                    break;
                default: break;
            }
        }

        DrawObjectList(2);
        DrawObjectList(3);
        DrawObjectList(4);
        if (activeTileLayers[2] < LAYER_COUNT) {
            switch (stageLayouts[activeTileLayers[2]].type) {
                case LAYER_HSCROLL: DrawHLineScrollLayer(2); break;
                case LAYER_VSCROLL: DrawVLineScrollLayer(2); break;
                case LAYER_3DFLOOR:
#if RETRO_SOFTWARE_RENDER
                    drawStageGFXHQ = false;
#endif
                    Draw3DFloorLayer(2);
                    break;
                case LAYER_3DSKY:
#if RETRO_SOFTWARE_RENDER
#if !RETRO_USE_ORIGINAL_CODE
                    if (Engine.useHQModes)
#endif
                        drawStageGFXHQ = true;
                    Draw3DSkyLayer(2);
#endif
#if RETRO_HARDWARE_RENDER
                    Draw3DFloorLayer(2);
#endif
                    break;
                default: break;
            }
        }
    }
    else if (tLayerMidPoint < 6) {
        DrawObjectList(0);
        if (activeTileLayers[0] < LAYER_COUNT) {
            switch (stageLayouts[activeTileLayers[0]].type) {
                case LAYER_HSCROLL: DrawHLineScrollLayer(0); break;
                case LAYER_VSCROLL: DrawVLineScrollLayer(0); break;
                case LAYER_3DFLOOR:
#if RETRO_SOFTWARE_RENDER
                    drawStageGFXHQ = false;
#endif
                    Draw3DFloorLayer(0);
                    break;
                case LAYER_3DSKY:
#if RETRO_SOFTWARE_RENDER
#if !RETRO_USE_ORIGINAL_CODE
                    if (Engine.useHQModes)
#endif
                        drawStageGFXHQ = true;
                    Draw3DSkyLayer(0);
#endif
#if RETRO_HARDWARE_RENDER
                    Draw3DFloorLayer(0);
#endif
                    break;
                default: break;
            }
        }
#if RETRO_HARDWARE_RENDER
        gfxIndexSizeOpaque  = gfxIndexSize;
        gfxVertexSizeOpaque = gfxVertexSize;
#endif

        DrawObjectList(1);
        if (activeTileLayers[1] < LAYER_COUNT) {
            switch (stageLayouts[activeTileLayers[1]].type) {
                case LAYER_HSCROLL: DrawHLineScrollLayer(1); break;
                case LAYER_VSCROLL: DrawVLineScrollLayer(1); break;
                case LAYER_3DFLOOR:
#if RETRO_SOFTWARE_RENDER
                    drawStageGFXHQ = false;
#endif
                    Draw3DFloorLayer(1);
                    break;
                case LAYER_3DSKY:
#if RETRO_SOFTWARE_RENDER
#if !RETRO_USE_ORIGINAL_CODE
                    if (Engine.useHQModes)
#endif
                        drawStageGFXHQ = true;
                    Draw3DSkyLayer(1);
#endif
#if RETRO_HARDWARE_RENDER
                    Draw3DFloorLayer(1);
#endif
                    break;
                default: break;
            }
        }

        DrawObjectList(2);
        if (activeTileLayers[2] < LAYER_COUNT) {
            switch (stageLayouts[activeTileLayers[2]].type) {
                case LAYER_HSCROLL: DrawHLineScrollLayer(2); break;
                case LAYER_VSCROLL: DrawVLineScrollLayer(2); break;
                case LAYER_3DFLOOR:
#if RETRO_SOFTWARE_RENDER
                    drawStageGFXHQ = false;
#endif
                    Draw3DFloorLayer(2);
                    break;
                case LAYER_3DSKY:
#if RETRO_SOFTWARE_RENDER
#if !RETRO_USE_ORIGINAL_CODE
                    if (Engine.useHQModes)
#endif
                        drawStageGFXHQ = true;
                    Draw3DSkyLayer(2);
#endif
#if RETRO_HARDWARE_RENDER
                    Draw3DFloorLayer(2);
#endif
                    break;
                default: break;
            }
        }
        DrawObjectList(3);
        DrawObjectList(4);
    }

    if (tLayerMidPoint < 6) {
        if (activeTileLayers[3] < LAYER_COUNT) {
            switch (stageLayouts[activeTileLayers[3]].type) {
                case LAYER_HSCROLL: DrawHLineScrollLayer(3); break;
                case LAYER_VSCROLL: DrawVLineScrollLayer(3); break;
                case LAYER_3DFLOOR:
#if RETRO_SOFTWARE_RENDER
                    drawStageGFXHQ = false;
#endif
                    Draw3DFloorLayer(3);
                    break;
                case LAYER_3DSKY:
#if RETRO_SOFTWARE_RENDER
#if !RETRO_USE_ORIGINAL_CODE
                    if (Engine.useHQModes)
#endif
                        drawStageGFXHQ = true;
                    Draw3DSkyLayer(3);
#endif
#if RETRO_HARDWARE_RENDER
                    Draw3DFloorLayer(3);
#endif
                    break;
                default: break;
            }
        }

        DrawObjectList(5);
        DrawObjectList(6);
    }

#if RETRO_SOFTWARE_RENDER
    if (drawStageGFXHQ) {
        CopyFrameOverlay2x();
        if (fadeMode > 0) {
            DrawRectangle(0, 0, SCREEN_XSIZE, SCREEN_YSIZE, fadeR, fadeG, fadeB, fadeA);
            SetFadeHQ(fadeR, fadeG, fadeB, fadeA);
        }
    }
    else {
        if (fadeMode > 0) {
            DrawRectangle(0, 0, SCREEN_XSIZE, SCREEN_YSIZE, fadeR, fadeG, fadeB, fadeA);
        }
    }
#endif

#if RETRO_HARDWARE_RENDER
    if (fadeMode > 0) {
        DrawRectangle(0, 0, SCREEN_XSIZE, SCREEN_YSIZE, fadeR, fadeG, fadeB, fadeA);
    }
#endif

#if !RETRO_USE_ORIGINAL_CODE
    if (Engine.showPaletteOverlay) {
        for (int p = 0; p < PALETTE_COUNT; ++p) {
            int x = (SCREEN_XSIZE - (0xF << 3));
            int y = (SCREEN_YSIZE - (0xF << 2));
            for (int c = 0; c < PALETTE_SIZE; ++c) {
                DrawRectangle(x + ((c & 0xF) << 1) + ((p % (PALETTE_COUNT / 2)) * (2 * 16)),
                              y + ((c >> 4) << 1) + ((p / (PALETTE_COUNT / 2)) * (2 * 16)), 2, 2, fullPalette32[p][c].r, fullPalette32[p][c].g,
                              fullPalette32[p][c].b, 0xFF);
            }
        }
    }
#endif
}

void DrawHLineScrollLayer(int layerID)
{
#if RETRO_SOFTWARE_RENDER
    TileLayer *layer   = &stageLayouts[activeTileLayers[layerID]];
    int screenwidth16  = (GFX_LINESIZE >> 4) - 1;
    int layerwidth     = layer->width;
    int layerheight    = layer->height;
    bool aboveMidPoint = layerID >= tLayerMidPoint;

    byte *lineScroll;
    int *deformationData;
    int *deformationDataW;

    int yscrollOffset = 0;
    if (activeTileLayers[layerID]) { // BG Layer
        int yScroll    = yScrollOffset * layer->parallaxFactor >> 8;
        int fullheight = layerheight << 7;
        layer->scrollPos += layer->scrollSpeed;
        if (layer->scrollPos > fullheight << 16)
            layer->scrollPos -= fullheight << 16;
        yscrollOffset    = (yScroll + (layer->scrollPos >> 16)) % fullheight;
        layerheight      = fullheight >> 7;
        lineScroll       = layer->lineScroll;
        deformationData  = &bgDeformationData2[(byte)(yscrollOffset + layer->deformationOffset)];
        deformationDataW = &bgDeformationData3[(byte)(yscrollOffset + waterDrawPos + layer->deformationOffsetW)];
    }
    else { // FG Layer
        lastXSize     = layer->width;
        yscrollOffset = yScrollOffset;
        lineScroll    = layer->lineScroll;
        for (int i = 0; i < PARALLAX_COUNT; ++i) hParallax.linePos[i] = xScrollOffset;
        deformationData  = &bgDeformationData0[(byte)(yscrollOffset + layer->deformationOffset)];
        deformationDataW = &bgDeformationData1[(byte)(yscrollOffset + waterDrawPos + layer->deformationOffsetW)];
    }

    if (layer->type == LAYER_HSCROLL) {
        if (lastXSize != layerwidth) {
            int fullLayerwidth = layerwidth << 7;
            for (int i = 0; i < hParallax.entryCount; ++i) {
                hParallax.linePos[i] = xScrollOffset * hParallax.parallaxFactor[i] >> 8;
                if (hParallax.scrollPos[i] > fullLayerwidth << 16)
                    hParallax.scrollPos[i] -= fullLayerwidth << 16;
                if (hParallax.scrollPos[i] < 0)
                    hParallax.scrollPos[i] += fullLayerwidth << 16;
                hParallax.linePos[i] += hParallax.scrollPos[i] >> 16;
                hParallax.linePos[i] %= fullLayerwidth;
            }
        }
        int w = -1;
        if (activeTileLayers[layerID])
            w = layerwidth;
        lastXSize = w;
    }

    ushort *frameBufferPtr = Engine.frameBuffer;
    byte *lineBuffer       = gfxLineBuffer;
    int tileYPos           = yscrollOffset % (layerheight << 7);
    if (tileYPos < 0)
        tileYPos += layerheight << 7;
    byte *scrollIndex = &lineScroll[tileYPos];
    int tileY16       = tileYPos & 0xF;
    int chunkY        = tileYPos >> 7;
    int tileY         = (tileYPos & 0x7F) >> 4;

    // Draw Above Water (if applicable)
    int drawableLines[2] = { waterDrawPos, SCREEN_YSIZE - waterDrawPos };
    for (int i = 0; i < 2; ++i) {
        while (drawableLines[i]--) {
            activePalette   = fullPalette[*lineBuffer];
            activePalette32 = fullPalette32[*lineBuffer];
            lineBuffer++;
            int chunkX = hParallax.linePos[*scrollIndex];
            if (i == 0) {
                if (hParallax.deform[*scrollIndex])
                    chunkX += *deformationData;
                ++deformationData;
            }
            else {
                if (hParallax.deform[*scrollIndex])
                    chunkX += *deformationDataW;
                ++deformationDataW;
            }
            ++scrollIndex;
            int fullLayerwidth = layerwidth << 7;
            if (chunkX < 0)
                chunkX += fullLayerwidth;
            if (chunkX >= fullLayerwidth)
                chunkX -= fullLayerwidth;
            int chunkXPos         = chunkX >> 7;
            int tilePxXPos        = chunkX & 0xF;
            int tileXPxRemain     = TILE_SIZE - tilePxXPos;
            int chunk             = (layer->tiles[(chunkX >> 7) + (chunkY << 8)] << 6) + ((chunkX & 0x7F) >> 4) + 8 * tileY;
            int tileOffsetY       = TILE_SIZE * tileY16;
            int tileOffsetYFlipX  = TILE_SIZE * tileY16 + 0xF;
            int tileOffsetYFlipY  = TILE_SIZE * (0xF - tileY16);
            int tileOffsetYFlipXY = TILE_SIZE * (0xF - tileY16) + 0xF;
            int lineRemain        = GFX_LINESIZE;

            byte *gfxDataPtr  = NULL;
            int tilePxLineCnt = 0;

            // Draw the first tile to the left
            if (tiles128x128.visualPlane[chunk] == (byte)aboveMidPoint) {
                tilePxLineCnt = TILE_SIZE - tilePxXPos;
                lineRemain -= tilePxLineCnt;
                switch (tiles128x128.direction[chunk]) {
                    case FLIP_NONE:
                        gfxDataPtr = &tilesetGFXData[tileOffsetY + tiles128x128.gfxDataPos[chunk] + tilePxXPos];
                        while (tilePxLineCnt--) {
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                        }
                        break;
                    case FLIP_X:
                        gfxDataPtr = &tilesetGFXData[tileOffsetYFlipX + tiles128x128.gfxDataPos[chunk] - tilePxXPos];
                        while (tilePxLineCnt--) {
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                        }
                        break;
                    case FLIP_Y:
                        gfxDataPtr = &tilesetGFXData[tileOffsetYFlipY + tiles128x128.gfxDataPos[chunk] + tilePxXPos];
                        while (tilePxLineCnt--) {
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                        }
                        break;
                    case FLIP_XY:
                        gfxDataPtr = &tilesetGFXData[tileOffsetYFlipXY + tiles128x128.gfxDataPos[chunk] - tilePxXPos];
                        while (tilePxLineCnt--) {
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                        }
                        break;
                    default: break;
                }
            }
            else {
                frameBufferPtr += tileXPxRemain;
                lineRemain -= tileXPxRemain;
            }

            // Draw the bulk of the tiles
            int chunkTileX   = ((chunkX & 0x7F) >> 4) + 1;
            int tilesPerLine = screenwidth16;
            while (tilesPerLine--) {
                if (chunkTileX <= 7) {
                    ++chunk;
                }
                else {
                    if (++chunkXPos == layerwidth)
                        chunkXPos = 0;
                    chunkTileX = 0;
                    chunk      = (layer->tiles[chunkXPos + (chunkY << 8)] << 6) + 8 * tileY;
                }
                lineRemain -= TILE_SIZE;

                // Loop Unrolling (faster but messier code)
                if (tiles128x128.visualPlane[chunk] == (byte)aboveMidPoint) {
                    switch (tiles128x128.direction[chunk]) {
                        case FLIP_NONE:
                            gfxDataPtr = &tilesetGFXData[tiles128x128.gfxDataPos[chunk] + tileOffsetY];
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            break;
                        case FLIP_X:
                            gfxDataPtr = &tilesetGFXData[tiles128x128.gfxDataPos[chunk] + tileOffsetYFlipX];
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            break;
                        case FLIP_Y:
                            gfxDataPtr = &tilesetGFXData[tiles128x128.gfxDataPos[chunk] + tileOffsetYFlipY];
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            ++gfxDataPtr;
                            break;
                        case FLIP_XY:
                            gfxDataPtr = &tilesetGFXData[tiles128x128.gfxDataPos[chunk] + tileOffsetYFlipXY];
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            ++frameBufferPtr;
                            --gfxDataPtr;
                            break;
                    }
                }
                else {
                    frameBufferPtr += 0x10;
                }
                ++chunkTileX;
            }

            // Draw any remaining tiles
            while (lineRemain > 0) {
                if (chunkTileX++ <= 7) {
                    ++chunk;
                }
                else {
                    chunkTileX = 0;
                    if (++chunkXPos == layerwidth)
                        chunkXPos = 0;
                    chunk = (layer->tiles[chunkXPos + (chunkY << 8)] << 6) + 8 * tileY;
                }

                tilePxLineCnt = lineRemain >= TILE_SIZE ? TILE_SIZE : lineRemain;
                lineRemain -= tilePxLineCnt;
                if (tiles128x128.visualPlane[chunk] == (byte)aboveMidPoint) {
                    switch (tiles128x128.direction[chunk]) {
                        case FLIP_NONE:
                            gfxDataPtr = &tilesetGFXData[tiles128x128.gfxDataPos[chunk] + tileOffsetY];
                            while (tilePxLineCnt--) {
                                if (*gfxDataPtr > 0)
                                    *frameBufferPtr = activePalette[*gfxDataPtr];
                                ++frameBufferPtr;
                                ++gfxDataPtr;
                            }
                            break;
                        case FLIP_X:
                            gfxDataPtr = &tilesetGFXData[tiles128x128.gfxDataPos[chunk] + tileOffsetYFlipX];
                            while (tilePxLineCnt--) {
                                if (*gfxDataPtr > 0)
                                    *frameBufferPtr = activePalette[*gfxDataPtr];
                                ++frameBufferPtr;
                                --gfxDataPtr;
                            }
                            break;
                        case FLIP_Y:
                            gfxDataPtr = &tilesetGFXData[tiles128x128.gfxDataPos[chunk] + tileOffsetYFlipY];
                            while (tilePxLineCnt--) {
                                if (*gfxDataPtr > 0)
                                    *frameBufferPtr = activePalette[*gfxDataPtr];
                                ++frameBufferPtr;
                                ++gfxDataPtr;
                            }
                            break;
                        case FLIP_XY:
                            gfxDataPtr = &tilesetGFXData[tiles128x128.gfxDataPos[chunk] + tileOffsetYFlipXY];
                            while (tilePxLineCnt--) {
                                if (*gfxDataPtr > 0)
                                    *frameBufferPtr = activePalette[*gfxDataPtr];
                                ++frameBufferPtr;
                                --gfxDataPtr;
                            }
                            break;
                        default: break;
                    }
                }
                else {
                    frameBufferPtr += tilePxLineCnt;
                }
            }

            if (++tileY16 > TILE_SIZE - 1) {
                tileY16 = 0;
                ++tileY;
            }
            if (tileY > 7) {
                if (++chunkY == layerheight) {
                    chunkY = 0;
                    scrollIndex -= 0x80 * layerheight;
                }
                tileY = 0;
            }
        }
    }
#endif

#if RETRO_HARDWARE_RENDER
    TileLayer *layer      = &stageLayouts[activeTileLayers[layerID]];
    byte *lineScrollPtr   = NULL;
    int chunkPosX         = 0;
    int chunkTileX        = 0;
    int gfxIndex          = 0;
    int yscrollOffset     = 0;
    int tileGFXPos        = 0;
    int deformX1          = 0;
    int deformX2          = 0;
    byte highPlane        = layerID >= tLayerMidPoint;
    int *deformationData  = NULL;
    int *deformationDataW = NULL;
    int deformOffset      = 0;
    int deformOffsetW     = 0;
    int lineID            = 0;
    int layerWidth        = layer->width;
    int layerHeight       = layer->height;
    int renderWidth       = (GFX_LINESIZE >> 4) + 3;
    bool flag             = false;

    if (activeTileLayers[layerID]) {
        layer            = &stageLayouts[activeTileLayers[layerID]];
        yscrollOffset    = layer->parallaxFactor * yScrollOffset >> 8;
        layerHeight      = layerHeight << 7;
        layer->scrollPos = layer->scrollPos + layer->scrollSpeed;
        if (layer->scrollPos > layerHeight << 16) {
            layer->scrollPos -= (layerHeight << 16);
        }
        yscrollOffset += (layer->scrollPos >> 16);
        yscrollOffset %= layerHeight;

        layerHeight      = layerHeight >> 7;
        lineScrollPtr    = layer->lineScroll;
        deformOffset     = (byte)(layer->deformationOffset + yscrollOffset);
        deformOffsetW    = (byte)(layer->deformationOffsetW + yscrollOffset);
        deformationData  = bgDeformationData2;
        deformationDataW = bgDeformationData3;
    }
    else {
        layer                = &stageLayouts[0];
        lastXSize            = layerWidth;
        yscrollOffset        = yScrollOffset;
        lineScrollPtr        = layer->lineScroll;
        hParallax.linePos[0] = xScrollOffset;
        deformOffset         = (byte)(stageLayouts[0].deformationOffset + yscrollOffset);
        deformOffsetW        = (byte)(stageLayouts[0].deformationOffsetW + yscrollOffset);
        deformationData      = bgDeformationData0;
        deformationDataW     = bgDeformationData1;
        yscrollOffset %= (layerHeight << 7);
    }

    if (layer->type == LAYER_HSCROLL) {
        if (lastXSize != layerWidth) {
            layerWidth = layerWidth << 7;
            for (int i = 0; i < hParallax.entryCount; i++) {
                hParallax.linePos[i]   = hParallax.parallaxFactor[i] * xScrollOffset >> 8;
                hParallax.scrollPos[i] = hParallax.scrollPos[i] + hParallax.scrollSpeed[i];
                if (hParallax.scrollPos[i] > layerWidth << 16) {
                    hParallax.scrollPos[i] = hParallax.scrollPos[i] - (layerWidth << 16);
                }
                hParallax.linePos[i] = hParallax.linePos[i] + (hParallax.scrollPos[i] >> 16);
                hParallax.linePos[i] = hParallax.linePos[i] % layerWidth;
            }
            layerWidth = layerWidth >> 7;
        }
        lastXSize = layerWidth;
    }

    if (yscrollOffset < 0)
        yscrollOffset += (layerHeight << 7);

    int deformY = yscrollOffset >> 4 << 4;
    lineID += deformY;
    deformOffset += (deformY - yscrollOffset);
    deformOffsetW += (deformY - yscrollOffset);

    if (deformOffset < 0)
        deformOffset += 0x100;
    if (deformOffsetW < 0)
        deformOffsetW += 0x100;

    deformY        = -(yscrollOffset & 15);
    int chunkPosY  = yscrollOffset >> 7;
    int chunkTileY = (yscrollOffset & 127) >> 4;
    waterDrawPos <<= 4;
    deformY <<= 4;
    for (int j = (deformY ? 0x110 : 0x100); j > 0; j -= 16) {
        int parallaxLinePos = hParallax.linePos[lineScrollPtr[lineID]] - 16;
        lineID += 8;

        if (parallaxLinePos == hParallax.linePos[lineScrollPtr[lineID]] - 16) {
            if (hParallax.deform[lineScrollPtr[lineID]]) {
                deformX1 = deformY < waterDrawPos ? deformationData[deformOffset] : deformationDataW[deformOffsetW];
                deformX2 = (deformY + 64) <= waterDrawPos ? deformationData[deformOffset + 8] : deformationDataW[deformOffsetW + 8];
                flag     = deformX1 != deformX2;
            }
            else {
                flag = false;
            }
        }
        else {
            flag = true;
        }

        lineID -= 8;
        if (flag) {
            if (parallaxLinePos < 0)
                parallaxLinePos += layerWidth << 7;
            if (parallaxLinePos >= layerWidth << 7)
                parallaxLinePos -= layerWidth << 7;

            chunkPosX  = parallaxLinePos >> 7;
            chunkTileX = (parallaxLinePos & 0x7F) >> 4;
            deformX1   = -((parallaxLinePos & 0xF) << 4);
            deformX1 -= 0x100;
            deformX2 = deformX1;
            if (hParallax.deform[lineScrollPtr[lineID]]) {
                deformX1 -= deformY < waterDrawPos ? deformationData[deformOffset] : deformationDataW[deformOffsetW];
                deformOffset += 8;
                deformOffsetW += 8;
                deformX2 -= (deformY + 64) <= waterDrawPos ? deformationData[deformOffset] : deformationDataW[deformOffsetW];
            }
            else {
                deformOffset += 8;
                deformOffsetW += 8;
            }
            lineID += 8;

            gfxIndex = (chunkPosX > -1 && chunkPosY > -1) ? (layer->tiles[chunkPosX + (chunkPosY << 8)] << 6) : 0;
            gfxIndex += chunkTileX + (chunkTileY << 3);
            for (int i = renderWidth; i > 0; i--) {
                if (tiles128x128.visualPlane[gfxIndex] == highPlane && tiles128x128.gfxDataPos[gfxIndex] > 0) {
                    tileGFXPos = 0;
                    switch (tiles128x128.direction[gfxIndex]) {
                        case FLIP_NONE: {
                            gfxPolyList[gfxVertexSize].x = deformX1;
                            gfxPolyList[gfxVertexSize].y = deformY;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x = deformX1 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y = deformY;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX2;
                            gfxPolyList[gfxVertexSize].y        = deformY + CHUNK_SIZE;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos] - 8;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX2 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxIndexSize += 6;
                            break;
                        }
                        case FLIP_X: {
                            gfxPolyList[gfxVertexSize].x = deformX1 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y = deformY;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x = deformX1;
                            gfxPolyList[gfxVertexSize].y = deformY;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX2 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y        = deformY + CHUNK_SIZE;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos] - 8;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX2;
                            gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxIndexSize += 6;
                            break;
                        }
                        case FLIP_Y: {
                            gfxPolyList[gfxVertexSize].x = deformX2;
                            gfxPolyList[gfxVertexSize].y = deformY + CHUNK_SIZE;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos] + 8;
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x = deformX2 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y = deformY + CHUNK_SIZE;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX1;
                            gfxPolyList[gfxVertexSize].y        = deformY;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX1 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxIndexSize += 6;
                            break;
                        }
                        case FLIP_XY: {
                            gfxPolyList[gfxVertexSize].x = deformX2 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y = deformY + CHUNK_SIZE;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos] + 8;
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x = deformX2;
                            gfxPolyList[gfxVertexSize].y = deformY + CHUNK_SIZE;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX1 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y        = deformY;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX1;
                            gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxIndexSize += 6;
                            break;
                        }
                    }
                }

                deformX1 += (CHUNK_SIZE * 2);
                deformX2 += (CHUNK_SIZE * 2);
                if (++chunkTileX < 8) {
                    gfxIndex++;
                }
                else {
                    if (++chunkPosX == layerWidth)
                        chunkPosX = 0;

                    chunkTileX = 0;
                    gfxIndex   = layer->tiles[chunkPosX + (chunkPosY << 8)] << 6;
                    gfxIndex += chunkTileX + (chunkTileY << 3);
                }
            }
            deformY += CHUNK_SIZE;
            parallaxLinePos = hParallax.linePos[lineScrollPtr[lineID]] - 16;

            if (parallaxLinePos < 0)
                parallaxLinePos += layerWidth << 7;
            if (parallaxLinePos >= layerWidth << 7)
                parallaxLinePos -= layerWidth << 7;

            chunkPosX  = parallaxLinePos >> 7;
            chunkTileX = (parallaxLinePos & 127) >> 4;
            deformX1   = -((parallaxLinePos & 15) << 4);
            deformX1 -= 0x100;
            deformX2 = deformX1;
            if (!hParallax.deform[lineScrollPtr[lineID]]) {
                deformOffset += 8;
                deformOffsetW += 8;
            }
            else {
                deformX1 -= deformY < waterDrawPos ? deformationData[deformOffset] : deformationDataW[deformOffsetW];
                deformOffset += 8;
                deformOffsetW += 8;
                deformX2 -= (deformY + 64) <= waterDrawPos ? deformationData[deformOffset] : deformationDataW[deformOffsetW];
            }

            lineID += 8;
            gfxIndex = (chunkPosX > -1 && chunkPosY > -1) ? (layer->tiles[chunkPosX + (chunkPosY << 8)] << 6) : 0;
            gfxIndex += chunkTileX + (chunkTileY << 3);
            for (int i = renderWidth; i > 0; i--) {
                if (tiles128x128.visualPlane[gfxIndex] == highPlane && tiles128x128.gfxDataPos[gfxIndex] > 0) {
                    tileGFXPos = 0;
                    switch (tiles128x128.direction[gfxIndex]) {
                        case FLIP_NONE: {
                            gfxPolyList[gfxVertexSize].x = deformX1;
                            gfxPolyList[gfxVertexSize].y = deformY;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos] + 8;
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x = deformX1 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y = deformY;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX2;
                            gfxPolyList[gfxVertexSize].y        = deformY + CHUNK_SIZE;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX2 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxIndexSize += 6;
                            break;
                        }
                        case FLIP_X: {
                            gfxPolyList[gfxVertexSize].x = deformX1 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y = deformY;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos] + 8;
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x = deformX1;
                            gfxPolyList[gfxVertexSize].y = deformY;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX2 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y        = deformY + CHUNK_SIZE;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX2;
                            gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxIndexSize += 6;
                            break;
                        }
                        case FLIP_Y: {
                            gfxPolyList[gfxVertexSize].x = deformX2;
                            gfxPolyList[gfxVertexSize].y = deformY + CHUNK_SIZE;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x = deformX2 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y = deformY + CHUNK_SIZE;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX1;
                            gfxPolyList[gfxVertexSize].y        = deformY;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos] - 8;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX1 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxIndexSize += 6;
                            break;
                        }
                        case FLIP_XY: {
                            gfxPolyList[gfxVertexSize].x = deformX2 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y = deformY + CHUNK_SIZE;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x = deformX2;
                            gfxPolyList[gfxVertexSize].y = deformY + CHUNK_SIZE;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX1 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y        = deformY;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos] - 8;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX1;
                            gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxIndexSize += 6;
                            break;
                        }
                    }
                }

                deformX1 += (CHUNK_SIZE * 2);
                deformX2 += (CHUNK_SIZE * 2);

                if (++chunkTileX < 8) {
                    gfxIndex++;
                }
                else {
                    if (++chunkPosX == layerWidth) {
                        chunkPosX = 0;
                    }
                    chunkTileX = 0;
                    gfxIndex   = layer->tiles[chunkPosX + (chunkPosY << 8)] << 6;
                    gfxIndex += chunkTileX + (chunkTileY << 3);
                }
            }
            deformY += CHUNK_SIZE;
        }
        else {
            if (parallaxLinePos < 0)
                parallaxLinePos += layerWidth << 7;
            if (parallaxLinePos >= layerWidth << 7)
                parallaxLinePos -= layerWidth << 7;

            chunkPosX  = parallaxLinePos >> 7;
            chunkTileX = (parallaxLinePos & 0x7F) >> 4;
            deformX1   = -((parallaxLinePos & 0xF) << 4);
            deformX1 -= 0x100;
            deformX2 = deformX1;

            if (hParallax.deform[lineScrollPtr[lineID]]) {
                deformX1 -= deformY < waterDrawPos ? deformationData[deformOffset] : deformationDataW[deformOffsetW];
                deformOffset += 16;
                deformOffsetW += 16;
                deformX2 -= (deformY + CHUNK_SIZE <= waterDrawPos) ? deformationData[deformOffset] : deformationDataW[deformOffsetW];
            }
            else {
                deformOffset += 16;
                deformOffsetW += 16;
            }
            lineID += 16;

            gfxIndex = (chunkPosX > -1 && chunkPosY > -1) ? (layer->tiles[chunkPosX + (chunkPosY << 8)] << 6) : 0;
            gfxIndex += chunkTileX + (chunkTileY << 3);
            for (int i = renderWidth; i > 0; i--) {
                if (tiles128x128.visualPlane[gfxIndex] == highPlane && tiles128x128.gfxDataPos[gfxIndex] > 0) {
                    tileGFXPos = 0;
                    switch (tiles128x128.direction[gfxIndex]) {
                        case FLIP_NONE: {
                            gfxPolyList[gfxVertexSize].x = deformX1;
                            gfxPolyList[gfxVertexSize].y = deformY;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x = deformX1 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y = deformY;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX2;
                            gfxPolyList[gfxVertexSize].y        = deformY + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX2 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxIndexSize += 6;
                            break;
                        }
                        case FLIP_X: {
                            gfxPolyList[gfxVertexSize].x = deformX1 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y = deformY;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x = deformX1;
                            gfxPolyList[gfxVertexSize].y = deformY;
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX2 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y        = deformY + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX2;
                            gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxIndexSize += 6;
                            break;
                        }
                        case FLIP_Y: {
                            gfxPolyList[gfxVertexSize].x = deformX2;
                            gfxPolyList[gfxVertexSize].y = deformY + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x = deformX2 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y = deformY + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX1;
                            gfxPolyList[gfxVertexSize].y        = deformY;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX1 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxIndexSize += 6;
                            break;
                        }
                        case FLIP_XY: {
                            gfxPolyList[gfxVertexSize].x = deformX2 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y = deformY + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x = deformX2;
                            gfxPolyList[gfxVertexSize].y = deformY + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].u = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            tileGFXPos++;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX1 + (CHUNK_SIZE * 2);
                            gfxPolyList[gfxVertexSize].y        = deformY;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = tileUVArray[tiles128x128.gfxDataPos[gfxIndex] + tileGFXPos];
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxPolyList[gfxVertexSize].x        = deformX1;
                            gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                            gfxVertexSize++;

                            gfxIndexSize += 6;
                            break;
                        }
                    }
                }

                deformX1 += (CHUNK_SIZE * 2);
                deformX2 += (CHUNK_SIZE * 2);
                if (++chunkTileX < 8) {
                    gfxIndex++;
                }
                else {
                    if (++chunkPosX == layerWidth)
                        chunkPosX = 0;

                    chunkTileX = 0;
                    gfxIndex   = layer->tiles[chunkPosX + (chunkPosY << 8)] << 6;
                    gfxIndex += chunkTileX + (chunkTileY << 3);
                }
            }
            deformY += CHUNK_SIZE * 2;
        }

        if (++chunkTileY > 7) {
            if (++chunkPosY == layerHeight) {
                chunkPosY = 0;
                lineID -= (layerHeight << 7);
            }
            chunkTileY = 0;
        }
    }
    waterDrawPos >>= 4;
#endif
}
void DrawVLineScrollLayer(int layerID)
{
#if RETRO_SOFTWARE_RENDER
    TileLayer *layer   = &stageLayouts[activeTileLayers[layerID]];
    int layerwidth     = layer->width;
    int layerheight    = layer->height;
    bool aboveMidPoint = layerID >= tLayerMidPoint;

    byte *lineScroll;
    int *deformationData;

    int xscrollOffset = 0;
    if (activeTileLayers[layerID]) { // BG Layer
        int xScroll        = xScrollOffset * layer->parallaxFactor >> 8;
        int fullLayerwidth = layerwidth << 7;
        layer->scrollPos += layer->scrollSpeed;
        if (layer->scrollPos > fullLayerwidth << 16)
            layer->scrollPos -= fullLayerwidth << 16;
        xscrollOffset   = (xScroll + (layer->scrollPos >> 16)) % fullLayerwidth;
        layerwidth      = fullLayerwidth >> 7;
        lineScroll      = layer->lineScroll;
        deformationData = &bgDeformationData2[(byte)(xscrollOffset + layer->deformationOffset)];
    }
    else { // FG Layer
        lastYSize            = layer->height;
        xscrollOffset        = xScrollOffset;
        lineScroll           = layer->lineScroll;
        vParallax.linePos[0] = yScrollOffset;
        vParallax.deform[0]  = true;
        deformationData      = &bgDeformationData0[(byte)(xScrollOffset + layer->deformationOffset)];
    }

    if (layer->type == LAYER_VSCROLL) {
        if (lastYSize != layerheight) {
            int fullLayerheight = layerheight << 7;
            for (int i = 0; i < vParallax.entryCount; ++i) {
                vParallax.linePos[i] = xScrollOffset * vParallax.parallaxFactor[i] >> 8;
                if (vParallax.scrollPos[i] > fullLayerheight << 16)
                    vParallax.scrollPos[i] -= fullLayerheight << 16;
                if (vParallax.scrollPos[i] < 0)
                    vParallax.scrollPos[i] += vParallax.scrollPos[i] << 16;
                vParallax.linePos[i] += vParallax.scrollPos[i] >> 16;
                vParallax.linePos[i] %= fullLayerheight;
            }
            layerheight = fullLayerheight >> 7;
        }
        lastYSize = layerwidth;
    }

    ushort *frameBufferPtr = Engine.frameBuffer;
    activePalette          = fullPalette[gfxLineBuffer[0]];
    activePalette32        = fullPalette32[gfxLineBuffer[0]];
    int tileXPos           = xscrollOffset % (layerheight << 7);
    if (tileXPos < 0)
        tileXPos += layerheight << 7;
    byte *scrollIndex = &lineScroll[tileXPos];
    int tileX16       = tileXPos & 0xF;
    int tileX         = (tileXPos & 0x7F) >> 4;

    // Draw Above Water (if applicable)
    int drawableLines = waterDrawPos;
    while (drawableLines--) {
        int chunkY = vParallax.linePos[*scrollIndex];
        if (vParallax.deform[*scrollIndex])
            chunkY += *deformationData;
        ++deformationData;
        ++scrollIndex;
        int fullLayerHeight = layerheight << 7;
        if (chunkY < 0)
            chunkY += fullLayerHeight;
        if (chunkY >= fullLayerHeight)
            chunkY -= fullLayerHeight;
        int chunkYPos         = chunkY >> 7;
        int tileY             = chunkY & 0xF;
        int tileYPxRemain     = 0x10 - tileY;
        int chunk             = (layer->tiles[chunkY + (chunkY >> 7 << 8)] << 6) + tileX + 8 * ((chunkY & 0x7F) >> 4);
        int tileOffsetXFlipX  = 0xF - tileX16;
        int tileOffsetXFlipY  = tileX16 + SCREEN_YSIZE;
        int tileOffsetXFlipXY = 0xFF - tileX16;
        int lineRemain        = SCREEN_YSIZE;

        byte *gfxDataPtr  = NULL;
        int tilePxLineCnt = 0;

        // Draw the first tile to the left
        if (tiles128x128.visualPlane[chunk] == (byte)aboveMidPoint) {
            tilePxLineCnt = 0x10 - tileY;
            lineRemain -= tilePxLineCnt;
            switch (tiles128x128.direction[chunk]) {
                case FLIP_NONE:
                    gfxDataPtr = &tilesetGFXData[0x10 * tileY + tileX16 + tiles128x128.gfxDataPos[chunk]];
                    while (tilePxLineCnt--) {
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                    }
                    break;
                case FLIP_X:
                    gfxDataPtr = &tilesetGFXData[0x10 * tileY + tileOffsetXFlipX + tiles128x128.gfxDataPos[chunk]];
                    while (tilePxLineCnt--) {
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                    }
                    break;
                case FLIP_Y:
                    gfxDataPtr = &tilesetGFXData[tileOffsetXFlipY + tiles128x128.gfxDataPos[chunk] - 0x10 * tileY];
                    while (tilePxLineCnt--) {
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                    }
                    break;
                case FLIP_XY:
                    gfxDataPtr = &tilesetGFXData[tileOffsetXFlipXY + tiles128x128.gfxDataPos[chunk] - 16 * tileY];
                    while (tilePxLineCnt--) {
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                    }
                    break;
                default: break;
            }
        }
        else {
            frameBufferPtr += GFX_LINESIZE * tileYPxRemain;
            lineRemain -= tileYPxRemain;
        }

        // Draw the bulk of the tiles
        int chunkTileY   = ((chunkY & 0x7F) >> 4) + 1;
        int tilesPerLine = 14;
        while (tilesPerLine--) {
            if (chunkTileY <= 7) {
                chunk += 8;
            }
            else {
                if (++chunkYPos == layerheight)
                    chunkYPos = 0;
                chunkTileY = 0;
                chunk      = (layer->tiles[chunkY + (chunkYPos << 8)] << 6) + tileX;
            }
            lineRemain -= TILE_SIZE;

            // Loop Unrolling (faster but messier code)
            if (tiles128x128.visualPlane[chunk] == (byte)aboveMidPoint) {
                switch (tiles128x128.direction[chunk]) {
                    case FLIP_NONE:
                        gfxDataPtr = &tilesetGFXData[tiles128x128.gfxDataPos[chunk] + tileX16];
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        break;
                    case FLIP_X:
                        gfxDataPtr = &tilesetGFXData[tiles128x128.gfxDataPos[chunk] + tileOffsetXFlipX];
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr += 0x10;
                        break;
                    case FLIP_Y:
                        gfxDataPtr = &tilesetGFXData[tiles128x128.gfxDataPos[chunk] + tileOffsetXFlipY];
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        break;
                    case FLIP_XY:
                        gfxDataPtr = &tilesetGFXData[tiles128x128.gfxDataPos[chunk] + tileOffsetXFlipXY];
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        if (*gfxDataPtr > 0)
                            *frameBufferPtr = activePalette[*gfxDataPtr];
                        frameBufferPtr += GFX_LINESIZE;
                        gfxDataPtr -= 0x10;
                        break;
                }
            }
            else {
                frameBufferPtr += 0x10;
            }
            ++chunkTileY;
        }

        // Draw any remaining tiles
        while (lineRemain > 0) {
            if (chunkTileY++ <= 7) {
                chunk += 8;
            }
            else {
                chunkTileY = 0;
                if (++chunkYPos == layerheight)
                    chunkYPos = 0;
                chunkTileY = 0;
                chunk      = (layer->tiles[chunkY + (chunkYPos << 8)] << 6) + tileX;
            }

            tilePxLineCnt = lineRemain >= TILE_SIZE ? TILE_SIZE : lineRemain;
            lineRemain -= tilePxLineCnt;
            if (tiles128x128.visualPlane[chunk] == (byte)aboveMidPoint) {
                switch (tiles128x128.direction[chunk]) {
                    case FLIP_NONE:
                        gfxDataPtr = &tilesetGFXData[0x10 * tileY + tileX16 + tiles128x128.gfxDataPos[chunk]];
                        while (tilePxLineCnt--) {
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            frameBufferPtr += GFX_LINESIZE;
                            gfxDataPtr += 0x10;
                        }
                        break;
                    case FLIP_X:
                        gfxDataPtr = &tilesetGFXData[0x10 * tileY + tileOffsetXFlipX + tiles128x128.gfxDataPos[chunk]];
                        while (tilePxLineCnt--) {
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            frameBufferPtr += GFX_LINESIZE;
                            gfxDataPtr += 0x10;
                        }
                        break;
                    case FLIP_Y:
                        gfxDataPtr = &tilesetGFXData[tileOffsetXFlipY + tiles128x128.gfxDataPos[chunk] - 0x10 * tileY];
                        while (tilePxLineCnt--) {
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            frameBufferPtr += GFX_LINESIZE;
                            gfxDataPtr -= 0x10;
                        }
                        break;
                    case FLIP_XY:
                        gfxDataPtr = &tilesetGFXData[tileOffsetXFlipXY + tiles128x128.gfxDataPos[chunk] - 16 * tileY];
                        while (tilePxLineCnt--) {
                            if (*gfxDataPtr > 0)
                                *frameBufferPtr = activePalette[*gfxDataPtr];
                            frameBufferPtr += GFX_LINESIZE;
                            gfxDataPtr -= 0x10;
                        }
                        break;
                    default: break;
                }
            }
            else {
                frameBufferPtr += GFX_LINESIZE * tileYPxRemain;
            }
        }

        if (++tileX16 > 0xF) {
            tileX16 = 0;
            ++tileX;
        }
        if (tileX > 7) {
            if (++chunkY == layerwidth) {
                chunkY = 0;
                scrollIndex -= 0x80 * layerwidth;
            }
            tileX = 0;
        }
    }
#endif

#if RETRO_HARDWARE_RENDER
    // unimplimented in RSDK
#endif
}
void Draw3DFloorLayer(int layerID)
{
#if RETRO_SOFTWARE_RENDER
    TileLayer *layer       = &stageLayouts[activeTileLayers[layerID]];
    int layerWidth         = layer->width << 7;
    int layerHeight        = layer->height << 7;
    int layerYPos          = layer->YPos;
    int layerZPos          = layer->ZPos;
    int sinValue           = sinValM7[layer->angle];
    int cosValue           = cosValM7[layer->angle];
    byte *linePtr          = gfxLineBuffer;
    ushort *frameBufferPtr = &Engine.frameBuffer[132 * GFX_LINESIZE];
    int layerXPos          = layer->XPos >> 4;
    int ZBuffer            = layerZPos >> 4;
    for (int i = 4; i < 112; ++i) {
        if (!(i & 1)) {
            activePalette   = fullPalette[*linePtr];
            activePalette32 = fullPalette32[*linePtr];
            linePtr++;
        }
        int XBuffer    = layerYPos / (i << 9) * -cosValue >> 8;
        int YBuffer    = sinValue * (layerYPos / (i << 9)) >> 8;
        int XPos       = layerXPos + (3 * sinValue * (layerYPos / (i << 9)) >> 2) - XBuffer * SCREEN_CENTERX;
        int YPos       = ZBuffer + (3 * cosValue * (layerYPos / (i << 9)) >> 2) - YBuffer * SCREEN_CENTERX;
        int lineBuffer = 0;
        while (lineBuffer < GFX_LINESIZE) {
            int tileX = XPos >> 12;
            int tileY = YPos >> 12;
            if (tileX > -1 && tileX < layerWidth && tileY > -1 && tileY < layerHeight) {
                int chunk       = tile3DFloorBuffer[(YPos >> 16 << 8) + (XPos >> 16)];
                byte *tilePixel = &tilesetGFXData[tiles128x128.gfxDataPos[chunk]];
                switch (tiles128x128.direction[chunk]) {
                    case FLIP_NONE: tilePixel += 16 * (tileY & 0xF) + (tileX & 0xF); break;
                    case FLIP_X: tilePixel += 16 * (tileY & 0xF) + 15 - (tileX & 0xF); break;
                    case FLIP_Y: tilePixel += (tileX & 0xF) + SCREEN_YSIZE - 16 * (tileY & 0xF); break;
                    case FLIP_XY: tilePixel += 15 - (tileX & 0xF) + SCREEN_YSIZE - 16 * (tileY & 0xF); break;
                    default: break;
                }
                if (*tilePixel > 0)
                    *frameBufferPtr = activePalette[*tilePixel];
            }
            ++frameBufferPtr;
            ++lineBuffer;
            XPos += XBuffer;
            YPos += YBuffer;
        }
    }
#endif

#if RETRO_HARDWARE_RENDER
    TileLayer *layer = &stageLayouts[activeTileLayers[layerID]];
    int tileOffset, tileX, tileY, tileSinBlock, tileCosBlock;
    int sinValue512, cosValue512;
    int layerWidth         = layer->width << 7;
    int layerHeight        = layer->height << 7;
    ushort *currentTileMap = layer->tiles;
    vertexSize3D           = 0;
    indexSize3D            = 0;

    // low quality render
    polyList3D[vertexSize3D].x        = 0.0f;
    polyList3D[vertexSize3D].y        = 0.0f;
    polyList3D[vertexSize3D].z        = 0.0f;
    polyList3D[vertexSize3D].u        = 512;
    polyList3D[vertexSize3D].v        = 0;
    polyList3D[vertexSize3D].colour.r = 0xFF;
    polyList3D[vertexSize3D].colour.g = 0xFF;
    polyList3D[vertexSize3D].colour.b = 0xFF;
    polyList3D[vertexSize3D].colour.a = 0xFF;
    vertexSize3D++;

    polyList3D[vertexSize3D].x        = 4096.0f;
    polyList3D[vertexSize3D].y        = 0.0f;
    polyList3D[vertexSize3D].z        = 0.0f;
    polyList3D[vertexSize3D].u        = 1024;
    polyList3D[vertexSize3D].v        = 0;
    polyList3D[vertexSize3D].colour.r = 0xFF;
    polyList3D[vertexSize3D].colour.g = 0xFF;
    polyList3D[vertexSize3D].colour.b = 0xFF;
    polyList3D[vertexSize3D].colour.a = 0xFF;
    vertexSize3D++;

    polyList3D[vertexSize3D].x        = 0.0f;
    polyList3D[vertexSize3D].y        = 0.0f;
    polyList3D[vertexSize3D].z        = 4096.0f;
    polyList3D[vertexSize3D].u        = 512;
    polyList3D[vertexSize3D].v        = 512;
    polyList3D[vertexSize3D].colour.r = 0xFF;
    polyList3D[vertexSize3D].colour.g = 0xFF;
    polyList3D[vertexSize3D].colour.b = 0xFF;
    polyList3D[vertexSize3D].colour.a = 0xFF;
    vertexSize3D++;

    polyList3D[vertexSize3D].x        = 4096.0f;
    polyList3D[vertexSize3D].y        = 0.0f;
    polyList3D[vertexSize3D].z        = 4096.0f;
    polyList3D[vertexSize3D].u        = 1024;
    polyList3D[vertexSize3D].v        = 512;
    polyList3D[vertexSize3D].colour.r = 0xFF;
    polyList3D[vertexSize3D].colour.g = 0xFF;
    polyList3D[vertexSize3D].colour.b = 0xFF;
    polyList3D[vertexSize3D].colour.a = 0xFF;
    vertexSize3D++;

    indexSize3D += 6;
    if (hq3DFloorEnabled) {
        sinValue512 = (layer->XPos >> 16) - 0x100;
        sinValue512 += (sinVal512[layer->angle] >> 1);
        sinValue512 = sinValue512 >> 4 << 4;

        cosValue512 = (layer->ZPos >> 16) - 0x100;
        cosValue512 += (cosVal512[layer->angle] >> 1);
        cosValue512 = cosValue512 >> 4 << 4;
        for (int i = 32; i > 0; i--) {
            for (int j = 32; j > 0; j--) {
                if (sinValue512 > -1 && sinValue512 < layerWidth && cosValue512 > -1 && cosValue512 < layerHeight) {
                    tileX         = sinValue512 >> 7;
                    tileY         = cosValue512 >> 7;
                    tileSinBlock  = (sinValue512 & 127) >> 4;
                    tileCosBlock  = (cosValue512 & 127) >> 4;
                    int tileIndex = currentTileMap[tileX + (tileY << 8)] << 6;
                    tileIndex     = tileIndex + tileSinBlock + (tileCosBlock << 3);
                    if (tiles128x128.gfxDataPos[tileIndex] > 0) {
                        tileOffset = 0;
                        switch (tiles128x128.direction[tileIndex]) {
                            case FLIP_NONE: {
                                polyList3D[vertexSize3D].x = sinValue512;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = cosValue512;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x = sinValue512 + 16;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = cosValue512 + 16;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                indexSize3D += 6;
                                break;
                            }
                            case FLIP_X: {
                                polyList3D[vertexSize3D].x = sinValue512 + 16;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = cosValue512;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x = sinValue512;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = cosValue512 + 16;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                indexSize3D += 6;
                                break;
                            }
                            case 2: {
                                polyList3D[vertexSize3D].x = sinValue512;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = cosValue512 + 16;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x = sinValue512 + 16;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = cosValue512;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                indexSize3D += 6;
                                break;
                            }
                            case FLIP_XY: {
                                polyList3D[vertexSize3D].x = sinValue512 + 16;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = cosValue512 + 16;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x = sinValue512;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = cosValue512;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                indexSize3D += 6;
                                break;
                            }
                        }
                    }
                }
                sinValue512 += 16;
            }
            sinValue512 -= 0x200;
            cosValue512 += 16;
        }
    }
    else {
        sinValue512 = (layer->XPos >> 16) - 160;
        sinValue512 += sinVal512[layer->angle] / 3;
        sinValue512 = sinValue512 >> 4 << 4;

        cosValue512 = (layer->ZPos >> 16) - 160;
        cosValue512 += cosVal512[layer->angle] / 3;
        cosValue512 = cosValue512 >> 4 << 4;
        for (int i = 20; i > 0; i--) {
            for (int j = 20; j > 0; j--) {
                if (sinValue512 > -1 && sinValue512 < layerWidth && cosValue512 > -1 && cosValue512 < layerHeight) {
                    tileX         = sinValue512 >> 7;
                    tileY         = cosValue512 >> 7;
                    tileSinBlock  = (sinValue512 & 127) >> 4;
                    tileCosBlock  = (cosValue512 & 127) >> 4;
                    int tileIndex = currentTileMap[tileX + (tileY << 8)] << 6;
                    tileIndex     = tileIndex + tileSinBlock + (tileCosBlock << 3);
                    if (tiles128x128.gfxDataPos[tileIndex] > 0) {
                        tileOffset = 0;
                        switch (tiles128x128.direction[tileIndex]) {
                            case FLIP_NONE: {
                                polyList3D[vertexSize3D].x = sinValue512;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = cosValue512;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x = sinValue512 + 16;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = cosValue512 + 16;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                indexSize3D += 6;
                                break;
                            }
                            case FLIP_X: {
                                polyList3D[vertexSize3D].x = sinValue512 + 16;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = cosValue512;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x = sinValue512;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = cosValue512 + 16;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                indexSize3D += 6;
                                break;
                            }
                            case FLIP_Y: {
                                polyList3D[vertexSize3D].x = sinValue512;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = cosValue512 + 16;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x = sinValue512 + 16;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = cosValue512;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                indexSize3D += 6;
                                break;
                            }
                            case FLIP_XY: {
                                polyList3D[vertexSize3D].x = sinValue512 + 16;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = cosValue512 + 16;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x = sinValue512;
                                polyList3D[vertexSize3D].y = 0.0f;
                                polyList3D[vertexSize3D].z = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                tileOffset++;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = cosValue512;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = tileUVArray[tiles128x128.gfxDataPos[tileIndex] + tileOffset];
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                polyList3D[vertexSize3D].x        = polyList3D[vertexSize3D - 2].x;
                                polyList3D[vertexSize3D].y        = 0.0f;
                                polyList3D[vertexSize3D].z        = polyList3D[vertexSize3D - 1].z;
                                polyList3D[vertexSize3D].u        = polyList3D[vertexSize3D - 2].u;
                                polyList3D[vertexSize3D].v        = polyList3D[vertexSize3D - 1].v;
                                polyList3D[vertexSize3D].colour.r = 0xFF;
                                polyList3D[vertexSize3D].colour.g = 0xFF;
                                polyList3D[vertexSize3D].colour.b = 0xFF;
                                polyList3D[vertexSize3D].colour.a = 0xFF;
                                vertexSize3D++;

                                indexSize3D += 6;
                                break;
                            }
                        }
                    }
                }
                sinValue512 += 16;
            }
            sinValue512 -= 0x140;
            cosValue512 -= 16;
        }
    }
    floor3DXPos     = (layer->XPos >> 8) * -0.00390625f;
    floor3DYPos     = (layer->YPos >> 8) * 0.00390625f;
    floor3DZPos     = (layer->ZPos >> 8) * -0.00390625f;
    floor3DAngle    = layer->angle / 512.0f * -360.0f;
    render3DEnabled = true;
#endif
}
void Draw3DSkyLayer(int layerID)
{
#if RETRO_SOFTWARE_RENDER
    TileLayer *layer       = &stageLayouts[activeTileLayers[layerID]];
    int layerWidth         = layer->width << 7;
    int layerHeight        = layer->height << 7;
    int layerYPos          = layer->YPos;
    int sinValue           = sinValM7[layer->angle & 0x1FF];
    int cosValue           = cosValM7[layer->angle & 0x1FF];
    ushort *frameBufferPtr = &Engine.frameBuffer[((SCREEN_YSIZE / 2) + 12) * GFX_LINESIZE];
    ushort *bufferPtr      = Engine.frameBuffer2x;
    if (!drawStageGFXHQ)
        bufferPtr = &Engine.frameBuffer[((SCREEN_YSIZE / 2) + 12) * GFX_LINESIZE];
    byte *linePtr = &gfxLineBuffer[((SCREEN_YSIZE / 2) + 12)];
    int layerXPos = layer->XPos >> 4;
    int layerZPos = layer->ZPos >> 4;
    for (int i = TILE_SIZE / 2; i < SCREEN_YSIZE - TILE_SIZE; ++i) {
        if (!(i & 1)) {
            activePalette   = fullPalette[*linePtr];
            activePalette32 = fullPalette32[*linePtr];
            linePtr++;
        }
        int xBuffer    = layerYPos / (i << 8) * -cosValue >> 9;
        int yBuffer    = sinValue * (layerYPos / (i << 8)) >> 9;
        int XPos       = layerXPos + (3 * sinValue * (layerYPos / (i << 8)) >> 2) - xBuffer * GFX_LINESIZE;
        int YPos       = layerZPos + (3 * cosValue * (layerYPos / (i << 8)) >> 2) - yBuffer * GFX_LINESIZE;
        int lineBuffer = 0;
        while (lineBuffer < GFX_LINESIZE * 2) {
            int tileX = XPos >> 12;
            int tileY = YPos >> 12;
            if (tileX > -1 && tileX < layerWidth && tileY > -1 && tileY < layerHeight) {
                int chunk       = tile3DFloorBuffer[(YPos >> 16 << 8) + (XPos >> 16)];
                byte *tilePixel = &tilesetGFXData[tiles128x128.gfxDataPos[chunk]];
                switch (tiles128x128.direction[chunk]) {
                    case FLIP_NONE: tilePixel += TILE_SIZE * (tileY & 0xF) + (tileX & 0xF); break;
                    case FLIP_X: tilePixel += TILE_SIZE * (tileY & 0xF) + 0xF - (tileX & 0xF); break;
                    case FLIP_Y: tilePixel += (tileX & 0xF) + SCREEN_YSIZE - TILE_SIZE * (tileY & 0xF); break;
                    case FLIP_XY: tilePixel += 0xF - (tileX & 0xF) + SCREEN_YSIZE - TILE_SIZE * (tileY & 0xF); break;
                    default: break;
                }

                if (*tilePixel > 0)
                    *bufferPtr = activePalette[*tilePixel];
                else if (drawStageGFXHQ)
                    *bufferPtr = *frameBufferPtr;
            }
            else if (drawStageGFXHQ) {
                *bufferPtr = *frameBufferPtr;
            }
            if (lineBuffer & 1)
                ++frameBufferPtr;
            if (drawStageGFXHQ) {
                bufferPtr++;
            }
            else if (lineBuffer & 1) {
                ++bufferPtr;
            }
            lineBuffer++;
            XPos += xBuffer;
            YPos += yBuffer;
        }
        if (!(i & 1))
            frameBufferPtr -= GFX_LINESIZE;
        if (!(i & 1) && !drawStageGFXHQ) {
            bufferPtr -= GFX_LINESIZE;
        }
    }

    if (drawStageGFXHQ) {
        frameBufferPtr = &Engine.frameBuffer[((SCREEN_YSIZE / 2) + 12) * GFX_LINESIZE];
        int cnt        = ((SCREEN_YSIZE / 2) - 12) * GFX_LINESIZE;
        while (cnt--) *frameBufferPtr++ = 0xF81F; // Magenta
    }
#endif

#if RETRO_HARDWARE_RENDER
    // unimplimented in RSDK
#endif
}

void DrawRectangle(int XPos, int YPos, int width, int height, int R, int G, int B, int A)
{
    if (A > 0xFF)
        A = 0xFF;
#if RETRO_SOFTWARE_RENDER
    if (width + XPos > GFX_LINESIZE)
        width = GFX_LINESIZE - XPos;
    if (XPos < 0) {
        width += XPos;
        XPos = 0;
    }

    if (height + YPos > SCREEN_YSIZE)
        height = SCREEN_YSIZE - YPos;
    if (YPos < 0) {
        height += YPos;
        YPos = 0;
    }
    if (width <= 0 || height <= 0 || A <= 0)
        return;
    int pitch              = GFX_LINESIZE - width;
    ushort *frameBufferPtr = &Engine.frameBuffer[XPos + GFX_LINESIZE * YPos];
    ushort clr             = PACK_RGB888(R, G, B);
    if (A == 0xFF) {
        int h = height;
        while (h--) {
            int w = width;
            while (w--) {
                *frameBufferPtr = clr;
                ++frameBufferPtr;
            }
            frameBufferPtr += pitch;
        }
    }
    else {
        int h = height;
        while (h--) {
            int w = width;
            while (w--) {
                ushort *blendPtrB = &blendLookupTable[BLENDTABLE_XSIZE * (0xFF - A)];
                ushort *blendPtrA = &blendLookupTable[BLENDTABLE_XSIZE * A];

                *frameBufferPtr   = (blendPtrB[*frameBufferPtr & 0x1F] + blendPtrA[((byte)(B >> 3) | (byte)(32 * (G >> 2))) & 0x1F])
                                  | ((blendPtrB[(*frameBufferPtr & 0x7E0) >> 6] + blendPtrA[(clr & 0x7E0) >> 6]) << 6)
                                  | ((blendPtrB[(*frameBufferPtr & 0xF800) >> 11] + blendPtrA[(clr & 0xF800) >> 11]) << 11);
                ++frameBufferPtr;
            }
            frameBufferPtr += pitch;
        }
    }
#endif

#if RETRO_HARDWARE_RENDER
    if (gfxVertexSize < VERTEX_LIMIT) {
        gfxPolyList[gfxVertexSize].x        = XPos << 4;
        gfxPolyList[gfxVertexSize].y        = YPos << 4;
        gfxPolyList[gfxVertexSize].colour.r = R;
        gfxPolyList[gfxVertexSize].colour.g = G;
        gfxPolyList[gfxVertexSize].colour.b = B;
        gfxPolyList[gfxVertexSize].colour.a = A;
        gfxPolyList[gfxVertexSize].u        = 0.0f;
        gfxPolyList[gfxVertexSize].v        = 0.0f;
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = XPos + width << 4;
        gfxPolyList[gfxVertexSize].y        = YPos << 4;
        gfxPolyList[gfxVertexSize].colour.r = R;
        gfxPolyList[gfxVertexSize].colour.g = G;
        gfxPolyList[gfxVertexSize].colour.b = B;
        gfxPolyList[gfxVertexSize].colour.a = A;
        gfxPolyList[gfxVertexSize].u        = 0.01f;
        gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = XPos << 4;
        gfxPolyList[gfxVertexSize].y        = YPos + height << 4;
        gfxPolyList[gfxVertexSize].colour.r = R;
        gfxPolyList[gfxVertexSize].colour.g = G;
        gfxPolyList[gfxVertexSize].colour.b = B;
        gfxPolyList[gfxVertexSize].colour.a = A;
        gfxPolyList[gfxVertexSize].u        = 0.0f;
        gfxPolyList[gfxVertexSize].v        = 0.01f;
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = gfxPolyList[gfxVertexSize - 2].x;
        gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
        gfxPolyList[gfxVertexSize].colour.r = R;
        gfxPolyList[gfxVertexSize].colour.g = G;
        gfxPolyList[gfxVertexSize].colour.b = B;
        gfxPolyList[gfxVertexSize].colour.a = A;
        gfxPolyList[gfxVertexSize].u        = 0.01f;
        gfxPolyList[gfxVertexSize].v        = 0.01f;
        gfxVertexSize++;
        gfxIndexSize += 6;
    }
#endif
}

void SetFadeHQ(int R, int G, int B, int A)
{
#if RETRO_SOFTWARE_RENDER
    if (A <= 0)
        return;
    if (A > 0xFF)
        A = 0xFF;
    int pitch              = GFX_LINESIZE * 2;
    ushort *frameBufferPtr = Engine.frameBuffer2x;
    ushort clr             = PACK_RGB888(R, G, B);
    if (A == 0xFF) {
        int h = SCREEN_YSIZE;
        while (h--) {
            int w = pitch;
            while (w--) {
                *frameBufferPtr = clr;
                ++frameBufferPtr;
            }
        }
    }
    else {
        int h = SCREEN_YSIZE;
        while (h--) {
            int w = pitch;
            while (w--) {
                ushort *blendPtrB = &blendLookupTable[BLENDTABLE_XSIZE * (0xFF - A)];
                ushort *blendPtrA = &blendLookupTable[BLENDTABLE_XSIZE * A];
                *frameBufferPtr   = (blendPtrB[*frameBufferPtr & 0x1F] + blendPtrA[((byte)(B >> 3) | (byte)(32 * (G >> 2))) & 0x1F])
                                  | ((blendPtrB[(*frameBufferPtr & 0x7E0) >> 6] + blendPtrA[(clr & 0x7E0) >> 6]) << 6)
                                  | ((blendPtrB[(*frameBufferPtr & 0xF800) >> 11] + blendPtrA[(clr & 0xF800) >> 11]) << 11);
                ++frameBufferPtr;
            }
        }
    }
#endif

#if RETRO_HARDWARE_RENDER
    // TODO: this
#endif
}

void DrawTintRectangle(int XPos, int YPos, int width, int height)
{
#if RETRO_SOFTWARE_RENDER
    if (width + XPos > GFX_LINESIZE)
        width = GFX_LINESIZE - XPos;
    if (XPos < 0) {
        width += XPos;
        XPos = 0;
    }

    if (height + YPos > SCREEN_YSIZE)
        height = SCREEN_YSIZE - YPos;
    if (YPos < 0) {
        height += YPos;
        YPos = 0;
    }
    if (width <= 0 || height <= 0)
        return;
    int yOffset = GFX_LINESIZE - width;
    for (ushort *frameBufferPtr = &Engine.frameBuffer[XPos + GFX_LINESIZE * YPos];; frameBufferPtr += yOffset) {
        height--;
        if (!height)
            break;
        int w = width;
        while (w--) {
            *frameBufferPtr = tintLookupTable[*frameBufferPtr];
            ++frameBufferPtr;
        }
    }
#endif

#if RETRO_HARDWARE_RENDER
    // Not avaliable in HW Render mode
#endif
}
void DrawScaledTintMask(int direction, int XPos, int YPos, int pivotX, int pivotY, int scaleX, int scaleY, int width, int height, int sprX, int sprY,
                        int sheetID)
{
#if RETRO_SOFTWARE_RENDER
    int roundedYPos = 0;
    int roundedXPos = 0;
    int truescaleX  = 4 * scaleX;
    int truescaleY  = 4 * scaleY;
    int widthM1     = width - 1;
    int trueXPos    = XPos - (truescaleX * pivotX >> 11);
    width           = truescaleX * width >> 11;
    int trueYPos    = YPos - (truescaleY * pivotY >> 11);
    height          = truescaleY * height >> 11;
    int finalscaleX = (signed int)(float)((float)(2048.0 / (float)truescaleX) * 2048.0);
    int finalscaleY = (signed int)(float)((float)(2048.0 / (float)truescaleY) * 2048.0);
    if (width + trueXPos > GFX_LINESIZE) {
        width = GFX_LINESIZE - trueXPos;
    }

    if (direction) {
        if (trueXPos < 0) {
            widthM1 -= trueXPos * -finalscaleX >> 11;
            roundedXPos = (ushort)trueXPos * -(short)finalscaleX & 0x7FF;
            width += trueXPos;
            trueXPos = 0;
        }
    }
    else if (trueXPos < 0) {
        sprX += trueXPos * -finalscaleX >> 11;
        roundedXPos = (ushort)trueXPos * -(short)finalscaleX & 0x7FF;
        width += trueXPos;
        trueXPos = 0;
    }

    if (height + trueYPos > SCREEN_YSIZE) {
        height = SCREEN_YSIZE - trueYPos;
    }
    if (trueYPos < 0) {
        sprY += trueYPos * -finalscaleY >> 11;
        roundedYPos = (ushort)trueYPos * -(short)finalscaleY & 0x7FF;
        height += trueYPos;
        trueYPos = 0;
    }

    if (width <= 0 || height <= 0)
        return;

    GFXSurface *surface = &gfxSurface[sheetID];
    int pitch           = GFX_LINESIZE - width;
    int gfxwidth        = surface->width;
    // byte *lineBuffer       = &gfxLineBuffer[trueYPos];
    byte *gfxData          = &graphicData[sprX + surface->width * sprY + surface->dataPosition];
    ushort *frameBufferPtr = &Engine.frameBuffer[trueXPos + GFX_LINESIZE * trueYPos];
    if (direction == FLIP_X) {
        byte *gfxDataPtr = &gfxData[widthM1];
        int gfxPitch     = 0;
        while (height--) {
            int roundXPos = roundedXPos;
            int w         = width;
            while (w--) {
                if (*gfxDataPtr > 0)
                    *frameBufferPtr = tintLookupTable[*gfxDataPtr];
                int offsetX = finalscaleX + roundXPos;
                gfxDataPtr -= offsetX >> 11;
                gfxPitch += offsetX >> 11;
                roundXPos = offsetX & 0x7FF;
                ++frameBufferPtr;
            }
            frameBufferPtr += pitch;
            int offsetY = finalscaleY + roundedYPos;
            gfxDataPtr += gfxPitch + (offsetY >> 11) * gfxwidth;
            roundedYPos = offsetY & 0x7FF;
            gfxPitch    = 0;
        }
    }
    else {
        int gfxPitch = 0;
        int h        = height;
        while (h--) {
            int roundXPos = roundedXPos;
            int w         = width;
            while (w--) {
                if (*gfxData > 0)
                    *frameBufferPtr = tintLookupTable[*gfxData];
                int offsetX = finalscaleX + roundXPos;
                gfxData += offsetX >> 11;
                gfxPitch += offsetX >> 11;
                roundXPos = offsetX & 0x7FF;
                ++frameBufferPtr;
            }
            frameBufferPtr += pitch;
            int offsetY = finalscaleY + roundedYPos;
            gfxData += (offsetY >> 11) * gfxwidth - gfxPitch;
            roundedYPos = offsetY & 0x7FF;
            gfxPitch    = 0;
        }
    }
#endif

#if RETRO_HARDWARE_RENDER
    // Not avaliable in HW Render mode
#endif
}

void DrawSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int sheetID)
{
#if RETRO_SOFTWARE_RENDER
    if (width + XPos > GFX_LINESIZE)
        width = GFX_LINESIZE - XPos;
    if (XPos < 0) {
        sprX -= XPos;
        width += XPos;
        XPos = 0;
    }
    if (height + YPos > SCREEN_YSIZE)
        height = SCREEN_YSIZE - YPos;
    if (YPos < 0) {
        sprY -= YPos;
        height += YPos;
        YPos = 0;
    }
    if (width <= 0 || height <= 0)
        return;

    GFXSurface *surface    = &gfxSurface[sheetID];
    int pitch              = GFX_LINESIZE - width;
    int gfxPitch           = surface->width - width;
    byte *lineBuffer       = &gfxLineBuffer[YPos];
    byte *gfxDataPtr       = &graphicData[sprX + surface->width * sprY + surface->dataPosition];
    ushort *frameBufferPtr = &Engine.frameBuffer[XPos + GFX_LINESIZE * YPos];
    while (height--) {
        activePalette   = fullPalette[*lineBuffer];
        activePalette32 = fullPalette32[*lineBuffer];
        lineBuffer++;
        int w = width;
        while (w--) {
            if (*gfxDataPtr > 0) {
                *frameBufferPtr = activePalette[*gfxDataPtr];
            }
            ++gfxDataPtr;
            ++frameBufferPtr;
        }
        frameBufferPtr += pitch;
        gfxDataPtr += gfxPitch;
    }
#endif

#if RETRO_HARDWARE_RENDER
    GFXSurface *surface = &gfxSurface[sheetID];
    if (surface->texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && XPos > -512 && XPos < 872 && YPos > -512 && YPos < 752) {
        gfxPolyList[gfxVertexSize].x        = XPos << 4;
        gfxPolyList[gfxVertexSize].y        = YPos << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0xFF;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX);
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = XPos + width << 4;
        gfxPolyList[gfxVertexSize].y        = YPos << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0xFF;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX + width);
        gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = XPos << 4;
        gfxPolyList[gfxVertexSize].y        = YPos + height << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0xFF;
        gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY + height);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = gfxPolyList[gfxVertexSize - 2].x;
        gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0xFF;
        gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
        gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
        gfxVertexSize++;

        gfxIndexSize += 6;
    }
#endif
}

void DrawSpriteFlipped(int XPos, int YPos, int width, int height, int sprX, int sprY, int direction, int sheetID)
{
#if RETRO_SOFTWARE_RENDER
    int widthFlip  = width;
    int heightFlip = height;

    if (width + XPos > GFX_LINESIZE) {
        width = GFX_LINESIZE - XPos;
    }
    if (XPos < 0) {
        sprX -= XPos;
        width += XPos;
        widthFlip += XPos + XPos;
        XPos = 0;
    }
    if (height + YPos > SCREEN_YSIZE) {
        height = SCREEN_YSIZE - YPos;
    }
    if (YPos < 0) {
        sprY -= YPos;
        height += YPos;
        heightFlip += YPos + YPos;
        YPos = 0;
    }
    if (width <= 0 || height <= 0)
        return;

    GFXSurface *surface = &gfxSurface[sheetID];
    int pitch;
    int gfxPitch;
    byte *lineBuffer;
    byte *gfxData;
    ushort *frameBufferPtr;
    switch (direction) {
        case FLIP_NONE:
            pitch          = GFX_LINESIZE - width;
            gfxPitch       = surface->width - width;
            lineBuffer     = &gfxLineBuffer[YPos];
            gfxData        = &graphicData[sprX + surface->width * sprY + surface->dataPosition];
            frameBufferPtr = &Engine.frameBuffer[XPos + GFX_LINESIZE * YPos];

            while (height--) {
                activePalette   = fullPalette[*lineBuffer];
                activePalette32 = fullPalette32[*lineBuffer];
                lineBuffer++;
                int w = width;
                while (w--) {
                    if (*gfxData > 0)
                        *frameBufferPtr = activePalette[*gfxData];
                    ++gfxData;
                    ++frameBufferPtr;
                }
                frameBufferPtr += pitch;
                gfxData += gfxPitch;
            }
            break;
        case FLIP_X:
            pitch          = GFX_LINESIZE - width;
            gfxPitch       = width + surface->width;
            lineBuffer     = &gfxLineBuffer[YPos];
            gfxData        = &graphicData[widthFlip - 1 + sprX + surface->width * sprY + surface->dataPosition];
            frameBufferPtr = &Engine.frameBuffer[XPos + GFX_LINESIZE * YPos];
            while (height--) {
                activePalette   = fullPalette[*lineBuffer];
                activePalette32 = fullPalette32[*lineBuffer];
                lineBuffer++;
                int w = width;
                while (w--) {
                    if (*gfxData > 0)
                        *frameBufferPtr = activePalette[*gfxData];
                    --gfxData;
                    ++frameBufferPtr;
                }
                frameBufferPtr += pitch;
                gfxData += gfxPitch;
            }
            break;
        case FLIP_Y:
            pitch          = GFX_LINESIZE - width;
            gfxPitch       = width + surface->width;
            lineBuffer     = &gfxLineBuffer[YPos];
            gfxData        = &graphicData[sprX + surface->width * (sprY + heightFlip - 1) + surface->dataPosition];
            frameBufferPtr = &Engine.frameBuffer[XPos + GFX_LINESIZE * YPos];
            while (height--) {
                activePalette   = fullPalette[*lineBuffer];
                activePalette32 = fullPalette32[*lineBuffer];
                lineBuffer++;
                int w = width;
                while (w--) {
                    if (*gfxData > 0)
                        *frameBufferPtr = activePalette[*gfxData];
                    ++gfxData;
                    ++frameBufferPtr;
                }
                frameBufferPtr += pitch;
                gfxData -= gfxPitch;
            }
            break;
        case FLIP_XY:
            pitch          = GFX_LINESIZE - width;
            gfxPitch       = surface->width - width;
            lineBuffer     = &gfxLineBuffer[YPos];
            gfxData        = &graphicData[widthFlip - 1 + sprX + surface->width * (sprY + heightFlip - 1) + surface->dataPosition];
            frameBufferPtr = &Engine.frameBuffer[XPos + GFX_LINESIZE * YPos];
            while (height--) {
                activePalette   = fullPalette[*lineBuffer];
                activePalette32 = fullPalette32[*lineBuffer];
                lineBuffer++;
                int w = width;
                while (w--) {
                    if (*gfxData > 0)
                        *frameBufferPtr = activePalette[*gfxData];
                    --gfxData;
                    ++frameBufferPtr;
                }
                frameBufferPtr += pitch;
                gfxData -= gfxPitch;
            }
            break;
        default: break;
    }
#endif

#if RETRO_HARDWARE_RENDER
    GFXSurface *surface = &gfxSurface[sheetID];
    if (surface->texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && XPos > -512 && XPos < 872 && YPos > -512 && YPos < 752) {
        switch (direction) {
            case FLIP_NONE:
                gfxPolyList[gfxVertexSize].x        = XPos << 4;
                gfxPolyList[gfxVertexSize].y        = YPos << 4;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX);
                gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY);
                gfxVertexSize++;

                gfxPolyList[gfxVertexSize].x        = XPos + width << 4;
                gfxPolyList[gfxVertexSize].y        = YPos << 4;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX + width);
                gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                gfxVertexSize++;

                gfxPolyList[gfxVertexSize].x        = XPos << 4;
                gfxPolyList[gfxVertexSize].y        = YPos + height << 4;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY + height);
                gfxVertexSize++;

                gfxPolyList[gfxVertexSize].x        = gfxPolyList[gfxVertexSize - 2].x;
                gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                gfxVertexSize++;
                break;
            case FLIP_X:
                gfxPolyList[gfxVertexSize].x        = XPos << 4;
                gfxPolyList[gfxVertexSize].y        = YPos << 4;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX + width);
                gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY);
                gfxVertexSize++;

                gfxPolyList[gfxVertexSize].x        = XPos + width << 4;
                gfxPolyList[gfxVertexSize].y        = YPos << 4;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX);
                gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                gfxVertexSize++;

                gfxPolyList[gfxVertexSize].x        = XPos << 4;
                gfxPolyList[gfxVertexSize].y        = YPos + height << 4;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY + height);
                gfxVertexSize++;

                gfxPolyList[gfxVertexSize].x        = gfxPolyList[gfxVertexSize - 2].x;
                gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                gfxVertexSize++;
                break;
            case FLIP_Y:
                gfxPolyList[gfxVertexSize].x        = XPos << 4;
                gfxPolyList[gfxVertexSize].y        = YPos << 4;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX);
                gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY + height);
                gfxVertexSize++;

                gfxPolyList[gfxVertexSize].x        = XPos + width << 4;
                gfxPolyList[gfxVertexSize].y        = YPos << 4;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX + width);
                gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                gfxVertexSize++;

                gfxPolyList[gfxVertexSize].x        = XPos << 4;
                gfxPolyList[gfxVertexSize].y        = YPos + height << 4;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY);
                gfxVertexSize++;

                gfxPolyList[gfxVertexSize].x        = gfxPolyList[gfxVertexSize - 2].x;
                gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                gfxVertexSize++;
                break;
            case FLIP_XY:
                gfxPolyList[gfxVertexSize].x        = XPos << 4;
                gfxPolyList[gfxVertexSize].y        = YPos << 4;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX + width);
                gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY + height);
                gfxVertexSize++;

                gfxPolyList[gfxVertexSize].x        = XPos + width << 4;
                gfxPolyList[gfxVertexSize].y        = YPos << 4;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX);
                gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                gfxVertexSize++;

                gfxPolyList[gfxVertexSize].x        = XPos << 4;
                gfxPolyList[gfxVertexSize].y        = YPos + height << 4;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY);
                gfxVertexSize++;

                gfxPolyList[gfxVertexSize].x        = gfxPolyList[gfxVertexSize - 2].x;
                gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
                gfxPolyList[gfxVertexSize].colour.r = 0xFF;
                gfxPolyList[gfxVertexSize].colour.g = 0xFF;
                gfxPolyList[gfxVertexSize].colour.b = 0xFF;
                gfxPolyList[gfxVertexSize].colour.a = 0xFF;
                gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
                gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
                gfxVertexSize++;
                break;
        }
        gfxIndexSize += 6;
    }
#endif
}
void DrawSpriteScaled(int direction, int XPos, int YPos, int pivotX, int pivotY, int scaleX, int scaleY, int width, int height, int sprX, int sprY,
                      int sheetID)
{
#if RETRO_SOFTWARE_RENDER
    int roundedYPos = 0;
    int roundedXPos = 0;
    int truescaleX  = 4 * scaleX;
    int truescaleY  = 4 * scaleY;
    int widthM1     = width - 1;
    int trueXPos    = XPos - (truescaleX * pivotX >> 11);
    width           = truescaleX * width >> 11;
    int trueYPos    = YPos - (truescaleY * pivotY >> 11);
    height          = truescaleY * height >> 11;
    int finalscaleX = (signed int)(float)((float)(2048.0 / (float)truescaleX) * 2048.0);
    int finalscaleY = (signed int)(float)((float)(2048.0 / (float)truescaleY) * 2048.0);
    if (width + trueXPos > GFX_LINESIZE) {
        width = GFX_LINESIZE - trueXPos;
    }

    if (direction) {
        if (trueXPos < 0) {
            widthM1 -= trueXPos * -finalscaleX >> 11;
            roundedXPos = (ushort)trueXPos * -(short)finalscaleX & 0x7FF;
            width += trueXPos;
            trueXPos = 0;
        }
    }
    else if (trueXPos < 0) {
        sprX += trueXPos * -finalscaleX >> 11;
        roundedXPos = (ushort)trueXPos * -(short)finalscaleX & 0x7FF;
        width += trueXPos;
        trueXPos = 0;
    }

    if (height + trueYPos > SCREEN_YSIZE) {
        height = SCREEN_YSIZE - trueYPos;
    }
    if (trueYPos < 0) {
        sprY += trueYPos * -finalscaleY >> 11;
        roundedYPos = (ushort)trueYPos * -(short)finalscaleY & 0x7FF;
        height += trueYPos;
        trueYPos = 0;
    }

    if (width <= 0 || height <= 0)
        return;

    GFXSurface *surface    = &gfxSurface[sheetID];
    int pitch              = GFX_LINESIZE - width;
    int gfxwidth           = surface->width;
    byte *lineBuffer       = &gfxLineBuffer[trueYPos];
    byte *gfxData          = &graphicData[sprX + surface->width * sprY + surface->dataPosition];
    ushort *frameBufferPtr = &Engine.frameBuffer[trueXPos + GFX_LINESIZE * trueYPos];
    if (direction == FLIP_X) {
        byte *gfxDataPtr = &gfxData[widthM1];
        int gfxPitch     = 0;
        while (height--) {
            activePalette   = fullPalette[*lineBuffer];
            activePalette32 = fullPalette32[*lineBuffer];
            lineBuffer++;
            int roundXPos = roundedXPos;
            int w         = width;
            while (w--) {
                if (*gfxDataPtr > 0)
                    *frameBufferPtr = activePalette[*gfxDataPtr];
                int offsetX = finalscaleX + roundXPos;
                gfxDataPtr -= offsetX >> 11;
                gfxPitch += offsetX >> 11;
                roundXPos = offsetX & 0x7FF;
                ++frameBufferPtr;
            }
            frameBufferPtr += pitch;
            int offsetY = finalscaleY + roundedYPos;
            gfxDataPtr += gfxPitch + (offsetY >> 11) * gfxwidth;
            roundedYPos = offsetY & 0x7FF;
            gfxPitch    = 0;
        }
    }
    else {
        int gfxPitch = 0;
        int h        = height;
        while (h--) {
            activePalette   = fullPalette[*lineBuffer];
            activePalette32 = fullPalette32[*lineBuffer];
            lineBuffer++;
            int roundXPos = roundedXPos;
            int w         = width;
            while (w--) {
                if (*gfxData > 0)
                    *frameBufferPtr = activePalette[*gfxData];
                int offsetX = finalscaleX + roundXPos;
                gfxData += offsetX >> 11;
                gfxPitch += offsetX >> 11;
                roundXPos = offsetX & 0x7FF;
                ++frameBufferPtr;
            }
            frameBufferPtr += pitch;
            int offsetY = finalscaleY + roundedYPos;
            gfxData += (offsetY >> 11) * gfxwidth - gfxPitch;
            roundedYPos = offsetY & 0x7FF;
            gfxPitch    = 0;
        }
    }
#endif

#if RETRO_HARDWARE_RENDER
    if (gfxVertexSize < VERTEX_LIMIT && XPos > -512 && XPos < 872 && YPos > -512 && YPos < 752) {
        scaleX <<= 2;
        scaleY <<= 2;
        XPos -= pivotX * scaleX >> 11;
        scaleX = width * scaleX >> 11;
        YPos -= pivotY * scaleY >> 11;
        scaleY              = height * scaleY >> 11;
        GFXSurface *surface = &gfxSurface[sheetID];
        if (surface->texStartX > -1) {
            gfxPolyList[gfxVertexSize].x        = XPos << 4;
            gfxPolyList[gfxVertexSize].y        = YPos << 4;
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX);
            gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY);
            gfxVertexSize++;

            gfxPolyList[gfxVertexSize].x        = XPos + scaleX << 4;
            gfxPolyList[gfxVertexSize].y        = YPos << 4;
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX + width);
            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
            gfxVertexSize++;

            gfxPolyList[gfxVertexSize].x        = XPos << 4;
            gfxPolyList[gfxVertexSize].y        = YPos + scaleY << 4;
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
            gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY + height);
            gfxVertexSize++;

            gfxPolyList[gfxVertexSize].x        = gfxPolyList[gfxVertexSize - 2].x;
            gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
            gfxVertexSize++;
            gfxIndexSize += 6;
        }
    }
#endif
}
#if RETRO_REV01
void DrawScaledChar(int direction, int XPos, int YPos, int pivotX, int pivotY, int scaleX, int scaleY, int width, int height, int sprX, int sprY,
                    int sheetID)
{
#if RETRO_SOFTWARE_RENDER
    // Not avaliable in SW Render mode
#endif

#if RETRO_HARDWARE_RENDER
    GFXSurface *surface = &gfxSurface[sheetID];
    if (gfxVertexSize < VERTEX_LIMIT && XPos > -8192 && XPos < 13951 && YPos > -1024 && YPos < 4864) {
        XPos -= pivotX * scaleX >> 5;
        scaleX = width * scaleX >> 5;
        YPos -= pivotY * scaleY >> 5;
        scaleY = height * scaleY >> 5;
        if (gfxSurface[sheetID].texStartX > -1 && gfxVertexSize < 4096) {
            gfxPolyList[gfxVertexSize].x        = XPos;
            gfxPolyList[gfxVertexSize].y        = YPos;
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = gfxSurface[sheetID].texStartX + sprX;
            gfxPolyList[gfxVertexSize].v        = gfxSurface[sheetID].texStartY + sprY;
            gfxVertexSize++;

            gfxPolyList[gfxVertexSize].x        = XPos + scaleX;
            gfxPolyList[gfxVertexSize].y        = YPos;
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = gfxSurface[sheetID].texStartX + sprX + width;
            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
            gfxVertexSize++;

            gfxPolyList[gfxVertexSize].x        = XPos;
            gfxPolyList[gfxVertexSize].y        = YPos + scaleY;
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
            gfxPolyList[gfxVertexSize].v        = gfxSurface[sheetID].texStartY + sprY + height;
            gfxVertexSize++;

            gfxPolyList[gfxVertexSize].x        = gfxPolyList[gfxVertexSize - 2].x;
            gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
            gfxVertexSize++;

            gfxIndexSize += 6;
        }
    }
#endif
}
#endif
void DrawSpriteRotated(int direction, int XPos, int YPos, int pivotX, int pivotY, int sprX, int sprY, int width, int height, int rotation,
                       int sheetID)
{
#if RETRO_SOFTWARE_RENDER
    int sprXPos    = (pivotX + sprX) << 9;
    int sprYPos    = (pivotY + sprY) << 9;
    int fullwidth  = width + sprX;
    int fullheight = height + sprY;
    int angle      = rotation & 0x1FF;
    if (angle < 0)
        angle += 0x200;
    if (angle)
        angle = 0x200 - angle;
    int sine   = sinVal512[angle];
    int cosine = cosVal512[angle];
    int xPositions[4];
    int yPositions[4];

    if (direction == FLIP_X) {
        xPositions[0] = XPos + ((sine * (-pivotY - 2) + cosine * (pivotX + 2)) >> 9);
        yPositions[0] = YPos + ((cosine * (-pivotY - 2) - sine * (pivotX + 2)) >> 9);
        xPositions[1] = XPos + ((sine * (-pivotY - 2) + cosine * (pivotX - width - 2)) >> 9);
        yPositions[1] = YPos + ((cosine * (-pivotY - 2) - sine * (pivotX - width - 2)) >> 9);
        xPositions[2] = XPos + ((sine * (height - pivotY + 2) + cosine * (pivotX + 2)) >> 9);
        yPositions[2] = YPos + ((cosine * (height - pivotY + 2) - sine * (pivotX + 2)) >> 9);
        int a         = pivotX - width - 2;
        int b         = height - pivotY + 2;
        xPositions[3] = XPos + ((sine * b + cosine * a) >> 9);
        yPositions[3] = YPos + ((cosine * b - sine * a) >> 9);
    }
    else {
        xPositions[0] = XPos + ((sine * (-pivotY - 2) + cosine * (-pivotX - 2)) >> 9);
        yPositions[0] = YPos + ((cosine * (-pivotY - 2) - sine * (-pivotX - 2)) >> 9);
        xPositions[1] = XPos + ((sine * (-pivotY - 2) + cosine * (width - pivotX + 2)) >> 9);
        yPositions[1] = YPos + ((cosine * (-pivotY - 2) - sine * (width - pivotX + 2)) >> 9);
        xPositions[2] = XPos + ((sine * (height - pivotY + 2) + cosine * (-pivotX - 2)) >> 9);
        yPositions[2] = YPos + ((cosine * (height - pivotY + 2) - sine * (-pivotX - 2)) >> 9);
        int a         = width - pivotX + 2;
        int b         = height - pivotY + 2;
        xPositions[3] = XPos + ((sine * b + cosine * a) >> 9);
        yPositions[3] = YPos + ((cosine * b - sine * a) >> 9);
    }

    int left = GFX_LINESIZE;
    for (int i = 0; i < 4; ++i) {
        if (xPositions[i] < left)
            left = xPositions[i];
    }
    if (left < 0)
        left = 0;

    int right = 0;
    for (int i = 0; i < 4; ++i) {
        if (xPositions[i] > right)
            right = xPositions[i];
    }
    if (right > GFX_LINESIZE)
        right = GFX_LINESIZE;
    int maxX = right - left;

    int top = SCREEN_YSIZE;
    for (int i = 0; i < 4; ++i) {
        if (yPositions[i] < top)
            top = yPositions[i];
    }
    if (top < 0)
        top = 0;

    int bottom = 0;
    for (int i = 0; i < 4; ++i) {
        if (yPositions[i] > bottom)
            bottom = yPositions[i];
    }
    if (bottom > SCREEN_YSIZE)
        bottom = SCREEN_YSIZE;
    int maxY = bottom - top;

    if (maxX <= 0 || maxY <= 0)
        return;

    GFXSurface *surface    = &gfxSurface[sheetID];
    int pitch              = GFX_LINESIZE - maxX;
    int lineSize           = surface->widthShift;
    ushort *frameBufferPtr = &Engine.frameBuffer[left + GFX_LINESIZE * top];
    byte *lineBuffer       = &gfxLineBuffer[top];
    int startX             = left - XPos;
    int startY             = top - YPos;
    int shiftPivot         = (sprX << 9) - 1;
    fullwidth <<= 9;
    int shiftheight = (sprY << 9) - 1;
    fullheight <<= 9;
    byte *gfxData = &graphicData[surface->dataPosition];
    if (cosine < 0 || sine < 0)
        sprYPos += sine + cosine;

    if (direction == FLIP_X) {
        int drawX = sprXPos - (cosine * startX - sine * startY) - 0x100;
        int drawY = cosine * startY + sprYPos + sine * startX;
        while (maxY--) {
            activePalette   = fullPalette[*lineBuffer];
            activePalette32 = fullPalette32[*lineBuffer];
            lineBuffer++;
            int finalX = drawX;
            int finalY = drawY;
            int w      = maxX;
            while (w--) {
                if (finalX > shiftPivot && finalX < fullwidth && finalY > shiftheight && finalY < fullheight) {
                    byte index = gfxData[(finalY >> 9 << lineSize) + (finalX >> 9)];
                    if (index > 0)
                        *frameBufferPtr = activePalette[index];
                }
                ++frameBufferPtr;
                finalX -= cosine;
                finalY += sine;
            }
            drawX += sine;
            drawY += cosine;
            frameBufferPtr += pitch;
        }
    }
    else {
        int drawX = sprXPos + cosine * startX - sine * startY;
        int drawY = cosine * startY + sprYPos + sine * startX;
        while (maxY--) {
            activePalette   = fullPalette[*lineBuffer];
            activePalette32 = fullPalette32[*lineBuffer];
            lineBuffer++;
            int finalX = drawX;
            int finalY = drawY;
            int w      = maxX;
            while (w--) {
                if (finalX > shiftPivot && finalX < fullwidth && finalY > shiftheight && finalY < fullheight) {
                    byte index = gfxData[(finalY >> 9 << lineSize) + (finalX >> 9)];
                    if (index > 0)
                        *frameBufferPtr = activePalette[index];
                }
                ++frameBufferPtr;
                finalX += cosine;
                finalY += sine;
            }
            drawX -= sine;
            drawY += cosine;
            frameBufferPtr += pitch;
        }
    }
#endif

#if RETRO_HARDWARE_RENDER
    GFXSurface *surface = &gfxSurface[sheetID];
    XPos <<= 4;
    YPos <<= 4;
    rotation -= rotation >> 9 << 9;
    if (rotation < 0) {
        rotation += 0x200;
    }
    if (rotation != 0) {
        rotation = 0x200 - rotation;
    }
    int sin = sinVal512[rotation];
    int cos = cosVal512[rotation];
    if (surface->texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && XPos > -8192 && XPos < 13952 && YPos > -8192 && YPos < 12032) {
        if (direction == FLIP_NONE) {
            int x                               = -pivotX;
            int y                               = -pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX);
            gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY);
            gfxVertexSize++;

            x                                   = width - pivotX;
            y                                   = -pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX + width);
            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
            gfxVertexSize++;

            x                                   = -pivotX;
            y                                   = height - pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
            gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY + height);
            gfxVertexSize++;

            x                                   = width - pivotX;
            y                                   = height - pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
            gfxVertexSize++;
            gfxIndexSize += 6;
        }
        else {
            int x                               = pivotX;
            int y                               = -pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX);
            gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY);
            gfxVertexSize++;

            x                                   = pivotX - width;
            y                                   = -pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX + width);
            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
            gfxVertexSize++;

            x                                   = pivotX;
            y                                   = height - pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
            gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY + height);
            gfxVertexSize++;

            x                                   = pivotX - width;
            y                                   = height - pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
            gfxVertexSize++;
            gfxIndexSize += 6;
        }
    }
#endif
}

void DrawSpriteRotozoom(int direction, int XPos, int YPos, int pivotX, int pivotY, int sprX, int sprY, int width, int height, int rotation, int scale,
                        int sheetID)
{
#if RETRO_SOFTWARE_RENDER
    if (scale == 0)
        return;

    int sprXPos    = (pivotX + sprX) << 9;
    int sprYPos    = (pivotY + sprY) << 9;
    int fullwidth  = width + sprX;
    int fullheight = height + sprY;
    int angle      = rotation & 0x1FF;
    if (angle < 0)
        angle += 0x200;
    if (angle)
        angle = 0x200 - angle;
    int sine   = scale * sinVal512[angle] >> 9;
    int cosine = scale * cosVal512[angle] >> 9;
    int xPositions[4];
    int yPositions[4];

    if (direction == FLIP_X) {
        xPositions[0] = XPos + ((sine * (-pivotY - 2) + cosine * (pivotX + 2)) >> 9);
        yPositions[0] = YPos + ((cosine * (-pivotY - 2) - sine * (pivotX + 2)) >> 9);
        xPositions[1] = XPos + ((sine * (-pivotY - 2) + cosine * (pivotX - width - 2)) >> 9);
        yPositions[1] = YPos + ((cosine * (-pivotY - 2) - sine * (pivotX - width - 2)) >> 9);
        xPositions[2] = XPos + ((sine * (height - pivotY + 2) + cosine * (pivotX + 2)) >> 9);
        yPositions[2] = YPos + ((cosine * (height - pivotY + 2) - sine * (pivotX + 2)) >> 9);
        int a         = pivotX - width - 2;
        int b         = height - pivotY + 2;
        xPositions[3] = XPos + ((sine * b + cosine * a) >> 9);
        yPositions[3] = YPos + ((cosine * b - sine * a) >> 9);
    }
    else {
        xPositions[0] = XPos + ((sine * (-pivotY - 2) + cosine * (-pivotX - 2)) >> 9);
        yPositions[0] = YPos + ((cosine * (-pivotY - 2) - sine * (-pivotX - 2)) >> 9);
        xPositions[1] = XPos + ((sine * (-pivotY - 2) + cosine * (width - pivotX + 2)) >> 9);
        yPositions[1] = YPos + ((cosine * (-pivotY - 2) - sine * (width - pivotX + 2)) >> 9);
        xPositions[2] = XPos + ((sine * (height - pivotY + 2) + cosine * (-pivotX - 2)) >> 9);
        yPositions[2] = YPos + ((cosine * (height - pivotY + 2) - sine * (-pivotX - 2)) >> 9);
        int a         = width - pivotX + 2;
        int b         = height - pivotY + 2;
        xPositions[3] = XPos + ((sine * b + cosine * a) >> 9);
        yPositions[3] = YPos + ((cosine * b - sine * a) >> 9);
    }
    int truescale = (signed int)(float)((float)(512.0 / (float)scale) * 512.0);
    sine          = truescale * sinVal512[angle] >> 9;
    cosine        = truescale * cosVal512[angle] >> 9;

    int left = GFX_LINESIZE;
    for (int i = 0; i < 4; ++i) {
        if (xPositions[i] < left)
            left = xPositions[i];
    }
    if (left < 0)
        left = 0;

    int right = 0;
    for (int i = 0; i < 4; ++i) {
        if (xPositions[i] > right)
            right = xPositions[i];
    }
    if (right > GFX_LINESIZE)
        right = GFX_LINESIZE;
    int maxX = right - left;

    int top = SCREEN_YSIZE;
    for (int i = 0; i < 4; ++i) {
        if (yPositions[i] < top)
            top = yPositions[i];
    }
    if (top < 0)
        top = 0;

    int bottom = 0;
    for (int i = 0; i < 4; ++i) {
        if (yPositions[i] > bottom)
            bottom = yPositions[i];
    }
    if (bottom > SCREEN_YSIZE)
        bottom = SCREEN_YSIZE;
    int maxY = bottom - top;

    if (maxX <= 0 || maxY <= 0)
        return;

    GFXSurface *surface    = &gfxSurface[sheetID];
    int pitch              = GFX_LINESIZE - maxX;
    int lineSize           = surface->widthShift;
    ushort *frameBufferPtr = &Engine.frameBuffer[left + GFX_LINESIZE * top];
    byte *lineBuffer       = &gfxLineBuffer[top];
    int startX             = left - XPos;
    int startY             = top - YPos;
    int shiftPivot         = (sprX << 9) - 1;
    fullwidth <<= 9;
    int shiftheight = (sprY << 9) - 1;
    fullheight <<= 9;
    byte *gfxData = &graphicData[surface->dataPosition];
    if (cosine < 0 || sine < 0)
        sprYPos += sine + cosine;

    if (direction == FLIP_X) {
        int drawX = sprXPos - (cosine * startX - sine * startY) - (truescale >> 1);
        int drawY = cosine * startY + sprYPos + sine * startX;
        while (maxY--) {
            activePalette   = fullPalette[*lineBuffer];
            activePalette32 = fullPalette32[*lineBuffer];
            lineBuffer++;
            int finalX = drawX;
            int finalY = drawY;
            int w      = maxX;
            while (w--) {
                if (finalX > shiftPivot && finalX < fullwidth && finalY > shiftheight && finalY < fullheight) {
                    byte index = gfxData[(finalY >> 9 << lineSize) + (finalX >> 9)];
                    if (index > 0)
                        *frameBufferPtr = activePalette[index];
                }
                ++frameBufferPtr;
                finalX -= cosine;
                finalY += sine;
            }
            drawX += sine;
            drawY += cosine;
            frameBufferPtr += pitch;
        }
    }
    else {
        int drawX = sprXPos + cosine * startX - sine * startY;
        int drawY = cosine * startY + sprYPos + sine * startX;
        while (maxY--) {
            activePalette   = fullPalette[*lineBuffer];
            activePalette32 = fullPalette32[*lineBuffer];
            lineBuffer++;
            int finalX = drawX;
            int finalY = drawY;
            int w      = maxX;
            while (w--) {
                if (finalX > shiftPivot && finalX < fullwidth && finalY > shiftheight && finalY < fullheight) {
                    byte index = gfxData[(finalY >> 9 << lineSize) + (finalX >> 9)];
                    if (index > 0)
                        *frameBufferPtr = activePalette[index];
                }
                ++frameBufferPtr;
                finalX += cosine;
                finalY += sine;
            }
            drawX -= sine;
            drawY += cosine;
            frameBufferPtr += pitch;
        }
    }
#endif

#if RETRO_HARDWARE_RENDER
    GFXSurface *surface = &gfxSurface[sheetID];
    XPos <<= 4;
    YPos <<= 4;
    rotation -= rotation >> 9 << 9;
    if (rotation < 0)
        rotation += 0x200;
    if (rotation != 0)
        rotation = 0x200 - rotation;

    int sin = sinVal512[rotation] * scale >> 9;
    int cos = cosVal512[rotation] * scale >> 9;
    if (surface->texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && XPos > -8192 && XPos < 13952 && YPos > -8192 && YPos < 12032) {
        if (direction == FLIP_NONE) {
            int x                               = -pivotX;
            int y                               = -pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX);
            gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY);
            gfxVertexSize++;

            x                                   = width - pivotX;
            y                                   = -pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX + width);
            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
            gfxVertexSize++;

            x                                   = -pivotX;
            y                                   = height - pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
            gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY + height);
            gfxVertexSize++;

            x                                   = width - pivotX;
            y                                   = height - pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
            gfxVertexSize++;
            gfxIndexSize += 6;
        }
        else {
            int x                               = pivotX;
            int y                               = -pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX);
            gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY);
            gfxVertexSize++;

            x                                   = pivotX - width;
            y                                   = -pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX + width);
            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
            gfxVertexSize++;

            x                                   = pivotX;
            y                                   = height - pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
            gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY + height);
            gfxVertexSize++;

            x                                   = pivotX - width;
            y                                   = height - pivotY;
            gfxPolyList[gfxVertexSize].x        = XPos + (x * cos + y * sin >> 5);
            gfxPolyList[gfxVertexSize].y        = YPos + (y * cos - x * sin >> 5);
            gfxPolyList[gfxVertexSize].colour.r = 0xFF;
            gfxPolyList[gfxVertexSize].colour.g = 0xFF;
            gfxPolyList[gfxVertexSize].colour.b = 0xFF;
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
            gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
            gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
            gfxVertexSize++;
            gfxIndexSize += 6;
        }
    }
#endif
}

void DrawBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int sheetID)
{
#if RETRO_SOFTWARE_RENDER
    if (width + XPos > GFX_LINESIZE)
        width = GFX_LINESIZE - XPos;
    if (XPos < 0) {
        sprX -= XPos;
        width += XPos;
        XPos = 0;
    }
    if (height + YPos > SCREEN_YSIZE)
        height = SCREEN_YSIZE - YPos;
    if (YPos < 0) {
        sprY -= YPos;
        height += YPos;
        YPos = 0;
    }
    if (width <= 0 || height <= 0)
        return;

    GFXSurface *surface    = &gfxSurface[sheetID];
    int pitch              = GFX_LINESIZE - width;
    int gfxPitch           = surface->width - width;
    byte *lineBuffer       = &gfxLineBuffer[YPos];
    byte *gfxData          = &graphicData[sprX + surface->width * sprY + surface->dataPosition];
    ushort *frameBufferPtr = &Engine.frameBuffer[XPos + GFX_LINESIZE * YPos];
    while (height--) {
        activePalette   = fullPalette[*lineBuffer];
        activePalette32 = fullPalette32[*lineBuffer];
        lineBuffer++;
        int w = width;
        while (w--) {
            if (*gfxData > 0)
                *frameBufferPtr = ((activePalette[*gfxData] & 0xF7DE) >> 1) + ((*frameBufferPtr & 0xF7DE) >> 1);
            ++gfxData;
            ++frameBufferPtr;
        }
        frameBufferPtr += pitch;
        gfxData += gfxPitch;
    }
#endif

#if RETRO_HARDWARE_RENDER
    GFXSurface *surface = &gfxSurface[sheetID];
    if (surface->texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && XPos > -512 && XPos < 872 && YPos > -512 && YPos < 752) {
        gfxPolyList[gfxVertexSize].x        = XPos << 4;
        gfxPolyList[gfxVertexSize].y        = YPos << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0x80;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX);
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = XPos + width << 4;
        gfxPolyList[gfxVertexSize].y        = YPos << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0x80;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX + width);
        gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = XPos << 4;
        gfxPolyList[gfxVertexSize].y        = YPos + height << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0x80;
        gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY + height);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = gfxPolyList[gfxVertexSize - 2].x;
        gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0x80;
        gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
        gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
        gfxVertexSize++;

        gfxIndexSize += 6;
    }
#endif
}
void DrawAlphaBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int alpha, int sheetID)
{
    if (alpha > 0xFF)
        alpha = 0xFF;
#if RETRO_SOFTWARE_RENDER
    if (width + XPos > GFX_LINESIZE)
        width = GFX_LINESIZE - XPos;
    if (XPos < 0) {
        sprX -= XPos;
        width += XPos;
        XPos = 0;
    }
    if (height + YPos > SCREEN_YSIZE)
        height = SCREEN_YSIZE - YPos;
    if (YPos < 0) {
        sprY -= YPos;
        height += YPos;
        YPos = 0;
    }
    if (width <= 0 || height <= 0 || alpha <= 0)
        return;

    GFXSurface *surface    = &gfxSurface[sheetID];
    int pitch              = GFX_LINESIZE - width;
    int gfxPitch           = surface->width - width;
    byte *lineBuffer       = &gfxLineBuffer[YPos];
    byte *gfxData          = &graphicData[sprX + surface->width * sprY + surface->dataPosition];
    ushort *frameBufferPtr = &Engine.frameBuffer[XPos + GFX_LINESIZE * YPos];
    if (alpha == 0xFF) {
        while (height--) {
            activePalette   = fullPalette[*lineBuffer];
            activePalette32 = fullPalette32[*lineBuffer];
            lineBuffer++;
            int w = width;
            while (w--) {
                if (*gfxData > 0)
                    *frameBufferPtr = activePalette[*gfxData];
                ++gfxData;
                ++frameBufferPtr;
            }
            frameBufferPtr += pitch;
            gfxData += gfxPitch;
        }
    }
    else {
        while (height--) {
            activePalette   = fullPalette[*lineBuffer];
            activePalette32 = fullPalette32[*lineBuffer];
            lineBuffer++;
            int w = width;
            while (w--) {
                if (*gfxData > 0) {
                    setPixelAlpha(activePalette[*gfxData], *frameBufferPtr, alpha);
                }
                ++gfxData;
                ++frameBufferPtr;
            }
            frameBufferPtr += pitch;
            gfxData += gfxPitch;
        }
    }
#endif

#if RETRO_HARDWARE_RENDER
    GFXSurface *surface = &gfxSurface[sheetID];
    if (surface->texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && XPos > -512 && XPos < 872 && YPos > -512 && YPos < 752) {
        gfxPolyList[gfxVertexSize].x        = XPos << 4;
        gfxPolyList[gfxVertexSize].y        = YPos << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = alpha;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX);
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = XPos + width << 4;
        gfxPolyList[gfxVertexSize].y        = YPos << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = alpha;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX + width);
        gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = XPos << 4;
        gfxPolyList[gfxVertexSize].y        = YPos + height << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = alpha;
        gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY + height);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = gfxPolyList[gfxVertexSize - 2].x;
        gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = alpha;
        gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
        gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
        gfxVertexSize++;

        gfxIndexSize += 6;
    }
#endif
}
void DrawAdditiveBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int alpha, int sheetID)
{
    if (alpha > 0xFF)
        alpha = 0xFF;
#if RETRO_SOFTWARE_RENDER
    if (width + XPos > GFX_LINESIZE)
        width = GFX_LINESIZE - XPos;
    if (XPos < 0) {
        sprX -= XPos;
        width += XPos;
        XPos = 0;
    }
    if (height + YPos > SCREEN_YSIZE)
        height = SCREEN_YSIZE - YPos;
    if (YPos < 0) {
        sprY -= YPos;
        height += YPos;
        YPos = 0;
    }
    if (width <= 0 || height <= 0 || alpha <= 0)
        return;

    ushort *blendTablePtr  = &blendLookupTable[BLENDTABLE_XSIZE * alpha];
    GFXSurface *surface    = &gfxSurface[sheetID];
    int pitch              = GFX_LINESIZE - width;
    int gfxPitch           = surface->width - width;
    byte *lineBuffer       = &gfxLineBuffer[YPos];
    byte *gfxData          = &graphicData[sprX + surface->width * sprY + surface->dataPosition];
    ushort *frameBufferPtr = &Engine.frameBuffer[XPos + GFX_LINESIZE * YPos];

    while (height--) {
        activePalette   = fullPalette[*lineBuffer];
        activePalette32 = fullPalette32[*lineBuffer];
        lineBuffer++;
        int w = width;
        while (w--) {
            if (*gfxData > 0) {
                ushort colour   = activePalette[*gfxData];
                int v20         = 0;
                int v21         = 0;
                int finalColour = 0;

                if ((blendTablePtr[(colour & 0xF800) >> 11] << 11) + (*frameBufferPtr & 0xF800) <= 0xF800)
                    v20 = (blendTablePtr[(colour & 0xF800) >> 11] << 11) + (*frameBufferPtr & 0xF800);
                else
                    v20 = 0xF800;
                int v12 = (blendTablePtr[(colour & 0x7E0) >> 6] << 6) + (*frameBufferPtr & 0x7E0);
                if (v12 <= 0x7E0)
                    v21 = v12 | v20;
                else
                    v21 = v20 | 0x7E0;
                int v13 = blendTablePtr[colour & 0x1F] + (*frameBufferPtr & 0x1F);
                if (v13 <= 0x1F)
                    finalColour = v13 | v21;
                else
                    finalColour = v21 | 0x1F;
                *frameBufferPtr = finalColour;
            }
            ++gfxData;
            ++frameBufferPtr;
        }
        frameBufferPtr += pitch;
        gfxData += gfxPitch;
    }
#endif

#if RETRO_HARDWARE_RENDER
    GFXSurface *surface = &gfxSurface[sheetID];
    if (surface->texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && XPos > -512 && XPos < 872 && YPos > -512 && YPos < 752) {
        gfxPolyList[gfxVertexSize].x        = XPos << 4;
        gfxPolyList[gfxVertexSize].y        = YPos << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = alpha;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX);
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = XPos + width << 4;
        gfxPolyList[gfxVertexSize].y        = YPos << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = alpha;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX + width);
        gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = XPos << 4;
        gfxPolyList[gfxVertexSize].y        = YPos + height << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = alpha;
        gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY + height);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = gfxPolyList[gfxVertexSize - 2].x;
        gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = alpha;
        gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
        gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
        gfxVertexSize++;

        gfxIndexSize += 6;
    }
#endif
}
void DrawSubtractiveBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int alpha, int sheetID)
{
    if (alpha > 0xFF)
        alpha = 0xFF;

#if RETRO_SOFTWARE_RENDER
    if (width + XPos > GFX_LINESIZE)
        width = GFX_LINESIZE - XPos;
    if (XPos < 0) {
        sprX -= XPos;
        width += XPos;
        XPos = 0;
    }
    if (height + YPos > SCREEN_YSIZE)
        height = SCREEN_YSIZE - YPos;
    if (YPos < 0) {
        sprY -= YPos;
        height += YPos;
        YPos = 0;
    }
    if (width <= 0 || height <= 0 || alpha <= 0)
        return;

    ushort *subBlendTable  = &subtractLookupTable[BLENDTABLE_XSIZE * alpha];
    GFXSurface *surface    = &gfxSurface[sheetID];
    int pitch              = GFX_LINESIZE - width;
    int gfxPitch           = surface->width - width;
    byte *lineBuffer       = &gfxLineBuffer[YPos];
    byte *gfxData          = &graphicData[sprX + surface->width * sprY + surface->dataPosition];
    ushort *frameBufferPtr = &Engine.frameBuffer[XPos + GFX_LINESIZE * YPos];

    while (height--) {
        activePalette   = fullPalette[*lineBuffer];
        activePalette32 = fullPalette32[*lineBuffer];
        lineBuffer++;
        int w = width;
        while (w--) {
            if (*gfxData > 0) {
                ushort colour      = activePalette[*gfxData];
                ushort finalColour = 0;
                if ((*frameBufferPtr & 0xF800) - (subBlendTable[(colour & 0xF800) >> 11] << 11) <= 0)
                    finalColour = 0;
                else
                    finalColour = (*frameBufferPtr & 0xF800) - (subBlendTable[(colour & 0xF800) >> 11] << 11);
                int v12 = (*frameBufferPtr & 0x7E0) - (subBlendTable[(colour & 0x7E0) >> 6] << 6);
                if (v12 > 0)
                    finalColour |= v12;
                int v13 = (*frameBufferPtr & 0x1F) - subBlendTable[colour & 0x1F];
                if (v13 > 0)
                    finalColour |= v13;
                *frameBufferPtr = finalColour;
            }
            ++gfxData;
            ++frameBufferPtr;
        }
        frameBufferPtr += pitch;
        gfxData += gfxPitch;
    }
#endif

#if RETRO_HARDWARE_RENDER
    GFXSurface *surface = &gfxSurface[sheetID];
    if (surface->texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && XPos > -512 && XPos < 872 && YPos > -512 && YPos < 752) {
        gfxPolyList[gfxVertexSize].x        = XPos << 4;
        gfxPolyList[gfxVertexSize].y        = YPos << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = alpha;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX);
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = XPos + width << 4;
        gfxPolyList[gfxVertexSize].y        = YPos << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = alpha;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + sprX + width);
        gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = XPos << 4;
        gfxPolyList[gfxVertexSize].y        = YPos + height << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = alpha;
        gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + sprY + height);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = gfxPolyList[gfxVertexSize - 2].x;
        gfxPolyList[gfxVertexSize].y        = gfxPolyList[gfxVertexSize - 1].y;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = alpha;
        gfxPolyList[gfxVertexSize].u        = gfxPolyList[gfxVertexSize - 2].u;
        gfxPolyList[gfxVertexSize].v        = gfxPolyList[gfxVertexSize - 1].v;
        gfxVertexSize++;
        gfxIndexSize += 6;
    }
#endif
}

void DrawObjectAnimation(void *objScr, void *ent, int XPos, int YPos)
{
    ObjectScript *objectScript = (ObjectScript *)objScr;
    Entity *entity             = (Entity *)ent;
    SpriteAnimation *sprAnim   = &animationList[objectScript->animFile->aniListOffset + entity->animation];
    SpriteFrame *frame         = &animFrames[sprAnim->frameListOffset + entity->frame];
    int rotation               = 0;

    switch (sprAnim->rotationFlag) {
        case ROTFLAG_NONE:
            switch (entity->direction) {
                case FLIP_NONE:
                    DrawSpriteFlipped(frame->pivotX + XPos, frame->pivotY + YPos, frame->width, frame->height, frame->sprX, frame->sprY, FLIP_NONE,
                                      frame->sheetID);
                    break;
                case FLIP_X:
                    DrawSpriteFlipped(XPos - frame->width - frame->pivotX, frame->pivotY + YPos, frame->width, frame->height, frame->sprX,
                                      frame->sprY, FLIP_X, frame->sheetID);
                    break;
                case FLIP_Y:
                    DrawSpriteFlipped(frame->pivotX + XPos, YPos - frame->height - frame->pivotY, frame->width, frame->height, frame->sprX,
                                      frame->sprY, FLIP_Y, frame->sheetID);
                    break;
                case FLIP_XY:
                    DrawSpriteFlipped(XPos - frame->width - frame->pivotX, YPos - frame->height - frame->pivotY, frame->width, frame->height,
                                      frame->sprX, frame->sprY, FLIP_XY, frame->sheetID);
                    break;
                default: break;
            }
            break;
        case ROTFLAG_FULL:
            DrawSpriteRotated(entity->direction, XPos, YPos, -frame->pivotX, -frame->pivotY, frame->sprX, frame->sprY, frame->width, frame->height,
                              entity->rotation, frame->sheetID);
            break;
        case ROTFLAG_45DEG:
            if (entity->rotation >= 0x100)
                DrawSpriteRotated(entity->direction, XPos, YPos, -frame->pivotX, -frame->pivotY, frame->sprX, frame->sprY, frame->width,
                                  frame->height, 0x200 - ((0x214 - entity->rotation) >> 6 << 6), frame->sheetID);
            else
                DrawSpriteRotated(entity->direction, XPos, YPos, -frame->pivotX, -frame->pivotY, frame->sprX, frame->sprY, frame->width,
                                  frame->height, (entity->rotation + 20) >> 6 << 6, frame->sheetID);
            break;
        case ROTFLAG_STATICFRAMES: {
            if (entity->rotation >= 0x100)
                rotation = 8 - ((532 - entity->rotation) >> 6);
            else
                rotation = (entity->rotation + 20) >> 6;
            int frameID = entity->frame;
            switch (rotation) {
                case 0: // 0 deg
                case 8: // 360 deg
                    rotation = 0x00;
                    break;
                case 1: // 45 deg
                    frameID += sprAnim->frameCount;
                    if (entity->direction)
                        rotation = 0;
                    else
                        rotation = 0x80;
                    break;
                case 2: // 90 deg
                    rotation = 0x80;
                    break;
                case 3: // 135 deg
                    frameID += sprAnim->frameCount;
                    if (entity->direction)
                        rotation = 0x80;
                    else
                        rotation = 0x100;
                    break;
                case 4: // 180 deg
                    rotation = 0x100;
                    break;
                case 5: // 225 deg
                    frameID += sprAnim->frameCount;
                    if (entity->direction)
                        rotation = 0x100;
                    else
                        rotation = 384;
                    break;
                case 6: // 270 deg
                    rotation = 384;
                    break;
                case 7: // 315 deg
                    frameID += sprAnim->frameCount;
                    if (entity->direction)
                        rotation = 384;
                    else
                        rotation = 0;
                    break;
                default: break;
            }

            frame = &animFrames[sprAnim->frameListOffset + frameID];
            DrawSpriteRotated(entity->direction, XPos, YPos, -frame->pivotX, -frame->pivotY, frame->sprX, frame->sprY, frame->width, frame->height,
                              rotation, frame->sheetID);
            // DrawSpriteRotozoom(entity->direction, XPos, YPos, -frame->pivotX, -frame->pivotY, frame->sprX, frame->sprY, frame->width,
            // frame->height,
            //                  rotation, entity->scale, frame->sheetID);
            break;
        }
        default: break;
    }
}

void DrawFace(void *v, uint colour)
{
    Vertex *verts = (Vertex *)v;
    int alpha     = (colour & 0x7F000000) >> 23;
    if (alpha < 1)
        return;
    if (alpha > 0xFF)
        alpha = 0xFF;
    if (verts[0].x < 0 && verts[1].x < 0 && verts[2].x < 0 && verts[3].x < 0)
        return;
    if (verts[0].x > GFX_LINESIZE && verts[1].x > GFX_LINESIZE && verts[2].x > GFX_LINESIZE && verts[3].x > GFX_LINESIZE)
        return;
    if (verts[0].y < 0 && verts[1].y < 0 && verts[2].y < 0 && verts[3].y < 0)
        return;
    if (verts[0].y > SCREEN_YSIZE && verts[1].y > SCREEN_YSIZE && verts[2].y > SCREEN_YSIZE && verts[3].y > SCREEN_YSIZE)
        return;
    if (verts[0].x == verts[1].x && verts[1].x == verts[2].x && verts[2].x == verts[3].x)
        return;
    if (verts[0].y == verts[1].y && verts[1].y == verts[2].y && verts[2].y == verts[3].y)
        return;

#if RETRO_SOFTWARE_RENDER
    int vertexA = 0;
    int vertexB = 1;
    int vertexC = 2;
    int vertexD = 3;
    if (verts[1].y < verts[0].y) {
        vertexA = 1;
        vertexB = 0;
    }
    if (verts[2].y < verts[vertexA].y) {
        int temp = vertexA;
        vertexA  = 2;
        vertexC  = temp;
    }
    if (verts[3].y < verts[vertexA].y) {
        int temp = vertexA;
        vertexA  = 3;
        vertexD  = temp;
    }
    if (verts[vertexC].y < verts[vertexB].y) {
        int temp = vertexB;
        vertexB  = vertexC;
        vertexC  = temp;
    }
    if (verts[vertexD].y < verts[vertexB].y) {
        int temp = vertexB;
        vertexB  = vertexD;
        vertexD  = temp;
    }
    if (verts[vertexD].y < verts[vertexC].y) {
        int temp = vertexC;
        vertexC  = vertexD;
        vertexD  = temp;
    }

    int faceTop    = verts[vertexA].y;
    int faceBottom = verts[vertexD].y;
    if (faceTop < 0)
        faceTop = 0;
    if (faceBottom > SCREEN_YSIZE)
        faceBottom = SCREEN_YSIZE;
    for (int i = faceTop; i < faceBottom; ++i) {
        faceLineStart[i] = 100000;
        faceLineEnd[i]   = -100000;
    }

    processScanEdge(&verts[vertexA], &verts[vertexB]);
    processScanEdge(&verts[vertexA], &verts[vertexC]);
    processScanEdge(&verts[vertexA], &verts[vertexD]);
    processScanEdge(&verts[vertexB], &verts[vertexC]);
    processScanEdge(&verts[vertexC], &verts[vertexD]);
    processScanEdge(&verts[vertexB], &verts[vertexD]);

    ushort colour16 = PACK_RGB888(((colour >> 16) & 0xFF), ((colour >> 8) & 0xFF), ((colour >> 0) & 0xFF));

    ushort *frameBufferPtr = &Engine.frameBuffer[GFX_LINESIZE * faceTop];
    if (alpha == 255) {
        while (faceTop < faceBottom) {
            int startX = faceLineStart[faceTop];
            int endX   = faceLineEnd[faceTop];
            if (startX >= GFX_LINESIZE || endX <= 0) {
                frameBufferPtr += GFX_LINESIZE;
            }
            else {
                if (startX < 0)
                    startX = 0;
                if (endX > GFX_LINESIZE_MINUSONE)
                    endX = GFX_LINESIZE_MINUSONE;
                ushort *fbPtr = &frameBufferPtr[startX];
                frameBufferPtr += GFX_LINESIZE;
                int vertexwidth = endX - startX + 1;
                while (vertexwidth--) {
                    *fbPtr = colour16;
                    ++fbPtr;
                }
            }
            ++faceTop;
        }
    }
    else {
        while (faceTop < faceBottom) {
            int startX = faceLineStart[faceTop];
            int endX   = faceLineEnd[faceTop];
            if (startX >= GFX_LINESIZE || endX <= 0) {
                frameBufferPtr += GFX_LINESIZE;
            }
            else {
                if (startX < 0)
                    startX = 0;
                if (endX > GFX_LINESIZE_MINUSONE)
                    endX = GFX_LINESIZE_MINUSONE;
                ushort *fbPtr = &frameBufferPtr[startX];
                frameBufferPtr += GFX_LINESIZE;
                int vertexwidth = endX - startX + 1;
                while (vertexwidth--) {
                    setPixelAlpha(colour16, *fbPtr, alpha);
                    ++fbPtr;
                }
            }
            ++faceTop;
        }
    }
#endif

#if RETRO_HARDWARE_RENDER
    if (gfxVertexSize < VERTEX_LIMIT) {
        gfxPolyList[gfxVertexSize].x        = verts[0].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[0].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = ((colour >> 16) & 0xFF);
        gfxPolyList[gfxVertexSize].colour.g = ((colour >> 8) & 0xFF);
        gfxPolyList[gfxVertexSize].colour.b = ((colour >> 0) & 0xFF);

        if (alpha > 0xFD) {
            gfxPolyList[gfxVertexSize].colour.a = 0xFF;
        }
        else {
            gfxPolyList[gfxVertexSize].colour.a = alpha;
        }

        gfxPolyList[gfxVertexSize].u = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxPolyList[gfxVertexSize].v = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = verts[1].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[1].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = gfxPolyList[gfxVertexSize - 1].colour.r;
        gfxPolyList[gfxVertexSize].colour.g = gfxPolyList[gfxVertexSize - 1].colour.g;
        gfxPolyList[gfxVertexSize].colour.b = gfxPolyList[gfxVertexSize - 1].colour.b;
        gfxPolyList[gfxVertexSize].colour.a = gfxPolyList[gfxVertexSize - 1].colour.a;
        gfxPolyList[gfxVertexSize].u        = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxPolyList[gfxVertexSize].v        = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = verts[2].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[2].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = gfxPolyList[gfxVertexSize - 1].colour.r;
        gfxPolyList[gfxVertexSize].colour.g = gfxPolyList[gfxVertexSize - 1].colour.g;
        gfxPolyList[gfxVertexSize].colour.b = gfxPolyList[gfxVertexSize - 1].colour.b;
        gfxPolyList[gfxVertexSize].colour.a = gfxPolyList[gfxVertexSize - 1].colour.a;
        gfxPolyList[gfxVertexSize].u        = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxPolyList[gfxVertexSize].v        = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = verts[3].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[3].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = gfxPolyList[gfxVertexSize - 1].colour.r;
        gfxPolyList[gfxVertexSize].colour.g = gfxPolyList[gfxVertexSize - 1].colour.g;
        gfxPolyList[gfxVertexSize].colour.b = gfxPolyList[gfxVertexSize - 1].colour.b;
        gfxPolyList[gfxVertexSize].colour.a = gfxPolyList[gfxVertexSize - 1].colour.a;
        gfxPolyList[gfxVertexSize].u        = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxPolyList[gfxVertexSize].v        = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxVertexSize++;

        gfxIndexSize += 6;
    }
#endif
}
void DrawFadedFace(void *v, uint colour, uint fogColour, int alpha)
{
    Vertex *verts = (Vertex *)v;
    if (alpha > 0xFF)
        alpha = 0xFF;

    if (alpha < 1)
        return;
    if (verts[0].x < 0 && verts[1].x < 0 && verts[2].x < 0 && verts[3].x < 0)
        return;
    if (verts[0].x > GFX_LINESIZE && verts[1].x > GFX_LINESIZE && verts[2].x > GFX_LINESIZE && verts[3].x > GFX_LINESIZE)
        return;
    if (verts[0].y < 0 && verts[1].y < 0 && verts[2].y < 0 && verts[3].y < 0)
        return;
    if (verts[0].y > SCREEN_YSIZE && verts[1].y > SCREEN_YSIZE && verts[2].y > SCREEN_YSIZE && verts[3].y > SCREEN_YSIZE)
        return;
    if (verts[0].x == verts[1].x && verts[1].x == verts[2].x && verts[2].x == verts[3].x)
        return;
    if (verts[0].y == verts[1].y && verts[1].y == verts[2].y && verts[2].y == verts[3].y)
        return;

#if RETRO_SOFTWARE_RENDER
    int vertexA = 0;
    int vertexB = 1;
    int vertexC = 2;
    int vertexD = 3;
    if (verts[1].y < verts[0].y) {
        vertexA = 1;
        vertexB = 0;
    }
    if (verts[2].y < verts[vertexA].y) {
        int temp = vertexA;
        vertexA  = 2;
        vertexC  = temp;
    }
    if (verts[3].y < verts[vertexA].y) {
        int temp = vertexA;
        vertexA  = 3;
        vertexD  = temp;
    }
    if (verts[vertexC].y < verts[vertexB].y) {
        int temp = vertexB;
        vertexB  = vertexC;
        vertexC  = temp;
    }
    if (verts[vertexD].y < verts[vertexB].y) {
        int temp = vertexB;
        vertexB  = vertexD;
        vertexD  = temp;
    }
    if (verts[vertexD].y < verts[vertexC].y) {
        int temp = vertexC;
        vertexC  = vertexD;
        vertexD  = temp;
    }

    int faceTop    = verts[vertexA].y;
    int faceBottom = verts[vertexD].y;
    if (faceTop < 0)
        faceTop = 0;
    if (faceBottom > SCREEN_YSIZE)
        faceBottom = SCREEN_YSIZE;
    for (int i = faceTop; i < faceBottom; ++i) {
        faceLineStart[i] = 100000;
        faceLineEnd[i]   = -100000;
    }

    processScanEdge(&verts[vertexA], &verts[vertexB]);
    processScanEdge(&verts[vertexA], &verts[vertexC]);
    processScanEdge(&verts[vertexA], &verts[vertexD]);
    processScanEdge(&verts[vertexB], &verts[vertexC]);
    processScanEdge(&verts[vertexC], &verts[vertexD]);
    processScanEdge(&verts[vertexB], &verts[vertexD]);

    ushort colour16    = PACK_RGB888(((colour >> 16) & 0xFF), ((colour >> 8) & 0xFF), ((colour >> 0) & 0xFF));
    ushort fogColour16 = PACK_RGB888(((fogColour >> 16) & 0xFF), ((fogColour >> 8) & 0xFF), ((fogColour >> 0) & 0xFF));

    ushort *frameBufferPtr = &Engine.frameBuffer[GFX_LINESIZE * faceTop];
    while (faceTop < faceBottom) {
        int startX = faceLineStart[faceTop];
        int endX   = faceLineEnd[faceTop];
        if (startX >= GFX_LINESIZE || endX <= 0) {
            frameBufferPtr += GFX_LINESIZE;
        }
        else {
            if (startX < 0)
                startX = 0;
            if (endX > GFX_LINESIZE_MINUSONE)
                endX = GFX_LINESIZE_MINUSONE;
            ushort *fbPtr = &frameBufferPtr[startX];
            frameBufferPtr += GFX_LINESIZE;
            int vertexwidth = endX - startX + 1;
            while (vertexwidth--) {
                ushort *blendTableA = &blendLookupTable[BLENDTABLE_XSIZE * ((BLENDTABLE_YSIZE - 1) - alpha)];
                ushort *blendTableB = &blendLookupTable[BLENDTABLE_XSIZE * alpha];
                *fbPtr              = (blendTableA[fogColour16 & 0x1F] + blendTableB[colour16 & 0x1F])
                         | ((blendTableA[(fogColour16 & 0x7E0) >> 6] + blendTableB[(colour16 & 0x7E0) >> 6]) << 6)
                         | ((blendTableA[(fogColour16 & 0xF800) >> 11] + blendTableB[(colour16 & 0xF800) >> 11]) << 11);
                ++fbPtr;
            }
        }
        ++faceTop;
    }
#endif

#if RETRO_HARDWARE_RENDER
    if (gfxVertexSize < VERTEX_LIMIT) {
        byte cr = ((colour >> 16) & 0xFF);
        byte cg = ((colour >> 8) & 0xFF);
        byte cb = ((colour >> 0) & 0xFF);
        byte fr = ((fogColour >> 16) & 0xFF);
        byte fg = ((fogColour >> 8) & 0xFF);
        byte fb = ((fogColour >> 0) & 0xFF);

        gfxPolyList[gfxVertexSize].x        = verts[0].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[0].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = ((ushort)(fr * (0xFF - alpha) + alpha * cr) >> 8);
        gfxPolyList[gfxVertexSize].colour.g = ((ushort)(fg * (0xFF - alpha) + alpha * cg) >> 8);
        gfxPolyList[gfxVertexSize].colour.b = ((ushort)(fb * (0xFF - alpha) + alpha * cb) >> 8);
        gfxPolyList[gfxVertexSize].colour.a = 0xFF;
        gfxPolyList[gfxVertexSize].u        = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxPolyList[gfxVertexSize].v        = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = verts[1].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[1].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = gfxPolyList[gfxVertexSize - 1].colour.r;
        gfxPolyList[gfxVertexSize].colour.g = gfxPolyList[gfxVertexSize - 1].colour.g;
        gfxPolyList[gfxVertexSize].colour.b = gfxPolyList[gfxVertexSize - 1].colour.b;
        gfxPolyList[gfxVertexSize].colour.a = gfxPolyList[gfxVertexSize - 1].colour.a;
        gfxPolyList[gfxVertexSize].u        = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxPolyList[gfxVertexSize].v        = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = verts[3].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[3].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = gfxPolyList[gfxVertexSize - 1].colour.r;
        gfxPolyList[gfxVertexSize].colour.g = gfxPolyList[gfxVertexSize - 1].colour.g;
        gfxPolyList[gfxVertexSize].colour.b = gfxPolyList[gfxVertexSize - 1].colour.b;
        gfxPolyList[gfxVertexSize].colour.a = gfxPolyList[gfxVertexSize - 1].colour.a;
        gfxPolyList[gfxVertexSize].u        = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxPolyList[gfxVertexSize].v        = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = verts[2].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[2].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = gfxPolyList[gfxVertexSize - 1].colour.r;
        gfxPolyList[gfxVertexSize].colour.g = gfxPolyList[gfxVertexSize - 1].colour.g;
        gfxPolyList[gfxVertexSize].colour.b = gfxPolyList[gfxVertexSize - 1].colour.b;
        gfxPolyList[gfxVertexSize].colour.a = gfxPolyList[gfxVertexSize - 1].colour.a;
        gfxPolyList[gfxVertexSize].u        = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxPolyList[gfxVertexSize].v        = 10.24f; // 0.01f, scaled upto pixels vs floats
        gfxVertexSize++;

        gfxIndexSize += 6;
    }
#endif
}
void DrawTexturedFace(void *v, byte sheetID)
{
    Vertex *verts = (Vertex *)v;

    if (verts[0].x < 0 && verts[1].x < 0 && verts[2].x < 0 && verts[3].x < 0)
        return;
    if (verts[0].x > GFX_LINESIZE && verts[1].x > GFX_LINESIZE && verts[2].x > GFX_LINESIZE && verts[3].x > GFX_LINESIZE)
        return;
    if (verts[0].y < 0 && verts[1].y < 0 && verts[2].y < 0 && verts[3].y < 0)
        return;
    if (verts[0].y > SCREEN_YSIZE && verts[1].y > SCREEN_YSIZE && verts[2].y > SCREEN_YSIZE && verts[3].y > SCREEN_YSIZE)
        return;
    if (verts[0].x == verts[1].x && verts[1].x == verts[2].x && verts[2].x == verts[3].x)
        return;
    if (verts[0].y == verts[1].y && verts[1].y == verts[2].y && verts[2].y == verts[3].y)
        return;

#if RETRO_SOFTWARE_RENDER
    int vertexA = 0;
    int vertexB = 1;
    int vertexC = 2;
    int vertexD = 3;
    if (verts[1].y < verts[0].y) {
        vertexA = 1;
        vertexB = 0;
    }
    if (verts[2].y < verts[vertexA].y) {
        int temp = vertexA;
        vertexA  = 2;
        vertexC  = temp;
    }
    if (verts[3].y < verts[vertexA].y) {
        int temp = vertexA;
        vertexA  = 3;
        vertexD  = temp;
    }
    if (verts[vertexC].y < verts[vertexB].y) {
        int temp = vertexB;
        vertexB  = vertexC;
        vertexC  = temp;
    }
    if (verts[vertexD].y < verts[vertexB].y) {
        int temp = vertexB;
        vertexB  = vertexD;
        vertexD  = temp;
    }
    if (verts[vertexD].y < verts[vertexC].y) {
        int temp = vertexC;
        vertexC  = vertexD;
        vertexD  = temp;
    }

    int faceTop    = verts[vertexA].y;
    int faceBottom = verts[vertexD].y;
    if (faceTop < 0)
        faceTop = 0;
    if (faceBottom > SCREEN_YSIZE)
        faceBottom = SCREEN_YSIZE;
    for (int i = faceTop; i < faceBottom; ++i) {
        faceLineStart[i] = 100000;
        faceLineEnd[i]   = -100000;
    }

    processScanEdgeUV(&verts[vertexA], &verts[vertexB]);
    processScanEdgeUV(&verts[vertexA], &verts[vertexC]);
    processScanEdgeUV(&verts[vertexA], &verts[vertexD]);
    processScanEdgeUV(&verts[vertexB], &verts[vertexC]);
    processScanEdgeUV(&verts[vertexC], &verts[vertexD]);
    processScanEdgeUV(&verts[vertexB], &verts[vertexD]);

    ushort *frameBufferPtr = &Engine.frameBuffer[GFX_LINESIZE * faceTop];
    byte *sheetPtr         = &graphicData[gfxSurface[sheetID].dataPosition];
    int shiftwidth         = gfxSurface[sheetID].widthShift;
    byte *lineBuffer       = &gfxLineBuffer[faceTop];
    while (faceTop < faceBottom) {
        activePalette   = fullPalette[*lineBuffer];
        activePalette32 = fullPalette32[*lineBuffer];
        lineBuffer++;
        int startX = faceLineStart[faceTop];
        int endX   = faceLineEnd[faceTop];
        int UPos   = faceLineStartU[faceTop];
        int VPos   = faceLineStartV[faceTop];
        if (startX >= GFX_LINESIZE || endX <= 0) {
            frameBufferPtr += GFX_LINESIZE;
        }
        else {
            int posDifference = endX - startX;
            int bufferedUPos  = 0;
            int bufferedVPos  = 0;
            if (endX == startX) {
                bufferedUPos = 0;
                bufferedVPos = 0;
            }
            else {
                bufferedUPos = (faceLineEndU[faceTop] - UPos) / posDifference;
                bufferedVPos = (faceLineEndV[faceTop] - VPos) / posDifference;
            }
            if (endX > GFX_LINESIZE_MINUSONE)
                posDifference = GFX_LINESIZE_MINUSONE - startX;
            if (startX < 0) {
                posDifference += startX;
                UPos -= startX * bufferedUPos;
                VPos -= startX * bufferedVPos;
                startX = 0;
            }
            ushort *fbPtr = &frameBufferPtr[startX];
            frameBufferPtr += GFX_LINESIZE;
            int counter = posDifference + 1;
            while (counter--) {
                if (UPos < 0)
                    UPos = 0;
                if (VPos < 0)
                    VPos = 0;
                ushort index = sheetPtr[(VPos >> 16 << shiftwidth) + (UPos >> 16)];
                if (index > 0)
                    *fbPtr = activePalette[index];
                fbPtr++;
                UPos += bufferedUPos;
                VPos += bufferedVPos;
            }
        }
        ++faceTop;
    }
#endif

#if RETRO_HARDWARE_RENDER
    GFXSurface *surface = &gfxSurface[sheetID];
    if (gfxVertexSize < VERTEX_LIMIT) {
        gfxPolyList[gfxVertexSize].x        = verts[0].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[0].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0xFF;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + verts[0].u);
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + verts[0].v);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = verts[1].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[1].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0xFF;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + verts[1].u);
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + verts[1].v);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = verts[2].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[2].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0xFF;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + verts[2].u);
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + verts[2].v);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = verts[3].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[3].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0xFF;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + verts[3].u);
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + verts[3].v);
        gfxVertexSize++;

        gfxIndexSize += 6;
    }
#endif
}
void DrawTexturedFaceBlended(void *v, byte sheetID)
{
    Vertex *verts = (Vertex *)v;
    if (verts[0].x < 0 && verts[1].x < 0 && verts[2].x < 0 && verts[3].x < 0)
        return;
    if (verts[0].x > GFX_LINESIZE && verts[1].x > GFX_LINESIZE && verts[2].x > GFX_LINESIZE && verts[3].x > GFX_LINESIZE)
        return;
    if (verts[0].y < 0 && verts[1].y < 0 && verts[2].y < 0 && verts[3].y < 0)
        return;
    if (verts[0].y > SCREEN_YSIZE && verts[1].y > SCREEN_YSIZE && verts[2].y > SCREEN_YSIZE && verts[3].y > SCREEN_YSIZE)
        return;
    if (verts[0].x == verts[1].x && verts[1].x == verts[2].x && verts[2].x == verts[3].x)
        return;
    if (verts[0].y == verts[1].y && verts[1].y == verts[2].y && verts[2].y == verts[3].y)
        return;

#if RETRO_SOFTWARE_RENDER
    int vertexA = 0;
    int vertexB = 1;
    int vertexC = 2;
    int vertexD = 3;
    if (verts[1].y < verts[0].y) {
        vertexA = 1;
        vertexB = 0;
    }
    if (verts[2].y < verts[vertexA].y) {
        int temp = vertexA;
        vertexA  = 2;
        vertexC  = temp;
    }
    if (verts[3].y < verts[vertexA].y) {
        int temp = vertexA;
        vertexA  = 3;
        vertexD  = temp;
    }
    if (verts[vertexC].y < verts[vertexB].y) {
        int temp = vertexB;
        vertexB  = vertexC;
        vertexC  = temp;
    }
    if (verts[vertexD].y < verts[vertexB].y) {
        int temp = vertexB;
        vertexB  = vertexD;
        vertexD  = temp;
    }
    if (verts[vertexD].y < verts[vertexC].y) {
        int temp = vertexC;
        vertexC  = vertexD;
        vertexD  = temp;
    }

    int faceTop    = verts[vertexA].y;
    int faceBottom = verts[vertexD].y;
    if (faceTop < 0)
        faceTop = 0;
    if (faceBottom > SCREEN_YSIZE)
        faceBottom = SCREEN_YSIZE;
    for (int i = faceTop; i < faceBottom; ++i) {
        faceLineStart[i] = 100000;
        faceLineEnd[i]   = -100000;
    }

    processScanEdgeUV(&verts[vertexA], &verts[vertexB]);
    processScanEdgeUV(&verts[vertexA], &verts[vertexC]);
    processScanEdgeUV(&verts[vertexA], &verts[vertexD]);
    processScanEdgeUV(&verts[vertexB], &verts[vertexC]);
    processScanEdgeUV(&verts[vertexC], &verts[vertexD]);
    processScanEdgeUV(&verts[vertexB], &verts[vertexD]);

    ushort *frameBufferPtr = &Engine.frameBuffer[GFX_LINESIZE * faceTop];
    byte *sheetPtr         = &graphicData[gfxSurface[sheetID].dataPosition];
    int shiftwidth         = gfxSurface[sheetID].widthShift;
    byte *lineBuffer       = &gfxLineBuffer[faceTop];
    while (faceTop < faceBottom) {
        activePalette   = fullPalette[*lineBuffer];
        activePalette32 = fullPalette32[*lineBuffer];
        lineBuffer++;
        int startX = faceLineStart[faceTop];
        int endX   = faceLineEnd[faceTop];
        int UPos   = faceLineStartU[faceTop];
        int VPos   = faceLineStartV[faceTop];
        if (startX >= GFX_LINESIZE || endX <= 0) {
            frameBufferPtr += GFX_LINESIZE;
        }
        else {
            int posDifference = endX - startX;
            int bufferedUPos  = 0;
            int bufferedVPos  = 0;
            if (endX == startX) {
                bufferedUPos = 0;
                bufferedVPos = 0;
            }
            else {
                bufferedUPos = (faceLineEndU[faceTop] - UPos) / posDifference;
                bufferedVPos = (faceLineEndV[faceTop] - VPos) / posDifference;
            }
            if (endX > GFX_LINESIZE_MINUSONE)
                posDifference = GFX_LINESIZE_MINUSONE - startX;
            if (startX < 0) {
                posDifference += startX;
                UPos -= startX * bufferedUPos;
                VPos -= startX * bufferedVPos;
                startX = 0;
            }
            ushort *fbPtr = &frameBufferPtr[startX];
            frameBufferPtr += GFX_LINESIZE;
            int counter = posDifference + 1;
            while (counter--) {
                if (UPos < 0)
                    UPos = 0;
                if (VPos < 0)
                    VPos = 0;
                ushort index = sheetPtr[(VPos >> 16 << shiftwidth) + (UPos >> 16)];
                if (index > 0)
                    *fbPtr = ((activePalette[index] & 0xF7BC) >> 1) + ((*fbPtr & 0xF7BC) >> 1);
                fbPtr++;
                UPos += bufferedUPos;
                VPos += bufferedVPos;
            }
        }
        ++faceTop;
    }
#endif

#if RETRO_HARDWARE_RENDER
    GFXSurface *surface = &gfxSurface[sheetID];
    if (gfxVertexSize < VERTEX_LIMIT) {
        gfxPolyList[gfxVertexSize].x        = verts[0].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[0].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0x80;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + verts[0].u);
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + verts[0].v);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = verts[1].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[1].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0x80;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + verts[1].u);
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + verts[1].v);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = verts[2].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[2].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0x80;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + verts[2].u);
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + verts[2].v);
        gfxVertexSize++;

        gfxPolyList[gfxVertexSize].x        = verts[3].x << 4;
        gfxPolyList[gfxVertexSize].y        = verts[3].y << 4;
        gfxPolyList[gfxVertexSize].colour.r = 0xFF;
        gfxPolyList[gfxVertexSize].colour.g = 0xFF;
        gfxPolyList[gfxVertexSize].colour.b = 0xFF;
        gfxPolyList[gfxVertexSize].colour.a = 0x80;
        gfxPolyList[gfxVertexSize].u        = (surface->texStartX + verts[3].u);
        gfxPolyList[gfxVertexSize].v        = (surface->texStartY + verts[3].v);
        gfxVertexSize++;

        gfxIndexSize += 6;
    }
#endif
}

#if RETRO_REV01
void DrawBitmapText(void *menu, int XPos, int YPos, int scale, int spacing, int rowStart, int rowCount)
{
    TextMenu *tMenu = (TextMenu *)menu;
    int Y           = YPos << 9;
    if (rowCount < 0)
        rowCount = tMenu->rowCount;
    if (rowStart + rowCount > tMenu->rowCount)
        rowCount = tMenu->rowCount - rowStart;

    while (rowCount > 0) {
        int X = XPos << 9;
        for (int i = 0; i < tMenu->entrySize[rowStart]; ++i) {
            ushort c             = tMenu->textData[tMenu->entryStart[rowStart] + i];
            FontCharacter *fChar = &fontCharacterList[c];
#if RETRO_SOFTWARE_RENDER
            DrawSpriteScaled(FLIP_NONE, X >> 9, Y >> 9, -fChar->pivotX, -fChar->pivotY, scale, scale, fChar->width, fChar->height, fChar->srcX,
                             fChar->srcY, textMenuSurfaceNo);
#endif
#if RETRO_HARDWARE_RENDER
            DrawScaledChar(FLIP_NONE, X >> 5, Y >> 5, -fChar->pivotX, -fChar->pivotY, scale, scale, fChar->width, fChar->height, fChar->srcX,
                           fChar->srcY, textMenuSurfaceNo);
#endif
            X += fChar->xAdvance * scale;
        }
        Y += spacing * scale;
        rowStart++;
        rowCount--;
    }
}
#endif

void DrawTextMenuEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight)
{
    TextMenu *tMenu = (TextMenu *)menu;
    int id          = tMenu->entryStart[rowID];
    for (int i = 0; i < tMenu->entrySize[rowID]; ++i) {
        DrawSprite(XPos + (i << 3) - (((tMenu->entrySize[rowID] % 2) & (tMenu->alignment == 2)) * 4), YPos, 8, 8,
                   (int)((int)(tMenu->textData[id] & 0xF) << 3), (int)((int)(tMenu->textData[id] >> 4) << 3) + textHighlight, textMenuSurfaceNo);
        id++;
    }
}
void DrawStageTextEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight)
{
    TextMenu *tMenu = (TextMenu *)menu;
    int id          = tMenu->entryStart[rowID];
    for (int i = 0; i < tMenu->entrySize[rowID]; ++i) {
        if (i == tMenu->entrySize[rowID] - 1) {
            DrawSprite(XPos + (i << 3), YPos, 8, 8, (int)((int)(tMenu->textData[id] & 0xF) << 3), (int)((int)(tMenu->textData[id] >> 4) << 3),
                       textMenuSurfaceNo);
        }
        else {
            DrawSprite(XPos + (i << 3), YPos, 8, 8, (int)((int)(tMenu->textData[id] & 0xF) << 3),
                       (int)((int)(tMenu->textData[id] >> 4) << 3) + textHighlight, textMenuSurfaceNo);
        }
        id++;
    }
}
void DrawBlendedTextMenuEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight)
{
    TextMenu *tMenu = (TextMenu *)menu;
    int id          = tMenu->entryStart[rowID];
    for (int i = 0; i < tMenu->entrySize[rowID]; ++i) {
        DrawBlendedSprite(XPos + (i << 3), YPos, 8, 8, (int)((int)(tMenu->textData[id] & 0xF) << 3),
                          (int)((int)(tMenu->textData[id] >> 4) << 3) + textHighlight, textMenuSurfaceNo);
        id++;
    }
}
void DrawTextMenu(void *menu, int XPos, int YPos)
{
    TextMenu *tMenu = (TextMenu *)menu;
    int cnt         = 0;

    if (tMenu->visibleRowCount > 0) {
        cnt = (int)(tMenu->visibleRowCount + tMenu->visibleRowOffset);
    }
    else {
        tMenu->visibleRowOffset = 0;
        cnt                     = (int)tMenu->rowCount;
    }
    if (tMenu->selectionCount == 3) {
        tMenu->selection2 = -1;
        for (int i = 0; i < tMenu->selection1 + 1; ++i) {
            if (tMenu->entryHighlight[i]) {
                tMenu->selection2 = i;
            }
        }
    }
    switch (tMenu->alignment) {
        case 0:
            for (int i = (int)tMenu->visibleRowOffset; i < cnt; ++i) {
                switch (tMenu->selectionCount) {
                    case 1:
                        if (i == tMenu->selection1)
                            DrawTextMenuEntry(tMenu, i, XPos, YPos, 128);
                        else
                            DrawTextMenuEntry(tMenu, i, XPos, YPos, 0);
                        break;
                    case 2:
                        if (i == tMenu->selection1 || i == tMenu->selection2)
                            DrawTextMenuEntry(tMenu, i, XPos, YPos, 128);
                        else
                            DrawTextMenuEntry(tMenu, i, XPos, YPos, 0);
                        break;
                    case 3:
                        if (i == tMenu->selection1)
                            DrawTextMenuEntry(tMenu, i, XPos, YPos, 128);
                        else
                            DrawTextMenuEntry(tMenu, i, XPos, YPos, 0);
                        if (i == tMenu->selection2 && i != tMenu->selection1)
                            DrawStageTextEntry(tMenu, i, XPos, YPos, 128);
                        break;
                }
                YPos += 8;
            }
            return;
        case 1:
            for (int i = (int)tMenu->visibleRowOffset; i < cnt; ++i) {
                int XPos2 = XPos - (tMenu->entrySize[i] << 3);
                switch (tMenu->selectionCount) {
                    case 1:
                        if (i == tMenu->selection1)
                            DrawTextMenuEntry(tMenu, i, XPos2, YPos, 128);
                        else
                            DrawTextMenuEntry(tMenu, i, XPos2, YPos, 0);
                        break;
                    case 2:
                        if (i == tMenu->selection1 || i == tMenu->selection2)
                            DrawTextMenuEntry(tMenu, i, XPos2, YPos, 128);
                        else
                            DrawTextMenuEntry(tMenu, i, XPos2, YPos, 0);
                        break;
                    case 3:
                        if (i == tMenu->selection1)
                            DrawTextMenuEntry(tMenu, i, XPos2, YPos, 128);
                        else
                            DrawTextMenuEntry(tMenu, i, XPos2, YPos, 0);
                        if (i == tMenu->selection2 && i != tMenu->selection1)
                            DrawStageTextEntry(tMenu, i, XPos2, YPos, 128);
                        break;
                }
                YPos += 8;
            }
            return;
        case 2:
            for (int i = (int)tMenu->visibleRowOffset; i < cnt; ++i) {
                int XPos2 = XPos - (tMenu->entrySize[i] >> 1 << 3);
                switch (tMenu->selectionCount) {
                    case 1:
                        if (i == tMenu->selection1)
                            DrawTextMenuEntry(tMenu, i, XPos2, YPos, 128);
                        else
                            DrawTextMenuEntry(tMenu, i, XPos2, YPos, 0);
                        break;
                    case 2:
                        if (i == tMenu->selection1 || i == tMenu->selection2)
                            DrawTextMenuEntry(tMenu, i, XPos2, YPos, 128);
                        else
                            DrawTextMenuEntry(tMenu, i, XPos2, YPos, 0);
                        break;
                    case 3:
                        if (i == tMenu->selection1)
                            DrawTextMenuEntry(tMenu, i, XPos2, YPos, 128);
                        else
                            DrawTextMenuEntry(tMenu, i, XPos2, YPos, 0);

                        if (i == tMenu->selection2 && i != tMenu->selection1)
                            DrawStageTextEntry(tMenu, i, XPos2, YPos, 128);
                        break;
                }
                YPos += 8;
            }
            return;
        default: return;
    }
}
