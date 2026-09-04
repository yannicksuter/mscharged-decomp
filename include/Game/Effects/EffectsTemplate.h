#ifndef GAME_EFFECTS_EFFECTS_TEMPLATE_H
#define GAME_EFFECTS_EFFECTS_TEMPLATE_H

#include "NL/nlColour.h"
#include "NL/nlMath.h"

class nlChunk;

struct fxRange
{
    /* 0x00 */ float base;
    /* 0x04 */ float range;
}; // size: 0x08

enum eEffectsEmitter
{
    Emitter_Circle = 0,
    Emitter_Sphere = 1,
    Emitter_Spindle = 2,
    Emitter_Hemisphere = 3,
    Emitter_Unidentified4 = 4,
};

enum eEffectsBlend
{
    EfBlend_Normal = 0,
    EfBlend_Additive = 1,
};

enum eEffectsBillboard
{
    EfBill_Billboard = 0,
    EfBill_Groundboard = 1,
    EfBill_SoftwareControlled = 2,
};

extern unsigned int uSeed;

struct UnidentifiedEffectsKey_802E0010
{
    /* 0x00 */ float mUnidentified000;
    /* 0x04 */ float mUnidentified004;
    /* 0x08 */ float mUnidentified008;
    /* 0x0C */ float mUnidentified00C;
    /* 0x10 */ float mUnidentified010;
}; // size: 0x14

class Unidentified_802E0010
{
public:
    float fn_802E0010(float value) const;
    float fn_802E0108() const;

    /* 0x00 */ unsigned long mUnidentified000;
    /* 0x04 */ float base;
    /* 0x08 */ float range;
    /* 0x0C */ unsigned long mUnidentified00C;
    /* 0x10 */ UnidentifiedEffectsKey_802E0010* mUnidentified010;
}; // size: 0x14

class EffectsTemplate
{
public:
    static EffectsTemplate* fn_802E01C8(nlChunk* chunk);
    void fn_802E04C8();
    float fn_802E04CC() const;

    /* 0x00 */ unsigned long m_uHashID;
    /* 0x04 */ float m_fFountainLife;
    /* 0x08 */ fxRange m_rMass;
    /* 0x10 */ fxRange m_rParticleLife;
    /* 0x18 */ fxRange m_rInheritVelocity;
    /* 0x20 */ fxRange m_rAcceleration;
    /* 0x28 */ fxRange m_rRotation;
    /* 0x30 */ float mUnidentified030;
    /* 0x34 */ unsigned char m_eEmitter;
    /* 0x35 */ unsigned char m_eBlend;
    /* 0x36 */ unsigned char m_eBillboard;
    /* 0x37 */ unsigned char mUnidentified037;
    /* 0x38 */ unsigned long m_hTexture;
    /* 0x3C */ int m_nFrames;
    /* 0x40 */ unsigned long mUnidentified040;
    /* 0x44 */ unsigned long mUnidentified044;
    /* 0x48 */ unsigned long mUnidentified048;
    /* 0x4C */ fxRange m_rFPS;
    /* 0x54 */ unsigned long m_uModelID;
    /* 0x58 */ Unidentified_802E0010* mUnidentified058[8];
    /* 0x78 */ nlColour m_cColour[25];

    bool IsInFront() const { return (mUnidentified037 & 1) != 0; }
    bool IsLocalSpace() const { return (mUnidentified037 & 2) != 0; }
    bool IsLit() const { return (mUnidentified037 & 4) != 0; }
}; // size: 0xDC

inline float RandomizedValue(float base, float range)
{
    float randomOffset = nlRandomf(0.5f * range, &uSeed);
    unsigned int randomSign = nlRandom(0x7FFFFFFF, &uSeed);

    if (randomSign & 1)
    {
        return base + randomOffset;
    }
    else
    {
        return base - randomOffset;
    }
}

inline float RandomizedValue(const fxRange& value)
{
    return RandomizedValue(value.base, value.range);
}

#endif // GAME_EFFECTS_EFFECTS_TEMPLATE_H
