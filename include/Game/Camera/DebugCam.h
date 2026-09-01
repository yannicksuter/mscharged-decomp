#ifndef _DEBUGCAM_H_
#define _DEBUGCAM_H_

#include "Game/Camera/BaseCamera.h"
#include "types.h"

class cGlobalPad;
struct UnidentifiedDebugCameraTarget;
struct UnidentifiedDebugCameraTargetEntry;

class cDebugCamera : public cBaseCamera
{
public:
    cDebugCamera(bool bUnidentified);
    virtual ~cDebugCamera();
    virtual eCameraType GetType();
    virtual void Update(float dt);
    virtual const nlMatrix4& GetViewMatrix() const;
    virtual float GetFOV() const;
    virtual const nlVector3& GetTargetPosition() const;
    virtual const nlVector3& GetCameraPosition() const;

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
    /* 0x94 */ UnidentifiedDebugCameraTargetEntry* m_pTargetEntry;
    /* 0x98 */ u32 mUnidentified98;
    /* 0x9C */ UnidentifiedDebugCameraTargetEntry* m_pTargets;
}; // total size: 0xA0

#endif // _DEBUGCAM_H_
