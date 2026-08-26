#ifndef GAME_AI_POWERUPS_H
#define GAME_AI_POWERUPS_H

#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"
#include "NL/nlTimer.h"
#include "Game/Drawable/DrawableObj.h"

class BlurHandler;
class cFielder;
class PhysicsObject;
class SFXEmitter;

class unk_8000CD44
{
public:
    unk_8000CD44(int);
    virtual ~unk_8000CD44();

private:
    /* 0x04 */ u8 mUnidentified004[0x10];
};

class PowerupBase;

class unk_804F47E0 : public unk_8000CD44
{
public:
    unk_804F47E0(PowerupBase* pPowerup)
        : unk_8000CD44(2)
        , m_pPowerup(pPowerup)
        , mUnidentified018(0)
    {
    }
    virtual ~unk_804F47E0();

private:
    /* 0x14 */ PowerupBase* m_pPowerup;
    /* 0x18 */ void* mUnidentified018;
};

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

enum eThrowStyle
{
    THROW_ARROW = 0,
    THROW_HORIZONTAL_LINE = 1,
    THROW_SURROUND = 2,
    THROW_SPREAD = 3,
    NUM_THROW_TYPES = 4,
};

struct unk_8009A5D8
{
    /* 0x00 */ int nnumOfPowerups;
    /* 0x04 */ eThrowStyle eStyle;
    /* 0x08 */ ePowerupSize eSize;
    /* 0x0C */ ePowerUpType eType;
    /* 0x10 */ float fRadius;
    /* 0x14 */ bool bExplode;
}; // total size: 0x18

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
    virtual void ThrowAt(cFielder* pThrower);
    virtual void fn_8009CAC0(cFielder* pFielder);
    virtual void Init(cFielder* pFielder);
    virtual void Update(float dt);

    float GetRadius() const;
    void fn_8009CEBC(const nlVector3& v3Unidentified);
    void fn_8009D500();
    void fn_8009D74C(float seconds, bool bEnableCollisions);
    void SpeedManagement();
    void UpdateTransform();
    static void CollisionCallback(PhysicsObject* pObjA,
        PhysicsObject* pObjB, const nlVector3& v3Pos, void* pParam);
    static unsigned long GetSoundType(ePowerUpType type, PowerupSound powerupSnd);
    static void PlayPowerupSound(ePowerUpType type, PowerupSound powerupSnd,
        PhysicsObject* pPhysObj, float fVol, void* pParam);
    static void PlayPowerupSound(ePowerUpType type, PowerupSound powerupSnd,
        const nlVector3& v3Pos, float fVol, void* pParam);
    static void StopPowerupInEffectSound(ePowerUpType type, PowerupSound powerupSnd,
        void* pParam);

    /* 0x04 */ bool m_bShouldDestroy;
    /* 0x08 */ DrawableObject* m_pDrawableObj;
    /* 0x0C */ PhysicsObject* m_pPhysicsObject;
    /* 0x10 */ cFielder* m_pTarget;
    /* 0x14 */ cFielder* m_pThrower;
    /* 0x18 */ unk_8000CD44* m_unk18;
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
    Bobomb(cFielder* pTarget, int nIndex, float fRadius,
        ePowerupSize eSize, bool bExplode);
    virtual ~Bobomb();
    static void operator delete(void* ptr)
    {
        m_BobombSlotPool.Free((Bobomb*)ptr);
    }

    virtual void Destroy(bool bSilent);
    virtual void ThrowAt(cFielder* pThrower);
    virtual void Update(float dt);

    void fn_8009F454(PowerupBase*, int nThrowOrder);

    /* 0xA9 */ bool mbIsMine;
    /* 0xAC */ float m_unkAC;
    /* 0xB0 */ SFXEmitter* pMovementEmitter;

    static SlotPool<Bobomb> m_BobombSlotPool;
}; // total size: 0xB4

class FreezeShell : public PowerupBase
{
public:
    FreezeShell(cFielder* pTarget, int nIndex, float fRadius,
        ePowerupSize eSize, bool bExplode);
    virtual ~FreezeShell();
    static void operator delete(void* ptr)
    {
        m_FreezeShellSlotPool.Free((FreezeShell*)ptr);
    }
    virtual void Update(float fDeltaT);
    void Destroy(bool bSilent);

    static SlotPool<FreezeShell> m_FreezeShellSlotPool;
}; // total size: 0xAC

class SpinyShell : public PowerupBase
{
public:
    SpinyShell(cFielder* pTarget, int nIndex, float fRadius,
        ePowerupSize eSize, bool bExplode);
    virtual ~SpinyShell();
    static void operator delete(void* ptr)
    {
        m_SpinyShellSlotPool.Free((SpinyShell*)ptr);
    }
    virtual void Update(float dt);
    void Destroy(bool bSilent);

    static SlotPool<SpinyShell> m_SpinyShellSlotPool;
}; // total size: 0xAC

class Banana : public PowerupBase
{
public:
    Banana(cFielder* pTarget, int nIndex, float fRadius,
        ePowerupSize eSize, bool bExplode);
    virtual ~Banana();
    static void operator delete(void* ptr)
    {
        m_BananaSlotPool.Free((Banana*)ptr);
    }
    virtual void Update(float dt);
    void Destroy(bool bSilent);
    virtual void ThrowAt(cFielder* pThrower);

    static SlotPool<Banana> m_BananaSlotPool;
}; // total size: 0xAC

class RedShell : public PowerupBase
{
public:
    RedShell(cFielder* pTarget, int nIndex, float fRadius,
        ePowerupSize eSize, bool bExplode);
    virtual ~RedShell();
    static void operator delete(void* ptr)
    {
        m_RedShellSlotPool.Free((RedShell*)ptr);
    }
    virtual void Update(float dt);
    void Destroy(bool bSilent);
    void SeekTarget();

    static SlotPool<RedShell> m_RedShellSlotPool;
}; // total size: 0xAC

class GreenShell : public PowerupBase
{
public:
    GreenShell(cFielder* pTarget, int nIndex, float fRadius,
        ePowerupSize eSize, bool bExplode);
    virtual ~GreenShell();
    static void operator delete(void* ptr)
    {
        m_GreenShellSlotPool.Free((GreenShell*)ptr);
    }
    virtual void Update(float dt);
    void Destroy(bool bSilent);

    static SlotPool<GreenShell> m_GreenShellSlotPool;
}; // total size: 0xAC

PowerupBase* FindPowerUp(unsigned long hashOfDrawable);
cFielder* FindPowerupTarget(cFielder* pThrower, ePowerUpType eType);
u8 PowerupCreateAndThrow(cFielder* pThrower, cFielder* pTarget,
    const unk_8009A5D8* pSettings);
void PowerupThrowPosition(int nThrowOrder, eThrowStyle eStyle,
    PowerupBase* pNewPowerup, PowerupBase* pFirstPowerup,
    unsigned short aDirection);
void CompactPowerups();
void InitializePowerups();

#endif // GAME_AI_POWERUPS_H
