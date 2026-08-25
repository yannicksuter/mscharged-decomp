#ifndef GAME_AI_POWERUPS_H
#define GAME_AI_POWERUPS_H

#include "NL/nlMath.h"
#include "NL/nlTimer.h"

class BlurHandler;
class Bowser;
class cFielder;
class DrawableObject;
class PhysicsObject;

enum ePowerUpType
{
    POWER_UP_NONE = -1,
    POWER_UP_GREEN_SHELL = 0,
    POWER_UP_RED_SHELL = 1,
    POWER_UP_SPINY_SHELL = 2,
    POWER_UP_FREEZE_SHELL = 3,
    POWER_UP_BANANA = 4,
    POWER_UP_BOBOMB = 5,
    POWER_UP_CHAIN_CHOMP = 6,
    NUM_DRAWABLE_POWER_UPS = 6,
    POWER_UP_MUSHROOM = 7,
    POWER_UP_STAR = 8,
    NUM_POWER_UPS = 9,
};

enum ePowerupSize
{
    POWERUPSIZE_SMALL = 0,
    POWERUPSIZE_MEDIUM = 1,
    POWERUPSIZE_LARGE = 2,
};

class PowerupBase
{
public:
    PowerupBase(cFielder* pTarget, ePowerUpType eType, float fRadius,
        ePowerupSize eSize, bool bExplode, int nIndex);
    virtual ~PowerupBase();
    virtual void Destroy(bool bSilent);
    virtual void PreThrow(cFielder* pFielder, Bowser* pBowser);
    virtual void ThrowAt(cFielder* pThrower, Bowser* pBowser);
    virtual void fn_8009CAC0(void*);
    virtual void Init(cFielder* pFielder, Bowser* pBowser);
    virtual void Update(float dt);

    float GetRadius() const;

    /* 0x04 */ bool m_bShouldDestroy;
    /* 0x08 */ DrawableObject* m_pDrawableObj;
    /* 0x0C */ PhysicsObject* m_pPhysicsObject;
    /* 0x10 */ cFielder* m_pTarget;
    /* 0x14 */ cFielder* m_pThrower;
    /* 0x18 */ void* m_unk18;
    /* 0x1C */ ePowerUpType m_eType;
    /* 0x20 */ bool m_unk20;
    /* 0x24 */ Timer mtActiveTimer;
    /* 0x2C */ Timer mtNoHitTimer;
    /* 0x34 */ Timer m_unk34;
    /* 0x3C */ Timer m_unk3C;
    /* 0x44 */ Timer m_unk44;
    /* 0x4C */ float m_unk4C;
    /* 0x50 */ nlVector3 m_unk50;
    /* 0x5C */ u16 m_aOrientation;
    /* 0x60 */ float m_scale;
    /* 0x64 */ nlVector3 m_v3Position;
    /* 0x70 */ nlVector3 m_v3PrevPosition;
    /* 0x7C */ nlVector3 m_v3Velocity;
    /* 0x88 */ const char* m_szStreakTexture;
    /* 0x8C */ float m_fBlurWidth;
    /* 0x90 */ float m_fBlurLength;
    /* 0x94 */ u32 m_uVoiceID;
    /* 0x98 */ BlurHandler* m_pBlurHandler;
    /* 0x9C */ s32 m_nIndex;
    /* 0xA0 */ s32 m_nThrowerPadID;
    /* 0xA4 */ ePowerupSize meSize;
    /* 0xA8 */ bool mbExploder;
}; // total size: 0xAC

PowerupBase* FindPowerUp(unsigned long hashOfDrawable);

#endif // GAME_AI_POWERUPS_H
