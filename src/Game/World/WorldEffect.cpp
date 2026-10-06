#include "NL/nlDLListContainer.inl"
#include "Game/World/WorldEffect.h"

#include "Game/Debug/ShapeRender.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Render/Frustum.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/World.h"
#include "Game/World/worldanim.h"
#include "NL/gl/glView.h"
#include "NL/nlMemory.h"

static bool s_drawEffectBounds;

void WorldEffect::Initialize(WorldObjectLoadContext* pContext)
{
    m_bActive = true;
    if (m_nTimingMode == 0)
    {
        float fEmissionInterval = m_fEmissionInterval;
        m_fEmissionTime = fEmissionInterval + 1.0f;
    }
    else
    {
        m_fEmissionTime = 0.0f;
    }
    m_fPreviousEmissionTime = m_fEmissionTime;
    m_nRemainingEmissions = m_nEmissionCount;
    pContext->m_pWorld->AddEffect(this);
}

void WorldEffect::ReleaseResources()
{
    nlDLListIterator<EmissionController*> controllerIterator;
    controllerIterator = EmissionManager::Instance()->GetContainer()->Begin();
    DLListEntry<EmissionController*>* pHead = controllerIterator.m_Head;
    DLListEntry<EmissionController*>* pCurrent = controllerIterator.m_Curr;
    while (pCurrent != 0)
    {
        EmissionController* pController = pCurrent->entry;
        if (pController->m_uUserData == (u32)this)
        {
            pController->ClearParticles();
            pController->mUpdateCallback.Clear();
        }
        if (nlDLRingIsEnd(pHead, pCurrent) || pCurrent == 0)
        {
            pCurrent = 0;
        }
        else
        {
            pCurrent = pCurrent->m_next;
        }
    }
}

void WorldEffect::Update(float fDeltaT)
{
    if (fDeltaT != 0.0f && m_bActive)
    {
        bool bEmit = false;
        if (m_nTimingMode == 0)
        {
            if (m_nRemainingEmissions > 0
                || m_nRemainingEmissions == -1)
            {
                float fEmissionTime = m_fEmissionTime;
                float fEmissionInterval
                    = m_fEmissionInterval;
                fEmissionTime += fDeltaT;
                m_fEmissionTime = fEmissionTime;
                if (fEmissionInterval <= fEmissionTime
                    && nlRandom(100, &nlDefaultSeed)
                        < m_uProbability)
                {
                    bEmit = true;
                }
            }
        }
        else if (m_nRemainingEmissions > 0
            || m_nRemainingEmissions == -1)
        {
            float fEmissionTime = m_fEmissionTime;
            float fEmissionInterval = m_fEmissionInterval;
            if (fEmissionInterval <= fEmissionTime
                && nlRandom(100, &nlDefaultSeed)
                    < m_uProbability)
            {
                bEmit = true;
            }
        }

        if (bEmit)
        {
            Emit();
        }
    }
}

void WorldEffect::Emit()
{
    EffectsGroup* pGroup
        = fxGetGroup(EmissionManager::Instance(),
            m_uEffectHash);
    if (pGroup != 0)
    {
        EmissionController* pController
            = EmissionManager::Instance()->Create(pGroup,
                1, true, 0);
        m_fEmissionRadius = pController->GetBoundingRadius();

        nlVector3 velocity = { 0.0f, 0.0f, 0.0f };
        pController->SetVelocity(velocity);
        pController->m_fGround = 0.02f;

        nlMatrix4* pMatrix = GetWorldMatrix();
        pController->SetPosition(
            *(nlVector3*)&pMatrix->e2[3][0]);
        pMatrix = GetWorldMatrix();
        nlVector3 direction;
        nlVec3Set(direction, pMatrix->e2[2][0], pMatrix->e2[2][1],
            pMatrix->e2[2][2]);
        pController->SetDirection(direction);

        if (m_pAnimController != 0)
        {
            pController->SetUpdateCallback(
                Function1<void, EmissionController&>(
                    UpdateAnimatedWorldEffectController));
            pController->m_uUserData = (u32)this;
        }
        else
        {
            pController->SetUpdateCallback(
                Function1<void, EmissionController&>(
                    UpdateWorldEffectControllerVisibility));
            pController->m_uUserData = (u32)this;
        }
        m_nEmissionID = pController->m_Id;
    }
    else
    {
        m_nEmissionID = -1;
    }

    m_fPreviousEmissionTime = m_fEmissionTime;
    m_fEmissionTime = 0.0f;
    --m_nRemainingEmissions;
    if (m_nRemainingEmissions < -1)
    {
        m_nRemainingEmissions = -1;
    }
}

static const nlColour s_effectBoundsColour
    = { 0xFF, 0xFF, 0x80, 0xFF };

void WorldEffect::UpdateVisibility(EmissionController* pController)
{
    const nlVector3& position = pController->GetPosition();
    float radius = m_fEmissionRadius;
    if (!m_bActive)
    {
        pController->m_bVisible = false;
    }
    else if (m_bAlwaysVisible == true)
    {
        pController->m_bVisible = true;
    }
    else
    {
        const nlVector4* pCullData = m_pWorld->m_pOpaqueView
                                         ->m_Interface->GetShadowMatrix();
        if (ClassifySphereInFrustum(pCullData, &position, radius)
                == FRUSTUM_OUTSIDE
            || !m_pWorld->m_bRenderingEnabled)
        {
            pController->m_bVisible = false;
        }
        else
        {
            pController->m_bVisible = true;
        }
    }

    if (s_drawEffectBounds)
    {
        nlColour colour = s_effectBoundsColour;
        g_ShapeRenderer.DrawSphere(position, colour, radius);
    }
}

void UpdateAnimatedWorldEffectController(EmissionController& controller)
{
    WorldEffect* pEffect
        = (WorldEffect*)controller.m_uUserData;
    if (pEffect != 0
        && pEffect->m_pAnimController->GetAnimationTime() != 0.0f)
    {
        nlMatrix4& nodeMatrix
            = pEffect->m_pAnimController->GetNodeMatrix(
                pEffect->m_nAnimNode);
        controller.SetPosition(*(nlVector3*)&nodeMatrix.e2[3][0]);
        controller.SetDirection(*(nlVector3*)&nodeMatrix.e2[2][0]);
        pEffect->UpdateVisibility(&controller);
    }
}

void UpdateWorldEffectControllerVisibility(EmissionController& controller)
{
    WorldEffect* pEffect
        = (WorldEffect*)controller.m_uUserData;
    if (pEffect != 0)
    {
        pEffect->UpdateVisibility(&controller);
    }
}

WorldEffect::~WorldEffect()
{
}
