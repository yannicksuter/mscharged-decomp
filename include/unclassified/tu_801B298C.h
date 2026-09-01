#ifndef UNCLASSIFIED_TU_801B298C_H
#define UNCLASSIFIED_TU_801B298C_H

#include "NL/nlMath.h"
#include "types.h"

class PhysicsObject;
class DrawableObject;
struct glModelPacket;

typedef DrawableObject RenderObject;

class UnidentifiedThwompMember_801B2EAC
{
public:
    virtual ~UnidentifiedThwompMember_801B2EAC();
};

struct ThwompObject
{
    /* 0x00 */ int mState;
    /* 0x04 */ int mIndex;
    /* 0x08 */ bool mVisible;
    /* 0x09 */ u8 mPadding009[3];
    /* 0x0C */ PhysicsObject* mPhysics;
    /* 0x10 */ RenderObject* mDrawable;
    /* 0x14 */ UnidentifiedThwompMember_801B2EAC* mUnidentified014;
    /* 0x18 */ float mUnidentified018;
    /* 0x1C */ float mUnidentified01C;
    /* 0x20 */ float mUnidentified020;
    /* 0x24 */ float mUnidentified024;
    /* 0x28 */ int mUnidentified028;
    /* 0x2C */ u8 mPadding02C[0x1C];
    /* 0x48 */ glModelPacket* mPacket48;
    /* 0x4C */ glModelPacket* mPacket4C;
    /* 0x50 */ unsigned long mTexture50;
    /* 0x54 */ unsigned long mResolvedTexture54;
    /* 0x58 */ unsigned long mTexture58;
    /* 0x5C */ unsigned long mResolvedTexture5C;
    /* 0x60 */ unsigned long mTexture60;
    /* 0x64 */ unsigned long mResolvedTexture64;
}; // size: 0x68

extern "C"
{
    ThwompObject* fn_801B298C(ThwompObject*, int);
    ThwompObject* fn_801B2B60(ThwompObject*, int);
    void fn_801B2BFC(ThwompObject*);
    void fn_801B2C00(ThwompObject*, float);
    void fn_801B2DF4(ThwompObject*, float, float);
    void fn_801B2E64(ThwompObject*, bool);
    void fn_801B2EAC(ThwompObject*, int);
    const nlVector3* fn_801B327C(const ThwompObject*);
    void fn_801B3284(ThwompObject*);
    float fn_801B3364(const ThwompObject*);
    void fn_801B339C(ThwompObject*);
}

#endif // UNCLASSIFIED_TU_801B298C_H
