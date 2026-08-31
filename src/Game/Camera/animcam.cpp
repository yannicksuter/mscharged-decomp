#include "Game/Camera/animcam.h"

#include "Game/AI/AiUtil.h"
#include "Game/Render/depthoffield.h"
#include "NL/gl/glMatrix.h"
#include "NL/nlList.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/platqmath.h"
#include "NL/platvmath.h"

static float dofBehindTarget = 2.0f;
static float lbl_806DC464 = 45.0f;
static float lbl_806DC468 = 1.0f;
static float lbl_806DC46C = 1.0f;

cCameraData* cAnimCamera::m_cameraDataList;

extern "C" bool fn_80273B00();

struct UnidentifiedCameraDisplayState
{
    u8 padding_0x00[0xC4];
    bool field_0xC4;
};

extern "C" UnidentifiedCameraDisplayState* fn_80284A58();
extern "C" float fn_800F2410(float fFOV);

struct UnidentifiedSimulationTimeProvider
{
    u8 padding_0x00[0x2C];
    float mSimulationTime;
};

extern "C" UnidentifiedSimulationTimeProvider* fn_8011166C();

static void EnableDofDebug()
{
    if (DepthOfFieldManager::instance.m_bDebugView)
    {
        DepthOfFieldManager::instance.m_bDebugView = 0;
    }
    else
    {
        DepthOfFieldManager::instance.m_bDebugView = 1;
    }
}

struct DofDebugFlag
{
    DofDebugFlag()
    {
        m_active = false;
        m_callback = EnableDofDebug;
    }

    bool m_active;
    void (*m_callback)();
};

static DofDebugFlag g_EnableDofDebug;

cCameraData::cCameraData()
{
    next = NULL;
    m_uHashID = 0;
    m_uKeyCount = 0;
    field_0x0C = NULL;
    cameraPos = NULL;
    targetPos = NULL;
    cameraRot = NULL;
    fFOV = NULL;
    fFocalLength = NULL;
}

cCameraData::~cCameraData()
{
    if (ownsKeyData)
    {
        delete[] cameraPos;
        delete[] targetPos;
        delete[] cameraRot;
        delete[] fFOV;
        delete[] fFocalLength;
        delete[] field_0x0C;
    }
}

static bool LoadAnimCameraData(nlChunk* outerChunk, nlChunk* outerEnd, cCameraData* pAnimCameraData, bool ownsKeyData)
{
    pAnimCameraData->ownsKeyData = ownsKeyData;
    while (outerChunk < outerEnd)
    {
        u32 id = outerChunk->m_ID;
        u32 type = id & 0x80FFFFFF;
        switch (type)
        {
        case 0x2500C:
            pAnimCameraData->m_uKeyCount = *(u32*)outerChunk->GetData();
            if (ownsKeyData)
            {
                pAnimCameraData->cameraPos = (nlVector3*)nlMalloc(pAnimCameraData->m_uKeyCount * sizeof(nlVector3), 8, false);
                pAnimCameraData->targetPos = (nlVector3*)nlMalloc(pAnimCameraData->m_uKeyCount * sizeof(nlVector3), 8, false);
                pAnimCameraData->cameraRot = (nlQuaternion*)nlMalloc(pAnimCameraData->m_uKeyCount * sizeof(nlQuaternion), 8, false);
                pAnimCameraData->fFOV = (float*)nlMalloc(pAnimCameraData->m_uKeyCount * sizeof(float), 8, false);
                pAnimCameraData->fFocalLength = (float*)nlMalloc(pAnimCameraData->m_uKeyCount * sizeof(float), 8, false);
            }
            else
            {
                pAnimCameraData->cameraPos = NULL;
                pAnimCameraData->targetPos = NULL;
                pAnimCameraData->cameraRot = NULL;
                pAnimCameraData->fFOV = NULL;
                pAnimCameraData->fFocalLength = NULL;
            }
            break;
        case 0x25003:
        {
            nlVector3* v3Pos = (nlVector3*)outerChunk->GetData();
            if (ownsKeyData)
            {
                unsigned long offset;
                unsigned long i;
                i = 0;
                offset = 0;
                while (i < pAnimCameraData->m_uKeyCount)
                {
                    *(nlVector3*)((u8*)pAnimCameraData->cameraPos + offset) = *v3Pos;
                    i++;
                    offset += sizeof(nlVector3);
                    v3Pos++;
                }
            }
            else
            {
                pAnimCameraData->cameraPos = v3Pos;
            }
            break;
        }
        case 0x25006:
        {
            nlVector3* v3Pos = (nlVector3*)outerChunk->GetData();
            if (ownsKeyData)
            {
                unsigned long offset;
                unsigned long i;
                i = 0;
                offset = 0;
                while (i < pAnimCameraData->m_uKeyCount)
                {
                    *(nlVector3*)((u8*)pAnimCameraData->targetPos + offset) = *v3Pos;
                    i++;
                    offset += sizeof(nlVector3);
                    v3Pos++;
                }
            }
            else
            {
                pAnimCameraData->targetPos = v3Pos;
            }
            break;
        }
        case 0x25004:
        {
            nlQuaternion* rot = (nlQuaternion*)outerChunk->GetData();
            if (ownsKeyData)
            {
                unsigned long offset;
                unsigned long i;
                i = 0;
                offset = 0;
                while (i < pAnimCameraData->m_uKeyCount)
                {
                    *(nlQuaternion*)((u8*)pAnimCameraData->cameraRot + offset) = *rot;
                    i++;
                    offset += sizeof(nlQuaternion);
                    rot++;
                }
            }
            else
            {
                pAnimCameraData->cameraRot = rot;
            }
            break;
        }
        case 0x25009:
            if (ownsKeyData)
            {
                unsigned long offset;
                unsigned long i;
                i = 0;
                offset = 0;
                while (i < pAnimCameraData->m_uKeyCount)
                {
                    float* src = (float*)outerChunk->GetData();
                    *(float*)((u8*)pAnimCameraData->fFOV + offset) = *(float*)((u8*)src + offset);
                    i++;
                    offset += sizeof(float);
                }
            }
            else
            {
                pAnimCameraData->fFOV = (float*)outerChunk->GetData();
            }
            break;
        case 0x2500A:
            if (ownsKeyData)
            {
                unsigned long offset;
                unsigned long i;
                i = 0;
                offset = 0;
                while (i < pAnimCameraData->m_uKeyCount)
                {
                    float* src = (float*)outerChunk->GetData();
                    *(float*)((u8*)pAnimCameraData->fFocalLength + offset) = *(float*)((u8*)src + offset);
                    i++;
                    offset += sizeof(float);
                }
            }
            else
            {
                pAnimCameraData->fFocalLength = (float*)outerChunk->GetData();
            }
            break;
        case 0x25000:
            if (ownsKeyData)
            {
                pAnimCameraData->field_0x0C = (char*)nlMalloc(32, 8, false);
                nlStrNCpy(pAnimCameraData->field_0x0C, (char*)outerChunk->GetData(), 32);
            }
            else
            {
                pAnimCameraData->field_0x0C = (char*)outerChunk->GetData();
            }
            break;
        }
        outerChunk = outerChunk->GetNextChunk();
    }
    return true;
}

bool cAnimCamera::LoadCameraAnimation(nlChunk* begin, unsigned long, const char* cameraName, bool ownsKeyData)
{
    nlChunk* end = begin->GetNextChunk();
    nlChunk* first = begin->GetFirstChunk();

    cCameraData* pData = new (8, false) cCameraData();
    pData->m_uHashID = nlStringLowerHash(cameraName);
    bool b = LoadAnimCameraData(first, end, pData, ownsKeyData);
    nlListAddStart(&m_cameraDataList, pData, (cCameraData**)NULL);
    if (ownsKeyData)
    {
        nlFree(begin);
    }
    return b;
}

void cAnimCamera::FreeCameraAnimations()
{
    nlDeleteList(&m_cameraDataList);
    m_cameraDataList = NULL;
}

cAnimCamera::cAnimCamera()
{
    m_bUseSimulationTime = false;
    m_LetManagerDoUpdate = true;
    m_bUnusedPad = false;
    m_fAnimationTime = 0.0f;
    m_fAnimationSpeed = 1.0f;
    m_fLastSimulationTime = -1.0f;
    m_pActiveCameraData = NULL;
    mFacingAngle = 0;
    m_EndOfAnimationCallback = NULL;
    nlVec3Set(m_vecTarget, 0.0f, 0.0f, 0.0f);
    nlVec3Set(m_OffsetPos, 0.0f, 0.0f, 0.0f);
    nlVec3Set(m_Mirror, 1.0f, 1.0f, 1.0f);
    m_pActiveCameraData = m_cameraDataList;
    m_bCyclic = true;
    m_matView.SetIdentity();
}

cAnimCamera::~cAnimCamera()
{
}

void cAnimCamera::SetAnimationTime(float fTime, bool bUpdateView)
{
    m_fAnimationTime = fTime;
    if (bUpdateView)
    {
        BuildAnimViewMatrix(m_matView);
    }
}

void cAnimCamera::BuildAnimViewMatrix(nlMatrix4& mView)
{
    float fRealIndex = m_fAnimationTime * (float)(m_pActiveCameraData->m_uKeyCount - 1);
    int nIndex = (int)fRealIndex;
    float fWeightB = fRealIndex - (float)nIndex;
    float fWeightA = 1.0f - fWeightB;
    nlVector3 cameraPos = { };
    nlVector3 targetPos = { };
    nlQuaternion cameraRot = { 0.0f, 0.0f, 0.0f, 1.0f };
    nlMatrix4 viewMatrix;
    nlMatrix4 facingAngleMatrix;
    float focalLength;
    if (m_fAnimationTime >= 1.0f)
    {
        m_Fov = m_pActiveCameraData->fFOV[nIndex];
        cameraPos = m_pActiveCameraData->cameraPos[nIndex];
        targetPos = m_pActiveCameraData->targetPos[nIndex];
        cameraRot = m_pActiveCameraData->cameraRot[nIndex];
        focalLength = m_pActiveCameraData->fFocalLength[nIndex];
    }
    else
    {
        nlVector3& cpN = m_pActiveCameraData->cameraPos[nIndex + 1];
        nlVector3& cpK = m_pActiveCameraData->cameraPos[nIndex];
        nlVector3 delta;
        nlVec3Sub(delta, cpK, cpN);
        float distSq = delta.GetLengthSq3D();
        if (distSq > 16.0f)
        {
            if (fWeightB < 0.5f)
            {
                cameraPos = m_pActiveCameraData->cameraPos[nIndex];
                targetPos = m_pActiveCameraData->targetPos[nIndex];
                cameraRot = m_pActiveCameraData->cameraRot[nIndex];
                m_Fov = m_pActiveCameraData->fFOV[nIndex];
                focalLength = m_pActiveCameraData->fFocalLength[nIndex];
            }
            else
            {
                cameraPos = m_pActiveCameraData->cameraPos[nIndex + 1];
                targetPos = m_pActiveCameraData->targetPos[nIndex + 1];
                cameraRot = m_pActiveCameraData->cameraRot[nIndex + 1];
                m_Fov = m_pActiveCameraData->fFOV[nIndex + 1];
                focalLength = m_pActiveCameraData->fFocalLength[nIndex + 1];
            }
        }
        else
        {
            m_Fov = fWeightA * m_pActiveCameraData->fFOV[nIndex] + fWeightB * m_pActiveCameraData->fFOV[nIndex + 1];
            nlVec3WeightedSum(cameraPos, fWeightA, m_pActiveCameraData->cameraPos[nIndex], fWeightB, m_pActiveCameraData->cameraPos[nIndex + 1]);
            nlVec3WeightedSum(targetPos, fWeightA, m_pActiveCameraData->targetPos[nIndex], fWeightB, m_pActiveCameraData->targetPos[nIndex + 1]);
            nlQuatSlerp(cameraRot, m_pActiveCameraData->cameraRot[nIndex], m_pActiveCameraData->cameraRot[nIndex + 1], fWeightB);
            focalLength = fWeightA * m_pActiveCameraData->fFocalLength[nIndex] + fWeightB * m_pActiveCameraData->fFocalLength[nIndex + 1];
        }
    }

    if (fn_80273B00())
    {
        UnidentifiedCameraDisplayState* state = fn_80284A58();
        if (state->field_0xC4)
        {
            m_Fov = fn_800F2410(m_Fov);
        }
    }

    cameraPos.x *= m_Mirror.x;
    cameraPos.y *= m_Mirror.y;
    cameraPos.z *= m_Mirror.z;
    targetPos.x *= m_Mirror.x;
    targetPos.y *= m_Mirror.y;
    targetPos.z *= m_Mirror.z;
    if (m_Mirror.x != 1.0f || m_Mirror.y != 1.0f || m_Mirror.z != 1.0f)
    {
        cameraRot.x *= -m_Mirror.x;
        cameraRot.y *= -m_Mirror.y;
        cameraRot.z *= -m_Mirror.z;
    }
    nlVec3Add(m_vecCamera, cameraPos, m_OffsetPos);
    nlVec3Add(m_vecTarget, targetPos, m_OffsetPos);
    GetLocalPoint(m_vecCamera, m_vecCamera, m_OffsetPos, 0);
    GetWorldPoint(m_vecCamera, m_vecCamera, m_OffsetPos, mFacingAngle);
    if (m_bUnusedPad)
    {
        nlVector3 up = { 0.0f, 0.0f, 1.0f };
        glMatrixLookAt(mView, m_vecCamera, m_vecTarget, up);
    }
    else
    {
        nlQuatToMatrix(viewMatrix, cameraRot, true);
        float facingAngleRadians = (float)mFacingAngle * 0.0000958738f;
        nlMakeRotationMatrixZ(facingAngleMatrix, facingAngleRadians);
        nlMultMatrices(viewMatrix, viewMatrix, facingAngleMatrix);
        viewMatrix.m41 = m_vecCamera.x;
        viewMatrix.m42 = m_vecCamera.y;
        viewMatrix.m43 = m_vecCamera.z;
        viewMatrix.m44 = 1.0f;
        nlInvertMatrix(mView, viewMatrix);
    }

    float fScale = lbl_806DC464 / m_Fov;
    fScale = (1.0f - lbl_806DC468) * 1.0f + fScale * lbl_806DC468;
    fScale *= fScale;
    lbl_806DC46C = fScale;
    m_FocalLength = dofBehindTarget * fScale + focalLength;
    DepthOfFieldManager::instance.m_fDistanceFromCamera = m_FocalLength;
}

void cAnimCamera::UnselectCameraAnimation()
{
    m_pActiveCameraData = NULL;
}

void cAnimCamera::SelectCameraAnimation(const char* name)
{
    m_fLastSimulationTime = -1.0f;
    u32 hash = nlStringLowerHash(name);

    cCameraData* pData = m_cameraDataList;
    while (pData != NULL)
    {
        if (pData->m_uHashID == hash)
        {
            m_pActiveCameraData = pData;
            return;
        }
        pData = pData->next;
    }

    m_fAnimationTime = 0.0f;
    BuildAnimViewMatrix(m_matView);
}

void cAnimCamera::Update(float dt)
{
    if (!m_LetManagerDoUpdate)
    {
        return;
    }
    ManualUpdate(dt);
}

float cAnimCamera::ManualUpdate(float dt)
{
    float overrun = 0.0f;
    if (m_bUseSimulationTime)
    {
        if (m_fLastSimulationTime < 0.0f)
        {
            m_fLastSimulationTime = fn_8011166C()->mSimulationTime;
        }
        else
        {
            UnidentifiedSimulationTimeProvider* provider = fn_8011166C();
            cCameraData* pData = m_pActiveCameraData;
            float simTime = provider->mSimulationTime;
            float delta = simTime - m_fLastSimulationTime;
            float duration;
            if (pData != NULL)
            {
                duration = (float)pData->m_uKeyCount / 30.0f;
            }
            else
            {
                duration = 0.0f;
            }
            m_fAnimationTime += (delta * m_fAnimationSpeed) / duration;
            m_fLastSimulationTime = simTime;
        }
    }
    else
    {
        float duration;
        if (m_pActiveCameraData != NULL)
        {
            duration = (float)m_pActiveCameraData->m_uKeyCount / 30.0f;
        }
        else
        {
            duration = 0.0f;
        }
        m_fAnimationTime += (dt * m_fAnimationSpeed) / duration;
    }

    if (m_fAnimationTime >= 1.0f)
    {
        float duration;
        if (m_pActiveCameraData != NULL)
        {
            duration = (float)m_pActiveCameraData->m_uKeyCount / 30.0f;
        }
        else
        {
            duration = 0.0f;
        }
        overrun = (m_fAnimationTime - 1.0f) * duration;
        if (m_bCyclic)
        {
            m_fAnimationTime -= 1.0f;
        }
        else
        {
            m_fAnimationTime = 1.0f;
        }
        if (m_EndOfAnimationCallback != NULL)
        {
            m_EndOfAnimationCallback();
        }
    }

    BuildAnimViewMatrix(m_matView);
    return overrun;
}
