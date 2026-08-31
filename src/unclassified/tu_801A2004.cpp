#include "unclassified/tu_801A2004.h"

#include "NL/gl/glDraw2.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlColour.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

struct HighRangeTargetInfo_801A2394
{
    /* 0x00 */ u32 height;
    /* 0x04 */ u32 width;
    /* 0x08 */ u32 unknown08;
    /* 0x0C */ u32 unknown0C;
    /* 0x10 */ u32 unknown10;
    /* 0x14 */ u32 format;
    /* 0x18 */ u32 unknown18;
    /* 0x1C */ u32 unknown1C;
    /* 0x20 */ u32 unknown20;
    /* 0x24 */ u8 colour[4];
};

extern "C"
{
    int lbl_806DCE58 = -1;
    bool lbl_806DCE5C = true;
    int lbl_806DCE60 = 1;
    int lbl_806DCE64 = 2;
    int lbl_806DCE68 = 3;

    HighRangeTweakValues_801A2004* fn_80277DC8();
    GLViewInterface* fn_802726A0();
    GLView* fn_8027267C(int);
    GLView* fn_802B77A0();
}

extern "C" void fn_801A2004(
    HighRangeTweakValues_801A2004* values, const char* group)
{
    bool registered
        = ((TweakValue_804F4DC8*)&values->miHighRangeIndex)
              ->fn_802C4FEC(
                  "miHighRangeIndex", 0.0f, group, true, 6.0f, 1.0f);
    if (!registered)
    {
        *values->miHighRangeIndex.m_pValue
            = values->miHighRangeIndex.UnidentifiedVirtual3C();
    }
    if (!registered)
    {
        *values->miHighRangeIndex.m_pValue = 6;
    }

    registered = values->mfHighRangeMult.fn_802C4FEC(
        "mfHighRangeMult", 0.5f, group, true, 16.0f, 0.125f);
    if (!registered)
    {
        values->mfHighRangeMult
            = values->mfHighRangeMult.GetDefaultValue();
    }
    if (!registered)
    {
        values->mfHighRangeMult = 2.5f;
    }

    registered = values->mfHighRangeOffset.fn_802C4FEC(
        "mfHighRangeOffset", 0.0f, group, true, 8.0f, 0.125f);
    if (!registered)
    {
        values->mfHighRangeOffset
            = values->mfHighRangeOffset.GetDefaultValue();
    }
    if (!registered)
    {
        values->mfHighRangeOffset = 3.5f;
    }

    registered
        = ((TweakValue_804F4DC8*)&values->miHighRangeGray)
              ->fn_802C4FEC(
                  "miHighRangeGray", 0.0f, group, true, 255.0f, 1.0f);
    if (!registered)
    {
        *values->miHighRangeGray.m_pValue
            = values->miHighRangeGray.UnidentifiedVirtual3C();
    }
    if (!registered)
    {
        *values->miHighRangeGray.m_pValue = 80;
    }

    registered
        = ((TweakValue_804F4DC8*)&values->mbFineHighRange)
              ->fn_802C4FEC(
                  "mbFineHighRange", 0.0f, group, true, 0.0f, 0.0f);
    if (!registered)
    {
        *values->mbFineHighRange.m_pValue
            = values->mbFineHighRange.UnidentifiedVirtual3C();
    }
    if (!registered)
    {
        *values->mbFineHighRange.m_pValue = true;
    }

    registered = ((TweakValue_804F4DC8*)&values->miRed)
                     ->fn_802C4FEC(
                         "miRed", 0.0f, group, true, 255.0f, 1.0f);
    if (!registered)
    {
        *values->miRed.m_pValue = values->miRed.UnidentifiedVirtual3C();
    }
    if (!registered)
    {
        *values->miRed.m_pValue = 255;
    }

    registered = ((TweakValue_804F4DC8*)&values->miGreen)
                     ->fn_802C4FEC(
                         "miGreen", 0.0f, group, true, 255.0f, 1.0f);
    if (!registered)
    {
        *values->miGreen.m_pValue
            = values->miGreen.UnidentifiedVirtual3C();
    }
    if (!registered)
    {
        *values->miGreen.m_pValue = 255;
    }

    registered = ((TweakValue_804F4DC8*)&values->miBlue)
                     ->fn_802C4FEC(
                         "miBlue", 0.0f, group, true, 255.0f, 1.0f);
    if (!registered)
    {
        *values->miBlue.m_pValue
            = values->miBlue.UnidentifiedVirtual3C();
    }
    if (!registered)
    {
        *values->miBlue.m_pValue = 255;
    }

    registered = ((TweakValue_804F4DC8*)&values->miAlpha)
                     ->fn_802C4FEC(
                         "miAlpha", 0.0f, group, true, 255.0f, 1.0f);
    if (!registered)
    {
        *values->miAlpha.m_pValue
            = values->miAlpha.UnidentifiedVirtual3C();
    }
    if (!registered)
    {
        *values->miAlpha.m_pValue = 108;
    }
}

extern "C" bool fn_801A238C(const HighRangeState_801A2394*)
{
    return lbl_806DCE5C;
}

extern "C" void fn_801A2394(HighRangeState_801A2394* state)
{
    u32 widths[7] = { 320, 160, 80, 40, 80, 160, 320 };
    u32 heights[7] = { 224, 112, 56, 28, 56, 112, 224 };
    int i;

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

    HighRangeTargetInfo_801A2394 info;
    for (i = 0; i < 7; ++i)
    {
        nlZeroMemory(&info, sizeof(info));
        info.width = widths[i];
        info.height = heights[i];
        info.format = 6;
        info.unknown18 = 0;
        info.unknown1C = 0;
        state->mRenderPairs[i] = fn_802CD884(state->mNames[i], &info);
    }

    for (i = 0; i < 7; ++i)
    {
        GLView* view = new (8, false) GLView(
            fn_802726A0(), state->mRenderPairs[i], GLViewSort_Texture);
        view->m_Name = state->mNames[i];
        view->m_ViewportX = state->mViewports[i].x;
        view->m_ViewportY = state->mViewports[i].y;
        view->m_ViewportWidth = state->mViewports[i].width;
        view->m_ViewportHeight = state->mViewports[i].height;
        view->m_Unknown33 = false;
        view->m_ClearColour = false;
        view->m_Target = 10;
        state->mViews[i] = view;
    }

    for (i = 0; i < 7; ++i)
    {
        GLView* view = state->mViews[i];
        view->m_Enabled = true;
        GLView* parent = fn_8027267C(17);
        parent->m_Children.AddEnd(view);
        view->m_Parent = parent;
    }
}

extern "C" void fn_801A2860(
    HighRangeState_801A2394* state, int enabled)
{
    bool fineHighRange = *fn_80277DC8()->mbFineHighRange.m_pValue;
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

extern "C" void fn_801A28F0(HighRangeState_801A2394*)
{
    glPoly2 poly;
    int index = *fn_80277DC8()->miHighRangeIndex.m_pValue;

    glSetDefaultState(false);
    glSetCurrentTexture(lbl_80572020.mTextures[index], GLTT_Diffuse);
    glSetRasterState(GLS_AlphaBlend, 3);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTextureState(glHandleizeTextureState());

    HighRangeTweakValues_801A2004* values = fn_80277DC8();
    nlColour colour;
    nlColourSet(colour, *values->miRed.m_pValue, *values->miGreen.m_pValue, *values->miBlue.m_pValue, *values->miAlpha.m_pValue);
    poly.FullCoverage(colour, 0.0f);
    poly.Attach(fn_8027267C(18), 0, 0);

    int debugIndex = lbl_806DCE58;
    if (debugIndex >= 0)
    {
        glSetDefaultState(false);
        glSetCurrentTexture(
            lbl_80572020.mTextures[debugIndex], GLTT_Diffuse);
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
        debugPoly.Attach(fn_802B77A0(), 2, 0);
    }
}

extern "C" void fn_801A2A78(HighRangeState_801A2394* state)
{
    int fineLevels[7] = { 0, 1, 2, 3, 4, 5, 6 };
    int coarseLevels[5] = { 0, 1, 2, 5, 6 };

    if (*fn_80277DC8()->mbFineHighRange.m_pValue)
    {
        for (int i = 1; i < 7; ++i)
        {
            fn_801A2B80(state, fineLevels[i], fineLevels[i - 1]);
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            fn_801A2B80(state, coarseLevels[i], coarseLevels[i - 1]);
        }
    }
}

extern "C" float lbl_80511EA8[8] = {
    0.0f,
    0.0f,
    0.0f,
    480.0f,
    640.0f,
    480.0f,
    640.0f,
    0.0f,
};
