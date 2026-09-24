#include "Game/BasicStadium.h"

#include "Game/Effects/EmissionManager.h"
#include "Game/Drawable/DrawableObj.h"
#include "Game/GL/GLInventory.h"
#include "Game/Render/Frustum.h"
#include "Game/Render/ImpostorManager.h"
#include "Game/Render/Warble.h"
#include "Game/Render/WorldNPC.h"
#include "Game/World/WorldEffect.h"
#include "Game/Render/RLView.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glTarget.h"
#include "NL/gl/glModel.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlTask.h"
#include "Game/Render/HighRange.h"
#include "Game/TweakValue.inl"

#include "Game/UnidentifiedStaticStorage.h"

extern "C"
{
    void fn_80184AF8(float timeScale);
    void fn_80184B08();
}

/**
 * Address/Size: 0x8027856C | size: 0x90
 */
void BasicStadium::Update(float fDeltaT, bool bUpdateState, bool bUpdateNPCs)
{
    m_fTime += fDeltaT;
    World::Update(fDeltaT, bUpdateState);

    if (gpWorldNPCManager != 0 && bUpdateNPCs)
    {
        gpWorldNPCManager->Update(fDeltaT);
    }

    ImpostorManager::GetInstance()->UpdateAnimations(fDeltaT);
    UpdateWarblePhase(&gWarbleEnabled, fDeltaT);
    glGetCurrentResourcePool()->m_inventory->Update(fDeltaT);
}

/**
 * Address/Size: 0x802785FC | size: 0x170
 */
void fn_802785FC(BasicStadium* pStadium, float fDeltaT)
{
    pStadium->m_fTime += fDeltaT;
    pStadium->World::UpdateAnimations(fDeltaT);

    if (nlTaskManager::m_pInstance->mCurrentState == 0x10)
    {
        pStadium->UpdateEffects(fDeltaT);

        for (nlListIterator<WorldEffect*> iterator
                 = pStadium->m_worldEffects.Begin();
             iterator.IsValid(); iterator.Next())
        {
            WorldEffect* pEffect = iterator.Current();
            if (pEffect->m_nTimingMode != 0x37U)
            {
                continue;
            }
            if (pEffect->m_nRemainingEmissions <= 0
                && pEffect->m_nRemainingEmissions != -1)
            {
                continue;
            }

            float fNextTime = pEffect->m_fPreviousEmissionTime - fDeltaT;
            if (fNextTime <= 0.0f)
            {
                if (nlRandom(100, &nlDefaultSeed) < pEffect->m_uProbability)
                {
                    pEffect->Emit();
                    if (-1.0f != pEffect->mUnidentified064)
                    {
                        fNextTime = pEffect->m_fEmissionInterval
                            + pEffect->mUnidentified064;
                    }
                    else
                    {
                        fNextTime = pEffect->m_fEmissionInterval;
                    }
                }
            }
            pEffect->m_fPreviousEmissionTime = fNextTime;
        }
    }

    if (gpWorldNPCManager != 0)
    {
        gpWorldNPCManager->Update(fDeltaT);
    }

    ImpostorManager::GetInstance()->UpdateAnimations(fDeltaT);
    UpdateWarblePhase(&gWarbleEnabled, fDeltaT);
    glGetCurrentResourcePool()->m_inventory->Update(fDeltaT);
}

/**
 * Address/Size: 0x8027876C | size: 0x40
 */
void fn_8027876C(BasicStadium* pStadium, DrawableObject* pObject)
{
    unsigned long uKey = pObject->m_uHashID;
    pStadium->m_registeredDrawables.Add(uKey, pObject);
}

/**
 * Address/Size: 0x802787AC | size: 0x6C
 */
DrawableObject* fn_802787AC(BasicStadium* pStadium, unsigned long uHashID)
{
    DrawableObject** ppObject;
    if (!pStadium->m_registeredDrawables.FindGet(uHashID, &ppObject))
    {
        return 0;
    }
    return *ppObject;
}

/**
 * Address/Size: 0x80278818 | size: 0x48
 */
void fn_80278818(BasicStadium* pStadium, nlVector4* corners)
{
    GetFrustumCorners(
        pStadium->m_pOpaqueView->m_Interface->GetShadowMatrix(),
        corners);
}

/**
 * Address/Size: 0x80278860 | size: 0x5C
 */
void fn_80278860(BasicStadium* pStadium, int active)
{
    if (EmissionManager::Instance() == 0)
    {
        return;
    }

    for (nlListIterator<WorldEffect*> iterator = pStadium->m_worldEffects.Begin();
         iterator.IsValid(); iterator.Next())
    {
        iterator.Current()->m_bActive = active;
    }
}

/**
 * Address/Size: 0x802788BC | size: 0x50
 */
void fn_802788BC(BasicStadium* pStadium, float fTimeScale)
{
    pStadium->m_shadowHeight = fTimeScale;
    fn_80184AF8(pStadium->m_shadowHeight);
    GetEmissionManager()->mTimeScale = pStadium->m_shadowHeight;
}

/**
 * Address/Size: 0x8027890C | size: 0x94
 */
void fn_8027890C(BasicStadium* pStadium, const char* effects, unsigned long uType)
{
    EmissionManager* pManager = EmissionManager::Instance();
    EffectsGroup* pGroup = pManager->GetEffectsGroup(effects);

    for (nlListIterator<WorldEffect*> iterator = pStadium->m_worldEffects.Begin();
         iterator.IsValid(); iterator.Next())
    {
        WorldEffect* pEffect = iterator.Current();
        if (uType == (unsigned long)pEffect->m_nTimingMode && pGroup != 0)
        {
            pManager->Kill((unsigned long)pEffect, pGroup);
        }
    }
}

/**
 * Address/Size: 0x802789A0 | size: 0x8
 */
float fn_802789A0(BasicStadium* pStadium)
{
    return pStadium->m_fTime;
}

/**
 * Address/Size: 0x802789A8 | size: 0x58
 */
void fn_802789A8(BasicStadium* pStadium, unsigned long uType)
{
    for (nlListIterator<WorldEffect*> iterator = pStadium->m_worldEffects.Begin();
         iterator.IsValid(); iterator.Next())
    {
        WorldEffect* pEffect = iterator.Current();
        if (uType == (unsigned long)pEffect->m_nTimingMode)
        {
            pEffect->Emit();
        }
    }
}

/**
 * Address/Size: 0x80278A00 | size: 0x2C
 */
void BasicStadium::SetEffectsActive(unsigned long uType, int active)
{
    for (nlListIterator<WorldEffect*> iterator = m_worldEffects.Begin();
         iterator.IsValid(); iterator.Next())
    {
        WorldEffect* pEffect = iterator.Current();
        if (uType == (unsigned long)pEffect->m_nTimingMode)
        {
            pEffect->m_bActive = active;
        }
    }
}

/**
 * Address/Size: 0x80278A2C | size: 0x284
 */
BasicStadium::BasicStadium(GLResourcePool* pResource)
    : World(pResource)
{
    m_shadowHeight = 0.0f;
    m_fTime = 0.0f;

    GetEmissionManager()->mTimeScale = 0.0f;
    fn_80184B08();

    m_shadowLightPosition.x = 10.0f;
    m_shadowLightPosition.y = -10.0f;
    m_shadowLightPosition.z = 40.0f;

    m_pStadiumHighRangeTweaks = new (
        nlMalloc(sizeof(HighRangeTweaks), 8, false)) HighRangeTweaks();
    BindHighRangeTweaks(m_pStadiumHighRangeTweaks, "/Rendering/Effects/HighRange/Stadium");

    m_pMegastrikeHighRangeTweaks = new (
        nlMalloc(sizeof(HighRangeTweaks), 8, false)) HighRangeTweaks();
    BindHighRangeTweaks(
        m_pMegastrikeHighRangeTweaks, "/Rendering/Effects/HighRange/Megastrike");

    m_pHighRangeTweaks = m_pStadiumHighRangeTweaks;
}
