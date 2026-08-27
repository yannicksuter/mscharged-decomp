#ifndef _RUMBLEFILTER_H_
#define _RUMBLEFILTER_H_

#include "NL/nlMath.h"

class cCameraFilter
{
public:
    virtual void Update(float) = 0;
    virtual void Filter(const nlMatrix4&, nlMatrix4&) = 0;
    virtual void Reset() = 0;
    virtual int vfunc_0x14() = 0;
};

class UnidentifiedCameraFilter : public cCameraFilter
{
public:
    virtual void vfunc_0x18(nlVector3, float, float) = 0;
};

class cRumbleFilter : public cCameraFilter
{
public:
    cRumbleFilter();
    virtual void Update(float dt);
    virtual void Filter(const nlMatrix4& matViewIn, nlMatrix4& matViewOut);
    virtual void Reset();
    virtual int vfunc_0x14();

    void Rumble(float x, float y);

    /* 0x04 */ nlVector2 v2Pos0;
    /* 0x0C */ nlVector2 v2Pos1;
    /* 0x14 */ nlVector2 v2Vel0;
    /* 0x1C */ nlVector2 v2Vel1;
    /* 0x24 */ float Ks;
    /* 0x28 */ float Kd;
    /* 0x2C */ nlVector2 v2Force0;
    /* 0x34 */ nlVector2 v2Force1;
}; // total size: 0x3C

#endif // _RUMBLEFILTER_H_
