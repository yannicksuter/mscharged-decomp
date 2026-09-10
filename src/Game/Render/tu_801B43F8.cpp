#include "Game/Render/tu_801B43F8.h"

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
#include "Game/Sys/audio.h"
#include "NL/nlString.h"

extern "C" const nlVector3 lbl_804DCF80;
extern "C" const nlVector3 lbl_804DCF8C;
static RLView* lbl_806E16E8;

static inline void fn_801B4F4C(CollisionWindDebrisPlayerData* pData)
{
    g_CollisionWindDebrisPlayerDataPool.Free(pData);
}

UnidentifiedNPC_801B43F8::UnidentifiedNPC_801B43F8(
    cSHierarchy& pHierarchy, int nModelID, unsigned long param1,
    unsigned long param2, PhysicsNPC& rPhysObj,
    cInventory<cSAnim>* pInventorySAnim, void* resource)
    : SkinAnimatedMovableNPC(pHierarchy, nModelID, rPhysObj, resource)
    , mUnidentified088(param1)
    , mUnidentified08C(param2)
    , mUnidentified090(false)
    , mUnidentified094(0.0f)
{
    lbl_806E16E8 = GetUnshadowedView();
    mUnidentified084 = pInventorySAnim->Find((unsigned int)nlStringHash("tumble"));
    SetAnimState(*mUnidentified084, 0.2f, PM_CYCLIC);
    mpPhysObj->mpAINPC = this;
    fn_801B4B24(false);
}

UnidentifiedNPC_801B43F8::~UnidentifiedNPC_801B43F8()
{
}

void UnidentifiedNPC_801B43F8::Update(float fDeltaT)
{
    if (mbIsVisible == true && !mUnidentified090)
    {
        nlVector3 pos;
        pos.x = mv3Position.x + fDeltaT * mv3Velocity.x;
        pos.y = mv3Position.y + fDeltaT * mv3Velocity.y;
        pos.z = mv3Position.z + fDeltaT * mv3Velocity.z;
        SetPosition(pos);
        if (mUnidentified084 != 0)
        {
            SkinAnimatedNPC::Update(fDeltaT);
        }
    }
    else if (mUnidentified090 == true)
    {
        mUnidentified094 -= fDeltaT;
    }

    float x = cField::GetGoalLineX(1U);
    float y = (2.0f * cField::mv3FieldPosition.y) * 0.5f;
    if (mv3Velocity.x > 0.0f && mv3Position.x > 2.0f * x)
    {
        fn_801B4B24(false);
    }
    else if (mv3Velocity.x < 0.0f && mv3Position.x < -2.0f * x)
    {
        fn_801B4B24(false);
    }
    if (mv3Velocity.y > 0.0f && mv3Position.y > 2.0f * y)
    {
        fn_801B4B24(false);
    }
    else if (mv3Velocity.y < 0.0f && mv3Position.y < -2.0f * y)
    {
        fn_801B4B24(false);
    }
}

void UnidentifiedNPC_801B43F8::fn_801B4830(
    PhysicsObject* pPhysObj, PhysicsObject* pObjA, const nlVector3& v3Pos)
{
    cPlayer* pPlayer = 0;
    UnidentifiedNPC_801B43F8* pDebris
        = (UnidentifiedNPC_801B43F8*)((PhysicsNPC*)pPhysObj)->mpAINPC;
    bool bUnidentified = pDebris->mUnidentified094 > 0.0f;
    if (bUnidentified)
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
            fn_8014A044(pDebris);
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

    if (pPlayer != 0 && pPlayer->m_eClassType == 2)
    {
        cFielder* pFielder = (cFielder*)pPlayer;
        if (pFielder->m_eActionState != 35 && pFielder->m_eActionState != 3
            && !pFielder->fn_800344B0())
        {
            CollisionWindDebrisPlayerData* pData = g_CollisionWindDebrisPlayerDataPool.Allocate();
            pData->pFielder = pFielder;
            pData->pDebris = pDebris;
            g_pGame->mUnidentified49C.mEvent34.Queue(pData,
                Function<CollisionWindDebrisPlayerData*>(fn_801B4F4C));
        }
    }
}

void UnidentifiedNPC_801B43F8::fn_801B4AD0()
{
    mpPhysObj->EnableCollisions();
    mbIsVisible = true;
    if (mUnidentified088 != 0)
    {
        PlaySound(11, mUnidentified088, 0, 0);
    }
}

void UnidentifiedNPC_801B43F8::fn_801B4B24(bool param)
{
    SetPosition(lbl_804DCF8C);
    maFacingDirection = 0;
    mv3Velocity = lbl_804DCF80;
    mpPhysObj->DisableCollisions();
    mbIsVisible = false;
}

void UnidentifiedNPC_801B43F8::fn_801B4B9C()
{
    fn_801B4B24(false);
}

void UnidentifiedNPC_801B43F8::fn_801B4C14(float param)
{
}

void UnidentifiedNPC_801B43F8::Move(float fDeltaT)
{
}

void UnidentifiedNPC_801B43F8::DrawShadow(
    const cPoseAccumulator& pa, const nlMatrix4& worldMatrix)
{
    if (mbIsVisible == true)
    {
        SkinAnimatedNPC::DrawShadow(mpLastModel, worldMatrix);
    }
}

#include "NL/nlBind_impl.h"
