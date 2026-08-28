#include "Game/Effects/EmissionController.h"

void EmissionController::SetPosition(const nlVector3& pos)
{
    m_vPosition = pos;
}

void EmissionController::SetDirection(const nlVector3& dir)
{
    m_vDirection = dir;
}

void EmissionController::SetVelocity(const nlVector3& velocity)
{
    m_vVelocity = velocity;
}

void EmissionController::SetPoseAccumulator(
    const cPoseAccumulator& pose)
{
    m_pPose = &pose;
}

void EmissionController::SetAnimController(
    const cPN_SAnimController& animController)
{
    m_pAnimController = &animController;
}

void EmissionController::SetUpdateCallback(
    const Function1<void, EmissionController&>& callback)
{
    mUpdateCallback = callback;
}

void EmissionController::SetFinishedCallback(
    const Function1<void, EmissionController&>& callback)
{
    mFinishedCallback = callback;
}
