#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/AudioResourcePlatform.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioSystem.h"
#include "Game/Sys/debug.h"

#include "NL/nlChunk.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/UnidentifiedSoundPools.h"
#include "NL/MemAlloc.h"
#include "NL/nlAVLTree.h"
#include "NL/nlDebugFile.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlstring_tmpl.h"
#include "revolution/sp.h"

void OnAudioBankReadComplete(
    nlFile* file, void* data, unsigned int size, unsigned long userParam);

struct AudioBankValue
{
    char m_Name[32];
    unsigned long m_Size;
};

typedef AVLTreeEntry<AudioMemoryLoader*, AudioBankValue>
    AudioBankTreeEntry;

typedef AVLTreeBase<AudioMemoryLoader*, AudioBankValue,
    nlStaticArrayAllocator<AudioBankTreeEntry, 32>,
    DefaultKeyCompare<AudioMemoryLoader*> >
    AudioBankTree;

class AudioMemoryReportWriter
{
public:
    AudioMemoryReportWriter()
        : m_File(0)
    {
    }

    void Print(const char* format, const char* name = 0, unsigned long value = 0)
    {
        if (m_File != 0)
        {
            char buffer[256];
            nlSNPrintf(buffer, sizeof(buffer), format, name != 0 ? (unsigned long)name : value, value);
            nlWriteLineDebug(m_File, buffer, false);
        }
        else
        {
            tDebugPrintManager::Print(DC_SOUND, format, name != 0 ? (unsigned long)name : value, value);
        }
    }

    void* m_File;
};

static unsigned long sCurrentStreamBytes;
static AudioBankTree sAudioBankTree;
static AudioMemoryReportWriter sReport;
static unsigned long sResidentTotal;
nlArrayAllocator<AudioStreamBlock>* g_pAudioStreamBlockPool;

bool PrintAudioBankMemory(
    AudioMemoryLoader* const&, AudioBankValue* value)
{
    sReport.Print("%-25s%dk\n", value->m_Name, value->m_Size >> 10);
    return true;
}

static inline void WriteAudioBankMemory()
{
    sReport.Print(
        "\n\n  --- AUDIO BANK MEMORY ---\n", 0, 0);

    sResidentTotal = 0;
    sAudioBankTree.Walk(PrintAudioBankMemory);

    unsigned long freeMemory = GetAudioBundleManager()->GetBackend()->m_Unknown434.TotalFreeMemory() >> 10;
    unsigned long totalMemory = GetAudioBundleManager()->GetBackend()->m_Unknown434.m_08 >> 10;

    sReport.Print(
        "<resident total>           %dk\n", 0, sResidentTotal);
    sReport.Print(
        "\nCurrent stream load      %dk\n", 0, totalMemory - sResidentTotal - freeMemory);
    sReport.Print(
        "\n<free>                   %dk\n", 0, freeMemory);
    sReport.Print(
        "-------------------------------\n", 0, 0);
    sReport.Print(
        "<total>                  %dk\n", 0, totalMemory);
    sReport.Print("\n\n", 0, 0);
}

void DumpAudioBankMemory(const char* path)
{
    sReport.m_File = nlOpenFileDebug(path, false, false);
    WriteAudioBankMemory();
    nlCloseFileDebug(sReport.m_File);
    sReport.m_File = 0;
}

void AudioMemoryLoader::Load(const char* name)
{
    unsigned int allocSize;
    char path[128];
    nlSNPrintf(path, sizeof(path), "%s%s.nlxwb", g_pAudioSystem->m_ResourcePath, name);

    nlFile* file;
    void* data;
    file = nlOpen(path);
    unsigned int size = nlFileSize(file, &allocSize);
    if (size == 0)
    {
        nlClose(file);
        OnLoadComplete();
        return;
    }

    data = g_pAudioBackend->AllocateAudioMemory(allocSize);
    AudioBankValue* const value = sAudioBankTree.Add(this);
    if (value != 0)
    {
        nlStrNCpy(value->m_Name, name, sizeof(value->m_Name));
        value->m_Size = size;
        sCurrentStreamBytes += size;
    }

    nlReadAsync(file, data, size, OnAudioBankReadComplete, (unsigned long)this, 0);
}

void AudioMemoryLoader::Unload()
{
    if (m_Data != 0)
    {
        g_pAudioBackend->FreeAudioMemory(m_Data);
        sAudioBankTree.Remove(this);
        m_Data = 0;
    }
}

void AudioMemoryLoader::ParseChunk(nlChunk* chunk)
{
    switch ((int)chunk->GetID())
    {
    case 0x23703:
        m_SoundTable = (SPSoundTable*)chunk->GetData();
        break;
    default:
        AudioBankLoader::ParseChunk(chunk);
        break;
    }
}

void OnAudioBankReadComplete(
    nlFile* file, void* data, unsigned int,
    unsigned long userParam)
{
    AudioMemoryLoader* loader = (AudioMemoryLoader*)userParam;
    nlClose(file);
    loader->m_Data = data;
    SPInitSoundTable(loader->m_SoundTable, (u32)data, (u32)g_pAudioSilenceBuffer);
    loader->OnLoadComplete();
}

bool InitializeAudioStreamBlockPool()
{
    AudioStreamBlock* storage = (AudioStreamBlock*)nlMalloc(16 * sizeof(AudioStreamBlock), 0x20, false);
    g_pAudioStreamBlockPool = new nlArrayAllocator<AudioStreamBlock>(storage, 16);
    return true;
}

void AudioFileLoader::Load(const char* name)
{
    char path[128];
    nlSNPrintf(path, sizeof(path), "%s%s.nlxwb", g_pAudioSystem->m_ResourcePath, name);
    m_File = nlOpen(path);
    OnLoadComplete();
}

void AudioFileLoader::Unload()
{
    nlClose(m_File);
    m_File = 0;
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
template struct UnidentifiedSoundPools<UnidentifiedSoundPoolTag>;
