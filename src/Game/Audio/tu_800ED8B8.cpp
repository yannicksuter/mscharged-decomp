#include "Game/Sys/audio.h"

#include "Game/Audio/AudioBundleManager_802EDA7C.h"
#include "Game/Player.h"
#include "Game/TweakRegistry.h"
#include "NL/globalpad.h"
#include "NL/nlString.h"
#include "unclassified/tu_80336B2C.h"

extern "C" long long OSGetTime();
extern "C" void fn_802F49AC(unsigned long*, int);

struct AudioControllerOwner_800EDCE8
{
    u8 m_Unknown00[4];
    int m_PadIndex;
};

struct RestrictedStreamSlot_800ED92C
{
    unsigned long m_CueId;
    unsigned long m_LastPlayedMilliseconds;
};

void* lbl_806E0E80;
XSoundHandle_802ED74C* lbl_806E0E84;

static char lbl_805041B0[] = "user/RestrictStreams";
static char lbl_805041C8[] = "CrowdReaction";
static char lbl_805041D8[] = "CaptainChant";
static char lbl_805041E8[] = "CapSigPUActivate";
static char lbl_805041FC[] = "Sudden Death Music";
static char lbl_80504210[] = "ControllerSpeaker";

static RestrictedStreamSlot_800ED92C lbl_8056DCA8[2];

static bool lbl_806E0E88;
static bool lbl_806E0E89;
static bool lbl_806E0E8A;
static bool lbl_806E0E8B;

inline bool AudioBundleManager_802EDA7C::fn_800ED8B8()
{
    return m_Initialized;
}

void XSoundHandle_802ED74C::fn_800ED8C0(unsigned char enabled)
{
    m_CallbackEnabled = enabled;
}

extern "C" void fn_800ED8C8(XSoundHandle_802ED74C* handle)
{
    handle->m_State = 9;
}

extern "C" void fn_800ED8D4()
{
}

static inline unsigned long GetAudioMilliseconds_800ED92C()
{
    unsigned long ticksPerMillisecond
        = (*(unsigned long*)0x800000F8 >> 2) / 1000;
    return (unsigned long)(OSGetTime() / ticksPerMillisecond);
}

extern "C" void fn_800ED8D8()
{
    int i = 0;
    RestrictedStreamSlot_800ED92C* slot = lbl_8056DCA8;
    for (; i < 2; ++i, ++slot)
    {
        fn_800EC12C(slot->m_CueId, slot);
    }
}

extern "C" void fn_800ED92C(unsigned long cueId)
{
    if (fn_802C2C84(lbl_805041B0, false) || lbl_806E0E8B)
    {
        return;
    }

    RestrictedStreamSlot_800ED92C* slot = 0;
    for (int i = 0; i < 2; ++i)
    {
        if (lbl_8056DCA8[i].m_CueId == cueId)
        {
            slot = &lbl_8056DCA8[i];
            break;
        }
    }

    bool play = true;
    unsigned long now = GetAudioMilliseconds_800ED92C();
    if (slot != 0)
    {
        play = now - slot->m_LastPlayedMilliseconds > 1200;
    }

    if (!play)
    {
        return;
    }

    if (slot == 0)
    {
        slot = &lbl_8056DCA8[0];
        if (lbl_8056DCA8[1].m_LastPlayedMilliseconds
            < slot->m_LastPlayedMilliseconds)
        {
            slot = &lbl_8056DCA8[1];
        }
    }

    fn_800EC12C(slot->m_CueId, slot);
    slot->m_LastPlayedMilliseconds = now;
    slot->m_CueId = cueId;
    fn_800EBC84(14, cueId, 0, lbl_805041C8, slot);
}

extern "C" void fn_800EDA84(
    int slotId, unsigned long cueId, void* context)
{
    lbl_806E0E8B = true;

    int i = 0;
    RestrictedStreamSlot_800ED92C* slot = lbl_8056DCA8;
    for (; i < 2; ++i, ++slot)
    {
        fn_800EC12C(slot->m_CueId, slot);
    }

    fn_800EC058(slotId, cueId, 0, lbl_805041D8, context, false);
}

extern "C" void fn_800EDB04(unsigned long cueId, void* context)
{
    lbl_806E0E8B = false;
    fn_800EC12C(cueId, context);
}

extern "C" void fn_800EDB10(
    int slotId, unsigned long cueId, void* context)
{
    lbl_806E0E8B = true;

    int i = 0;
    RestrictedStreamSlot_800ED92C* slot = lbl_8056DCA8;
    for (; i < 2; ++i, ++slot)
    {
        fn_800EC12C(slot->m_CueId, slot);
    }

    fn_800EC058(slotId, cueId, 0, lbl_805041E8, context, false);
}

extern "C" void fn_800EDB90(unsigned long cueId, void* context)
{
    lbl_806E0E8B = false;
    fn_800EC12C(cueId, context);
}

extern "C" void fn_800EDB9C()
{
    if (!lbl_806E0E8A)
    {
        fn_800EC058(18, 0x1326CB0C, 0, lbl_805041FC,
            (void*)-1, true);
        lbl_806E0E88 = true;
    }
    lbl_806E0E89 = true;
}

extern "C" void fn_800EDBF8()
{
    fn_800EC2A4(0x1326CB0C, (void*)-1);
    lbl_806E0E8A = true;
}

extern "C" void fn_800EDC2C()
{
    lbl_806E0E8A = false;
    if (lbl_806E0E88)
    {
        fn_800EC400(0x1326CB0C, (void*)-1);
    }
    else if (lbl_806E0E89)
    {
        fn_800EC058(18, 0x1326CB0C, 0, lbl_805041FC,
            (void*)-1, true);
        lbl_806E0E88 = true;
        lbl_806E0E89 = true;
    }
}

extern "C" void fn_800EDCAC()
{
    fn_800EC12C(0x1326CB0C, (void*)-1);
    lbl_806E0E8A = false;
    lbl_806E0E88 = false;
    lbl_806E0E89 = false;
}

extern "C" void fn_800EDCE8(cPlayer* player)
{
    DetInput* globalPad = 0;
    if (player != 0)
    {
        globalPad = player->GetGlobalPad();
    }

    AudioControllerOwner_800EDCE8* owner = 0;
    if (globalPad != 0)
    {
        owner = (AudioControllerOwner_800EDCE8*)fn_80336D90(
            (UnidentifiedNetworkPeerChannel*)globalPad->m_pMyUser);
    }

    if (owner != 0)
    {
        int padIndex = owner->m_PadIndex;
        unsigned long hash = nlStringLowerHash(lbl_80504210);
        fn_802F49AC(&hash, padIndex + 1);
    }
    else
    {
        unsigned long hash = nlStringLowerHash(lbl_80504210);
        fn_802F49AC(&hash, -1);
    }
}
