#ifndef _DEBUGCAM_H_
#define _DEBUGCAM_H_

#include "Game/Camera/BaseCam.h"
#include "NL/nlDLListContainer.h"
#include "types.h"

class cGlobalPad;
struct DebugCameraTarget;

extern float sfDebugCamFOV;

class cDebugCamera : public cBaseCamera
{
public:
    cDebugCamera(bool);
    /* 0x08 */ virtual ~cDebugCamera();
    /* 0x0C */ virtual eCameraType GetType() { return eCameraType_Debug; };
    /* 0x20 */ virtual const nlVector3& GetTargetPosition() const { return m_vecTarget; };
    /* 0x24 */ virtual const nlVector3& GetCameraPosition() const { return m_vecCamera; };
    /* 0x18 */ virtual float GetFOV() const { return sfDebugCamFOV; };
    /* 0x14 */ virtual const nlMatrix4& GetViewMatrix() const { return m_matView; };
    /* 0x10 */ virtual void Update(float dt);

    void UpdateTargetPositions();
    void UpdateOrbitControls(float dt);
    void UpdatePanControls(float dt, float controlSpeed);
    void UpdateRadiusAndHeightControls(float dt, float controlSpeed);

    /* 0x20 */ nlMatrix4 m_matView;
    /* 0x60 */ float m_fRadius;
    /* 0x64 */ float m_fAzimuth;
    /* 0x68 */ float m_fTheta;
    /* 0x6C */ float m_fHeight;
    /* 0x70 */ nlVector3 m_vecCamera;
    /* 0x7C */ nlVector3 m_vecTarget;
    /* 0x88 */ cGlobalPad* m_pPad;
    /* 0x8C */ bool m_bUseWiiControls;
    /* 0x8D */ bool m_bEnableControls;
    /* 0x8E */ bool mUnidentified8E;
    /* 0x8F */ bool m_bUpdateTargets;
    /* 0x90 */ DebugCameraTarget* m_pTarget;
    /* 0x94 */ DLListEntry<DebugCameraTarget*>* m_pTargetEntry;
    /* 0x98 */ nlDLListContainer<DebugCameraTarget*> m_Targets;
}; // total size: 0xA0

#endif // _DEBUGCAM_H_
