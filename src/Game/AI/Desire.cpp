#include "Game/AI/Desire.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/Variant.h"

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

extern "C" DebugFieldType lbl_80533C98[];
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" unsigned short fn_80338EBC(DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_8004F594(int, const char*, ...);
extern "C" void fn_80098098(cFielder*);
extern "C" void fn_800401C0(cFielder*, const nlVector3&, float, float);
extern "C" void fn_80040368(cFielder*);

float lbl_806DC04C = 60.0f;
float lbl_806DC050 = 0.3f;
extern float lbl_806DC054;

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

void Desire::UnidentifiedSetContext(UnidentifiedDesireContext* context)
{
    shdStateMachine::UnidentifiedSetContext(context);
    if (context != 0)
    {
        mUnidentifiedFielder = (cFielder*)context->mUnidentifiedValue->mData.pointer;
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

void DesireFinishAction::UnidentifiedUpdate(UnidentifiedDesireUpdate* update, float)
{
    if (update->mData.i == 2)
    {
        fn_8004F594(4,
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
    fn_80338F88(cache, 22, lbl_80533C98[22].size, 0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
}

void Desire::UnidentifiedUpdate(UnidentifiedDesireUpdate*, float)
{
}

DesireCutAndBreak::~DesireCutAndBreak()
{
}

void DesireCutAndBreak::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireCutAndBreak");
    fn_80338F88(cache, 22, lbl_80533C98[22].size, 0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    fn_80338F78(cache);
}

DesireDeke::~DesireDeke()
{
}

bool DesireDeke::UnidentifiedInitialize(void* context)
{
    mUnidentifiedA4 = 0;
    if (fn_8030F030((UnidentifiedVariantCollection*)context, 14))
    {
        mUnidentifiedA4 = fn_8030F060(
            (UnidentifiedVariantCollection*)context, 14)->mData.pointer;
    }
    return true;
}

void DesireDeke::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireDeke");
    fn_80338F88(cache, 22, lbl_80533C98[22].size, 0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    fn_80338F78(cache);
}

void DesireHit::UnidentifiedUpdate(UnidentifiedDesireUpdate*, float)
{
}

DesireHit::~DesireHit()
{
}

void DesireHit::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireHit");
    fn_80338F88(cache, 22, lbl_80533C98[22].size, 0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    fn_80338F78(cache);
}

void DesireGetOpen::UnidentifiedUpdate(UnidentifiedDesireUpdate*, float)
{
    fn_800401C0(mUnidentifiedFielder, mvDesiredPosition, 1.2f, 1.0f);
}

DesireGetOpen::~DesireGetOpen()
{
}

void DesireGetOpen::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireGetOpen");
    fn_80338F88(cache, 22, lbl_80533C98[22].size, 0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    fn_80338F78(cache);
}

bool DesireGetInPosition::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    fn_80040368(mUnidentifiedFielder);
    return result;
}

DesireGetInPosition::~DesireGetInPosition()
{
}

void DesireGetInPosition::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireGetInPosition");
    fn_80338F88(cache, 22, lbl_80533C98[22].size, 0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    fn_80338F78(cache);
}

bool DesireRunUpfield::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    fn_80040368(mUnidentifiedFielder);
    return result;
}

DesireRunUpfield::~DesireRunUpfield()
{
}

void DesireRunUpfield::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireRunUpfield");
    fn_80338F88(cache, 22, lbl_80533C98[22].size, 0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    fn_80338F78(cache);
}

bool DesireRunDownfield::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    fn_80040368(mUnidentifiedFielder);
    return result;
}

DesireRunDownfield::~DesireRunDownfield()
{
}

void DesireRunDownfield::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireRunDownfield");
    fn_80338F88(cache, 22, lbl_80533C98[22].size, 0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    fn_80338F78(cache);
}

void DesireRunInDirection::UnidentifiedCleanup()
{
}

DesireRunInDirection::~DesireRunInDirection()
{
}

void DesireRunInDirection::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireRunInDirection");
    fn_80338F88(cache, 22, lbl_80533C98[22].size, 0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    fn_80338F88(cache, 19, lbl_80533C98[19].size, (u8*)&m_aDirection - (u8*)&mvDesiredPosition, "m_aDirection");
    fn_80338F88(cache, 17, lbl_80533C98[17].size, (u8*)&m_fMaxDistance - (u8*)&mvDesiredPosition, "m_fMaxDistance");
    fn_80338F88(cache, 17, lbl_80533C98[17].size, (u8*)&m_fDistTravelled - (u8*)&mvDesiredPosition, "m_fDistTravelled");
    fn_80338F88(cache, 17, lbl_80533C98[17].size, (u8*)&m_fSpeed - (u8*)&mvDesiredPosition, "m_fSpeed");
    fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&m_eFieldDirection - (u8*)&mvDesiredPosition, "m_eFieldDirection");
    fn_80338F88(cache, 15, lbl_80533C98[15].size, (u8*)&m_pTarget - (u8*)&mvDesiredPosition, "m_pTarget");
    fn_80338F78(cache);
}

void DesireRunToTarget::UnidentifiedCleanup()
{
}

DesireRunToTarget::~DesireRunToTarget()
{
}

void DesireRunToTarget::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireRunToTarget");
    fn_80338F88(cache, 22, lbl_80533C98[22].size, 0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    fn_80338F88(cache, 15, lbl_80533C98[15].size, (u8*)&m_pTargetFielder - (u8*)&mvDesiredPosition, "m_pTargetFielder");
    fn_80338F88(cache, 22, lbl_80533C98[22].size, (u8*)&m_vTargetPos - (u8*)&mvDesiredPosition, "m_vTargetPos");
    fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&m_eDirection - (u8*)&mvDesiredPosition, "m_eDirection");
    fn_80338F88(cache, 17, lbl_80533C98[17].size, (u8*)&m_fDistOffset - (u8*)&mvDesiredPosition, "m_fDistOffset");
    fn_80338F88(cache, 17, lbl_80533C98[17].size, (u8*)&m_fUrgency - (u8*)&mvDesiredPosition, "m_fUrgency");
    fn_80338F88(cache, 17, lbl_80533C98[17].size, (u8*)&m_fSpeedCoeff - (u8*)&mvDesiredPosition, "m_fSpeedCoeff");
    fn_80338F88(cache, 17, lbl_80533C98[17].size, (u8*)&m_fAvoidanceCoeff - (u8*)&mvDesiredPosition, "m_fAvoidanceCoeff");
    fn_80338F78(cache);
}
