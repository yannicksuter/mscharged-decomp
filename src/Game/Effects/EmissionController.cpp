#include "Game/Effects/EmissionController.h"

#include "NL/nlFile.h"
#include "NL/nlMemory.h"

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

bool EmissionController::IsLingering() const
{
    return m_bLingering;
}

void* fxLoadEntireFileHigh(const char* filename, unsigned long* fileSize)
{
    void* buffer = 0;
    u32 datasize = 0;

    nlFile* file = nlOpen(filename);
    if (file != 0)
    {
        unsigned int size;
        datasize = nlFileSize(file, &size);
        buffer = nlMalloc(size, 0x20, true);
        nlRead(file, buffer, datasize, 0);
        nlClose(file);
    }

    if (fileSize != 0)
    {
        *fileSize = datasize;
    }

    return buffer;
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
