#include "Game/Audio/AudioSystem.h"

#include "Game/Audio/AudioBankTable.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/XSoundCueHandle.h"
#include "Game/Audio/Plat3dSoundSrc.h"
#include "Game/Audio/XSoundHandle.h"
#include "NL/nlBind.h"
#include "NL/nlDebugFile.h"
#include "NL/nlstring_tmpl.h"

class UnidentifiedAudioBundleManager_802ECD34 : public AudioBundleManager
{
public:
    UnidentifiedAudioBundleManager_802ECD34()
        : m_Unknown4C(0), m_Unknown50(0)
    {
    }
    virtual ~UnidentifiedAudioBundleManager_802ECD34();
    virtual bool Initialize();
    virtual void ParseChunk(nlChunk* chunk);
    virtual void Shutdown();
    virtual void Update(float dt);

    void* m_Unknown4C;
    void* m_Unknown50;
};

extern "C" void fn_802F499C(void*, unsigned long, XSoundHandle*);
extern "C" void fn_802F49A4(void*, XSoundHandle*);
u32 FindAudioResourceCue(AudioResourceLoadOwner*, u32, u32, u32, u32);

unsigned int lbl_806E2018;
AudioSystem* g_pAudioSystem;
AudioBackend* g_pAudioBackend;

AudioSystem::AudioSystem()
    : m_Unknown48(false), m_AsyncLoading(true), m_BundleManager(0), m_Unknown2E0(0)
{
    m_ResourcePath[0] = '\0';
    g_pAudioSystem = this;
    m_BundleManager = new (8, false) UnidentifiedAudioBundleManager_802ECD34;
    m_Listener = new (8, false) PlatAudioListener;
}

void AudioSystem::SetResourcePath(const char* path)
{
    if (m_BundleManager->Initialize())
    {
        nlStrNCpy(m_ResourcePath, path, sizeof(m_ResourcePath));
        m_BundleManager->Load(m_ResourcePath);
    }
}

bool AudioSystem::IsIdle()
{
    if (!m_ActiveSoundList.IsEmpty())
        return false;
    if (m_SoundInstancePool.IsEmpty() == false)
        return false;
    return m_SoundOwnerPool.IsEmpty();
}

void AudioSystem::Shutdown()
{
    if (m_BundleManager != 0)
        m_BundleManager->Shutdown();
    m_SoundInstancePool.m_Allocator.FreeBlocks();
    m_SoundOwnerPool.m_Allocator.FreeBlocks();
}

XSoundHandle* CreateAudioSoundHandle(AudioSystem* audio, int slotId, XSoundOwner* owner,
    unsigned long cueId, int value1, int value2, int value3, int callback, int context)
{
    if (!audio->IsInitialized())
        return 0;
    AudioResourceLoadOwner* resource = audio->GetBundleManager()->GetSoundMap()->records_0C[slotId].field_10;
    u32 cueIndex = FindAudioResourceCue(resource, cueId, value1, value2, value3);
    if (cueIndex == 0xFFFF)
        return 0;
    ++lbl_806E2018;
    XSoundHandle* handle = new XSoundCueHandle(resource, owner, cueIndex,
        (XSoundHitMarkerCallback)callback, (void*)context);
    if (owner != 0)
        ++audio->m_Unknown2E0;
    if (handle != 0)
        audio->m_ActiveSoundList.AddEnd(handle);
    if (handle != 0)
        fn_802F499C((u8*)audio->GetBundleManager() + 0x18,
            (value3 ^ value1) ^ (value2 ^ cueId), handle);
    return handle;
}

struct AudioSliderSet_802EC1F4
{
    u32 m_Unknown00;
    AudioParameter* m_Parameters;
};

void UpdateAudioSystem(AudioSystem* audio, float dt)
{
    if (!audio->IsInitialized())
        return;
    while (audio->m_UnknownD0.GetCount() > 0)
        audio->m_UnknownD0.Pop()->IsValid();
    if (audio->m_Listener->IsTransformValid())
    {
        audio->m_Listener->Update(dt);
        audio->m_SoundInstancePool.Walk(
            Function1<bool, Plat3dSoundSrc&>(Bind<bool>(MemFun(&AudioSystem::fn_802ECDC8), audio, dt, placeholder0)));
        unsigned int count = audio->m_Unknown2E0;
        nlDLListIterator<XSoundHandle*> it = audio->m_ActiveSoundList.Begin();
        while (it.hasNext() && count != 0)
        {
            XSoundHandle* handle = *it;
            if (handle->m_Owner != 0)
            {
                Plat3dSoundSrc* owner = (Plat3dSoundSrc*)handle->m_Owner;
                AudioSliderSet_802EC1F4* sliders = (AudioSliderSet_802EC1F4*)handle->m_LocalSliders;
                float value1 = owner->m_Unknown10;
                float value2 = owner->m_Unknown14;
                float value3 = owner->m_Unknown18;
                if (sliders != 0)
                {
                    AudioParameter* parameter = (AudioParameter*)((u8*)sliders->m_Parameters + 0xC8);
                    if (value1 < parameter->m_Min)
                        parameter->m_Value = parameter->m_Min;
                    else if (value1 > parameter->m_Max)
                        parameter->m_Value = parameter->m_Max;
                    else
                        parameter->m_Value = value1;
                    parameter->m_Time = 0.0f;
                    parameter = sliders->m_Parameters;
                    if (value2 < parameter->m_Min)
                        parameter->m_Value = parameter->m_Min;
                    else if (value2 > parameter->m_Max)
                        parameter->m_Value = parameter->m_Max;
                    else
                        parameter->m_Value = value2;
                    parameter->m_Time = 0.0f;
                    parameter = (AudioParameter*)((u8*)sliders->m_Parameters + 0x28);
                    if (value3 < parameter->m_Min)
                        parameter->m_Value = parameter->m_Min;
                    else if (value3 > parameter->m_Max)
                        parameter->m_Value = parameter->m_Max;
                    else
                        parameter->m_Value = value3;
                    parameter->m_Time = 0.0f;
                }
                --count;
            }
            it.next();
        }
    }
    audio->GetBundleManager()->Update(dt);
    nlDLListIterator<XSoundHandle*> it = audio->m_ActiveSoundList.Begin();
    while (it.hasNext())
    {
        XSoundHandle* handle = *it;
        handle->Update(dt);
        if (handle->m_State == 9)
        {
            fn_802F49A4((u8*)audio->GetBundleManager() + 0x18, handle);
            if (handle->m_Owner != 0)
                --audio->m_Unknown2E0;
            audio->m_ActiveSoundList.Remove(&it);
            delete handle;
            --lbl_806E2018;
        }
        it.next();
    }
    nlDLListIterator<XSoundOwner*> owners = audio->m_SoundOwnerPool.Begin();
    while (owners.hasNext())
    {
        XSoundOwner* owner = *owners;
        if (owner->count.references == 0)
        {
            audio->m_SoundOwnerPool.Remove(&owners);
            DLListEntry<Plat3dSoundSrc>* entry = (DLListEntry<Plat3dSoundSrc>*)((u8*)owner - 8);
            nlDLRingRemove(&audio->m_SoundInstancePool.m_Head, entry);
            audio->m_SoundInstancePool.m_Allocator.Free(entry);
        }
        else
            owners.next();
    }
}

void FlushAudio(AudioSystem* audio, int param2, bool param3)
{
    nlDLListIterator<XSoundHandle*> it = audio->m_ActiveSoundList.Begin();
    while (it.hasNext())
    {
        XSoundHandle* handle = *it;
        int state = handle->m_State;
        if ((param3 && state == 7) || (unsigned int)(state - 2) <= 3)
        {
            if (param2 == 2)
                param2 = handle->IsCallbackEnabled();
            handle->Stop(param2 != 0, (void*)param3);
        }
        else if (state == 8 && param2 == 1)
            handle->Release();
        it.Step();
    }
}

extern "C" void fn_802EC9D0(AudioSystem* audio)
{
    nlDLListIterator<XSoundHandle*> it = audio->m_ActiveSoundList.Begin();
    while (it.hasNext())
    {
        (*it)->PrintState();
        it.Step();
    }
}

void DumpAudioSystem(AudioSystem* audio, const char* path)
{
    void* file = nlOpenFileDebug(path, false, false);
    if (file != 0)
    {
        nlDLListIterator<XSoundHandle*> it = audio->m_ActiveSoundList.Begin();
        while (it.hasNext())
        {
            char buffer[256];
            (*it)->FormatState(buffer, sizeof(buffer));
            nlWriteLineDebug(file, buffer, false);
            it.Step();
        }
        nlCloseFileDebug(file);
    }
}

extern "C" Plat3dSoundSrc* fn_802ECB68(AudioSystem* audio)
{
    return audio->m_SoundInstancePool.AllocateAtEnd(0);
}

void ReleaseAudioSoundOwner(void* value, void* owner)
{
    AudioSystem* audio = (AudioSystem*)value;
    audio->m_SoundOwnerPool.AddEnd((XSoundOwner*)owner);
}

bool AudioListener::IsTransformValid() { return m_TransformValid; }
bool AudioListener::IsEnabled() { return m_Enabled; }
bool AudioListener::HasTransform() { return m_HasTransform; }
void AudioListener::SetHasTransform(bool value) { m_HasTransform = value; }

UnidentifiedAudioBundleManager_802ECD34::~UnidentifiedAudioBundleManager_802ECD34()
{
}

bool AudioSystem::fn_802ECDC8(float dt, Plat3dSoundSrc& source)
{
    source.Update((PlatAudioListener*)m_Listener, dt);
    return true;
}

void XSoundHandle::Update(float dt)
{
    if (m_State == 4)
    {
        m_PreviousTime = m_CurrentTime;
        m_CurrentTime += dt;
    }
}

int XSoundHandle::IsCallbackEnabled() { return m_CallbackEnabled; }
Plat3dSoundSrc::~Plat3dSoundSrc() { }
