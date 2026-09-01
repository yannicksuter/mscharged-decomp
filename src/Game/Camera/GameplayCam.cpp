#include "Game/Camera/GameplayCam.h"

#include "Game/Ball.h"
#include "Game/AI/AiUtil.h"
#include "Game/GameInfo.h"
#include "Game/Net.h"
#include "Game/Player.h"
#include "Game/ReplayManager.h"
#include "Game/Team.h"
#include "NL/nlTask.h"
#include "NL/gl/glMatrix.h"

extern "C" bool fn_80273B00();

bool lbl_806DC4F0 = true;
float lbl_806DC4F4 = 1.0f;
float lbl_806DC4F8 = 0.05f;
float lbl_806DC4FC = 0.5f;
float lbl_806DC500 = 0.2f;
float lbl_806DC504 = 0.2f;
float lbl_806DC508 = 30.0f;
float lbl_806DC50C = 38.0f;

float lbl_806E0F10;
bool lbl_806E0F14;
bool gGameplayCameraInReplay;

static const float nearZoomPositiveTargetKnotsY[3] = { -5.2f, 0.0f, 10.0f };
static const float nearZoomPositiveFieldKnotsY[3] = { -5.6f, 0.0f, 11.65f };
static const float farZoomPositiveTargetKnotsY[3] = { -2.6f, -2.6f, 2.0f };
static const float farZoomPositiveFieldKnotsY[3] = { 0.0f, 0.0f, 11.65f };

static const float nearZoomPositiveTargetKnotsX[2] = { -16.8f, 16.8f };
static const float nearZoomPositiveFieldKnotsX[2] = { -17.0f, 14.5f };
static const float nearZoomNegativeFieldKnotsX[2] = { -14.5f, 17.0f };
static const float nearZoomNeutralFieldKnotsX[2] = { -15.75f, 15.75f };
static const float farZoomPositiveTargetKnotsX[2] = { -13.3f, 13.3f };
static const float farZoomPositiveFieldKnotsX[2] = { -13.0f, 8.0f };
static const float farZoomNegativeFieldKnotsX[2] = { -8.0f, 13.0f };
static const float farZoomNeutralFieldKnotsX[2] = { -10.5f, 10.5f };
static const float nearZoomWidePositiveTargetKnotsX[2] = { -15.5f, 15.5f };
static const float farZoomWidePositiveTargetKnotsX[2] = { -10.1f, 10.1f };

static const CameraData gCameraData[4] = {
    {
        2,
        3,
        { nearZoomPositiveTargetKnotsX, nearZoomPositiveTargetKnotsX, nearZoomPositiveTargetKnotsX },
        { nearZoomNeutralFieldKnotsX, nearZoomPositiveFieldKnotsX, nearZoomNegativeFieldKnotsX },
        { nearZoomPositiveTargetKnotsY, nearZoomPositiveTargetKnotsY, nearZoomPositiveTargetKnotsY },
        { nearZoomPositiveFieldKnotsY, nearZoomPositiveFieldKnotsY, nearZoomPositiveFieldKnotsY },
        27.3f,
        20.0f,
        lbl_806DC508,
        270.0f,
    },
    {
        2,
        3,
        { farZoomPositiveTargetKnotsX, farZoomPositiveTargetKnotsX, farZoomPositiveTargetKnotsX },
        { farZoomNeutralFieldKnotsX, farZoomPositiveFieldKnotsX, farZoomNegativeFieldKnotsX },
        { farZoomPositiveTargetKnotsY, farZoomPositiveTargetKnotsY, farZoomPositiveTargetKnotsY },
        { farZoomPositiveFieldKnotsY, farZoomPositiveFieldKnotsY, farZoomPositiveFieldKnotsY },
        27.3f,
        35.0f,
        lbl_806DC50C,
        270.0f,
    },
    {
        2,
        3,
        { nearZoomWidePositiveTargetKnotsX, nearZoomWidePositiveTargetKnotsX, nearZoomWidePositiveTargetKnotsX },
        { nearZoomNeutralFieldKnotsX, nearZoomPositiveFieldKnotsX, nearZoomNegativeFieldKnotsX },
        { nearZoomPositiveTargetKnotsY, nearZoomPositiveTargetKnotsY, nearZoomPositiveTargetKnotsY },
        { nearZoomPositiveFieldKnotsY, nearZoomPositiveFieldKnotsY, nearZoomPositiveFieldKnotsY },
        27.5f,
        20.0f,
        lbl_806DC508,
        270.0f,
    },
    {
        2,
        3,
        { farZoomWidePositiveTargetKnotsX, farZoomWidePositiveTargetKnotsX, farZoomWidePositiveTargetKnotsX },
        { farZoomNeutralFieldKnotsX, farZoomPositiveFieldKnotsX, farZoomNegativeFieldKnotsX },
        { farZoomPositiveTargetKnotsY, farZoomPositiveTargetKnotsY, farZoomPositiveTargetKnotsY },
        { farZoomPositiveFieldKnotsY, farZoomPositiveFieldKnotsY, farZoomPositiveFieldKnotsY },
        27.5f,
        35.0f,
        lbl_806DC50C,
        270.0f,
    },
};

GameplayCamera::GameplayCamera()
{
    m_bDynamicZoom = true;
    m_fZoom = 0.0f;
    m_fDesiredZoom = 0.0f;
    m_fZoomSeekSpeed = 0.0f;
    m_ForceNeutralAndNearZoom = false;
    m_fZoomOverride = 0.0f;
    m_matView.SetIdentity();
}

void GameplayCamera::Update(float deltaTime)
{
    m_bDynamicZoom = GameInfoManager::Instance()->mVisualOptions.mIsAutoZoomCamera;
    m_fDesiredZoom = 1.0f - GameInfoManager::Instance()->mVisualOptions.mCameraZoomLevel;

    bool gamePaused = nlTaskManager::m_pInstance->mCurrentState == 1
                   || nlTaskManager::m_pInstance->mCurrentState == 32;

    if (fn_80273B00())
    {
        m_nearZoom.m_CameraData = gCameraData + 2;
        m_farZoom.m_CameraData = gCameraData + 3;
    }
    else
    {
        m_nearZoom.m_CameraData = gCameraData;
        m_farZoom.m_CameraData = gCameraData + 1;
    }

    m_nearZoom.Update(deltaTime, m_ForceNeutralAndNearZoom);
    m_farZoom.Update(deltaTime, m_ForceNeutralAndNearZoom);

    if (m_fZoomOverride != 0.0f || lbl_806E0F14)
    {
        m_fZoom = m_fZoomOverride;
        if (lbl_806E0F14)
        {
            m_fZoom = lbl_806E0F10;
        }
    }
    else if (m_ForceNeutralAndNearZoom)
    {
        if (g_pTeams[0]->m_nScore == 0 && g_pTeams[1]->m_nScore == 0)
        {
            m_fZoom = lbl_806DC500;
        }
        else
        {
            m_fZoom = lbl_806DC504;
        }
    }
    else
    {
        float clampedDesiredZoom = Interpolate(lbl_806DC4F8, lbl_806DC4F4, m_fDesiredZoom);
        float smoothTime = gamePaused ? 0.1f : 0.75f;
        float omega = 2.0f / smoothTime;
        float x = omega * deltaTime;
        float exp = 1.0f / (((0.48f * x * x) + (1.0f + x)) + (x * (0.235f * x * x)));
        float change = m_fZoom - clampedDesiredZoom;
        float currentVelocity = m_fZoomSeekSpeed;

        m_fZoomSeekSpeed = exp * (currentVelocity - (omega * (deltaTime * ((omega * change) + currentVelocity))));
        m_fZoom = (exp * (change + (deltaTime * ((omega * change) + currentVelocity)))) + clampedDesiredZoom;
    }

    float inverseZoom = 1.0f - m_fZoom;
    float zoom = m_fZoom;

    m_v3Target.x = (inverseZoom * m_nearZoom.m_v3Target.x) + (zoom * m_farZoom.m_v3Target.x);
    m_v3Target.y = (inverseZoom * m_nearZoom.m_v3Target.y) + (zoom * m_farZoom.m_v3Target.y);
    m_v3Target.z = (inverseZoom * m_nearZoom.m_v3Target.z) + (zoom * m_farZoom.m_v3Target.z);

    m_v3Camera.x = (inverseZoom * m_nearZoom.m_v3Camera.x) + (zoom * m_farZoom.m_v3Camera.x);
    m_v3Camera.y = (inverseZoom * m_nearZoom.m_v3Camera.y) + (zoom * m_farZoom.m_v3Camera.y);
    m_v3Camera.z = (inverseZoom * m_nearZoom.m_v3Camera.z) + (zoom * m_farZoom.m_v3Camera.z);

    m_fFOV = Interpolate(m_nearZoom.m_CameraData->fov, m_farZoom.m_CameraData->fov, m_fZoom);
    glMatrixLookAt(m_matView, m_v3Camera, m_v3Target, mUpVector);
}

void GameplayCamera::SetForceNeutralAndNearZoom(bool forceNeutralAndNearZoom)
{
    if (forceNeutralAndNearZoom && !m_ForceNeutralAndNearZoom)
    {
        Reactivate();
        m_v3Target.x = 0.0f;
        m_v3Target.y = 0.0f;
        m_v3Target.z = 0.0f;
    }
    m_ForceNeutralAndNearZoom = forceNeutralAndNearZoom;
}

void GameplayCamera::Reactivate()
{
    m_nearZoom.CalcDesiredTarget();
    m_nearZoom.m_fDampenedTargetX = m_nearZoom.m_fDesiredTargetX;
    m_nearZoom.m_fDampenedTargetY = m_nearZoom.m_fDesiredTargetY;

    m_farZoom.CalcDesiredTarget();
    m_farZoom.m_fDampenedTargetX = m_farZoom.m_fDesiredTargetX;
    m_farZoom.m_fDampenedTargetY = m_farZoom.m_fDesiredTargetY;

    if (m_pFilter[1] != NULL)
    {
        m_pFilter[1]->Reset();
    }
}

static inline float MapFromFieldPosToTargetPos(float fPos, const float* pFieldKnots, const float* pTargetKnots, int nNumKnots)
{
    float fMin = pFieldKnots[0];
    float fMax = pFieldKnots[nNumKnots - 1] - 0.001f;

    fPos = (fPos >= fMin) ? fPos : fMin;
    fPos = (fPos <= fMax) ? fPos : fMax;

    int nKnot;
    for (nKnot = 0; nKnot < nNumKnots - 1; nKnot++)
    {
        if (fPos < pFieldKnots[nKnot + 1])
        {
            break;
        }
    }

    float fKnotPercent;
    if (pFieldKnots[nKnot] == pFieldKnots[nKnot + 1])
    {
        fKnotPercent = 0.0f;
    }
    else
    {
        fKnotPercent = (fPos - pFieldKnots[nKnot]) / (pFieldKnots[nKnot + 1] - pFieldKnots[nKnot]);
    }

    return Interpolate(pTargetKnots[nKnot], pTargetKnots[nKnot + 1], fKnotPercent);
}

void GameplayCameraZoomLevel::CalcDesiredTarget()
{
    nlVector3 v3OOIPos = { 0.0f, 0.0f, 0.0f };
    float fKnotTableBlendWeights[3];
    float fCurrWeight;
    float fDampenedBlendRiser;
    float fAccumulatedWeight;
    float fBlendPercent;
    float* pKnotTableBlendWeights;
    int i;

    if (!gGameplayCameraInReplay)
    {
        if (g_pBall != NULL)
        {
            cPlayer* pBallOwner = g_pBall->m_pOwner;
            if (pBallOwner != NULL)
            {
                v3OOIPos = pBallOwner->m_v3Position;
            }
            else if (ReplayManager::Instance()->mRender != NULL)
            {
                v3OOIPos = ReplayManager::Instance()->mRender->mBall.mPosition;
            }
            else
            {
                v3OOIPos = g_pBall->m_v3Position;
            }
        }
    }
    else if (ReplayManager::Instance()->mRender != NULL)
    {
        v3OOIPos = ReplayManager::Instance()->mRender->mBall.mPosition;
    }

    pKnotTableBlendWeights = fKnotTableBlendWeights;

    pKnotTableBlendWeights[0] = 0.0f;
    pKnotTableBlendWeights[1] = 0.0f;
    pKnotTableBlendWeights[2] = 0.0f;

    fCurrWeight = 1.0f;
    for (int j = 0; j < 5; j++)
    {
        fDampenedBlendRiser = m_KnotTableBlendQueue[j].fBlendRiser;
        pKnotTableBlendWeights[m_KnotTableBlendQueue[j].nKnotTable] += fCurrWeight * fDampenedBlendRiser;
        if (fDampenedBlendRiser == 1.0f)
        {
            break;
        }
        fCurrWeight *= 1.0f - fDampenedBlendRiser;
    }

    fAccumulatedWeight = 0.0f;
    m_fDesiredTargetX = 0.0f;
    m_fDesiredTargetY = 0.0f;

    i = 0;
    for (; i < 3; pKnotTableBlendWeights++, i++)
    {
        if (*pKnotTableBlendWeights > 0.0f)
        {
            fAccumulatedWeight += *pKnotTableBlendWeights;
            float fMappedX = MapFromFieldPosToTargetPos(v3OOIPos.x, m_CameraData->fieldKnotsX[i], m_CameraData->targetKnotsX[i], m_CameraData->numKnotsX);
            float fMappedY = MapFromFieldPosToTargetPos(v3OOIPos.y, m_CameraData->fieldKnotsY[i], m_CameraData->targetKnotsY[i], m_CameraData->numKnotsY);
            fBlendPercent = *pKnotTableBlendWeights / fAccumulatedWeight;

            m_fDesiredTargetX = Interpolate(m_fDesiredTargetX, fMappedX, fBlendPercent);
            m_fDesiredTargetY = Interpolate(m_fDesiredTargetY, fMappedY, fBlendPercent);
        }
    }
}

static void CalcCurrentKnotTable(GameplayCameraZoomLevel* self, bool forceNeutral)
{
    cPlayer* pBallOwner;
    if (g_pBall != NULL)
    {
        pBallOwner = g_pBall->m_pOwner;
    }
    else
    {
        pBallOwner = NULL;
    }

    if (pBallOwner == NULL)
    {
        pBallOwner = g_pBall->m_pPassTarget;
    }

    int nNewKnotTable;
    if (pBallOwner != NULL && !forceNeutral)
    {
        if (pBallOwner->m_pTeam->GetOtherNet()->m_v3NetLocation.x > 0.0f)
        {
            nNewKnotTable = 1;
        }
        else
        {
            nNewKnotTable = 2;
        }
    }
    else
    {
        nNewKnotTable = 0;
    }

    if (nNewKnotTable != self->m_KnotTableBlendQueue[0].nKnotTable)
    {
        for (int i = 4; i > 0; i--)
        {
            self->m_KnotTableBlendQueue[i] = self->m_KnotTableBlendQueue[i - 1];
        }
        self->m_KnotTableBlendQueue[0].nKnotTable = nNewKnotTable;
        self->m_KnotTableBlendQueue[0].fBlendRiser = 0.0f;
    }
}

void GameplayCameraZoomLevel::Update(float fDeltaT, bool forceNeutral)
{
    if (gGameplayCameraInReplay)
    {
        forceNeutral = true;
    }

    CalcCurrentKnotTable(this, forceNeutral);

    if (forceNeutral)
    {
        m_KnotTableBlendQueue[0].fBlendRiser = 1.0f;
    }

    float t = fDeltaT / 0.75f;
    for (int i = 0; i < 5; i++)
    {
        m_KnotTableBlendQueue[i].fBlendRiser += t;
        if (m_KnotTableBlendQueue[i].fBlendRiser >= 1.0f)
        {
            m_KnotTableBlendQueue[i].fBlendRiser = 1.0f;
            break;
        }
    }

    CalcDesiredTarget();

    if (!forceNeutral)
    {
        float omega = 2.0f / m_fTargetSeekTime;
        float x = omega * fDeltaT;
        float exp = 1.0f / (((0.48f * x * x) + (1.0f + x)) + (x * (0.235f * x * x)));

        float change = m_fDampenedTargetX - m_fDesiredTargetX;
        float currentVelocity = m_fTargetSeekSpeedX;
        m_fTargetSeekSpeedX = exp * (currentVelocity - (omega * (fDeltaT * ((omega * change) + currentVelocity))));
        m_fDampenedTargetX = (exp * (change + (fDeltaT * ((omega * change) + currentVelocity)))) + m_fDesiredTargetX;

        change = m_fDampenedTargetY - m_fDesiredTargetY;
        currentVelocity = m_fTargetSeekSpeedY;
        m_fTargetSeekSpeedY = exp * (currentVelocity - (omega * (fDeltaT * ((omega * change) + currentVelocity))));
        m_fDampenedTargetY = (exp * (change + (fDeltaT * ((omega * change) + currentVelocity)))) + m_fDesiredTargetY;
    }
    else
    {
        m_fDampenedTargetX = m_fDesiredTargetX;
        m_fDampenedTargetY = m_fDesiredTargetY;
    }

    float fSin;
    float fCos;
    float fOrientSin;
    float fOrientCos;
    nlSinCos(&fSin, &fCos, ((s32)(65536.0f * m_CameraData->pitch)) / 360);
    nlSinCos(&fOrientSin, &fOrientCos, ((s32)(65536.0f * m_CameraData->orientation)) / 360);

    float fXYDist = fCos * m_CameraData->distance;
    m_v3Camera.x = (fOrientCos * fXYDist) + m_fDampenedTargetX;
    m_v3Camera.y = (fOrientSin * fXYDist) + m_fDampenedTargetY;
    m_v3Camera.z = fSin * m_CameraData->distance;

    m_v3Target.x = m_fDampenedTargetX;
    m_v3Target.y = m_fDampenedTargetY;
    m_v3Target.z = 0.0f;
}
