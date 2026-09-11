#include "Game/AI/DesireSuperPower.h"

#include "Game/AI/AIPad.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/Fielder.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DebugWriteCache.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Effects/EmitterCallbacks.h"
#include "Game/Event.h"
#include "Game/Game.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Render/NPCManager.h"
#include "Game/Sys/audio.h"
#include "Game/Team.h"
#include "NL/nlAVLTree.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"
#include "unclassified/tu_801B535C.h"
#include <stdlib.h>
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/UnidentifiedRegistryPools.h"

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry;

extern "C" UnidentifiedEventRegistry* g_pEventRegistry;
extern "C" cGame* g_pGame;
extern "C" void fn_800C2C18(UnidentifiedDesireUpdate*, int);
extern "C" void fn_800C93A4(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CA07C(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CA57C(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CADBC(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CB2BC(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CB7A8(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CBF64(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CC720(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CCC0C(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800CD61C(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800D01A0(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800D0EAC(
    DesireSuperPower*, UnidentifiedDesireUpdate*, float);
extern "C" void fn_800D1140(void*);
extern "C" void fn_800D12E8(void*);
extern "C" eCharacterClass fn_800D1440(const cCharacter*);
extern "C" unsigned short fn_800D1448(const cCharacter*);
extern "C" const nlVector3* fn_800D1450(const cCharacter*);
extern "C" bool fn_800D1458(const cGame*);
extern "C" void fn_8002E52C(cFielder*);
extern "C" void fn_800395C0(cFielder*);
extern "C" void fn_80316968(shdStateMachine*);
extern "C" void fn_80038158(cFielder*, int);
extern "C" void fn_801B98A0(cFielder*);
extern "C" void fn_801BB0DC(cFielder*);
extern "C" void fn_801BC094(cFielder*);
extern "C" void fn_801B881C(cFielder*);
extern "C" void fn_80039CF0(cFielder*, int);
extern "C" void fn_801A0C58(cFielder*);
extern "C" EffectsGroup* fn_802E7CDC(EmissionManager*, const char*);
extern "C" void fn_802E83C4(EmissionManager*, EffectsGroup*);
extern "C" EmissionController* fn_802E7FE4(
    EmissionManager*, EffectsGroup*, int, bool, bool);
extern "C" void fn_80139D1C(int, void*);
extern "C" void fn_800A6968(cTeam*);
extern "C" void fn_803198F4();
extern "C" void fn_802F4E84(unsigned long*, int, int);
extern "C" const nlVector2 lbl_804DC348[6];
extern "C" const nlVector2 lbl_804DC378[2];
extern FuzzyVariant fvNotSet;
extern const nlVector3 lbl_804DC338;
extern bool lbl_806DC2F8;
extern bool lbl_806DC314;
static unsigned short sDesireSuperPowerType = 0xFFFF;

static inline void UnidentifiedRegisterEventCallback(
    const char* name, void (*callback)(void*))
{
    Function<void*> function(callback);
    unsigned int hash = HashEventName(name, -1);
    UnidentifiedEventBase** foundEvent = 0;
    g_pEventRegistry->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event;
    if (foundEvent != 0)
    {
        event = *foundEvent;
    }
    else
    {
        event = 0;
    }
    ((UnidentifiedTypedEvent<void>*)event)->Add(function, 0, -1);
}

/**
 * Offset/Address/Size: 0x0 | 0x800C86FC | size: 0x60
 */
DesireSuperPower::DesireSuperPower()
    : Desire(23, UnidentifiedStateTransition(lbl_806E20B8))
    , mpDKShockAvoidable(0)
    , mpTarget(0)
{
}

/**
 * Offset/Address/Size: 0x60 | 0x800C875C | size: 0x274
 */
void DesireSuperPower::UnidentifiedSetContext(
    UnidentifiedScriptMachine* context)
{
    Desire::UnidentifiedSetContext(context);

    if (mUnidentifiedFielder->mUnidentified024.m_eCharacterClass == YOSHI)
    {
        UnidentifiedRegisterEventCallback(
            "CollisionPatchGround", fn_800D1140);
        UnidentifiedRegisterEventCallback(
            "CollisionPatchPlayer", fn_800D1140);
        UnidentifiedRegisterEventCallback(
            "CollisionPatchWall", fn_800D12E8);
    }
}

/**
 * Offset/Address/Size: 0x87C | 0x800C8F78 | size: 0x174
 */
void DesireSuperPower::Update(
    UnidentifiedDesireUpdate* update, float fDeltaT)
{
    if (!fn_800D1458(g_pGame))
    {
        fn_800C2C18(update, 1);
        return;
    }

    switch (fn_800D1440(mUnidentifiedFielder))
    {
    case BIRDO:
        fn_800CBF64(this, update, fDeltaT);
        break;
    case DAISY:
        fn_800C93A4(this, update, fDeltaT);
        break;
    case DONKEYKONG:
        fn_800CA57C(this, update, fDeltaT);
        break;
    case HAMMERBROS:
        fn_800CB2BC(this, update, fDeltaT);
        break;
    case KOOPA:
        fn_800CB7A8(this, update, fDeltaT);
        break;
    case LUIGI:
        fn_800CC720(this, update, fDeltaT);
        break;
    case MARIO:
        fn_800CD61C(this, update, fDeltaT);
        break;
    case PEACH:
        fn_800D01A0(this, update, fDeltaT);
        break;
    case TOAD:
        fn_800D0EAC(this, update, fDeltaT);
        break;
    case WALUIGI:
        fn_800CA07C(this, update, fDeltaT);
        break;
    case WARIO:
        fn_800CADBC(this, update, fDeltaT);
        break;
    case YOSHI:
        fn_800CCC0C(this, update, fDeltaT);
        break;
    }
}

/**
 * Offset/Address/Size: 0x9F0 | 0x800C90EC | size: 0x2B8
 */
void DesireSuperPower::UnidentifiedCleanup()
{
    fn_800A6968(mUnidentifiedFielder->m_pTeam);
    fn_803198F4();

    switch (mUnidentifiedFielder->mUnidentified024.m_eCharacterClass)
    {
    case DAISY:
        mUnidentifiedFielder->fn_80050008();
        mUnidentifiedFielder->m_pTeam->ClearCurrentPowerUp();
        fn_801B98A0(mUnidentifiedFielder);
        break;
    case WALUIGI:
        fn_80038158(mUnidentifiedFielder, 0);
        {
            EffectsGroup* group = fn_802E7CDC(
                EmissionManager::Instance(), "bowserjr_shriek_mouth");
            if (group != 0)
            {
                fn_802E83C4(EmissionManager::Instance(), group);
            }
        }
        break;
    case DONKEYKONG:
        fn_80038158(mUnidentifiedFielder, 0);
        break;
    case WARIO:
        mUnidentifiedFielder->mUnidentified181 = false;
        mUnidentifiedFielder->mUnidentified182 = false;
        fn_80038158(mUnidentifiedFielder, 0);
        break;
    case HAMMERBROS:
        fn_801BB0DC(mUnidentifiedFielder);
        fn_80038158(mUnidentifiedFielder, 0);
        delete (AvoidablePoint*)mpDKShockAvoidable;
        mpDKShockAvoidable = 0;
        break;
    case KOOPA:
        mUnidentifiedFielder->m_pTweaks
            = mUnidentifiedFielder->mUnidentified32C;
        mUnidentifiedFielder->fn_8001EE74(1.0f, 0.25f, 1.0f);
        fn_801BC094(mUnidentifiedFielder);
        break;
    case BIRDO:
        mUnidentifiedFielder->m_pTweaks
            = mUnidentifiedFielder->mUnidentified32C;
        mUnidentifiedFielder->fn_8001EE74(1.0f, 0.25f, 1.0f);
        fn_801BC094(mUnidentifiedFielder);
        break;
    case LUIGI:
        mUnidentifiedFielder->fn_8004FA34();
        fn_801A0C58((cFielder*)0);
        break;
    case YOSHI:
        fn_80038158(mUnidentifiedFielder, 0);
        mUnidentifiedFielder->mUnidentified404 = 0.0f;
        mUnidentifiedFielder->mUnidentified408 = 0.0f;
        mUnidentifiedFielder->fn_80050008();
        mUnidentifiedFielder->m_pTeam->ClearCurrentPowerUp();
        if (mUnidentifiedFielder->m_eAnimID == 104)
        {
            mUnidentifiedFielder->EndDesire();
            mUnidentifiedFielder->StartRunning();
        }
        break;
    case MARIO:
        if (mUnidentifiedFielder->fn_8002E060() == 12)
        {
            mUnidentifiedFielder->EndDesire();
        }
        mUnidentifiedFielder->fn_80050008();
        mUnidentifiedFielder->m_pTeam->ClearCurrentPowerUp();
        break;
    case PEACH:
        mUnidentifiedFielder->fn_80050008();
        mUnidentifiedFielder->m_pTeam->ClearCurrentPowerUp();
        if (mUnidentifiedFielder->m_eAnimID == 104)
        {
            mUnidentifiedFielder->EndDesire();
            mUnidentifiedFielder->StartRunning();
        }
        break;
    case TOAD:
        mUnidentifiedFielder->m_pTweaks
            = mUnidentifiedFielder->mUnidentified32C;
        fn_80039CF0(mUnidentifiedFielder, 0);
        mUnidentifiedFielder->bYoshiInWindup = false;
        fn_801B881C(mUnidentifiedFielder);
        gNPCManager->mUnidentified024->fn_801B5DD0();
        break;
    }

    unsigned long sound = PowerupBase::GetSoundType(
        (ePowerUpType)mUnidentifiedFielder->mUnidentified11C->unknown_0x14,
        PowerupBase::PWRUP_SOUND_ACTIVATE);
    StopCaptainPowerupStream(sound, mUnidentifiedFielder);
    if ((mUnidentifiedFielder->mUnidentified024.m_eCharacterClass == BIRDO)
        || (mUnidentifiedFielder->mUnidentified024.m_eCharacterClass == KOOPA))
    {
        ResumeSuddenDeathMusic();
        unsigned long hash = nlStringLowerHash("MarioPowerup");
        fn_802F4E84(&hash, 1, 0);
    }
}

/**
 * Offset/Address/Size: 0x8D44 | 0x800D1440 | size: 0x8
 */
extern "C" eCharacterClass fn_800D1440(const cCharacter* character)
{
    return character->mUnidentified024.m_eCharacterClass;
}

/**
 * Offset/Address/Size: 0x8D4C | 0x800D1448 | size: 0x8
 */
extern "C" unsigned short fn_800D1448(const cCharacter* character)
{
    return character->mUnidentified024.m_aActualFacingDirection;
}

/**
 * Offset/Address/Size: 0x8D54 | 0x800D1450 | size: 0x8
 */
extern "C" const nlVector3* fn_800D1450(const cCharacter* character)
{
    return &character->mUnidentified024.m_v3Position;
}

/**
 * Offset/Address/Size: 0x8D5C | 0x800D1458 | size: 0x20
 */
extern "C" bool fn_800D1458(const cGame* game)
{
    return game->m_eGameState == 5 || game->m_eGameState == 6;
}

/**
 * Offset/Address/Size: 0x9540 | 0x800D1C3C | size: 0x10
 */
void nlVector3::Set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

/**
 * Offset/Address/Size: 0x9550 | 0x800D1C4C | size: 0x34
 */
extern "C" nlVector3* fn_800D1C4C(
    nlVector3* result, const nlVector3* first,
    const nlVector3* second)
{
    nlVec3Sub(*result, *first, *second);
    return result;
}

/**
 * Offset/Address/Size: 0x9584 | 0x800D1C80 | size: 0x4C
 */
extern "C" float fn_800D1C80(
    const nlVector2* first, const nlVector2* second)
{
    nlVector2 delta = {
        first->x - second->x,
        first->y - second->y,
    };
    return nlVec2Length(delta);
}

/**
 * Offset/Address/Size: 0x9538 | 0x800D1C34 | size: 0x8
 */
extern "C" bool fn_800D1C34(const cFielder* fielder)
{
    return fielder->mUnidentified3DC;
}

/**
 * Offset/Address/Size: 0x51E8 | 0x800CD8E4 | size: 0x14
 */
extern "C" void fn_800CD8E4(int* dst, const int* src)
{
    dst[0] = src[0];
    dst[1] = src[1];
}

/**
 * Offset/Address/Size: 0x9640 | 0x800D1D3C | size: 0x8
 */
extern "C" UnidentifiedVariantCollection* fn_800D1D3C(
    shdStateMachine* stateMachine)
{
    return &stateMachine->mUnidentified01C;
}

/**
 * Offset/Address/Size: 0x9648 | 0x800D1D44 | size: 0xC
 */
extern "C" float fn_800D1D44(const cCharacter* character)
{
    return character->mUnidentified024.m_fDesiredMovementScale;
}

/**
 * Offset/Address/Size: 0x9650 | 0x800D1D4C | size: 0x8
 */
extern "C" float fn_800D1D4C(const cCharacter* character)
{
    return character->mUnidentified024.m_fDesiredPlayerScale;
}

/**
 * Offset/Address/Size: 0x86B4 | 0x800D0DB0 | size: 0xFC
 */
extern "C" bool fn_800D0DB0(DesireSuperPower* self, void*)
{
    gNPCManager->mUnidentified024->fn_801B5858(self->mUnidentifiedFielder);
    self->mUnidentifiedFielder->m_pTweaks
        = self->mUnidentifiedFielder->mUnidentified328;
    fn_8002E52C(self->mUnidentifiedFielder);
    if (self->mUnidentifiedFielder->m_tFireTimer.m_uPackedTime != 0)
    {
        self->mUnidentifiedFielder->fn_8009750C();
        self->mUnidentifiedFielder->EndAction();
    }
    self->mUnidentifiedFielder->bYoshiInWindup
        = (self->mUnidentifiedFielder->m_eActionState
            == ACTION_UNKNOWN_30);
    if ((self->mUnidentifiedFielder->fn_8002E060() == 21)
        || (self->mUnidentifiedFielder->fn_8002E060() == 19)
        || (self->mUnidentifiedFielder->fn_8002E060() == 18)
        || (self->mUnidentifiedFielder->fn_8002E060() == 9))
    {
        self->mUnidentifiedFielder->EndDesire();
        self->mUnidentifiedFielder->StartRunning();
    }
    else if (self->mUnidentifiedFielder->m_eActionState
        == ACTION_UNKNOWN_30)
    {
        self->mUnidentifiedFielder->StartRunning();
    }
    fn_800395C0(self->mUnidentifiedFielder);
    self->mUnidentified078 = 5.5f;
    return true;
}

/**
 * Offset/Address/Size: 0x1850 | 0x800C9F4C | size: 0x130
 */
extern "C" bool fn_800C9F4C(DesireSuperPower* self, void*)
{
    if (self->mUnidentifiedFielder->GetGlobalPad() != 0)
    {
        if (self->mUnidentifiedFielder->m_pController
                ->GetMovementStickMagnitude() > 0.01f)
        {
            self->mUnidentifiedFielder->m_pController
                ->GetMovementStickDirection();
        }
    }
    short dir = 0;
    cFielder* target = FindPowerupTarget(
        self->mUnidentifiedFielder, (ePowerUpType)-1);
    self->mpTarget = target;
    if ((target != 0) && (lbl_806DC2F8 != 0))
    {
        dir = self->mUnidentifiedFielder->GetFacingDeltaToPosition(
            target->mUnidentified024.m_v3Position);
    }
    self->mUnidentifiedFielder->InitDesire(
        (eFielderDesireState)21, 0.5f, -1.0f, fvNotSet, fvNotSet);
    self->mUnidentifiedFielder->SetAction((eFielderActionState)29);
    self->mUnidentifiedFielder->muInvincibleStatus |= 1;
    self->mUnidentifiedFielder->SetAnimState(104, true, 0.2f, false, false);
    self->mUnidentifiedFielder->InitMovementFromAnim(
        dir, lbl_804DC338, 0.15f, false);
    self->mUnidentified078 = 1234567.0f;
    return self->mUnidentifiedFielder->m_eActionState
        == (eFielderActionState)29;
}

/**
 * Offset/Address/Size: 0x1678 | 0x800C9D74 | size: 0x40
 */
extern "C" void fn_800C9D74(DesireSuperPower* self, int param)
{
    if (param != 0)
    {
        self->mUnidentifiedFielder->fn_8004FF40();
    }
    fn_80316968(self);
}

/**
 * Offset/Address/Size: 0x2590 | 0x800CAC8C | size: 0x130
 */
extern "C" bool fn_800CAC8C(DesireSuperPower* self, void*)
{
    if (self->mUnidentifiedFielder->GetGlobalPad() != 0)
    {
        if (self->mUnidentifiedFielder->m_pController
                ->GetMovementStickMagnitude() > 0.01f)
        {
            self->mUnidentifiedFielder->m_pController
                ->GetMovementStickDirection();
        }
    }
    short dir = 0;
    cFielder* target = FindPowerupTarget(
        self->mUnidentifiedFielder, (ePowerUpType)-1);
    self->mpTarget = target;
    if ((target != 0) && (lbl_806DC314 != 0))
    {
        dir = self->mUnidentifiedFielder->GetFacingDeltaToPosition(
            target->mUnidentified024.m_v3Position);
    }
    self->mUnidentifiedFielder->InitDesire(
        (eFielderDesireState)21, 0.5f, -1.0f, fvNotSet, fvNotSet);
    self->mUnidentifiedFielder->SetAction((eFielderActionState)29);
    self->mUnidentifiedFielder->muInvincibleStatus |= 1;
    self->mUnidentifiedFielder->SetAnimState(104, true, 0.2f, false, false);
    self->mUnidentifiedFielder->InitMovementFromAnim(
        dir, lbl_804DC338, 0.15f, false);
    self->mUnidentified078 = 1234567.0f;
    return self->mUnidentifiedFielder->m_eActionState
        == (eFielderActionState)29;
}

/**
 * Offset/Address/Size: 0x16B8 | 0x800C9DB4 | size: 0x198
 */
extern "C" void fn_800C9DB4(DesireSuperPower* self)
{
    fn_80038158(self->mUnidentifiedFielder, 0);
    float x;
    float y;
    nlPolarToCartesian(x, y,
        self->mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection, 1.0f);
    x *= 18.5f;
    y *= 18.5f;
    nlVector3 joint = self->mUnidentifiedFielder->GetJointPosition(
        self->mUnidentifiedFielder->m_nHeadJointIndex);
    nlVector3 pos;
    nlVector3 vel;
    pos.x = joint.x + x;
    pos.y = joint.y + y;
    pos.z = joint.z;
    vel.x = x;
    vel.y = y;
    vel.z = 0.0f;
    PhysicsPatch* patch = lbl_806E12C8->fn_801743A8(7,
        self->mUnidentifiedFielder, pos, vel, 0.25f, 5.0f, 0.7f);
    patch->fn_80173B08(1.0f);
    fn_80139D1C(1, self->mUnidentifiedFielder->GetGlobalPad());
    EffectsGroup* group = fn_802E7CDC(
        EmissionManager::Instance(), "bowserjr_shriek_mouth");
    if (group != 0)
    {
        EmissionController* controller = fn_802E7FE4(
            EmissionManager::Instance(), group, 3, true, false);
        controller->m_uUserData = (u32)self->mUnidentifiedFielder;
        controller->SetPosition(
            self->mUnidentifiedFielder->mUnidentified024.m_v3Position);
        controller->SetVelocity(
            self->mUnidentifiedFielder->mUnidentified024.m_v3Velocity);
        controller->SetUpdateCallback(
            Function1<void, EmissionController&>(
                UpdateEmitterFromCharacterForward));
    }
}

/**
 * Offset/Address/Size: 0x95D0 | 0x800D1CCC | size: 0x38
 */
extern "C" unsigned short fn_800D1CCC(float y, float x)
{
    return (unsigned short)(int)(10430.378f * nlATan2f(y, x));
}

/**
 * Offset/Address/Size: 0x9608 | 0x800D1D04 | size: 0xC
 */
extern "C" short fn_800D1D04(
    unsigned short first, unsigned short second)
{
    return nlAngleDiff(first, second);
}

/**
 * Offset/Address/Size: 0x9614 | 0x800D1D10 | size: 0x14
 */
extern "C" unsigned short fn_800D1D10(short angle)
{
    return angle < 0 ? -angle : angle;
}

/**
 * Offset/Address/Size: 0x9628 | 0x800D1D24 | size: 0x10
 */
extern "C" int fn_800D1D24(int value)
{
    return value < 0 ? -value : value;
}

/**
 * Offset/Address/Size: 0x9638 | 0x800D1D34 | size: 0x8
 */
extern "C" eVariantType fn_800D1D34(const Variant* value)
{
    return value->mType;
}

/**
 * Offset/Address/Size: 0x9658 | 0x800D1D54 | size: 0x110
 */
void DesireSuperPower::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field
        = cache->BeginType("DesireSuperPower");
    cache->AddField(22, gDebugFieldTypes[22].size,
        0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition,
        "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition,
        "mThinkTimer");
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&mpDKShockAvoidable - (u8*)&mvDesiredPosition,
        "mpDKShockAvoidable");
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&mpTarget - (u8*)&mvDesiredPosition, "mpTarget");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x9768 | 0x800D1E64 | size: 0xC4
 */
void DesireSuperPower::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireSuperPowerType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireSuperPowerType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = cache->WriteData(sDesireSuperPowerType,
        (u8*)this + offset, sizeof(DesireSuperPower) - offset);
    if (data != NULL)
    {
        DesireSuperPower* copy
            = (DesireSuperPower*)((u8*)data - offset);
        *(int*)&copy->mpDKShockAvoidable = -1;
        cFielder* target = mpTarget;
        *(int*)&copy->mpTarget
            = target == NULL ? -1 : target->mUnidentified120;
        cache->ChecksumData(sDesireSuperPowerType, data, context);
    }
}

/**
 * Offset/Address/Size: 0x982C | 0x800D1F28 | size: 0x5C
 */
DesireSuperPower::~DesireSuperPower()
{
}
