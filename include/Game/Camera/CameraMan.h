#ifndef _CAMERAMAN_H_
#define _CAMERAMAN_H_

#include "types.h"
#include "Game/Camera/BaseCamera.h"
#include "NL/nlDLRing.h"
#include "NL/nlMath.h"

enum eCameraTransition
{
    eCT_NONE = 0,
    eCT_EASE_IN = 1,
};

enum eCameraMessage
{
    eCM_ABORTED_BY_POP = 0,
    eCM_ABORTED_BY_PUSH = 1,
    eCM_COMPLETE = 2,
};

void FireCameraRumbleFilter(float fRumbleX, float fRumbleY, float fSpring, float fDamping);

class cCameraManager
{
public:
    static void Shutdown();
    static void Update(float fDeltaT);
    static void UpdateGameCameraType();
    static bool HasCamera(cBaseCamera* pCamera);
    static void PushCamera(cBaseCamera* pCamera);
    static void Remove(const cBaseCamera& camera);
    static void Remove(eCameraType type, bool bDeleteAfterRemoving);
    static void PushCameraWithTransition(cBaseCamera* pCamera, float fDuration, eCameraTransition transition, void (*pCallback)(eCameraMessage), bool bDeleteCurrentCamera);
    static cBaseCamera* PopCameraWithTransition(float fDuration, eCameraTransition transition, void (*pCallback)(eCameraMessage));
    static float GetDistanceFromCameraToObject(const nlVector3& objectPosition);
    static void GetViewVector(nlVector3& viewVector);
    static void GetUpVector(nlVector3& upVector);
    static void SetWorldUpVectorTilt(float fXAxisTilt, float fYAxisTilt);
    static void PushWorldUpVector();
    static void PopWorldUpVector();

    static inline cBaseCamera* PeekCamera()
    {
        cBaseCamera* pCamera = nlDLRingGetStart<cBaseCamera>(m_cameraStack);
        return pCamera;
    }

    static u16 m_aJoystickRemap;
    static cBaseCamera* m_cameraStack;
    static eCameraTransition m_transition;
    static void (*m_pCallback)(eCameraMessage);

    static nlMatrix4 m_matView;
    static nlVector3 m_cameraPosition;
    static nlMatrix4 m_matPrevView;
    static float m_fTransitionSpeed;
    static float m_fTransitionTime;
    static float m_fPrevFOV;
    static float m_fFOV;
    static int m_pBeginFrameCameraType;

    static nlVector3 m_UpVectorStack[2];
    static int m_UpVectorStackSize;
};

#endif // _CAMERAMAN_H_
