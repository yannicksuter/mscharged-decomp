#include "Game/FE/FEAudio.h"

#include "Game/Sys/audio.h"

static bool mIsEnabled = true;
static int sUnidentifiedAudioCategory = 0x15;

extern "C" void fn_801CBC54(
    int slotId,
    unsigned long cueId,
    const void* debugName,
    void* context)
{
    if (!mIsEnabled)
    {
        return;
    }

    PlaySound(
        slotId, cueId, debugName != 0 ? debugName : "FESFX", context);
}

extern "C" void fn_801CBC78(
    int slotId,
    unsigned long cueId,
    const void* debugName,
    void* context)
{
    if (!mIsEnabled)
    {
        return;
    }

    PlayTrackedSound(
        slotId, cueId, debugName != 0 ? debugName : "FESFX", context, true);
}

extern "C" void fn_801CBCA0(
    unsigned long cueId,
    const void* debugName,
    void* context,
    bool restartable)
{
    if (!mIsEnabled)
    {
        return;
    }

    PlayTrackedSound(sUnidentifiedAudioCategory, cueId,
        debugName != 0 ? debugName : "FESFX", context, restartable);
}

extern "C" void fn_801CBCE4(unsigned long cueId, void* context)
{
    StopSound(cueId, context);
}

extern "C" void fn_801CBCE8(unsigned long cueId, void* context)
{
    PauseSound(cueId, context);
}

extern "C" void fn_801CBCEC(unsigned long cueId, void* context)
{
    ResumeSound(cueId, context);
}

void FEAudio::EnableSounds(bool enable)
{
    mIsEnabled = enable;
}

extern "C" void fn_801CBCF8(int category)
{
    sUnidentifiedAudioCategory = category;
}

extern "C" bool fn_801CBD00(unsigned long cueId, void* context)
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
