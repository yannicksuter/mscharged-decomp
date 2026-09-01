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
    struct UnidentifiedSample
    {
        UnidentifiedSample()
            : mUnidentified00(-1)
        {
        }

        /* 0x00 */ int mUnidentified00;
        /* 0x04 */ nlVector3 mUnidentified04;
    }; // total size: 0x10

    UnidentifiedCameraFilter();
    virtual void Update(float);
    virtual void Filter(const nlMatrix4&, nlMatrix4&);
    virtual void Reset();
    virtual int vfunc_0x14();
    virtual void vfunc_0x18(nlVector3, float, float);

    /* 0x04 */ bool mUnidentified04;
    /* 0x05 */ u8 mPadding05[3];
    /* 0x08 */ int mUnidentified08;
    /* 0x0C */ float mUnidentified0C;
    /* 0x10 */ float mUnidentified10;
    /* 0x14 */ float mUnidentified14;
    /* 0x18 */ float mUnidentified18;
    /* 0x1C */ nlVector3 mUnidentified1C;
    /* 0x28 */ nlVector3 mUnidentified28;
    /* 0x34 */ nlVector3 mUnidentified34;
    /* 0x40 */ UnidentifiedSample mUnidentified40[2];
}; // total size: 0x60

class cRumbleFilter : public cCameraFilter
{
public:
    cRumbleFilter();
    virtual void Update(float dt);
    virtual void Filter(const nlMatrix4& matViewIn, nlMatrix4& matViewOut);
    virtual void Reset();
    virtual int vfunc_0x14();

    void Rumble(float x, float y, float ks, float kd);

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
