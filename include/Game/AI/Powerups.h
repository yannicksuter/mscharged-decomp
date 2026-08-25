#ifndef GAME_AI_POWERUPS_H
#define GAME_AI_POWERUPS_H

#include "NL/nlMath.h"
#include "NL/nlTimer.h"
#include "Game/Drawable/DrawableObj.h"

class BlurHandler;
class Bowser;
class cFielder;
class PhysicsObject;
class SFXEmitter;

struct PowerupSounds
{
    /* 0x00 */ unsigned long sndAcquire;
    /* 0x04 */ unsigned long sndActivate;
    /* 0x08 */ unsigned long sndInEffect;
    /* 0x0C */ unsigned long sndHit;
    /* 0x10 */ unsigned long sndBounceWall;
    /* 0x14 */ unsigned long sndBounceGround;
    /* 0x18 */ unsigned long sndExplode;
    /* 0x1C */ unsigned long sndEnd;
}; // total size: 0x20

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

struct PowerupModelPool
{
    PowerupModelPool() { mNum = 0; }
    void Initialize(int type, unsigned long objHashName);

    /* 0x000 */ int mNum;
    /* 0x004 */ DrawableObject* mObjs[NUM_DRAWABLE_POWER_UPS][25];
    /* 0x25C */ bool mFree[NUM_DRAWABLE_POWER_UPS][25];
}; // total size: 0x2F4

enum ePowerupSize
{
    POWERUPSIZE_SMALL = 0,
    POWERUPSIZE_MEDIUM = 1,
    POWERUPSIZE_LARGE = 2,
};

class PowerupBase
{
public:
    enum PowerupSound
    {
        PWRUP_SOUND_ACQUIRE = 0,
        PWRUP_SOUND_ACTIVATE = 1,
        PWRUP_SOUND_IN_EFFECT = 2,
        PWRUP_SOUND_HIT = 3,
        PWRUP_SOUND_BOUNCE_WALL = 4,
        PWRUP_SOUND_BOUNCE_GROUND = 5,
        PWRUP_SOUND_EXPLODE = 6,
        PWRUP_SOUND_END = 7,
    };

    PowerupBase(cFielder* pTarget, ePowerUpType eType, float fRadius,
        ePowerupSize eSize, bool bExplode, int nIndex);
    virtual ~PowerupBase();
    virtual void Destroy(bool bSilent);
    virtual void PreThrow(cFielder* pFielder);
    virtual void ThrowAt(cFielder* pThrower, Bowser* pBowser);
    virtual void fn_8009CAC0(void*);
    virtual void Init(cFielder* pFielder, Bowser* pBowser);
    virtual void Update(float dt);

    float GetRadius() const;
    static void CollisionCallback(PhysicsObject* pObjA,
        PhysicsObject* pObjB, const nlVector3& v3Pos, void* pParam);
    static unsigned long GetSoundType(ePowerUpType type, PowerupSound powerupSnd);

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

class Bobomb : public PowerupBase
{
public:
    /* 0xA9 */ bool mbIsMine;
    /* 0xAC */ float m_unkAC;
    /* 0xB0 */ SFXEmitter* pMovementEmitter;
}; // total size: 0xB4

PowerupBase* FindPowerUp(unsigned long hashOfDrawable);
void InitializePowerups();

#endif // GAME_AI_POWERUPS_H
