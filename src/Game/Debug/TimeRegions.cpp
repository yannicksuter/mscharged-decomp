#include "Game/Debug/TimeRegions.h"

#include "Game/Ball.h"
#include "Game/Camera/CameraMan.h"
#include "NL/nlTask.h"

#include <math.h>

TimeRegion* pGamePlayTimeRegion;
TimeRegion* pNISTimeRegion;
TimeRegion* pAutoReplayTimeRegion;
TimeRegion* pCentreFieldTimeRegion;
TimeRegion* pLeftFieldTimeRegion;
TimeRegion* pRightFieldTimeRegion;
TimeRegion* pShotTimeRegion;

static bool IsDuringGameplay()
{
    return nlTaskManager::m_pInstance->mCurrentState == 0x2;
}

static bool CentreOfField()
{
    const nlVector3& targetPosition
        = (*nlDLRingGetStart<cBaseCamera>(cCameraManager::m_cameraStack))
              .GetTargetPosition();
    bool isCenter = false;
    if (nlTaskManager::m_pInstance->mCurrentState == 0x2
        && (float)fabs(targetPosition.x) < 5.7f)
    {
        isCenter = true;
    }
    return isCenter;
}

inline bool IsInCenterZone()
{
    const nlVector3& v = nlDLRingGetStart<cBaseCamera>(
        cCameraManager::m_cameraStack)->GetTargetPosition();
    unsigned long curState = nlTaskManager::m_pInstance->mCurrentState;
    bool isCenter = false;

    if (curState == 2 && fabsf(v.x) < 5.7f)
    {
        isCenter = true;
    }
    return isCenter;
}

static bool LeftSideOfField()
{
    const nlVector3& v = nlDLRingGetStart<cBaseCamera>(
        cCameraManager::m_cameraStack)->GetTargetPosition();
    unsigned long curState = nlTaskManager::m_pInstance->mCurrentState;

    return curState == 2 && !IsInCenterZone() && v.x < 0.0f;
}

static bool RightSideOfField()
{
    const nlVector3& v = nlDLRingGetStart<cBaseCamera>(
        cCameraManager::m_cameraStack)->GetTargetPosition();
    unsigned long curState = nlTaskManager::m_pInstance->mCurrentState;

    return curState == 2 && !IsInCenterZone() && v.x > 0.0f;
}

static bool IsDuringNIS()
{
    return nlTaskManager::m_pInstance->mCurrentState == 0x10;
}

static bool IsDuringAutoreplay()
{
    return nlTaskManager::m_pInstance->mCurrentState == 0x8;
}

static bool IsShotInProgress()
{
    return g_pBall->m_tLightningTimer.m_uPackedTime != 0;
}

inline TimeRegion::TimeRegion(
    const char* pName, bool (*pConditionFunc)())
    : m_pName(pName)
    , m_pConditionFunc(pConditionFunc)
    , m_fThreshold(0.0f)
    , m_unk10(0)
    , m_unk14(pName, 5, lbl_806E6178, lbl_806E617C)
{
    ListEntry<TimeRegion*>* entry
        = (ListEntry<TimeRegion*>*)nlMalloc(
            sizeof(ListEntry<TimeRegion*>), 8, false);
    if (entry != 0)
    {
        entry->next = 0;
        entry->entry = this;
    }
    nlListAddEnd<ListEntry<TimeRegion*> >(
        &sTimeRegionList.m_Head, &sTimeRegionList.m_Tail, entry);
}

void InitializeTimeRegions()
{
    pGamePlayTimeRegion
        = new (8, false) TimeRegion("during gameplay", IsDuringGameplay);
    pNISTimeRegion = new (8, false) TimeRegion("during NIS", IsDuringNIS);
    pAutoReplayTimeRegion
        = new (8, false) TimeRegion("during auto-replay", IsDuringAutoreplay);
    pCentreFieldTimeRegion
        = new (8, false) TimeRegion("at centre of field", CentreOfField);
    pLeftFieldTimeRegion
        = new (8, false) TimeRegion("on left side of field", LeftSideOfField);
    pRightFieldTimeRegion
        = new (8, false) TimeRegion("on right side of field", RightSideOfField);
    pShotTimeRegion
        = new (8, false) TimeRegion("when shot in progress", IsShotInProgress);
}

void DestroyTimeRegions()
{
    delete pGamePlayTimeRegion;
    delete pNISTimeRegion;
    delete pAutoReplayTimeRegion;
    delete pCentreFieldTimeRegion;
    delete pLeftFieldTimeRegion;
    delete pRightFieldTimeRegion;
    delete pShotTimeRegion;
}

TimeRegion::~TimeRegion()
{
    sTimeRegionList.RemoveEntry(this);
}
