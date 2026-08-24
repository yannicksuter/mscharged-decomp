#ifndef _GLXGX_H_
#define _GLXGX_H_

#include "NL/nlColour.h"

void gxInit();
void gxSetTevKColourSel(s32 stage, s32 sel);
void gxSetTevKAlphaSel(s32 stage, s32 sel);
void gxSetTevColourIn(s32 stage, s32 a, s32 b, s32 c, s32 d);
void gxSetTevAlphaIn(s32 stage, s32 a, s32 b, s32 c, s32 d);
bool gxSetDither(bool dither);
bool gxSetColourUpdate(bool bOn);
bool gxSetAlphaUpdate(bool bOn);
bool gxSetZCompLoc(bool bBefore);
void gxSetZMode(bool bTest, s32 func, bool bWrite);
void gxSaveZMode();
void gxRestoreZMode();
void gxSetAlphaCompare(s32 func, u8 ref);
void gxSaveBlendMode();
void gxRestoreBlendMode();
void gxSetBlendMode(bool bBlend, s32 src_factor, s32 dst_factor, bool bSubtract);
s32 gxSetCullMode(s32 mode);
u32 gxSetCurrentMtx(u32 id, bool bForce);
u32 gxSetNumChans(u32 numChans);
u32 gxSetNumTevStages(u32 numTEV);
u32 gxGetNumTevStages();
u32 gxSetNumTexGens(u32 numGens);
u32 gxGetNumTexGens();
void gxSetTevOrder(s32 stage, s32 coord, s32 map, s32 colour);
void gxSetTevColourOp(s32 stage, s32 op, s32 bias, s32 scale, bool clamp, s32 out_reg);
void gxSetTevAlphaOp(s32 stage, s32 op, s32 bias, s32 scale, bool clamp, s32 out_reg);
void gxSetTexCoordGen(s32 dst_coord, s32 func, s32 src_param, u32 arg);
void gxSetTexCoordGen(s32 dst_coord, s32 func, s32 src_param, u32 arg, bool normalize, u32 pt_texmtx);
nlColour gxSetChanMatColour(s32 chan, const nlColour& colour);
nlColour gxSetChanAmbColour(s32 chan, const nlColour& colour);
bool gxSetCoPlanar(bool coplanar);

#endif // _GLXGX_H_
