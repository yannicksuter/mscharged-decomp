#include "Game/Render/HighRange.h"

#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlDebugViews.h"

#include "Game/Render/RLView.h"
#include "Game/Render/RLViewLayers.h"

#include "NL/gl/glDraw2.h"
#include "NL/gl/glPlat.h"
#include "NL/gl/glState.h"
#include "Game/GL/GLFourTextureAddMeshWriter.h"
#include "NL/gl/glView.h"
#include "NL/nlColour.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

HighRange gHighRange;

int sHighRangeDebugTextureIndex = -1;
bool sHighRangeEnabled = true;
int sHighRangeSampleOffset0 = 1;
int sHighRangeSampleOffset1 = 2;
int sHighRangeSampleOffset2 = 3;

int sHighRangeSampleOffset3;
int sHighRangeSampleOffset4;
int sHighRangeSampleOffset5;
int sHighRangeSampleOffset6;

extern nlVector2 sHighRangeVertices[4];

void BindHighRangeTweaks(
    HighRangeTweaks* values, const char* group)
{
    bool registered
        = values->miHighRangeIndex.Bind(
            "miHighRangeIndex", 0.0f, group, true, 6.0f, 1.0f);
    if (!registered)
    {
        *values->miHighRangeIndex.m_pValue
            = values->miHighRangeIndex.GetDefault();
    }
    if (!registered)
    {
        *values->miHighRangeIndex.m_pValue = 6;
    }

    values->mfHighRangeMult.BindWithDefault(
        "mfHighRangeMult", 2.5f, group, true, 0.5f, 16.0f, 0.125f);
    values->mfHighRangeOffset.BindWithDefault(
        "mfHighRangeOffset", 3.5f, group, true, 0.0f, 8.0f, 0.125f);

    registered
        = values->miHighRangeGray.Bind(
            "miHighRangeGray", 0.0f, group, true, 255.0f, 1.0f);
    if (!registered)
    {
        *values->miHighRangeGray.m_pValue
            = values->miHighRangeGray.GetDefault();
    }
    if (!registered)
    {
        *values->miHighRangeGray.m_pValue = 80;
    }

    values->mbFineHighRange.BindWithDefault(
        "mbFineHighRange", true, group, true, 0.0f, 0.0f, 0.0f);

    registered = values->miRed.Bind(
        "miRed", 0.0f, group, true, 255.0f, 1.0f);
    if (!registered)
    {
        *values->miRed.m_pValue = values->miRed.GetDefault();
    }
    if (!registered)
    {
        *values->miRed.m_pValue = 255;
    }

    registered = values->miGreen.Bind(
        "miGreen", 0.0f, group, true, 255.0f, 1.0f);
    if (!registered)
    {
        *values->miGreen.m_pValue
            = values->miGreen.GetDefault();
    }
    if (!registered)
    {
        *values->miGreen.m_pValue = 255;
    }

    registered = values->miBlue.Bind(
        "miBlue", 0.0f, group, true, 255.0f, 1.0f);
    if (!registered)
    {
        *values->miBlue.m_pValue
            = values->miBlue.GetDefault();
    }
    if (!registered)
    {
        *values->miBlue.m_pValue = 255;
    }

    registered = values->miAlpha.Bind(
        "miAlpha", 0.0f, group, true, 255.0f, 1.0f);
    if (!registered)
    {
        *values->miAlpha.m_pValue
            = values->miAlpha.GetDefault();
    }
    if (!registered)
    {
        *values->miAlpha.m_pValue = 108;
    }
}

bool IsHighRangeEnabled(const HighRange*)
{
    return sHighRangeEnabled;
}

void InitializeHighRange(HighRange* state)
{
    u32 widths[7] = { 320, 160, 80, 40, 80, 160, 320 };
    u32 heights[7] = { 224, 112, 56, 28, 56, 112, 224 };
    int i;
    GLTargetInfo info;

    for (i = 0; i < 7; ++i)
    {
        state->mViewports[i].x = 0;
        state->mViewports[i].y = 0;
        state->mViewports[i].width = widths[i] * 2;
        state->mViewports[i].height = heights[i] * 2;
    }

    char targetName0[0x20];
    nlSNPrintf(state->mNames[0], sizeof(state->mNames[0]), "%s", "hr320d");
    nlSNPrintf(targetName0, sizeof(targetName0), "target/%s", "hr320d");
    state->mTextures[0] = glGetTexture(targetName0);

    char targetName1[0x20];
    const char* name1 = "hr160d";
    nlSNPrintf(state->mNames[1], sizeof(state->mNames[1]), "%s", name1);
    nlSNPrintf(targetName1, sizeof(targetName1), "target/%s", name1);
    state->mTextures[1] = glGetTexture(targetName1);

    char targetName2[0x20];
    const char* name2 = "hr80d";
    nlSNPrintf(state->mNames[2], sizeof(state->mNames[2]), "%s", name2);
    nlSNPrintf(targetName2, sizeof(targetName2), "target/%s", name2);
    state->mTextures[2] = glGetTexture(targetName2);

    char targetName3[0x20];
    const char* name3 = "hr40d";
    nlSNPrintf(state->mNames[3], sizeof(state->mNames[3]), "%s", name3);
    nlSNPrintf(targetName3, sizeof(targetName3), "target/%s", name3);
    state->mTextures[3] = glGetTexture(targetName3);

    char targetName4[0x20];
    const char* name4 = "hr80u";
    nlSNPrintf(state->mNames[4], sizeof(state->mNames[4]), "%s", name4);
    nlSNPrintf(targetName4, sizeof(targetName4), "target/%s", name4);
    state->mTextures[4] = glGetTexture(targetName4);

    char targetName5[0x20];
    const char* name5 = "hr160u";
    nlSNPrintf(state->mNames[5], sizeof(state->mNames[5]), "%s", name5);
    nlSNPrintf(targetName5, sizeof(targetName5), "target/%s", name5);
    state->mTextures[5] = glGetTexture(targetName5);

    char targetName6[0x20];
    const char* name6 = "hr320u";
    nlSNPrintf(state->mNames[6], sizeof(state->mNames[6]), "%s", name6);
    nlSNPrintf(targetName6, sizeof(targetName6), "target/%s", name6);
    state->mTextures[6] = glGetTexture(targetName6);

    for (i = 0; i < 7; ++i)
    {
        nlZeroMemory(&info, sizeof(info));
        info.width = widths[i];
        info.height = heights[i];
        info.format = GLTargetFormat_6;
        info.unknown18 = 0;
        info.unknown1C = 0;
        state->mRenderPairs[i]
            = glCreateTarget(state->mNames[i], &info);
    }

    for (i = 0; i < 7; ++i)
    {
        GLView* view = new (8, false) GLView(
            GetOrthoCamera(), state->mRenderPairs[i], GLViewSort_Texture);
        view->m_Name = state->mNames[i];
        view->m_ViewportX = state->mViewports[i].x;
        view->m_ViewportY = state->mViewports[i].y;
        view->m_ViewportWidth = state->mViewports[i].width;
        view->m_ViewportHeight = state->mViewports[i].height;
        view->m_ClearColour = false;
        view->m_ClearDepth = false;
        view->m_Target = 10;
        state->mViews[i] = view;
    }

    for (i = 0; i < 7; ++i)
    {
        state->mViews[i]->m_Enabled = true;
        GetLayerView(eCLV_HighRangeChain)->AddChild(state->mViews[i]);
    }
}

void SetHighRangeTargetsEnabled(
    HighRange* state, int enabled)
{
    bool fineHighRange = *GetHighRangeTweaks()->mbFineHighRange.m_pValue;
    for (int i = 0; i < 7; ++i)
    {
        GLView* view = state->mViews[i];
        int target = 0;
        if (enabled)
        {
            target = 10;
        }
        view->m_Target = target;
        if (!fineHighRange && (unsigned int)(i - 3) <= 1)
        {
            state->mViews[i]->m_Target = 0;
        }
    }
}

void CompositeHighRange(HighRange*)
{
    glPoly2 poly;
    int index = *GetHighRangeTweaks()->miHighRangeIndex.m_pValue;

    glSetDefaultState(false);
    glSetCurrentTexture(gHighRange.mTextures[index], GLTT_Diffuse);
    glSetRasterState(GLS_AlphaBlend, 3);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTextureState(glHandleizeTextureState());

    HighRangeTweaks* values = GetHighRangeTweaks();
    nlColour colour;
    nlColourSet(colour, *values->miRed.m_pValue, *values->miGreen.m_pValue, *values->miBlue.m_pValue, *values->miAlpha.m_pValue);
    poly.FullCoverage(colour, 0.0f);
    poly.Attach(GetLayerView(eCLV_HighRange2D), 0, 0);

    int debugIndex = sHighRangeDebugTextureIndex;
    if (debugIndex >= 0)
    {
        glSetDefaultState(false);
        glSetCurrentTexture(
            gHighRange.mTextures[debugIndex], GLTT_Diffuse);
        glSetRasterState(GLS_AlphaBlend, 1);
        glSetCurrentRasterState(glHandleizeRasterState());
        glSetTextureState(GLTS_DiffuseWrap, 3);
        glSetCurrentTextureState(glHandleizeTextureState());

        glPoly2 debugPoly;
        nlColour white;
        white.c[0] = 255;
        white.c[1] = 255;
        white.c[2] = 255;
        white.c[3] = 255;
        debugPoly.SetColour(white);
        debugPoly.SetupRectangle(
            32.0f, 24.0f, 200.0f, 200.0f, 10000000000.0f);
        debugPoly.Attach(GetDebugFontView(), 2, 0);
    }
}

void RenderHighRangeChain(HighRange* state)
{
    int fineLevels[7] = { 0, 1, 2, 3, 4, 5, 6 };
    int coarseLevels[5] = { 0, 1, 2, 5, 6 };

    if (*GetHighRangeTweaks()->mbFineHighRange.m_pValue)
    {
        for (int i = 1; i < 7; ++i)
        {
            RenderHighRangePass(state, fineLevels[i], fineLevels[i - 1]);
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            RenderHighRangePass(state, coarseLevels[i], coarseLevels[i - 1]);
        }
    }
}

static inline nlVector2 MakeHighRangeTexcoord(float x, float y)
{
    nlVector2 texcoord;
    nlVec2Set(texcoord, x, y);
    texcoord.x /= glplatGetOrthographicWidth();
    texcoord.y /= glplatGetOrthographicHeight();
    return texcoord;
}

void RenderHighRangePass(
    HighRange*, int outputLevel, int inputLevel)
{
    u32 texture;
    int* sampleOffset;
    nlVector2* vertex;
    HighRange* highRange;
    int gray;
    GLFourTextureAddMeshWriter writer;
    int sampleOffsets[7] = {
        sHighRangeSampleOffset0,
        sHighRangeSampleOffset1,
        sHighRangeSampleOffset2,
        sHighRangeSampleOffset3,
        sHighRangeSampleOffset4,
        sHighRangeSampleOffset5,
        sHighRangeSampleOffset6,
    };
    highRange = &gHighRange;
    texture = highRange->mTextures[inputLevel];

    glSetDefaultState(false);
    glSetRasterState(GLS_AlphaBlend, 6);
    glSetRasterState(GLS_ColourWrite, 2);
    glSetCurrentRasterState(glHandleizeRasterState());

    gray = *GetHighRangeTweaks()->miHighRangeGray.m_pValue;
    if (writer.Begin(4, 3, 0))
    {
        sampleOffset = sampleOffsets;
        vertex = sHighRangeVertices;
        for (int i = 0; i < 4; ++i, ++sampleOffset)
        {
            nlColour colour;
            nlColourSet(colour, gray, gray, gray, gray);
            writer.Colour(colour);

            HighRangeTweaks* values = GetHighRangeTweaks();
            float sampleOffset0 = *sampleOffset
                                * *values->mfHighRangeMult.m_pValue;
            sampleOffset0
                = sampleOffset0 * *values->mfHighRangeOffset.m_pValue;
            writer.Texcoord0(MakeHighRangeTexcoord(
                vertex->x - sampleOffset0, vertex->y));

            values = GetHighRangeTweaks();
            float sampleOffset1 = *sampleOffset
                                * *values->mfHighRangeMult.m_pValue;
            sampleOffset1
                = sampleOffset1 * *values->mfHighRangeOffset.m_pValue;
            writer.Texcoord1(MakeHighRangeTexcoord(
                vertex->x + sampleOffset1, vertex->y));

            values = GetHighRangeTweaks();
            float sampleOffset2 = *sampleOffset
                                * *values->mfHighRangeMult.m_pValue;
            sampleOffset2
                = sampleOffset2 * *values->mfHighRangeOffset.m_pValue;
            writer.Texcoord2(MakeHighRangeTexcoord(
                vertex->x, vertex->y - sampleOffset2));

            values = GetHighRangeTweaks();
            float sampleOffset3 = *sampleOffset
                                * *values->mfHighRangeMult.m_pValue;
            sampleOffset3
                = sampleOffset3 * *values->mfHighRangeOffset.m_pValue;
            nlVector2 texcoord3 = MakeHighRangeTexcoord(
                vertex->x, vertex->y + sampleOffset3);
            float positionX = vertex->x;
            float positionY = vertex->y;
            ++vertex;
            writer.Texcoord3(texcoord3);
            writer.Vertex(positionX, positionY, 0.0f);
        }

        writer.Texture(0, texture);
        writer.Texture(1, texture);
        writer.Texture(2, texture);
        writer.Texture(3, texture);

        if (writer.End())
        {
            highRange->mViews[outputLevel]->AttachModel(writer.model, 0);
        }
    }
}

nlVector2 sHighRangeVertices[4] = {
    { { 0.0f, 0.0f } },
    { { 0.0f, 480.0f } },
    { { 640.0f, 480.0f } },
    { { 640.0f, 0.0f } },
};
