#include "NL/nlDLListContainer.inl"
#include "Game/Camera/CameraMan.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/FE/feHelpFuncs_decl.h"
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
#include "Game/Camera/noisefilter.h"
#include "Game/Render/ImpostorManager.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/Game.h"
#include "Game/Task/BeginFrameTask.h"
#include "NL/nlConfig.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"

#include <string.h>
#include "NL/nlPrint.h"
#include "Game/Task/FixedUpdateTask.h"

eCameraType g_eCurrentCameraType;

cBaseCamera* cCameraManager::m_cameraStack;
float cCameraManager::m_fTransitionSpeed;
float cCameraManager::m_fPrevFOV;
eCameraTransition cCameraManager::m_transition;
u16 cCameraManager::m_aJoystickRemap;
void (*cCameraManager::m_pCallback)(eCameraMessage);
int cCameraManager::m_UpVectorStackSize;
int g_nCameraAnimationsRequested;
int g_nCameraAnimationsCompleted;
bool g_bFrontEndCameras;
cRumbleFilter* g_pRumbleFilter;
cNoiseFilter* g_pNoiseFilter;
int g_LastCameraType;

nlMatrix4 cCameraManager::m_matView;
nlVector3 cCameraManager::m_cameraPosition;
nlMatrix4 cCameraManager::m_matPrevView;

class CameraViewInitializer
{
public:
    CameraViewInitializer()
    {
        cCameraManager::m_matView.SetIdentity();
    }
};

static CameraViewInitializer sCameraViewInitializer;

int cCameraManager::m_BeginFrameCameraType = 14;
float cCameraManager::m_fTransitionTime = 1.0f;
float cCameraManager::m_fFOV = 50.0f;

nlVector3 cCameraManager::m_UpVectorStack[2] = {
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 1.0f },
};
nlVector3 g_CameraWorldUpVector = { 0.0f, 0.0f, 1.0f };

static char sTeamCameraAnimationNames[12][100];

struct CameraAnimationLoadInfo
{
    const char* fileName;
    const char* animationName;
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

static void ResetCameraFilters()
{
    for (int i = 0; i < 2; i++)
    {
        if (cCameraManager::PeekCamera()->m_pFilter[i] != 0)
        {
            cCameraManager::PeekCamera()->m_pFilter[i]->Reset();
            cCameraManager::PeekCamera()->Reactivate();
        }
    }
}

void ResetCameraTransitionTime()
{
    cCameraManager::m_fTransitionTime = 0.0f;
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

/**
 * Offset/Address/Size: 0x2C | 0x800F026C | size: 0x70
 */
void FireCameraNoiseFilter(nlVector3& amplitude, float frequency, float duration)
{
    cNoiseFilter* pFilter = static_cast<cNoiseFilter*>(cCameraManager::PeekCamera()->m_pFilter[1]);
    if (pFilter != 0)
    {
        pFilter->Start(amplitude, frequency, duration);
    }
}

/**
 * Offset/Address/Size: 0x9C | 0x800F02DC | size: 0x30
 */
void CameraAnimationLoadCallback(void* fileData, unsigned long fileSize, void* context)
{
    cAnimCamera::LoadCameraAnimation((nlChunk*)fileData, fileSize, (const char*)context, true);
    g_nCameraAnimationsCompleted++;
}

/**
 * Offset/Address/Size: 0xCC | 0x800F030C | size: 0x3C8
 */
void AsyncStartCameraLoading(bool frontEnd)
{
    cBaseCamera* pBaseCamera;
    char fileName[100];
    g_bFrontEndCameras = frontEnd;

    if (frontEnd)
    {
        pBaseCamera = new ((cDebugCamera*)nlMalloc(sizeof(cDebugCamera), 8, false)) cDebugCamera(false);
    }
    else
    {
        pBaseCamera = new ((GameplayCamera*)nlMalloc(sizeof(GameplayCamera), 8, false)) GameplayCamera();
    }

    cRumbleFilter* pRumbleFilter = new (8, false) cRumbleFilter();
    g_pRumbleFilter = pRumbleFilter;
    pBaseCamera->m_pFilter[pRumbleFilter->GetFilterIndex()] = pRumbleFilter;

    cNoiseFilter* pFilter = new (8, false) cNoiseFilter();
    g_pNoiseFilter = pFilter;
    pBaseCamera->m_pFilter[pFilter->GetFilterIndex()] = pFilter;

    cCameraManager::PushCamera(pBaseCamera);
    g_eCurrentCameraType = pBaseCamera->GetType();
    g_nCameraAnimationsCompleted = 0;
    g_nCameraAnimationsRequested = 0;

    if (!frontEnd)
    {
        if (nlLoadEntireFileAsync("art/cameras/ShootToScoreCamera.cam", CameraAnimationLoadCallback, (void*)"ShootToScoreCamera", 0x20, AllocateEnd, 0, 0, 0))
            g_nCameraAnimationsRequested++;

        int i = 0;
        for (; i < 12; i++)
        {
            nlSNPrintf(fileName, 100, "art/cameras/%s_shoottoscorecamera.cam", GetTeamName((eTeamID)i));
            nlSNPrintf(sTeamCameraAnimationNames[i], 100, "%s_ShootToScoreCamera", GetTeamName((eTeamID)i));
            if (nlLoadEntireFileAsync(fileName, CameraAnimationLoadCallback, (void*)sTeamCameraAnimationNames[i], 0x20, AllocateEnd, 0, 0, 0))
                g_nCameraAnimationsRequested++;
        }

        if (nlLoadEntireFileAsync("art/cameras/pause.cam", CameraAnimationLoadCallback, (void*)"pause", 0x20, AllocateEnd, 0, 0, 0))
            g_nCameraAnimationsRequested++;
    }
    else
    {
        CameraAnimationLoadInfo frontEndCameraAnimations[] = {
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
            if (nlLoadEntireFileAsync(frontEndCameraAnimations[animationIndex].fileName, CameraAnimationLoadCallback, (void*)frontEndCameraAnimations[animationIndex].animationName, 0x20, AllocateEnd, 0, 0, 0))
                g_nCameraAnimationsRequested++;
        }
    }
}

/**
 * Offset/Address/Size: 0x494 | 0x800F06D4 | size: 0x1D0
 */
void AsyncStartCameraLoadingForStadiumViewer()
{
    cBaseCamera* pBaseCamera = new ((cDebugCamera*)nlMalloc(sizeof(cDebugCamera), 8, false)) cDebugCamera(false);

    cRumbleFilter* pRumbleFilter = new (8, false) cRumbleFilter();
    g_pRumbleFilter = pRumbleFilter;
    pBaseCamera->m_pFilter[pRumbleFilter->GetFilterIndex()] = pRumbleFilter;

    cNoiseFilter* pFilter = new (8, false) cNoiseFilter();
    g_pNoiseFilter = pFilter;
    pBaseCamera->m_pFilter[pFilter->GetFilterIndex()] = pFilter;

    cCameraManager::PushCamera(pBaseCamera);
    g_nCameraAnimationsCompleted = 0;
    g_nCameraAnimationsRequested = 0;
}

/**
 * Offset/Address/Size: 0x664 | 0x800F08A4 | size: 0x40
 */
bool AsyncFinalizeCameraLoading()
{
    if (g_nCameraAnimationsCompleted >= g_nCameraAnimationsRequested)
    {
        cCameraManager::Update(0.017f);
        return true;
    }
    return false;
}

/**
 * Offset/Address/Size: 0x6A4 | 0x800F08E4 | size: 0xAC
 */
void cCameraManager::Shutdown()
{
    nlDeleteDLRing<cBaseCamera>(&m_cameraStack);
    m_cameraStack = NULL;
    cAnimCamera::FreeCameraAnimations();

    if (g_pRumbleFilter != 0)
    {
        delete g_pRumbleFilter;
        g_pRumbleFilter = NULL;
    }
    if (g_pNoiseFilter != 0)
    {
        delete g_pNoiseFilter;
        g_pNoiseFilter = NULL;
    }
}

/**
 * Offset/Address/Size: 0x750 | 0x800F0990 | size: 0x3EC
 */
void UpdateCameraTransition(float fDeltaT)
{
    nlQuaternion qPrev;
    nlQuaternion qCur;
    nlQuaternion qSlerped;
    nlVector3 v3TransFrom;
    nlVector3 v3TransTo;
    nlVector3 blendedPosition;
    nlMatrix4 currentCameraToWorldMatrix;
    nlMatrix4 previousCameraToWorldMatrix;
    nlMatrix4 curViewCopy;
    nlMatrix4 cameraToWorldMatrix;
    nlMatrix4 blendedViewMatrix;

    cBaseCamera* pCamera = nlDLRingGetStart<cBaseCamera>(cCameraManager::m_cameraStack);
    if (pCamera == 0)
        return;

    nlInvertRotTransMatrix(previousCameraToWorldMatrix, cCameraManager::m_matPrevView);
    nlMatrixToQuat(qPrev, previousCameraToWorldMatrix);
    v3TransFrom = previousCameraToWorldMatrix.GetTranslation();

    curViewCopy = cCameraManager::PeekCamera()->GetViewMatrix();
    ApplyCameraFilters(curViewCopy);

    nlInvertRotTransMatrix(currentCameraToWorldMatrix, curViewCopy);
    nlMatrixToQuat(qCur, currentCameraToWorldMatrix);

    float t = cCameraManager::m_fTransitionTime;
    float smoothT = t * t * t * (t * (6.0f * t + (-15.0f)) + 10.0f);
    v3TransTo = currentCameraToWorldMatrix.GetTranslation();
    nlQuatSlerp(qSlerped, qPrev, qCur, smoothT);

    nlVecLerp(blendedPosition, v3TransFrom, v3TransTo, smoothT);
    nlQuatToMatrix(cameraToWorldMatrix, qSlerped, true);
    cameraToWorldMatrix.m41 = blendedPosition.x;
    cameraToWorldMatrix.m42 = blendedPosition.y;
    cameraToWorldMatrix.m43 = blendedPosition.z;
    cameraToWorldMatrix.m44 = 1.0f;

    nlInvertRotTransMatrix(blendedViewMatrix, cameraToWorldMatrix);
    cCameraManager::m_matView = blendedViewMatrix;

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
 * Offset/Address/Size: 0xB3C | 0x800F0D7C | size: 0x4DC
 */
void cCameraManager::Update(float fDeltaT)
{
    nlMatrix4 cameraToWorldMatrix;
    nlMatrix4 curViewCopy;
    nlVector3 viewZAxis;

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
            UpdateCameraTransition(g_fSimulationTick);
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

    nlVec3Set(viewZAxis, m_matView.m13, m_matView.m23, m_matView.m33);
    m_aJoystickRemap = (u16)(int)(nlATan2f(viewZAxis.y, viewZAxis.x) * 10430.378f);
    m_aJoystickRemap += 0x8000;

    eCameraType cameraType = PeekCamera()->GetType();
    if (cameraType != g_LastCameraType)
    {
        ImpostorManager* impostorManager = ImpostorManager::GetInstance();
        impostorManager->mUpdateClusters = cameraType != eCameraType_Gameplay;
        int updatePeriod = cameraType == eCameraType_Gameplay ? 2 : 1;
        ImpostorManager::GetInstance()->SetUpdatePeriod(updatePeriod);
    }
    g_LastCameraType = cameraType;
}

/**
 * Offset/Address/Size: 0x1018 | 0x800F1258 | size: 0x3D8
 */
void cCameraManager::UpdateGameCameraType()
{
    cBaseCamera* pBaseCamera = nlDLRingGetEnd(m_cameraStack);

    if (g_eCurrentCameraType != pBaseCamera->GetType())
    {
        bool noCameraTweakCrash = GetConfigBool(Config::Global(), "nocameratweakcrash", false);

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
            pBaseCamera = new ((cDebugCamera*)nlMalloc(sizeof(cDebugCamera), 8, false)) cDebugCamera(true);
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
 * Offset/Address/Size: 0x13F0 | 0x800F1630 | size: 0x40
 */
bool cCameraManager::HasCamera(cBaseCamera* pCamera)
{
    return nlDLRingValidateContainsElement<cBaseCamera>(m_cameraStack, pCamera);
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
 * Offset/Address/Size: 0x1888 | 0x800F1AC8 | size: 0x14C
 */
cBaseCamera* cCameraManager::PopCamera()
{
    if (cCameraManager::m_transition != eCT_NONE)
    {
        nlPrintf("Camera Transition In Progress\n");
        if (cCameraManager::m_pCallback != 0)
        {
            (*cCameraManager::m_pCallback)(eCM_ABORTED_BY_POP);
        }
    }

    cBaseCamera* pCamera = nlDLRingRemoveStart<cBaseCamera>(&cCameraManager::m_cameraStack);
    ResetCameraFilters();
    return pCamera;
}

/**
 * Offset/Address/Size: 0x19D4 | 0x800F1C14 | size: 0x30
 */
cBaseCamera* GetNextCamera()
{
    cBaseCamera* pCamera = cCameraManager::PeekCamera();
    if (pCamera != 0)
        return pCamera->m_next;
    return 0;
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
    ResetCameraTransitionTime();
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
    ResetCameraFilters();
    return pCamera;
}

/**
 * Offset/Address/Size: 0x20EC | 0x800F232C | size: 0x68
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
 * Offset/Address/Size: 0x2154 | 0x800F2394 | size: 0x30
 */
void cCameraManager::GetViewVector(nlVector3& viewVector)
{
    nlVec3Set(viewVector,
        -m_matView.m13,
        -m_matView.m23,
        -m_matView.m33);
}

/**
 * Offset/Address/Size: 0x2184 | 0x800F23C4 | size: 0x24
 */
void cCameraManager::GetUpVector(nlVector3& upVector)
{
    nlVec3Set(upVector,
        m_matView.m12,
        m_matView.m22,
        m_matView.m32);
}

/**
 * Offset/Address/Size: 0x21A8 | 0x800F23E8 | size: 0xC
 */
void cCameraManager::SetWorldUpVectorTilt(float fXAxisTilt, float fYAxisTilt)
{
    fn_8005B330(&g_CameraWorldUpVector, fXAxisTilt, fYAxisTilt);
}

/**
 * Offset/Address/Size: 0x21B4 | 0x800F23F4 | size: 0x10
 */
void cCameraManager::PushWorldUpVector()
{
    m_UpVectorStackSize++;
}

/**
 * Offset/Address/Size: 0x21C4 | 0x800F2404 | size: 0xC
 */
void cCameraManager::PopWorldUpVector()
{
    m_UpVectorStackSize = 0;
}

/**
 * Offset/Address/Size: 0x21D0 | 0x800F2410 | size: 0xB0
 */
float AdjustFOVForWidescreen(float fFOV)
{
    float fTan = nlTan((u16)(((int)(65536.0f * (0.5f * fFOV))) / 360));
    float widescreenAspect = GetWidescreenAspectRatio();
    float standardAspect = GetStandardAspectRatio();
    float halfFovRadians = nlATan((fTan * standardAspect) / widescreenAspect);
    return 2.0f * ((180.0f * halfFovRadians) / 3.1415927f);
}
