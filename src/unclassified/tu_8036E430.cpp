#include "NL/gl/glMatrix.h"
#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"

void glplatGetMatrix(unsigned long matrix, nlMatrix4& m)
{
    GLMatrix* matrixPtr = (GLMatrix*)matrix;
    matrixPtr->Get(m);
}

void glplatSetMatrix(unsigned long matrix, const nlMatrix4& m)
{
    GLMatrix* matrixPtr = (GLMatrix*)matrix;
    matrixPtr->Set(m);
}

extern "C" void fn_8036E438(
    glModelPacket* packet, bool permanent, void* allocator)
{
    ((UnidentifiedPacketResource*)packet->unknown10)->fn_Unknown2(packet);
    packet->displayList = 0;
    if (permanent)
    {
        packet->displayList = dlMakeDisplayList(packet, allocator, permanent);
    }
    packet->unknown2C = 0;
    packet->unknown28 = 0;
}

extern "C" void fn_8036E4C0(glModelPacket*, void*)
{
}
