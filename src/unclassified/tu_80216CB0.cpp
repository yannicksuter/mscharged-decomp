#include "unclassified/tu_80216CB0.h"

#include "Game/Effects/EmissionManager.h"
#include "Game/FE/tlComponentInstance.h"
#include "unclassified/tu_802492E4.h"

extern TLComponentInstance* lbl_80578450[4];

extern "C" EffectsGroup* fn_802E7CDC(EmissionManager* manager, const char* name);
extern "C" void fn_802E83C4(EmissionManager* manager, EffectsGroup* group);
extern "C" void fn_801CBCE4(unsigned long hash, bool enabled);
extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);

TU80216CB0Scene::~TU80216CB0Scene()
{
}

void fn_802172C4()
{
    EmissionManager* manager = EmissionManager::Instance();
    EffectsGroup* group = fn_802E7CDC(manager, "fe_light_cone_trophy");
    if (group != 0)
    {
        fn_802E83C4(manager, group);
    }
    fn_801CBCE4(0x1C4C829E, true);
}

void TU80217320Scene::fn_80217880(
    const TU80300104Event* event, int index, float)
{
    mUnidentified12C.fn_80219608(event);
    mUnidentified618.fn_80219608(event);

    for (int i = 0; i < 2; ++i)
    {
        mUnidentified4B0[i].fn_80219608(event);
    }
    for (int i = 0; i < 4; ++i)
    {
        mUnidentified1E0[i].fn_80219608(event);
    }

    if (mControllerCounts[index] <= 0)
    {
        mUnidentified6EC->m_bVisible = false;
    }
}

void TU80217320Scene::fn_80218104(int, void*)
{
}

void TU80217320Scene::fn_802182B4(int index, void* context)
{
    unsigned int item = (unsigned int)context;

    --mControllerCounts[index];
    mTrophyTimers[item] = 0.1f;

    if (item == 0)
    {
        mUnidentified12C.mValues[index] = 0;
    }
    else if (item <= 2)
    {
        mUnidentified4B0[item - 1].mValues[index] = 0;
    }
    else if (item <= 6)
    {
        mUnidentified1E0[item - 3].mValues[index] = 0;
    }
}

void TU80217320Scene::fn_80218548(int index, void*)
{
    unsigned int which = index;
    ++mControllerCounts[index];
    if (!mUnidentified618.fn_802192FC(1, which))
    {
        mUnidentified6CC->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF33, 0, 0, 1);
        mUnidentified618.mValues[which] = 1;
    }
}

void TU80217320Scene::fn_802185E4(int index, void*)
{
    unsigned int which = index;
    --mControllerCounts[index];
    if (!mUnidentified618.fn_802192FC(1, which))
    {
        mUnidentified6CC->SetActiveSlide("off", true, false);
        mUnidentified618.mValues[which] = 0;
    }
}

void TU80217320Scene::fn_80218668(int, void*)
{
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }
    fn_801CBCA0(0xBB142B94, 0, 0, 1);
    fn_80249A94(mMode, 0);
}

void TU80218858Scene::fn_80217880(
    const TU80300104Event* event, int, float)
{
    mUnidentified12C.fn_80219608(event);
}

void TU80218858Scene::fn_80218D58(int index, void*)
{
    unsigned int which = index;
    ++mControllerCounts[index];
    if (!mUnidentified12C.fn_802192FC(1, which))
    {
        mUnidentified1E0->SetActiveSlide("over", true, false);
        mUnidentified12C.fn_80300D74(
            mUnidentified1E0, true, 0.0f, 0.0f, 1.0f, 1.0f);
        mUnidentified12C.mValues[which] = 1;
    }
}

void TU80218858Scene::fn_80218E08(int index, void*)
{
    unsigned int which = index;
    --mControllerCounts[index];
    if (!mUnidentified12C.fn_802192FC(1, which))
    {
        mUnidentified1E0->SetActiveSlide("off", true, false);
        mUnidentified12C.fn_80300D74(
            mUnidentified1E0, true, 0.0f, 0.0f, 1.0f, 1.0f);
        mUnidentified12C.mValues[which] = 0;
    }
}

void TU80218858Scene::fn_80218EB8(int, void*)
{
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }
    fn_80249A94(4, 0);
}

TU80217320Scene::~TU80217320Scene()
{
}

TU80218858Scene::~TU80218858Scene()
{
}
