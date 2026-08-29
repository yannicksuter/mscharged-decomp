#include <stddef.h>

#include "Game/Ball.h"

#include "Game/Drawable/DrawableObj.h"
#include "Game/Net.h"
#include "Game/Physics/PhysicsBall.h"
#include "Game/Player.h"
#include "Game/Team.h"

struct UnidentifiedGameState
{
    u8 mUnidentified00[0x28];
    u32 mUnidentified28;
    u32 mUnidentified2C;
    u32 mUnidentified30;
    u8 mUnidentified34[0x0C];
    bool mUnidentified40;
};

extern UnidentifiedGameState* lbl_806E0C94;

static float lbl_806DB514 = 2.5f;

nlVector3* cBall::GetAIVelocity() const
{
    cPlayer* temp_r4 = m_pOwner;
    if (temp_r4 != NULL)
    {
        return &(temp_r4->m_v3Velocity);
    }
    return (nlVector3*)&(m_v3Velocity);
}

nlVector3* cBall::GetDrawablePosition() const
{
    const nlMatrix4& mtx = m_pDrawableBall->GetWorldMatrix();
    return (nlVector3*)&(mtx.e2[3][0]);
}

float cBall::fn_80014F38(float fScale) const
{
    return fScale * (0.18f * lbl_806DB514);
}

cFielder* cBall::GetOwnerFielder()
{
    cPlayer* player = m_pOwner;
    if ((player == NULL) || (player->m_eClassType != FIELDER))
    {
        return NULL;
    }
    return (cFielder*)player;
}

cPlayer* cBall::GetOwnerGoalie()
{
    cPlayer* player = m_pOwner;
    if ((player == NULL) || (player->m_eClassType != GOALIE))
    {
        return NULL;
    }
    return player;
}

cFielder* cBall::GetPassTargetFielder() const
{
    cPlayer* player = m_pPassTarget;
    if ((player == NULL) || (player->m_eClassType != FIELDER))
    {
        return NULL;
    }
    return (cFielder*)player;
}

bool cBall::GetInNet(int& nSide)
{
    if (lbl_806E0C94->mUnidentified40 == 0)
    {
        if (m_pPhysicsBall->m_bInsideNet)
        {
            float fDirection = g_pTeams[0]->m_pNet->m_fDirection;
            nSide = !(m_v3Position.x * fDirection > 1.0f);
            return true;
        }
    }
    else if (lbl_806E0C94->mUnidentified2C
                 > lbl_806E0C94->mUnidentified28
             && lbl_806E0C94->mUnidentified30 != 0)
    {
        float fDirection = g_pTeams[0]->m_pNet->m_fDirection;
        nSide = !(m_v3Position.x * fDirection > 1.0f);
        return true;
    }

    return false;
}
