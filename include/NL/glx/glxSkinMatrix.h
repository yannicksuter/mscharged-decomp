#ifndef NL_GLX_GLXSKINMATRIX_H
#define NL_GLX_GLXSKINMATRIX_H

class nlMatrix4;

extern unsigned long glx_SkinMatrixSlots[10];

void glx_LoadDefaultSkinMatrices(const nlMatrix4* matrix);
void glx_LoadSkinMatrices(const float (*matrices)[3][4], int count, const nlMatrix4* matrix, int unknown);

#endif
