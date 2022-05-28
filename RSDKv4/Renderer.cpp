#include "RetroEngine.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float retroVertexList[40];
float screenBufferVertexList[40];

int vertexListSize = 0;
DrawVertex drawVertexList[DRAWVERTEX_COUNT];
ushort drawIndexList[DRAWINDEX_COUNT];

byte vertexR = 0xFF;
byte vertexG = 0xFF;
byte vertexB = 0xFF;

TextureInfo textureList[TEXTURE_COUNT];
MeshInfo meshList[MESH_COUNT];

int renderStateCount = 0;
RenderState renderStateList[RENDERSTATE_COUNT];
RenderState currentRenderState;

void SetIdentityMatrixF(MatrixF *matrix)
{
    matrix->values[0][0] = 1.0f;
    matrix->values[0][1] = 0.0f;
    matrix->values[0][2] = 0.0f;
    matrix->values[0][3] = 0.0f;
    matrix->values[1][0] = 0.0f;
    matrix->values[1][1] = 1.0f;
    matrix->values[1][2] = 0.0f;
    matrix->values[1][3] = 0.0f;
    matrix->values[2][0] = 0.0f;
    matrix->values[2][1] = 0.0f;
    matrix->values[2][2] = 1.0f;
    matrix->values[2][3] = 0.0f;
    matrix->values[3][0] = 0.0f;
    matrix->values[3][1] = 0.0f;
    matrix->values[3][2] = 0.0f;
    matrix->values[3][3] = 1.0f;
}
void MatrixMultiplyF(MatrixF *matrixA, MatrixF *matrixB)
{
    float output[16];

    for (int i = 0; i < 0x10; ++i) {
        uint rowA = i / 4;
        uint rowB = i % 4;
        output[i] = (matrixA->values[rowA][3] * matrixB->values[3][rowB]) + (matrixA->values[rowA][2] * matrixB->values[2][rowB])
                    + (matrixA->values[rowA][1] * matrixB->values[1][rowB]) + (matrixA->values[rowA][0] * matrixB->values[0][rowB]);
    }

    for (int i = 0; i < 0x10; ++i) matrixA->values[i / 4][i % 4] = output[i];
}
void MatrixTranslateXYZF(MatrixF *matrix, float x, float y, float z)
{
    matrix->values[0][0] = 1.0f;
    matrix->values[0][1] = 0.0f;
    matrix->values[0][2] = 0.0f;
    matrix->values[0][3] = 0.0f;
    matrix->values[1][0] = 0.0f;
    matrix->values[1][1] = 1.0f;
    matrix->values[1][2] = 0.0f;
    matrix->values[1][3] = 0.0f;
    matrix->values[2][0] = 0.0f;
    matrix->values[2][1] = 0.0f;
    matrix->values[2][2] = 1.0f;
    matrix->values[2][3] = 0.0f;
    matrix->values[3][0] = x;
    matrix->values[3][1] = y;
    matrix->values[3][2] = z;
    matrix->values[3][3] = 1.0f;
}
void MatrixScaleXYZF(MatrixF *matrix, float scaleX, float scaleY, float scaleZ)
{
    matrix->values[0][0] = scaleX;
    matrix->values[0][1] = 0.0f;
    matrix->values[0][2] = 0.0f;
    matrix->values[0][3] = 0.0f;
    matrix->values[1][0] = 0.0f;
    matrix->values[1][1] = scaleY;
    matrix->values[1][2] = 0.0f;
    matrix->values[1][3] = 0.0f;
    matrix->values[2][0] = 0.0f;
    matrix->values[2][1] = 0.0f;
    matrix->values[2][2] = scaleZ;
    matrix->values[2][3] = 0.0f;
    matrix->values[3][0] = 0.0f;
    matrix->values[3][1] = 0.0f;
    matrix->values[3][2] = 0.0f;
    matrix->values[3][3] = 1.0f;
}
void MatrixRotateXF(MatrixF *matrix, float angle)
{
    float sine           = sinf(angle);
    float cosine         = cosf(angle);
    matrix->values[0][0] = 1.0f;
    matrix->values[0][1] = 0.0f;
    matrix->values[0][2] = 0.0f;
    matrix->values[0][3] = 0.0f;
    matrix->values[1][0] = 0.0f;
    matrix->values[1][1] = cosine;
    matrix->values[1][2] = sine;
    matrix->values[1][3] = 0.0f;
    matrix->values[2][0] = 0.0f;
    matrix->values[2][1] = -sine;
    matrix->values[2][2] = cosine;
    matrix->values[2][3] = 0.0f;
    matrix->values[3][0] = 0.0f;
    matrix->values[3][1] = 0.0f;
    matrix->values[3][2] = 0.0f;
    matrix->values[3][3] = 1.0f;
}
void MatrixRotateYF(MatrixF *matrix, float angle)
{
    float sine           = sinf(angle);
    float cosine         = cosf(angle);
    matrix->values[0][0] = cosine;
    matrix->values[0][1] = 0.0f;
    matrix->values[0][2] = sine;
    matrix->values[0][3] = 0.0f;
    matrix->values[1][0] = 0.0f;
    matrix->values[1][1] = 1.0f;
    matrix->values[1][2] = 0.0f;
    matrix->values[1][3] = 0.0f;
    matrix->values[2][0] = -sine;
    matrix->values[2][1] = 0.0f;
    matrix->values[2][2] = cosine;
    matrix->values[2][3] = 0.0f;
    matrix->values[3][0] = 0.0f;
    matrix->values[3][1] = 0.0f;
    matrix->values[3][2] = 0.0f;
    matrix->values[3][3] = 1.0f;
}
void MatrixRotateZF(MatrixF *matrix, float angle)
{
    float sine   = sinf(angle);
    float cosine = cosf(angle);

    matrix->values[0][0] = cosine;
    matrix->values[0][1] = -sine;
    matrix->values[0][2] = 0.0;
    matrix->values[0][3] = 0.0;
    matrix->values[1][0] = sine;
    matrix->values[1][1] = cosine;
    matrix->values[1][2] = 0;
    matrix->values[1][3] = 0;
    matrix->values[2][0] = 0.0;
    matrix->values[2][1] = 0.0;
    matrix->values[2][2] = 1.0;
    matrix->values[2][3] = 0.0;
    matrix->values[3][0] = 0.0;
    matrix->values[3][1] = 0.0;
    matrix->values[3][3] = 1.0;
    matrix->values[3][2] = 0.0;
}
void MatrixRotateXYZF(MatrixF *matrix, float angleX, float angleY, float angleZ)
{
    float sinX = sinf(angleX);
    float cosX = cosf(angleX);
    float sinY = sinf(angleY);
    float cosY = cosf(angleY);
    float sinZ = sinf(angleZ);
    float cosZ = cosf(angleZ);

    matrix->values[0][0] = (cosZ * cosY) + (sinZ * (sinY * sinX));
    matrix->values[0][1] = (sinZ * cosY) - (cosZ * (sinY * sinX));
    matrix->values[0][2] = sinY * cosX;
    matrix->values[0][3] = 0.0f;
    matrix->values[1][0] = sinZ * -cosX;
    matrix->values[1][1] = cosZ * cosX;
    matrix->values[1][2] = sinX;
    matrix->values[1][3] = 0.0f;
    matrix->values[2][0] = (sinZ * (cosY * sinX)) - (cosZ * sinY);
    matrix->values[2][1] = (sinZ * -sinY) - (cosZ * (cosY * sinX));
    matrix->values[2][2] = cosY * cosX;
    matrix->values[2][3] = 0.0f;
    matrix->values[3][0] = 0.0f;
    matrix->values[3][1] = 0.0f;
    matrix->values[3][2] = 0.0f;
    matrix->values[3][3] = 1.0f;
}

void MatrixInvertF(MatrixF *dstMatrix, MatrixF *matrix)
{
    double inv[16], det;
    double m[16];
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            m[(y << 2) + x] = matrix->values[y][x];
        }
    }

    inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];

    inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];

    inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];

    inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

    inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];

    inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];

    inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];

    inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

    inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];

    inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];

    inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];

    inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return;

    det = 1.0 / det;

    for (int i = 0; i < 0x10; ++i) dstMatrix->values[i / 4][i % 4] = inv[i] * det;
}

// Render States
void ResetRenderStates()
{
    currentRenderState.renderMatrix = NULL;
    currentRenderState.vertPtr      = drawVertexList;
    currentRenderState.indexCount   = 0;
    currentRenderState.id           = 0;
    currentRenderState.blendMode    = 0;
    currentRenderState.useColors    = false;
    currentRenderState.useTexture   = false;
    currentRenderState.depthTest    = false;
    currentRenderState.useNormals   = false;
    currentRenderState.useFilter    = false;
    currentRenderState.indexPtr     = drawIndexList;
    renderStateCount                = -1;
    vertexListSize                  = 0;
    vertexR                         = 0xFF;
    vertexG                         = 0xFF;
    vertexB                         = 0xFF;
}
void SetRenderBlendMode(byte mode)
{
    if (currentRenderState.blendMode != mode && currentRenderState.indexCount) {
        RenderState *state = &renderStateList[renderStateCount++];
        memcpy(state, &currentRenderState, sizeof(RenderState));

        currentRenderState.indexCount = 0;
        currentRenderState.id         = 0;
        currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
    }
    currentRenderState.blendMode = mode;
}
void SetRenderVertexColor(byte r, byte g, byte b)
{
    vertexR = r;
    vertexG = g;
    vertexB = b;
}

#undef near
#undef far
void SetPerspectiveMatrix(float w, float h, float near, float far)
{
    float m[19];

    float val = tanf((float)(0.017453292f * w) * 0.5f);
    m[11]     = 1.0;
    m[0]      = 1.0 / val;
    m[1]      = 0.0;
    m[2]      = 0.0;
    m[3]      = 0.0;
    m[4]      = 0.0;
    m[6]      = 0.0;
    m[7]      = 0.0;
    m[8]      = 0.0;
    m[9]      = 0.0;
    m[12]     = 0.0;
    m[13]     = 0.0;
    m[15]     = 0.0;
    m[5]      = 1.0 / (val * h);
    m[10]     = (far + near) / (far - near);
    m[14]     = -((far + far) * near) / (far - near);
#if RETRO_USING_OPENGL
    glMultMatrixf(m);
#endif
}
void SetupDrawIndexList()
{
    int index = 0;
    for (int i = 0; i < DRAWINDEX_COUNT;) {
        drawIndexList[i + 2] = index + 0;
        drawIndexList[i + 1] = index + 1;
        drawIndexList[i + 0] = index + 2;
        drawIndexList[i + 5] = index + 1;
        drawIndexList[i + 4] = index + 3;
        drawIndexList[i + 3] = index + 2;
        index += 4;
        i += 6;
    }
}
void SetRenderMatrix(MatrixF *matrix) { currentRenderState.renderMatrix = matrix; }
void NewRenderState()
{
    if (renderStateCount < RENDERSTATE_COUNT) {
        if (currentRenderState.indexCount) {
            RenderState *state = &renderStateList[renderStateCount];
            memcpy(state, &currentRenderState, sizeof(RenderState));

            currentRenderState.id         = 0;
            currentRenderState.indexCount = 0;
            currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
            currentRenderState.indexPtr   = drawIndexList;
            renderStateCount++;
        }
    }
}
void RenderScene()
{
#if RETRO_USING_OPENGL
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
    if (renderStateCount == -1)
        return;

#if !RETRO_USE_ORIGINAL_CODE
    float dimAmount = 1.0;
    if ((!Engine.masterPaused || Engine.frameStep) && !drawStageGFXHQ) {
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

        dimAmount = Engine.dimMax * Engine.dimPercent;
    }

    if (dimAmount < 1.0) {
        SetRenderBlendMode(RENDER_BLEND_ALPHA);
        RenderRect(-SCREEN_CENTERX_F, SCREEN_CENTERY_F, 160.0, SCREEN_XSIZE_F, SCREEN_YSIZE_F, 0, 0, 0, 0xFF - (dimAmount * 0xFF));
        SetRenderBlendMode(RENDER_BLEND_NONE);
    }
#endif

#if RETRO_USING_OPENGL
    glEnableClientState(GL_VERTEX_ARRAY);
    glLoadIdentity();
#endif
    if (currentRenderState.indexCount) {
        RenderState *state = &renderStateList[renderStateCount];
        memcpy(state, &currentRenderState, sizeof(RenderState));

        currentRenderState.indexCount = 0;
        currentRenderState.id         = 0;
        currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
        renderStateCount++;
    }

    MatrixF *prevMat   = NULL;
    bool prevTextures  = false;
    uint prevTexID     = 0;
    bool prevColors    = false;
    bool prevNormals   = false;
    bool prevDepth     = false;
    byte prevBlendMode = 0;

    for (int i = 0; i < renderStateCount; ++i) {
        RenderState *state = &renderStateList[i];

        if (state->renderMatrix != prevMat) {
            if (state->renderMatrix) {
#if RETRO_USING_OPENGL
                glLoadMatrixf((const GLfloat *)state->renderMatrix);
#endif
                prevMat = state->renderMatrix;
            }
            else {
#if RETRO_USING_OPENGL
                glLoadIdentity();
#endif
                prevMat = NULL;
            }
        }

#if RETRO_USING_OPENGL
        glVertexPointer(3, GL_FLOAT, sizeof(DrawVertex), &state->vertPtr->vertX);
#endif
        if (state->useTexture) {
            if (!prevTextures) {
#if RETRO_USING_OPENGL
                glEnable(GL_TEXTURE_2D);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
            }
#if RETRO_USING_OPENGL
            glTexCoordPointer(2, GL_FLOAT, sizeof(DrawVertex), &state->vertPtr->texCoordX);
#endif
            prevTextures = true;
            if (state->id != prevTexID) {
#if RETRO_USING_OPENGL
                glBindTexture(GL_TEXTURE_2D, state->id);
#endif
                prevTexID = state->id;
            }
        }
        else {
            if (prevTextures) {
#if RETRO_USING_OPENGL
                glDisable(GL_TEXTURE_2D);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
            }
            prevTextures = false;
        }

        if (state->useColors) {
#if RETRO_USING_OPENGL
            if (!prevColors)
                glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(DrawVertex), &state->vertPtr->r);
#endif
            prevColors = true;
        }
        else {
#if RETRO_USING_OPENGL
            if (prevColors)
                glDisableClientState(GL_COLOR_ARRAY);
#endif
            prevColors = false;
        }

        if (state->useNormals) {
            if (!prevNormals) {
#if RETRO_USING_OPENGL
                glEnableClientState(GL_NORMAL_ARRAY);
                glEnable(GL_LIGHTING);
#endif
            }
#if RETRO_USING_OPENGL
            glNormalPointer(GL_FLOAT, sizeof(DrawVertex), &state->vertPtr->normalX);
#endif
            prevNormals = true;
        }
        else {
            if (prevNormals) {
#if RETRO_USING_OPENGL
                glDisableClientState(GL_NORMAL_ARRAY);
                glDisable(GL_LIGHTING);
#endif
            }
            prevNormals = false;
        }

        if (state->depthTest) {
#if RETRO_USING_OPENGL
            if (!prevDepth)
                glEnable(GL_DEPTH_TEST);
#endif
            prevDepth = true;
        }
        else {
#if RETRO_USING_OPENGL
            if (prevDepth)
                glDisable(GL_DEPTH_TEST);
#endif
            prevDepth = false;
        }

        if (state->blendMode != prevBlendMode) {
            switch (state->blendMode) {
                default: prevBlendMode = state->blendMode; break;
                case 0:
#if RETRO_USING_OPENGL
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glDisable(GL_BLEND);
#endif
                    prevBlendMode = 0;
                    break;
                case 1:
#if RETRO_USING_OPENGL
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glEnable(GL_BLEND);
#endif
                    prevBlendMode = 1;
                    break;
                case 2:
#if RETRO_USING_OPENGL
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glEnable(GL_BLEND);
#endif
                    prevBlendMode = 2;
                    break;
                case 3:
#if RETRO_USING_OPENGL
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glEnable(GL_BLEND);
#endif
                    prevBlendMode = 3;
                    break;
            }
        }

        if (state->useFilter && mixFiltersOnJekyll) {
#if RETRO_USING_OPENGL
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, framebufferHiRes);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Engine.scalingMode ? GL_LINEAR : GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Engine.scalingMode ? GL_LINEAR : GL_NEAREST);
            glVertexPointer(3, GL_FLOAT, sizeof(DrawVertex), screenBufferVertexList);
            glTexCoordPointer(2, GL_FLOAT, sizeof(DrawVertex), &screenBufferVertexList[6]);
            glViewport(0, 0, GFX_LINESIZE_DOUBLE, SCREEN_YSIZE * 2);
            glPushMatrix();
            glLoadIdentity();
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glDrawElements(GL_TRIANGLES, state->indexCount, GL_UNSIGNED_SHORT, state->indexPtr);

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
            glBindTexture(GL_TEXTURE_2D, renderbufferHiRes);
            glVertexPointer(3, GL_FLOAT, sizeof(DrawVertex), state->vertPtr);
            glTexCoordPointer(2, GL_FLOAT, sizeof(DrawVertex), &state->vertPtr->texCoordX);
            glViewport(displaySettings.offsetX, 0, displaySettings.width, displaySettings.height);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
#endif
        }

#if RETRO_USING_OPENGL
        glDrawElements(GL_TRIANGLES, state->indexCount, GL_UNSIGNED_SHORT, state->indexPtr);
#endif
    }

#if RETRO_USING_OPENGL
    glDisableClientState(GL_VERTEX_ARRAY);
    if (prevTextures)
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    if (prevColors)
        glDisableClientState(GL_COLOR_ARRAY);
#endif
}

int stb_read_cb(void *user, char *data, int size)
{
    FileRead(data, size);
    return size;
}
void stb_skip_cb(void *user, int n) { FileSkip(n); }
int stb_eof_cb(void *user) { return ReachedEndOfFile(); }

// Textures
int LoadTexture(const char *filePath, int format)
{
    int texID = 0;
    for (int i = 0; i < TEXTURE_COUNT; ++i) {
        if (StrComp(textureList[texID].fileName, filePath))
            return texID;
        if (!StrLength(textureList[texID].fileName))
            break;
        texID++;
    }
    if (texID == TEXTURE_COUNT)
        return 0;

    FileInfo info;
    if (LoadFile(filePath, &info)) {

        stbi_io_callbacks callbacks;
        callbacks.read = stb_read_cb;
        callbacks.skip = stb_skip_cb;
        callbacks.eof  = stb_eof_cb;

        int width     = 0;
        int height    = 0;
        int channels  = 0;
        stbi_uc *data = stbi_load_from_callbacks(&callbacks, NULL, &width, &height, &channels, 4);

        if (width > 0 && height > 0) {
            TextureInfo *texture = &textureList[texID];
            texture->width       = width;
            texture->height      = height;
            texture->format      = format;
            StrCopy(texture->fileName, filePath);

            float normalize = 0;
            if (FindStringToken(fileName, "@2", 1) > 0)
                normalize = 2.0;
            else if (FindStringToken(fileName, "@1", 1) > 0)
                normalize = 0.5;
            else
                normalize = 1.0;
            texture->widthN  = normalize / width;
            texture->heightN = normalize / height;

#if RETRO_USING_OPENGL
            glGenTextures(1, &texture->id);
            glBindTexture(GL_TEXTURE_2D, texture->id);
#endif

            int id = 0;
            switch (format) {
                default: break;
                case TEXFMT_RGBA4444: {
                    ushort *pixels = (ushort *)malloc(width * height * sizeof(ushort));

                    for (int y = 0; y < height; ++y) {
                        for (int x = 0; x < width; ++x) {
                            int r                   = (data[id++] >> 4) << 12;
                            int g                   = (data[id++] >> 4) << 8;
                            int b                   = (data[id++]) & 0xF0;
                            int a                   = (data[id++] >> 4);
                            pixels[x + (y * width)] = a | r | g | b;
                        }
                    }

#if RETRO_USING_OPENGL
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, pixels);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glBindTexture(GL_TEXTURE_2D, 0);
#endif

                    free(pixels);
                    break;
                }
                case TEXFMT_RGBA5551: {
                    ushort *pixels = (ushort *)malloc(width * height * sizeof(ushort));

                    for (int y = 0; y < height; ++y) {
                        for (int x = 0; x < width; ++x) {
                            int r                   = data[id++];
                            int g                   = data[id++];
                            int b                   = data[id++];
                            int a                   = data[id++];
                            pixels[x + (y * width)] = RGB888_TO_RGB5551(r, g, b) | (a ? 1 : 0);
                        }
                    }

#if RETRO_USING_OPENGL
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, pixels);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glBindTexture(GL_TEXTURE_2D, 0);
#endif

                    free(pixels);
                    break;
                }
                case TEXFMT_RGBA8888: {
                    uint *pixels = (uint *)malloc(width * height * sizeof(uint));

                    for (int y = 0; y < height; ++y) {
                        for (int x = 0; x < width; ++x) {
                            int r                   = data[id++];
                            int g                   = data[id++];
                            int b                   = data[id++];
                            int a                   = data[id++];
                            pixels[x + (y * width)] = (a << 24) | (b << 16) | (g << 8) | (r << 0);
                        }
                    }

#if RETRO_USING_OPENGL
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glBindTexture(GL_TEXTURE_2D, 0);
#endif

                    free(pixels);
                    break;
                }
            }
        }

        CloseFile();
        stbi_image_free(data);

        return texID;
    }
    return 0;
}
void ReplaceTexture(const char *filePath, int texID)
{
    FileInfo info;
    if (LoadFile(filePath, &info)) {

        stbi_io_callbacks callbacks;
        callbacks.read = stb_read_cb;
        callbacks.skip = stb_skip_cb;
        callbacks.eof  = stb_eof_cb;

        int width     = 0;
        int height    = 0;
        int channels  = 0;
        stbi_uc *data = stbi_load_from_callbacks(&callbacks, NULL, &width, &height, &channels, 4);

        if (width > 0 && height > 0) {
            TextureInfo *texture = &textureList[texID];
            StrCopy(texture->fileName, filePath);

            float normalize = 0;
            if (FindStringToken(fileName, "@2", 1) > 0)
                normalize = 2.0;
            else if (FindStringToken(fileName, "@1", 1) > 0)
                normalize = 0.5;
            else
                normalize = 1.0;
            texture->widthN  = normalize / width;
            texture->heightN = normalize / height;

#if RETRO_USING_OPENGL
            glBindTexture(GL_TEXTURE_2D, texture->id);
#endif

            int id = 0;
            switch (texture->format) {
                default: break;
                case TEXFMT_RGBA4444: {
                    ushort *pixels = (ushort *)malloc(width * height * sizeof(ushort));

                    for (int y = 0; y < height; ++y) {
                        for (int x = 0; x < width; ++x) {
                            int r                   = (data[id++] >> 4) << 12;
                            int g                   = (data[id++] >> 4) << 8;
                            int b                   = (data[id++]) & 0xF0;
                            int a                   = (data[id++] >> 4);
                            pixels[x + (y * width)] = a | r | g | b;
                        }
                    }

#if RETRO_USING_OPENGL
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture->width, texture->height, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, pixels);
                    glBindTexture(GL_TEXTURE_2D, 0);
#endif

                    free(pixels);
                    break;
                }
                case TEXFMT_RGBA5551: {
                    ushort *pixels = (ushort *)malloc(width * height * sizeof(ushort));

                    for (int y = 0; y < height; ++y) {
                        for (int x = 0; x < width; ++x) {
                            int r                   = data[id++];
                            int g                   = data[id++];
                            int b                   = data[id++];
                            int a                   = data[id++];
                            pixels[x + (y * width)] = RGB888_TO_RGB5551(r, g, b) | (a ? 1 : 0);
                        }
                    }
#if RETRO_USING_OPENGL
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture->width, texture->height, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, pixels);
                    glBindTexture(GL_TEXTURE_2D, 0);
#endif

                    free(pixels);
                    break;
                }
                case TEXFMT_RGBA8888: {
                    uint *pixels = (uint *)malloc(width * height * sizeof(uint));

                    for (int y = 0; y < height; ++y) {
                        for (int x = 0; x < width; ++x) {
                            int r                   = data[id++];
                            int g                   = data[id++];
                            int b                   = data[id++];
                            int a                   = data[id++];
                            pixels[x + (y * width)] = (a << 24) | (b << 16) | (g << 8) | (r << 0);
                        }
                    }

#if RETRO_USING_OPENGL
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture->width, texture->height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
                    glBindTexture(GL_TEXTURE_2D, 0);
#endif

                    free(pixels);
                    break;
                }
            }
        }

        CloseFile();
        stbi_image_free(data);
    }
}
void ClearTextures(bool keepBuffer)
{
    for (int i = (keepBuffer ? 1 : 0); i < TEXTURE_COUNT; ++i) {
#if RETRO_USING_OPENGL
        glDeleteTextures(1, &textureList[i].id);
#endif
        StrCopy(textureList[i].fileName, "");
    }
}

// Meshes
MeshInfo *LoadMesh(const char *filePath, byte textureID)
{
    int meshID = 0;
    for (int i = 0; i < MESH_COUNT; ++i) {
        if (StrComp(meshList[meshID].fileName, filePath) && meshList[meshID].textureID == textureID)
            return &meshList[meshID];
        if (!StrLength(meshList[meshID].fileName))
            break;
        meshID++;
    }
    if (meshID == MESH_COUNT)
        return 0;

    FileInfo info;
    if (LoadFile(filePath, &info)) {
        byte buffer[4];
        FileRead(buffer, 4 * sizeof(byte));
        if (buffer[0] == 'R' && buffer[1] == '3' && buffer[2] == 'D' && buffer[3] == '\0') {
            MeshInfo *mesh = &meshList[meshID];

            StrCopy(mesh->fileName, filePath);
            mesh->textureID = textureID;

            FileRead(buffer, sizeof(ushort));
            mesh->vertexCount = buffer[0] + (buffer[1] << 8);
            mesh->vertices    = (DrawVertex *)malloc(sizeof(DrawVertex) * mesh->vertexCount);

            for (int v = 0; v < mesh->vertexCount; ++v) {
                float buf = 0;
                FileRead(&buf, sizeof(float));
                mesh->vertices[v].texCoordX = buf;

                FileRead(&buf, sizeof(float));
                mesh->vertices[v].texCoordY = buf;

                mesh->vertices[v].r = 0xFF;
                mesh->vertices[v].g = 0xFF;
                mesh->vertices[v].b = 0xFF;
                mesh->vertices[v].a = 0xFF;
            }

            FileRead(buffer, sizeof(ushort));
            mesh->indexCount = buffer[0] + (buffer[1] << 8);
            mesh->indices    = (ushort *)malloc(sizeof(ushort) * (3 * mesh->indexCount));

            int id = 0;
            for (int i = 0; i < mesh->indexCount; ++i) {
                FileRead(buffer, sizeof(ushort));
                mesh->indices[id + 2] = buffer[0] + (buffer[1] << 8);

                FileRead(buffer, sizeof(ushort));
                mesh->indices[id + 1] = buffer[0] + (buffer[1] << 8);

                FileRead(buffer, sizeof(ushort));
                mesh->indices[id + 0] = buffer[0] + (buffer[1] << 8);

                id += 3;
            }

            FileRead(buffer, sizeof(ushort));
            mesh->frameCount = buffer[0] + (buffer[1] << 8);

            if (mesh->frameCount <= 1) {
                for (int v = 0; v < mesh->vertexCount; ++v) {
                    float buf = 0;
                    FileRead(&buf, sizeof(float));
                    mesh->vertices[v].vertX = buf;

                    FileRead(&buf, sizeof(float));
                    mesh->vertices[v].vertY = buf;

                    FileRead(&buf, sizeof(float));
                    mesh->vertices[v].vertZ = buf;

                    FileRead(&buf, sizeof(float));
                    mesh->vertices[v].normalX = buf;

                    FileRead(&buf, sizeof(float));
                    mesh->vertices[v].normalY = buf;

                    FileRead(&buf, sizeof(float));
                    mesh->vertices[v].normalZ = buf;
                }
            }
            else {
                mesh->frames = (MeshVertex *)malloc(mesh->frameCount * mesh->vertexCount * sizeof(MeshVertex));
                for (int f = 0; f < mesh->frameCount; ++f) {
                    int frameOff = (f * mesh->vertexCount);
                    for (int v = 0; v < mesh->vertexCount; ++v) {
                        float buf = 0;
                        FileRead(&buf, sizeof(float));
                        mesh->frames[frameOff + v].vertX = buf;

                        FileRead(&buf, sizeof(float));
                        mesh->frames[frameOff + v].vertY = buf;

                        FileRead(&buf, sizeof(float));
                        mesh->frames[frameOff + v].vertZ = buf;

                        FileRead(&buf, sizeof(float));
                        mesh->frames[frameOff + v].normalX = buf;

                        FileRead(&buf, sizeof(float));
                        mesh->frames[frameOff + v].normalY = buf;

                        FileRead(&buf, sizeof(float));
                        mesh->frames[frameOff + v].normalZ = buf;
                    }
                }
            }
            CloseFile();

            return mesh;
        }
        else {
            CloseFile();
        }
    }
    return NULL;
}
void ClearMeshData()
{
    for (int i = 0; i < MESH_COUNT; ++i) {
        MeshInfo *mesh = &meshList[i];
        if (StrLength(mesh->fileName)) {
            if (mesh->frameCount > 1)
                free(mesh->frames);
            if (mesh->indexCount)
                free(mesh->indices);
            if (mesh->vertexCount)
                free(mesh->vertices);

            mesh->frameCount  = 0;
            mesh->indexCount  = 0;
            mesh->vertexCount = 0;

            StrCopy(meshList[i].fileName, "");
        }
    }
}
void SetMeshAnimation(MeshInfo *mesh, MeshAnimator *animator, ushort frameID, ushort frameCount, float speed)
{
    animator->frameCount = frameCount;
    if (frameCount >= mesh->frameCount)
        animator->frameCount = mesh->frameCount - 1;
    if (frameID < mesh->frameCount) {
        animator->loopIndex = frameID;
        animator->frameID   = frameID;
    }
    else {
        animator->loopIndex = 0;
        animator->frameID   = 0;
    }
    animator->animationSpeed = speed;
}
void AnimateMesh(MeshInfo *mesh, MeshAnimator *animator)
{
    if (mesh->frameCount > 1) {
        if (!animator->animationFinished) {
            animator->animationTimer += animator->animationSpeed;

            while (animator->animationTimer > 1.0f) { // new frame (forwards)
                animator->animationTimer -= 1.0f;
                animator->frameID++;

                if (animator->loopAnimation) {
                    if (animator->frameID >= animator->frameCount)
                        animator->frameID = animator->loopIndex;
                }
                else if (animator->frameID >= animator->frameCount) {
                    animator->frameID           = animator->frameCount;
                    animator->animationFinished = true;
                    animator->animationTimer    = 0.0f;
                }
            }
            while (animator->animationTimer < 0.0f) { // new frame (backwards)
                animator->animationTimer += 1.0f;
                animator->frameID--;

                if (animator->frameID < animator->loopIndex || animator->frameID >= animator->frameCount) {
                    if (animator->loopAnimation) {
                        animator->frameID = animator->frameCount;
                    }
                    else {
                        animator->frameID           = animator->loopIndex;
                        animator->animationTimer    = 0.0f;
                        animator->animationFinished = true;
                    }
                }
            }

            ushort frameID   = animator->frameID;
            ushort nextFrame = animator->frameID + 1;
            if (nextFrame >= animator->frameCount && animator->animationSpeed >= 0)
                nextFrame = animator->loopIndex;
            if (frameID >= animator->frameCount && animator->animationSpeed < 0)
                frameID = animator->loopIndex;

            float interp2 = animator->animationTimer;
            float interp  = 1.0 - animator->animationTimer;

            MeshVertex *vert     = &mesh->frames[frameID * mesh->vertexCount];
            MeshVertex *nextVert = &mesh->frames[nextFrame * mesh->vertexCount];
            for (int v = 0; v < mesh->vertexCount; ++v) {
                mesh->vertices[v].vertX   = (vert->vertX * interp) + (nextVert->vertX * interp2);
                mesh->vertices[v].vertY   = (vert->vertY * interp) + (nextVert->vertY * interp2);
                mesh->vertices[v].vertZ   = (vert->vertZ * interp) + (nextVert->vertZ * interp2);
                mesh->vertices[v].normalX = (vert->normalX * interp) + (nextVert->normalX * interp2);
                mesh->vertices[v].normalY = (vert->normalY * interp) + (nextVert->normalY * interp2);
                mesh->vertices[v].normalZ = (vert->normalZ * interp) + (nextVert->normalZ * interp2);

                vert++;
                nextVert++;
            }
        }
        else if (animator->loopAnimation)
            animator->animationFinished = false;
    }
}

void SetMeshVertexColors(MeshInfo *mesh, byte r, byte g, byte b, byte a)
{
    for (int v = 0; v < mesh->vertexCount; ++v) {
        mesh->vertices[v].r = r;
        mesh->vertices[v].g = g;
        mesh->vertices[v].b = b;
        mesh->vertices[v].a = a;
    }
}

// Rendering
void TransferRetroBuffer()
{
#if RETRO_USING_OPENGL
    glBindTexture(GL_TEXTURE_2D, textureList[0].id);
    if (convertTo32Bit) {
        ushort *frameBufferPtr = Engine.frameBuffer;
        uint *texBufferPtr     = Engine.texBuffer;
        for (int y = 0; y < SCREEN_YSIZE; ++y) {
            for (int x = 0; x < GFX_LINESIZE; ++x) {
                texBufferPtr[x] = gfxPalette16to32[frameBufferPtr[x]];
            }
            texBufferPtr += GFX_LINESIZE;
            frameBufferPtr += GFX_LINESIZE;
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GFX_LINESIZE, SCREEN_YSIZE, GL_RGBA, GL_UNSIGNED_BYTE, Engine.texBuffer);
    }
    else {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GFX_LINESIZE, SCREEN_YSIZE, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, Engine.frameBuffer);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
#endif
}
void RenderRetroBuffer(int alpha, float z)
{
    if (vertexListSize < DRAWVERTEX_COUNT && textureList[0].format) {
        if (renderStateCount < 0 || currentRenderState.id != textureList[0].id) {
            if (renderStateCount >= 0) {
                RenderState *state = &renderStateList[renderStateCount];
                memcpy(state, &currentRenderState, sizeof(RenderState));
            }
            currentRenderState.indexCount = 0;
            currentRenderState.id         = textureList[0].id;
            currentRenderState.useColors  = true;
            currentRenderState.useTexture = true;
            currentRenderState.useFilter  = true;
            currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
            currentRenderState.indexPtr   = drawIndexList;
            renderStateCount++;
        }

        if (renderStateCount < RENDERSTATE_COUNT) {
            int a = 0;
            if (alpha >= 0)
                a = alpha;
            if (a > 0xFF)
                a = 0xFF;

            DrawVertex *vertex1 = &drawVertexList[vertexListSize];
            vertex1->vertX      = retroVertexList[0];
            vertex1->vertY      = retroVertexList[1];
            vertex1->vertZ      = z;
            vertex1->texCoordX  = retroVertexList[6];
            vertex1->texCoordY  = retroVertexList[7];
            vertex1->r          = vertexR;
            vertex1->g          = vertexG;
            vertex1->b          = vertexB;
            vertex1->a          = a;

            DrawVertex *vertex2 = &drawVertexList[vertexListSize + 1];
            vertex2->vertX      = retroVertexList[9];
            vertex2->vertY      = retroVertexList[10];
            vertex2->vertZ      = z;
            vertex2->texCoordX  = retroVertexList[15];
            vertex2->texCoordY  = retroVertexList[16];
            vertex2->r          = vertexR;
            vertex2->g          = vertexG;
            vertex2->b          = vertexB;
            vertex2->a          = a;

            DrawVertex *vertex3 = &drawVertexList[vertexListSize + 2];
            vertex3->vertX      = retroVertexList[18];
            vertex3->vertY      = retroVertexList[19];
            vertex3->vertZ      = z;
            vertex3->texCoordX  = retroVertexList[24];
            vertex3->texCoordY  = retroVertexList[25];
            vertex3->r          = vertexR;
            vertex3->b          = vertexB;
            vertex3->g          = vertexG;
            vertex3->a          = a;

            DrawVertex *vertex4 = &drawVertexList[vertexListSize + 3];
            vertex4->vertX      = retroVertexList[27];
            vertex4->vertY      = retroVertexList[28];
            vertex4->vertZ      = z;
            vertex4->texCoordX  = retroVertexList[33];
            vertex4->texCoordY  = retroVertexList[34];
            vertex4->r          = vertexR;
            vertex4->g          = vertexG;
            vertex4->b          = vertexB;
            vertex4->a          = a;

            currentRenderState.indexCount += 6;
            vertexListSize += 4;
        }
    }
}

void RenderImage(float x, float y, float z, float scaleX, float scaleY, float pivotX, float pivotY, float sprW, float sprH, float sprX, float sprY,
                 int alpha, byte texture)
{
    if (vertexListSize < DRAWVERTEX_COUNT && textureList[texture].format) {
        if (renderStateCount < 0) {
            currentRenderState.indexCount = 0;
            currentRenderState.id         = textureList[texture].id;
            currentRenderState.useColors  = true;
            currentRenderState.useTexture = true;
            currentRenderState.useFilter  = false;
            currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
            currentRenderState.indexPtr   = drawIndexList;
            renderStateCount++;
        }
        else {
            bool flag = false;
            if (currentRenderState.useTexture)
                flag = currentRenderState.id == textureList[texture].id;

            if (!flag) {
                RenderState *state = &renderStateList[renderStateCount];
                memcpy(state, &currentRenderState, sizeof(RenderState));

                currentRenderState.indexCount = 0;
                currentRenderState.id         = textureList[texture].id;
                currentRenderState.useColors  = true;
                currentRenderState.useTexture = true;
                currentRenderState.useFilter  = false;
                currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
                currentRenderState.indexPtr   = drawIndexList;
                renderStateCount++;
            }
        }

        if (renderStateCount < RENDERSTATE_COUNT) {
            int a = 0;
            if (alpha >= 0)
                a = alpha;
            if (a > 0xFF)
                a = 0xFF;

            DrawVertex *vertex1 = &drawVertexList[vertexListSize];
            vertex1->vertX      = x - (pivotX * scaleX);
            vertex1->vertY      = (pivotY * scaleY) + y;
            vertex1->vertZ      = z;
            vertex1->texCoordX  = sprX * textureList[texture].widthN;
            vertex1->texCoordY  = sprY * textureList[texture].heightN;
            vertex1->r          = vertexR;
            vertex1->g          = vertexG;
            vertex1->b          = vertexB;
            vertex1->a          = a;

            DrawVertex *vertex2 = &drawVertexList[vertexListSize + 1];
            vertex2->vertX      = ((sprW - pivotX) * scaleX) + x;
            vertex2->vertY      = vertex1->vertY;
            vertex2->vertZ      = z;
            vertex2->texCoordX  = (sprX + sprW) * textureList[texture].widthN;
            vertex2->texCoordY  = vertex1->texCoordY;
            vertex2->r          = vertexR;
            vertex2->g          = vertexG;
            vertex2->b          = vertexB;
            vertex2->a          = a;

            DrawVertex *vertex3 = &drawVertexList[vertexListSize + 2];
            vertex3->vertX      = vertex1->vertX;
            vertex3->vertY      = y - ((sprH - pivotY) * scaleY);
            vertex3->vertZ      = z;
            vertex3->texCoordX  = vertex1->texCoordX;
            vertex3->texCoordY  = (sprY + sprH) * textureList[texture].heightN;
            vertex3->r          = vertexR;
            vertex3->g          = vertexG;
            vertex3->b          = vertexB;
            vertex3->a          = a;

            DrawVertex *vertex4 = &drawVertexList[vertexListSize + 3];
            vertex4->vertX      = vertex2->vertX;
            vertex4->vertY      = vertex3->vertY;
            vertex4->vertZ      = z;
            vertex4->texCoordX  = vertex2->texCoordX;
            vertex4->texCoordY  = vertex3->texCoordY;
            vertex4->r          = vertexR;
            vertex4->g          = vertexG;
            vertex4->b          = vertexB;
            vertex4->a          = a;
            vertexListSize += 4;
            currentRenderState.indexCount += 6;
        }
    }
}
void RenderImageClipped(float x, float y, float z, float scaleX, float scaleY, float pivotX, float pivotY, float sprW, float sprH, float sprX,
                        float sprY, int alpha, byte texture)
{
    if (vertexListSize < DRAWVERTEX_COUNT && textureList[texture].format) {
        if (renderStateCount < 0) {
            currentRenderState.indexCount = 0;
            currentRenderState.id         = textureList[texture].id;
            currentRenderState.useColors  = true;
            currentRenderState.useTexture = true;
            currentRenderState.useFilter  = false;
            currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
            currentRenderState.indexPtr   = drawIndexList;
            renderStateCount++;
        }
        else {
            bool flag = false;
            if (currentRenderState.useTexture)
                flag = currentRenderState.id == textureList[texture].id;

            if (!flag) {
                RenderState *state = &renderStateList[renderStateCount];
                memcpy(state, &currentRenderState, sizeof(RenderState));

                currentRenderState.indexCount = 0;
                currentRenderState.id         = textureList[texture].id;
                currentRenderState.useColors  = true;
                currentRenderState.useTexture = true;
                currentRenderState.useFilter  = false;
                currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
                currentRenderState.indexPtr   = drawIndexList;
                renderStateCount++;
            }
        }

        if (renderStateCount < RENDERSTATE_COUNT) {
            int a = 0;
            if (alpha >= 0)
                a = alpha;
            if (a > 0xFF)
                a = 0xFF;

            DrawVertex *vertex1 = &drawVertexList[vertexListSize];
            vertex1->vertX      = x - (pivotX * scaleX);
            vertex1->vertY      = (pivotY * scaleY) + y;
            vertex1->vertZ      = z;
            vertex1->texCoordX  = sprX * textureList[texture].widthN;
            vertex1->texCoordY  = sprY * textureList[texture].heightN;
            vertex1->r          = vertexR;
            vertex1->g          = vertexG;
            vertex1->b          = vertexB;
            vertex1->a          = a;
            if (vertex1->vertY > 76.0) {
                vertex1->texCoordY = (((vertex1->vertY - 76.0) / scaleY) + sprY) * textureList[texture].heightN;
                vertex1->vertY     = 76.0;
            }

            DrawVertex *vertex2 = &drawVertexList[vertexListSize + 1];
            vertex2->vertX      = ((sprW - pivotX) * scaleX) + x;
            vertex2->vertY      = vertex1->vertY;
            vertex2->vertZ      = z;
            vertex2->texCoordX  = (sprX + sprW) * textureList[texture].widthN;
            vertex2->texCoordY  = vertex1->texCoordY;
            vertex2->r          = vertexR;
            vertex2->g          = vertexG;
            vertex2->b          = vertexB;
            vertex2->a          = a;

            DrawVertex *vertex3 = &drawVertexList[vertexListSize + 2];
            vertex3->vertX      = vertex1->vertX;
            vertex3->vertY      = y - ((sprH - pivotY) * scaleY);
            vertex3->vertZ      = z;
            vertex3->texCoordX  = vertex1->texCoordX;
            vertex3->texCoordY  = (sprY + sprH) * textureList[texture].heightN;
            vertex3->r          = vertexR;
            vertex3->g          = vertexG;
            vertex3->b          = vertexB;
            vertex3->a          = a;
            if (vertex3->vertY < -76.0) {
                vertex3->texCoordY = (((vertex3->vertY + 76.0) / scaleY) + (sprY + sprH)) * textureList[texture].heightN;
                vertex3->vertY     = -76.0;
            }

            DrawVertex *vertex4 = &drawVertexList[vertexListSize + 3];
            vertex4->vertX      = vertex2->vertX;
            vertex4->vertY      = vertex3->vertY;
            vertex4->vertZ      = z;
            vertex4->texCoordX  = vertex2->texCoordX;
            vertex4->texCoordY  = vertex3->texCoordY;
            vertex4->r          = vertexR;
            vertex4->g          = vertexG;
            vertex4->b          = vertexB;
            vertex4->a          = a;
            vertexListSize += 4;
            currentRenderState.indexCount += 6;
        }
    }
}

void RenderImageFlipH(float x, float y, float z, float scaleX, float scaleY, float pivotX, float pivotY, float sprW, float sprH, float sprX,
                      float sprY, int alpha, byte texture)
{
    if (vertexListSize < DRAWVERTEX_COUNT && textureList[texture].format) {
        if (renderStateCount < 0) {
            currentRenderState.indexCount = 0;
            currentRenderState.id         = textureList[texture].id;
            currentRenderState.useColors  = true;
            currentRenderState.useTexture = true;
            currentRenderState.useFilter  = false;
            currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
            currentRenderState.indexPtr   = drawIndexList;
            renderStateCount++;
        }
        else {
            bool flag = false;
            if (currentRenderState.useTexture)
                flag = currentRenderState.id == textureList[texture].id;

            if (!flag) {
                RenderState *state = &renderStateList[renderStateCount];
                memcpy(state, &currentRenderState, sizeof(RenderState));

                currentRenderState.indexCount = 0;
                currentRenderState.id         = textureList[texture].id;
                currentRenderState.useColors  = true;
                currentRenderState.useTexture = true;
                currentRenderState.useFilter  = false;
                currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
                currentRenderState.indexPtr   = drawIndexList;
                renderStateCount++;
            }
        }

        if (renderStateCount < RENDERSTATE_COUNT) {
            int a = 0;
            if (alpha >= 0)
                a = alpha;
            if (a > 0xFF)
                a = 0xFF;

            DrawVertex *vertex1 = &drawVertexList[vertexListSize];
            vertex1->vertX      = x - (pivotX * scaleX);
            vertex1->vertY      = (pivotY * scaleY) + y;
            vertex1->vertZ      = z;
            vertex1->texCoordX  = (sprX + sprW) * textureList[texture].widthN;
            vertex1->texCoordY  = sprY * textureList[texture].heightN;
            vertex1->r          = vertexR;
            vertex1->g          = vertexG;
            vertex1->b          = vertexB;
            vertex1->a          = a;

            DrawVertex *vertex2 = &drawVertexList[vertexListSize + 1];
            vertex2->vertX      = ((sprW - pivotX) * scaleX) + x;
            vertex2->vertY      = vertex1->vertY;
            vertex2->vertZ      = z;
            vertex2->texCoordX  = sprX * textureList[texture].widthN;
            vertex2->texCoordY  = vertex1->texCoordY;
            vertex2->r          = vertexR;
            vertex2->g          = vertexG;
            vertex2->b          = vertexB;
            vertex2->a          = a;

            DrawVertex *vertex3 = &drawVertexList[vertexListSize + 2];
            vertex3->vertX      = vertex1->vertX;
            vertex3->vertY      = y - ((sprH - pivotY) * scaleY);
            vertex3->vertZ      = z;
            vertex3->texCoordX  = vertex1->texCoordX;
            vertex3->texCoordY  = (sprY + sprH) * textureList[texture].heightN;
            vertex3->r          = vertexR;
            vertex3->g          = vertexG;
            vertex3->b          = vertexB;
            vertex3->a          = a;

            DrawVertex *vertex4 = &drawVertexList[vertexListSize + 3];
            vertex4->vertX      = vertex2->vertX;
            vertex4->vertY      = vertex3->vertY;
            vertex4->vertZ      = z;
            vertex4->texCoordX  = vertex2->texCoordX;
            vertex4->texCoordY  = vertex3->texCoordY;
            vertex4->r          = vertexR;
            vertex4->g          = vertexG;
            vertex4->b          = vertexB;
            vertex4->a          = a;
            vertexListSize += 4;
            currentRenderState.indexCount += 6;
        }
    }
}

void RenderText(ushort *text, int fontID, float x, float y, int z, float scale, int alpha)
{
    BitmapFont *font = &fontList[fontID];
    float posX       = x;
    float posY       = (font->base * scale) + y;

    if (vertexListSize < DRAWVERTEX_COUNT) {
        if (renderStateCount < 0 || (!currentRenderState.useTexture || currentRenderState.useColors)) {
            if (renderStateCount >= 0) {
                RenderState *state = &renderStateList[renderStateCount];
                memcpy(state, &currentRenderState, sizeof(RenderState));
            }
            currentRenderState.indexCount = 0;
            currentRenderState.id         = textureList[font->characters[*text].textureID].id;
            currentRenderState.useColors  = true;
            currentRenderState.useTexture = true;
            currentRenderState.useFilter  = false;
            currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
            currentRenderState.indexPtr   = drawIndexList;
            renderStateCount++;
        }

        if (renderStateCount < RENDERSTATE_COUNT) {
            int a = 0;
            if (alpha >= 0)
                a = alpha;
            if (a > 0xFF)
                a = 0xFF;

            ushort character = *text++;
            while (character && vertexListSize < DRAWVERTEX_COUNT) {
                BitmapFontCharacter *fontChar = &font->characters[character];
                TextureInfo *texture          = &textureList[fontChar->textureID];

                if (texture->format) {
                    if (character == 1) {
                        posX = x;
                        posY -= (font->lineHeight * scale);
                    }
                    else {
                        if (currentRenderState.id != texture->id && renderStateCount < RENDERSTATE_COUNT) {
                            currentRenderState.indexCount = 0;
                            memcpy(&renderStateList[renderStateCount++], &currentRenderState, sizeof(RenderState));
                            currentRenderState.vertPtr  = &drawVertexList[vertexListSize];
                            currentRenderState.indexPtr = drawIndexList;
                            currentRenderState.id       = texture->id;
                        }

                        DrawVertex *vertex1 = &drawVertexList[vertexListSize];
                        vertex1->vertX      = posX + (fontChar->xOffset * scale);
                        vertex1->vertY      = posY - (fontChar->yOffset * scale);
                        vertex1->vertZ      = z;
                        vertex1->texCoordX  = fontChar->x * texture->widthN;
                        vertex1->texCoordY  = fontChar->y * texture->heightN;
                        vertex1->r          = vertexR;
                        vertex1->g          = vertexG;
                        vertex1->b          = vertexB;
                        vertex1->a          = a;

                        DrawVertex *vertex2 = &drawVertexList[vertexListSize + 1];
                        vertex2->vertX      = posX + ((fontChar->width + fontChar->xOffset) * scale);
                        vertex2->vertY      = vertex1->vertY;
                        vertex2->vertZ      = z;
                        vertex2->texCoordX  = (fontChar->x + fontChar->width) * texture->widthN;
                        vertex2->texCoordY  = vertex1->texCoordY;
                        vertex2->r          = vertexR;
                        vertex2->g          = vertexG;
                        vertex2->b          = vertexB;
                        vertex2->a          = a;

                        DrawVertex *vertex3 = &drawVertexList[vertexListSize + 2];
                        vertex3->vertX      = vertex1->vertX;
                        vertex3->vertY      = posY - ((fontChar->height + fontChar->yOffset) * scale);
                        vertex3->vertZ      = z;
                        vertex3->texCoordX  = vertex1->texCoordX;
                        vertex3->texCoordY  = (fontChar->y + fontChar->height) * texture->heightN;
                        vertex3->r          = vertexR;
                        vertex3->g          = vertexG;
                        vertex3->b          = vertexB;
                        vertex3->a          = a;

                        DrawVertex *vertex4 = &drawVertexList[vertexListSize + 3];
                        vertex4->vertX      = vertex2->vertX;
                        vertex4->vertY      = vertex3->vertY;
                        vertex4->vertZ      = z;
                        vertex4->texCoordX  = vertex2->texCoordX;
                        vertex4->texCoordY  = vertex3->texCoordY;
                        vertex4->r          = vertexR;
                        vertex4->g          = vertexG;
                        vertex4->b          = vertexB;
                        vertex4->a          = a;
                        vertexListSize += 4;
                        currentRenderState.indexCount += 6;
                    }
                }
                posX += (fontChar->xAdvance * scale);
                character = *text++;
            }
        }
    }
}
void RenderTextClipped(ushort *text, int fontID, float x, float y, int z, float scale, int alpha)
{
    BitmapFont *font = &fontList[fontID];
    float posX       = x;
    float posY       = (font->base * scale) + y;

    if (vertexListSize < DRAWVERTEX_COUNT) {
        if (renderStateCount < 0 || (!currentRenderState.useTexture || currentRenderState.useColors)) {
            if (renderStateCount >= 0) {
                RenderState *state = &renderStateList[renderStateCount];
                memcpy(state, &currentRenderState, sizeof(RenderState));
            }
            currentRenderState.indexCount = 0;
            currentRenderState.id         = textureList[font->characters[*text].textureID].id;
            currentRenderState.useColors  = true;
            currentRenderState.useTexture = true;
            currentRenderState.useFilter  = false;
            currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
            currentRenderState.indexPtr   = drawIndexList;
            renderStateCount++;
        }

        if (renderStateCount < RENDERSTATE_COUNT) {
            int a = 0;
            if (alpha >= 0)
                a = alpha;
            if (a > 0xFF)
                a = 0xFF;

            ushort character = *text++;
            while (character && vertexListSize < DRAWVERTEX_COUNT) {
                BitmapFontCharacter *fontChar = &font->characters[character];
                TextureInfo *texture          = &textureList[fontChar->textureID];

                if (texture->format) {
                    if (character == 1) {
                        posX = x;
                        posY -= (font->lineHeight * scale);
                    }
                    else {
                        if (currentRenderState.id != texture->id && renderStateCount < RENDERSTATE_COUNT) {
                            currentRenderState.indexCount = 0;
                            memcpy(&renderStateList[renderStateCount++], &currentRenderState, sizeof(RenderState));
                            currentRenderState.vertPtr  = &drawVertexList[vertexListSize];
                            currentRenderState.indexPtr = drawIndexList;
                            currentRenderState.id       = texture->id;
                        }

                        DrawVertex *vertex1 = &drawVertexList[vertexListSize];
                        vertex1->vertX      = posX + (fontChar->xOffset * scale);
                        vertex1->vertY      = posY - (fontChar->yOffset * scale);
                        vertex1->vertZ      = z;
                        vertex1->texCoordX  = fontChar->x * texture->widthN;
                        vertex1->texCoordY  = fontChar->y * texture->heightN;
                        vertex1->r          = vertexR;
                        vertex1->g          = vertexG;
                        vertex1->b          = vertexB;
                        vertex1->a          = a;
                        if (vertex1->vertY > 76.0) {
                            vertex1->texCoordY = (((vertex1->vertY - 76.0) / scale) + fontChar->y) * texture->heightN;
                            vertex1->vertY     = 76.0;
                        }

                        DrawVertex *vertex2 = &drawVertexList[vertexListSize + 1];
                        vertex2->vertX      = posX + ((fontChar->width + fontChar->xOffset) * scale);
                        vertex2->vertY      = vertex1->vertY;
                        vertex2->vertZ      = z;
                        vertex2->texCoordX  = (fontChar->x + fontChar->width) * texture->widthN;
                        vertex2->texCoordY  = vertex1->texCoordY;
                        vertex2->r          = vertexR;
                        vertex2->g          = vertexG;
                        vertex2->b          = vertexB;
                        vertex2->a          = a;

                        DrawVertex *vertex3 = &drawVertexList[vertexListSize + 2];
                        vertex3->vertX      = vertex1->vertX;
                        vertex3->vertY      = posY - ((fontChar->height + fontChar->yOffset) * scale);
                        vertex3->vertZ      = z;
                        vertex3->texCoordX  = vertex1->texCoordX;
                        vertex3->texCoordY  = (fontChar->y + fontChar->height) * texture->heightN;
                        vertex3->r          = vertexR;
                        vertex3->g          = vertexG;
                        vertex3->b          = vertexB;
                        vertex3->a          = a;
                        if (vertex3->vertY < -76.0) {
                            vertex3->texCoordY = (((vertex3->vertY + 76.0) / scale) + (fontChar->y + fontChar->height)) * texture->heightN;
                            vertex3->vertY     = -76.0;
                        }

                        DrawVertex *vertex4 = &drawVertexList[vertexListSize + 3];
                        vertex4->vertX      = vertex2->vertX;
                        vertex4->vertY      = vertex3->vertY;
                        vertex4->vertZ      = z;
                        vertex4->texCoordX  = vertex2->texCoordX;
                        vertex4->texCoordY  = vertex3->texCoordY;
                        vertex4->r          = vertexR;
                        vertex4->g          = vertexG;
                        vertex4->b          = vertexB;
                        vertex4->a          = a;
                        vertexListSize += 4;
                        currentRenderState.indexCount += 6;
                    }
                }
                posX += (fontChar->xAdvance * scale);
                character = *text++;
            }
        }
    }
}

void RenderRect(float x, float y, float z, float w, float h, byte r, byte g, byte b, int alpha)
{
    if (vertexListSize < DRAWVERTEX_COUNT) {
        if (renderStateCount < 0 || (currentRenderState.useTexture || !currentRenderState.useColors)) {
            if (renderStateCount >= 0) {
                RenderState *state = &renderStateList[renderStateCount];
                memcpy(state, &currentRenderState, sizeof(RenderState));
            }

            currentRenderState.indexCount = 0;
            currentRenderState.id         = 0;
            currentRenderState.useColors  = true;
            currentRenderState.useTexture = false;
            currentRenderState.useFilter  = false;
            currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
            currentRenderState.indexPtr   = drawIndexList;
            renderStateCount++;
        }

        if (renderStateCount < RENDERSTATE_COUNT) {
            int a = 0;
            if (alpha >= 0)
                a = alpha;
            if (a > 0xFF)
                a = 0xFF;

            DrawVertex *vertex1 = &drawVertexList[vertexListSize];
            vertex1->vertX      = x;
            vertex1->vertY      = y;
            vertex1->vertZ      = z;
            vertex1->r          = r;
            vertex1->g          = g;
            vertex1->b          = b;
            vertex1->a          = a;

            DrawVertex *vertex2 = &drawVertexList[vertexListSize + 1];
            vertex2->vertX      = w + x;
            vertex2->vertY      = y;
            vertex2->vertZ      = z;
            vertex2->r          = r;
            vertex2->g          = g;
            vertex2->b          = b;
            vertex2->a          = a;

            DrawVertex *vertex3 = &drawVertexList[vertexListSize + 2];
            vertex3->vertX      = x;
            vertex3->vertY      = y - h;
            vertex3->vertZ      = z;
            vertex3->r          = r;
            vertex3->g          = g;
            vertex3->b          = b;
            vertex3->a          = a;

            DrawVertex *vertex4 = &drawVertexList[vertexListSize + 3];
            vertex4->vertX      = vertex2->vertX;
            vertex4->vertY      = y - h;
            vertex4->vertZ      = z;
            vertex4->r          = r;
            vertex4->g          = g;
            vertex4->b          = b;
            vertex4->a          = a;

            vertexListSize += 4;
            currentRenderState.indexCount += 6;
        }
    }
}

#if !RETRO_USE_ORIGINAL_CODE
void RenderRectClipped(float x, float y, float z, float w, float h, byte r, byte g, byte b, int alpha)
{
    if (vertexListSize < DRAWVERTEX_COUNT) {
        if (renderStateCount < 0 || (currentRenderState.useTexture || !currentRenderState.useColors)) {
            if (renderStateCount >= 0) {
                RenderState *state = &renderStateList[renderStateCount];
                memcpy(state, &currentRenderState, sizeof(RenderState));
            }

            currentRenderState.indexCount = 0;
            currentRenderState.id         = 0;
            currentRenderState.useColors  = true;
            currentRenderState.useTexture = false;
            currentRenderState.useFilter  = false;
            currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
            currentRenderState.indexPtr   = drawIndexList;
            renderStateCount++;
        }

        if (renderStateCount < RENDERSTATE_COUNT) {
            int a = 0;
            if (alpha >= 0)
                a = alpha;
            if (a > 0xFF)
                a = 0xFF;

            DrawVertex *vertex1 = &drawVertexList[vertexListSize];
            vertex1->vertX      = x;
            vertex1->vertY      = y;
            vertex1->vertZ      = z;
            vertex1->r          = r;
            vertex1->g          = g;
            vertex1->b          = b;
            vertex1->a          = a;
            if (vertex1->vertY > 76.0)
                vertex1->vertY = 76.0;

            DrawVertex *vertex2 = &drawVertexList[vertexListSize + 1];
            vertex2->vertX      = w + x;
            vertex2->vertY      = y;
            vertex2->vertZ      = z;
            vertex2->r          = r;
            vertex2->g          = g;
            vertex2->b          = b;
            vertex2->a          = a;
            if (vertex2->vertY > 76.0)
                vertex2->vertY = 76.0;

            DrawVertex *vertex3 = &drawVertexList[vertexListSize + 2];
            vertex3->vertX      = x;
            vertex3->vertY      = y - h;
            vertex3->vertZ      = z;
            vertex3->r          = r;
            vertex3->g          = g;
            vertex3->b          = b;
            vertex3->a          = a;
            if (vertex3->vertY < -76.0)
                vertex3->vertY = -76.0;

            DrawVertex *vertex4 = &drawVertexList[vertexListSize + 3];
            vertex4->vertX      = vertex2->vertX;
            vertex4->vertY      = y - h;
            vertex4->vertZ      = z;
            vertex4->r          = r;
            vertex4->g          = g;
            vertex4->b          = b;
            vertex4->a          = a;
            if (vertex4->vertY < -76.0)
                vertex4->vertY = -76.0;

            vertexListSize += 4;
            currentRenderState.indexCount += 6;
        }
    }
}
#endif

void RenderMesh(MeshInfo *mesh, byte type, byte depthTest)
{
    if (!mesh)
        return;

    if (renderStateCount < RENDERSTATE_COUNT) {
        if (currentRenderState.indexCount) {
            RenderState *state = &renderStateList[renderStateCount++];
            memcpy(state, &currentRenderState, sizeof(RenderState));
        }

        currentRenderState.vertPtr    = mesh->vertices;
        currentRenderState.indexPtr   = mesh->indices;
        currentRenderState.indexCount = mesh->indexCount * 3;
        if (mesh->textureID >= TEXTURE_COUNT) {
            currentRenderState.useTexture = false;
            currentRenderState.id         = 0;
        }
        else {
            currentRenderState.useTexture = true;
            currentRenderState.id         = textureList[mesh->textureID].id;
        }

        switch (type) {
            case MESH_COLORS:
                currentRenderState.useColors  = true;
                currentRenderState.useNormals = false;
                break;
            case MESH_NORMALS:
                currentRenderState.useColors  = false;
                currentRenderState.useNormals = true;
                break;
            case MESH_COLORS_NORMALS:
                currentRenderState.useColors  = true;
                currentRenderState.useNormals = true;
                break;
        }
        currentRenderState.depthTest = depthTest;

        RenderState *state = &renderStateList[renderStateCount];
        memcpy(state, &currentRenderState, sizeof(RenderState));

        currentRenderState.indexCount = 0;
        currentRenderState.id         = 0;
        currentRenderState.useColors  = true;
        currentRenderState.useTexture = false;
        currentRenderState.useNormals = false;
        currentRenderState.depthTest  = false;
        currentRenderState.useFilter  = false;
        currentRenderState.vertPtr    = &drawVertexList[vertexListSize];
        currentRenderState.indexPtr   = drawIndexList;

        renderStateCount++;
    }
}