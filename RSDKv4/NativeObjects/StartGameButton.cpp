#include "RetroEngine.hpp"

void StartGameButton_Create(void *objPtr)
{
    RSDK_THIS(StartGameButton);
    entity->textureCircle = LoadTexture("Data/Game/Menu/Circle.png", TEXFMT_RGBA4444);

    int package = 0;
    switch (Engine.globalBoxRegion) {
        case REGION_JP:
            package          = LoadTexture("Data/Game/Models/Package_JP.png", TEXFMT_RGBA5551);
            entity->meshCart = LoadMesh("Data/Game/Models/JPCartridge.bin", package);
            break;
        case REGION_US:
            package          = LoadTexture("Data/Game/Models/Package_US.png", TEXFMT_RGBA5551);
            entity->meshCart = LoadMesh("Data/Game/Models/Cartridge.bin", package);
            break;
        case REGION_EU:
            package          = LoadTexture("Data/Game/Models/Package_EU.png", TEXFMT_RGBA5551);
            entity->meshCart = LoadMesh("Data/Game/Models/Cartridge.bin", package);
            break;
    }
    entity->prevRegion       = Engine.globalBoxRegion;
    entity->x                = 0.0;
    entity->y                = 16.0;
    entity->z                = 160.0;
    entity->r                = 0xFF;
    entity->g                = 0xFF;
    entity->b                = 0x00;
    entity->labelPtr         = CREATE_ENTITY(TextLabel);
    entity->labelPtr->fontID = FONT_HEADING;
    entity->labelPtr->scale  = 0.15;
    entity->labelPtr->alpha  = 0;
    entity->labelPtr->state  = TEXTLABEL_STATE_IDLE;
    SetStringToFont(entity->labelPtr->text, strStartGame, FONT_HEADING);
    entity->labelPtr->alignPtr(entity->labelPtr, ALIGN_CENTER);
}
void StartGameButton_Main(void *objPtr)
{
    RSDK_THIS(StartGameButton);
    if (entity->prevRegion != Engine.globalBoxRegion) {
        int package = 0;
        switch (Engine.globalBoxRegion) {
            case REGION_JP:
                package          = LoadTexture("Data/Game/Models/Package_JP.png", TEXFMT_RGBA5551);
                entity->meshCart = LoadMesh("Data/Game/Models/JPCartridge.bin", package);
                break;
            case REGION_US:
                package          = LoadTexture("Data/Game/Models/Package_US.png", TEXFMT_RGBA5551);
                entity->meshCart = LoadMesh("Data/Game/Models/Cartridge.bin", package);
                break;
            case REGION_EU:
                package          = LoadTexture("Data/Game/Models/Package_EU.png", TEXFMT_RGBA5551);
                entity->meshCart = LoadMesh("Data/Game/Models/Cartridge.bin", package);
                break;
        }
        entity->prevRegion = Engine.globalBoxRegion;
    }

    if (entity->visible) {
        if (entity->scale < 0.2) {
            entity->scale += ((0.25 - entity->scale) / ((60.0 * Engine.deltaTime) * 16.0));
            if (entity->scale > 0.2)
                entity->scale = 0.2;
        }
        SetRenderBlendMode(RENDER_BLEND_ALPHA);
        SetRenderVertexColor(entity->r, entity->g, entity->b);
        RenderImage(entity->x, entity->y, entity->z, entity->scale, entity->scale, 256.0, 256.0, 512.0, 512.0, 0.0, 0.0, 255, entity->textureCircle);
        SetRenderVertexColor(0xFF, 0xFF, 0xFF);
        SetRenderBlendMode(RENDER_BLEND_NONE);

        entity->angle -= Engine.deltaTime;
        if (entity->angle < -M_PI_2)
            entity->angle += M_PI_2;

        NewRenderState();
        matrixRotateXYZF(&entity->renderMatrix, sinf(entity->angle), entity->angle, 0.0);
        matrixTranslateXYZF(&entity->matrixTemp, entity->x, entity->y, entity->z - 8.0);
        matrixMultiplyF(&entity->renderMatrix, &entity->matrixTemp);
        SetRenderMatrix(&entity->renderMatrix);
        RenderMesh(entity->meshCart, MESH_NORMALS, true);
        SetRenderMatrix(NULL);

        NativeEntity_TextLabel *label = entity->labelPtr;
        label->x                      = entity->x;
        label->y                      = entity->y - 72.0;
        label->z                      = entity->z;
        if (label->x <= -8.0 || label->x >= 8.0) {
            if (label->alpha > 0)
                label->alpha -= 8;
        }
        else if (label->alpha < 0x100)
            label->alpha += 8;
    }
}
