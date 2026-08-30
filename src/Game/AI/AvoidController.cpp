#include "Game/AI/AvoidController.h"

#include "Game/AI/Fielder.h"
#include "NL/nlMemory.h"

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

extern "C" DebugFieldType lbl_80533C98[];
extern "C" unsigned short fn_80338EBC(DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" void fn_80339090(DebugWriteCache*, int, unsigned short,
    unsigned int, unsigned int, const char*);
extern "C" void* fn_8033930C(
    DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(
    DebugWriteCache*, unsigned short, void*, void*);
extern "C" int fn_8000CEF8(eAvoidableThings);

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

static float sUnidentifiedMemorySeconds = 0.0f;
static float sUnidentifiedRepulsionValue0 = 0.0f;
static float sUnidentifiedRepulsionValue1 = 0.0f;
static float sUnidentifiedInitialValue0 = 0.5f;
static float sUnidentifiedInitialValue1 = 1.0f;
static unsigned short sAvoidControllerType = 0xFFFF;

inline UnidentifiedAvoidanceMemory::UnidentifiedAvoidanceMemory()
    : mTimer()
{
    mTimer.SetSeconds(sUnidentifiedMemorySeconds);
}

UnidentifiedAvoidanceHistoryBase::UnidentifiedAvoidanceHistoryBase()
    : mUnidentified004(0.3f)
    , mUnidentified008(20)
    , mUnidentified00C(0)
    , mUnidentified010(0)
    , mUnidentified014((nlVector3*)nlMalloc(20 * sizeof(nlVector3), 8, false))
    , mUnidentified018((float*)nlMalloc(20 * sizeof(float), 8, false))
    , mUnidentified028(0.0f)
{
    mUnidentified01C = v3Zero;
    mUnidentified02C = v3Zero;
}

UnidentifiedAvoidanceHistoryBase::~UnidentifiedAvoidanceHistoryBase()
{
    delete[] mUnidentified014;
    delete[] mUnidentified018;
}

UnidentifiedAvoidanceHistory::UnidentifiedAvoidanceHistory()
{
}

UnidentifiedAvoidanceHistory::~UnidentifiedAvoidanceHistory()
{
}

void UnidentifiedAvoidanceHistory::UnidentifiedGetValue(
    nlVector3& value, float magnitude, const nlVector3& input) const
{
    if (magnitude > 0.0001f)
    {
        float scale = 1.0f / magnitude;
        value.x = input.x * scale;
        value.y = input.y * scale;
        value.z = input.z * scale;
        return;
    }

    if (mUnidentified00C != mUnidentified010)
    {
        int index = mUnidentified00C - 1;
        if (index < 0)
        {
            index = mUnidentified008 - 1;
        }
        value = mUnidentified014[index];
    }
    else
    {
        value = mUnidentified02C;
    }
}

extern "C" void fn_8000F178(AvoidController* controller)
{
    controller->m_ThingsToAvoid = AVOID_NOTHING;
    controller->m_CurrentlyAvoiding = AVOID_NOTHING;
    controller->m_fRepulsionMult = 1.0f;
    controller->m_VeryCloseToSideline = false;
    controller->m_SidelineUnavoidable = false;
    controller->m_SidelineNormal.x = 0.0f;
    controller->m_SidelineNormal.y = 0.0f;
    controller->m_SidelineDirection.x = 0.0f;
    controller->m_SidelineDirection.y = 0.0f;

    for (int i = 0; i < NUM_AVOIDABLES; ++i)
    {
        controller->m_LastRepulVec[i] = v3Zero;
        controller->mUnidentified094[i] = 0.0f;
        controller->mUnidentified0B4[i].mTimer.SetSeconds(0.0f);
        controller->mUnidentified0B4[i].mRepulsion = v3Zero;
    }

    controller->mUnidentified174.Clear();
    controller->mUnidentified198 = 0;
}

AvoidController::AvoidController(cFielder* fielder)
    : mUnidentified174(16, 16)
{
    sUnidentifiedRepulsionValue0 = sUnidentifiedInitialValue0;
    sUnidentifiedRepulsionValue1 = sUnidentifiedInitialValue1;
    fn_8000F178(this);
    m_pFielder = fielder;
}

AvoidController::~AvoidController()
{
    mUnidentified174.Clear();
}

void AvoidController::SetThingsToAvoid(int thingsToAvoid)
{
    m_ThingsToAvoid = thingsToAvoid;
    if (thingsToAvoid == AVOID_NOTHING)
    {
        fn_8000F178(this);
    }
}

nlVector3& AvoidController::GetLastRepulsionVector(eAvoidableThings things)
{
    return m_LastRepulVec[fn_8000CEF8(things)];
}

extern "C" float fn_8000F558(
    AvoidController* controller, eAvoidableThings things)
{
    return controller->mUnidentified094[fn_8000CEF8(things)];
}

extern "C" void fn_8000F324(AvoidController* controller,
    void* context, DebugWriteCache* cache)
{
    if (sAvoidControllerType == 0xFFFF)
    {
        sAvoidControllerType = fn_80338EBC(cache, "AvoidController");
        fn_80338F88(cache, 15, lbl_80533C98[15].size, 0,
            "m_pFielder");
        fn_80338F88(cache, 8, lbl_80533C98[8].size,
            (u8*)&controller->m_ThingsToAvoid - (u8*)controller,
            "m_ThingsToAvoid");
        fn_80338F88(cache, 8, lbl_80533C98[8].size,
            (u8*)&controller->m_CurrentlyAvoiding - (u8*)controller,
            "m_CurrentlyAvoiding");
        fn_80338F88(cache, 17, lbl_80533C98[17].size,
            (u8*)&controller->m_fRepulsionMult - (u8*)controller,
            "m_fRepulsionMult");
        fn_80338F88(cache, 16, lbl_80533C98[16].size,
            (u8*)&controller->m_VeryCloseToSideline - (u8*)controller,
            "m_VeryCloseToSideline");
        fn_80338F88(cache, 16, lbl_80533C98[16].size,
            (u8*)&controller->m_SidelineUnavoidable - (u8*)controller,
            "m_SidelineUnavoidable");
        fn_80338F88(cache, 21, lbl_80533C98[21].size,
            (u8*)&controller->m_SidelineNormal - (u8*)controller,
            "m_SidelineNormal");
        fn_80338F88(cache, 21, lbl_80533C98[21].size,
            (u8*)&controller->m_SidelineDirection - (u8*)controller,
            "m_SidelineDirection");
        fn_80339090(cache, 22, lbl_80533C98[22].size,
            NUM_AVOIDABLES,
            (u8*)&controller->m_LastRepulVec - (u8*)controller,
            "m_LastRepulVec[]");
        fn_80338F78(cache);
    }

    AvoidController* copy = (AvoidController*)fn_8033930C(
        cache, sAvoidControllerType, controller, sizeof(AvoidController));
    if (copy != 0)
    {
        *(int*)&copy->m_pFielder = controller->m_pFielder == 0
            ? -1
            : controller->m_pFielder->mUnidentified120;
        fn_80339450(cache, sAvoidControllerType, copy, context);
    }
}
