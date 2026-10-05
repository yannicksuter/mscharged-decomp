#include "Game/ExcitementSystem.h"
#include "Game/AI/FielderActions.h"

#include "Game/Event.h"
#include "Game/EventRegistry.h"
#include "Game/EventDataTypes.h"
#include "Game/GameEventQueue.h"
#include "NL/nlBind.h"
#include "NL/nlDebug.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlFunction.inl"

namespace
{
char sExcitementSystemByteCode[] = "art/Scripts/ExcitementSystem.byte_code";
const char sMainFunctionName[] = "Main";
} // namespace

inline ExcitementSystem::ExcitementSystem()
    : InterpreterCore(100)
    , mMaxBallDistanceSq(200.0f)
    , mExcitement(0)
    , mExcitementCount(0)
    , mByteCode(0)
{
    ClearExcitementValues();
    LoadScript();
}

ExcitementSystem& ExcitementSystem::Instance()
{
    static ExcitementSystem instance;
    return instance;
}

void ExcitementSystem::ClearExcitementValues()
{
    for (int i = 0; i < 130; i++)
    {
        mFielderAnimExcitement[i] = 0;
    }
    for (int i = 0; i < 178; i++)
    {
        mGoalieAnimExcitement[i] = 0;
    }
    for (int i = 0; i < 4; i++)
    {
        mEventExcitement[i] = 0;
    }
}

void ExcitementSystem::RegisterEventHandlers()
{
    typedef BindExp2<void,
        Detail::MemFunImpl<void, void (ExcitementSystem::*)(PlayerAttackData*)>,
        ExcitementSystem*, Placeholder<0> > AttackBinding;
    typedef BindExp2<void,
        Detail::MemFunImpl<void, void (ExcitementSystem::*)(LightningStrikeData*)>,
        ExcitementSystem*, Placeholder<0> > LightningBinding;
    typedef BindExp2<void,
        Detail::MemFunImpl<void, void (ExcitementSystem::*)(CollisionBallGoalpostData*)>,
        ExcitementSystem*, Placeholder<0> > GoalpostBinding;

    UnidentifiedFindEvent<PlayerAttackData>("AttackSuccess", -1)->Add(Function<PlayerAttackData*>(AttackBinding(MemFun(&ExcitementSystem::OnAttackSuccess), this, placeholder0)), 0, -1);
    UnidentifiedFindEvent<LightningStrikeData>("LightningStrike", -1)->Add(Function<LightningStrikeData*>(LightningBinding(MemFun(&ExcitementSystem::OnLightningStrike), this, placeholder0)), 0, -1);
    UnidentifiedFindEvent<CollisionBallGoalpostData>("CollisionBallGoalpost", -1)->Add(Function<CollisionBallGoalpostData*>(GoalpostBinding(MemFun(&ExcitementSystem::OnCollisionBallGoalpost), this, placeholder0)), 0, -1);
}

void ExcitementSystem::OnAttackSuccess(
    PlayerAttackData* event)
{
    if (event->mUnidentified10 == 1 && mEventExcitement[1] != 0)
    {
        mExcitement += mEventExcitement[1];
        mExcitementCount++;
    }
}

void ExcitementSystem::OnLightningStrike(LightningStrikeData*)
{
    if (mEventExcitement[3] != 0)
    {
        mExcitement += mEventExcitement[3];
        mExcitementCount++;
    }
}

void ExcitementSystem::OnCollisionBallGoalpost(CollisionBallGoalpostData*)
{
    if (mEventExcitement[0] != 0)
    {
        mExcitement += mEventExcitement[0];
        mExcitementCount++;
    }
}

void ExcitementSystem::DoFunctionCall(unsigned int function)
{
    switch (function)
    {
    case 0:
    {
        float value = *(float*)(m_SP - 1);
        m_SP--;
        mMaxBallDistanceSq = value * value;
        break;
    }
    case 1:
    {
        unsigned int value = m_SP[-1];
        unsigned int index = m_SP[-2];
        m_SP -= 2;
        mFielderAnimExcitement[index] = value;
        break;
    }
    case 2:
    {
        unsigned int value = m_SP[-1];
        unsigned int index = m_SP[-2];
        m_SP -= 2;
        mGoalieAnimExcitement[index] = value;
        break;
    }
    case 3:
    {
        unsigned int value = m_SP[-1];
        unsigned int index = m_SP[-2];
        m_SP -= 2;
        mEventExcitement[index] = value;
        break;
    }
    default:
        nlBreak();
        break;
    }
}

void ExcitementSystem::LoadScript()
{
    if (mByteCode != 0)
    {
        nlFree(mByteCode);
        mByteCode = 0;
    }

    unsigned long fileSize = 0;
    mByteCode = nlLoadEntireFile(sExcitementSystemByteCode,
        &fileSize,
        0x20,
        AllocateStart,
        0,
        0,
        0);
    LoadByteCode(mByteCode);
    CallFunction(nlStringHash(sMainFunctionName));
}
