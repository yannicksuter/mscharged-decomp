#ifndef NL_GLX_TU_8036D894_H
#define NL_GLX_TU_8036D894_H

#include "NL/gl/glTarget.h"
#include "NL/nlColour.h"

class PlatTexture;

class TargetPlatform_8036DE50
{
public:
    TargetPlatform_8036DE50(const TargetInfo_8036DE50* targetInfo)
        : mUnidentified024(-1)
        , mUnidentified028(0)
    {
        mUnidentified010 = targetInfo->width;
        mUnidentified014 = targetInfo->height;
        mUnidentified020 = targetInfo->format;
        mUnidentified018 = (targetInfo->unknown18 & 1) != 0;
        mUnidentified019 = (targetInfo->unknown18 & 4) != 0;
        mUnidentified019 |= targetInfo->unknown1C != 0;
        nlColourSet(mUnidentified01A, targetInfo->colour[0], targetInfo->colour[1], targetInfo->colour[2], targetInfo->colour[3]);
        mUnidentified008 = 0;
        mUnidentified00C = 0;
    }

    virtual ~TargetPlatform_8036DE50();
    virtual void fn_8036D9A0(unsigned long mode);
    virtual void fn_8036D9A8();
    virtual void fn_8036D9AC(unsigned long hash);
    virtual void fn_8036DBAC(unsigned long hash);
    virtual void fn_8036DBB0();
    virtual void fn_8036DBCC(bool flag0, bool flag1, bool flag2);
    virtual void fn_8036DBA8();

    void fn_8036DD14(bool flag0, bool flag1);

    /* 0x04 */ unsigned long mUnidentified004;
    /* 0x08 */ void* mUnidentified008;
    /* 0x0C */ PlatTexture* mUnidentified00C;
    /* 0x10 */ unsigned long mUnidentified010;
    /* 0x14 */ unsigned long mUnidentified014;
    /* 0x18 */ bool mUnidentified018;
    /* 0x19 */ bool mUnidentified019;
    /* 0x1A */ nlColour mUnidentified01A;
    /* 0x20 */ int mUnidentified020;
    /* 0x24 */ int mUnidentified024;
    /* 0x28 */ unsigned long mUnidentified028;
}; // size: 0x2C

extern "C" TargetPlatform_8036DE50* fn_8036D894();
extern "C" void fn_8036D89C();
extern "C" TargetPlatform_8036DE50* fn_8036DE50(const TargetInfo_8036DE50* targetInfo);
extern "C" void fn_8036DF24(TargetPlatform_8036DE50* target, bool flag0, bool flag1);

#endif // NL_GLX_TU_8036D894_H
