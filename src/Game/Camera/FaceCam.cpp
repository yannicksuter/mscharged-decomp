#include "Game/Camera/FaceCam.h"

#include "Game/PoseAccumulator.h"
#include "NL/gl/glMatrix.h"

const nlVector3& FaceCam::GetTargetPosition() const
{
    return mTargetPosition;
}

const nlVector3& FaceCam::GetCameraPosition() const
{
    return mCameraPosition;
}

void FaceCam::Update(float dt)
{
    mTargetPosition = mpCharacter->GetJointPosition(mpCharacter->GetHeadJointIndex());
    const nlMatrix4& headMatrix = mpCharacter->m_pPoseAccumulator->GetNodeMatrix(mpCharacter->GetHeadJointIndex());

    nlVector3 forward;
    forward.x = headMatrix.e2[1][0];
    forward.y = headMatrix.e2[1][1];
    forward.z = 0.0f;

    float len = nlRecipSqrt(forward.z * forward.z + (forward.x * forward.x + (forward.y * forward.y)), 1);

    nlVec3Scale(forward, forward, len);
    nlVec3ScaleAdd(mCameraPosition, mDistance, forward, mTargetPosition);

    glMatrixLookAt(mViewMatrix, GetCameraPosition(), GetTargetPosition(), mUpVector);
}

FaceCam::FaceCam(float distance)
{
    mpCharacter = 0;
    mDistance = distance;
}

void FaceCam::SetToUserCharacter()
{
}
