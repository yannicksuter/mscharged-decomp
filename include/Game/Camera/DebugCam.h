#ifndef _DEBUGCAM_H_
#define _DEBUGCAM_H_

#include "Game/Camera/BaseCamera.h"
#include "NL/nlDLListContainer.h"
#include "types.h"

class cGlobalPad;
struct UnidentifiedDebugCameraTarget;

extern float sfDebugCamFOV;

class cDebugCamera : public cBaseCamera
{
public:
    cDebugCamera(bool bUnidentified);
    /* 0x08 */ virtual ~cDebugCamera();
    /* 0x14 */ virtual const nlMatrix4& GetViewMatrix() const { return m_matView; };
    /* 0x18 */ virtual float GetFOV() const { return sfDebugCamFOV; };
    /* 0x24 */ virtual const nlVector3& GetCameraPosition() const { return m_vecCamera; };
    /* 0x20 */ virtual const nlVector3& GetTargetPosition() const { return m_vecTarget; };
    /* 0x0C */ virtual eCameraType GetType() { return eCameraType_Debug; };
    /* 0x10 */ virtual void Update(float dt);

    void RenderTarget() const;
    void fn_800F2A8C(float dt);
    void fn_800F2BD0(float dt, float controlSpeed);
    void fn_800F2DA8(float dt, float controlSpeed);

    /* 0x20 */ nlMatrix4 m_matView;
    /* 0x60 */ float m_fRadius;
    /* 0x64 */ float m_fAzimuth;
    /* 0x68 */ float m_fTheta;
    /* 0x6C */ float m_fHeight;
    /* 0x70 */ nlVector3 m_vecCamera;
    /* 0x7C */ nlVector3 m_vecTarget;
    /* 0x88 */ cGlobalPad* m_pPad;
    /* 0x8C */ bool mUnidentified8C;
    /* 0x8D */ bool m_bEnableControls;
    /* 0x8E */ bool mUnidentified8E;
    /* 0x8F */ bool m_bRenderTarget;
    /* 0x90 */ UnidentifiedDebugCameraTarget* m_pTarget;
    /* 0x94 */ DLListEntry<UnidentifiedDebugCameraTarget*>* m_pTargetEntry;
    /* 0x98 */ nlDLListContainer<UnidentifiedDebugCameraTarget*> m_Targets;
}; // total size: 0xA0

#endif // _DEBUGCAM_H_
