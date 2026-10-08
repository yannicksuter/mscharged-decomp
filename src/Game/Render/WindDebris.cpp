#include "NL/nlDLListContainer.inl"
#include "Game/Render/WindDebris.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/EventDataTypes.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/Inventory.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Render/RLView.h"
#include "Game/Render/ThwompObject.h"
#include "Game/Sys/audio.h"
#include "NL/nlString.h"
#include "NL/nlFunction.inl"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

static RLView* sUnshadowedView;

static inline void FreeCollisionWindDebrisPlayerData(CollisionWindDebrisPlayerData* pData);

WindDebris::WindDebris(
    cSHierarchy& pHierarchy, int nModelID, unsigned long activationSoundCue,
    unsigned long impactSoundCue, PhysicsNPC& rPhysObj,
    cInventory<cSAnim>* pInventorySAnim, void* resource)
    : SkinAnimatedMovableNPC(pHierarchy, nModelID, rPhysObj, resource)
    , mActivationSoundCue(activationSoundCue)
    , mUnidentified08C(impactSoundCue)
    , mbUpdateSuspended(false)
    , mfCollisionDelay(0.0f)
{
    sUnshadowedView = GetUnshadowedView();
    mpTumbleAnim = pInventorySAnim->Find((unsigned int)nlStringHash("tumble"));
    SetAnimState(*mpTumbleAnim, 0.2f, PM_CYCLIC);
    mpPhysObj->mpAINPC = this;
    Deactivate(false);
}

WindDebris::~WindDebris()
{
}

void WindDebris::Update(float fDeltaT)
{
    if (mbIsVisible == true && !mbUpdateSuspended)
    {
        nlVector3 pos;
        pos.x = mv3Position.x + fDeltaT * mv3Velocity.x;
        pos.y = mv3Position.y + fDeltaT * mv3Velocity.y;
        pos.z = mv3Position.z + fDeltaT * mv3Velocity.z;
        SetPosition(pos);
        if (mpTumbleAnim != 0)
        {
            SkinAnimatedNPC::Update(fDeltaT);
        }
    }
    else if (mbUpdateSuspended == true)
    {
        mfCollisionDelay -= fDeltaT;
    }

    float x = cField::GetGoalLineX(1U);
    float width = 2.0f * cField::mv3FieldPosition.y;
    float y = 0.5f * width;
    if (mv3Velocity.x > 0.0f && mv3Position.x > 2.0f * x)
    {
        Deactivate(false);
    }
    else if (mv3Velocity.x < 0.0f && mv3Position.x < -2.0f * x)
    {
        Deactivate(false);
    }
    if (mv3Velocity.y > 0.0f && mv3Position.y > 2.0f * y)
    {
        Deactivate(false);
    }
    else if (mv3Velocity.y < 0.0f && mv3Position.y < -2.0f * y)
    {
        Deactivate(false);
    }
}

void WindDebris::CollisionCallback(
    PhysicsObject* pPhysObj, PhysicsObject* pObjA, const nlVector3& v3Pos)
{
    cPlayer* pPlayer = 0;
    WindDebris* pDebris
        = (WindDebris*)((PhysicsNPC*)pPhysObj)->mpAINPC;
    bool collisionDelayed = pDebris->mfCollisionDelay > 0.0f;
    if (collisionDelayed)
    {
        return;
    }

    switch (pObjA->GetObjectType())
    {
    case 4:
        pPlayer = (cPlayer*)((PhysicsCharacter*)pObjA->m_parentObject)->m_pAICharacter;
        break;
    case 16:
    {
        cBall* pBall = ((PhysicsAIBall*)pObjA)->m_pAIBall;
        if (pBall->m_pOwner != 0)
        {
            pPlayer = pBall->m_pOwner;
        }
        else if (pBall->meBallState != 10)
        {
            QueueCollisionDebrisBall(pDebris);
        }
        break;
    }
    case 20:
        ((PhysicsShell*)pObjA)->m_pPowerupObject->m_bShouldDestroy = true;
        break;
    case 21:
        ((PhysicsBanana*)pObjA)->m_pPowerupObject->m_bShouldDestroy = true;
        break;
    }

    if (pPlayer != 0 && pPlayer->m_eClassType == FIELDER)
    {
        cFielder* pFielder = (cFielder*)pPlayer;
        if (pFielder->m_eActionState != 35 && pFielder->m_eActionState != 3
            && !pFielder->fn_800344B0())
        {
            CollisionWindDebrisPlayerData* pData = g_CollisionWindDebrisPlayerDataPool.Allocate();
            pData->pFielder = pFielder;
            pData->pDebris = pDebris;
            g_pGame->mUnidentified49C.mCollisionWindDebrisPlayerEvent.Queue(pData,
                Function<CollisionWindDebrisPlayerData*>(FreeCollisionWindDebrisPlayerData));
        }
    }
}

void WindDebris::Activate()
{
    mpPhysObj->EnableCollisions();
    mbIsVisible = true;
    if (mActivationSoundCue != 0)
    {
        PlaySound(11, mActivationSoundCue, 0, 0);
    }
}

void WindDebris::Deactivate(bool)
{
    SetPosition(gWindDebrisHiddenPosition);
    maFacingDirection = 0;
    mv3Velocity = gWindDebrisZeroVelocity;
    mpPhysObj->DisableCollisions();
    mbIsVisible = false;
}

void WindDebris::Reset()
{
    Deactivate(false);
}

void WindDebris::fn_801B4C14(float duration)
{
}

void WindDebris::Move(float fDeltaT)
{
}

void WindDebris::DrawShadow(
    const cPoseAccumulator& pa, const nlMatrix4& worldMatrix)
{
    if (mbIsVisible == true)
    {
        SkinAnimatedNPC::DrawShadow(mpLastModel, worldMatrix);
    }
}

#include "Game/Render/WindDebris.inl"
#include "NL/nlBind_impl.h"
