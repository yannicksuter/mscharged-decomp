#ifndef _RENDERSNAPSHOT_H_
#define _RENDERSNAPSHOT_H_

#include "Game/RenderSnapshot_fwd.h"
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

union RenderSnapshotFlags
{
    u32 raw;
    struct
    {
        u32 yoshiEgg : 1;
        u32 flyingCameras : 1;
        u32 hammers : 1;
        u32 bulletBills : 1;
        u32 koopaShell : 1;
        u32 birdoEgg : 1;
        u32 daisyFists : 1;
        u32 diddyBanana : 1;
        u32 windDebris : 1;
        u32 thwomps : 1;
        u32 reserved : 22;
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
    void Render(float);
    void RenderDebugInfo(const RenderSnapshot&, const RenderSnapshot&, float) const;
    void Blend(const float*, RenderSnapshot&, RenderSnapshot&);
    RenderSnapshot& GetMutable();

    DrawableCharacter& GetCharacter(int index)
    {
        return mCharacters[index];
    }

    DrawablePowerup& GetPowerup(int index)
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
    DrawableCharacter mDiddyBanana;
    DrawableYoshiEgg mYoshiEgg;
    DrawableBirdoEgg mBirdoEgg;
    DrawableKoopaShell mKoopaShell;
    u32 mNumVisibleDaisyFists;
    DrawableDaisyFist mDaisyFists[8];
    u32 mNumBulletBills;
    DrawableBulletBill mBulletBills[6];
    bool mHasHammers;
    DrawableHammer mHammers[15];
    DrawableCharacter mWindDebris[3];
    DrawableThwomp mThwomps[8];
    u32 mNumFlyingCameras;
    DrawableFlyingCamera _2298[10];
    DrawableNetMesh* mpNetMeshPositiveX;
    DrawableNetMesh* mpNetMeshNegativeX;
    bool mPositiveGoalieNetCheck;
    bool mNegativeGoalieNetCheck;
    nlVector3 mCameraUp;
    nlVector3 mPatchPositions[60];
    float mFrameBlendPercent;
    RenderSnapshotFlags mFlags;
    float mSimulationTime;
};

DrawableBulletBill& GetSnapshotBulletBill(RenderSnapshot* snapshot, unsigned int index);

inline void SaveFrame::fn_80191504()
{
}

inline void LoadFrame::fn_80191504()
{
}

#endif // _RENDERSNAPSHOT_H_
