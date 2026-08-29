#include "Game/Audio/AudioBundleManager_802EDA7C.h"

#include "Game/Audio/AudioLoadMode_806E201C.h"

#include "Game/SAnim.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"

extern "C" void* fn_802EB644(nlChunk* chunk);
extern "C" void fn_802EBADC(void* data);
extern "C" void* fn_802EE964(nlChunk* chunk);
extern "C" void* fn_802EF218(nlChunk* chunk);

AudioBundleManager_802EDA7C::AudioBundleManager_802EDA7C()
    : m_Backend(0)
    , m_Initialized(false)
    , m_Chunk13100(0)
    , m_Chunk13400(0)
    , m_Chunk13500(0)
    , m_Runtime()
    , m_Loaded(false)
{
    m_Backend = new (8, false) AudioBackend_8035B8E8;
}

bool AudioBundleManager_802EDA7C::fn_802EDB00()
{
    return m_Backend->fn_8035B8E8_1();
}

void AudioBundleManager_802EDA7C::fn_802EDB14(
    void* data, unsigned long, void* manager)
{
    nlChunk* chunk = (nlChunk*)data;
    AudioBundleManager_802EDA7C* self = (AudioBundleManager_802EDA7C*)manager;
    self->m_LoadedData = data;

    nlChunk* end = chunk->GetNextChunk();
    chunk = chunk->GetFirstChunk();
    while (chunk != end)
    {
        self->fn_802EDC34(chunk);
        chunk = chunk->GetNextChunk();
    }
    self->fn_802EE160();
}

void AudioBundleManager_802EDA7C::fn_802EDC34(nlChunk* chunk)
{
    switch (chunk->GetID())
    {
    case 0x23800:
        break;
    case 0x13500:
        m_Chunk13500 = fn_802EB644(chunk);
        fn_802EBADC(m_Chunk13500);
        break;
    case 0x13400:
        m_Chunk13400 = fn_802EF218(chunk);
        break;
    case 0x13100:
        m_Chunk13100 = fn_802EE964(chunk);
        break;
    case 0x21200:
        if (m_Runtime.fn_802F479C_1(
                chunk->GetData(),
                chunk->GetSize()
                    - ((u8*)chunk->GetData()
                        - (u8*)chunk->GetUnalignedData()),
                false))
        {
            m_Runtime.m_Unknown20 = m_Runtime.fn_802F479C_3();
        }
        break;
    case 0x23704:
        m_Runtime.fn_802F48FC(
            chunk->GetData(),
            chunk->GetSize()
                - ((u8*)chunk->GetData()
                    - (u8*)chunk->GetUnalignedData()));
        break;
    }
}

void AudioBundleManager_802EDA7C::fn_802EDEE4(const char* path)
{
    char fileName[128];
    nlSNPrintf(fileName, sizeof(fileName), "%s%s", path, "nlxgs.bun");

    if (lbl_806E201C->fn_806E201C_0())
    {
        nlLoadEntireFileAsync(fileName, fn_802EDB14, this, 0x20, AllocateStart, 0, 0, 0);
        return;
    }

    unsigned long size;
    void* data = nlLoadEntireFile(
        fileName, &size, 0x20, AllocateStart, 0, 0, 0);
    m_LoadedData = data;

    nlChunk* outerChunk = (nlChunk*)data;
    nlChunk* end = outerChunk->GetNextChunk();
    nlChunk* chunk = outerChunk->GetFirstChunk();
    while (chunk != end)
    {
        fn_802EDC34(chunk);
        chunk = chunk->GetNextChunk();
    }
    fn_802EE160();
}
