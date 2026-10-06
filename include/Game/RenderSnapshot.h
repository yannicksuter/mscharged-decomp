#ifndef _RENDERSNAPSHOT_H_
#define _RENDERSNAPSHOT_H_

#include "types.h"
#include "NL/nlMath.h"

#include "Game/Drawable/DrawableBall.h"
#include "Game/Drawable/DrawableBirdoEgg.h"
#include "Game/Drawable/DrawableBulletBill.h"
#include "Game/Drawable/DrawableCharacter.h"
#include "Game/Drawable/DrawableFlyingCamera.h"
#include "Game/Drawable/DrawableHammer.h"
#include "Game/Drawable/DrawableKoopaShell.h"
#include "Game/Drawable/DrawableNetMesh.h"
#include "Game/Drawable/DrawablePowerup.h"
#include "Game/Drawable/DrawableThwomp.h"
#include "Game/Drawable/DrawableYoshiEgg.h"
#include "Game/Drawable/DrawableDaisyFist.h"

// Necessary to force extrwi instead of rlwinm
union RenderSnapshotFlags
{
    u32 raw;
    struct
    {
        u32 _b0    : 1;   // 0x80000000
        u32 _b1    : 1;   // 0x40000000  -> _2294
        u32 _b2_6  : 5;
        u32 _b7    : 1;   // 0x01000000  -> Bowser
        u32 _b8    : 1;   // 0x00800000  -> _1FC0
        u32 _b9_31 : 23;
    } bits;
};

class RenderSnapshot
{
public:
    template <typename T>
    void Replay(T& frame);

    RenderSnapshot();

    void Initialize();
    void Free();
    void Grab();
    int NumDrawableObjects() const;
    const nlVector3* GetPositionForDrawableObject(int) const;
    void Invalidate();
    void Render(float) const;
    void RenderDebugInfo(const RenderSnapshot&, const RenderSnapshot&, float) const;
    void Blend(const float*, const RenderSnapshot&, const RenderSnapshot&);
    RenderSnapshot& GetMutable();

    DrawableCharacter& GetCharacter(int index)
    {
        return mCharacters[index];
    }

    DrawablePowerup& GetUnidentifiedPowerup(int index)
    {
        return mPowerups[index];
    }

    unsigned int mEvents;
    bool mValid;
    bool mGoalLight;
    DrawableBall mBall;
    DrawableCharacter mCharacters[10];
    DrawablePowerup mPowerups[150];
    DrawableCharacter mChainChomp;
    DrawableCharacter mBowser;
    DrawableYoshiEgg _1BA0;
    DrawableBirdoEgg _1BC4;
    DrawableKoopaShell _1BE8;
    u32 _1C00;
    DrawableDaisyFist mDaisyFists[8];
    u32 _1CC4;
    DrawableBulletBill _1CC8[6];
    bool _1DA0;
    DrawableHammer _1DA4[15];
    DrawableCharacter _1FC0[3];
    DrawableThwomp _2194[8];
    u32 _2294;
    DrawableFlyingCamera _2298[10];
    DrawableNetMesh* mpNetMeshPositiveX;
    DrawableNetMesh* mpNetMeshNegativeX;
    bool _2430;
    bool _2431;
    nlVector3 mCameraUp;
    nlVector3 _2440[60];
    float mFrameBlendPercent;
    RenderSnapshotFlags _2714;
    float _2718;
};

DrawableBulletBill& GetSnapshotBulletBill(RenderSnapshot* snapshot, unsigned int index);

inline void SaveFrame::fn_80191504()
{
}

inline void LoadFrame::fn_80191504()
{
}

#endif // _RENDERSNAPSHOT_H_
