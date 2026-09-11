#include "Game/Audio/AudioGlobals.h"
#include "Game/Audio/UnidentifiedRegistryPools.h"
#include "Game/Audio/XSoundHandle.h"
#include "Game/Sys/debug.h"

#include "NL/nlMemory.h"
#include "NL/nlPrint.h"

void ReleaseAudioSoundOwner(void* audioSystem, void* value);

static char sHitMarkerWarning[]
    = "Warning: hit marker (%d) with no callback\n";
static char sStateFormat[]
    = "Slot: %d CueIndex: %d State: %d\n";
static char sLogFormat[] = "%s\n";

XSoundHandle::XSoundHandle(void* value1,
    XSoundOwner* owner, void* value2,
    XSoundHitMarkerCallback callback, void* callbackContext)
    : m_Slot(0)
    , m_CueIndex(0xFFFF)
    , m_State(0)
    , m_CallbackEnabled(false)
    , m_Owner(owner)
    , m_PreviousTime(0.0f)
    , m_CurrentTime(0.0f)
    , m_Unknown20(0)
    , m_Callback(callback)
    , m_CallbackContext(callbackContext)
{
    SetCue((u32**)value1, (u32)value2);
    if (m_Owner != 0)
    {
        m_Owner->count.references++;
    }
}

XSoundHandle::~XSoundHandle()
{
    if (m_Owner != 0)
    {
        m_Owner->count.references--;
        if ((m_Owner->m_ReferencesAndFlags >> 12) & 1)
        {
            ReleaseAudioSoundOwner(g_pAudioSystem, m_Owner);
            m_Owner = 0;
        }
    }
}

void XSoundHandle::FormatState(char* buffer, u32 size)
{
    nlSNPrintf(buffer, size, sStateFormat, **m_Slot, m_CueIndex, m_State);
}

void XSoundHandle::SetCue(u32** slot, u32 cueIndex)
{
    m_Slot = slot;
    m_CueIndex = cueIndex;
    m_State = 1;
}

void XSoundHandle::OnHitMarker(void* value)
{
    if (m_Callback != 0)
    {
        m_Callback(m_CallbackContext, this, value);
        return;
    }
    tDebugPrintManager::Print(DC_SOUND, sHitMarkerWarning, value);
}

void XSoundHandle::PrintState()
{
    char buffer[0x100];
    nlSNPrintf(buffer, sizeof(buffer), sStateFormat, **m_Slot, m_CueIndex, m_State);
    tDebugPrintManager::Print(DC_SOUND, sLogFormat, buffer);
}
