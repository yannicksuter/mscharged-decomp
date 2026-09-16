#include "Game/Ball.h"
#include "Game/AI/SkillTweaks.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlDebug.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FielderInput.h"
#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/TeamPlayMachine.h"
#include "Game/AI/Variant.h"
#include "Game/InterpreterCore.h"
#include "Game/Team.h"
#include "NL/nlList.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"
#include "types.h"

struct UnidentifiedFielderIterator
{
    unsigned int mCurrent;
    unsigned int mEnd;
    cTeam* mTeam;
    cFielder* mSkip;
};

struct UnidentifiedRuntimeFielderReference
{
    int mIndex;
    u8 mUnidentified004[0x04];
    cTeam* mTeam;
};

struct UnidentifiedTransitionReference;
float AbleToInterceptBall(cPlayer*);
extern "C" float fn_800D82C0(cFielder*);
extern "C" float fn_800D7AB8(cFielder*);
extern "C" float fn_800D7B00(cFielder*);
extern "C" float fn_800D85F8(cFielder*);
extern "C" float fn_800D8764(cFielder*, int);
extern "C" float fn_800D84F8(cFielder*);
extern "C" float fn_800D7988(int, cFielder*);
extern "C" void fn_80318D34(UnidentifiedScriptMachine*, int, const char*, bool);
float AggressiveT(cTeam*);
extern "C" float fn_800DA330(cFielder*);
extern "C" float fn_800DA310(cFielder*);
extern "C" float fn_800DD9C8(cFielder*, cPlayer*);
float PlayerShotDistance(cFielder*);
extern "C" float fn_800DDD70(cFielder*);
extern "C" float fn_800D96F4(cFielder*);
extern "C" void* fn_800E3BF4(void*, void*);
extern "C" void* fn_800E3C04(void*, void*);
extern "C" void* fn_800E3C0C(void*, void*);
extern "C" void* fn_800E3BE4(void*, void*);
extern "C" cFielder* fn_800E3F10(void*, UnidentifiedFielderIterator*);
extern "C" cFielder* fn_800E3F1C(void*, UnidentifiedFielderIterator*);
extern "C" UnidentifiedFielderInput* fn_800E3F28(void*, UnidentifiedFielderIterator*);
extern "C" void* fn_800E3BFC(void*, void*);
extern "C" void* fn_800E3BEC(void*, void*);
extern "C" void* fn_800E3C5C(void*, Variant*);
extern "C" void* fn_800E3C44(void*, Variant*);
extern "C" float fn_800E7ECC(void*, Variant*);
extern "C" unsigned long fn_800E7ED4(void*, Variant*);
extern "C" void* fn_800E3C14(void*, Variant*);
extern "C" void* fn_800E3C2C(void*, Variant*);
extern "C" unsigned long fn_800E8CB0(void*, Variant*);
extern "C" unsigned long fn_800E7EE4(void*, Variant*);
extern "C" float fn_800E7EEC(void*, Variant*);
extern "C" unsigned long fn_800E7EDC(void*, Variant*);
extern "C" UnidentifiedFuzzyRuntimeValue* fn_8031479C(void*, UnidentifiedFuzzyRuntimeBase*);
extern "C" float fn_800D9B0C(cFielder*);
extern "C" float fn_800D9A38(cFielder*);
extern "C" float fn_800D9B74(cFielder*);
extern "C" float fn_800D9BDC(cFielder*);
extern "C" float fn_800D9C24(cFielder*);
extern "C" float fn_800E06F4(cPlayer*);
extern "C" float fn_800DF838(cPlayer*);
float BallOwner(cPlayer*);
float BallOwnerT(cTeam*);
extern "C" cFielder* fn_800D671C(cTeam*);
float Captain(cFielder*);
extern "C" cFielder* fn_800D6708(cTeam*);
extern "C" float fn_800D795C(cFielder*, int);
extern "C" float fn_800DF590(cBall*);
extern "C" float fn_800DED80(cPlayer*);
extern "C" float fn_80314494(float, float, float);
extern "C" cFielder* fn_800D674C(cPlayer*);
extern "C" float fn_800D8A9C(cFielder*);
extern "C" float fn_800D8BAC(cFielder*);
float CloseTo(cPlayer*, cPlayer*);
float CloseToBall(cPlayer*);
extern "C" float fn_800DD45C(cFielder*);
extern "C" float fn_800DD504(cPlayer*, cFielder*);
float CloseToFormationPosition(cFielder*);
float CloseToMyGoalie(cPlayer*);
float CloseToMyNet(cPlayer*);
extern "C" float fn_800D92DC(cFielder*);
float CloseToSideline(cFielder*);
extern "C" float fn_800DD2F4(cBall*);
extern "C" float fn_800D912C(cFielder*);
extern "C" float fn_800DE804(cBall*, cTeam*);
float CloseToTheirGoalie(cPlayer*);
float CloseToTheirNet(cPlayer*);
float ClosingTo(cPlayer*, cPlayer*);
float ClosingTo(cPlayer*, cBall*);
extern "C" float fn_800E7EF4(void*, UnidentifiedVariant_80054AB8*);
extern "C" float fn_800DA130(cFielder*);
extern "C" void fn_803148C4(float);
extern "C" void fn_803148D0(void*, const char*);
float Defence(cFielder*);
float Defensive(cTeam*);
extern "C" float fn_800D7910(cFielder*);
float Difficult(cTeam*);
extern "C" bool fn_803147C0(UnidentifiedFuzzyRuntimeBase*, unsigned long);
float DoingS2S(cFielder*);
extern "C" float fn_800DE4B0(cPlayer*, cPlayer*);
extern "C" void fn_80314740(void*, bool);
extern "C" float fn_800DE1F0(cPlayer*, cPlayer*);
extern "C" float fn_800DDF54(cPlayer*, cPlayer*);
extern "C" float fn_800DE0A8(cPlayer*);
extern "C" float fn_800DD744(cFielder*);
float FallenDown(cFielder*);
extern "C" float fn_800DA050(cFielder*);
extern "C" float fn_800DD4CC(cFielder*);
extern "C" float fn_800DD684(cPlayer*, cFielder*);
extern "C" float fn_800DD294(cFielder*);
extern "C" float fn_800DD37C(cFielder*);
float FarTo(cPlayer*, cPlayer*);
float FarToBall(cPlayer*);
float FarToFormationPosition(cFielder*);
float FarToMyGoalie(cPlayer*);
float FarToMyNet(cPlayer*);
extern "C" float fn_800D93F4(cFielder*);
extern "C" float fn_800D924C(cFielder*);
extern "C" float fn_800DE994(cBall*, cTeam*);
float FarToTheirGoalie(cPlayer*);
float FarToTheirNet(cPlayer*);
extern "C" float fn_800E7F48(bool);
extern "C" void fn_800E3EF8(void*, UnidentifiedFielderIterator*);
extern "C" UnidentifiedFielderIterator* fn_800E3E68(void*, UnidentifiedFielderIterator*);
extern "C" bool fn_800E3EDC(void*, UnidentifiedFielderIterator*);
extern "C" UnidentifiedFielderIterator* fn_800E3C74(void*, cTeam*);
float FielderType(cPlayer*);
extern "C" float fn_800D7A70(cFielder*);
extern "C" float fn_800D9FC8(cFielder*);
extern "C" void fn_80313FA0(UnidentifiedFuzzyRuntimeBase*, bool, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_800E3B34(void*, cBall*, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_800E8CB8(void*, cPlayer*, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_8031423C(UnidentifiedFuzzyRuntimeBase*, const Variant&, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_800E3A84(void*, cPlayer*, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_800E8D68(UnidentifiedFuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_800E8D6C(UnidentifiedFuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_800E8D70(UnidentifiedFuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_800E8D74(UnidentifiedFuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_800E8D78(void*, UnidentifiedRuntimeFielderReference*, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_80314034(UnidentifiedFuzzyRuntimeBase*, float, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_803140CC(UnidentifiedFuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_803141F4(UnidentifiedFuzzyRuntimeBase*, const Variant&, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void fn_80314160(UnidentifiedFuzzyRuntimeBase*, unsigned long, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" UnidentifiedVariant_80054AB8* fn_800E7F60(UnidentifiedFuzzyRuntime*, bool, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E8E38(UnidentifiedFuzzyRuntime*, cPlayer*, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E3958(UnidentifiedFuzzyRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E35D4(UnidentifiedFuzzyRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E3700(UnidentifiedFuzzyRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E8090(UnidentifiedFuzzyRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E382C(UnidentifiedFuzzyRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E82E8(UnidentifiedFuzzyRuntime*, float, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E8F8C(UnidentifiedFuzzyRuntime*, UnidentifiedRuntimeFielderReference*, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E8414(UnidentifiedFuzzyRuntime*, float, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E81BC(UnidentifiedFuzzyRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E8540(UnidentifiedFuzzyRuntime*, UnidentifiedVariant_80054AB8*, float);
extern "C" UnidentifiedVariant_80054AB8* fn_800E8B80(UnidentifiedFuzzyRuntime*, unsigned long, float);
extern "C" void fn_80314438(void*, UnidentifiedVariant_80054AB8*);
extern "C" void fn_80314434(void*, UnidentifiedVariant_80054AB8*, float);
extern "C" float fn_800E8CAC(void*, float, bool);
extern "C" float fn_80314444(void*, float, bool);
extern "C" void* fn_8031443C(void*, void*, bool);
extern "C" float fn_80314428(UnidentifiedFuzzyRuntimeBase*);
extern "C" cBall* fn_800E34E4();
extern "C" cPlayer* fn_800E34D8();
extern "C" void* fn_800E34EC();
extern "C" void* fn_800E90EC(void*, cPlayer*);
extern "C" void* fn_800E9194(void*, cPlayer*);
extern "C" void* fn_800E923C(void*, cTeam*);
extern "C" int fn_803147A4(UnidentifiedFuzzyRuntimeBase*);
extern "C" Variant* fn_80314830(UnidentifiedFuzzyRuntimeBase*, unsigned long);
extern "C" float fn_80314690(UnidentifiedFuzzyRuntimeBase*, unsigned long);
extern "C" Goalie* fn_800D66A0(cFielder*);
float GoalieOutOfPosition(cFielder*);
float GoalieType(cPlayer*);
extern "C" float fn_800DF888(cTeam*);
extern "C" float fn_800D763C(cFielder*);
extern "C" float fn_800D76B8(cFielder*);
extern "C" unsigned long fn_800E3FDC(const char*);
float High(cBall*);
extern "C" float fn_800D7734(cFielder*);
float InBetweenMyNetAnd(cFielder*, cFielder*);
extern "C" float fn_800DC19C(cFielder*, cBall*);
extern "C" float fn_800DBEF4(cFielder*, cFielder*);
extern "C" float fn_800DC434(cFielder*, cBall*);
float Incapacitated(cPlayer*);
float InControlOfBall(cFielder*);
float InDefensiveZone(cPlayer*);
float InDefensiveZoneOfPlayer(cBall*, cPlayer*);
float InFrontOfMyNet(cFielder*);
float InFrontOfTheirNet(cFielder*);
extern "C" float fn_800DA518(cFielder*);
extern "C" float fn_800E0470(cPlayer*);
extern "C" float fn_800E05A4(cBall*, cPlayer*);
float InOffensiveZone(cPlayer*);
float InOffensiveZoneOfPlayer(cBall*, cPlayer*);
float InPassingLane(cFielder*);
extern "C" float fn_800D74D8(cFielder*);
extern "C" float fn_803144BC(float, float, float);
extern "C" float fn_803144C8(float, float, float);
extern "C" float fn_80314504(float, float, float, float, float);
extern "C" float fn_80314538(float, float, float, float, float);
float Invincible(cFielder*);
extern "C" float fn_800D9D04(cFielder*);
extern "C" bool fn_80314798(void*);
extern "C" bool fn_803145C8(UnidentifiedFuzzyRuntimeBase*, unsigned long);
extern "C" float fn_800D6AF0(cFielder*);
float LastBallOwner(cPlayer*);
extern "C" float fn_800D79F4(int, cFielder*);
extern "C" float fn_800D8834(cFielder*, int);
float Loose(cTeam*);
float Losing(cTeam*);
extern "C" void* fn_800D673C(void*);
float Marking(cFielder*, cPlayer*);
extern "C" cFielder* fn_800D6734(cFielder*);
float Midfield(cFielder*);
float Moderate(cTeam*);
float NearTo(cPlayer*, cPlayer*);
float NearToBall(cPlayer*);
extern "C" float fn_800DD494(cFielder*);
extern "C" float fn_800DD5C4(cPlayer*, cFielder*);
float NearToFormationPosition(cFielder*);
float NearToMyGoalie(cPlayer*);
float NearToMyNet(cPlayer*);
extern "C" float fn_800D9368(cFielder*);
extern "C" float fn_800DD234(cFielder*);
extern "C" float fn_800DD31C(cFielder*);
extern "C" float fn_800D91BC(cFielder*);
extern "C" float fn_800DE8CC(cBall*, cTeam*);
float NearToTheirGoalie(cPlayer*);
float NearToTheirNet(cPlayer*);
extern "C" float fn_80314448(float, float, float);
float Offensive(cTeam*);
extern "C" float fn_800D78C4(cFielder*);
extern "C" float fn_800DACF4(cPlayer*);
extern "C" float fn_800DAD3C(cBall*);
extern "C" float fn_800DA91C(cFielder*);
extern "C" float fn_800DF028(cFielder*);
extern "C" float fn_800DED3C(cFielder*);
extern "C" float fn_800D6D14(cPlayer*, cPlayer*);
extern "C" float fn_800D6D78(cPlayer*);
extern "C" float fn_800D6CD4(cPlayer*, cPlayer*);
float OnTheGround(cPlayer*);
extern "C" float fn_800DBAB0(cFielder*);
float OpenTo(cPlayer*, cPlayer*);
float LikelyToScore(cFielder*);
extern "C" float fn_800DBB88(cFielder*);
extern "C" Goalie* fn_800D66C4(cFielder*);
extern "C" UnidentifiedFielderIterator* fn_800E3D00(void*, cFielder*);
extern "C" cTeam* fn_800D6688(cFielder*);
extern "C" float fn_800DE71C(cPlayer*);
float Ownerless(cBall*);
extern "C" float fn_800E3FE0();
extern "C" float fn_800E3FE4();
float Passive(cTeam*);
extern "C" float fn_800DF2C0(cFielder*);
extern "C" cPlayer* fn_800D6744(cBall*);
extern "C" float fn_800E3FE8();
extern "C" float fn_800D782C(cFielder*);
extern "C" float fn_800D7878(cFielder*);
extern "C" float fn_800D9480(cFielder*);
extern "C" float fn_800D88B4(cFielder*);
float RandomChance(float);
float ReallyCloseToBall(cPlayer*);
float ReallyHigh(cBall*);
extern "C" float fn_800DF118(cFielder*);
extern "C" float fn_800DF474(cFielder*);
float ReceivingPass(cFielder*);
extern "C" float fn_800DF1B8(cFielder*);
extern "C" float fn_800DF0B8(cFielder*);
extern "C" float fn_800DF390(cPlayer*);
extern "C" float fn_800D9D78(cPlayer*);
float GenerateFilteredRandom();
extern "C" float fn_800D6BD8(cFielder*);
float SeparatingFrom(cPlayer*, cPlayer*);
float SeparatingFrom(cPlayer*, cBall*);
extern "C" void fn_80314744(UnidentifiedFuzzyRuntimeBase*, int);
extern "C" void fn_80314750(void*, UnidentifiedTransitionReference*, const char*);
extern "C" void fn_800E92E4(UnidentifiedScriptMachine*, const char*);
extern "C" float fn_803146E8(UnidentifiedFuzzyRuntimeBase*, unsigned long, float);
extern "C" float fn_800E3FEC();
extern "C" float fn_800D77B0(cFielder*);
float Stalling(cTeam*);
extern "C" float fn_800D6A90(cFielder*);
extern "C" cFielder* fn_800DF790(cTeam*);
float StrategicBallOwner(cFielder*);
extern "C" float fn_800D8970(cFielder*);
float Striker(cFielder*);
extern "C" float fn_800DA0C8(cFielder*);
extern "C" float fn_800DD7F4(cFielder*);
extern "C" float fn_800DE7D8(Goalie*);
extern "C" float fn_800DD944(cPlayer*);
extern "C" float fn_800DD99C(cFielder*);
extern "C" UnidentifiedFielderIterator* fn_800E3D98(void*, cFielder*);
extern "C" cTeam* fn_800D6670(cFielder*);
extern "C" char fn_80312358(void*, char);
float Tied(cTeam*);
float TimeCloseToOver(cGame*);
float TimeFarFromOver(cGame*);
float TimeNearlyOver(cGame*);
extern "C" float fn_800D9DD8(cPlayer*);
extern "C" float fn_800DE40C(cPlayer*, cPlayer*);
float UserControlled(cFielder*);
float UserControlledT(cTeam*);
extern "C" bool fn_8031462C(UnidentifiedFuzzyRuntimeBase*, unsigned long);
extern "C" float fn_800DBB0C(cFielder*);
extern "C" float fn_800DEB04(cFielder*);
extern "C" float fn_800DEBBC(cPlayer*);
extern "C" float fn_800DEAB4(cFielder*);
extern "C" float fn_800DEBF4(cFielder*);
extern "C" float fn_800DEC88(cFielder*);
float Winger(cFielder*);
float Winning(cTeam*);
extern "C" bool fn_800E7EB4(InterpreterCore*);

extern "C" void fn_800A695C();
extern "C" float fn_80314444(void*, float, bool);
extern "C" void fn_803140CC(UnidentifiedFuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void* fn_80312E0C(void*, const Variant&);
extern "C" void fn_800B6A1C(
    UnidentifiedVariant_80054AB8*, int, const Variant&);

char lbl_80503EEC[] = "art/Scripts/FuzzyAI.byte_code";
char lbl_80503F0C[] = "Direction";
char lbl_80503F18[] = "Distance";
char* lbl_806DC3F0 = lbl_80503EEC;
char lbl_806DC3F4[] = "Speed";
char lbl_806DC3FC[] = "Target";
char lbl_806DC408[] = "Powerup";
char lbl_806DC410[] = "Lob";

SlotPool<UnidentifiedFielderIterator> lbl_8056DBB0(16, 16);

UnidentifiedFuzzyRuntime::UnidentifiedFuzzyRuntime()
    : UnidentifiedFuzzyRuntimeBase(0)
{
    if (lbl_806E20B0.mHead == 0)
    {
        UnidentifiedVirtual15();
    }
}

UnidentifiedFuzzyRuntime::~UnidentifiedFuzzyRuntime()
{
    lbl_8056DBB0.FreeBlocks();
}

extern "C" UnidentifiedFuzzyRuntimeBase* fn_800E30A8(cFielder* pFielder)
{
    return pFielder->fn_8002E198();
}

extern "C" void fn_800E30AC()
{
    fn_800A695C();
}

void UnidentifiedFuzzyRuntime::UnidentifiedVirtual15()
{
    UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual15();
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_806DC3F4, 13));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_806DC3FC, 14));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_806DC408, 15));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_806DC410, 16));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_80503F0C, 17));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_80503F18, 18));
}

extern "C" const char* fn_800E3198()
{
    return lbl_806DC3F0;
}

float UnidentifiedFuzzyRuntime::UnidentifiedVirtual8()
{
    float result = UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual8();
    SkillTweaks* value = 0;

    switch (mValue->GetType())
    {
    case FT_PLAYER:
        value = fn_800A636C(mValue->GetPlayer()->m_pTeam);
        break;
    case FT_TEAM:
        value = fn_800A636C(mValue->GetTeam());
        break;
    case FT_GAME:
        value = 0;
        break;
    }

    if (value != 0)
    {
        UnidentifiedActionQueue* collection = mCollection.mHead->mQueue;
        collection->fn_80310034(value->GetDecisionWeights(), 4);
    }

    return result;
}

UnidentifiedVariant_80054AB8*
UnidentifiedFuzzyRuntime::UnidentifiedVirtual9()
{
    return UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual9();
}

void UnidentifiedFuzzyRuntime::UnidentifiedVirtual12(
    UnidentifiedVariant_80054AB8* action)
{
    UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual12(action);
}

extern "C" cPlayer* fn_800E34D8()
{
    return g_pBall->m_pOwner;
}

extern "C" cBall* fn_800E34E4()
{
    return g_pBall;
}

extern "C" void* fn_800E34EC()
{
    return g_pGame;
}

extern "C" float fn_800E7EF4(
    void*, UnidentifiedVariant_80054AB8* value)
{
    if (value->ExtraData.IsSet(4))
    {
        return value->ExtraData.Get(4)->mData.f;
    }

    return 0.0f;
}

float UnidentifiedFuzzyRuntime::fn_800E34F4(unsigned long hash)
{
    SkillTweaks* teamContext = 0;
    cFielder* fielder = 0;

    switch (mValue->GetType())
    {
    case FT_PLAYER:
    {
        cPlayer* player = mValue->GetPlayer();
        teamContext = fn_800A636C(player->GetTeam());
        if (player->m_eClassType == FIELDER)
        {
            fielder = static_cast<cFielder*>(player);
        }
        break;
    }
    case FT_TEAM:
    {
        cTeam* team = mValue->GetTeam();
        teamContext = fn_800A636C(team);
        fielder = team->GetCaptain();
        break;
    }
    }

    float result = 1.0f;
    teamContext->GetSkillValue(hash, &result, fielder);
    return result;
}

extern "C" UnidentifiedVariant_80054AB8* fn_800E35D4(
    UnidentifiedFuzzyRuntime* runtime, int value, float confidence)
{
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(FT_INT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = runtime->GetInstructionOffset() + 1;
    return runtime->UnidentifiedReturn(result, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* fn_800E3700(
    UnidentifiedFuzzyRuntime* runtime, int value, float confidence)
{
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(FT_INT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = runtime->GetInstructionOffset() + 1;
    return runtime->UnidentifiedReturn(result, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* fn_800E382C(
    UnidentifiedFuzzyRuntime* runtime, int value, float confidence)
{
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(FT_INT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = runtime->GetInstructionOffset() + 1;
    return runtime->UnidentifiedReturn(result, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* fn_800E3958(
    UnidentifiedFuzzyRuntime* runtime, int value, float confidence)
{
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(FT_INT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = runtime->GetInstructionOffset() + 1;
    return runtime->UnidentifiedReturn(result, confidence);
}

extern "C" void fn_800E3A84(
    void*, cPlayer* value, unsigned long descriptor,
    UnidentifiedVariant_80054AB8* runtime)
{
    int index = fn_80312208(descriptor);
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = value;
    runtime->ExtraData.Set(index, variant);
}

extern "C" void fn_800E3B34(
    void*, cBall* value, unsigned long descriptor,
    UnidentifiedVariant_80054AB8* runtime)
{
    int index = fn_80312208(descriptor);
    FuzzyVariant variant;
    variant.mType = FT_BALL;
    variant.mData.pointer = value;
    runtime->ExtraData.Set(index, variant);
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

extern "C" unsigned long fn_800E3FDC(const char* value)
{
    return nlStringHash(value);
}

extern "C" float fn_800DFF1C();
extern "C" float fn_800DFF60();
extern "C" float fn_800E00F8();
extern "C" float fn_800E0034();

extern "C" float fn_800E3FE0()
{
    return fn_800DFF1C();
}

extern "C" float fn_800E3FE4()
{
    return fn_800DFF60();
}

extern "C" float fn_800E3FE8()
{
    return fn_800E00F8();
}

extern "C" float fn_800E3FEC()
{
    return fn_800E0034();
}

extern "C" void fn_800E3EF8(
    void*, UnidentifiedFielderIterator* entry)
{
    lbl_8056DBB0.Free(entry);
}

extern "C" bool fn_800E3EDC(
    void*, UnidentifiedFielderIterator* iterator)
{
    return iterator->mCurrent < iterator->mEnd;
}

extern "C" UnidentifiedFielderIterator* fn_800E3C74(
    void*, cTeam* team)
{
    UnidentifiedFielderIterator* iterator = 0;
    lbl_8056DBB0.Allocate(iterator);
    iterator->mCurrent = 0;
    iterator->mEnd = 4;
    iterator->mTeam = team;
    iterator->mSkip = 0;
    return iterator;
}

extern "C" UnidentifiedFielderIterator* fn_800E3D00(
    void*, cFielder* fielder)
{
    cTeam* team = fielder->m_pTeam->GetOtherTeam();
    UnidentifiedFielderIterator* iterator = 0;
    lbl_8056DBB0.Allocate(iterator);
    iterator->mCurrent = 0;
    iterator->mEnd = 4;
    iterator->mTeam = team;
    iterator->mSkip = 0;
    return iterator;
}

extern "C" UnidentifiedFielderIterator* fn_800E3D98(
    void*, cFielder* fielder)
{
    UnidentifiedFielderIterator* iterator = 0;
    cTeam* team = fielder->m_pTeam;
    lbl_8056DBB0.Allocate(iterator);
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

extern "C" UnidentifiedFielderIterator* fn_800E3E68(
    void*, UnidentifiedFielderIterator* iterator)
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

extern "C" cFielder* fn_800E3F10(
    void*, UnidentifiedFielderIterator* iterator)
{
    return iterator->mTeam->GetFielder(iterator->mCurrent);
}

extern "C" cFielder* fn_800E3F1C(
    void*, UnidentifiedFielderIterator* iterator)
{
    return iterator->mTeam->GetFielder(iterator->mCurrent);
}

extern "C" UnidentifiedFielderInput* fn_800E3F28(
    void*, UnidentifiedFielderIterator* iterator)
{
    cFielder* fielder = iterator->mTeam->GetFielder(iterator->mCurrent);
    return fielder->mUnidentified428;
}

extern "C" bool fn_800E7EB4(InterpreterCore* value)
{
    return value->m_RunState == 3;
}

extern "C" float fn_800E7ECC(void*, Variant* value)
{
    return value->mData.f;
}

extern "C" unsigned long fn_800E7ED4(void*, Variant* value)
{
    return value->mData.u;
}

extern "C" unsigned long fn_800E7EDC(void*, Variant* value)
{
    return value->mData.u;
}

extern "C" unsigned long fn_800E7EE4(void*, Variant* value)
{
    return value->mData.u;
}

extern "C" float fn_800E7EEC(void*, Variant* value)
{
    return value->mData.f;
}

extern "C" UnidentifiedVariant_80054AB8* fn_800E7F60(
    UnidentifiedFuzzyRuntime* runtime, bool value, float confidence)
{
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(FT_BOOL, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = runtime->GetInstructionOffset() + 1;
    return runtime->UnidentifiedReturn(result, confidence);
}

extern "C" float fn_800E7F48(bool value)
{
    return value ? 1.0f : 0.0f;
}

UnidentifiedVariant_80054AB8* UnidentifiedFuzzyRuntimeBase::UnidentifiedReturn(
    UnidentifiedVariant_80054AB8* value, float)
{
    return value;
}

extern "C" UnidentifiedVariant_80054AB8* fn_800E8090(
    UnidentifiedFuzzyRuntime* runtime, int value, float confidence)
{
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(FT_INT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = runtime->GetInstructionOffset() + 1;
    return runtime->UnidentifiedReturn(result, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* fn_800E81BC(
    UnidentifiedFuzzyRuntime* runtime, int value, float confidence)
{
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(FT_INT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = runtime->GetInstructionOffset() + 1;
    return runtime->UnidentifiedReturn(result, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* fn_800E82E8(
    UnidentifiedFuzzyRuntime* runtime, float value, float confidence)
{
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(FT_FLOAT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = runtime->GetInstructionOffset() + 1;
    return runtime->UnidentifiedReturn(result, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* fn_800E8414(
    UnidentifiedFuzzyRuntime* runtime, float value, float confidence)
{
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(FT_FLOAT, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = runtime->GetInstructionOffset() + 1;
    return runtime->UnidentifiedReturn(result, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* fn_800E8540(
    UnidentifiedFuzzyRuntime* runtime,
    UnidentifiedVariant_80054AB8* value, float confidence)
{
    UnidentifiedVariant_80054AB8* result;
    lbl_805842C8.Allocate(result);
    result = new (result) UnidentifiedVariant_80054AB8(value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = runtime->GetInstructionOffset() + 1;
    return runtime->UnidentifiedReturn(result, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* fn_800E8B80(
    UnidentifiedFuzzyRuntime* runtime, unsigned long value, float confidence)
{
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(FT_U32, value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = runtime->GetInstructionOffset() + 1;
    return runtime->UnidentifiedReturn(result, confidence);
}

extern "C" unsigned long fn_800E8CB0(void*, Variant* value)
{
    return value->mData.u;
}

extern "C" void fn_800E8CB8(
    void*, cPlayer* value, unsigned long descriptor,
    UnidentifiedVariant_80054AB8* runtime)
{
    int index = fn_80312208(descriptor);
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = value;
    runtime->ExtraData.Set(index, variant);
}

extern "C" float fn_800E8CAC(void* runtime, float value, bool flag)
{
    return fn_80314444(runtime, value, flag);
}

extern "C" void fn_800E8D68(UnidentifiedFuzzyRuntimeBase* runtime, int value, unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    fn_803140CC(runtime, value, hash, action);
}

extern "C" void fn_800E8D6C(UnidentifiedFuzzyRuntimeBase* runtime, int value, unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    fn_803140CC(runtime, value, hash, action);
}

extern "C" void fn_800E8D70(UnidentifiedFuzzyRuntimeBase* runtime, int value, unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    fn_803140CC(runtime, value, hash, action);
}

extern "C" void fn_800E8D74(UnidentifiedFuzzyRuntimeBase* runtime, int value, unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    fn_803140CC(runtime, value, hash, action);
}

extern "C" void fn_800E8D78(
    void*, UnidentifiedRuntimeFielderReference* value,
    unsigned long descriptor, UnidentifiedVariant_80054AB8* runtime)
{
    cFielder* fielder = value->mTeam->GetFielder(value->mIndex);
    int index = fn_80312208(descriptor);
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = fielder;
    runtime->ExtraData.Set(index, variant);
}

extern "C" UnidentifiedVariant_80054AB8* fn_800E8E38(
    UnidentifiedFuzzyRuntime* runtime, cPlayer* value, float confidence)
{
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(value);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = runtime->GetInstructionOffset() + 1;
    return runtime->UnidentifiedReturn(result, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* fn_800E8F8C(
    UnidentifiedFuzzyRuntime* runtime,
    UnidentifiedRuntimeFielderReference* value, float confidence)
{
    cFielder* fielder = value->mTeam->GetFielder(value->mIndex);
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(fielder);
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = runtime->GetInstructionOffset() + 1;
    return runtime->UnidentifiedReturn(result, confidence);
}

extern "C" void* fn_800E90EC(void* runtime, cPlayer* value)
{
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = value;
    return fn_80312E0C(runtime, variant);
}

extern "C" void* fn_800E9194(void* runtime, cPlayer* value)
{
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = value;
    return fn_80312E0C(runtime, variant);
}

extern "C" void* fn_800E923C(void* runtime, cTeam* value)
{
    FuzzyVariant variant;
    variant.mType = FT_TEAM;
    variant.mData.pTeam = value;
    return fn_80312E0C(runtime, variant);
}

extern "C" void fn_800E92E4(
    UnidentifiedScriptMachine* state, const char* name)
{
    UnidentifiedStringHash value(name);
    state->mUnidentified00C.mUnidentifiedHash = value.mUnidentifiedHash;
    state->mUnidentified00C.mUnidentifiedFunction = value.mUnidentifiedFunction;
}

void UnidentifiedFuzzyRuntime::DoFunctionCall(unsigned int function)
{
    switch (function)
    {
    case 0:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = AbleToInterceptBall(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 1:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D82C0(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 2:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D7AB8(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 3:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D7B00(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 4:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D85F8(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 5:
    {
        int arg1 = (int)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800D8764(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 6:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D84F8(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 7:
    {
        cFielder* arg1 = (cFielder*)m_SP[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800D7988(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 8:
    {
        bool arg3 = m_SP[-1] != 0;
        const char* arg2 = (const char*)m_SP[-2];
        int arg1 = (int)m_SP[-3];
        UnidentifiedScriptMachine* arg0 = (UnidentifiedScriptMachine*)m_SP[-4];
        m_SP -= 4;
        fn_80318D34(arg0, arg1, arg2, arg3);
        break;
    }
    case 9:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = AggressiveT(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 10:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DA330(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 11:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DA310(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 12:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DD9C8(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 13:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = PlayerShotDistance(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 14:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DDD70(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 15:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D96F4(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 16:
    {
        void* arg0 = (void*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3BF4(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 17:
    {
        void* arg0 = (void*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3C04(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 18:
    {
        void* arg0 = (void*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3C0C(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 19:
    {
        void* arg0 = (void*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3BE4(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 20:
    {
        UnidentifiedFielderIterator* arg0 = (UnidentifiedFielderIterator*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3F10(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 21:
    {
        UnidentifiedFielderIterator* arg0 = (UnidentifiedFielderIterator*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3F1C(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 22:
    {
        UnidentifiedFielderIterator* arg0 = (UnidentifiedFielderIterator*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3F28(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 23:
    {
        void* arg0 = (void*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3BFC(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 24:
    {
        void* arg0 = (void*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3BEC(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 25:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3C5C(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 26:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3C44(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 27:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800E7ECC(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 28:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E7ED4(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 29:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3C14(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 30:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3C2C(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 31:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E8CB0(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 32:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E7EE4(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 33:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800E7EEC(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 34:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E7EDC(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 35:
    {
        UnidentifiedFuzzyRuntimeBase* arg0 = (UnidentifiedFuzzyRuntimeBase*)m_SP[-1];
        m_SP[-1] = (u32)fn_8031479C(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 36:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9B0C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 37:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9A38(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 38:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9B74(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 39:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9BDC(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 40:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9C24(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 41:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800E06F4(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 42:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF838(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 43:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = BallOwner(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 44:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = BallOwnerT(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 45:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D671C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 46:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = Captain(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 47:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D6708(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 48:
    {
        int arg1 = (int)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800D795C(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 49:
    {
        cBall* arg0 = (cBall*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF590(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 50:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DED80(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 51:
    {
        float arg2 = ((float*)m_SP)[-1];
        float arg1 = ((float*)m_SP)[-2];
        float arg0 = ((float*)m_SP)[-3];
        m_SP -= 2;
        ((float*)m_SP)[-1] = fn_80314494(arg0, arg1, arg2);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 52:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D674C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 53:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D8A9C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 54:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D8BAC(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 55:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = CloseTo(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 56:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = CloseToBall(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 57:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD45C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 58:
    {
        cFielder* arg1 = (cFielder*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DD504(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 59:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = CloseToFormationPosition(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 60:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = CloseToMyGoalie(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 61:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = CloseToMyNet(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 62:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D92DC(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 63:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = CloseToSideline(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 64:
    {
        cBall* arg0 = (cBall*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD2F4(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 65:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D912C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 66:
    {
        cTeam* arg1 = (cTeam*)m_SP[-1];
        cBall* arg0 = (cBall*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DE804(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 67:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = CloseToTheirGoalie(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 68:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = CloseToTheirNet(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 69:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = ClosingTo(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 70:
    {
        cBall* arg1 = (cBall*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = ClosingTo(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 71:
    {
        UnidentifiedVariant_80054AB8* arg0 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800E7EF4(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 72:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DA130(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 73:
    {
        float arg0 = ((float*)m_SP)[-1];
        m_SP -= 1;
        fn_803148C4(arg0);
        break;
    }
    case 74:
    {
        const char* arg0 = (const char*)m_SP[-1];
        m_SP -= 1;
        fn_803148D0(this, arg0);
        break;
    }
    case 75:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = Defence(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 76:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Defensive(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 77:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D7910(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 78:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Difficult(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 79:
    {
        unsigned long arg0 = (unsigned long)m_SP[-1];
        m_SP[-1] = (u32)fn_803147C0(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 80:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = DoingS2S(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 81:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DE4B0(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 82:
    {
        bool arg0 = m_SP[-1] != 0;
        m_SP -= 1;
        fn_80314740(this, arg0);
        break;
    }
    case 83:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DE1F0(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 84:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DDF54(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 85:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DE0A8(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 86:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD744(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 87:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = FallenDown(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 88:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DA050(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 89:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD4CC(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 90:
    {
        cFielder* arg1 = (cFielder*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DD684(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 91:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD294(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 92:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD37C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 93:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = FarTo(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 94:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = FarToBall(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 95:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = FarToFormationPosition(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 96:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = FarToMyGoalie(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 97:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = FarToMyNet(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 98:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D93F4(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 99:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D924C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 100:
    {
        cTeam* arg1 = (cTeam*)m_SP[-1];
        cBall* arg0 = (cBall*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DE994(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 101:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = FarToTheirGoalie(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 102:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = FarToTheirNet(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 103:
    {
        bool arg0 = m_SP[-1] != 0;
        ((float*)m_SP)[-1] = fn_800E7F48(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 104:
    {
        UnidentifiedFielderIterator* arg0 = (UnidentifiedFielderIterator*)m_SP[-1];
        m_SP -= 1;
        fn_800E3EF8(this, arg0);
        break;
    }
    case 105:
    {
        UnidentifiedFielderIterator* arg0 = (UnidentifiedFielderIterator*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3E68(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 106:
    {
        UnidentifiedFielderIterator* arg0 = (UnidentifiedFielderIterator*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3EDC(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 107:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3C74(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 108:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = FielderType(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 109:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D7A70(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 110:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9FC8(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 111:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        bool arg0 = m_SP[-3] != 0;
        m_SP -= 3;
        fn_80313FA0(this, arg0, arg1, arg2);
        break;
    }
    case 112:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        cBall* arg0 = (cBall*)m_SP[-3];
        m_SP -= 3;
        fn_800E3B34(this, arg0, arg1, arg2);
        break;
    }
    case 113:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        cPlayer* arg0 = (cPlayer*)m_SP[-3];
        m_SP -= 3;
        fn_800E8CB8(this, arg0, arg1, arg2);
        break;
    }
    case 114:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        const Variant& arg0 = *(const Variant*)m_SP[-3];
        m_SP -= 3;
        fn_8031423C(this, arg0, arg1, arg2);
        break;
    }
    case 115:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        cPlayer* arg0 = (cPlayer*)m_SP[-3];
        m_SP -= 3;
        fn_800E3A84(this, arg0, arg1, arg2);
        break;
    }
    case 116:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        int arg0 = (int)m_SP[-3];
        m_SP -= 3;
        fn_800E8D68(this, arg0, arg1, arg2);
        break;
    }
    case 117:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        int arg0 = (int)m_SP[-3];
        m_SP -= 3;
        fn_800E8D6C(this, arg0, arg1, arg2);
        break;
    }
    case 118:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        int arg0 = (int)m_SP[-3];
        m_SP -= 3;
        fn_800E8D70(this, arg0, arg1, arg2);
        break;
    }
    case 119:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        int arg0 = (int)m_SP[-3];
        m_SP -= 3;
        fn_800E8D74(this, arg0, arg1, arg2);
        break;
    }
    case 120:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        UnidentifiedRuntimeFielderReference* arg0 = (UnidentifiedRuntimeFielderReference*)m_SP[-3];
        m_SP -= 3;
        fn_800E8D78(this, arg0, arg1, arg2);
        break;
    }
    case 121:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        float arg0 = ((float*)m_SP)[-3];
        m_SP -= 3;
        fn_80314034(this, arg0, arg1, arg2);
        break;
    }
    case 122:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        int arg0 = (int)m_SP[-3];
        m_SP -= 3;
        fn_803140CC(this, arg0, arg1, arg2);
        break;
    }
    case 123:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        const Variant& arg0 = *(const Variant*)m_SP[-3];
        m_SP -= 3;
        fn_803141F4(this, arg0, arg1, arg2);
        break;
    }
    case 124:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        unsigned long arg0 = (unsigned long)m_SP[-3];
        m_SP -= 3;
        fn_80314160(this, arg0, arg1, arg2);
        break;
    }
    case 125:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = UnidentifiedVirtual11();
        break;
    }
    case 126:
    {
        float arg0 = ((float*)m_SP)[-1];
        ((float*)m_SP)[-1] = UnidentifiedVirtual10(arg0);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 127:
    {
        float arg0 = ((float*)m_SP)[-1];
        ((float*)m_SP)[-1] = UnidentifiedVirtual6(arg0);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 128:
    {
        bool arg3 = m_SP[-1] != 0;
        float arg2 = ((float*)m_SP)[-2];
        float arg1 = ((float*)m_SP)[-3];
        float arg0 = ((float*)m_SP)[-4];
        m_SP -= 3;
        ((float*)m_SP)[-1] = UnidentifiedVirtual7(arg0, arg1, arg2, arg3);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 129:
    {
        float arg1 = ((float*)m_SP)[-1];
        bool arg0 = m_SP[-2] != 0;
        m_SP -= 1;
        m_SP[-1] = (u32)fn_800E7F60(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 130:
    {
        float arg1 = ((float*)m_SP)[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)fn_800E8E38(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 131:
    {
        float arg1 = ((float*)m_SP)[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)fn_800E3958(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 132:
    {
        float arg1 = ((float*)m_SP)[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)fn_800E35D4(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 133:
    {
        float arg1 = ((float*)m_SP)[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)fn_800E3700(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 134:
    {
        float arg1 = ((float*)m_SP)[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)fn_800E8090(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 135:
    {
        float arg1 = ((float*)m_SP)[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)fn_800E382C(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 136:
    {
        float arg1 = ((float*)m_SP)[-1];
        float arg0 = ((float*)m_SP)[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)fn_800E82E8(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 137:
    {
        float arg1 = ((float*)m_SP)[-1];
        UnidentifiedRuntimeFielderReference* arg0 = (UnidentifiedRuntimeFielderReference*)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)fn_800E8F8C(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 138:
    {
        float arg1 = ((float*)m_SP)[-1];
        float arg0 = ((float*)m_SP)[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)fn_800E8414(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 139:
    {
        float arg1 = ((float*)m_SP)[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)fn_800E81BC(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 140:
    {
        float arg1 = ((float*)m_SP)[-1];
        UnidentifiedVariant_80054AB8* arg0 = (UnidentifiedVariant_80054AB8*)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)fn_800E8540(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 141:
    {
        float arg1 = ((float*)m_SP)[-1];
        unsigned long arg0 = (unsigned long)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)fn_800E8B80(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 142:
    {
        UnidentifiedVariant_80054AB8* arg0 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        m_SP -= 1;
        fn_80314438(this, arg0);
        break;
    }
    case 143:
    {
        float arg1 = ((float*)m_SP)[-1];
        UnidentifiedVariant_80054AB8* arg0 = (UnidentifiedVariant_80054AB8*)m_SP[-2];
        m_SP -= 2;
        fn_80314434(this, arg0, arg1);
        break;
    }
    case 144:
    {
        bool arg1 = m_SP[-1] != 0;
        float arg0 = ((float*)m_SP)[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800E8CAC(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 145:
    {
        bool arg1 = m_SP[-1] != 0;
        float arg0 = ((float*)m_SP)[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_80314444(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 146:
    {
        bool arg1 = m_SP[-1] != 0;
        void* arg0 = (void*)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)fn_8031443C(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 147:
    {
        float arg1 = ((float*)m_SP)[-1];
        float arg0 = ((float*)m_SP)[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = UnidentifiedVirtual3(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 148:
    {
        ++m_SP;
        m_SP[-1] = (u32)UnidentifiedVirtual9();
        break;
    }
    case 149:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = UnidentifiedVirtual8();
        break;
    }
    case 150:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = fn_80314428(this);
        break;
    }
    case 151:
    {
        ++m_SP;
        m_SP[-1] = (u32)fn_800E34E4();
        break;
    }
    case 152:
    {
        ++m_SP;
        m_SP[-1] = (u32)fn_800E34D8();
        break;
    }
    case 153:
    {
        ++m_SP;
        m_SP[-1] = (u32)fn_800E34EC();
        break;
    }
    case 154:
    {
        float arg1 = ((float*)m_SP)[-1];
        float arg0 = ((float*)m_SP)[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = UnidentifiedVirtual4(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 155:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E90EC(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 156:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E9194(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 157:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E923C(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 158:
    {
        float arg0 = ((float*)m_SP)[-1];
        ((float*)m_SP)[-1] = UnidentifiedVirtual5(arg0);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 159:
    {
        UnidentifiedVariant_80054AB8* arg0 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        m_SP -= 1;
        UnidentifiedVirtual12(arg0);
        break;
    }
    case 160:
    {
        ++m_SP;
        m_SP[-1] = (u32)fn_803147A4(this);
        break;
    }
    case 161:
    {
        unsigned long arg0 = (unsigned long)m_SP[-1];
        m_SP[-1] = (u32)fn_80314830(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 162:
    {
        unsigned long arg0 = (unsigned long)m_SP[-1];
        ((float*)m_SP)[-1] = fn_80314690(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 163:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D66A0(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 164:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = GoalieOutOfPosition(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 165:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = GoalieType(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 166:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF888(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 167:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D763C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 168:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D76B8(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 169:
    {
        const char* arg0 = (const char*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3FDC(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 170:
    {
        cBall* arg0 = (cBall*)m_SP[-1];
        ((float*)m_SP)[-1] = High(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 171:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D7734(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 172:
    {
        cFielder* arg1 = (cFielder*)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = InBetweenMyNetAnd(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 173:
    {
        cBall* arg1 = (cBall*)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DC19C(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 174:
    {
        cFielder* arg1 = (cFielder*)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DBEF4(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 175:
    {
        cBall* arg1 = (cBall*)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DC434(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 176:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = Incapacitated(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 177:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = InControlOfBall(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 178:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = InDefensiveZone(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 179:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cBall* arg0 = (cBall*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = InDefensiveZoneOfPlayer(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 180:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = InFrontOfMyNet(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 181:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = InFrontOfTheirNet(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 182:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DA518(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 183:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800E0470(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 184:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cBall* arg0 = (cBall*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800E05A4(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 185:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = InOffensiveZone(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 186:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cBall* arg0 = (cBall*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = InOffensiveZoneOfPlayer(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 187:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = InPassingLane(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 188:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D74D8(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 189:
    {
        float arg2 = ((float*)m_SP)[-1];
        float arg1 = ((float*)m_SP)[-2];
        float arg0 = ((float*)m_SP)[-3];
        m_SP -= 2;
        ((float*)m_SP)[-1] = fn_803144BC(arg0, arg1, arg2);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 190:
    {
        float arg2 = ((float*)m_SP)[-1];
        float arg1 = ((float*)m_SP)[-2];
        float arg0 = ((float*)m_SP)[-3];
        m_SP -= 2;
        ((float*)m_SP)[-1] = fn_803144C8(arg0, arg1, arg2);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 191:
    {
        float arg4 = ((float*)m_SP)[-1];
        float arg3 = ((float*)m_SP)[-2];
        float arg2 = ((float*)m_SP)[-3];
        float arg1 = ((float*)m_SP)[-4];
        float arg0 = ((float*)m_SP)[-5];
        m_SP -= 4;
        ((float*)m_SP)[-1] = fn_80314504(arg0, arg1, arg2, arg3, arg4);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 192:
    {
        float arg4 = ((float*)m_SP)[-1];
        float arg3 = ((float*)m_SP)[-2];
        float arg2 = ((float*)m_SP)[-3];
        float arg1 = ((float*)m_SP)[-4];
        float arg0 = ((float*)m_SP)[-5];
        m_SP -= 4;
        ((float*)m_SP)[-1] = fn_80314538(arg0, arg1, arg2, arg3, arg4);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 193:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = Invincible(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 194:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9D04(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 195:
    {
        ++m_SP;
        m_SP[-1] = (u32)fn_80314798(this);
        break;
    }
    case 196:
    {
        unsigned long arg0 = (unsigned long)m_SP[-1];
        m_SP[-1] = (u32)fn_803145C8(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 197:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D6AF0(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 198:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = LastBallOwner(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 199:
    {
        cFielder* arg1 = (cFielder*)m_SP[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800D79F4(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 200:
    {
        int arg1 = (int)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800D8834(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 201:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Loose(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 202:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Losing(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 203:
    {
        void* arg0 = (void*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D673C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 204:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = Marking(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 205:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D6734(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 206:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = Midfield(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 207:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Moderate(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 208:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = NearTo(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 209:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = NearToBall(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 210:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD494(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 211:
    {
        cFielder* arg1 = (cFielder*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DD5C4(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 212:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = NearToFormationPosition(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 213:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = NearToMyGoalie(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 214:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = NearToMyNet(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 215:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9368(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 216:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD234(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 217:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD31C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 218:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D91BC(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 219:
    {
        cTeam* arg1 = (cTeam*)m_SP[-1];
        cBall* arg0 = (cBall*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DE8CC(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 220:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = NearToTheirGoalie(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 221:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = NearToTheirNet(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 222:
    {
        float arg2 = ((float*)m_SP)[-1];
        float arg1 = ((float*)m_SP)[-2];
        float arg0 = ((float*)m_SP)[-3];
        m_SP -= 2;
        ((float*)m_SP)[-1] = fn_80314448(arg0, arg1, arg2);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 223:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Offensive(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 224:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D78C4(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 225:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DACF4(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 226:
    {
        cBall* arg0 = (cBall*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DAD3C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 227:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DA91C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 228:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF028(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 229:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DED3C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 230:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800D6D14(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 231:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D6D78(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 232:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800D6CD4(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 233:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = OnTheGround(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 234:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DBAB0(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 235:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = OpenTo(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 236:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = LikelyToScore(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 237:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DBB88(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 238:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D66C4(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 239:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3D00(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 240:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D6688(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 241:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DE71C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 242:
    {
        cBall* arg0 = (cBall*)m_SP[-1];
        ((float*)m_SP)[-1] = Ownerless(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 243:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = fn_800E3FE0();
        break;
    }
    case 244:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = fn_800E3FE4();
        break;
    }
    case 245:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Passive(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 246:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF2C0(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 247:
    {
        cBall* arg0 = (cBall*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D6744(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 248:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = fn_800E3FE8();
        break;
    }
    case 249:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D782C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 250:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D7878(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 251:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9480(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 252:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D88B4(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 253:
    {
        float arg0 = ((float*)m_SP)[-1];
        ((float*)m_SP)[-1] = RandomChance(arg0);
        if (fn_800E7EB4(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 254:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = ReallyCloseToBall(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 255:
    {
        cBall* arg0 = (cBall*)m_SP[-1];
        ((float*)m_SP)[-1] = ReallyHigh(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 256:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF118(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 257:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF474(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 258:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = ReceivingPass(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 259:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF1B8(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 260:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF0B8(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 261:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF390(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 262:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9D78(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 263:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = GenerateFilteredRandom();
        break;
    }
    case 264:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D6BD8(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 265:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = SeparatingFrom(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 266:
    {
        cBall* arg1 = (cBall*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = SeparatingFrom(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 267:
    {
        int arg0 = (int)m_SP[-1];
        m_SP -= 1;
        fn_80314744(this, arg0);
        break;
    }
    case 268:
    {
        const char* arg1 = (const char*)m_SP[-1];
        UnidentifiedTransitionReference* arg0 = (UnidentifiedTransitionReference*)m_SP[-2];
        m_SP -= 2;
        fn_80314750(this, arg0, arg1);
        break;
    }
    case 269:
    {
        const char* arg1 = (const char*)m_SP[-1];
        UnidentifiedScriptMachine* arg0 = (UnidentifiedScriptMachine*)m_SP[-2];
        m_SP -= 2;
        fn_800E92E4(arg0, arg1);
        break;
    }
    case 270:
    {
        float arg1 = ((float*)m_SP)[-1];
        unsigned long arg0 = (unsigned long)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_803146E8(this, arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 271:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = fn_800E3FEC();
        break;
    }
    case 272:
    {
        unsigned long arg0 = (unsigned long)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800E34F4(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 273:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D77B0(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 274:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Stalling(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 275:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D6A90(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 276:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        m_SP[-1] = (u32)fn_800DF790(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 277:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = StrategicBallOwner(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 278:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D8970(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 279:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = Striker(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 280:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DA0C8(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 281:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD7F4(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 282:
    {
        Goalie* arg0 = (Goalie*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DE7D8(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 283:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD944(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 284:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD99C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 285:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3D98(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 286:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D6670(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 287:
    {
        char arg0 = (char)m_SP[-1];
        m_SP[-1] = (unsigned char)fn_80312358(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (unsigned char)arg0;
        }
        break;
    }
    case 288:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Tied(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 289:
    {
        cGame* arg0 = (cGame*)m_SP[-1];
        ((float*)m_SP)[-1] = TimeCloseToOver(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 290:
    {
        cGame* arg0 = (cGame*)m_SP[-1];
        ((float*)m_SP)[-1] = TimeFarFromOver(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 291:
    {
        cGame* arg0 = (cGame*)m_SP[-1];
        ((float*)m_SP)[-1] = TimeNearlyOver(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 292:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9DD8(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 293:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DE40C(arg0, arg1);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 294:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = UserControlled(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 295:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = UserControlledT(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 296:
    {
        unsigned long arg0 = (unsigned long)m_SP[-1];
        m_SP[-1] = (u32)fn_8031462C(this, arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 297:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DBB0C(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 298:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DEB04(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 299:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DEBBC(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 300:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DEAB4(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 301:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DEBF4(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 302:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DEC88(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 303:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = Winger(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 304:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Winning(arg0);
        if (fn_800E7EB4(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    default:
        nlBreak();
        break;
    }
}
