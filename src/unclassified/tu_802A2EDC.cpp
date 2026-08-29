#include <revolution/gx.h>

#include "NL/gl/glMatrix.h"
#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/platvmath.h"

void gxSetTevColourOp(int, int, int, int, bool, int);
void gxSetTevAlphaOp(int, int, int, int, bool, int);
void gxSetTexCoordGen(int, int, int, unsigned int);
unsigned int gxSetNumTexGens(unsigned int);
unsigned int gxSetNumChans(unsigned int);
unsigned int gxSetNumTevStages(unsigned int);
void gxSetTevOrder(int, int, int, int);
void gxSetTevColourIn(int, int, int, int, int);
void gxSetTevAlphaIn(int, int, int, int, int);
unsigned int gxSetCurrentMtx(unsigned int, bool);

class MatrixProvider_802A2EDC
{
public:
    virtual void getMatrix(nlMatrix4*);
};

struct SetupContext_802A2EDC
{
    u8 unknown00[0x50];
    MatrixProvider_802A2EDC* matrixProvider;
};

struct SkinParameters_802A2FF8
{
    u8 diffuse[8];
    void* matrices;
    u32 matrixBytes;
};

extern nlMatrix4 lbl_8057B470;

extern "C" void fn_802A6280(void*, bool);
extern "C" void fn_802A6348(void*, const glModelPacket*);
extern "C" void fn_802A63A0(void*, const glModelPacket*);
extern "C" void fn_802CC978(void*, const glModelPacket*, u32);
extern "C" void fn_8036D774(const nlMatrix4*);
extern "C" void fn_8036D7EC(void*, u32, const nlMatrix4*, int);

extern "C" void fn_802A2EDC(void* renderer, SetupContext_802A2EDC* context)
{
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevAlphaOp(0, 0, 0, 0, true, 0);
    gxSetTexCoordGen(0, 1, 4, 60);
    gxSetNumTexGens(1);
    gxSetNumChans(0);
    gxSetNumTevStages(1);
    gxSetTevOrder(0, 0, 0, 255);
    gxSetTevColourIn(0, 15, 12, 8, 15);
    gxSetTevAlphaIn(0, 7, 6, 4, 7);
    fn_802A6280(renderer, true);

    MatrixProvider_802A2EDC* provider = context->matrixProvider;
    provider->getMatrix(&lbl_8057B470);
}

extern "C" void fn_802A2FE0()
{
    gxSetCurrentMtx(0, true);
}

extern "C" void fn_802A2FEC(void* renderer, const glModelPacket* packet)
{
    u32* parameter = static_cast<u32*>(packet->unknown20);
    fn_802CC978(renderer, packet, *parameter);
}

extern "C" void fn_802A2FF8(void* renderer, const glModelPacket* packet)
{
    fn_802A6348(renderer, packet);
    fn_802A63A0(renderer, packet);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);

    nlMatrix4 matrix;
    nlMatrix4 product;
    glGetMatrix(packet->matrix, matrix);
    nlMultMatrices(product, matrix, lbl_8057B470);

    if (packet->unknown28 == 0)
    {
        SkinParameters_802A2FF8* parameters = static_cast<SkinParameters_802A2FF8*>(packet->unknown20);
        fn_8036D7EC(parameters->matrices, parameters->matrixBytes / 0x30, &product, 0);
    }
    else
    {
        fn_8036D774(&product);
    }

    GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    gxSetCurrentMtx(0, true);
}
