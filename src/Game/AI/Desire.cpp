#include "Game/AI/Desire.h"
#include "Game/Sys/debug.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FielderInput.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/TeamPlayMachine.h"
#include "Game/AI/Variant.h"
#include <stddef.h>
#include "Game/AI/SpaceSearch.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/Field.h"
#include "Game/Goalie.h"
#include "Game/MathHelpers.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "NL/nlMemory.h"
#include "Game/DebugWriteCache.h"

#include "Game/UnidentifiedStaticStorage.h"

extern "C" void fn_80098098(cFielder*);
extern "C" bool fn_800381B4(cFielder*, nlVector3*);
extern "C" float fn_8002E1B0(cFielder*);
extern "C" float fn_80039574(cFielder*);
extern bool lbl_806E0E20;
extern float lbl_806DC058;

float lbl_806DC04C = 60.0f;
float lbl_806DC050 = 0.3f;
extern float lbl_806DC054;
extern unsigned short lbl_806DC05C;
extern unsigned short lbl_806DC060;
extern unsigned short lbl_806DC06C;
extern unsigned short lbl_806DC06A;
extern unsigned short lbl_806DC068;
extern unsigned short lbl_806DC094;
extern unsigned short lbl_806DC0A0;
extern unsigned short lbl_806DC070;
extern unsigned short lbl_806DC06E;

extern "C" int fn_800B045C()
{
    return 6;
}

extern nlVector3* lbl_8056DA30[6];

extern "C" nlVector3* fn_800B0464(int index)
{
    return lbl_8056DA30[index];
}

extern int lbl_8056DA48[6];

extern "C" int fn_800B0478(int index)
{
    return lbl_8056DA48[index];
}

extern nlVector3* lbl_8056DA00[6];

extern "C" nlVector3* fn_800B048C(int index)
{
    return lbl_8056DA00[index];
}

extern int lbl_8056DA18[6];

extern "C" int fn_800B04A0(int index)
{
    return lbl_8056DA18[index];
}

Desire::Desire(int state, const UnidentifiedStateTransition& transition)
    : shdStateMachine(state, transition)
    , mThinkTimer()
{
    m_pFielder = 0;
    mvDesiredPosition.x = 0.0f;
    mvDesiredPosition.y = 0.0f;
    mvDesiredPosition.z = 0.0f;
    mTurboRequest = 0;
    mUnidentified080 = 0.33f;
    mUnidentified084 = 1.0f;
}

void Desire::UnidentifiedSetContext(UnidentifiedScriptMachine* context)
{
    shdStateMachine::UnidentifiedSetContext(context);
    if (context != 0)
    {
        m_pFielder
            = (cFielder*)context->mUnidentified064->mData.pointer;
    }
    else
    {
        m_pFielder = 0;
    }
}

bool Desire::UnidentifiedInitialize(void*)
{
    return true;
}

bool Desire::UnidentifiedReinitialize(void* context)
{
    UnidentifiedCleanup();
    mUnidentifiedTimer.SetSeconds(lbl_806DC054);
    return UnidentifiedInitialize(context);
}

bool DesireFinishAction::UnidentifiedInitialize(void*)
{
    mUnidentified078 = lbl_806DC04C;
    return true;
}

void DesireFinishAction::Update(DesireUpdate* update, float)
{
    if (update->mData.i == 2)
    {
        tDebugPrintManager::Print(DC_AI,
            "** WARNING! DesireFinishAction has expired after %f seconds, probably a bug!\n",
            mUnidentifiedTimer.GetSeconds());
    }
    fn_80098098(m_pFielder);
}

bool DesireWait::UnidentifiedInitialize(void*)
{
    mUnidentified078 = lbl_806DC050;
    return true;
}

void DesireWait::Update(DesireUpdate*, float)
{
    m_pFielder->fn_8003057C(0);
    m_pFielder->AddDesiredPosition(m_pFielder->mUnidentified024.m_v3Position, 1.0f, 1.0f);
}

DesireFinishAction::~DesireFinishAction()
{
}

DesireWait::~DesireWait()
{
}

void Desire::UnidentifiedCleanup()
{
}

void Desire::Update(DesireUpdate*, float)
{
}

bool DesireCutAndBreak::UnidentifiedInitialize(void* context)
{
    bool initialized = Desire::UnidentifiedInitialize(context);
    nlVector3 searchCenter;
    if (fn_800381B4(m_pFielder, &searchCenter))
    {
        searchCenter = m_pFielder->mUnidentified024.m_v3Position;
    }

    SSearchCutAndBreak* search = new (nlMalloc(sizeof(SSearchCutAndBreak), 8, false))
        SSearchCutAndBreak(m_pFielder);
    mUnidentifiedA4 = search;
    m_pFielder->SetSpaceSearch(search);
    m_pFielder->m_pSpaceSearch->m_bDebugOn = lbl_806E0E20;
    m_pFielder->m_pSpaceSearch->FindBestPosition(
        mvDesiredPosition, searchCenter, DIR_NONE, 0, lbl_806DC058, 0x8000);
    return initialized;
}

void DesireCutAndBreak::UnidentifiedCleanup()
{
    if (mUnidentifiedA4 == m_pFielder->m_pSpaceSearch)
    {
        m_pFielder->SetSpaceSearch(0);
    }
    mUnidentifiedA4 = 0;
}

DesireCutAndBreak::~DesireCutAndBreak()
{
}

void DesireCutAndBreak::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireCutAndBreak");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

void DesireCutAndBreak::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (lbl_806DC05C == 0xFFFF)
    {
        UnidentifiedVirtual8(&lbl_806DC05C, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(lbl_806DC05C, data, context);
    cache->WriteData(lbl_806DC05C, data, sizeof(DesireCutAndBreak) - offset);
}

DesireDeke::~DesireDeke()
{
}

bool DesireDeke::UnidentifiedInitialize(void* context)
{
    mUnidentifiedA4 = 0;
    if (((UnidentifiedVariantCollection*)context)->IsSet(14))
    {
        mUnidentifiedA4
            = ((UnidentifiedVariantCollection*)context)->Get(14)->mData.pointer;
    }
    return true;
}

void DesireDeke::Update(DesireUpdate* update, float)
{
    if (m_pFielder->m_eActionState == 1)
    {
        return;
    }

    bool avoidSideline = false;
    unsigned short direction = m_pFielder->mUnidentified024.m_aActualFacingDirection;
    switch (m_pFielder->mUnidentified024.m_eCharacterClass)
    {
    case BIRDO:
    case KOOPA:
        avoidSideline = true;
        if (m_pFielder->fn_8003E6EC() && mUnidentifiedA4 != 0)
        {
            cFielder* target = (cFielder*)mUnidentifiedA4;
            nlVector3 delta;
            nlVec3Sub(delta, target->mUnidentified024.m_v3Position,
                m_pFielder->mUnidentified024.m_v3Position);
            direction = (u16)(s32)(nlATan2f(delta.y, delta.x) * 10430.378f);
            break;
        }
        // Without an explicit target, use the nearest opponent below.

    case TOAD:
    case 14:
    case 16:
    case 19:
    {
        avoidSideline = true;
        cPlayer* opponent = m_pFielder->fn_800966AC(0, true);
        if (opponent == 0)
        {
            update->SetDesireFinished();
            return;
        }
        Goalie* goalie = m_pFielder->m_pTeam->GetOtherTeam()->GetGoalie();
        nlVector3 opponentPosition = opponent->mUnidentified024.m_v3Position;
        direction = opponent->mUnidentified024.m_aActualMovementDirection;
        nlVector3 opponentDelta;
        nlVec3Sub2D(opponentDelta, opponent->mUnidentified024.m_v3Position,
            m_pFielder->mUnidentified024.m_v3Position);
        nlVector3 goalieDelta;
        nlVec3Sub2D(goalieDelta, goalie->mUnidentified024.m_v3Position,
            m_pFielder->mUnidentified024.m_v3Position);
        if (goalieDelta.GetLengthSq2D() < opponentDelta.GetLengthSq2D())
        {
            opponentPosition = goalie->mUnidentified024.m_v3Position;
            direction = goalie->mUnidentified024.m_aActualFacingDirection;
        }
        nlVector3 delta;
        nlVec3Sub(delta, m_pFielder->mUnidentified024.m_v3Position, opponentPosition);
        unsigned short away = (u16)(s32)(nlATan2f(delta.y, delta.x) * 10430.378f);
        direction += 0x8000;
        direction = direction + (s16)(s32)(0.5f * (s16)(away - direction));
        break;
    }

    case LUIGI:
    case WARIO:
    case 15:
    {
        avoidSideline = true;
        cPlayer* opponent = m_pFielder->fn_800966AC(0, true);
        if (opponent == 0)
        {
            update->SetDesireFinished();
            return;
        }
        nlVector3 opponentDelta;
        nlVec3Sub(opponentDelta, opponent->mUnidentified024.m_v3Position,
            m_pFielder->mUnidentified024.m_v3Position);
        unsigned short opponentAngle = (u16)(s32)(nlATan2f(opponentDelta.y, opponentDelta.x) * 10430.378f);
        Goalie* goalie = m_pFielder->m_pTeam->GetOtherTeam()->GetGoalie();
        nlVector3 goalieDelta;
        nlVec3Sub(goalieDelta, goalie->mUnidentified024.m_v3Position,
            m_pFielder->mUnidentified024.m_v3Position);
        unsigned short goalieAngle = (u16)(s32)(nlATan2f(goalieDelta.y, goalieDelta.x) * 10430.378f);
        nlVector3 distance;
        nlVec3Sub2D(distance, goalie->mUnidentified024.m_v3Position,
            m_pFielder->mUnidentified024.m_v3Position);
        float blend = InterpolateRangeClamped(1.0f, 0.0f, 2.5f, 0.66f,
            nlSqrt(distance.GetLengthSq2D(), true));
        direction = goalieAngle + (s16)(s32)(blend * (s16)(opponentAngle - goalieAngle));
        break;
    }

    case DONKEYKONG:
    case MARIO:
    case 17:
    {
        float dekeDistance = fn_80039574(m_pFielder);
        cNet* net = m_pFielder->m_pTeam->GetOtherNet();
        Goalie* goalie = m_pFielder->m_pTeam->GetOtherTeam()->GetGoalie();
        nlVector3 goalieDelta;
        nlVec3Sub(goalieDelta, goalie->mUnidentified024.m_v3Position,
            m_pFielder->mUnidentified024.m_v3Position);
        unsigned short goalieAngle = (u16)(s32)(nlATan2f(goalieDelta.y, goalieDelta.x) * 10430.378f);
        nlVector3 goalLine = m_pFielder->mUnidentified024.m_v3Position;
        goalLine.x = cField::GetGoalLineX(1U);
        goalLine.x *= AIsgn(net->m_v3NetLocation.x);
        nlVector3 goalDelta;
        nlVec3Sub(goalDelta, goalLine, m_pFielder->mUnidentified024.m_v3Position);
        unsigned short goalAngle = (u16)(s32)(nlATan2f(goalDelta.y, goalDelta.x) * 10430.378f);
        nlVector3 goalDistance;
        nlVec3Sub2D(goalDistance, goalLine, m_pFielder->mUnidentified024.m_v3Position);
        AvoidableObject* playerObject = m_pFielder->mUnidentified320;
        float goalDistanceLength = nlSqrt(goalDistance.GetLengthSq2D(), true);
        float goalRange = goalDistanceLength - playerObject->GetRadius();
        float blend = InterpolateRangeClamped(1.0f, 0.0f,
            0.8f * dekeDistance, 2.0f * dekeDistance, goalRange);
        goalie = m_pFielder->m_pTeam->GetOtherTeam()->GetGoalie();
        nlVector3 distance;
        nlVec3Sub2D(distance, m_pFielder->mUnidentified024.m_v3Position,
            goalie->mUnidentified024.m_v3Position);
        float goalieRange = nlSqrt(distance.GetLengthSq2D(), true);
        goalie = m_pFielder->m_pTeam->GetOtherTeam()->GetGoalie();
        playerObject = m_pFielder->mUnidentified320;
        float goalieRadius = goalie->mUnidentified320->GetRadius();
        float playerRadius = playerObject->GetRadius();
        if (goalieRange + (playerRadius + goalieRadius) > dekeDistance)
        {
            blend = 0.0f;
        }
        if (goalRange < dekeDistance)
        {
            blend = 1.0f;
        }
        direction = goalAngle + (s16)(s32)(blend * (s16)(goalieAngle - goalAngle));
        break;
    }

    case DAISY:
    case HAMMERBROS:
    case PEACH:
    case WALUIGI:
    case YOSHI:
    case MYSTERY:
    case 13:
    case 18:
    {
        cPlayer* opponent = m_pFielder->fn_800966AC(0, true);
        if (opponent == 0)
        {
            update->SetDesireFinished();
            return;
        }
        nlVector3 delta;
        nlVec3Sub(delta, opponent->mUnidentified024.m_v3Position,
            m_pFielder->mUnidentified024.m_v3Position);
        direction = (u16)(s32)(nlATan2f(delta.y, delta.x) * 10430.378f);
        break;
    }
    }

    if (avoidSideline)
    {
        nlVector2 sidelineDirection;
        if (CloseToSideline(m_pFielder->mUnidentified024.m_v3Position, 0, false, &sidelineDirection) > 0.2f)
        {
            unsigned short sidelineAngle = (u16)(s32)(nlATan2f(sidelineDirection.y, sidelineDirection.x) * 10430.378f);
            short delta = sidelineAngle - direction;
            unsigned short angleDistance = delta < 0 ? -delta : delta;
            if (angleDistance < 0x2000)
            {
                direction += 0x8000;
            }
        }
    }
    m_pFielder->fn_800447C0(direction);
}

void DesireDeke::UnidentifiedCleanup()
{
    m_pFielder->mUnidentified1E4.m_eLastPadAction = 50;
}

void DesireDeke::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireDeke");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

void DesireDeke::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (lbl_806DC060 == 0xFFFF)
    {
        UnidentifiedVirtual8(&lbl_806DC060, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(lbl_806DC060, data, context);
    cache->WriteData(lbl_806DC060, data, sizeof(DesireDeke) - offset);
}

void DesireHit::Update(DesireUpdate*, float)
{
}

bool DesireHit::UnidentifiedInitialize(void* context)
{
    bool initialized = Desire::UnidentifiedInitialize(context);
    UserControlledT(m_pFielder->m_pTeam);
    m_pFielder->InitActionHit(
        (cFielder*)((UnidentifiedVariantCollection*)context)->Get(14)->mData.pPlayer,
        m_pFielder->mUnidentified024.m_aActualFacingDirection);
    return initialized;
}

DesireHit::~DesireHit()
{
}

void DesireHit::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireHit");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

void DesireHit::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (lbl_806DC0A0 == 0xFFFF)
    {
        UnidentifiedVirtual8(&lbl_806DC0A0, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(lbl_806DC0A0, data, context);
    cache->WriteData(lbl_806DC0A0, data, sizeof(DesireHit) - offset);
}

void DesireGetOpen::Update(DesireUpdate*, float)
{
    m_pFielder->AddDesiredPosition(mvDesiredPosition, 1.2f, 1.0f);
}

void DesireGetOpen::UnidentifiedCleanup()
{
    if (mUnidentifiedA4 == m_pFielder->m_pSpaceSearch)
    {
        m_pFielder->SetSpaceSearch(0);
    }
    mUnidentifiedA4 = 0;
}

DesireGetOpen::~DesireGetOpen()
{
}

void DesireGetOpen::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireGetOpen");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

void DesireGetOpen::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (lbl_806DC094 == 0xFFFF)
    {
        UnidentifiedVirtual8(&lbl_806DC094, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(lbl_806DC094, data, context);
    cache->WriteData(lbl_806DC094, data, sizeof(DesireGetOpen) - offset);
}

bool DesireGetInPosition::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    m_pFielder->StartRunning();
    return result;
}

DesireGetInPosition::~DesireGetInPosition()
{
}

void DesireGetInPosition::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireGetInPosition");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

void DesireGetInPosition::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (lbl_806DC068 == 0xFFFF)
    {
        UnidentifiedVirtual8(&lbl_806DC068, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(lbl_806DC068, data, context);
    cache->WriteData(lbl_806DC068, data, sizeof(DesireGetInPosition) - offset);
}

bool DesireRunUpfield::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    m_pFielder->StartRunning();
    return result;
}

DesireRunUpfield::~DesireRunUpfield()
{
}

void DesireRunUpfield::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireRunUpfield");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

void DesireRunUpfield::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (lbl_806DC06A == 0xFFFF)
    {
        UnidentifiedVirtual8(&lbl_806DC06A, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(lbl_806DC06A, data, context);
    cache->WriteData(lbl_806DC06A, data, sizeof(DesireRunUpfield) - offset);
}

bool DesireRunDownfield::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    m_pFielder->StartRunning();
    return result;
}

DesireRunDownfield::~DesireRunDownfield()
{
}

void DesireRunDownfield::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireRunDownfield");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

void DesireRunDownfield::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (lbl_806DC06C == 0xFFFF)
    {
        UnidentifiedVirtual8(&lbl_806DC06C, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(lbl_806DC06C, data, context);
    cache->WriteData(lbl_806DC06C, data, sizeof(DesireRunDownfield) - offset);
}

void DesireRunInDirection::UnidentifiedCleanup()
{
}

bool DesireRunInDirection::UnidentifiedInitialize(void* context)
{
    m_fSpeed = 1.0f;
    m_pTarget = 0;
    m_fDistTravelled = 0.0f;
    m_fMaxDistance = -1.0f;
    m_aDirection = 0;
    m_eFieldDirection = DIR_NONE;
    bool initialized = true;
    UnidentifiedVariantCollection* parameters = (UnidentifiedVariantCollection*)context;

    if (parameters->IsSet(14))
    {
        m_pTarget = (cFielder*)parameters->Get(14)->mData.pointer;
        if (m_pTarget == m_pFielder)
        {
            return false;
        }
    }
    if (parameters->IsSet(13))
    {
        m_fSpeed = parameters->Get(13)->mData.f;
    }
    if (parameters->IsSet(17))
    {
        if (m_pTarget != 0)
        {
            m_eFieldDirection = parameters->Get(17)->mData.i;
            m_aDirection = 0;
            if (m_eFieldDirection != DIR_TOWARD_TARGET && m_eFieldDirection != DIR_AWAYFROM_TARGET)
            {
                return false;
            }
        }
        else
        {
            m_aDirection = parameters->Get(17)->mData.i;
        }

        m_fMaxDistance = -1.0f;
        if (parameters->IsSet(18))
        {
            m_fMaxDistance = parameters->Get(18)->mData.f;
            mUnidentified078 = 0.5f + m_fMaxDistance / fn_8002E1B0(m_pFielder);
        }
        m_pFielder->StartRunning();
    }
    else
    {
        initialized = false;
    }
    return initialized;
}

void DesireRunInDirection::Update(DesireUpdate* update, float deltaTime)
{
    if (m_pFielder->fn_8003EA6C() && Incapacitated(m_pTarget) != 0.0f)
    {
        update->SetDesireFinished();
        return;
    }
    if (update->mData.i != DESIRE_CONTINUE)
    {
        return;
    }
    if (m_fMaxDistance > 0.0f && m_fDistTravelled >= m_fMaxDistance)
    {
        update->SetDesireFinished();
        return;
    }

    if (m_pTarget != 0)
    {
        nlVector3 direction;
        nlVec3Sub(direction, m_pTarget->mUnidentified024.m_v3Position,
            m_pFielder->mUnidentified024.m_v3Position);
        if (m_eFieldDirection == DIR_AWAYFROM_TARGET)
        {
            nlVec3Scale(direction, -1.0f);
        }
        m_aDirection = (u16)(s32)(nlATan2f(direction.y, direction.x) * 10430.378f);
    }

    nlVector3 direction;
    direction.z = 0.0f;
    nlPolar polar;
    polar.a = m_aDirection;
    polar.r = 1.0f;
    nlPolarToCartesian(direction, polar);
    nlVec3ScaleAdd(mvDesiredPosition, 5.0f, direction,
        m_pFielder->mUnidentified024.m_v3Position);
    m_pFielder->AddDesiredPosition(mvDesiredPosition, m_fSpeed, 1.0f);
    m_fDistTravelled += deltaTime * m_pFielder->mUnidentified024.m_fActualSpeed;
}

DesireRunInDirection::~DesireRunInDirection()
{
}

void DesireRunInDirection::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireRunInDirection");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->AddField(19, gDebugFieldTypes[19].size, (u8*)&m_aDirection - (u8*)&mvDesiredPosition, "m_aDirection");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_fMaxDistance - (u8*)&mvDesiredPosition, "m_fMaxDistance");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_fDistTravelled - (u8*)&mvDesiredPosition, "m_fDistTravelled");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_fSpeed - (u8*)&mvDesiredPosition, "m_fSpeed");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&m_eFieldDirection - (u8*)&mvDesiredPosition, "m_eFieldDirection");
    cache->AddField(15, gDebugFieldTypes[15].size, (u8*)&m_pTarget - (u8*)&mvDesiredPosition, "m_pTarget");
    cache->EndType();
}

void DesireRunInDirection::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (lbl_806DC06E == 0xFFFF)
    {
        UnidentifiedVirtual8(&lbl_806DC06E, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = cache->WriteData(lbl_806DC06E,
        (u8*)this + offset, sizeof(DesireRunInDirection) - offset);
    if (data != 0)
    {
        DesireRunInDirection* desire = (DesireRunInDirection*)((u8*)data - offset);
        desire->m_pTarget = (cFielder*)(m_pTarget == 0
                ? -1 : m_pTarget->mUnidentified120);
        cache->ChecksumData(lbl_806DC06E, data, context);
    }
}

void DesireRunToTarget::UnidentifiedCleanup()
{
}

DesireRunToTarget::~DesireRunToTarget()
{
}

void DesireRunToTarget::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireRunToTarget");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->AddField(15, gDebugFieldTypes[15].size, (u8*)&m_pTargetFielder - (u8*)&mvDesiredPosition, "m_pTargetFielder");
    cache->AddField(22, gDebugFieldTypes[22].size, (u8*)&m_vTargetPos - (u8*)&mvDesiredPosition, "m_vTargetPos");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&m_eDirection - (u8*)&mvDesiredPosition, "m_eDirection");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_fDistOffset - (u8*)&mvDesiredPosition, "m_fDistOffset");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_fUrgency - (u8*)&mvDesiredPosition, "m_fUrgency");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_fSpeedCoeff - (u8*)&mvDesiredPosition, "m_fSpeedCoeff");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_fAvoidanceCoeff - (u8*)&mvDesiredPosition, "m_fAvoidanceCoeff");
    cache->EndType();
}

void DesireRunToTarget::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (lbl_806DC070 == 0xFFFF)
    {
        UnidentifiedVirtual8(&lbl_806DC070, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = cache->WriteData(lbl_806DC070,
        (u8*)this + offset, sizeof(DesireRunToTarget) - offset);
    if (data != 0)
    {
        DesireRunToTarget* desire = (DesireRunToTarget*)((u8*)data - offset);
        desire->m_pTargetFielder = (cFielder*)(m_pTargetFielder == 0
                ? -1 : m_pTargetFielder->mUnidentified120);
        cache->ChecksumData(lbl_806DC070, data, context);
    }
}

class SandTombWeather;

extern "C" int fn_800B04B4(SandTombWeather*)
{
    return 4;
}
