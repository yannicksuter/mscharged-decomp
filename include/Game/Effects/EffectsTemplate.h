#ifndef GAME_EFFECTS_EFFECTS_TEMPLATE_H
#define GAME_EFFECTS_EFFECTS_TEMPLATE_H

#include "NL/nlMath.h"

class nlChunk;

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

    /* 0x00 */ unsigned char mUnidentified000[0x10];
    /* 0x10 */ float mUnidentified010;
    /* 0x14 */ float mUnidentified014;
    /* 0x18 */ unsigned char mUnidentified018[0x1C];
    /* 0x34 */ unsigned char m_eEmitter;
    /* 0x35 */ unsigned char mPadding035[3];
    /* 0x38 */ unsigned long m_hTexture;
    /* 0x3C */ unsigned char mUnidentified03C[0x18];
    /* 0x54 */ unsigned long m_uModelID;
    /* 0x58 */ Unidentified_802E0010* mUnidentified058[8];
}; // size: 0x78

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

#endif // GAME_EFFECTS_EFFECTS_TEMPLATE_H
