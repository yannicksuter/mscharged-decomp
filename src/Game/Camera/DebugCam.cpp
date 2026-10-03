#include "NL/plat/PlatPadManager.h"
#include "NL/plat/WiiPad.h"
#include "Game/Camera/DebugCam.h"

#include "Game/CharacterTemplate.h"
#include "Game/Player.h"
#include "Game/RenderSnapshot.h"
#include "Game/ReplayManager.h"
#include "Game/Team.h"
#include "Game/MathHelpers.h"
#include "Game/Task/ProfilerTask.h"
#include "Game/Task/TweakerTask.h"
#include "Game/TweakValue.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "Game/TweakValueFloat.h"
#include "NL/globalpad.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "Game/UnidentifiedTweakAction.h"
#include "Game/UnidentifiedStaticStorage.h"

static void ApplyDebugCameraFOV();

struct DebugCameraTarget
{
    u8 mUnidentified00[0x20];
    nlVector3 mPosition;
};

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

float sfDebugCamFOV = 60.0f;
static float sfControlSpeedScale = 100.0f;
static float sfControlDistanceScale = 1.0f;
static float sfControlHeightScale = 1.0f;
static float sfTargetFollowStep = 1.0f;

static u32 sLightRampTexture = glGetTexture("global/lightramp");
static u32 sBlackTexture = glGetTexture("global/black");
static u32 sWhiteTexture = glGetTexture("global/white");

static TweakValueFloat gDebugCameraSensitivity(
    "gDebugCameraSensitivity", "Controller Config/DPD", 3.0f);

static float sDebugCamFOVTweak = sfDebugCamFOV;
static UnidentifiedTweakAction sDebugCamFOVAction(
    "Fov", gLastTweakCategory, Function0<void>(ApplyDebugCameraFOV));
static TweakFloatBinding sSpeed0(
    "Speed 0", "Controller Config/DPD", &sControlTweakValues.speed0);
static TweakFloatBinding sWeight0(
    "Weight 0", "Controller Config/DPD", &sControlTweakValues.weight0);
static TweakFloatBinding sSpeed1(
    "Speed 1", "Controller Config/DPD", &sControlTweakValues.speed1);
static TweakFloatBinding sWeight1(
    "Weight 1", "Controller Config/DPD", &sControlTweakValues.weight1);
static TweakFloatBinding sSpeed2(
    "Speed 2", "Controller Config/DPD", &sControlTweakValues.speed2);
static TweakFloatBinding sWeight2(
    "Weight 2", "Controller Config/DPD", &sControlTweakValues.weight2);

static u32 sSightTexture = nlStringLowerHash("global/sight");
static u32 sLightBlobTexture = nlStringLowerHash("global/light_blob");

static void ApplyDebugCameraFOV()
{
    sfDebugCamFOV = sDebugCamFOVTweak;
}

cDebugCamera::cDebugCamera(bool)
    : m_fRadius(10.0f)
    , m_fAzimuth(215.0f)
    , m_fTheta(25.0f)
    , m_fHeight(0.0f)
    , m_pPad(0)
    , m_bUseWiiControls(false)
    , m_bEnableControls(true)
    , mUnidentified8E(false)
    , m_bUpdateTargets(false)
    , m_pTarget(0)
{
    nlVec3Set(m_vecTarget, 0.0f, 0.0f, 0.0f);
    m_matView.SetIdentity();
    Update(0.0f);

    cGlobalPad* pPad = g_pPadManager->GetPad(0);
    const int classID = pPad->mBackend->GetClassID();
    const bool bEnableDPD =
        classID == gWiiRemotePadClassID || classID == gWiiFreestylePadClassID;
    if (bEnableDPD)
    {
        g_pPlatPadManager->SetDPDEnabled(pPad->m_padIndex, true);
    }
    mUnidentified8E = true;
}

static inline void ClearTargetEntries(nlDLListContainer<DebugCameraTarget*>& targets)
{
    targets.Clear();
}

cDebugCamera::~cDebugCamera()
{
    nlDLListIterator<DebugCameraTarget*> iterator = m_Targets.Begin();
    while (iterator.hasNext())
    {
        delete *iterator;
        iterator.Step();
    }
    ClearTargetEntries(m_Targets);

    if (m_bUseWiiControls)
    {
        g_pPlatPadManager->SetDPDEnabled(m_pPad->m_padIndex, false);
    }
}

void cDebugCamera::UpdateTargetPositions()
{
    if (!m_bUpdateTargets)
    {
        return;
    }

    nlDLListIterator<DebugCameraTarget*> iterator = m_Targets.Begin();
    iterator.Step();

    DebugCameraTarget* target;
    for (int i = 0; i < 10; i++)
    {
        target = *iterator;

        nlVector3 position;
        if (ReplayManager::Instance()->mRender != 0)
        {
            cPlayer* player = (cPlayer*)g_pCharacters[i];
            if (player->m_eClassType == FIELDER)
            {
                int index = player->mUnidentified1E4.m_ID + 4 * player->m_pTeam->m_nSide;
                position = ReplayManager::Instance()->mRender->mCharacters[index].position;
            }
            else if (player->m_eClassType == GOALIE)
            {
                int side = player->m_pTeam->m_nSide;
                position = ReplayManager::Instance()->mRender->mCharacters[side + 8].position;
            }
        }

        if (target != 0)
        {
            target->mPosition = position;
            iterator.Step();
        }
    }

    for (int i = 0; i < 2; i++)
    {
        iterator.Step();
    }
    if (target != 0)
    {
        DebugCameraTarget* ballTarget = *iterator;
        nlVector3 position = { 0.0f, 0.0f, 0.0f };
        if (ReplayManager::Instance()->mRender != 0)
        {
            position = ReplayManager::Instance()->mRender->mBall.mPosition;
        }
        ballTarget->mPosition = position;
    }
}

void cDebugCamera::UpdateOrbitControls(float dt)
{
    float x = 0.0f;
    float y = 0.0f;

    if (!m_bUseWiiControls)
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

void cDebugCamera::UpdatePanControls(float dt, float controlSpeed)
{
    float x = 0.0f;
    float y = 0.0f;

    int classID = m_pPad->mBackend->GetClassID();
    if (classID != gWiiFreestylePadClassID)
    {
        x = m_pPad->AnalogLeftX();
        y = m_pPad->AnalogLeftY();
    }
    else
    {
        g_pPlatPadManager->GetFreestyleStatus(0);
        if (m_pPad->IsPressed(0x400, false))
        {
            x = m_pPad->AnalogLeftX();
            y = m_pPad->AnalogLeftY();
        }
    }

    if (x != 0.0f || y != 0.0f)
    {
        m_pTarget = 0;
    }

    nlVector3 offset;
    m_matView.GetColumn_(0, offset);
    nlVec3Scale(offset, offset, dt * (x * controlSpeed));
    offset.z = 0.0f;
    nlVec3Add(m_vecTarget, m_vecTarget, offset);

    m_matView.GetColumn_(2, offset);
    nlVec3Scale(offset, offset, dt * (-y * controlSpeed));
    offset.z = 0.0f;
    nlVec3Add(m_vecTarget, m_vecTarget, offset);
}

void cDebugCamera::UpdateRadiusAndHeightControls(float dt, float controlSpeed)
{
    bool heightControls = m_bUseWiiControls
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
        float delta = up - down;
        m_fHeight += dt * (delta * controlSpeed);
    }

    if (m_fHeight < 0.0f)
    {
        m_fHeight = 0.0f;
    }
    if (m_fRadius < 0.001)
    {
        m_fRadius = 0.001f;
    }
}

void cDebugCamera::Update(float dt)
{
    nlVector3 vecUp;
    float sn;
    float cs;
    float x;
    float y;

    float controlSpeed = sfControlDistanceScale *
        (1.0f + sfControlHeightScale * (m_fRadius + m_fHeight));

    if (!g_bTweaking && !IsProfiling())
    {
        m_pPad = g_pPadManager->GetPad(0);
        int classID = m_pPad->mBackend->GetClassID();
        m_bUseWiiControls = classID == gWiiRemotePadClassID || classID == gWiiFreestylePadClassID;

        float yPressure = m_pPad->GetPressure(3, true);
        float xPressure = m_pPad->GetPressure(2, true);
        if ((yPressure > 0.0f && m_pPad->PlatJustPressed(2, true)) ||
            (xPressure > 0.0f && m_pPad->PlatJustPressed(3, true)))
        {
            m_bEnableControls = !m_bEnableControls;
        }

        if (m_Targets.m_Head != 0)
        {
            DLListEntry<DebugCameraTarget*>* entry = m_pTargetEntry;
            if (m_pPad->PlatJustPressed(13, true))
            {
                if (entry != 0)
                {
                    entry = entry->m_prev;
                }
                m_pTarget = entry->entry;
            }
            if (m_pPad->PlatJustPressed(14, true))
            {
                if (entry != 0)
                {
                    entry = entry->m_next;
                }
                m_pTarget = entry->entry;
            }
            m_pTargetEntry = entry;
        }

        if (m_bEnableControls)
        {
            UpdateOrbitControls(dt);
            UpdatePanControls(dt, controlSpeed);
            UpdateRadiusAndHeightControls(dt, controlSpeed);
        }
    }

    UpdateTargetPositions();

    if (m_pTarget != 0)
    {
        float stepSquared = sfTargetFollowStep * sfTargetFollowStep;
        float distanceSquared = CalculateDistanceSquared(
            m_vecTarget, m_pTarget->mPosition);
        if (distanceSquared < stepSquared)
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

    nlVec3Set(vecUp, 0.0f, 0.0f, 1.0f);

    nlSinCos(&sn, &cs,
        (s16)(10430.378f * DegreesToRadians(m_fTheta)));
    float z = m_fRadius * sn;
    float d = m_fRadius * cs;
    nlSinCos(&sn, &cs,
        (s16)(10430.378f * DegreesToRadians(m_fAzimuth)));

    x = d * cs;
    y = d * sn;
    nlVec3Set(m_vecCamera, x, y, z);
    m_vecTarget.z = m_fHeight;
    nlVec3Add(m_vecCamera, m_vecCamera, m_vecTarget);

    glMatrixLookAt(m_matView, m_vecCamera, m_vecTarget, vecUp);
}
