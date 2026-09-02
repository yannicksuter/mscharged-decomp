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

    fn_800EBBFC(
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

    fn_800EBF78(
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

    fn_800EBF78(sUnidentifiedAudioCategory, cueId,
        debugName != 0 ? debugName : "FESFX", context, restartable);
}

extern "C" void fn_801CBCE4(unsigned long cueId, void* context)
{
    fn_800EC12C(cueId, context);
}

extern "C" void fn_801CBCE8(unsigned long cueId, void* context)
{
    fn_800EC2A4(cueId, context);
}

extern "C" void fn_801CBCEC(unsigned long cueId, void* context)
{
    fn_800EC400(cueId, context);
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
    XSoundHandle_802ED74C* handle = fn_800EBEF4(cueId, context);
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
