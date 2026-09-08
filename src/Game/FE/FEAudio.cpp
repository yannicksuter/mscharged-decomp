#include "Game/FE/FEAudio.h"

#include "Game/Sys/audio.h"

static bool mIsEnabled = true;
static int sSoundCategory = 0x15;

void FEAudio::PlaySound(
    int slotId,
    unsigned long cueId,
    const void* debugName,
    void* context)
{
    if (!mIsEnabled)
    {
        return;
    }

    ::PlaySound(
        slotId, cueId, debugName != 0 ? debugName : "FESFX", context);
}

void FEAudio::PlayTrackedSound(
    int slotId,
    unsigned long cueId,
    const void* debugName,
    void* context)
{
    if (!mIsEnabled)
    {
        return;
    }

    ::PlayTrackedSound(
        slotId, cueId, debugName != 0 ? debugName : "FESFX", context, true);
}

void FEAudio::PlayAnimAudioEvent(
    unsigned long cueId,
    const void* debugName,
    void* context,
    bool restartable)
{
    if (!mIsEnabled)
    {
        return;
    }

    ::PlayTrackedSound(sSoundCategory, cueId,
        debugName != 0 ? debugName : "FESFX", context, restartable);
}

void FEAudio::StopAnimAudioEvent(unsigned long cueId, void* context)
{
    ::StopSound(cueId, context);
}

void FEAudio::PauseSound(unsigned long cueId, void* context)
{
    ::PauseSound(cueId, context);
}

void FEAudio::ResumeSound(unsigned long cueId, void* context)
{
    ::ResumeSound(cueId, context);
}

void FEAudio::EnableSounds(bool enable)
{
    mIsEnabled = enable;
}

void FEAudio::SetSoundCategory(int category)
{
    sSoundCategory = category;
}

bool FEAudio::IsSoundFinished(unsigned long cueId, void* context)
{
    XSoundHandle* handle = FindSoundHandle(cueId, context);
    if (handle == 0)
    {
        return true;
    }

    switch (handle->m_State)
    {
    case 2:
    case 3:
    case 4:
    case 5:
        return false;
    default:
        return true;
    }
}
