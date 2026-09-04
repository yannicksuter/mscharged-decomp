#ifndef UNCLASSIFIED_TU_801A6AAC_H
#define UNCLASSIFIED_TU_801A6AAC_H

#include "types.h"

struct UnidentifiedMegaBallState;

extern "C" void fn_801A6AAC(
    UnidentifiedMegaBallState* pState, unsigned int nTextureIndex);

struct UnidentifiedMegaBallTween
{
    UnidentifiedMegaBallTween()
        : mUnidentified000(0.0f)
        , mUnidentified004(1.0f)
        , mUnidentified008(0.0f)
        , mUnidentified00C(0.0f)
        , mUnidentified010(0.0f)
        , mUnidentified014(0.0f)
        , mUnidentified018(0)
        , mUnidentified01C(false)
    {
    }

    /* 0x00 */ float mUnidentified000;
    /* 0x04 */ float mUnidentified004;
    /* 0x08 */ float mUnidentified008;
    /* 0x0C */ float mUnidentified00C;
    /* 0x10 */ float mUnidentified010;
    /* 0x14 */ float mUnidentified014;
    /* 0x18 */ int mUnidentified018;
    /* 0x1C */ bool mUnidentified01C;
}; // size: 0x20

struct UnidentifiedMegaBallState
{
    UnidentifiedMegaBallState();

    /* 0x00 */ float mUnidentified000;
    /* 0x04 */ float mUnidentified004;
    /* 0x08 */ float mUnidentified008;
    /* 0x0C */ float mUnidentified00C;
    /* 0x10 */ float mUnidentified010;
    /* 0x14 */ float mUnidentified014;
    /* 0x18 */ float mUnidentified018;
    /* 0x1C */ unsigned int mUnidentified01C;
    /* 0x20 */ unsigned int mUnidentified020;
    /* 0x24 */ unsigned int mUnidentified024;
    /* 0x28 */ bool mUnidentified028;
    /* 0x29 */ bool mUnidentified029;
    /* 0x2C */ UnidentifiedMegaBallTween mUnidentified02C;
    /* 0x4C */ UnidentifiedMegaBallTween mUnidentified04C;
    /* 0x6C */ UnidentifiedMegaBallTween mUnidentified06C;
}; // size: 0x8C

extern "C" void fn_801A6B64(
    UnidentifiedMegaBallState* pState, unsigned int nTextureIndex);
extern "C" bool fn_801A6C34(
    UnidentifiedMegaBallTween* pTween, float fDeltaT);
extern "C" void fn_801A6D44(UnidentifiedMegaBallState* pState,
    int nRepeat, float fTransitionTime, float fEnd, float fEndTime,
    float fStart);
extern "C" void fn_801A6D80(UnidentifiedMegaBallState* pState,
    int nRepeat, float fTransitionTime, float fEnd, float fEndTime,
    float fStart);
extern "C" void fn_801A6DC4(UnidentifiedMegaBallState* pState);
extern "C" void fn_801A6DD8();
extern "C" void fn_801A7258();
extern "C" UnidentifiedMegaBallState* fn_801A75A8(
    float fX, float fY, float fScale);
extern "C" void fn_801A7610(UnidentifiedMegaBallState* pState);
extern "C" UnidentifiedMegaBallState* fn_801A7620(unsigned int nIndex);
extern "C" UnidentifiedMegaBallState* fn_801A7634(unsigned int nIndex);
extern "C" UnidentifiedMegaBallState* fn_801A7648(
    float fX, float fY, float fScale, float fAngle);
extern "C" UnidentifiedMegaBallState* fn_801A76BC(unsigned int nIndex);
extern "C" void fn_801A76D0(
    unsigned int nTexture1, unsigned int nTexture2);
extern "C" void fn_801A76E4(
    bool bParam, float fX, float fY, float fScale);
extern "C" void fn_801A7800();
extern "C" float fn_801A78B8(UnidentifiedMegaBallState* pState1,
    UnidentifiedMegaBallState* pState2);
extern "C" void fn_801A7A3C(int nX, int nY, unsigned int nTexture,
    float fWidth, float fHeight, float fOpacity, float fAngle);
extern "C" void fn_801A7C40(void* pControllerInfo);
extern "C" void fn_801A7C48(float fDeltaT);
extern "C" void fn_801A8504();
extern "C" void fn_801A86D8(float fDeltaT);
extern "C" void fn_801A8908(unsigned int nCount);
extern "C" void fn_801A8C8C(unsigned int nIndex, int nValue);
extern "C" void fn_801A8CA0(float fDeltaT);
void UpdateAndRenderMegaBallIndicators(float fDeltaT);
extern "C" void fn_801A9074(void* pMessage);

#endif // UNCLASSIFIED_TU_801A6AAC_H
