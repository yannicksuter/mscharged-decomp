#include "Game/Audio/XSoundHandle_802ED74C.h"

#include "NL/nlMemory.h"
#include "NL/nlPrint.h"

extern void* lbl_806E201C;

extern "C" void fn_802ECC54(void* audioSystem);
extern "C" void fn_8004F594(int category, const char* format, ...);

static char sHitMarkerWarning[]
    = "Warning: hit marker (%d) with no callback\n";
static char sStateFormat[]
    = "Slot: %d CueIndex: %d State: %d\n";
static char sLogFormat[] = "%s\n";

XSoundHandle_802ED74C::XSoundHandle_802ED74C(void* value1,
    XSoundOwner_802ED74C* owner, void* value2,
    XSoundHitMarkerCallback_802ED74C callback, void* callbackContext)
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
    fn_802ED8BC((u32**)value1, (u32)value2);
    if (m_Owner != 0)
    {
        m_Owner->count.references++;
    }
}

XSoundHandle_802ED74C::~XSoundHandle_802ED74C()
{
    if (m_Owner != 0)
    {
        m_Owner->count.references--;
        if ((m_Owner->m_ReferencesAndFlags >> 12) & 1)
        {
            fn_802ECC54(lbl_806E201C);
            m_Owner = 0;
        }
    }
}

void XSoundHandle_802ED74C::fn_802ED88C(char* buffer, u32 size)
{
    nlSNPrintf(buffer, size, sStateFormat, **m_Slot, m_CueIndex, m_State, this);
}

void XSoundHandle_802ED74C::fn_802ED8BC(u32** slot, u32 cueIndex)
{
    m_Slot = slot;
    m_CueIndex = cueIndex;
    m_State = 1;
}

void XSoundHandle_802ED74C::fn_802ED8D0(void* value)
{
    if (m_Callback != 0)
    {
        m_Callback(m_CallbackContext, this, value);
        return;
    }
    fn_8004F594(10, sHitMarkerWarning, value);
}

void XSoundHandle_802ED74C::fn_802ED904()
{
    char buffer[0x100];
    nlSNPrintf(buffer, sizeof(buffer), sStateFormat, **m_Slot, m_CueIndex, m_State);
    fn_8004F594(10, sLogFormat, buffer);
}
