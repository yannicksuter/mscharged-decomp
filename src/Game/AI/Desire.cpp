#include "Game/AI/Desire.h"
#include "Game/Sys/debug.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FielderInput.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/TeamPlayMachine.h"
#include "Game/AI/Variant.h"
#include "Game/DebugWriteCache.h"

#include "Game/UnidentifiedStaticStorage.h"

extern "C" void fn_80098098(cFielder*);
extern "C" void fn_800401C0(cFielder*, const nlVector3&, float, float);

float lbl_806DC04C = 60.0f;
float lbl_806DC050 = 0.3f;
extern float lbl_806DC054;

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
    mUnidentifiedFielder = 0;
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
        mUnidentifiedFielder
            = (cFielder*)context->mUnidentified064->mData.pointer;
    }
    else
    {
        mUnidentifiedFielder = 0;
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

void DesireFinishAction::Update(UnidentifiedDesireUpdate* update, float)
{
    if (update->mData.i == 2)
    {
        tDebugPrintManager::Print(DC_AI,
            "** WARNING! DesireFinishAction has expired after %f seconds, probably a bug!\n",
            mUnidentifiedTimer.GetSeconds());
    }
    fn_80098098(mUnidentifiedFielder);
}

bool DesireWait::UnidentifiedInitialize(void*)
{
    mUnidentified078 = lbl_806DC050;
    return true;
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

void Desire::UnidentifiedVirtual8(void*, DebugWriteCache* cache)
{
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
}

void Desire::Update(UnidentifiedDesireUpdate*, float)
{
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

void DesireDeke::UnidentifiedCleanup()
{
    mUnidentifiedFielder->m_eLastPadAction = 50;
}

void DesireDeke::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireDeke");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

void DesireHit::Update(UnidentifiedDesireUpdate*, float)
{
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

void DesireGetOpen::Update(UnidentifiedDesireUpdate*, float)
{
    fn_800401C0(mUnidentifiedFielder, mvDesiredPosition, 1.2f, 1.0f);
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

bool DesireGetInPosition::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    mUnidentifiedFielder->StartRunning();
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

bool DesireRunUpfield::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    mUnidentifiedFielder->StartRunning();
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

bool DesireRunDownfield::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    mUnidentifiedFielder->StartRunning();
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

void DesireRunInDirection::UnidentifiedCleanup()
{
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

class UnidentifiedWeatherExtendedStateF;

extern "C" int fn_800B04B4(UnidentifiedWeatherExtendedStateF*)
{
    return 4;
}
