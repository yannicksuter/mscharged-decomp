#include "Game/AI/AIPad.h"

#include "Game/NetworkSession.h"
#include "NL/glx/GXMaterialCrystalTweaks.h"
#include "NL/nlConfig.h"
#include "NL/nlFormat.h"
#include "unclassified/tu_80336B2C.h"

static float g_fMovementDeadZone = 0.3f;
static float g_fCStickDeadZone = 0.5f;
float lbl_806DB3C8 = 0.5f;

static GXMaterialFloatTweak_804F4190 sTweak_80568410(
    "DPD_Sensitivity", "Controller Config/DPD", 1.8f);
static GXMaterialFloatTweak_804F4190 sTweak_80568430(
    "gfLeftShakeThreshold", "Controller Config", 2.5f);
static GXMaterialFloatTweak_804F4190 sTweak_80568450(
    "gfRightShakeThreshold", "Controller Config", 1.33f);

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
    float mag = m_pGlobalPad->m_PolarAnalogLeft.r;
    float dz = g_fMovementDeadZone;
    return (mag - dz) / (1.0f - dz);
}

u16 cAIPad::GetMovementStickDirection()
{
    return m_pGlobalPad->m_aRemapAngle + m_pGlobalPad->m_PolarAnalogLeft.a;
}

float cAIPad::GetCStickMovementStickMagnitude()
{
    float mag = m_pGlobalPad->m_PolarAnalogRight.r;
    float dz = g_fCStickDeadZone;
    return (mag - dz) / (1.0f - dz);
}

u16 cAIPad::GetCStickMovementStickDirection()
{
    return m_pGlobalPad->m_aRemapAngle + m_pGlobalPad->m_PolarAnalogRight.a;
}

void AIPadManager::Startup()
{
    for (int i = 0; i < 16; ++i)
    {
        mAIPads[i].m_pGlobalPad = 0;
    }

    int numGroups = fn_80338BF0(lbl_806E20D8);
    for (s8 groupIndex = 0; groupIndex < numGroups; ++groupIndex)
    {
        UnidentifiedNetworkPeer* group = fn_80338BF8(lbl_806E20D8, groupIndex);
        for (s8 controllerIndex = 0;
            controllerIndex < (int)group->mUnidentified004;
            ++controllerIndex)
        {
            UnidentifiedNetworkPeerChannel* controller
                = fn_80336B6C(group, controllerIndex);
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

GXMaterialFloatTweak_804F4190::~GXMaterialFloatTweak_804F4190()
{
}

void GXMaterialFloatTweak_804F4190::UnidentifiedVirtual2C(
    TweakValueBase_8052BF70* other)
{
    switch (other->UnidentifiedVirtual10())
    {
    case 1:
        value = *(float*)((u8*)other + 0x0C);
        break;
    case 2:
        value = **(float**)((u8*)other + 0x0C);
        break;
    }
}

int GXMaterialFloatTweak_804F4190::UnidentifiedVirtual10()
{
    return 1;
}

int GXMaterialFloatTweak_804F4190::UnidentifiedVirtual0C()
{
    return 5;
}

void* GXMaterialFloatTweak_804F4190::UnidentifiedVirtual20()
{
    return &value;
}

void GXMaterialFloatTweak_804F4190::UnidentifiedVirtual24(
    char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%.3f", value);
}

void GXMaterialFloatTweak_804F4190::UnidentifiedVirtual28(
    const char* string)
{
    value = (float)atof(string);
}

void GXMaterialFloatTweak_804F4190::UnidentifiedVirtual14(
    float* minimum, float* maximum, float* increment)
{
    *minimum = 0.0f;
    *maximum = 0.0f;
    *increment = 0.0f;
}

void GXMaterialFloatTweak_804F4190::UnidentifiedVirtual18()
{
}
