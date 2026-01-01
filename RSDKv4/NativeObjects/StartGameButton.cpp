#include "RetroEngine.hpp"

#if RETRO_USE_V6
void loadCartridgeValue(void *objPtr)
{
    RSDK_THIS(StartGameButton);
    int package = 0;
    if (Engine.gameType == GAME_SONICCD) {
        switch (Engine.globalBoxRegion) {
            case REGION_JP:
                package        = LoadTexture("Data/Game/Models/DiscJP.png", TEXFMT_RGBA5551);
                self->meshCart = LoadMesh("Data/Game/Models/MegaCDMedia.bin", package);
                break;

            case REGION_US:
            //DiscJP_Transparent goes unused as you can't change the region in v6
            //according to the original code, it loads the transparent disc texture for US and EU regions
            //so imma just follow that logic here
                package        = LoadTexture("Data/Game/Models/DiscJP_Transparent.png", TEXFMT_RGBA5551);
                self->meshCart = LoadMesh("Data/Game/Models/MegaCDMedia.bin", package);
                break;

            case REGION_EU:
                package        = LoadTexture("Data/Game/Models/DiscJP_Transparent.png", TEXFMT_RGBA5551);
                self->meshCart = LoadMesh("Data/Game/Models/MegaCDMedia.bin", package);
                break;
        }
        self->prevRegion = Engine.globalBoxRegion;
    }
    else{ //Sonic 1 and Sonic 2
        switch (Engine.globalBoxRegion) {
            case REGION_JP:
                package        = LoadTexture("Data/Game/Models/Package_JP.png", TEXFMT_RGBA5551);
                self->meshCart = LoadMesh("Data/Game/Models/JPCartridge.bin", package);
                break;

            case REGION_US:
                package        = LoadTexture("Data/Game/Models/Package_US.png", TEXFMT_RGBA5551);
                self->meshCart = LoadMesh("Data/Game/Models/Cartridge.bin", package);
                break;

            case REGION_EU:
                package        = LoadTexture("Data/Game/Models/Package_EU.png", TEXFMT_RGBA5551);
                self->meshCart = LoadMesh("Data/Game/Models/Cartridge.bin", package);
                break;
        }
        self->prevRegion = Engine.globalBoxRegion;
    }
}
#endif
void StartGameButton_Create(void *objPtr)
{
    RSDK_THIS(StartGameButton);
    self->textureCircle = LoadTexture("Data/Game/Menu/Circle.png", TEXFMT_RGBA4444);
#if RETRO_USE_V6
    loadCartridgeValue(objPtr);
#else
    int package = 0;
    switch (Engine.globalBoxRegion) {
        case REGION_JP:
            package        = LoadTexture("Data/Game/Models/Package_JP.png", TEXFMT_RGBA5551);
            self->meshCart = LoadMesh("Data/Game/Models/JPCartridge.bin", package);
            break;

        case REGION_US:
            package        = LoadTexture("Data/Game/Models/Package_US.png", TEXFMT_RGBA5551);
            self->meshCart = LoadMesh("Data/Game/Models/Cartridge.bin", package);
            break;

        case REGION_EU:
            package        = LoadTexture("Data/Game/Models/Package_EU.png", TEXFMT_RGBA5551);
            self->meshCart = LoadMesh("Data/Game/Models/Cartridge.bin", package);
            break;
    }
#endif
    self->prevRegion       = Engine.globalBoxRegion;
    self->x                = 0.0;
    self->y                = 16.0;
    self->z                = 160.0;
    self->r                = 0xFF;
    self->g                = 0xFF;
    self->b                = 0x00;
    self->labelPtr         = CREATE_ENTITY(TextLabel);
    self->labelPtr->fontID = FONT_HEADING;
    self->labelPtr->scale  = 0.15;
    self->labelPtr->alpha  = 0;
    self->labelPtr->state  = TEXTLABEL_STATE_IDLE;
    SetStringToFont(self->labelPtr->text, strStartGame, FONT_HEADING);
    self->labelPtr->alignPtr(self->labelPtr, ALIGN_CENTER);
}
void StartGameButton_Main(void *objPtr)
{
    RSDK_THIS(StartGameButton);
    if (self->prevRegion != Engine.globalBoxRegion) {
        int package = 0;
        switch (Engine.globalBoxRegion) {
            case REGION_JP:
                package        = LoadTexture("Data/Game/Models/Package_JP.png", TEXFMT_RGBA5551);
                self->meshCart = LoadMesh("Data/Game/Models/JPCartridge.bin", package);
                break;
            case REGION_US:
                package        = LoadTexture("Data/Game/Models/Package_US.png", TEXFMT_RGBA5551);
                self->meshCart = LoadMesh("Data/Game/Models/Cartridge.bin", package);
                break;
            case REGION_EU:
                package        = LoadTexture("Data/Game/Models/Package_EU.png", TEXFMT_RGBA5551);
                self->meshCart = LoadMesh("Data/Game/Models/Cartridge.bin", package);
                break;
        }
        self->prevRegion = Engine.globalBoxRegion;
    }

    if (self->visible) {
        if (self->scale < 0.2) {
            self->scale += ((0.25 - self->scale) / ((60.0 * Engine.deltaTime) * 16.0));
            if (self->scale > 0.2)
                self->scale = 0.2;
        }
        SetRenderBlendMode(RENDER_BLEND_ALPHA);
        SetRenderVertexColor(self->r, self->g, self->b);
        RenderImage(self->x, self->y, self->z, self->scale, self->scale, 256.0, 256.0, 512.0, 512.0, 0.0, 0.0, 255, self->textureCircle);
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);
        SetRenderBlendMode(RENDER_BLEND_NONE);

        self->angle -= Engine.deltaTime;
        if (self->angle < -M_PI_2)
            self->angle += M_PI_2;

        NewRenderState();
        MatrixRotateXYZF(&self->renderMatrix, sinf(self->angle), self->angle, 0.0);
        MatrixTranslateXYZF(&self->matrixTemp, self->x, self->y, self->z - 8.0);
        MatrixMultiplyF(&self->renderMatrix, &self->matrixTemp);
        SetRenderMatrix(&self->renderMatrix);
        RenderMesh(self->meshCart, MESH_NORMALS, true);
        SetRenderMatrix(NULL);

        NativeEntity_TextLabel *label = self->labelPtr;
        label->x                      = self->x;
        label->y                      = self->y - 72.0;
        label->z                      = self->z;
        if (label->x <= -8.0 || label->x >= 8.0) {
            if (label->alpha > 0)
                label->alpha -= 8;
        }
        else if (label->alpha < 0x100)
            label->alpha += 8;
    }
}
