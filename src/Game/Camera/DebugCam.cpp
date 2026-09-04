#include "Game/Camera/DebugCam.h"

#include "Game/CharacterTemplate.h"
#include "Game/MathHelpers.h"
#include "Game/Task/ProfilerTask.h"
#include "Game/Task/TweakerTask.h"
#include "Game/TweakValue.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/glx/GXMaterialCrystalTweaks.h"
#include "NL/globalpad.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"

struct UnidentifiedDebugCameraTarget
{
    u8 mUnidentified00[0x20];
    nlVector3 mPosition;
};

struct UnidentifiedDebugCameraTargetEntry
{
    UnidentifiedDebugCameraTargetEntry* mNext;
    UnidentifiedDebugCameraTargetEntry* mPrev;
    UnidentifiedDebugCameraTarget* mTarget;
};

namespace
{
struct DebugCameraControlTweakValues
{
    float speed0;
    float weight0;
    float speed1;
    float weight1;
    float speed2;
    float weight2;
};

static DebugCameraControlTweakValues sControlTweakValues = {
    0.15f,
    0.0f,
    0.5f,
    0.1f,
    4.0f,
    1.0f,
};

static float sfDebugCamFOV = 60.0f;
static float sfControlSpeedScale = 100.0f;
static float sfControlDistanceScale = 1.0f;
static float sfControlHeightScale = 1.0f;
static float sfTargetFollowStep = 1.0f;

static u32 sLightRampTexture = glGetTexture("global/lightramp");
static u32 sBlackTexture = glGetTexture("global/black");
static u32 sWhiteTexture = glGetTexture("global/white");

static GXMaterialFloatTweak_804F4190 gDebugCameraSensitivity(
    "gDebugCameraSensitivity", "Controller Config/DPD", 3.0f);

static float sDebugCamFOVTweak;
static TweakValueImpl_804F4DC8 sSpeed0(
    "Speed 0", "Controller Config/DPD", &sControlTweakValues.speed0);
static TweakValueImpl_804F4DC8 sWeight0(
    "Weight 0", "Controller Config/DPD", &sControlTweakValues.weight0);
static TweakValueImpl_804F4DC8 sSpeed1(
    "Speed 1", "Controller Config/DPD", &sControlTweakValues.speed1);
static TweakValueImpl_804F4DC8 sWeight1(
    "Weight 1", "Controller Config/DPD", &sControlTweakValues.weight1);
static TweakValueImpl_804F4DC8 sSpeed2(
    "Speed 2", "Controller Config/DPD", &sControlTweakValues.speed2);
static TweakValueImpl_804F4DC8 sWeight2(
    "Weight 2", "Controller Config/DPD", &sControlTweakValues.weight2);

static u32 sSightTexture = nlStringLowerHash("global/sight");
static u32 sLightBlobTexture = nlStringLowerHash("global/light_blob");
} // namespace

extern void* lbl_806E1E28;
extern "C" cGlobalPad* fn_802C082C(void* manager, int index);

extern "C" void fn_800F2504()
{
    sfDebugCamFOV = sDebugCamFOVTweak;
}

cDebugCamera::cDebugCamera(bool bUnidentified)
{
    (void)bUnidentified;

    m_fRadius = 10.0f;
    m_fAzimuth = 215.0f;
    m_fTheta = 25.0f;
    m_fHeight = 0.0f;
    m_pPad = 0;
    mUnidentified8C = false;
    m_bEnableControls = true;
    mUnidentified8E = false;
    m_bRenderTarget = false;
    m_pTarget = 0;
    m_pTargets = 0;
    nlVec3Set(m_vecTarget, 0.0f, 0.0f, 0.0f);
    m_matView.SetIdentity();
    Update(0.0f);

    m_pPad = fn_802C082C(lbl_806E1E28, 0);
    mUnidentified8E = true;
}

cDebugCamera::~cDebugCamera()
{
    UnidentifiedDebugCameraTargetEntry* head = m_pTargets;
    if (head != 0)
    {
        UnidentifiedDebugCameraTargetEntry* entry = head->mNext;
        for (;;)
        {
            UnidentifiedDebugCameraTargetEntry* next = entry->mNext;
            delete entry->mTarget;
            if (entry == head)
            {
                break;
            }
            entry = next;
        }
    }

    while (m_pTargets != 0)
    {
        UnidentifiedDebugCameraTargetEntry* entry = m_pTargets->mNext;
        if (entry->mNext == entry)
        {
            m_pTargets = 0;
        }
        else
        {
            entry->mPrev->mNext = entry->mNext;
            entry->mNext->mPrev = entry->mPrev;
            if (m_pTargets == entry)
            {
                m_pTargets = entry->mPrev;
            }
        }
        delete entry;
    }
}

void cDebugCamera::RenderTarget() const
{
    if (!m_bRenderTarget || m_pTargets == 0)
    {
        return;
    }

    UnidentifiedDebugCameraTargetEntry* entry = m_pTargets->mNext;
    for (int i = 0; i < 10 && entry != 0; i++)
    {
        if (entry->mTarget != 0 && g_pCharacters[i] != 0)
        {
            entry->mTarget->mPosition = g_pCharacters[i]->m_v3Position;
        }

        if (entry == m_pTargets)
        {
            entry = 0;
        }
        else
        {
            entry = entry->mNext;
        }
    }
}

void cDebugCamera::fn_800F2A8C(float dt)
{
    float x = 0.0f;
    float y = 0.0f;

    if (!mUnidentified8C)
    {
        x = m_pPad->AnalogRightX();
        y = m_pPad->AnalogRightY();
    }
    else if (!m_pPad->IsPressed(0x400, false))
    {
        x = m_pPad->AnalogLeftX();
        y = m_pPad->AnalogLeftY();
    }

    m_fAzimuth += dt * (x * sfControlSpeedScale);
    m_fTheta += dt * (y * sfControlSpeedScale);

    if (m_fTheta > 89.0f)
    {
        m_fTheta = 89.0f;
    }
    if (m_fTheta < -89.0f)
    {
        m_fTheta = -89.0f;
    }
}

void cDebugCamera::fn_800F2BD0(float dt, float controlSpeed)
{
    float x = m_pPad->AnalogLeftX();
    float y = m_pPad->AnalogLeftY();

    if (x != 0.0f || y != 0.0f)
    {
        m_pTarget = 0;
    }

    nlVector3 offset;
    float forward = dt * (x * controlSpeed);
    nlVec3Set(offset,
        forward * m_matView.m11,
        forward * m_matView.m21,
        0.0f);
    nlVec3Add(m_vecTarget, m_vecTarget, offset);

    float side = dt * (-y * controlSpeed);
    nlVec3Set(offset,
        side * m_matView.m13,
        side * m_matView.m23,
        0.0f);
    nlVec3Add(m_vecTarget, m_vecTarget, offset);
}

void cDebugCamera::fn_800F2DA8(float dt, float controlSpeed)
{
    bool heightControls = mUnidentified8C
        ? m_pPad->IsPressed(0x400, false)
        : m_pPad->IsPressed(12, true);

    if (!heightControls)
    {
        if (m_pPad->IsPressed(3, true))
        {
            m_fRadius += controlSpeed * dt;
        }
        if (m_pPad->IsPressed(2, true))
        {
            m_fRadius -= controlSpeed * dt;
        }
    }
    else
    {
        if (m_pPad->IsPressed(2, true))
        {
            m_fHeight -= controlSpeed * dt;
        }
        if (m_pPad->IsPressed(3, true))
        {
            m_fHeight += controlSpeed * dt;
        }
    }

    if (nlTaskManager::m_pInstance->mCurrentState != 0x20000)
    {
        float down = m_pPad->GetPressure(5, true);
        float up = m_pPad->GetPressure(6, true);
        m_fHeight += dt * ((up - down) * controlSpeed);
    }

    if (m_fHeight < 0.0f)
    {
        m_fHeight = 0.0f;
    }
    if (m_fRadius < 0.001f)
    {
        m_fRadius = 0.001f;
    }
}

void cDebugCamera::Update(float dt)
{
    float controlSpeed = sfControlDistanceScale *
        (1.0f + sfControlHeightScale * (m_fRadius + m_fHeight));

    if (!g_bTweaking && !IsProfiling())
    {
        m_pPad = fn_802C082C(lbl_806E1E28, 0);

        float yPressure = m_pPad->GetPressure(3, true);
        float xPressure = m_pPad->GetPressure(2, true);
        if ((yPressure > 0.0f && m_pPad->PlatJustPressed(2, true)) ||
            (xPressure > 0.0f && m_pPad->PlatJustPressed(3, true)))
        {
            m_bEnableControls = !m_bEnableControls;
        }

        if (m_pTargets != 0)
        {
            UnidentifiedDebugCameraTargetEntry* entry = m_pTargetEntry;
            if (m_pPad->PlatJustPressed(13, true))
            {
                if (entry != 0)
                {
                    entry = entry->mPrev;
                }
                m_pTarget = entry->mTarget;
            }
            if (m_pPad->PlatJustPressed(14, true))
            {
                if (entry != 0)
                {
                    entry = entry->mNext;
                }
                m_pTarget = entry->mTarget;
            }
            m_pTargetEntry = entry;
        }

        if (m_bEnableControls)
        {
            fn_800F2A8C(dt);
            fn_800F2BD0(dt, controlSpeed);
            fn_800F2DA8(dt, controlSpeed);
        }
    }

    RenderTarget();

    if (m_pTarget != 0)
    {
        float distanceSquared = CalculateDistanceSquared(
            m_vecTarget, m_pTarget->mPosition);
        if (distanceSquared < sfTargetFollowStep * sfTargetFollowStep)
        {
            m_vecTarget = m_pTarget->mPosition;
        }
        else
        {
            nlVector3 delta;
            nlVec3Sub(delta, m_pTarget->mPosition, m_vecTarget);
            float scale = sfTargetFollowStep / nlSqrt(distanceSquared, true);
            nlVec3Scale(delta, delta, scale);
            nlVec3Add(m_vecTarget, m_vecTarget, delta);
        }
    }

    nlVector3 vecUp;
    nlVec3Set(vecUp, 0.0f, 0.0f, 1.0f);

    float sn;
    float cs;
    nlSinCos(&sn, &cs,
        (u16)(10430.378f * ((3.1415927f * m_fTheta) / 180.0f)));
    float z = m_fRadius * sn;
    float distance = m_fRadius * cs;
    nlSinCos(&sn, &cs,
        (u16)(10430.378f * ((3.1415927f * m_fAzimuth) / 180.0f)));

    nlVec3Set(m_vecCamera, distance * cs, distance * sn, z);
    m_vecTarget.z = m_fHeight;
    nlVec3Add(m_vecCamera, m_vecCamera, m_vecTarget);

    glMatrixLookAt(m_matView, m_vecCamera, m_vecTarget, vecUp);
}

extern "C" void fn_800F33DC(
    void*, UnidentifiedDebugCameraTargetEntry* entry)
{
    delete entry;
}

const nlMatrix4& cDebugCamera::GetViewMatrix() const
{
    return m_matView;
}

float cDebugCamera::GetFOV() const
{
    return sfDebugCamFOV;
}

const nlVector3& cDebugCamera::GetCameraPosition() const
{
    return m_vecCamera;
}

const nlVector3& cDebugCamera::GetTargetPosition() const
{
    return m_vecTarget;
}

eCameraType cDebugCamera::GetType()
{
    return eCameraType_Debug;
}
