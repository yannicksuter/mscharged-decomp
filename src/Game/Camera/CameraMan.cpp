#include "Game/Camera/CameraMan.h"
#include "Game/AI/AiUtil.h"
#include "Game/Camera/AnimViewerCam.h"
#include "Game/Camera/DebugCam.h"
#include "Game/Camera/FaceCam.h"
#include "Game/Camera/GoalCam.h"
#include "Game/Camera/GameplayCam.h"
#include "Game/Camera/MatrixEffectCam.h"
#include "Game/Camera/ReplayCamera.h"
#include "Game/Camera/ShootToScoreCam.h"
#include "Game/Camera/TopDownCamera.h"
#include "Game/Camera/animcam.h"
#include "Game/Camera/kickoffcam.h"
#include "Game/Render/ImpostorManager.h"
#include "NL/nlConfig.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"

#include <string.h>

extern float g_fSimulationTick;

extern "C" void fn_800F02DC(void*, unsigned long, void*);
extern "C" cRumbleFilter* fn_800EF5F8(void*);
extern "C" UnidentifiedCameraFilter* fn_800EF9F0(void*);
extern "C" const char* fn_801CBE80(int);
extern "C" void fn_8005B330(nlVector3*, float, float);
extern "C" float fn_80112E0C();
extern "C" float fn_80112E14();
extern "C" void fn_80277BB0();
extern "C" void fn_800F0990(float);
extern const char lbl_806DBA60[5];
extern void nlPrintf(const char*, ...);
extern int nlSNPrintf(char*, unsigned long, const char*, ...);

eCameraType g_eCurrentCameraType;

cBaseCamera* cCameraManager::m_cameraStack;
float cCameraManager::m_fTransitionSpeed;
float cCameraManager::m_fPrevFOV;
eCameraTransition cCameraManager::m_transition;
u16 cCameraManager::m_aJoystickRemap;
void (*cCameraManager::m_pCallback)(eCameraMessage);
int cCameraManager::m_UpVectorStackSize;
int lbl_806E0ED0;
int lbl_806E0ED4;
bool lbl_806E0ED8;
cRumbleFilter* lbl_806E0EDC;
UnidentifiedCameraFilter* lbl_806E0EE0;
int lbl_806E0EE4;

nlMatrix4 cCameraManager::m_matView;
nlVector3 cCameraManager::m_cameraPosition;
nlMatrix4 cCameraManager::m_matPrevView;

int cCameraManager::m_pBeginFrameCameraType = 14;
float cCameraManager::m_fTransitionTime = 1.0f;
float cCameraManager::m_fFOV = 50.0f;

nlVector3 cCameraManager::m_UpVectorStack[2] = {
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 1.0f },
};
nlVector3 lbl_80504360 = { 0.0f, 0.0f, 1.0f };

static char lbl_8056DD70[12][100];

struct UnidentifiedCameraAnimationLoadInfo
{
    const char* pUnidentified0;
    const char* pUnidentified1;
};

static void ApplyCameraFilters(nlMatrix4& matView)
{
    nlMatrix4 filteredView;

    for (int i = 0; i < 2; i++)
    {
        if (cCameraManager::PeekCamera()->m_pFilter[i] != 0)
        {
            cCameraManager::PeekCamera()->m_pFilter[i]->Filter(matView, filteredView);
            matView = filteredView;
        }
    }
}

/**
 * Offset/Address/Size: 0x21D0 | 0x800F2410 | size: 0xB0
 */
extern "C" float fn_800F2410(float fFOV)
{
    float fTan = nlTan((u16)(((int)(65536.0f * (0.5f * fFOV))) / 360));
    float fUnidentified0 = fn_80112E14();
    float fUnidentified1 = fn_80112E0C();
    float fResult = nlATan((fTan * fUnidentified1) / fUnidentified0);
    return 2.0f * ((180.0f * fResult) / 3.1415927f);
}

/**
 * Offset/Address/Size: 0x314C | 0x800F2404 | size: 0xC
 */
void cCameraManager::PopWorldUpVector()
{
    m_UpVectorStackSize = 0;
}

/**
 * Offset/Address/Size: 0x313C | 0x800F23F4 | size: 0x10
 */
void cCameraManager::PushWorldUpVector()
{
    m_UpVectorStackSize++;
}

/**
 * Offset/Address/Size: 0x3130 | 0x800F23E8 | size: 0xC
 */
void cCameraManager::SetWorldUpVectorTilt(float fXAxisTilt, float fYAxisTilt)
{
    fn_8005B330(&lbl_80504360, fXAxisTilt, fYAxisTilt);
}

/**
 * Offset/Address/Size: 0x310C | 0x800F23C4 | size: 0x24
 */
void cCameraManager::GetUpVector(nlVector3& upVector)
{
    nlVec3Set(upVector,
        m_matView.m12,
        m_matView.m22,
        m_matView.m32);
}

/**
 * Offset/Address/Size: 0x30DC | 0x800F2394 | size: 0x30
 */
void cCameraManager::GetViewVector(nlVector3& viewVector)
{
    nlVec3Set(viewVector,
        -m_matView.m13,
        -m_matView.m23,
        -m_matView.m33);
}

/**
 * Offset/Address/Size: 0x3074 | 0x800F232C | size: 0x68
 */
float cCameraManager::GetDistanceFromCameraToObject(const nlVector3& objectPosition)
{
    nlVector3 diff;
    float dy = objectPosition.y - cCameraManager::m_cameraPosition.y;
    float dx = objectPosition.x - cCameraManager::m_cameraPosition.x;
    nlVec3Set(diff, dx, dy, objectPosition.z - cCameraManager::m_cameraPosition.z);

    return nlSqrt(((diff.x) * (diff.x)) + ((diff.y) * (diff.y)) + ((diff.z) * (diff.z)), 1);
}

/**
 * Offset/Address/Size: 0x1D94 | 0x800F1FD4 | size: 0x358
 */
cBaseCamera* cCameraManager::PopCameraWithTransition(float fDuration, eCameraTransition transition, void (*pCallback)(eCameraMessage))
{
    if (m_transition != eCT_NONE)
    {
        nlPrintf("Camera Transition In Progress\n");
        if (m_pCallback != 0)
        {
            (*m_pCallback)(eCM_ABORTED_BY_POP);
        }
    }

    m_matPrevView = PeekCamera()->GetViewMatrix();
    m_fPrevFOV = PeekCamera()->GetFOV();

    ApplyCameraFilters(m_matPrevView);

    float fTransitionTime = m_fTransitionTime;
    m_transition = transition;
    m_pCallback = pCallback;
    m_fTransitionSpeed = 1.0f / fDuration;
    m_fTransitionTime = 1.0f - fTransitionTime;

    cBaseCamera* pCamera = nlDLRingRemoveStart<cBaseCamera>(&m_cameraStack);
    for (int i = 0; i < 2; i++)
    {
        if (PeekCamera()->m_pFilter[i] != 0)
        {
            PeekCamera()->m_pFilter[i]->Reset();
            PeekCamera()->Reactivate();
        }
    }
    return pCamera;
}

/**
 * Offset/Address/Size: 0x1A04 | 0x800F1C44 | size: 0x390
 */
void cCameraManager::PushCameraWithTransition(cBaseCamera* pCamera, float fDuration, eCameraTransition transition, void (*pCallback)(eCameraMessage), bool bDeleteCurrentCamera)
{
    if (m_transition != eCT_NONE)
    {
        nlPrintf("Camera Transition In Progress\n");
        if (m_pCallback != 0)
        {
            (*m_pCallback)(eCM_ABORTED_BY_PUSH);
        }
    }

    m_matPrevView = PeekCamera()->GetViewMatrix();
    m_fPrevFOV = PeekCamera()->GetFOV();

    ApplyCameraFilters(m_matPrevView);

    m_transition = transition;
    m_fTransitionSpeed = 1.0f / fDuration;
    m_fTransitionTime = 0.0f;
    m_pCallback = pCallback;

    if (bDeleteCurrentCamera)
    {
        cBaseCamera* pCurrentCamera = nlDLRingRemoveStart<cBaseCamera>(&m_cameraStack);
        if (pCurrentCamera != 0)
            delete pCurrentCamera;
    }

    if (PeekCamera() != 0)
    {
        cRumbleFilter* pFilter = static_cast<cRumbleFilter*>(PeekCamera()->m_pFilter[0]);
        if (pFilter != 0)
        {
            nlVector2 diff_pos;
            nlVec2Sub(diff_pos, pFilter->v2Pos0, pFilter->v2Pos1);
            nlSqrt(diff_pos.x * diff_pos.x + diff_pos.y * diff_pos.y, true);
        }
    }

    nlDLRingAddStart<cBaseCamera>(&m_cameraStack, pCamera);
}

/**
 * Offset/Address/Size: 0x19D4 | 0x800F1C14 | size: 0x30
 */
extern "C" cBaseCamera* fn_800F1C14()
{
    cBaseCamera* pCamera = cCameraManager::PeekCamera();
    if (pCamera != 0)
        return pCamera->m_next;
    return 0;
}

/**
 * Offset/Address/Size: 0x1888 | 0x800F1AC8 | size: 0x14C
 */
extern "C" cBaseCamera* fn_800F1AC8()
{
    int i;

    if (cCameraManager::m_transition != eCT_NONE)
    {
        nlPrintf("Camera Transition In Progress\n");
        if (cCameraManager::m_pCallback != 0)
        {
            (*cCameraManager::m_pCallback)(eCM_ABORTED_BY_POP);
        }
    }

    cBaseCamera* pCamera = nlDLRingRemoveStart<cBaseCamera>(&cCameraManager::m_cameraStack);
    for (i = 0; i < 2; i++)
    {
        if (cCameraManager::PeekCamera()->m_pFilter[i] != 0)
        {
            cCameraManager::PeekCamera()->m_pFilter[i]->Reset();
            cCameraManager::PeekCamera()->Reactivate();
        }
    }
    return pCamera;
}

/**
 * Offset/Address/Size: 0x16AC | 0x800F18EC | size: 0x1DC
 */
void cCameraManager::Remove(eCameraType type, bool bDeleteAfterRemoving)
{
    cBaseCamera* pCamera = m_cameraStack;

    if (m_cameraStack != 0)
    {
        cBaseCamera* pCameraNext;
        do
        {
            pCameraNext = pCamera->m_next;
            if (type == pCamera->GetType())
            {
                Remove(*pCamera);
                if (bDeleteAfterRemoving)
                {
                    delete pCamera;
                }
            }
            pCamera = pCameraNext;
        } while (pCameraNext != m_cameraStack);
    }
}

/**
 * Offset/Address/Size: 0x1538 | 0x800F1778 | size: 0x174
 */
void cCameraManager::Remove(const cBaseCamera& camera)
{
    bool actuallyRemoved = true;
    while (actuallyRemoved)
    {
        actuallyRemoved = nlDLRingRemoveSafely<cBaseCamera>(&m_cameraStack, &camera);
        if (actuallyRemoved)
        {
            for (int i = 0; i < 2; i++)
            {
                if (PeekCamera()->m_pFilter[i] != 0)
                {
                    PeekCamera()->m_pFilter[i]->Reset();
                    PeekCamera()->Reactivate();
                }
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x1430 | 0x800F1670 | size: 0x108
 */
void cCameraManager::PushCamera(cBaseCamera* pCamera)
{
    if (m_transition != eCT_NONE)
    {
        nlPrintf("Camera Transition In Progress\n");
        if (m_pCallback != 0)
        {
            (*m_pCallback)(eCM_ABORTED_BY_PUSH);
        }
    }

    m_transition = eCT_NONE;

    if (PeekCamera() != 0)
    {
        cRumbleFilter* pFilter = static_cast<cRumbleFilter*>(PeekCamera()->m_pFilter[0]);
        if (pFilter != 0)
        {
            nlVector2 diff_pos;
            nlVec2Sub(diff_pos, pFilter->v2Pos0, pFilter->v2Pos1);
            nlSqrt(diff_pos.x * diff_pos.x + diff_pos.y * diff_pos.y, true);
        }
    }

    nlDLRingAddStart<cBaseCamera>(&m_cameraStack, pCamera);
}

/**
 * Offset/Address/Size: 0x13F0 | 0x800F1630 | size: 0x40
 */
bool cCameraManager::HasCamera(cBaseCamera* pCamera)
{
    return nlDLRingValidateContainsElement<cBaseCamera>(m_cameraStack, pCamera);
}

/**
 * Offset/Address/Size: 0x1018 | 0x800F1258 | size: 0x3D8
 */
void cCameraManager::UpdateGameCameraType()
{
    cBaseCamera* pBaseCamera = nlDLRingGetEnd(m_cameraStack);

    if (g_eCurrentCameraType != pBaseCamera->GetType())
    {
        Config* pConfig;
        const char* pTag = "nocameratweakcrash";
        pConfig = &Config::Global();
        Config::TagValuePair& tvp = pConfig->FindTvp(pTag);

        bool noCameraTweakCrash;
        if (tvp.tag == 0)
        {
            pConfig->Set(pTag, false);
            noCameraTweakCrash = false;
        }
        else if (tvp.type == CONFIG_BOOL)
        {
            noCameraTweakCrash = tvp.value.boolValue;
        }
        else if (tvp.type == CONFIG_INT)
        {
            noCameraTweakCrash = tvp.value.intValue;
        }
        else if (tvp.type == CONFIG_FLOAT)
        {
            noCameraTweakCrash = tvp.value.floatValue;
        }
        else if (tvp.type == CONFIG_STRING)
        {
            noCameraTweakCrash = strcmp(lbl_806DBA60, tvp.value.stringValue) == 0;
        }
        else
        {
            noCameraTweakCrash = false;
        }

        if (noCameraTweakCrash && g_eCurrentCameraType > eCameraType_Gameplay)
        {
            g_eCurrentCameraType = eCameraType_Gameplay;
        }

        fn_80277BB0();
        pBaseCamera->m_pFilter[0] = NULL;
        nlDLRingRemoveEnd(&cCameraManager::m_cameraStack);
        delete pBaseCamera;

        switch (g_eCurrentCameraType)
        {
        case eCameraType_Debug:
        {
            pBaseCamera = new ((cDebugCamera*)nlMalloc(0xA0, 8, false)) cDebugCamera(true);
            break;
        }
        case eCameraType_Replay:
        {
            pBaseCamera = new ((ReplayCamera*)nlMalloc(sizeof(ReplayCamera), 8, false)) ReplayCamera();
            break;
        }
        case eCameraType_TopDown:
        {
            pBaseCamera = new ((TopDownCamera*)nlMalloc(sizeof(TopDownCamera), 8, false)) TopDownCamera();
            break;
        }
        case eCameraType_FollowCharacter:
        {
            pBaseCamera = new ((cFollowCamera*)nlMalloc(sizeof(cFollowCamera), 8, false))
                cFollowCamera(cFollowCamera::FOLLOW_CHARACTER);
            break;
        }
        case eCameraType_FollowBall:
        {
            pBaseCamera = new ((cFollowCamera*)nlMalloc(sizeof(cFollowCamera), 8, false))
                cFollowCamera(cFollowCamera::FOLLOW_BALL);
            break;
        }
        case eCameraType_Animated:
        {
            pBaseCamera = new ((cAnimCamera*)nlMalloc(sizeof(cAnimCamera), 8, false))
                cAnimCamera();
            break;
        }
        case eCameraType_KickOff:
        {
            pBaseCamera = new ((cKickOffCamera*)nlMalloc(sizeof(cKickOffCamera), 8, false)) cKickOffCamera();
            break;
        }
        case eCameraType_Gameplay:
        {
            pBaseCamera = new ((GameplayCamera*)nlMalloc(sizeof(GameplayCamera), 8, false)) GameplayCamera();
            break;
        }
        case eCameraType_MatrixEffect:
        {
            pBaseCamera = new ((MatrixEffectCam*)nlMalloc(sizeof(MatrixEffectCam), 8, false)) MatrixEffectCam();
            break;
        }
        case eCameraType_Goal:
        {
            pBaseCamera = new ((GoalCamera*)nlMalloc(sizeof(GoalCamera), 8, false)) GoalCamera();
            break;
        }
        case eCameraType_AnimViewer:
        {
            pBaseCamera = new ((cAnimViewerCamera*)nlMalloc(sizeof(cAnimViewerCamera), 8, false))
                cAnimViewerCamera();
            break;
        }
        case eCameraType_FaceCloseup:
        {
            pBaseCamera = new ((FaceCam*)nlMalloc(sizeof(FaceCam), 8, false)) FaceCam(2.0f);
            break;
        }
        case eCameraType_ShootToScore:
        {
            pBaseCamera = new ((cShootToScoreCamera*)nlMalloc(sizeof(cShootToScoreCamera), 8, false)) cShootToScoreCamera();
            break;
        }
        }

        nlDLRingAddEnd(&m_cameraStack, pBaseCamera);
    }
}

/**
 * Offset/Address/Size: 0x1AC4 | 0x800F0D7C | size: 0x4DC
 */
void cCameraManager::Update(float fDeltaT)
{
    nlMatrix4 cameraToWorldMatrix;
    nlMatrix4 curViewCopy;
    nlVector3 v3Unidentified;

    if (m_cameraStack == 0)
        return;

    UpdateGameCameraType();

    cBaseCamera* pCamera = nlDLRingGetStart<cBaseCamera>(m_cameraStack);
    if (pCamera->GetType() == eCameraType_Gameplay)
    {
        if (m_transition != eCT_EASE_IN)
        {
            pCamera->mUpVector = m_UpVectorStack[m_UpVectorStackSize];
        }
        else
        {
            pCamera->mUpVector.x = 0.0f;
            pCamera->mUpVector.y = 0.0f;
            pCamera->mUpVector.z = 1.0f;
        }
    }

    pCamera->Update(fDeltaT);
    for (int i = 0; i < 2; i++)
    {
        if (pCamera->m_pFilter[i] != 0)
            pCamera->m_pFilter[i]->Update(fDeltaT);
    }

    if (m_transition != eCT_NONE)
    {
        switch (m_transition)
        {
        case eCT_EASE_IN:
            fn_800F0990(g_fSimulationTick);
            nlInvertRotTransMatrix(cameraToWorldMatrix, m_matView);
            m_cameraPosition = cameraToWorldMatrix.GetTranslation();
            break;
        default:
            break;
        }
    }
    else
    {
        m_matView = pCamera->GetViewMatrix();
        m_cameraPosition = pCamera->GetCameraPosition();
        m_fFOV = pCamera->GetFOV();
        if (m_fFOV < 1.0f)
            m_fFOV = 1.0f;

        curViewCopy = m_matView;
        ApplyCameraFilters(curViewCopy);
        m_matView = curViewCopy;
    }

    nlVec3Set(v3Unidentified, m_matView.m13, m_matView.m23, m_matView.m33);
    m_aJoystickRemap = (u16)(int)(nlATan2f(v3Unidentified.y, v3Unidentified.x) * 10430.378f);
    m_aJoystickRemap += 0x8000;

    eCameraType cameraType = PeekCamera()->GetType();
    if (cameraType != lbl_806E0EE4)
    {
        ImpostorManager* pUnidentified = ImpostorManager::GetInstance();
        pUnidentified->mUnidentified037 = cameraType != eCameraType_Gameplay;
        ImpostorManager::GetInstance()->SetUpdatePeriod(
            (cameraType == eCameraType_Gameplay) + 1);
    }
    lbl_806E0EE4 = cameraType;
}

/**
 * Offset/Address/Size: 0x750 | 0x800F0990 | size: 0x3EC
 */
extern "C" void fn_800F0990(float fDeltaT)
{
    nlQuaternion qPrev;
    nlQuaternion qCur;
    nlQuaternion qSlerped;
    nlVector3 v3TransFrom;
    nlVector3 v3TransTo;
    nlVector3 v3Unidentified;
    nlMatrix4 matUnidentified0;
    nlMatrix4 prevViewCopy;
    nlMatrix4 curViewCopy;
    nlMatrix4 cameraToWorldMatrix;
    nlMatrix4 matUnidentified1;

    cBaseCamera* pCamera = nlDLRingGetStart<cBaseCamera>(cCameraManager::m_cameraStack);
    if (pCamera == 0)
        return;

    nlInvertRotTransMatrix(prevViewCopy, cCameraManager::m_matPrevView);
    nlMatrixToQuat(qPrev, prevViewCopy);
    v3TransFrom = prevViewCopy.GetTranslation();

    curViewCopy = cCameraManager::PeekCamera()->GetViewMatrix();
    ApplyCameraFilters(curViewCopy);

    nlInvertRotTransMatrix(matUnidentified0, curViewCopy);
    nlMatrixToQuat(qCur, matUnidentified0);

    float t = cCameraManager::m_fTransitionTime;
    float smoothT = t * t * t * (t * (6.0f * t + (-15.0f)) + 10.0f);
    v3TransTo = matUnidentified0.GetTranslation();
    nlQuatSlerp(qSlerped, qPrev, qCur, smoothT);

    nlVecLerp(v3Unidentified, v3TransFrom, v3TransTo, smoothT);
    nlQuatToMatrix(cameraToWorldMatrix, qSlerped, true);
    cameraToWorldMatrix.m41 = v3Unidentified.x;
    cameraToWorldMatrix.m42 = v3Unidentified.y;
    cameraToWorldMatrix.m43 = v3Unidentified.z;
    cameraToWorldMatrix.m44 = 1.0f;

    nlInvertRotTransMatrix(matUnidentified1, cameraToWorldMatrix);
    cCameraManager::m_matView = matUnidentified1;

    cCameraManager::m_fFOV = Interpolate(cCameraManager::m_fPrevFOV, pCamera->GetFOV(), smoothT);
    if (cCameraManager::m_fFOV < 1.0f)
        cCameraManager::m_fFOV = 1.0f;

    cCameraManager::m_fTransitionTime = cCameraManager::m_fTransitionTime + fDeltaT * cCameraManager::m_fTransitionSpeed;
    if (cCameraManager::m_fTransitionTime > 1.0f)
    {
        cCameraManager::m_transition = eCT_NONE;
        if (cCameraManager::m_pCallback != 0)
        {
            cCameraManager::m_pCallback(eCM_COMPLETE);
            cCameraManager::m_pCallback = 0;
        }
    }
}

/**
 * Offset/Address/Size: 0x6A4 | 0x800F08E4 | size: 0xAC
 */
void cCameraManager::Shutdown()
{
    nlDeleteDLRing<cBaseCamera>(&m_cameraStack);
    m_cameraStack = NULL;
    cAnimCamera::FreeCameraAnimations();

    if (lbl_806E0EDC != 0)
    {
        delete lbl_806E0EDC;
        lbl_806E0EDC = NULL;
    }
    if (lbl_806E0EE0 != 0)
    {
        delete lbl_806E0EE0;
        lbl_806E0EE0 = NULL;
    }
}

/**
 * Offset/Address/Size: 0x664 | 0x800F08A4 | size: 0x40
 */
extern "C" bool fn_800F08A4()
{
    if (lbl_806E0ED4 >= lbl_806E0ED0)
    {
        cCameraManager::Update(0.017f);
        return true;
    }
    return false;
}

/**
 * Offset/Address/Size: 0x494 | 0x800F06D4 | size: 0x1D0
 */
extern "C" void fn_800F06D4()
{
    cBaseCamera* pBaseCamera = new ((cDebugCamera*)nlMalloc(0xA0, 8, false)) cDebugCamera(false);

    cRumbleFilter* pRumbleFilter = static_cast<cRumbleFilter*>(nlMalloc(sizeof(cRumbleFilter), 8, false));
    if (pRumbleFilter != 0)
        pRumbleFilter = fn_800EF5F8(pRumbleFilter);
    lbl_806E0EDC = pRumbleFilter;
    pBaseCamera->m_pFilter[pRumbleFilter->vfunc_0x14()] = pRumbleFilter;

    UnidentifiedCameraFilter* pFilter = static_cast<UnidentifiedCameraFilter*>(nlMalloc(sizeof(UnidentifiedCameraFilter), 8, false));
    if (pFilter != 0)
        pFilter = fn_800EF9F0(pFilter);
    lbl_806E0EE0 = pFilter;
    pBaseCamera->m_pFilter[pFilter->vfunc_0x14()] = pFilter;

    cCameraManager::PushCamera(pBaseCamera);
    lbl_806E0ED4 = 0;
    lbl_806E0ED0 = 0;
}

/**
 * Offset/Address/Size: 0xCC | 0x800F030C | size: 0x3C8
 */
extern "C" void fn_800F030C(bool bUnidentified)
{
    cBaseCamera* pBaseCamera;
    char fileName[100];
    lbl_806E0ED8 = bUnidentified;

    if (bUnidentified)
    {
        pBaseCamera = new ((cDebugCamera*)nlMalloc(0xA0, 8, false)) cDebugCamera(false);
    }
    else
    {
        pBaseCamera = new ((GameplayCamera*)nlMalloc(sizeof(GameplayCamera), 8, false)) GameplayCamera();
    }

    cRumbleFilter* pRumbleFilter = static_cast<cRumbleFilter*>(nlMalloc(sizeof(cRumbleFilter), 8, false));
    if (pRumbleFilter != 0)
        pRumbleFilter = fn_800EF5F8(pRumbleFilter);
    lbl_806E0EDC = pRumbleFilter;
    pBaseCamera->m_pFilter[pRumbleFilter->vfunc_0x14()] = pRumbleFilter;

    UnidentifiedCameraFilter* pFilter = static_cast<UnidentifiedCameraFilter*>(nlMalloc(sizeof(UnidentifiedCameraFilter), 8, false));
    if (pFilter != 0)
        pFilter = fn_800EF9F0(pFilter);
    lbl_806E0EE0 = pFilter;
    pBaseCamera->m_pFilter[pFilter->vfunc_0x14()] = pFilter;

    cCameraManager::PushCamera(pBaseCamera);
    g_eCurrentCameraType = pBaseCamera->GetType();
    lbl_806E0ED4 = 0;
    lbl_806E0ED0 = 0;

    if (!bUnidentified)
    {
        if (nlLoadEntireFileAsync("art/cameras/ShootToScoreCamera.cam", fn_800F02DC, (void*)"ShootToScoreCamera", 0x20, AllocateEnd, 0, 0, 0))
            lbl_806E0ED0++;

        int i = 0;
        for (; i < 12; i++)
        {
            nlSNPrintf(fileName, 100, "art/cameras/%s_shoottoscorecamera.cam", fn_801CBE80(i));
            nlSNPrintf(lbl_8056DD70[i], 100, "%s_ShootToScoreCamera", fn_801CBE80(i));
            if (nlLoadEntireFileAsync(fileName, fn_800F02DC, (void*)lbl_8056DD70[i], 0x20, AllocateEnd, 0, 0, 0))
                lbl_806E0ED0++;
        }

        if (nlLoadEntireFileAsync("art/cameras/pause.cam", fn_800F02DC, (void*)"pause", 0x20, AllocateEnd, 0, 0, 0))
            lbl_806E0ED0++;
    }
    else
    {
        UnidentifiedCameraAnimationLoadInfo unidentifiedLoadInfo[] = {
            { "art/fe/environments/cameras/camera_idle.cam", "fechoosecaptains" },
            { "art/fe/environments/cameras/camera_idle_start.cam", "fetitleidlecam" },
            { "art/fe/environments/cameras/camera_eject.cam", "feejectcam" },
            { "art/fe/environments/cameras/start_idle.cam", "startidle" },
            { "art/fe/environments/cameras/start_main_menu_move.cam", "startmainmenumove" },
            { "art/fe/environments/cameras/start_main_menu_back.cam", "startmainmenuback" },
            { "art/fe/environments/cameras/start_main_menu_ball.cam", "startmainmenuball" },
            { "art/fe/environments/cameras/start_main_menu_move2.cam", "startmainmenumove2" },
            { "art/fe/environments/cameras/camera_push.cam", "stadiumselectcam" },
            { "art/fe/environments/cameras/trophy_camera_idle.cam", "trophycameraidle" },
            { "art/fe/environments/cameras/trophy_camera_intro.cam", "trophycameraintro" },
            { "art/fe/environments/cameras/trophy_captain_select_cam.cam", "trophycaptainselect" },
            { "art/fe/environments/cameras/trophy_captain_select_to_bronxe_cam.cam", "trophytransitiontobronze" },
            { "art/fe/environments/cameras/trophy_bronze_centre_cam.cam", "trophycentreofbronzehof" },
            { "art/fe/environments/cameras/trophy_bronze_centre_rtsc_cam.cam", "trophycentreofbronze" },
            { "art/fe/environments/cameras/trophy_bronze_left_cam.cam", "trophyleftofbronze" },
            { "art/fe/environments/cameras/trophy_bronze_right_cam.cam", "trophyrightofbronze" },
            { "art/fe/environments/cameras/trophy_silver_centre_cam.cam", "trophycentreofsilverhof" },
            { "art/fe/environments/cameras/trophy_silver_centre_rtsc_cam.cam", "trophycentreofsilver" },
            { "art/fe/environments/cameras/trophy_silver_left_cam.cam", "trophyleftofsilver" },
            { "art/fe/environments/cameras/trophy_silver_right_cam.cam", "trophyrightofsilver" },
            { "art/fe/environments/cameras/trophy_gold_centre_cam.cam", "trophycentreofgoldhof" },
            { "art/fe/environments/cameras/trophy_gold_centre_rtsc_cam.cam", "trophycentreofgold" },
            { "art/fe/environments/cameras/trophy_gold_left_cam.cam", "trophyleftofgold" },
            { "art/fe/environments/cameras/trophy_gold_right_cam.cam", "trophyrightofgold" },
            { "art/fe/environments/cameras/trophy_choose_sides.cam", "trophychoosesides" },
            { "art/fe/environments/cameras/hof_bronze_cam.cam", "hofbronze" },
            { "art/fe/environments/cameras/hof_silver_cam.cam", "hofsilver" },
            { "art/fe/environments/cameras/hof_gold_cam.cam", "hofgold" },
            { "art/fe/environments/cameras/43_hof_bronze_cam.cam", "43hofbronze" },
            { "art/fe/environments/cameras/43_hof_silver_cam.cam", "43hofsilver" },
            { "art/fe/environments/cameras/43_hof_gold_cam.cam", "43hofgold" },
            { "art/fe/environments/cameras/hof_profiles_cam.cam", "hofprofiles" },
            { "art/fe/environments/cameras/challenges_cam.cam", "challengecam" },
            { "art/fe/environments/cameras/101_cam.cam", "tutorcam" },
            { "art/fe/environments/cameras/camera_outofball.cam", "outofballcam" },
            { "art/fe/environments/cameras/intro_movie_to_start.cam", "movietostart" },
        };

        for (int animationIndex = 0; animationIndex < 37; animationIndex++)
        {
            if (nlLoadEntireFileAsync(unidentifiedLoadInfo[animationIndex].pUnidentified0, fn_800F02DC, (void*)unidentifiedLoadInfo[animationIndex].pUnidentified1, 0x20, AllocateEnd, 0, 0, 0))
                lbl_806E0ED0++;
        }
    }
}

/**
 * Offset/Address/Size: 0x9C | 0x800F02DC | size: 0x30
 */
extern "C" void fn_800F02DC(void* pUnidentified0, unsigned long pUnidentified1, void* pUnidentified2)
{
    cAnimCamera::LoadCameraAnimation((nlChunk*)pUnidentified0, pUnidentified1, (const char*)pUnidentified2, true);
    lbl_806E0ED4++;
}

/**
 * Offset/Address/Size: 0x2C | 0x800F026C | size: 0x70
 */
extern "C" void fn_800F026C(const nlVector3& v3Unidentified, float fUnidentified0, float fUnidentified1)
{
    UnidentifiedCameraFilter* pFilter = static_cast<UnidentifiedCameraFilter*>(cCameraManager::PeekCamera()->m_pFilter[1]);
    if (pFilter != 0)
    {
        pFilter->vfunc_0x18(v3Unidentified, fUnidentified0, fUnidentified1);
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x800F0240 | size: 0x2C
 */
void FireCameraRumbleFilter(float fRumbleX, float fRumbleY, float fSpring, float fDamping)
{
    cBaseCamera* pCamera = nlDLRingGetStart<cBaseCamera>(cCameraManager::m_cameraStack);
    if (pCamera->m_pFilter[0] != 0)
        static_cast<cRumbleFilter*>(pCamera->m_pFilter[0])->Rumble(fRumbleX, fRumbleY, fSpring, fDamping);
}
