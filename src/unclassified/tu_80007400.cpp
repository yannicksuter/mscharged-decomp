#include "Game/AI/AIPad.h"

static float g_fMovementDeadZone = 0.3f;
static float g_fCStickDeadZone = 0.5f;

cAIPad AIPadManager::mAIPads[16];

cAIPad::cAIPad()
{
    mUnidentified2D4 = 0;
    mUnidentified2D8 = -1;
    m_pGlobalPad = 0;

    for (int i = 0; i < 30; ++i)
    {
        mUnidentified004[i].x = 1000.0f;
        mUnidentified004[i].y = 0.0f;
        mUnidentified004[i].z = 0.0f;
        mUnidentified16C[i].x = 1000.0f;
        mUnidentified16C[i].y = 0.0f;
        mUnidentified16C[i].z = 0.0f;
    }
}

float cAIPad::GetMovementStickMagnitude()
{
    float mag = m_pGlobalPad->m_polarAnalogLeft.r;
    float dz = g_fMovementDeadZone;
    return (mag - dz) / (1.0f - dz);
}

u16 cAIPad::GetMovementStickDirection()
{
    return m_pGlobalPad->mUnidentified088 + m_pGlobalPad->m_polarAnalogLeft.a;
}

float cAIPad::GetCStickMovementStickMagnitude()
{
    float mag = m_pGlobalPad->m_polarAnalogRight.r;
    float dz = g_fCStickDeadZone;
    return (mag - dz) / (1.0f - dz);
}

u16 cAIPad::GetCStickMovementStickDirection()
{
    return m_pGlobalPad->mUnidentified088 + m_pGlobalPad->m_polarAnalogRight.a;
}

struct UnclassifiedControllerGroup
{
    void* mUnidentified000;
    int mNumControllers;
};

extern "C" void* lbl_806E20D8;
extern "C" int fn_80338BF0(void*);
extern "C" UnclassifiedControllerGroup* fn_80338BF8(void*, s8);
extern "C" void* fn_80336B6C(UnclassifiedControllerGroup*, s8);
extern "C" s8 fn_80336F68(s8, s8);
extern "C" cGlobalPad* fn_80336D68(void*);
extern "C" s8 fn_80338C20(void*);

void AIPadManager::Startup()
{
    for (int i = 0; i < 16; ++i)
    {
        mAIPads[i].m_pGlobalPad = 0;
    }

    int numGroups = fn_80338BF0(lbl_806E20D8);
    for (s8 groupIndex = 0; groupIndex < numGroups; ++groupIndex)
    {
        UnclassifiedControllerGroup* group = fn_80338BF8(lbl_806E20D8, groupIndex);
        for (s8 controllerIndex = 0; controllerIndex < group->mNumControllers; ++controllerIndex)
        {
            void* controller = fn_80336B6C(group, controllerIndex);
            s8 padIndex = fn_80336F68(controllerIndex, groupIndex);
            cAIPad& pad = mAIPads[padIndex];
            pad.m_pGlobalPad = fn_80336D68(controller);

            if (groupIndex == fn_80338C20(lbl_806E20D8))
            {
                pad.mUnidentified2D8 = controllerIndex;
            }
        }
    }
}

extern "C" cAIPad* fn_80007C3C(int index)
{
    return &AIPadManager::mAIPads[index];
}
