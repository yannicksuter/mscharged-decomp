#ifndef GAME_EFFECTS_EFFECTS_GROUP_H
#define GAME_EFFECTS_EFFECTS_GROUP_H

#include "Game/Effects/EffectsTemplate.h"
#include "NL/nlMath.h"

struct EffectsSpec
{
    /* 0x00 */ unsigned long m_uHashID;
    /* 0x04 */ EffectsTemplate* m_pTemplate;
    /* 0x08 */ unsigned char mPadding008[0x08];
    /* 0x10 */ float m_fDelay;
    /* 0x14 */ unsigned char mPadding014[0x08];
    /* 0x1C */ unsigned long m_bInFront;
    /* 0x20 */ unsigned char mPadding020[0x04];
    /* 0x24 */ unsigned long m_bLight;
    /* 0x28 */ unsigned char mPadding028[0x04];
    /* 0x2C */ nlVector3 m_vLocalOffset;
    /* 0x38 */ unsigned char mPadding038[0x04];
    /* 0x3C */ float m_fLingerStart;
    /* 0x40 */ float m_fLingerEnd;
    /* 0x44 */ unsigned char mPadding044[0x14];
}; // size: 0x58

class UserEffectSpec;

class EffectsGroup
{
public:
    unsigned long GetHashID() const { return m_hashID; }

    /* 0x00 */ unsigned long m_hashID;
    /* 0x04 */ EffectsSpec* m_specs;
    /* 0x08 */ int m_numSpecs;
    /* 0x0C */ bool m_isLingering;
    /* 0x10 */ UserEffectSpec** m_userSpecsPtr;
    /* 0x14 */ int m_userSpecs;
}; // size: 0x18

void SetEffectsGroupFountainLife(EffectsGroup* group, float life);

#endif // GAME_EFFECTS_EFFECTS_GROUP_H
