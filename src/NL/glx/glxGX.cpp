#include "NL/glx/glxGX.h"

#include "NL/gl/glPlat.h"

// R4QE01 links the Revolution SDK GX library as automatic objects. Its entry
// points are declared here rather than through <revolution/gx.h> because that
// header's <revolution/types.h> and the game's own "types.h" spell the
// fixed-width typedefs differently.
struct GXColor
{
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

extern "C"
{
    void GXSetTexCoordGen2(s32 dst_coord, s32 func, s32 src_param, u32 mtx, u8 normalize, u32 pt_texmtx);
    void GXSetNumTexGens(u8 nTexGens);
    void GXSetCullMode(s32 mode);
    void GXSetCoPlanar(u8 enable);
    void GXSetCopyFilter(u8 aa, const u8 sample_pattern[12][2], u8 vf, const u8 vfilter[7]);
    void GXSetChanAmbColor(s32 chan, GXColor amb_color);
    void GXSetChanMatColor(s32 chan, GXColor mat_color);
    void GXSetNumChans(u8 nChans);
    void GXSetChanCtrl(s32 chan, u8 enable, s32 amb_src, s32 mat_src, u32 light_mask, s32 diff_fn, s32 attn_fn);
    void GXSetTevColorIn(s32 stage, s32 a, s32 b, s32 c, s32 d);
    void GXSetTevAlphaIn(s32 stage, s32 a, s32 b, s32 c, s32 d);
    void GXSetTevColorOp(s32 stage, s32 op, s32 bias, s32 scale, u8 clamp, s32 out_reg);
    void GXSetTevAlphaOp(s32 stage, s32 op, s32 bias, s32 scale, u8 clamp, s32 out_reg);
    void GXSetTevKColorSel(s32 stage, s32 sel);
    void GXSetTevKAlphaSel(s32 stage, s32 sel);
    void GXSetTevSwapMode(s32 stage, s32 ras_sel, s32 tex_sel);
    void GXSetAlphaCompare(s32 comp0, u8 ref0, s32 op, s32 comp1, u8 ref1);
    void GXSetTevOrder(s32 stage, s32 coord, s32 map, s32 colour);
    void GXSetNumTevStages(u8 nStages);
    void GXSetBlendMode(s32 type, s32 src_factor, s32 dst_factor, s32 op);
    void GXSetColorUpdate(u8 update_enable);
    void GXSetAlphaUpdate(u8 update_enable);
    void GXSetZMode(u8 compare_enable, s32 func, u8 update_enable);
    void GXSetZCompLoc(u8 before_tex);
    void GXSetDither(u8 dither);
    void GXSetCurrentMtx(u32 id);
    void GXSetScissorBoxOffset(s32 x_off, s32 y_off);

}

enum
{
    GX_CULL_BACK = 2,

    GX_LEQUAL = 3,
    GX_ALWAYS = 7,

    GX_AOP_AND = 0,

    GX_BM_NONE = 0,
    GX_BM_SUBTRACT = 3,

    GX_BL_ZERO = 0,
    GX_BL_ONE = 1,

    GX_LO_CLEAR = 0,

    GX_CC_C0 = 2,
    GX_CA_A0 = 1,

    GX_TEV_KCSEL_K0 = 0x0C,
    GX_TEV_KASEL_K0_A = 0x1C,

    GX_TEV_SWAP0 = 0,

    GX_TEVOP_ADD = 0,
    GX_TB_ZERO = 0,
    GX_CS_SCALE_1 = 0,
    GX_TEVPREV = 0,

    GX_ALPHA0 = 4,
    GX_ALPHA1 = 5,

    GX_SRC_REG = 0,
    GX_SRC_VTX = 1,
    GX_DF_CLAMP = 2,
    GX_AF_SPOT = 1,

    GX_MAX_TEVSTAGE = 16
};

s32 gx_colourArg[GX_MAX_TEVSTAGE][4];
s32 gx_alphaArg[GX_MAX_TEVSTAGE][4];
s32 gx_kcolourSel[GX_MAX_TEVSTAGE];
s32 gx_kalphaSel[GX_MAX_TEVSTAGE];

static bool gx_dither;
static bool gx_colourupdate;
static bool gx_alphaupdate;
static bool gx_zcomploc;
static bool gx_ztest;
static bool save_ztest;
static s32 gx_zfunc;
static s32 save_zfunc;
static bool gx_zwrite;
static bool save_zwrite;
static s32 gx_alphafunc;
static u8 gx_alpharef;
static bool gx_blend;
static bool gx_blendSubtract;
static s32 gx_srcfactor;
static s32 gx_dstfactor;
static bool save_blend;
static bool save_blendSubtract;
static s32 save_srcfactor;
static s32 save_dstfactor;
static s32 gx_cullmode;
static u32 gx_currentmtx;
static u32 gx_numChans;
static u32 gx_numTEV;
static u32 gx_numGens;
static nlColour gx_matColour[2];
static nlColour gx_ambColour[2];
static bool gx_coplanar;

static inline void SetBlendMode(bool bBlend, s32 src_factor, s32 dst_factor, bool bSubtract)
{
    if ((bBlend != gx_blend) || (src_factor != gx_srcfactor) || (dst_factor != gx_dstfactor) || (bSubtract != gx_blendSubtract))
    {
        if ((bSubtract != 0) && (bBlend != 0))
        {
            GXSetBlendMode(GX_BM_SUBTRACT, src_factor, dst_factor, GX_LO_CLEAR);
        }
        else
        {
            GXSetBlendMode((s32)((u32)(-(s32)bBlend | bBlend) >> 0x1FU), src_factor, dst_factor, GX_LO_CLEAR);
        }
        gx_blend = bBlend;
        gx_blendSubtract = bSubtract;
        gx_srcfactor = src_factor;
        gx_dstfactor = dst_factor;
    }
}

void gxInit()
{
    for (int i = 0; i < GX_MAX_TEVSTAGE; i++)
    {
        gx_colourArg[i][0] = GX_CC_C0;
        gx_colourArg[i][1] = GX_CC_C0;
        gx_colourArg[i][2] = GX_CC_C0;
        gx_colourArg[i][3] = GX_CC_C0;
        gx_alphaArg[i][0] = GX_CA_A0;
        gx_alphaArg[i][1] = GX_CA_A0;
        gx_alphaArg[i][2] = GX_CA_A0;
        gx_alphaArg[i][3] = GX_CA_A0;
        gx_kcolourSel[i] = GX_TEV_KCSEL_K0;
        gx_kalphaSel[i] = GX_TEV_KASEL_K0_A;
    }

    GXSetDither(1);
    gx_dither = true;
    GXSetColorUpdate(1);
    gx_colourupdate = true;
    GXSetAlphaUpdate(1);
    gx_alphaupdate = true;
    GXSetZCompLoc(1);
    gx_zcomploc = true;

    GXSetZMode(1, GX_LEQUAL, 1);
    gx_ztest = true;
    gx_zfunc = GX_LEQUAL;
    gx_zwrite = true;

    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    gx_alphafunc = GX_ALWAYS;
    gx_alpharef = 0;

    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    gx_blend = GX_BM_NONE;
    gx_blendSubtract = GX_LO_CLEAR;
    gx_srcfactor = GX_BL_ONE;
    gx_dstfactor = GX_BL_ZERO;

    GXSetCullMode(GX_CULL_BACK);
    gx_cullmode = GX_CULL_BACK;

    GXSetCurrentMtx(0);
    gx_currentmtx = 0;

    GXSetNumChans(1);
    gx_numChans = 1;

    GXSetNumTevStages(1);
    gx_numTEV = 1;

    GXSetNumTexGens(0);
    gx_numGens = 0;

    GXColor white = { 255, 255, 255, 255 };
    GXColor black = { 0, 0, 0, 255 };

    GXSetChanMatColor(0, white);
    GXSetChanMatColor(1, white);
    GXSetChanAmbColor(0, black);
    GXSetChanAmbColor(1, black);

    nlColourSet(gx_matColour[0], 255, 255, 255, 255);
    gx_matColour[1] = gx_matColour[0];

    nlColourSet(gx_ambColour[0], 0, 0, 0, 255);
    gx_ambColour[1] = gx_ambColour[0];

    for (int stage = 0; stage < GX_MAX_TEVSTAGE; stage++)
    {
        GXSetTevColorOp(stage, GX_TEVOP_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);
        GXSetTevAlphaOp(stage, GX_TEVOP_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);
        GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
    }

    GXSetCopyFilter(glx_rmode.aa, glx_rmode.sample_pattern, 1, glx_rmode.vfilter);

    GXSetCoPlanar(0);
    gx_coplanar = false;

    GXSetChanCtrl(GX_ALPHA0, 0, GX_SRC_REG, GX_SRC_VTX, 0xFF, GX_DF_CLAMP, GX_AF_SPOT);
    GXSetChanCtrl(GX_ALPHA1, 0, GX_SRC_REG, GX_SRC_VTX, 0xFF, GX_DF_CLAMP, GX_AF_SPOT);

    GXSetScissorBoxOffset(0, 0);
}

void gxSetTevKColourSel(s32 stage, s32 sel)
{
    GXSetTevKColorSel(stage, sel);
    gx_kcolourSel[stage] = sel;
}

void gxSetTevKAlphaSel(s32 stage, s32 sel)
{
    GXSetTevKAlphaSel(stage, sel);
    gx_kalphaSel[stage] = sel;
}

void gxSetTevColourIn(s32 stage, s32 a, s32 b, s32 c, s32 d)
{
    GXSetTevColorIn(stage, a, b, c, d);
    gx_colourArg[stage][0] = a;
    gx_colourArg[stage][1] = b;
    gx_colourArg[stage][2] = c;
    gx_colourArg[stage][3] = d;
}

void gxSetTevAlphaIn(s32 stage, s32 a, s32 b, s32 c, s32 d)
{
    GXSetTevAlphaIn(stage, a, b, c, d);
    gx_alphaArg[stage][0] = a;
    gx_alphaArg[stage][1] = b;
    gx_alphaArg[stage][2] = c;
    gx_alphaArg[stage][3] = d;
}

bool gxSetDither(bool dither)
{
    bool prev = gx_dither;
    if (dither != prev)
    {
        GXSetDither(dither);
        gx_dither = dither;
    }
    return prev;
}

bool gxSetColourUpdate(bool bOn)
{
    bool prev = gx_colourupdate;
    if (bOn != prev)
    {
        GXSetColorUpdate(bOn);
        gx_colourupdate = bOn;
    }
    return prev;
}

bool gxSetAlphaUpdate(bool bOn)
{
    bool prev = gx_alphaupdate;
    if (bOn != prev)
    {
        GXSetAlphaUpdate(bOn);
        gx_alphaupdate = bOn;
    }
    return prev;
}

bool gxSetZCompLoc(bool bBefore)
{
    bool prev = gx_zcomploc;
    if (bBefore != prev)
    {
        GXSetZCompLoc(bBefore);
        gx_zcomploc = bBefore;
    }
    return prev;
}

void gxSetZMode(bool bTest, s32 func, bool bWrite)
{
    if ((bTest != gx_ztest) || (func != gx_zfunc) || (bWrite != gx_zwrite))
    {
        GXSetZMode(bTest, func, bWrite);
        gx_ztest = bTest;
        gx_zfunc = func;
        gx_zwrite = bWrite;
    }
}

void gxSaveZMode()
{
    save_ztest = gx_ztest;
    save_zfunc = gx_zfunc;
    save_zwrite = gx_zwrite;
}

void gxRestoreZMode()
{
    gx_ztest = save_ztest;
    gx_zfunc = save_zfunc;
    gx_zwrite = save_zwrite;
    GXSetZMode(gx_ztest, gx_zfunc, gx_zwrite);
}

void gxSetAlphaCompare(s32 func, u8 ref)
{
    if (gx_alphafunc != func || gx_alpharef != ref)
    {
        GXSetAlphaCompare(func, ref, GX_AOP_AND, func, ref);
        gx_alphafunc = func;
        gx_alpharef = ref;
    }
}

void gxSaveBlendMode()
{
    save_blend = gx_blend;
    save_srcfactor = gx_srcfactor;
    save_dstfactor = gx_dstfactor;
    save_blendSubtract = gx_blendSubtract;
}

void gxRestoreBlendMode()
{
    SetBlendMode(save_blend, save_srcfactor, save_dstfactor, save_blendSubtract);
}

void gxSetBlendMode(bool bBlend, s32 src_factor, s32 dst_factor, bool bSubtract)
{
    SetBlendMode(bBlend, src_factor, dst_factor, bSubtract);
}

s32 gxSetCullMode(s32 mode)
{
    s32 prev = gx_cullmode;
    if (mode != prev)
    {
        GXSetCullMode(mode);
        gx_cullmode = mode;
    }
    return prev;
}

u32 gxSetCurrentMtx(u32 id, bool bForce)
{
    u32 prev = gx_currentmtx;
    if (bForce || id != prev)
    {
        GXSetCurrentMtx(id);
        gx_currentmtx = id;
    }
    return prev;
}

u32 gxSetNumChans(u32 numChans)
{
    u32 prev = gx_numChans;
    if (numChans != prev)
    {
        GXSetNumChans(numChans);
        gx_numChans = numChans;
    }
    return prev;
}

u32 gxSetNumTevStages(u32 numTEV)
{
    u32 prev = gx_numTEV;
    if (numTEV != prev)
    {
        GXSetNumTevStages(numTEV);
        gx_numTEV = numTEV;
    }
    return prev;
}

u32 gxGetNumTevStages()
{
    return gx_numTEV;
}

u32 gxSetNumTexGens(u32 numGens)
{
    u32 prev = gx_numGens;
    if (numGens != prev)
    {
        GXSetNumTexGens(numGens);
        gx_numGens = numGens;
    }
    return prev;
}

u32 gxGetNumTexGens()
{
    return gx_numGens;
}

void gxSetTevOrder(s32 stage, s32 coord, s32 map, s32 colour)
{
    GXSetTevOrder(stage, coord, map, colour);
}

void gxSetTevColourOp(s32 stage, s32 op, s32 bias, s32 scale, bool clamp, s32 out_reg)
{
    GXSetTevColorOp(stage, op, bias, scale, clamp, out_reg);
}

void gxSetTevAlphaOp(s32 stage, s32 op, s32 bias, s32 scale, bool clamp, s32 out_reg)
{
    GXSetTevAlphaOp(stage, op, bias, scale, clamp, out_reg);
}

void gxSetTexCoordGen(s32 dst_coord, s32 func, s32 src_param, u32 arg)
{
    GXSetTexCoordGen2(dst_coord, func, src_param, arg, 0, 125);
}

void gxSetTexCoordGen(s32 dst_coord, s32 func, s32 src_param, u32 arg, bool normalize, u32 pt_texmtx)
{
    GXSetTexCoordGen2(dst_coord, func, src_param, arg, normalize, pt_texmtx);
}

static inline void SetGXChanMatColour(s32 chan, const nlColour& colour)
{
    u32 c = *(u32*)&colour;
    GXSetChanMatColor(chan, *(GXColor*)&c);
}

nlColour gxSetChanMatColour(s32 chan, const nlColour& colour)
{
    nlColour* pMat = &gx_matColour[chan];
    nlColour prev = *pMat;
    if (prev != colour)
    {
        *pMat = colour;
        SetGXChanMatColour(chan, colour);
    }
    return prev;
}

static inline void SetGXChanAmbColour(s32 chan, const nlColour& colour)
{
    u32 c = *(u32*)&colour;
    GXSetChanAmbColor(chan, *(GXColor*)&c);
}

nlColour gxSetChanAmbColour(s32 chan, const nlColour& colour)
{
    nlColour* pAmb = &gx_ambColour[chan];
    nlColour prev = *pAmb;
    if (prev != colour)
    {
        *pAmb = colour;
        SetGXChanAmbColour(chan, colour);
    }
    return prev;
}

bool gxSetCoPlanar(bool coplanar)
{
    bool prev = gx_coplanar;
    if (prev != coplanar)
    {
        gx_coplanar = coplanar;
        GXSetCoPlanar(coplanar);
    }
    return prev;
}
