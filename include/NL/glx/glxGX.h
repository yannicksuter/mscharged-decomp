#ifndef _GLXGX_H_
#define _GLXGX_H_

#include "NL/nlColour.h"

void gxInit();
void gxSetTevKColourSel(int stage, int sel);
void gxSetTevKAlphaSel(int stage, int sel);
void gxSetTevColourIn(int stage, int a, int b, int c, int d);
void gxSetTevAlphaIn(int stage, int a, int b, int c, int d);
bool gxSetDither(bool dither);
bool gxSetColourUpdate(bool bOn);
bool gxSetAlphaUpdate(bool bOn);
bool gxSetZCompLoc(bool bBefore);
void gxSetZMode(bool bTest, int func, bool bWrite);
void gxSaveZMode();
void gxRestoreZMode();
void gxSetAlphaCompare(int func, unsigned char ref);
void gxSaveBlendMode();
void gxRestoreBlendMode();
void gxSetBlendMode(bool bBlend, int src_factor, int dst_factor, bool bSubtract);
int gxSetCullMode(int mode);
unsigned int gxSetCurrentMtx(unsigned int id, bool bForce);
unsigned int gxSetNumChans(unsigned int numChans);
unsigned int gxSetNumTevStages(unsigned int numTEV);
unsigned int gxGetNumTevStages();
unsigned int gxSetNumTexGens(unsigned int numGens);
unsigned int gxGetNumTexGens();
void gxSetTevOrder(int stage, int coord, int map, int colour);
void gxSetTevColourOp(int stage, int op, int bias, int scale, bool clamp, int out_reg);
void gxSetTevAlphaOp(int stage, int op, int bias, int scale, bool clamp, int out_reg);
void gxSetTexCoordGen(int dst_coord, int func, int src_param, unsigned int arg);
void gxSetTexCoordGen(int dst_coord, int func, int src_param, unsigned int arg, bool normalize, unsigned int pt_texmtx);
nlColour gxSetChanMatColour(int chan, const nlColour& colour);
nlColour gxSetChanAmbColour(int chan, const nlColour& colour);
bool gxSetCoPlanar(bool coplanar);

#endif // _GLXGX_H_
