#include "Game/AI/Powerups.h"

#include "Game/AI/Fielder.h"
#include "Game/Physics/PhysicsSphere.h"
#include "NL/globalpad.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

extern "C" DrawableObject* fn_8027725C(unsigned long);
extern "C" void fn_802772A4(DrawableObject*);

static int gBobombAnticipationVoiceID;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

static PowerupSounds powerupSounds[NUM_POWER_UPS] = {
    { 0x4E4E16EA, 0xA82B862F, 0x00000000, 0x00000000, 0xA807A02B, 0x00000000, 0x5FCB9348, 0x00000000 },
    { 0x4E4E16EA, 0xA82B862F, 0x27BD7734, 0x00000000, 0xA807A02B, 0x00000000, 0x5FCB9348, 0x00000000 },
    { 0x4E4E16EA, 0xA82B862F, 0x00000000, 0x00000000, 0xA807A02B, 0x00000000, 0x5FCB9348, 0x00000000 },
    { 0x4E4E16EA, 0xA82B862F, 0x00000000, 0x00000000, 0xA807A02B, 0x00000000, 0x5FCB9348, 0x1DFB5C33 },
    { 0x4E4E16EA, 0x8D7345C6, 0x00000000, 0x3F34E69A, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0x4E4E16EA, 0x48F70F34, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x5FCB9348, 0x00000000 },
    { 0x4E4E16EA, 0x96033ECB, 0x36088C3A, 0x302EE87F, 0x00000000, 0xDB5B98D6, 0x00000000, 0x00000000 },
    { 0x4E4E16EA, 0x56C25EB1, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xBCFAF477 },
    { 0x4E4E16EA, 0x00000000, 0x70886861, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
};

namespace
{
struct Pair
{
    /* 0x0 */ unsigned long hashId;
    /* 0x4 */ const PowerupBase* powerup;
}; // total size: 0x8

struct PowerupRegistry
{
    PowerupRegistry()
    {
        for (int i = 0; i < NUM_POWERUP_REGISTRY_ENTRIES; i++)
        {
            registry[i].hashId = 0;
        }
    }

    enum
    {
        NUM_POWERUP_REGISTRY_ENTRIES = 25
    };

    /* 0x0 */ Pair registry[NUM_POWERUP_REGISTRY_ENTRIES];
}; // total size: 0xC8

PowerupModelPool powerupModelPool;
PowerupRegistry powerupRegistry;
}

static u8 gbAlwaysSurround;

namespace
{
const unsigned long uFREEZE_SHELL_MASTER_OBJECT = nlStringLowerHash("gameplay/blueshell");
const unsigned long uSPINY_SHELL_MASTER_OBJECT = nlStringLowerHash("gameplay/spikeshell");
const unsigned long uGREEN_SHELL_MASTER_OBJECT = nlStringLowerHash("gameplay/greenshell");
const unsigned long uRED_SHELL_MASTER_OBJECT = nlStringLowerHash("gameplay/redshell");
const unsigned long uBANANA_MASTER_OBJECT = nlStringLowerHash("gameplay/banana");
const unsigned long uBOBOMB_MASTER_OBJECT = nlStringLowerHash("gameplay/bobomb");
const char* uBANANA_STREAK_TEXTURE;
}

/**
 * Offset/Address/Size: 0x2214 | 0x8009B874 | size: 0x8
 */
float PowerupBase::GetRadius() const
{
    return ((PhysicsSphere*)m_pPhysicsObject)->GetRadius();
}

/**
 * Offset/Address/Size: 0x401C | 0x8009D67C | size: 0xD0
 */
void PowerupBase::PreThrow(cFielder* pFielder)
{
    nlVector3 pos;

    if (pFielder->m_nPowerupAnimID == 0x59 || pFielder->m_nPowerupAnimID == 0x5B)
    {
        pos = pFielder->GetJointPosition(pFielder->m_nLeftHandJointIndex);
    }
    else
    {
        pos = pFielder->GetJointPosition(pFielder->m_nRightHandJointIndex);
    }

    m_v3Position = pos;
    m_pPhysicsObject->SetPosition(m_v3Position, PhysicsObject::WORLD_COORDINATES);
    m_v3Velocity = v3Zero;
    m_pPhysicsObject->SetLinearVelocity(v3Zero);
}

static inline PowerupBase* FindPowerUpImpl(unsigned long hashOfDrawable)
{
    const Pair* entry = powerupRegistry.registry;
    for (int i = 0; i < 25; ++i)
    {
        if (hashOfDrawable == entry->hashId)
        {
            return const_cast<PowerupBase*>(powerupRegistry.registry[i].powerup);
        }
        ++entry;
    }
    return 0;
}

static inline void RegisterPowerup(unsigned long hashID, PowerupBase* powerup)
{
    int j = 0;
    for (; j < 25; j++)
    {
        if (powerupRegistry.registry[j].hashId == 0)
        {
            powerupRegistry.registry[j].hashId = hashID;
            powerupRegistry.registry[j].powerup = powerup;
            return;
        }
    }

    extern void nlBreak();
    nlBreak();
}

static inline DrawableObject* AcquirePowerupModel(int type)
{
    for (int i = 0; i < 25; i++)
    {
        if (powerupModelPool.mFree[type][i])
        {
            powerupModelPool.mFree[type][i] = false;
            return powerupModelPool.mObjs[type][i];
        }
    }

    return 0;
}

/**
 * Offset/Address/Size: 0x3A44 | 0x8009DADC | size: 0x158
 */
void PowerupBase::Init(cFielder* pFielder, Bowser*)
{
    m_pDrawableObj = AcquirePowerupModel(m_eType);

    {
        DrawableObject* pD = m_pDrawableObj;
        pD->m_uObjectFlags |= 2;
        unsigned long hashID = m_pDrawableObj->GetHashID();
        RegisterPowerup(hashID, this);
    }

    PreThrow(pFielder);

    m_pThrower = pFielder;

    if (pFielder != 0)
    {
        s32 padID;
        bool bHasPad = pFielder->GetGlobalPad() != 0;
        padID = bHasPad ? pFielder->GetGlobalPad()->fn_80332748() : -1;
        m_nThrowerPadID = padID;
    }
}

/**
 * Offset/Address/Size: 0x4194 | 0x8009DC34 | size: 0x1C
 */
unsigned long PowerupBase::GetSoundType(ePowerUpType type, PowerupSound powerupSnd)
{
    return (&powerupSounds[type].sndAcquire)[powerupSnd];
}

/**
 * Offset/Address/Size: 0x15FC | 0x8009AC5C | size: 0x4C
 */
PowerupBase* FindPowerUp(unsigned long hashOfDrawable)
{
    return FindPowerUpImpl(hashOfDrawable);
}

inline void PowerupModelPool::Initialize(int type, unsigned long objHashName)
{
    DrawableObject* obj;
    int i;
    char name[32];

    obj = fn_8027725C(objHashName);
    i = 0;

    obj->m_uObjectFlags &= ~1;

    for (; i < 25; i++)
    {
        nlSNPrintf(name, sizeof(name), "powerup_generated_%d", mNum);
        mObjs[type][i] = obj->Clone(nlStringLowerHash(name));
        mObjs[type][i]->m_uObjectFlags &= ~1;
        fn_802772A4(mObjs[type][i]);
        mFree[type][i] = true;
        mNum++;
    }
}

/**
 * Offset/Address/Size: 0x1648 | 0x8009ACA8 | size: 0x404
 */
void InitializePowerups()
{
    powerupModelPool.mNum = 0;
    powerupModelPool.Initialize(POWER_UP_FREEZE_SHELL, uFREEZE_SHELL_MASTER_OBJECT);
    powerupModelPool.Initialize(POWER_UP_SPINY_SHELL, uSPINY_SHELL_MASTER_OBJECT);
    powerupModelPool.Initialize(POWER_UP_GREEN_SHELL, uGREEN_SHELL_MASTER_OBJECT);
    powerupModelPool.Initialize(POWER_UP_RED_SHELL, uRED_SHELL_MASTER_OBJECT);
    powerupModelPool.Initialize(POWER_UP_BANANA, uBANANA_MASTER_OBJECT);
    powerupModelPool.Initialize(POWER_UP_BOBOMB, uBOBOMB_MASTER_OBJECT);
}
