#include "Game/AI/Powerups.h"

#include "Game/AI/AIPad.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/GameTweaks.h"
#include "Game/GameInfo.h"
#include "Game/ObjectBlur.h"
#include "Game/Physics/PhysicsBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Physics/PhysicsSphere.h"
#include "Game/Team.h"
#include "NL/globalpad.h"
#include "NL/nlFunction.h"
#include "NL/nlPrint.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"

inline void* operator new(unsigned long, void* memory)
{
    return memory;
}

extern "C" DrawableObject* fn_8027725C(unsigned long);
extern "C" void fn_802772A4(DrawableObject*);
extern "C" bool fn_8003877C(cFielder*);
extern "C" bool fn_800387CC(cFielder*);
extern "C" bool fn_8003886C(cFielder*);
extern "C" bool fn_800EBBFC(int, unsigned long, const void*, void*);
extern "C" void fn_800EBF78(int, unsigned long, const void*, void*, int);
extern "C" void fn_800EC12C(unsigned long, void*);
extern "C" PlayerTweaks* fn_8003E6E4(cFielder*);
extern "C" float fn_8002BFA8(PlayerTweaks*, float);
extern "C" float fn_8002CFF0(PlayerTweaks*);
extern "C" bool fn_800344B0(cFielder*);
extern "C" void fn_80146964(void*);
extern "C" void fn_8014777C(void*);
extern "C" void fn_801478C4(void*);
extern "C" void fn_80147A0C(void*);
extern "C" void fn_80147B54(void*);
extern "C" EffectsGroup* fn_802E7CDC(EmissionManager*, const char*);
extern "C" EmissionController* fn_802E7FE4(EmissionManager*, EffectsGroup*, int, bool, bool);
extern "C" void fn_8009F1B8(EmissionController&);
extern "C" bool fn_8002D2C4(nlVector3*, bool, float);
extern "C" void fn_800F0240(float, float, float, float);
extern "C" void fn_8017642C(const nlVector3*, cFielder*, bool, int, float);

enum eGameState
{
    GS_GAMEPLAY = 4,
};

struct cGame
{
    /* 0x00 */ u8 unknown_0x00[0x18];
    /* 0x18 */ eGameState m_eGameState;
};

extern cGame* lbl_806E0C94;

extern float lbl_806DBDA4;
extern float lbl_806DBDA8;
extern float lbl_806DBDAC;
extern float lbl_806DBDB0;
extern float lbl_806DBDB4;
extern float lbl_806DBDB8;
extern float lbl_806DBDBC;
extern float lbl_806DBDC8;
extern float lbl_806DBDCC;
extern float lbl_806DBDD0;
extern float lbl_806DBDD4;
extern bool lbl_806DBDD8;
extern float lbl_806DBDDC;
extern int gBobombAnticipationVoiceID;

struct CollisionBallShellData
{
    cBall* pBall;
    PowerupBase* pPowerup;
    nlVector3 v3CollisionVelocity;
};

struct CollisionPlayerBananaData
{
    cFielder* pPlayer;
    cFielder* pThrower;
    int nThrowerPadID;
    nlVector3 v3CollisionLocation;
};

struct CollisionPlayerShellData
{
    cFielder* pPlayer;
    cFielder* pThrower;
    u8 nThrowerPadID;
    bool bIsExploder;
    int eSize;
    nlVector3 v3CollisionLocation;
    nlVector3 v3CollisionVelocity;
};

struct CollisionPlayerFreezeData
{
    cFielder* pPlayer;
    cFielder* pThrower;
    int nThrowerPadID;
    int eSize;
};

struct PowerupHitPlayerEventData
{
    ePowerUpType Type;
    cPlayer* Thrower;
    cPlayer* Target;
};

extern SlotPool<CollisionBallShellData> lbl_80571618;
extern SlotPool<CollisionPlayerShellData> lbl_80571668;
extern SlotPool<CollisionPlayerFreezeData> lbl_80571690;
extern SlotPool<CollisionPlayerBananaData> lbl_805716B8;
extern SlotPool<PowerupHitPlayerEventData> lbl_805719D8;

static int lbl_806E0DA8;
static float lbl_806DBDE0 = 0.5f;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

unsigned long uPowerupTexID[NUM_POWER_UPS] = {
    nlStringLowerHash("fe/shell_green"),
    nlStringLowerHash("fe/shell_red"),
    nlStringLowerHash("fe/shell_spike"),
    nlStringLowerHash("fe/shell_blue"),
    nlStringLowerHash("fe/banana"),
    nlStringLowerHash("fe/babomb"),
    nlStringLowerHash("fe/chomp"),
    nlStringLowerHash("fe/mushroom"),
    nlStringLowerHash("fe/star"),
};

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

PowerupBase* g_pPowerups[25] = { 0 };

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

const char* uFREEZE_SHELL_STREAK_TEXTURE = "global/blueshellstreak";
const char* uSPINY_SHELL_STREAK_TEXTURE = "global/redshellstreak";
const char* uGREEN_SHELL_STREAK_TEXTURE = "global/greenshellstreak";
const char* uRED_SHELL_STREAK_TEXTURE = "global/redshellstreak";
const char* uBOBOMB_STREAK_TEXTURE = "global/bobombstreak";
} // namespace

static u8 gbAlwaysSurround = true;

SlotPool<FreezeShell> FreezeShell::m_FreezeShellSlotPool(16, 16);
SlotPool<GreenShell> GreenShell::m_GreenShellSlotPool(16, 16);
SlotPool<SpinyShell> SpinyShell::m_SpinyShellSlotPool(16, 16);
SlotPool<RedShell> RedShell::m_RedShellSlotPool(16, 16);
SlotPool<Banana> Banana::m_BananaSlotPool(16, 16);
SlotPool<Bobomb> Bobomb::m_BobombSlotPool(16, 16);

namespace
{
const unsigned long uFREEZE_SHELL_MASTER_OBJECT = nlStringLowerHash("gameplay/blueshell");
const unsigned long uSPINY_SHELL_MASTER_OBJECT = nlStringLowerHash("gameplay/spikeshell");
const unsigned long uGREEN_SHELL_MASTER_OBJECT = nlStringLowerHash("gameplay/greenshell");
const unsigned long uRED_SHELL_MASTER_OBJECT = nlStringLowerHash("gameplay/redshell");
const unsigned long uBANANA_MASTER_OBJECT = nlStringLowerHash("gameplay/banana");
const unsigned long uBOBOMB_MASTER_OBJECT = nlStringLowerHash("gameplay/bobomb");
const char* uBANANA_STREAK_TEXTURE;
} // namespace

static inline void ReleasePowerupModel(ePowerUpType type, DrawableObject* pDrawable)
{
    int i;

    for (i = 0; i < 25; i++)
    {
        if (pDrawable == powerupModelPool.mObjs[type][i])
        {
            pDrawable->m_uObjectFlags &= ~1;
            powerupModelPool.mFree[type][i] = true;
            break;
        }
    }
}

extern "C" int fn_80099C80(ePowerUpType eType)
{
    int nUnidentified = 0;
    switch (eType)
    {
    case POWER_UP_MUSHROOM:
        nUnidentified = 1;
        break;
    default:
        break;
    }
    return nUnidentified;
}

extern "C" bool fn_80099C94(ePowerUpType eType)
{
    bool bUnidentified = false;
    switch (eType)
    {
    case POWER_UP_CHAIN_CHOMP:
    case POWER_UP_STAR:
        bUnidentified = true;
        break;
    default:
        break;
    }
    return bUnidentified;
}

extern "C" bool fn_80099CC4(ePowerUpType eType)
{
    bool bUnidentified = false;
    switch (eType)
    {
    case POWER_UP_GREEN_SHELL:
    case POWER_UP_RED_SHELL:
    case POWER_UP_SPINY_SHELL:
    case POWER_UP_FREEZE_SHELL:
    case POWER_UP_BANANA:
    case POWER_UP_BOBOMB:
        bUnidentified = true;
        break;
    default:
        break;
    }
    return bUnidentified;
}

extern "C" bool fn_80099CE8(int nUnidentified)
{
    return nUnidentified >= NUM_POWER_UPS && nUnidentified <= 20;
}

cFielder* FindPowerupTarget(cFielder* pThrower, ePowerUpType eType)
{
    float fBestScore = 99999.9f;
    cFielder* pBestCandidate = 0;
    cTeam* pTeam;
    unsigned short aDirection;
    int i;
    float fTempScore;
    cFielder* pCandidate;

    pTeam = pThrower->m_pTeam->GetOtherTeam();
    aDirection = pThrower->m_aActualFacingDirection;

    if (pThrower->m_pController != 0)
    {
        float fMag = pThrower->m_pController->GetMovementStickMagnitude();
        if (fMag > 0.01f)
        {
            aDirection = pThrower->m_pController->GetMovementStickDirection();
        }
    }

    for (i = 0; i < 4; i++)
    {
        fTempScore = 99999.9f;
        pCandidate = pTeam->GetFielder(i);

        if (!pCandidate->IsFallenDown()
            && !fn_8003886C(pCandidate)
            && !fn_800387CC(pCandidate))
        {
            fTempScore = pThrower->DoFlashLight(
                pCandidate->m_v3Position, aDirection, 0.0001f, 0.0f, 30.0f);
        }

        if (gbAlwaysSurround || eType == POWER_UP_BOBOMB)
        {
            if ((g_pBall->GetOwnerFielder() != 0
                    && g_pBall->GetOwnerFielder() == pCandidate)
                || (g_pBall->GetPassTargetFielder() != 0
                    && g_pBall->GetPassTargetFielder() == pCandidate))
            {
                fTempScore = 0.0f;
            }
        }

        if (fTempScore < fBestScore)
        {
            pBestCandidate = pCandidate;
            fBestScore = fTempScore;
        }
    }

    return pBestCandidate;
}

void PowerupThrowPosition(int nThrowOrder, eThrowStyle eStyle,
    PowerupBase* pNewPowerup, PowerupBase* pFirstPowerup,
    unsigned short aDirection)
{
    if (pFirstPowerup->m_eType == POWER_UP_BOBOMB)
    {
        ((Bobomb*)pNewPowerup)->fn_8009F454(pFirstPowerup, nThrowOrder);
        return;
    }

    float fPowerupOffSet
        = 2.0f * ((PhysicsSphere*)pFirstPowerup->m_pPhysicsObject)->GetRadius();
    if (eStyle == THROW_ARROW)
    {
        fPowerupOffSet += lbl_806DBDB8;
    }
    else
    {
        fPowerupOffSet += lbl_806DBDB4;
    }

    switch (eStyle)
    {
    case THROW_HORIZONTAL_LINE:
    {
        nlVector3 v3StartPosition;
        nlVector3 v3VelocityDirection;
        nlVector3 v3PerpToVelocity;

        pNewPowerup->m_v3Velocity = pFirstPowerup->m_v3Velocity;
        pNewPowerup->m_pPhysicsObject->SetLinearVelocity(
            pFirstPowerup->m_v3Velocity);

        v3VelocityDirection = pFirstPowerup->m_v3Velocity;
        v3VelocityDirection.z = 0.0f;
        if (nlVec3Length(v3VelocityDirection) > 0.01f)
        {
            float invLen = nlRecipSqrt(
                v3VelocityDirection.x * v3VelocityDirection.x
                    + v3VelocityDirection.y * v3VelocityDirection.y
                    + v3VelocityDirection.z * v3VelocityDirection.z,
                true);
            nlVec3Set(v3VelocityDirection,
                invLen * v3VelocityDirection.x,
                invLen * v3VelocityDirection.y,
                invLen * v3VelocityDirection.z);
        }
        else
        {
            nlPolarToCartesian(v3VelocityDirection.x,
                v3VelocityDirection.y, aDirection, 1.0f);
        }

        if (nThrowOrder % 2 == 0)
        {
            RotateVectorZAxis(v3PerpToVelocity, v3VelocityDirection, 0x4000);
        }
        else
        {
            RotateVectorZAxis(v3PerpToVelocity, v3VelocityDirection, 0xC000);
        }

        fPowerupOffSet *= (float)((nThrowOrder + 1) / 2);
        nlVec3Set(v3PerpToVelocity,
            fPowerupOffSet * v3PerpToVelocity.x,
            fPowerupOffSet * v3PerpToVelocity.y,
            fPowerupOffSet * v3PerpToVelocity.z);
        nlVec3Set(v3StartPosition,
            pFirstPowerup->m_v3Position.x + v3PerpToVelocity.x,
            pFirstPowerup->m_v3Position.y + v3PerpToVelocity.y,
            pFirstPowerup->m_v3Position.z + v3PerpToVelocity.z);

        pNewPowerup->m_v3Position = v3StartPosition;
        pNewPowerup->m_pPhysicsObject->SetPosition(
            pNewPowerup->m_v3Position, PhysicsObject::WORLD_COORDINATES);
        break;
    }
    case THROW_ARROW:
    {
        nlVector3 v3StartPosition;
        nlVector3 v3VelocityDirection;
        nlVector3 v3PerpToVelocity;
        nlVector3 v3Offset;

        pNewPowerup->m_v3Velocity = pFirstPowerup->m_v3Velocity;
        pNewPowerup->m_pPhysicsObject->SetLinearVelocity(
            pFirstPowerup->m_v3Velocity);

        v3VelocityDirection = pFirstPowerup->m_v3Velocity;
        v3VelocityDirection.z = 0.0f;
        if (nlVec3Length(v3VelocityDirection) > 0.01f)
        {
            float invLen = nlRecipSqrt(
                v3VelocityDirection.x * v3VelocityDirection.x
                    + v3VelocityDirection.y * v3VelocityDirection.y
                    + v3VelocityDirection.z * v3VelocityDirection.z,
                true);
            nlVec3Set(v3VelocityDirection,
                invLen * v3VelocityDirection.x,
                invLen * v3VelocityDirection.y,
                invLen * v3VelocityDirection.z);
        }
        else
        {
            nlPolarToCartesian(v3VelocityDirection.x,
                v3VelocityDirection.y, aDirection, 1.0f);
        }

        if (nThrowOrder % 2 == 0)
        {
            RotateVectorZAxis(v3PerpToVelocity, v3VelocityDirection, 0x4000);
        }
        else
        {
            RotateVectorZAxis(v3PerpToVelocity, v3VelocityDirection, 0xC000);
        }

        fPowerupOffSet *= (float)((nThrowOrder + 1) / 2);
        nlVec3Set(v3PerpToVelocity,
            fPowerupOffSet * v3PerpToVelocity.x,
            fPowerupOffSet * v3PerpToVelocity.y,
            fPowerupOffSet * v3PerpToVelocity.z);
        nlVec3Set(v3VelocityDirection,
            fPowerupOffSet * v3VelocityDirection.x,
            fPowerupOffSet * v3VelocityDirection.y,
            fPowerupOffSet * v3VelocityDirection.z);

        RotateVectorZAxis(
            v3VelocityDirection, v3VelocityDirection, 0x8000);

        nlVec3Add(v3Offset, v3PerpToVelocity, v3VelocityDirection);
        nlVec3Add(v3StartPosition, pFirstPowerup->m_v3Position, v3Offset);

        pNewPowerup->m_v3Position = v3StartPosition;
        pNewPowerup->m_pPhysicsObject->SetPosition(
            pNewPowerup->m_v3Position, PhysicsObject::WORLD_COORDINATES);
        break;
    }
    case THROW_SURROUND:
    {
        pNewPowerup->m_v3Position = pFirstPowerup->m_v3Position;
        pNewPowerup->m_pPhysicsObject->SetPosition(
            pNewPowerup->m_v3Position, PhysicsObject::WORLD_COORDINATES);

        nlPolar pCurrentVelocity;
        nlCartesianToPolar(pCurrentVelocity,
            pFirstPowerup->m_v3Velocity.x,
            pFirstPowerup->m_v3Velocity.y);

        s16 nFlipAngle = 0x1FFF;
        if (nThrowOrder % 2 != 0)
        {
            nFlipAngle = -nFlipAngle;
        }

        if (pFirstPowerup->m_pTarget != 0)
        {
            nFlipAngle += pCurrentVelocity.a;
        }
        else
        {
            nFlipAngle += pFirstPowerup->m_pThrower->m_aActualFacingDirection;
        }

        nlVector3 v3CurrentVelocity = { 0.0f, 0.0f, 0.0f };
        nlPolarToCartesian(v3CurrentVelocity.x, v3CurrentVelocity.y,
            (u16)nFlipAngle, pCurrentVelocity.r);

        pNewPowerup->m_v3Velocity = v3CurrentVelocity;
        pNewPowerup->m_pPhysicsObject->SetLinearVelocity(v3CurrentVelocity);
        break;
    }
    case THROW_SPREAD:
    {
        pNewPowerup->m_v3Position = pFirstPowerup->m_v3Position;
        pNewPowerup->m_pPhysicsObject->SetPosition(
            pNewPowerup->m_v3Position, PhysicsObject::WORLD_COORDINATES);

        nlVector3 v3CurrentVelocity;
        v3CurrentVelocity = pFirstPowerup->m_v3Velocity;

        s16 nFlipAngle = (s16)(((nThrowOrder + 1) / 2) * 0x1999);
        if (nThrowOrder % 2 != 0)
        {
            nFlipAngle = -nFlipAngle;
        }
        RotateVectorZAxis(
            v3CurrentVelocity, v3CurrentVelocity, (u16)nFlipAngle);

        pNewPowerup->m_v3Velocity = v3CurrentVelocity;
        pNewPowerup->m_pPhysicsObject->SetLinearVelocity(v3CurrentVelocity);
        break;
    }
    default:
        break;
    }
}

extern "C" float fn_8009A478(ePowerUpType eType, ePowerupSize eSize)
{
    float fUnidentified = 0.0f;

    switch (eType)
    {
    case POWER_UP_GREEN_SHELL:
    case POWER_UP_RED_SHELL:
    case POWER_UP_SPINY_SHELL:
    case POWER_UP_FREEZE_SHELL:
        switch (eSize)
        {
        case POWERUPSIZE_LARGE:
            fUnidentified = lbl_8056CF08.m_pGameTweaks->fShellBigRadius;
            break;
        case POWERUPSIZE_MEDIUM:
            fUnidentified = lbl_8056CF08.m_pGameTweaks->fShellMediumRadius;
            break;
        case POWERUPSIZE_SMALL:
            fUnidentified = lbl_8056CF08.m_pGameTweaks->fShellSmallRadius;
            break;
        }
        break;
    case POWER_UP_BANANA:
        switch (eSize)
        {
        case POWERUPSIZE_LARGE:
            fUnidentified = lbl_8056CF08.m_pGameTweaks->fBananaBigRadius;
            break;
        case POWERUPSIZE_MEDIUM:
            fUnidentified = lbl_8056CF08.m_pGameTweaks->fBananaMediumRadius;
            break;
        case POWERUPSIZE_SMALL:
            fUnidentified = lbl_8056CF08.m_pGameTweaks->fBananaSmallRadius;
            break;
        }
        break;
    case POWER_UP_BOBOMB:
        switch (eSize)
        {
        case POWERUPSIZE_LARGE:
            fUnidentified = lbl_8056CF08.m_pGameTweaks->fBobombBigRadius;
            break;
        case POWERUPSIZE_MEDIUM:
            fUnidentified = lbl_8056CF08.m_pGameTweaks->fBobombMediumRadius;
            break;
        case POWERUPSIZE_SMALL:
            fUnidentified = lbl_8056CF08.m_pGameTweaks->fBobombSmallRadius;
            break;
        }
        break;
    default:
        break;
    }

    return fUnidentified;
}

extern "C" void fn_8009A5D8(cFielder* pThrower, ePowerUpType eType,
    int nnumOfPowerups, unk_8009A5D8* pUnidentified)
{
    pUnidentified->bExplode = false;
    pUnidentified->nnumOfPowerups = nnumOfPowerups;
    pUnidentified->eStyle = THROW_ARROW;
    pUnidentified->eSize = POWERUPSIZE_SMALL;
    pUnidentified->eType = eType;
    pUnidentified->fRadius = 0.0f;

    float fMediumChance = lbl_8056CF08.m_pGameTweaks->fShellMediumChance;
    float fExplodeChance = lbl_8056CF08.m_pGameTweaks->fShellExplodeChance;

    switch (eType)
    {
    case POWER_UP_BANANA:
        fMediumChance = lbl_8056CF08.m_pGameTweaks->fBananaMediumChance;
        fExplodeChance = lbl_8056CF08.m_pGameTweaks->fBananaExplodeChance;
        break;
    case POWER_UP_BOBOMB:
        fMediumChance = lbl_8056CF08.m_pGameTweaks->fBobombMediumChance;
        fExplodeChance = 1.0f;
        break;
    default:
        break;
    }

    if (GameInfoManager::Instance()->GetRule0x0() == 4
        && pUnidentified->nnumOfPowerups == 1)
    {
        fMediumChance = 0.0f;
    }
    else if (GameInfoManager::Instance()->GetRule0x0() == 1)
    {
        fExplodeChance = 1.0f;
    }

    if (pUnidentified->nnumOfPowerups > 1)
    {
        if (pUnidentified->eType == POWER_UP_RED_SHELL)
        {
            pUnidentified->eStyle = THROW_SURROUND;
        }
        else if (pUnidentified->eType == POWER_UP_BANANA)
        {
            pUnidentified->eStyle = THROW_HORIZONTAL_LINE;
        }
        else if (pUnidentified->nnumOfPowerups > 3)
        {
            pUnidentified->eStyle = THROW_ARROW;
        }
        else
        {
            pUnidentified->eStyle = THROW_HORIZONTAL_LINE;
        }
    }
    else
    {
        if (nlRandomf(1.0f) <= fExplodeChance)
        {
            pUnidentified->bExplode = true;
        }

        if (nlRandomf(1.0f) < fMediumChance)
        {
            pUnidentified->eSize = POWERUPSIZE_MEDIUM;
        }
        else
        {
            pUnidentified->eSize = POWERUPSIZE_LARGE;
        }
    }

    pUnidentified->fRadius = fn_8009A478(
        pUnidentified->eType, pUnidentified->eSize);
}

inline GreenShell::GreenShell(cFielder* pTarget, int nIndex, float fRadius,
    ePowerupSize eSize, bool bExplode)
    : PowerupBase(pTarget, POWER_UP_GREEN_SHELL, fRadius, eSize, bExplode,
          nIndex)
{
}

inline RedShell::RedShell(cFielder* pTarget, int nIndex, float fRadius,
    ePowerupSize eSize, bool bExplode)
    : PowerupBase(pTarget, POWER_UP_RED_SHELL, fRadius, eSize, bExplode,
          nIndex)
{
}

inline Banana::Banana(cFielder* pTarget, int nIndex, float fRadius,
    ePowerupSize eSize, bool bExplode)
    : PowerupBase(pTarget, POWER_UP_BANANA, fRadius, eSize, bExplode, nIndex)
{
}

inline SpinyShell::SpinyShell(cFielder* pTarget, int nIndex, float fRadius,
    ePowerupSize eSize, bool bExplode)
    : PowerupBase(pTarget, POWER_UP_SPINY_SHELL, fRadius, eSize, bExplode,
          nIndex)
{
}

inline FreezeShell::FreezeShell(cFielder* pTarget, int nIndex, float fRadius,
    ePowerupSize eSize, bool bExplode)
    : PowerupBase(pTarget, POWER_UP_FREEZE_SHELL, fRadius, eSize, bExplode,
          nIndex)
{
}

inline Bobomb::Bobomb(cFielder* pTarget, int nIndex, float fRadius,
    ePowerupSize eSize, bool bExplode)
    : PowerupBase(pTarget, POWER_UP_BOBOMB, fRadius, eSize, bExplode, nIndex)
{
    pMovementEmitter = 0;
    mbIsMine = false;
    m_unkAC = lbl_806DBDE0;
}

u8 PowerupCreateAndThrow(cFielder* pThrower, cFielder* pTarget,
    const unk_8009A5D8* pSettings)
{
    PowerupBase* pFirstPowerup = 0;
    cTeam* pTargetTeam = pThrower->m_pTeam->GetOtherTeam();
    cFielder* pTargetFielders[4];
    for (int i = 0; i < 4; i++)
    {
        pTargetFielders[i] = pTargetTeam->GetFielder(i);
    }

    for (int j = 0; j < pSettings->nnumOfPowerups; j++)
    {
        bool bFoundLocation = false;
        for (int i = 0; i < 25; i++)
        {
            if (bFoundLocation)
            {
                continue;
            }
            if (g_pPowerups[i] != 0)
            {
                continue;
            }

            PowerupBase* pPowerup;
            switch (pSettings->eType)
            {
            case POWER_UP_BANANA:
            {
                Banana* pBanana = 0;
                Banana::m_BananaSlotPool.Allocate(pBanana);
                new (pBanana) Banana(pTarget, i, pSettings->fRadius,
                    pSettings->eSize, pSettings->bExplode);
                pPowerup = pBanana;
                break;
            }
            case POWER_UP_BOBOMB:
            {
                Bobomb* pBobomb = 0;
                Bobomb::m_BobombSlotPool.Allocate(pBobomb);
                new (pBobomb) Bobomb(pTarget, i, pSettings->fRadius,
                    pSettings->eSize, true);
                pPowerup = pBobomb;
                break;
            }
            case POWER_UP_GREEN_SHELL:
            {
                GreenShell* pGreenShell = 0;
                GreenShell::m_GreenShellSlotPool.Allocate(pGreenShell);
                new (pGreenShell) GreenShell(pTarget, i, pSettings->fRadius,
                    pSettings->eSize, pSettings->bExplode);
                pPowerup = pGreenShell;
                break;
            }
            case POWER_UP_FREEZE_SHELL:
            {
                FreezeShell* pFreezeShell = 0;
                FreezeShell::m_FreezeShellSlotPool.Allocate(pFreezeShell);
                new (pFreezeShell) FreezeShell(pTarget, i,
                    pSettings->fRadius, pSettings->eSize,
                    pSettings->bExplode);
                pPowerup = pFreezeShell;
                break;
            }
            case POWER_UP_RED_SHELL:
            {
                RedShell* pRedShell = 0;
                RedShell::m_RedShellSlotPool.Allocate(pRedShell);
                new (pRedShell) RedShell(pTarget, i, pSettings->fRadius,
                    pSettings->eSize, pSettings->bExplode);
                pPowerup = pRedShell;
                break;
            }
            case POWER_UP_SPINY_SHELL:
            {
                SpinyShell* pSpinyShell = 0;
                SpinyShell::m_SpinyShellSlotPool.Allocate(pSpinyShell);
                new (pSpinyShell) SpinyShell(pTarget, i,
                    pSettings->fRadius, pSettings->eSize,
                    pSettings->bExplode);
                pPowerup = pSpinyShell;
                break;
            }
            default:
                break;
            }

            pPowerup->Init(pThrower);
            if (pFirstPowerup == 0)
            {
                pPowerup->ThrowAt(pThrower);
                pFirstPowerup = pPowerup;
            }
            else
            {
                PowerupThrowPosition(j, pSettings->eStyle, pPowerup,
                    pFirstPowerup, pThrower->m_aActualFacingDirection);

                if (pPowerup->m_eType == POWER_UP_RED_SHELL)
                {
                    for (int i = 0; i < 4; i++)
                    {
                        if (pFirstPowerup->m_pTarget == pTargetFielders[i])
                        {
                            pTargetFielders[i] = 0;
                        }

                        if (pTargetFielders[i] != 0
                            && fn_800387CC(pTargetFielders[i]) == true)
                        {
                            pTargetFielders[i] = 0;
                        }

                        if (pTargetFielders[i] != 0)
                        {
                            pPowerup->m_pTarget = pTargetFielders[i];
                            pTargetFielders[i] = 0;
                            break;
                        }

                        pPowerup->m_pTarget = 0;
                    }
                }
            }

            g_pPowerups[i] = pPowerup;
            bFoundLocation = true;
        }
    }

    return 1;
}

/**
 * Offset/Address/Size: 0x1A50 | 0x8009B0AC | size: 0xB4
 */
void CompactPowerups()
{
    GreenShell::m_GreenShellSlotPool.FreeBlocks();
    RedShell::m_RedShellSlotPool.FreeBlocks();
    SpinyShell::m_SpinyShellSlotPool.FreeBlocks();
    FreezeShell::m_FreezeShellSlotPool.FreeBlocks();
    Banana::m_BananaSlotPool.FreeBlocks();
    Bobomb::m_BobombSlotPool.FreeBlocks();
}

/**
 * Offset/Address/Size: 0x1B00 | 0x8009B160 | size: 0x60C
 */
PowerupBase::PowerupBase(cFielder* pTarget, ePowerUpType eType, float fRadius,
    ePowerupSize eSize, bool bExplode, int nIndex)
    : m_bShouldDestroy(false)
    , m_pDrawableObj(0)
    , m_pTarget(pTarget)
    , m_eType(eType)
    , m_unk20(true)
{
    unk_804F47E0* pUnidentified = 0;

    m_aOrientation = 0;
    m_scale = 1.0f;
    m_szStreakTexture = 0;
    m_fBlurWidth = 0.0f;
    m_fBlurLength = 0.0f;
    m_uVoiceID = 0;
    m_pBlurHandler = 0;
    m_nIndex = nIndex;
    meSize = eSize;
    mbExploder = bExplode;

    switch (eType)
    {
    case POWER_UP_GREEN_SHELL:
    {
        PhysicsShell* pShell = (PhysicsShell*)nlMalloc(sizeof(PhysicsShell), 8, false);
        pShell = new (pShell) PhysicsShell(fRadius);
        m_pPhysicsObject = pShell;
        PhysicsShell* pObj = (PhysicsShell*)m_pPhysicsObject;
        pObj->m_pPowerupObject = this;
        pObj->m_pTriggerCallbackFunc = (void (*)(PhysicsObject*, PhysicsObject*, nlVector3&, void*))CollisionCallback;
        pObj->m_pCallbackParam = this;
        m_szStreakTexture = uGREEN_SHELL_STREAK_TEXTURE;
        mtActiveTimer.SetSeconds(lbl_8056CF08.m_unk14->mUnidentified404);
        m_fBlurWidth = 2.0f * (fRadius / 3.0f);
        m_fBlurLength = (f32)(2.0 * fRadius);
        break;
    }
    case POWER_UP_RED_SHELL:
    {
        PhysicsShell* pShell = (PhysicsShell*)nlMalloc(sizeof(PhysicsShell), 8, false);
        pShell = new (pShell) PhysicsShell(fRadius);
        m_pPhysicsObject = pShell;
        PhysicsShell* pObj = (PhysicsShell*)m_pPhysicsObject;
        pObj->m_pPowerupObject = this;
        pObj->m_pTriggerCallbackFunc = (void (*)(PhysicsObject*, PhysicsObject*, nlVector3&, void*))CollisionCallback;
        pObj->m_pCallbackParam = this;
        m_szStreakTexture = uRED_SHELL_STREAK_TEXTURE;
        mtActiveTimer.SetSeconds(lbl_8056CF08.m_unk14->mUnidentified404);
        m_fBlurWidth = 2.0f * (fRadius / 3.0f);
        m_fBlurLength = (f32)(2.0 * fRadius);
        break;
    }
    case POWER_UP_SPINY_SHELL:
    {
        PhysicsShell* pShell = (PhysicsShell*)nlMalloc(sizeof(PhysicsShell), 8, false);
        pShell = new (pShell) PhysicsShell(fRadius);
        m_pPhysicsObject = pShell;
        PhysicsShell* pObj = (PhysicsShell*)m_pPhysicsObject;
        pObj->m_pPowerupObject = this;
        pObj->m_pTriggerCallbackFunc = (void (*)(PhysicsObject*, PhysicsObject*, nlVector3&, void*))CollisionCallback;
        pObj->m_pCallbackParam = this;
        m_szStreakTexture = uSPINY_SHELL_STREAK_TEXTURE;
        mtActiveTimer.SetSeconds(lbl_8056CF08.m_unk14->mUnidentified404);
        m_fBlurWidth = 2.0f * (fRadius / 3.0f);
        m_fBlurLength = (f32)(2.0 * fRadius);
        break;
    }
    case POWER_UP_FREEZE_SHELL:
    {
        PhysicsShell* pShell = (PhysicsShell*)nlMalloc(sizeof(PhysicsShell), 8, false);
        pShell = new (pShell) PhysicsShell(fRadius);
        m_pPhysicsObject = pShell;
        PhysicsShell* pObj = (PhysicsShell*)m_pPhysicsObject;
        pObj->m_pPowerupObject = this;
        pObj->m_pTriggerCallbackFunc = (void (*)(PhysicsObject*, PhysicsObject*, nlVector3&, void*))CollisionCallback;
        pObj->m_pCallbackParam = this;
        m_szStreakTexture = uFREEZE_SHELL_STREAK_TEXTURE;
        mtActiveTimer.SetSeconds(lbl_8056CF08.m_unk14->mUnidentified404);
        m_fBlurWidth = 2.0f * (fRadius / 3.0f);
        m_fBlurLength = (f32)(2.0 * fRadius);
        break;
    }
    case POWER_UP_BANANA:
    {
        PhysicsBanana* pBanana = (PhysicsBanana*)nlMalloc(sizeof(PhysicsBanana), 8, false);
        pBanana = new (pBanana) PhysicsBanana(fRadius);
        m_pPhysicsObject = pBanana;
        PhysicsBanana* pObj = (PhysicsBanana*)m_pPhysicsObject;
        pObj->m_pPowerupObject = this;
        pObj->m_pTriggerCallbackFunc = (void (*)(PhysicsObject*, PhysicsObject*, nlVector3&, void*))CollisionCallback;
        pObj->m_pCallbackParam = this;
        m_szStreakTexture = uBANANA_STREAK_TEXTURE;
        mtActiveTimer.SetSeconds(lbl_8056CF08.m_pGameTweaks->fBananaActiveTime);
        m_fBlurWidth = 0.0f;
        m_fBlurLength = 0.0f;
        break;
    }
    case POWER_UP_BOBOMB:
    {
        PhysicsBanana* pBobomb = (PhysicsBanana*)nlMalloc(sizeof(PhysicsBanana), 8, false);
        pBobomb = new (pBobomb) PhysicsBanana(fRadius);
        m_pPhysicsObject = pBobomb;
        PhysicsBanana* pObj = (PhysicsBanana*)m_pPhysicsObject;
        pObj->m_pPowerupObject = this;
        pObj->m_pTriggerCallbackFunc = (void (*)(PhysicsObject*, PhysicsObject*, nlVector3&, void*))CollisionCallback;
        pObj->m_pCallbackParam = this;
        m_szStreakTexture = uBOBOMB_STREAK_TEXTURE;
        mtActiveTimer.SetSeconds(lbl_8056CF08.m_pGameTweaks->fBobombActiveTime);
        m_fBlurWidth = 0.5f * fRadius;
        m_fBlurLength = (f32)(2.0 * fRadius);
        break;
    }
    }

    m_aOrientation = nlRandom(65000);

    if (eType != POWER_UP_BANANA)
    {
        mtNoHitTimer.SetSeconds(0.4f);
    }
    else
    {
        mtNoHitTimer.SetSeconds(1.0f);
    }

    m_unk44.m_unk0 = m_unk44.m_uPackedTime != 0;
    m_unk44.m_uPackedTime = 0;
    m_unk3C.m_unk0 = m_unk3C.m_uPackedTime != 0;
    m_unk3C.m_uPackedTime = 0;
    m_unk34.m_unk0 = m_unk34.m_uPackedTime != 0;
    m_unk34.m_uPackedTime = 0;
    m_unk4C = lbl_806DBDC8;
    nlVector3 v3Unidentified = v3Zero;
    m_unk50 = v3Unidentified;

    m_v3Position.x = 0.0f;
    m_v3Position.y = 0.0f;
    m_v3Position.z = fRadius;
    m_v3PrevPosition = m_v3Position;
    m_v3Velocity = v3Zero;

    m_pPhysicsObject->SetPosition(
        m_v3Position, PhysicsObject::WORLD_COORDINATES);
    m_pPhysicsObject->SetLinearVelocity(m_v3Velocity);
    m_pPhysicsObject->EnableCollisions();

    pUnidentified = (unk_804F47E0*)nlMalloc(sizeof(unk_804F47E0), 8, false);
    pUnidentified = new (pUnidentified) unk_804F47E0(this);
    m_unk18 = pUnidentified;

    if (eType == POWER_UP_RED_SHELL)
    {
        unsigned long soundID = powerupSounds[eType].sndInEffect;
        if (soundID != 0)
        {
            if (eType == POWER_UP_STAR)
            {
                fn_800EBF78(0x12, soundID, "Powerup", this, 0);
            }
            else if (this != 0)
            {
                fn_800EBBFC(0x10, soundID, "Powerup", this);
            }
            else
            {
                fn_800EBBFC(0x10, soundID, 0, 0);
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x2110 | 0x8009B76C | size: 0x108
 */
PowerupBase::~PowerupBase()
{
    delete m_unk18;
    ReleasePowerupModel(m_eType, m_pDrawableObj);
    delete m_pPhysicsObject;
}

/**
 * Offset/Address/Size: 0x2214 | 0x8009B874 | size: 0x8
 */
float PowerupBase::GetRadius() const
{
    return ((PhysicsSphere*)m_pPhysicsObject)->GetRadius();
}

/**
 * Offset/Address/Size: 0x221C | 0x8009B87C | size: 0x214
 */
void PowerupBase::Update(float dt)
{
    if (m_unk44.m_uPackedTime == 0)
    {
        UpdateTransform();

        if (mtNoHitTimer.m_uPackedTime != 0)
        {
            mtNoHitTimer.Countdown(dt, 0.0f);
        }

        if (m_unk4C > 0.0f)
        {
            m_unk4C -= dt;
        }
        else
        {
            m_unk4C = 0.0f;
        }

        fn_8009D500();
        mtActiveTimer.Countdown(dt, 0.0f);
        m_unk34.Countdown(dt, 0.0f);

        if (m_pBlurHandler != 0)
        {
            nlPolar polar;
            nlCartesianToPolar(polar, m_v3Velocity.x, m_v3Velocity.y);
            if (polar.r < 0.5f)
            {
                m_pBlurHandler->Die(0.5f);
                m_pBlurHandler = 0;
            }
        }
    }
    else
    {
        m_pPhysicsObject->SetPosition(m_v3Position, PhysicsObject::WORLD_COORDINATES);
        m_v3Velocity = v3Zero;
        m_pPhysicsObject->SetLinearVelocity(v3Zero);

        if (m_unk44.Countdown(dt, 0.0f))
        {
            m_unk44.m_unk0 = m_unk44.m_uPackedTime != 0;
            m_unk44.m_uPackedTime = 0;
            m_unk3C.m_unk0 = m_unk3C.m_uPackedTime != 0;
            m_unk3C.m_uPackedTime = 0;
            m_v3Velocity = m_unk50;
            m_pPhysicsObject->SetLinearVelocity(m_unk50);
            m_pPhysicsObject->EnableCollisions();
            m_unk20 = true;

            if (m_szStreakTexture != 0 && m_pBlurHandler == 0)
            {
                m_pBlurHandler = BlurManager::GetNewHandler(
                    m_szStreakTexture, m_fBlurWidth, 15, true);
            }
        }

        if (m_unk3C.m_uPackedTime != 0 && m_unk3C.Countdown(dt, 0.0f))
        {
            m_unk3C.m_unk0 = m_unk3C.m_uPackedTime != 0;
            m_unk3C.m_uPackedTime = 0;
            m_unk20 = false;
        }
    }
}

/**
 * Offset/Address/Size: 0x2DE0 | 0x8009C440 | size: 0x680
 */
void PowerupBase::CollisionCallback(PhysicsObject* pObjA,
    PhysicsObject* pObjB, const nlVector3& v3Pos, void* pParam)
{
    PowerupBase* pObj = (PowerupBase*)pParam;
    cCharacter* pCharacter = 0;
    cPlayer* pPlayerTarget = 0;

    if (pObj->m_unk44.m_uPackedTime != 0)
    {
        return;
    }

    int type = pObjB->GetObjectType();
    switch (type)
    {
    case 0x04:
        pCharacter = ((PhysicsCharacter*)pObjB->m_parentObject)->m_pAICharacter;
        break;
    case 0x10:
    {
        cBall* pBall = ((PhysicsBall*)pObjB)->m_pBall;
        if (pBall->m_bBallPathChangeCount != 0)
        {
            if (pBall->m_pOwner == 0)
            {
                CollisionBallShellData* pData = 0;
                lbl_80571618.Allocate(pData);
                pData->pPowerup = pObj;
                pData->pBall = pBall;
                pData->v3CollisionVelocity = pObj->m_v3Velocity;
                fn_80146964(pData);
            }
            else if (pBall->GetOwnerFielder() != 0)
            {
                pCharacter = pBall->GetOwnerFielder();
            }
        }
        break;
    }
    case 0x14:
    {
        if (((PhysicsShell*)pObjB)->m_pPowerupObject->m_unk44.m_uPackedTime != 0)
        {
            break;
        }

        ePowerupSize otherSize = ((PhysicsShell*)pObjB)->m_pPowerupObject->meSize;
        if (pObj->meSize > otherSize)
        {
            ((PhysicsShell*)pObjB)->m_pPowerupObject->m_bShouldDestroy = true;
        }
        else if (pObj->meSize == otherSize)
        {
            if (pObj->m_eType == POWER_UP_BANANA)
            {
                pObj->m_bShouldDestroy = true;
            }
            else if (pObj->m_eType == POWER_UP_SPINY_SHELL
                     && ((PhysicsShell*)pObjB)->m_pPowerupObject->m_eType == POWER_UP_SPINY_SHELL)
            {
            }
            else if (pObj->m_eType == POWER_UP_RED_SHELL
                     && ((PhysicsShell*)pObjB)->m_pPowerupObject->m_eType == POWER_UP_RED_SHELL)
            {
            }
            else if (pObj->m_eType == POWER_UP_SPINY_SHELL)
            {
                ((PhysicsShell*)pObjB)->m_pPowerupObject->m_bShouldDestroy = true;
            }
            else if (((PhysicsShell*)pObjB)->m_pPowerupObject->m_eType == POWER_UP_SPINY_SHELL)
            {
                pObj->m_bShouldDestroy = true;
            }
            else
            {
                ((PhysicsShell*)pObjB)->m_pPowerupObject->m_bShouldDestroy = true;
                pObj->m_bShouldDestroy = true;
            }
        }
        else
        {
            pObj->m_bShouldDestroy = true;
        }
        break;
    }
    case 0x15:
    {
        PowerupBase* pOther = ((PhysicsBanana*)pObjB)->m_pPowerupObject;
        if (pOther->m_unk44.m_uPackedTime != 0)
        {
            break;
        }

        ePowerupSize otherSize = pOther->meSize;
        if (pObj->meSize > otherSize)
        {
            pOther->m_bShouldDestroy = true;
        }
        else if (pObj->meSize == otherSize)
        {
            pOther->m_bShouldDestroy = true;
            if (pObj->m_eType != POWER_UP_SPINY_SHELL)
            {
                pObj->m_bShouldDestroy = true;
            }
        }
        else
        {
            pObj->m_bShouldDestroy = true;
        }
        break;
    }
    default:
        if (pObj->m_eType == POWER_UP_BOBOMB
            && pObjB->GetObjectType() == 0x12
            && !((Bobomb*)pObj)->mbIsMine)
        {
            ((Bobomb*)pObj)->mbIsMine = true;
            ((Bobomb*)pObj)->m_unkAC = lbl_806DBDE0;
            pObj->m_v3Velocity = v3Zero;
            pObj->m_pPhysicsObject->SetLinearVelocity(v3Zero);

            EffectsGroup* pEffectsGroup;
            if (pObj->meSize == POWERUPSIZE_SMALL)
            {
                pEffectsGroup = fn_802E7CDC(
                    EmissionManager::Instance(), "bobomb_ground");
            }
            else
            {
                pEffectsGroup = fn_802E7CDC(
                    EmissionManager::Instance(), "bobomb_ground_large");
            }

            EmissionController* pController = fn_802E7FE4(
                EmissionManager::Instance(), pEffectsGroup, 3, true, false);
            pController->SetPosition(pObj->m_v3Position);
            pController->m_uUserData = (u32)pObj;
            Function1<void, EmissionController&> callback(fn_8009F1B8);
            pController->SetUpdateCallback(callback);
        }
        break;
    }

    if (pCharacter != 0)
    {
        if (pCharacter->m_eClassType == GOALIE)
        {
            pObj->m_bShouldDestroy = true;
        }
        else
        {
            pPlayerTarget = (cPlayer*)pCharacter;
            if (((cFielder*)pCharacter)->mbWasHitByPowerupThisFrame)
            {
                return;
            }
            ((cFielder*)pCharacter)->mbWasHitByPowerupThisFrame = true;

            unsigned long soundID = powerupSounds[pObj->m_eType].sndHit;
            if (soundID != 0)
            {
                fn_800EBBFC(0x10, soundID, 0, 0);
            }

            {
                if (pObj->m_eType == POWER_UP_BANANA)
                {
                    CollisionPlayerBananaData* pData = 0;
                    lbl_805716B8.Allocate(pData);
                    pData->pPlayer = (cFielder*)pCharacter;
                    pData->pThrower = pObj->m_pThrower;
                    pData->nThrowerPadID = pObj->m_nThrowerPadID;
                    pData->v3CollisionLocation = v3Pos;
                    fn_801478C4(pData);
                    pObj->m_bShouldDestroy = true;
                }
                else if (pObj->m_eType != POWER_UP_BOBOMB)
                {
                    bool bUnknown = fn_8003877C((cFielder*)pCharacter);
                    if (pObj->m_eType != POWER_UP_FREEZE_SHELL)
                    {
                        if (!bUnknown || pObj->m_eType == POWER_UP_SPINY_SHELL)
                        {
                            CollisionPlayerShellData* pData = 0;
                            lbl_80571668.Allocate(pData);
                            pData->pPlayer = (cFielder*)pCharacter;
                            pData->eSize = (int)pObj->meSize;
                            pData->pThrower = pObj->m_pThrower;
                            pData->nThrowerPadID = (u8)pObj->m_nThrowerPadID;
                            if (pObj->mbExploder)
                            {
                                pData->bIsExploder = true;
                            }
                            else
                            {
                                pData->bIsExploder = false;
                            }
                            pData->v3CollisionLocation = v3Pos;
                            pData->v3CollisionVelocity = pObj->m_v3Velocity;
                            fn_80147A0C(pData);
                        }
                    }
                    else
                    {
                        CollisionPlayerFreezeData* pData = 0;
                        lbl_80571690.Allocate(pData);
                        pData->pPlayer = (cFielder*)pCharacter;
                        pData->eSize = (int)pObj->meSize;
                        pData->pThrower = pObj->m_pThrower;
                        pData->nThrowerPadID = pObj->m_nThrowerPadID;
                        fn_80147B54(pData);
                    }

                    if (!bUnknown)
                    {
                        if (pObj->meSize != POWERUPSIZE_LARGE)
                        {
                            pObj->m_bShouldDestroy = true;
                        }
                        else
                        {
                            pObj->m_pTarget = 0;
                        }
                    }
                }
                else if (pObj->m_eType == POWER_UP_BOBOMB)
                {
                    pObj->m_bShouldDestroy = true;
                }
            }

            bool bInvincible = !fn_8003886C((cFielder*)pCharacter)
                            && (((cFielder*)pCharacter)->muInvincibleStatus & 8) != 0;
            if (bInvincible)
            {
                pObj->m_bShouldDestroy = true;
            }
        }
    }

    if (pPlayerTarget != 0)
    {
        PowerupHitPlayerEventData* pData = 0;
        lbl_805719D8.Allocate(pData);
        pData->Type = pObj->m_eType;
        pData->Thrower = (cPlayer*)pObj->m_pThrower;
        pData->Target = pPlayerTarget;
        fn_8014777C(pData);
    }
}

/**
 * Offset/Address/Size: 0x3460 | 0x8009CAC0 | size: 0xCC
 */
void PowerupBase::fn_8009CAC0(cFielder* pFielder)
{
    unsigned long soundID = powerupSounds[m_eType].sndActivate;
    if (soundID != 0)
    {
        if (this != 0)
        {
            fn_800EBBFC(0x10, soundID, "Powerup", this);
        }
        else
        {
            fn_800EBBFC(0x10, soundID, 0, 0);
        }
    }

    m_v3Position = pFielder->m_v3Position;
    m_pPhysicsObject->SetPosition(
        m_v3Position, PhysicsObject::WORLD_COORDINATES);
    m_v3Velocity = v3Zero;
    m_pPhysicsObject->SetLinearVelocity(v3Zero);
}

/**
 * Offset/Address/Size: 0x352C | 0x8009CB8C | size: 0x330
 */
void PowerupBase::ThrowAt(cFielder* pThrower)
{
    unsigned long soundID = powerupSounds[m_eType].sndActivate;
    if (soundID != 0)
    {
        if (this != 0)
        {
            fn_800EBBFC(0x10, soundID, "Powerup", this);
        }
        else
        {
            fn_800EBBFC(0x10, soundID, 0, 0);
        }
    }

    nlVector3 v3TargetPos;
    nlVector3 v3TargetVel;

    v3TargetPos = pThrower->m_v3Position;
    v3TargetVel = pThrower->m_v3Velocity;

    if (m_pTarget != 0)
    {
        v3TargetPos = m_pTarget->m_v3Position;
        v3TargetVel = m_pTarget->m_v3Velocity;
    }

    float fSpeed = fn_8002CFF0(fn_8003E6E4(pThrower));

    if (gbAlwaysSurround || pThrower->GetGlobalPad() == 0)
    {
        nlVector3 v3Direction;
        nlVec3Set(v3Direction,
            v3TargetPos.x - m_v3Position.x,
            v3TargetPos.y - m_v3Position.y,
            v3TargetPos.z - m_v3Position.z);
        float fInvDistance = nlRecipSqrt(v3Direction.GetLengthSq3D(), true);
        nlVec3Scale(v3Direction, fInvDistance);

        int nNumSolutions;
        float pSolutions[2];
        CalcInterceptXY(m_v3Position, fSpeed, 0.0f, v3TargetPos,
            v3TargetVel, nNumSolutions, pSolutions);

        if (nNumSolutions != 0)
        {
            float t;
            if (nNumSolutions == 2)
            {
                t = (pSolutions[0] < pSolutions[1]) ? pSolutions[0] : pSolutions[1];
            }
            else
            {
                t = pSolutions[0];
            }

            nlVector3 v3BobombVelocity;
            nlVector2 v2TargetPos;
            v2TargetPos.x = v3TargetVel.x * t + v3TargetPos.x;
            v2TargetPos.y = v3TargetVel.y * t + v3TargetPos.y;

            v3BobombVelocity.x = (v2TargetPos.x - m_v3Position.x) / t;
            v3BobombVelocity.y = (v2TargetPos.y - m_v3Position.y) / t;
            v3BobombVelocity.z = 0.0f;

            m_v3Velocity = v3BobombVelocity;
            m_pPhysicsObject->SetLinearVelocity(v3BobombVelocity);
        }
        else
        {
            nlVector3 v3BobombVelocity;
            nlVec3Scale(v3BobombVelocity, v3Direction, fSpeed);
            v3BobombVelocity.z = 0.0f;

            m_v3Velocity = v3BobombVelocity;
            m_pPhysicsObject->SetLinearVelocity(v3BobombVelocity);
        }
    }
    else
    {
        unsigned short aDirection = pThrower->m_aActualFacingDirection;
        if (pThrower->m_pController != 0
            && pThrower->m_pController->GetMovementStickMagnitude() > 0.01f)
        {
            aDirection = pThrower->m_pController->GetMovementStickDirection();
        }

        nlVector3 v3BobombVelocity;
        nlPolarToCartesian(v3BobombVelocity.x, v3BobombVelocity.y, aDirection, 1.0f);
        v3BobombVelocity.z = 0.0f;
        float scaledY = fSpeed * v3BobombVelocity.y;
        float scaledX = fSpeed * v3BobombVelocity.x;
        v3BobombVelocity.x = scaledX;
        v3BobombVelocity.y = scaledY;
        v3BobombVelocity.z = fSpeed * v3BobombVelocity.z;

        m_v3Velocity = v3BobombVelocity;
        m_pPhysicsObject->SetLinearVelocity(v3BobombVelocity);
    }
}

/**
 * Offset/Address/Size: 0x385C | 0x8009CEBC | size: 0x23C
 */
void PowerupBase::fn_8009CEBC(const nlVector3& v3Unidentified)
{
    if (m_unk44.m_uPackedTime == 0)
    {
        if (m_eType != POWER_UP_BANANA)
        {
            nlVector3 v3NewVelocity;
            nlVector3 v3Direction;

            const nlVector3& v3Velocity = m_v3Velocity;
            float fVerticalVelocity = -v3Velocity.z;
            float fDirectionY = m_v3Position.y - v3Unidentified.y;
            float fDirectionX = m_v3Position.x - v3Unidentified.x;
            float fDirectionZ = m_v3Position.z - v3Unidentified.z;
            nlVec3Set(v3Direction, fDirectionX, fDirectionY, fDirectionZ);

            if (nlVec3DotProduct(v3Direction, v3Velocity) < 0.0f)
            {
                float fVelocityProjection = nlVec3DotProduct(
                    v3Velocity, v3Direction);
                float fDirectionLengthSquared = nlVec3LengthSquared(v3Direction);
                float fProjection = fVelocityProjection / fDirectionLengthSquared;
                nlVec3Scale(v3NewVelocity, v3Direction, fProjection);
                nlVec3ScaleAdd(v3NewVelocity, -2.0f,
                    v3NewVelocity, v3Velocity);
            }
            else
            {
                float fDirectionSpeed = nlVec2Length(
                    *(const nlVector2*)&v3Direction);
                float fCurrentSpeed = nlVec2Length(
                    *(const nlVector2*)&v3Velocity);
                nlVec3Scale(v3NewVelocity, v3Direction,
                    fCurrentSpeed / fDirectionSpeed);
            }

            if (fVerticalVelocity > 0.0f)
            {
                v3NewVelocity.z = fVerticalVelocity;
            }

            m_v3Velocity = v3NewVelocity;
            m_pPhysicsObject->SetLinearVelocity(v3NewVelocity);
        }
        else
        {
            nlVector3 v3Direction;
            float fDirectionY = m_v3Position.y - v3Unidentified.y;
            float fDirectionX = m_v3Position.x - v3Unidentified.x;
            float fDirectionZ = m_v3Position.z - v3Unidentified.z;
            nlVec3Set(v3Direction, fDirectionX, fDirectionY, fDirectionZ);
            float fInvDistance = nlRecipSqrt(
                nlVec3LengthSquared(v3Direction), true);
            nlVec2Set(*(nlVector2*)&v3Direction,
                fInvDistance * v3Direction.x,
                fInvDistance * v3Direction.y);
            v3Direction.z = 15.0f;
            nlVec2Set(*(nlVector2*)&v3Direction,
                15.0f * v3Direction.x,
                15.0f * v3Direction.y);

            m_v3Velocity = v3Direction;
            m_pPhysicsObject->SetLinearVelocity(v3Direction);
        }
    }
    else
    {
        m_bShouldDestroy = true;
    }
}

static inline void UnregisterPowerup(unsigned long hashID)
{
    for (int i = 0; i < 25; i++)
    {
        if (hashID == powerupRegistry.registry[i].hashId)
        {
            powerupRegistry.registry[i].hashId = 0;
            return;
        }
    }

    extern void nlBreak();
    nlBreak();
}

/**
 * Offset/Address/Size: 0x3A98 | 0x8009D0F8 | size: 0x408
 */
void PowerupBase::Destroy(bool bSilent)
{
    EmissionManager* pManager;
    EffectsGroup* pExplosionGroup;
    EffectsGroup* pGroundGroup;

    unsigned long soundID = powerupSounds[m_eType].sndInEffect;
    if (soundID != 0)
    {
        fn_800EC12C(soundID, this);
    }
    soundID = powerupSounds[m_eType].sndActivate;
    if (soundID != 0)
    {
        fn_800EC12C(soundID, this);
    }

    bool bUnidentified = m_v3Position.z < -1.0f;

    if (!bSilent && !bUnidentified
        && (m_eType == POWER_UP_BOBOMB || mbExploder))
    {
        if (m_eType != POWER_UP_FREEZE_SHELL)
        {
            pManager = EmissionManager::Instance();
            switch (meSize)
            {
            case POWERUPSIZE_LARGE:
                pExplosionGroup = fn_802E7CDC(
                    pManager, "bobomb_explode_big");
                pGroundGroup = fn_802E7CDC(
                    pManager, "bobomb_explode_ground_big");
                fn_800F0240(0.0f, 0.2f, 5000.0f, 10.0f);
                break;
            case POWERUPSIZE_MEDIUM:
                pExplosionGroup = fn_802E7CDC(
                    pManager, "bobomb_explode_med");
                pGroundGroup = fn_802E7CDC(
                    pManager, "bobomb_explode_ground_med");
                break;
            case POWERUPSIZE_SMALL:
                pExplosionGroup = fn_802E7CDC(
                    pManager, "bobomb_explode_small");
                pGroundGroup = fn_802E7CDC(
                    pManager, "bobomb_explode_ground_small");
                break;
            }

            EmissionController* pControl = fn_802E7FE4(
                pManager, pExplosionGroup, 0, true, false);
            pControl->SetPosition(m_pPhysicsObject->GetPosition());
            if ((m_v3Position.z
                    - ((PhysicsSphere*)m_pPhysicsObject)->GetRadius())
                < 1.0f)
            {
                EmissionController* pControl = fn_802E7FE4(
                    pManager, pGroundGroup, 0, true, false);
                pControl->SetPosition(m_pPhysicsObject->GetPosition());
            }

            soundID = powerupSounds[m_eType].sndExplode;
            if (soundID != 0)
            {
                fn_800EBBFC(0x10, soundID, 0, 0);
            }
        }

        float fExplosionRadius = 0.0f;
        switch (meSize)
        {
        case POWERUPSIZE_SMALL:
            fExplosionRadius = lbl_806DBDCC;
            break;
        case POWERUPSIZE_MEDIUM:
            fExplosionRadius = lbl_806DBDD0;
            break;
        case POWERUPSIZE_LARGE:
            fExplosionRadius = lbl_806DBDD4;
            break;
        }
        fn_8017642C(&m_v3Position, m_pThrower,
            m_eType == POWER_UP_FREEZE_SHELL, m_nThrowerPadID,
            fExplosionRadius);
    }
    else if (!bSilent && m_eType != POWER_UP_FREEZE_SHELL)
    {
        switch (meSize)
        {
        case POWERUPSIZE_LARGE:
            soundID = powerupSounds[m_eType].sndEnd;
            if (soundID != 0)
            {
                fn_800EBBFC(0x10, soundID, 0, 0);
            }
            break;
        case POWERUPSIZE_MEDIUM:
            soundID = powerupSounds[m_eType].sndEnd;
            if (soundID != 0)
            {
                fn_800EBBFC(0x10, soundID, 0, 0);
            }
            break;
        case POWERUPSIZE_SMALL:
            soundID = powerupSounds[m_eType].sndEnd;
            if (soundID != 0)
            {
                fn_800EBBFC(0x10, soundID, 0, 0);
            }
            break;
        }
    }

    g_pPowerups[m_nIndex] = 0;
    m_pTarget = 0;

    if (m_pBlurHandler != 0)
    {
        m_pBlurHandler->Die(bSilent ? 0.0f : 0.5f);
        m_pBlurHandler = 0;
    }

    UnregisterPowerup(m_pDrawableObj->GetHashID());

    delete this;
}

/**
 * Offset/Address/Size: 0x3EA0 | 0x8009D500 | size: 0x17C
 */
void PowerupBase::fn_8009D500()
{
    m_v3PrevPosition = m_v3Position;
    m_pPhysicsObject->GetPosition(&m_v3Position);
    m_pPhysicsObject->GetLinearVelocity(&m_v3Velocity);

    if (m_unk34.m_uPackedTime != 0)
    {
        nlVector3 velocity = m_v3Velocity;
        velocity.x *= lbl_806DBDBC;
        velocity.y *= lbl_806DBDBC;
        m_v3Velocity = velocity;
        m_pPhysicsObject->SetLinearVelocity(velocity);
    }

    u8 bUnidentified = false;
    if (m_pPhysicsObject->GetObjectType() == 0x15)
    {
        bUnidentified = ((PhysicsBanana*)m_pPhysicsObject)->mUnidentified044[1];
    }
    else if (m_pPhysicsObject->GetObjectType() == 0x14)
    {
        bUnidentified = ((PhysicsShell*)m_pPhysicsObject)->mUnidentified044[2];
    }

    float radius = ((PhysicsSphere*)m_pPhysicsObject)->GetRadius();
    if (m_v3Position.z < radius && !bUnidentified)
    {
        m_v3Position.z = ((PhysicsSphere*)m_pPhysicsObject)->GetRadius();
        m_pPhysicsObject->SetPosition(m_v3Position, PhysicsObject::WORLD_COORDINATES);
    }

    if (m_v3Position.z < -10.0f)
    {
        m_bShouldDestroy = true;
    }

    if (m_pBlurHandler != 0)
    {
        m_pBlurHandler->AddViewOrientedPoint(m_v3Position, m_v3Velocity);
    }
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

/**
 * Offset/Address/Size: 0x40EC | 0x8009D74C | size: 0x98
 */
void PowerupBase::fn_8009D74C(float seconds, bool bEnableCollisions)
{
    m_unk44.SetSeconds(seconds);
    m_unk50 = m_v3Velocity;
    m_unk3C.SetSeconds(0.2f);

    if (!bEnableCollisions)
    {
        m_pPhysicsObject->DisableCollisions();
    }
    else
    {
        m_pPhysicsObject->EnableCollisions();
    }

    if (m_pBlurHandler != 0)
    {
        m_pBlurHandler->Die(0.0f);
        m_pBlurHandler = 0;
    }
}

/**
 * Offset/Address/Size: 0x4184 | 0x8009D7E4 | size: 0x2F8
 */
void PowerupBase::UpdateTransform()
{
    nlPolar pDirectionalSpeed;
    float fSpeedNormalized;
    float fActualRadius;
    float fNormalRadius;

    if (m_eType == POWER_UP_BOBOMB)
    {
        if (!((Bobomb*)this)->mbIsMine)
        {
            m_aOrientation += 875;
        }
    }
    else if (m_eType != POWER_UP_BANANA)
    {
        nlCartesianToPolar(pDirectionalSpeed, m_v3Velocity.x, m_v3Velocity.y);
        fSpeedNormalized = NormalizeVal(pDirectionalSpeed.r, 0.0f,
            lbl_8056CF08.m_unk14->fGreenShellSpeed);

        {
            float z = 250.0f;
            m_aOrientation += (int)(1500.0f * fSpeedNormalized + z);
        }
    }

    switch (m_eType)
    {
    case POWER_UP_BANANA:
        switch (meSize)
        {
        case POWERUPSIZE_LARGE:
            fActualRadius = lbl_8056CF08.m_pGameTweaks->fBananaBigRadius;
            break;
        case POWERUPSIZE_MEDIUM:
            fActualRadius = lbl_8056CF08.m_pGameTweaks->fBananaMediumRadius;
            break;
        case POWERUPSIZE_SMALL:
            fActualRadius = lbl_8056CF08.m_pGameTweaks->fBananaSmallRadius;
            break;
        }
        fNormalRadius = lbl_8056CF08.m_pGameTweaks->fBananaSmallRadius;
        break;

    case POWER_UP_BOBOMB:
        switch (meSize)
        {
        case POWERUPSIZE_LARGE:
            fActualRadius = lbl_8056CF08.m_pGameTweaks->fBobombBigRadius;
            break;
        case POWERUPSIZE_MEDIUM:
            fActualRadius = lbl_8056CF08.m_pGameTweaks->fBobombMediumRadius;
            break;
        case POWERUPSIZE_SMALL:
            fActualRadius = lbl_8056CF08.m_pGameTweaks->fBobombSmallRadius;
            break;
        }
        fNormalRadius = lbl_8056CF08.m_pGameTweaks->fBobombSmallRadius;
        break;

    default:
        switch (meSize)
        {
        case POWERUPSIZE_LARGE:
            fActualRadius = lbl_8056CF08.m_pGameTweaks->fShellBigRadius;
            break;
        case POWERUPSIZE_MEDIUM:
            fActualRadius = lbl_8056CF08.m_pGameTweaks->fShellMediumRadius;
            break;
        case POWERUPSIZE_SMALL:
            fActualRadius = lbl_8056CF08.m_pGameTweaks->fShellSmallRadius;
            break;
        }
        fNormalRadius = lbl_8056CF08.m_pGameTweaks->fShellSmallRadius;
        break;
    }

    m_scale = fActualRadius / fNormalRadius;
    ((PhysicsSphere*)m_pPhysicsObject)->SetRadius(fActualRadius);

    if (m_unk4C > 0.0f)
    {
        m_scale = InterpolateRangeClamped(
            0.33f, m_scale, lbl_806DBDC8, 0.0f, m_unk4C);
        ((PhysicsSphere*)m_pPhysicsObject)->SetRadius(m_scale * fNormalRadius);

        if (m_pBlurHandler != 0 && m_scale <= 1.0f)
        {
            m_pBlurHandler->m_fLineWidth = m_scale * m_fBlurWidth;
        }
    }

    if (m_szStreakTexture != 0 && m_pBlurHandler == 0)
    {
        m_pBlurHandler = BlurManager::GetNewHandler(
            m_szStreakTexture, m_scale * m_fBlurWidth, 15, true);
    }
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
void PowerupBase::Init(cFielder* pFielder)
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

inline void PowerupBase::SpeedManagement()
{
    nlPolar aShell;
    nlVector2 v2NewVelocity;
    nlVector3 v3NewVelocity;

    if (mtNoHitTimer.m_uPackedTime == 0 && m_unk44.m_uPackedTime == 0)
    {
        nlCartesianToPolar(aShell, m_v3Velocity.x, m_v3Velocity.y);
        if (aShell.r < 0.0001f)
        {
            m_bShouldDestroy = true;
        }
        else if (aShell.r > 0.0f)
        {
            v2NewVelocity = *(const nlVector2*)&m_v3Velocity;
            f32 recipLen = nlRecipSqrt(
                v2NewVelocity.x * v2NewVelocity.x
                    + v2NewVelocity.y * v2NewVelocity.y,
                true);
            v2NewVelocity.y = recipLen * v2NewVelocity.y;
            v2NewVelocity.x = recipLen * v2NewVelocity.x;
            f32 scaledY = 30.0f * v2NewVelocity.y;
            f32 scaledX = 30.0f * v2NewVelocity.x;
            v2NewVelocity.x = scaledX;
            v2NewVelocity.y = scaledY;
            v3NewVelocity.y = v2NewVelocity.y;
            v3NewVelocity.x = v2NewVelocity.x;
            v3NewVelocity.z = m_v3Velocity.z;
            m_v3Velocity = v3NewVelocity;
            m_pPhysicsObject->SetLinearVelocity(v3NewVelocity);
        }
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
 * Offset/Address/Size: 0x41B0 | 0x8009DC50 | size: 0x2C
 */
void PowerupBase::StopPowerupInEffectSound(ePowerUpType type, PowerupSound powerupSnd,
    void* pParam)
{
    unsigned long soundID = (&powerupSounds[type].sndAcquire)[powerupSnd];
    if (soundID != 0)
    {
        fn_800EC12C(soundID, pParam);
    }
}

/**
 * Offset/Address/Size: 0x4320 | 0x8009DDC0 | size: 0x74
 */
void PowerupBase::PlayPowerupSound(ePowerUpType type, PowerupSound powerupSnd,
    PhysicsObject* pPhysObj, float fVol, void* pParam)
{
    unsigned long soundID = (&powerupSounds[type].sndAcquire)[powerupSnd];
    if (soundID == 0)
    {
        return;
    }

    if (type == POWER_UP_STAR && powerupSnd == PWRUP_SOUND_IN_EFFECT)
    {
        fn_800EBF78(0x12, soundID, "Powerup", pParam, 0);
    }
    else if (pParam != 0)
    {
        fn_800EBBFC(0x10, soundID, "Powerup", pParam);
    }
    else
    {
        fn_800EBBFC(0x10, soundID, 0, 0);
    }
}

/**
 * Offset/Address/Size: 0x41DC | 0x8009DC7C | size: 0xE8
 */
void PowerupBase::PlayPowerupSound(ePowerUpType type, PowerupSound powerupSnd,
    const nlVector3& v3Pos, float fVol, void* pParam)
{
    PhysicsShell dummyShell(1.0f);
    dummyShell.SetPosition(v3Pos, PhysicsObject::WORLD_COORDINATES);
    PlayPowerupSound(type, powerupSnd, &dummyShell, fVol, pParam);
}

/**
 * Offset/Address/Size: 0x4374 | 0x8009DE34 | size: 0x118
 */
GreenShell::~GreenShell()
{
}

/**
 * Offset/Address/Size: 0x448C | 0x8009DF4C | size: 0x134
 */
void GreenShell::Update(float dt)
{
    PowerupBase::Update(dt);

    if (m_unk44.m_uPackedTime != 0)
    {
        return;
    }

    SpeedManagement();

    if (m_bShouldDestroy)
    {
        m_pDrawableObj->m_uObjectFlags &= ~1;
        Destroy(false);
    }
}

/**
 * Offset/Address/Size: 0x45C0 | 0x8009E080 | size: 0xFC
 */
void GreenShell::Destroy(bool bSilent)
{
    bool bUnidentified = m_v3Position.z < 0.0f;
    if (!bSilent && !bUnidentified)
    {
        EmissionManager* pUnk = EmissionManager::Instance();
        EffectsGroup* pEffectsGroup;
        switch (meSize)
        {
        case POWERUPSIZE_LARGE:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "green_shell_explode_big");
            break;
        case POWERUPSIZE_MEDIUM:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "green_shell_explode_med");
            break;
        case POWERUPSIZE_SMALL:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "green_shell_explode");
            break;
        }

        EmissionController* pController = fn_802E7FE4(
            pUnk, pEffectsGroup, 0, true, false);
        pController->SetPosition(m_pPhysicsObject->GetPosition());
    }

    PowerupBase::Destroy(bSilent);
}

/**
 * Offset/Address/Size: 0x4B1C | 0x8009E17C | size: 0x118
 */
RedShell::~RedShell()
{
}

/**
 * Offset/Address/Size: 0x4C34 | 0x8009E294 | size: 0x150
 */
void RedShell::Update(float dt)
{
    PowerupBase::Update(dt);

    if (m_unk44.m_uPackedTime != 0)
    {
        return;
    }

    SpeedManagement();
    SeekTarget();

    if (mtActiveTimer.m_uPackedTime == 0)
    {
        m_pTarget = 0;
    }

    if (m_bShouldDestroy)
    {
        m_pDrawableObj->m_uObjectFlags &= ~1;
        Destroy(false);
    }
}

/**
 * Offset/Address/Size: 0x4D84 | 0x8009E3E4 | size: 0xFC
 */
void RedShell::Destroy(bool bSilent)
{
    bool bUnidentified = m_v3Position.z < 0.0f;
    if (!bSilent && !bUnidentified)
    {
        EmissionManager* pUnk = EmissionManager::Instance();
        EffectsGroup* pEffectsGroup;
        switch (meSize)
        {
        case POWERUPSIZE_LARGE:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "red_shell_explode_big");
            break;
        case POWERUPSIZE_MEDIUM:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "red_shell_explode_med");
            break;
        case POWERUPSIZE_SMALL:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "red_shell_explode");
            break;
        }

        EmissionController* pController = fn_802E7FE4(
            pUnk, pEffectsGroup, 0, true, false);
        pController->SetPosition(m_pPhysicsObject->GetPosition());
    }

    PowerupBase::Destroy(bSilent);
}

/**
 * Offset/Address/Size: 0x4E80 | 0x8009E4E0 | size: 0x1D8
 */
void RedShell::SeekTarget()
{
    float fCurrSpeed;
    nlVector3 v3NewVelocity;

    cFielder* target = m_pTarget;
    if (target == 0)
    {
        unsigned long soundID = powerupSounds[m_eType].sndInEffect;
        if (soundID != 0)
        {
            fn_800EC12C(soundID, this);
        }
        return;
    }

    if (!target->mbTangible || fn_800344B0(target))
    {
        m_pTarget = 0;
        return;
    }

    const nlVector3& targetPos =
        ((cCharacter*)m_pTarget)->m_v3Position;
    nlVector2 v2Delta;
    nlVector2 v2Direction;
    v2Delta.x = targetPos.x - m_v3Position.x;
    v2Delta.y = targetPos.y - m_v3Position.y;

    nlVec2Length(v2Delta);

    float invDist = 1.0f / nlVec2Length(v2Delta);
    nlVec2Set(v2Direction,
        invDist * v2Delta.x, invDist * v2Delta.y);

    float turnRate = lbl_806DBDA4;
    nlVec2Set(v2Delta,
        turnRate * v2Direction.x, turnRate * v2Direction.y);

    float velY = m_v3Velocity.y;
    float velX = m_v3Velocity.x;
    fCurrSpeed = nlGetLength2D(velY, velX);

    nlVector2 v2NewVelocity;
    v2NewVelocity.x = v2Delta.x + m_v3Velocity.x;
    v2NewVelocity.y = v2Delta.y + m_v3Velocity.y;

    float newSpeed = nlVec2Length(v2NewVelocity);
    float invNewSpeed = 1.0f / newSpeed;
    nlVector2 v2NormalizedVelocity;
    v2NormalizedVelocity.y = invNewSpeed * v2NewVelocity.y;
    v2NormalizedVelocity.x = invNewSpeed * v2NewVelocity.x;

    v2NewVelocity.y = fCurrSpeed * v2NormalizedVelocity.y;
    v2NewVelocity.x = fCurrSpeed * v2NormalizedVelocity.x;

    v3NewVelocity.x = v2NewVelocity.x;
    v3NewVelocity.y = v2NewVelocity.y;
    v3NewVelocity.z = m_v3Velocity.z;
    m_v3Velocity = v3NewVelocity;
    m_pPhysicsObject->SetLinearVelocity(v3NewVelocity);
    m_pPhysicsObject->SetLinearVelocity(m_v3Velocity);
}

/**
 * Offset/Address/Size: 0x5058 | 0x8009E6B8 | size: 0x118
 */
Banana::~Banana()
{
}

/**
 * Offset/Address/Size: 0x5170 | 0x8009E7D0 | size: 0x17C
 */
void Banana::ThrowAt(cFielder* pThrower)
{
    nlVector3 v3Unidentified = { 0.0f, 0.0f, 0.0f };
    unsigned short aDirection = pThrower->m_aActualFacingDirection;
    float fUnidentified = pThrower->mUnidentified0A0;
    float fRadius = GetRadius();
    float fUnidentified2 = fn_8002BFA8(
        fn_8003E6E4(pThrower), fUnidentified);

    nlPolarToCartesian(v3Unidentified.x, v3Unidentified.y,
        (unsigned short)(aDirection + 0x8000),
        0.15f + fRadius + fUnidentified2);

    nlVec3Add(v3Unidentified,
        pThrower->m_v3Position, v3Unidentified);

    m_v3Position = v3Unidentified;
    m_pPhysicsObject->SetPosition(
        m_v3Position, PhysicsObject::WORLD_COORDINATES);

    m_v3Velocity = v3Zero;
    m_pPhysicsObject->SetLinearVelocity(v3Zero);

    unsigned long soundID = powerupSounds[m_eType].sndActivate;
    if (soundID != 0)
    {
        if (this != 0)
        {
            fn_800EBBFC(0x10, soundID, "Powerup", this);
        }
        else
        {
            fn_800EBBFC(0x10, soundID, 0, 0);
        }
    }
}

/**
 * Offset/Address/Size: 0x52EC | 0x8009E94C | size: 0xE0
 */
void Banana::Update(float dt)
{
    PowerupBase::Update(dt);

    if (m_unk44.m_uPackedTime != 0)
    {
        return;
    }

    nlVector3 v3Unidentified = m_v3Position;
    float fUnidentified =
        ((PhysicsSphere*)m_pPhysicsObject)->GetRadius();
    if (fn_8002D2C4(&v3Unidentified, false,
            ((PhysicsSphere*)m_pPhysicsObject)->GetRadius()
                - 0.85f * fUnidentified))
    {
        m_bShouldDestroy = true;
    }

    if (mtActiveTimer.m_uPackedTime == 0)
    {
        m_bShouldDestroy = true;
    }

    if (m_bShouldDestroy)
    {
        m_pDrawableObj->m_uObjectFlags &= ~1;
        Destroy(false);
    }
}

/**
 * Offset/Address/Size: 0x53CC | 0x8009EA2C | size: 0xFC
 */
void Banana::Destroy(bool bSilent)
{
    bool bUnidentified = m_v3Position.z < 0.0f;
    if (!bSilent && !bUnidentified)
    {
        EmissionManager* pUnk = EmissionManager::Instance();
        EffectsGroup* pEffectsGroup;
        switch (meSize)
        {
        case POWERUPSIZE_LARGE:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "banana_explode_big");
            break;
        case POWERUPSIZE_MEDIUM:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "banana_explode_med");
            break;
        case POWERUPSIZE_SMALL:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "banana_explode");
            break;
        }

        EmissionController* pController = fn_802E7FE4(
            pUnk, pEffectsGroup, 0, true, false);
        pController->SetPosition(m_pPhysicsObject->GetPosition());
    }

    PowerupBase::Destroy(bSilent);
}

/**
 * Offset/Address/Size: 0x54C8 | 0x8009EB28 | size: 0x118
 */
SpinyShell::~SpinyShell()
{
}

/**
 * Offset/Address/Size: 0x55E0 | 0x8009EC40 | size: 0x134
 */
void SpinyShell::Update(float dt)
{
    PowerupBase::Update(dt);

    if (m_unk44.m_uPackedTime != 0)
    {
        return;
    }

    SpeedManagement();

    if (m_bShouldDestroy)
    {
        m_pDrawableObj->m_uObjectFlags &= ~1;
        Destroy(false);
    }
}

/**
 * Offset/Address/Size: 0x5714 | 0x8009ED74 | size: 0xFC
 */
void SpinyShell::Destroy(bool bSilent)
{
    bool bUnidentified = m_v3Position.z < 0.0f;
    if (!bSilent && !bUnidentified)
    {
        EmissionManager* pUnk = EmissionManager::Instance();
        EffectsGroup* pEffectsGroup;
        switch (meSize)
        {
        case POWERUPSIZE_LARGE:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "spiny_shell_explode_big");
            break;
        case POWERUPSIZE_MEDIUM:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "spiny_shell_explode_med");
            break;
        case POWERUPSIZE_SMALL:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "spiny_shell_explode");
            break;
        }

        EmissionController* pController = fn_802E7FE4(
            pUnk, pEffectsGroup, 0, true, false);
        pController->SetPosition(m_pPhysicsObject->GetPosition());
    }

    PowerupBase::Destroy(bSilent);
}

/**
 * Offset/Address/Size: 0x5810 | 0x8009EE70 | size: 0x118
 */
FreezeShell::~FreezeShell()
{
}

/**
 * Offset/Address/Size: 0x5928 | 0x8009EF88 | size: 0x134
 */
void FreezeShell::Update(float fDeltaT)
{
    PowerupBase::Update(fDeltaT);

    if (m_unk44.m_uPackedTime != 0)
    {
        return;
    }

    SpeedManagement();

    if (m_bShouldDestroy)
    {
        m_pDrawableObj->m_uObjectFlags &= ~1;
        Destroy(false);
    }
}

/**
 * Offset/Address/Size: 0x5A5C | 0x8009F0BC | size: 0xFC
 */
void FreezeShell::Destroy(bool bSilent)
{
    bool bUnidentified = m_v3Position.z < 0.0f;
    if (!bSilent && !bUnidentified)
    {
        EmissionManager* pUnk = EmissionManager::Instance();
        EffectsGroup* pEffectsGroup;
        switch (meSize)
        {
        case POWERUPSIZE_LARGE:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "freeze_shell_explode_big");
            break;
        case POWERUPSIZE_MEDIUM:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "freeze_shell_explode_med");
            break;
        case POWERUPSIZE_SMALL:
            pEffectsGroup = fn_802E7CDC(
                pUnk, "freeze_shell_explode");
            break;
        }

        EmissionController* pController = fn_802E7FE4(
            pUnk, pEffectsGroup, 0, true, false);
        pController->SetPosition(m_pPhysicsObject->GetPosition());
    }

    PowerupBase::Destroy(bSilent);
}

/**
 * Offset/Address/Size: 0x5B58 | 0x8009F1B8 | size: 0x60
 */
extern "C" void fn_8009F1B8(EmissionController& controller)
{
    if (lbl_806E0C94 == 0 || lbl_806E0C94->m_eGameState == GS_GAMEPLAY)
    {
        return;
    }

    controller.SetPosition(
        ((PowerupBase*)controller.m_uUserData)->m_v3Position);
    controller.SetVelocity(v3Zero);
}

/**
 * Offset/Address/Size: 0x5BB8 | 0x8009F218 | size: 0x118
 */
Bobomb::~Bobomb()
{
}

/**
 * Offset/Address/Size: 0x5CD0 | 0x8009F330 | size: 0x124
 */
void Bobomb::Update(float dt)
{
    PowerupBase::Update(dt);

    if (m_unk44.m_uPackedTime != 0)
    {
        return;
    }

    if (mbIsMine)
    {
        nlVector3 pos = m_v3Position;
        pos.z = 0.0f;
        m_v3Position = pos;
        m_pPhysicsObject->SetPosition(m_v3Position, PhysicsObject::WORLD_COORDINATES);

        m_v3Velocity = v3Zero;
        m_pPhysicsObject->SetLinearVelocity(v3Zero);

        m_unkAC -= dt;
        if (m_unkAC < 0.0f)
        {
            m_bShouldDestroy = true;
        }
    }

    if (mtActiveTimer.m_uPackedTime == 0)
    {
        m_bShouldDestroy = true;
    }

    if (m_bShouldDestroy)
    {
        m_pDrawableObj->m_uObjectFlags &= ~1;
        Destroy(false);
    }
}

/**
 * Offset/Address/Size: 0x5DF4 | 0x8009F454 | size: 0x3AC
 */
void Bobomb::fn_8009F454(PowerupBase*, int nThrowOrder)
{
    cFielder* pTarget = m_pThrower;
    if (lbl_806DBDD8 && m_pTarget != 0)
    {
        pTarget = m_pTarget;
    }

    float t = (float)nThrowOrder * lbl_806DBDAC + lbl_806DBDA8;
    nlVector3 v3TargetPos = pTarget->m_v3Position;
    nlVector3 v3TargetVel = pTarget->m_v3Velocity;

    if (nlVec3Length(v3TargetVel) < lbl_806DBDDC)
    {
        if (nlVec3Length(v3TargetVel) < 0.01f)
        {
            nlPolarToCartesian(v3TargetVel.x, v3TargetVel.y, pTarget->m_aActualFacingDirection, lbl_806DBDDC);
            v3TargetVel.z = 0.0f;
        }
        else
        {
            float invLength = nlRecipSqrt(v3TargetVel.GetLengthSq3D(), true);
            v3TargetVel.x *= invLength;
            v3TargetVel.y *= invLength;
            v3TargetVel.z *= invLength;
            nlVec3Scale(v3TargetVel, v3TargetVel, lbl_806DBDDC);
        }
    }

    nlVec3ScaleAdd(v3TargetPos, t, v3TargetVel, v3TargetPos);
    float height = -(t * t * (0.5f * m_pPhysicsObject->m_gravity));
    v3TargetPos.z = height + -lbl_806DBDB0 * t;

    float radius = 0.0f;
    switch (meSize)
    {
    case POWERUPSIZE_SMALL:
        radius = lbl_806DBDCC;
        break;
    case POWERUPSIZE_MEDIUM:
        radius = lbl_806DBDD0;
        break;
    case POWERUPSIZE_LARGE:
        radius = lbl_806DBDD4;
        break;
    }

    fn_8002D2C4(&v3TargetPos, true, radius);

    m_v3Position = v3TargetPos;
    m_pPhysicsObject->SetPosition(m_v3Position, PhysicsObject::WORLD_COORDINATES);

    nlVector3 v3BobombVelocity = v3Zero;
    v3BobombVelocity.z = lbl_806DBDB0;
    m_v3Velocity = v3BobombVelocity;
    m_pPhysicsObject->SetLinearVelocity(v3BobombVelocity);

    EffectsGroup* pEffectsGroup = fn_802E7CDC(
        EmissionManager::Instance(), "bobomb_tick");
    EmissionController* pController = fn_802E7FE4(
        EmissionManager::Instance(), pEffectsGroup, 3, true, false);
    nlVector3 pos = m_pPhysicsObject->GetPosition();
    pos.z += ((PhysicsSphere*)m_pPhysicsObject)->GetRadius();
    pController->SetPosition(pos);
    pController->m_uUserData = (u32)this;
    pController->SetUpdateCallback(
        Function1<void, EmissionController&>(fn_8009F1B8));

    unsigned long soundID = powerupSounds[m_eType].sndActivate;
    if (soundID != 0)
    {
        if (this != 0)
        {
            fn_800EBBFC(0x10, soundID, "Powerup", this);
        }
        else
        {
            fn_800EBBFC(0x10, soundID, 0, 0);
        }
    }
}

/**
 * Offset/Address/Size: 0x61A0 | 0x8009F800 | size: 0xC
 */
void Bobomb::ThrowAt(cFielder* pThrower)
{
    fn_8009F454(0, 0);
}

/**
 * Offset/Address/Size: 0x61AC | 0x8009F80C | size: 0x94
 */
void Bobomb::Destroy(bool bSilent)
{
    EffectsGroup* pEffectsGroup = fn_802E7CDC(
        EmissionManager::Instance(), "bobomb_tick");
    EmissionManager::Instance()->Destroy((unsigned long)this, pEffectsGroup);

    if (gBobombAnticipationVoiceID != -1)
    {
        gBobombAnticipationVoiceID = -1;
    }

    if (pMovementEmitter != 0)
    {
        pMovementEmitter = 0;
    }

    PowerupBase::Destroy(bSilent);
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
