#include "RetroEngine.hpp"

#if RETRO_USE_V6
void loadTextureAll()
{
    // Load all textures, because it skips the Title Screen (and in the decomp, skips SegaSplash, which also loads the texture like the Title Screen)
     ResetBitmapFonts();
    int heading = 0, labelTex = 0, textTex = 0;

    if (Engine.useHighResAssets)
        heading = LoadTexture("Data/Game/Menu/Heading_EN.png", TEXFMT_RGBA4444);
    else
        heading = LoadTexture("Data/Game/Menu/Heading_EN@1x.png", TEXFMT_RGBA4444);

    LoadBitmapFont("Data/Game/Menu/Heading_EN.fnt", FONT_HEADING, heading);

    if (Engine.useHighResAssets)
        labelTex = LoadTexture("Data/Game/Menu/Label_EN.png", TEXFMT_RGBA4444);
    else
        labelTex = LoadTexture("Data/Game/Menu/Label_EN@1x.png", TEXFMT_RGBA4444);
    LoadBitmapFont("Data/Game/Menu/Label_EN.fnt", FONT_LABEL, labelTex);

    textTex = LoadTexture("Data/Game/Menu/Text_EN.png", TEXFMT_RGBA4444);
    LoadBitmapFont("Data/Game/Menu/Text_EN.fnt", FONT_TEXT, textTex);

    switch (Engine.language) {
        case RETRO_JP:
            heading = LoadTexture("Data/Game/Menu/Heading_JA@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Heading_JA.fnt", FONT_HEADING, heading);

            labelTex = LoadTexture("Data/Game/Menu/Label_JA@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Label_JA.fnt", FONT_LABEL, labelTex);

            textTex = LoadTexture("Data/Game/Menu/Text_JA@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Text_JA.fnt", FONT_TEXT, textTex);
            break;
        case RETRO_RU:
            if (Engine.useHighResAssets)
                heading = LoadTexture("Data/Game/Menu/Heading_RU.png", TEXFMT_RGBA4444);
            else
                heading = LoadTexture("Data/Game/Menu/Heading_RU@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Heading_RU.fnt", FONT_HEADING, heading);

            if (Engine.useHighResAssets)
                labelTex = LoadTexture("Data/Game/Menu/Label_RU.png", TEXFMT_RGBA4444);
            else
                labelTex = LoadTexture("Data/Game/Menu/Label_RU@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Label_RU.fnt", FONT_LABEL, labelTex);
            break;
        case RETRO_KO:
            heading = LoadTexture("Data/Game/Menu/Heading_KO@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Heading_KO.fnt", FONT_HEADING, heading);

            labelTex = LoadTexture("Data/Game/Menu/Label_KO@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Label_KO.fnt", FONT_LABEL, labelTex);

            textTex = LoadTexture("Data/Game/Menu/Text_KO.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Text_KO.fnt", FONT_TEXT, textTex);
            break;
        case RETRO_ZH:
            heading = LoadTexture("Data/Game/Menu/Heading_ZH@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Heading_ZH.fnt", FONT_HEADING, heading);

            labelTex = LoadTexture("Data/Game/Menu/Label_ZH@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Label_ZH.fnt", FONT_LABEL, labelTex);

            textTex = LoadTexture("Data/Game/Menu/Text_ZH@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Text_ZH.fnt", FONT_TEXT, textTex);
            break;
        case RETRO_ZS:
            heading = LoadTexture("Data/Game/Menu/Heading_ZHS@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Heading_ZHS.fnt", FONT_HEADING, heading);
            labelTex = LoadTexture("Data/Game/Menu/Label_ZHS@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Label_ZHS.fnt", FONT_LABEL, labelTex);
            textTex = LoadTexture("Data/Game/Menu/Text_ZHS@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Text_ZHS.fnt", FONT_TEXT, textTex);
            break;
        default: break;
    }
    SetMusicTrack("MenuIntro.ogg", 0, false, 0);
    SetMusicTrack("MainMenu.ogg", 1, true, 106596);
        LoadTexture("Data/Game/Menu/Circle.png", TEXFMT_RGBA4444);
    LoadTexture("Data/Game/Menu/BG1.png", TEXFMT_RGBA4444);
    LoadTexture("Data/Game/Menu/ArrowButtons.png", TEXFMT_RGBA4444);
    if (Engine.gameDeviceType == RETRO_MOBILE)
        LoadTexture("Data/Game/Menu/VirtualDPad.png", TEXFMT_RGBA8888);
    else
        LoadTexture("Data/Game/Menu/Amazon.png", TEXFMT_RGBA8888);
    LoadTexture("Data/Game/Menu/PlayerSelect.png", TEXFMT_RGBA8888);
    LoadTexture("Data/Game/Menu/SegaID.png", TEXFMT_RGBA8888);
}
#endif

void CWSplash_Create(void *objPtr)
{
    RSDK_THIS(CWSplash);
    self->state     = CWSPLASH_STATE_ENTER;
    self->rectAlpha = 320.0;
    self->textureID = LoadTexture("Data/Game/Menu/CWLogo.png", TEXFMT_RGBA8888);
}
void CWSplash_Main(void *objPtr)
{
    RSDK_THIS(CWSplash);

    switch (self->state) {
        case CWSPLASH_STATE_ENTER:
            self->rectAlpha -= 300.0 * Engine.deltaTime;
            if (self->rectAlpha < -320.0)
                self->state = CWSPLASH_STATE_EXIT;
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0xFF, 0x90, 0x00, 0xFF);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImage(0.0, 0.0, 160.0, 0.25, 0.25, 512.0, 256.0, 1024.0, 512.0, 0.0, 0.0, 255, self->textureID);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, self->rectAlpha);
            break;
        case CWSPLASH_STATE_EXIT:
            self->rectAlpha += 300.0 * Engine.deltaTime;
            if (self->rectAlpha > 512.0)
                self->state = CWSPLASH_STATE_SPAWNTITLE;
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0xFF, 0x90, 0x00, 0xFF);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImage(0.0, 0.0, 160.0, 0.25, 0.25, 512.0, 256.0, 1024.0, 512.0, 0.0, 0.0, 255, self->textureID);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, self->rectAlpha);
            break;
#if RETRO_USE_V6
        case CWSPLASH_STATE_SPAWNTITLE: 
        #if RETRO_USE_V6
            //actually used in v6
            loadTextureAll();
            ResetNativeObject(self, MenuControl_Create, MenuControl_Main); 
        #else
            ResetNativeObject(self, TitleScreen_Create, TitleScreen_Main);
        #endif
        break;
#else
        case CWSPLASH_STATE_SPAWNTITLE: ResetNativeObject(self, TitleScreen_Create, TitleScreen_Main); break;
#endif
    }
}
