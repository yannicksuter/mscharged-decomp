#include <revolution/gx.h>
#include <revolution/mtx.h>

#include "NL/glx/glxSkinMatrix.h"
#include "NL/glx/glxMatrix.h"

unsigned long glx_SkinMatrixSlots[10] = {
    GX_PNMTX1, GX_PNMTX2, GX_PNMTX3, GX_PNMTX4, GX_PNMTX5, GX_PNMTX6, GX_PNMTX7, GX_PNMTX8, GX_PNMTX9, GX_PNMTX0
};

void glx_LoadDefaultSkinMatrices(const nlMatrix4* matrix)
{
    Mtx matrix0;
    Mtx matrix1;
    glxCopyMatrix(matrix0, *matrix);
    PSMTXInvXpose(matrix0, matrix1);
    for (int i = 0; i < 9; ++i)
    {
        GXLoadPosMtxImm(matrix0, glx_SkinMatrixSlots[i]);
        GXLoadNrmMtxImm(matrix1, glx_SkinMatrixSlots[i]);
    }
}

void glx_LoadSkinMatrices(const Mtx* matrices, int count, const nlMatrix4* matrix, int)
{
    Mtx matrix0;
    Mtx matrix1;
    Mtx matrix2;
    glxCopyMatrix(matrix0, *matrix);
    for (int i = 0; i < count; ++i)
    {
        PSMTXConcat(matrix0, matrices[i], matrix1);
        PSMTXInvXpose(matrix1, matrix2);
        GXLoadPosMtxImm(matrix1, glx_SkinMatrixSlots[i]);
        GXLoadNrmMtxImm(matrix2, glx_SkinMatrixSlots[i]);
    }
}
