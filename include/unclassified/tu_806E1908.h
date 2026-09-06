#ifndef UNCLASSIFIED_TU_806E1908_H
#define UNCLASSIFIED_TU_806E1908_H

#include <RVLFaceLib/RFL_MiddleDatabase.h>

class PlatTexture;

struct TU806E1908
{
    TU806E1908();
    virtual ~TU806E1908();

    /* 0x04 */ bool mUnidentified04;
    /* 0x05 */ bool mUnidentified05;
    /* 0x06 */ unsigned char mPadding06[2];
    /* 0x08 */ void* mUnidentified08;
    /* 0x0C */ void* mUnidentified0C;
    /* 0x10 */ void* mUnidentified10;
    /* 0x14 */ void* mUnidentified14[10];
    /* 0x3C */ unsigned long mUnidentified3C[10];
    /* 0x64 */ PlatTexture* mUnidentified64[10];
    /* 0x8C */ RFLMiddleDB mUnidentified8C;
};

extern TU806E1908* lbl_806E1908;
extern "C" void fn_8026EF60(void* buffer, unsigned long size, void* userData);
extern "C" void fn_8026F21C(TU806E1908* object);
extern "C" bool fn_8026F280(TU806E1908* object, int index, int slot, RFLExpression value);
extern "C" bool fn_8026F358(TU806E1908* object, const RFLStoreData* data, int slot, RFLExpression value);

#endif // UNCLASSIFIED_TU_806E1908_H
