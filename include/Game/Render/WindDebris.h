#ifndef GAME_RENDER_WINDDEBRIS_H
#define GAME_RENDER_WINDDEBRIS_H

#include "Game/Render/SkinAnimatedMovableNPC.h"

template <class T>
class cInventory;

class WindDebris : public SkinAnimatedMovableNPC
{
public:
    WindDebris(cSHierarchy& pHierarchy, int nModelID,
        unsigned long activationSoundCue, unsigned long impactSoundCue,
        PhysicsNPC& rPhysObj, cInventory<cSAnim>* pInventorySAnim, void* resource);
    virtual ~WindDebris();
    virtual SkinAnimatedNPC_Type GetSkinAnimatedNPC_Type() const
    {
        return SkinAnimatedNPC_WIND_DEBRIS;
    }
    virtual void Update(float fDeltaT);
    virtual void Move(float fDeltaT);
    virtual void DrawShadow(
        const cPoseAccumulator& pa, const nlMatrix4& worldMatrix);

    static void CollisionCallback(PhysicsObject* pPhysObj,
        PhysicsObject* pObjA, const nlVector3& v3Pos);
    void Activate();
    void Deactivate(bool);
    void Reset();
    void fn_801B4C14(float duration);

    /* 0x84 */ cSAnim* mpTumbleAnim;
    /* 0x88 */ unsigned long mActivationSoundCue;
    /* 0x8C */ unsigned long mUnidentified08C;
    /* 0x90 */ bool mbUpdateSuspended;
    /* 0x94 */ float mfCollisionDelay;
}; // total size: 0x98

#endif // GAME_RENDER_WINDDEBRIS_H
