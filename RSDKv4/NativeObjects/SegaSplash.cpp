#include "RetroEngine.hpp"

void SegaSplash_Create(void *objPtr)
{
    RSDK_THIS(SegaSplash);
    entity->state     = SEGAPLASH_STATE_ENTER;
    entity->rectAlpha = 320.0;
    entity->textureID = LoadTexture("Data/Game/Menu/CWLogo.png", TEXFMT_RGBA8888);
    if (Engine.useHighResAssets) {
        if (Engine.language == RETRO_JP)
            entity->textureID = LoadTexture("Data/Game/Menu/SegaJP@2x.png", TEXFMT_RGBA5551);
        else
            entity->textureID = LoadTexture("Data/Game/Menu/Sega@2x.png", TEXFMT_RGBA5551);
    }
    else {
        if (Engine.language == RETRO_JP)
            entity->textureID = LoadTexture("Data/Game/Menu/SegaJP.png", TEXFMT_RGBA5551);
        else
            entity->textureID = LoadTexture("Data/Game/Menu/Sega.png", TEXFMT_RGBA5551);
    }
    // code has been here from TitleScreen_Create due to the possibility of opening the dev menu before this loads :(
#if !RETRO_USE_ORIGINAL_CODE
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
#endif

    switch (Engine.language) {
        case RETRO_JP:
#if !RETRO_USE_ORIGINAL_CODE
            heading = LoadTexture("Data/Game/Menu/Heading_JA@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Heading_JA.fnt", FONT_HEADING, heading);

            labelTex = LoadTexture("Data/Game/Menu/Label_JA@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Label_JA.fnt", FONT_LABEL, labelTex);

            textTex = LoadTexture("Data/Game/Menu/Text_JA@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Text_JA.fnt", FONT_TEXT, textTex);
#endif
            break;
        case RETRO_RU: 
#if !RETRO_USE_ORIGINAL_CODE
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
#endif
            break;
        case RETRO_KO:
#if !RETRO_USE_ORIGINAL_CODE
            heading = LoadTexture("Data/Game/Menu/Heading_KO@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Heading_KO.fnt", FONT_HEADING, heading);

            labelTex = LoadTexture("Data/Game/Menu/Label_KO@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Label_KO.fnt", FONT_LABEL, labelTex);

            textTex = LoadTexture("Data/Game/Menu/Text_KO.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Text_KO.fnt", FONT_TEXT, textTex);
#endif
            break;
        case RETRO_ZH:
#if !RETRO_USE_ORIGINAL_CODE
            heading = LoadTexture("Data/Game/Menu/Heading_ZH@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Heading_ZH.fnt", FONT_HEADING, heading);

            labelTex = LoadTexture("Data/Game/Menu/Label_ZH@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Label_ZH.fnt", FONT_LABEL, labelTex);

            textTex = LoadTexture("Data/Game/Menu/Text_ZH@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Text_ZH.fnt", FONT_TEXT, textTex);
#endif
            break;
        case RETRO_ZS:
#if !RETRO_USE_ORIGINAL_CODE
            heading = LoadTexture("Data/Game/Menu/Heading_ZHS@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Heading_ZHS.fnt", FONT_HEADING, heading);
            labelTex = LoadTexture("Data/Game/Menu/Label_ZHS@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Label_ZHS.fnt", FONT_LABEL, labelTex);
            textTex = LoadTexture("Data/Game/Menu/Text_ZHS@1x.png", TEXFMT_RGBA4444);
            LoadBitmapFont("Data/Game/Menu/Text_ZHS.fnt", FONT_TEXT, textTex);
#endif
            break;
        default: break;
    }
}
void SegaSplash_Main(void *objPtr)
{
    RSDK_THIS(SegaSplash);

    switch (entity->state) {
        case SEGAPLASH_STATE_ENTER:
            entity->rectAlpha -= 300.0 * Engine.deltaTime;
            if (entity->rectAlpha < -320.0)
                entity->state = SEGAPLASH_STATE_EXIT;
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0xFF, 0xFF, 0xFF, 0xFF);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImage(0.0, 0.0, 160.0, 0.4, 0.4, 256.0, 128.0, 512.0, 256.0, 0.0, 0.0, 255, entity->textureID);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, entity->rectAlpha);
            break;
        case SEGAPLASH_STATE_EXIT:
            entity->rectAlpha += 300.0 * Engine.deltaTime;
            if (entity->rectAlpha > 512.0)
                entity->state = SEGAPLASH_STATE_SPAWNCWSPLASH;
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0xFF, 0xFF, 0xFF, 0xFF);
            SetRenderBlendMode(RENDER_BLEND_ALPHA);
            RenderImage(0.0, 0.0, 160.0, 0.4, 0.4, 256.0, 128.0, 512.0, 256.0, 0.0, 0.0, 255, entity->textureID);
            RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, entity->rectAlpha);
            break;
        case SEGAPLASH_STATE_SPAWNCWSPLASH: ResetNativeObject(entity, CWSplash_Create, CWSplash_Main); break;
    }
}
