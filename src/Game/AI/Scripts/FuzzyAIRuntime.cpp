#include "Game/Ball.h"
#include "Game/AI/SkillTweaks.h"
#include "Game/AI/Fuzzy.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Game.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlDebug.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/AIContext.h"
#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/TeamPlayMachine.h"
#include "Game/AI/Variant.h"
#include "Game/InterpreterCore.h"
#include "Game/Team.h"
#include "NL/nlList.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"
#include "types.h"

struct FuzzyFielderIterator
{
    unsigned int mCurrent;
    unsigned int mEnd;
    cTeam* mTeam;
    cFielder* mSkip;
};

struct FuzzyFielderReference
{
    int mIndex;
    u8 mUnidentified004[0x04];
    cTeam* mTeam;
};


SlotPool<FuzzyFielderIterator> g_FuzzyFielderIteratorPool(16, 16);

#include "src/Game/AI/Scripts/FuzzyAIRuntime_interp.cpp"

char g_FuzzyAIScriptFilename[] = "art/Scripts/FuzzyAI.byte_code";
char* g_pFuzzyAIScriptFilename = g_FuzzyAIScriptFilename;

extern "C" FuzzyRuntimeBase* FuzzyAIGetFielderRuntime(cFielder* pFielder)
{
    return pFielder->GetFuzzyRuntime();
}

extern "C" FuzzyRuntimeBase* FuzzyAIGetTeamRuntime(cTeam* pTeam)
{
    return GetTeamFuzzyRuntime(pTeam);
}

FuzzyAIRuntime::FuzzyAIRuntime()
    : FuzzyRuntimeBase(0)
{
    if (g_FuzzyParameters.mHead == 0)
    {
        RegisterParameters();
    }
}

FuzzyAIRuntime::~FuzzyAIRuntime()
{
    g_FuzzyFielderIteratorPool.FreeBlocks();
}

extern "C" const char* GetFuzzyAIScriptFilename()
{
    return g_pFuzzyAIScriptFilename;
}

void FuzzyAIRuntime::RegisterParameters()
{
    FuzzyRuntimeBase::RegisterParameters();
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Speed", 13));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Target", 14));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Powerup", 15));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Lob", 16));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Direction", 17));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Distance", 18));
}

float FuzzyAIRuntime::BeginActionQueue()
{
    float result = FuzzyRuntimeBase::BeginActionQueue();
    SkillTweaks* value = 0;

    switch (mAIContext->GetType())
    {
    case FT_PLAYER:
        value = fn_800A636C(mAIContext->GetPlayer()->m_pTeam);
        break;
    case FT_TEAM:
        value = fn_800A636C(mAIContext->GetTeam());
        break;
    case FT_GAME:
        value = 0;
        break;
    }

    if (value != 0)
    {
        UnidentifiedActionQueue* collection = mActionQueues.mHead->mQueue;
        collection->fn_80310034(value->GetDecisionWeights(), 4);
    }

    return result;
}

UnidentifiedVariant_80054AB8*
FuzzyAIRuntime::EndActionQueue()
{
    return FuzzyRuntimeBase::EndActionQueue();
}

void FuzzyAIRuntime::AddAction(
    UnidentifiedVariant_80054AB8* action)
{
    FuzzyRuntimeBase::AddAction(action);
}

extern "C" cPlayer* FuzzyAIGetBallOwner()
{
    return g_pBall->m_pOwner;
}

extern "C" cBall* FuzzyAIGetBall()
{
    return g_pBall;
}

extern "C" void* FuzzyAIGetGame()
{
    return g_pGame;
}

float FuzzyAIRuntime::GetSkillValue(unsigned long hash)
{
    SkillTweaks* skillTweaks = 0;
    cFielder* fielder = 0;

    switch (mAIContext->GetType())
    {
    case FT_PLAYER:
    {
        cPlayer* player = mAIContext->GetPlayer();
        skillTweaks = fn_800A636C(player->GetTeam());
        if (player->m_eClassType == FIELDER)
        {
            fielder = static_cast<cFielder*>(player);
        }
        break;
    }
    case FT_TEAM:
    {
        cTeam* team = mAIContext->GetTeam();
        skillTweaks = fn_800A636C(team);
        fielder = team->GetCaptain();
        break;
    }
    }

    float result = 1.0f;
    skillTweaks->GetSkillValue(hash, &result, fielder);
    return result;
}

extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnInt_800E35D4(
    FuzzyAIRuntime* runtime, int value, float confidence)
{
    return runtime->CreateReturnValue(FT_INT, value, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnInt_800E3700(
    FuzzyAIRuntime* runtime, int value, float confidence)
{
    return runtime->CreateReturnValue(FT_INT, value, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnInt_800E382C(
    FuzzyAIRuntime* runtime, int value, float confidence)
{
    return runtime->CreateReturnValue(FT_INT, value, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnInt_800E3958(
    FuzzyAIRuntime* runtime, int value, float confidence)
{
    return runtime->CreateReturnValue(FT_INT, value, confidence);
}

extern "C" void FuzzyAISetPlayerParameter_800E3A84(
    void*, cPlayer* value, unsigned long parameterHash,
    UnidentifiedVariant_80054AB8* action)
{
    int index = FuzzyFindParameterIndex(parameterHash);
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = value;
    action->ExtraData.Set(index, variant);
}

extern "C" void FuzzyAISetBallParameter(
    void*, cBall* value, unsigned long parameterHash,
    UnidentifiedVariant_80054AB8* action)
{
    int index = FuzzyFindParameterIndex(parameterHash);
    FuzzyVariant variant;
    variant.mType = FT_BALL;
    variant.mData.pointer = value;
    action->ExtraData.Set(index, variant);
}

extern "C" void* fn_800E3BE4(void*, void* value)
{
    return value;
}

extern "C" void* fn_800E3BEC(void*, void* value)
{
    return value;
}

extern "C" void* fn_800E3BF4(void*, void* value)
{
    return value;
}

extern "C" void* fn_800E3BFC(void*, void* value)
{
    return value;
}

extern "C" void* fn_800E3C04(void*, void* value)
{
    return value;
}

extern "C" void* fn_800E3C0C(void*, void* value)
{
    return value;
}

extern "C" void* fn_800E3C14(void*, Variant* value)
{
    return value != 0 ? value->mData.pointer : 0;
}

extern "C" void* fn_800E3C2C(void*, Variant* value)
{
    return value != 0 ? value->mData.pointer : 0;
}

extern "C" void* fn_800E3C44(void*, Variant* value)
{
    return value != 0 ? value->mData.pointer : 0;
}

extern "C" void* fn_800E3C5C(void*, Variant* value)
{
    return value != 0 ? value->mData.pointer : 0;
}

extern "C" FuzzyFielderIterator* FuzzyAICreateTeamFielderIterator(
    void*, cTeam* team)
{
    FuzzyFielderIterator* iterator = 0;
    g_FuzzyFielderIteratorPool.Allocate(iterator);
    iterator->mCurrent = 0;
    iterator->mEnd = 4;
    iterator->mTeam = team;
    iterator->mSkip = 0;
    return iterator;
}

extern "C" FuzzyFielderIterator* FuzzyAICreateOpponentFielderIterator(
    void*, cFielder* fielder)
{
    cTeam* team = fielder->m_pTeam->GetOtherTeam();
    FuzzyFielderIterator* iterator = 0;
    g_FuzzyFielderIteratorPool.Allocate(iterator);
    iterator->mCurrent = 0;
    iterator->mEnd = 4;
    iterator->mTeam = team;
    iterator->mSkip = 0;
    return iterator;
}

extern "C" FuzzyFielderIterator* FuzzyAICreateTeammateIterator(
    void*, cFielder* fielder)
{
    FuzzyFielderIterator* iterator = 0;
    cTeam* team = fielder->m_pTeam;
    g_FuzzyFielderIteratorPool.Allocate(iterator);
    iterator->mCurrent = 0;
    iterator->mEnd = 4;
    iterator->mTeam = team;
    iterator->mSkip = fielder;

    if (iterator->mCurrent < iterator->mEnd && iterator->mSkip != 0
        && iterator->mTeam->GetFielder(iterator->mCurrent)
               == iterator->mSkip)
    {
        ++iterator->mCurrent;
    }

    return iterator;
}

extern "C" FuzzyFielderIterator* FuzzyAIAdvanceFielderIterator(
    void*, FuzzyFielderIterator* iterator)
{
    ++iterator->mCurrent;
    if (iterator->mCurrent < iterator->mEnd && iterator->mSkip != 0
        && iterator->mTeam->GetFielder(iterator->mCurrent)
               == iterator->mSkip)
    {
        ++iterator->mCurrent;
    }
    return iterator;
}

extern "C" bool FuzzyAIHasNextFielder(
    void*, FuzzyFielderIterator* iterator)
{
    return iterator->mCurrent < iterator->mEnd;
}

extern "C" void FuzzyAIDestroyFielderIterator(
    void*, FuzzyFielderIterator* entry)
{
    g_FuzzyFielderIteratorPool.Free(entry);
}

extern "C" cFielder* FuzzyAIGetIteratorFielder_800E3F10(
    void*, FuzzyFielderIterator* iterator)
{
    return iterator->mTeam->GetFielder(iterator->mCurrent);
}

extern "C" cFielder* FuzzyAIGetIteratorFielder_800E3F1C(
    void*, FuzzyFielderIterator* iterator)
{
    return iterator->mTeam->GetFielder(iterator->mCurrent);
}

extern "C" AIContext* FuzzyAIGetIteratorAIContext(
    void*, FuzzyFielderIterator* iterator)
{
    cFielder* fielder = iterator->mTeam->GetFielder(iterator->mCurrent);
    return fielder->mUnidentified428;
}
