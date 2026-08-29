#include "Game/Audio/AudioResourcePlatform_8035DE28.h"

#include "Game/SAnim.h"
#include "NL/MemAlloc.h"
#include "NL/nlAVLTree.h"
#include "NL/nlDebugFile.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlstring_tmpl.h"

extern "C" void fn_8004F594(int category, const char* format, ...);
extern "C" void* fn_8035C0B4(void* host, unsigned long size);
extern "C" void fn_8035C114(void* host, void* data);
extern "C" unsigned char* fn_8035EA28();
extern "C" void SPInitSoundTable(void* table, void* data, unsigned long base);
extern "C" void fn_8035E640(
    nlFile* file, void* data, unsigned int size, unsigned long userParam);

extern unsigned char* lbl_806E201C;
extern void* lbl_806E2020;
extern unsigned long lbl_806E21E8;

struct AudioBankValue_8035DE28
{
    char m_Name[32];
    unsigned long m_Size;
};

typedef AVLTreeEntry<unsigned long, AudioBankValue_8035DE28>
    AudioBankTreeEntry_8035DE28;

class AudioBankTreeAllocator_8035DE28
{
public:
    AudioBankTreeAllocator_8035DE28()
        : m_FreeList(m_Entries)
        , m_Storage(m_Entries)
    {
        for (int i = 0; i < 31; ++i)
            m_Storage[i].node.left = (AVLTreeNode*)&m_Storage[i + 1];
        m_Storage[31].node.left = 0;
    }

    void Allocate(AudioBankTreeEntry_8035DE28*& entry)
    {
        entry = m_FreeList;
        if (entry != 0)
            m_FreeList = (AudioBankTreeEntry_8035DE28*)entry->node.left;
    }

    void Free(AudioBankTreeEntry_8035DE28* entry)
    {
        entry->node.left = (AVLTreeNode*)m_FreeList;
        m_FreeList = entry;
    }

private:
    AudioBankTreeEntry_8035DE28* m_FreeList;
    AudioBankTreeEntry_8035DE28* m_Storage;
    AudioBankTreeEntry_8035DE28 m_Entries[32];
};

class AudioBankCallbackObject_8035DE28
{
public:
    virtual void fn_8035DE28_0(bool destroy) = 0;
    virtual bool fn_8035DE28_1(
        const unsigned long& key, AudioBankValue_8035DE28* value) = 0;
};

class AudioBankCallback_8035DE28
{
public:
    typedef bool (*Function)(
        const unsigned long&, AudioBankValue_8035DE28*);

    AudioBankCallback_8035DE28(Function function)
        : m_Mode(1)
        , m_Function(function)
    {
    }

    ~AudioBankCallback_8035DE28()
    {
        if (m_Mode == 2 && m_Object != 0)
            m_Object->fn_8035DE28_0(true);
        m_Mode = 0;
    }

    int m_Mode;
    union
    {
        Function m_Function;
        AudioBankCallbackObject_8035DE28* m_Object;
    };
};

typedef AVLTreeBase<unsigned long, AudioBankValue_8035DE28,
    AudioBankTreeAllocator_8035DE28, DefaultKeyCompare<unsigned long> >
    AudioBankTree_8035DE28;

static inline void RemoveAudioBank_8035DE28(
    AudioBankTree_8035DE28* tree, const unsigned long& key)
{
    AudioBankTreeEntry_8035DE28* node =
        (AudioBankTreeEntry_8035DE28*)tree->RemoveAVLNode(
            (AVLTreeNode**)&tree->m_Root, (void*)&key);
    if (node != 0)
        tree->m_Allocator.Free(node);
}

inline bool WalkAudioBanks_8035DE28(
    AudioBankTree_8035DE28* tree, AudioBankTreeEntry_8035DE28* node,
    const AudioBankCallback_8035DE28& callback)
{
    while (node != 0)
    {
        if (!WalkAudioBanks_8035DE28(tree,
                (AudioBankTreeEntry_8035DE28*)node->node.left, callback))
            return false;

        bool result;
        if (callback.m_Mode == 1)
            result = callback.m_Function(node->key, &node->value);
        else
            result = callback.m_Object->fn_8035DE28_1(
                node->key, &node->value);
        if (!result)
            return false;

        node = (AudioBankTreeEntry_8035DE28*)node->node.right;
    }
    return true;
}

struct AudioBlockPool_8035E6A0
{
    void* m_FreeList;
    void* m_Storage;
};

static unsigned long sCurrentStreamBytes;
static void* sDebugFile;
static unsigned long sResidentTotal;
static AudioBlockPool_8035E6A0* sBlockPool;
static AudioBankTree_8035DE28 sAudioBankTree;

static inline void AudioBankPrint_8035DE28(
    const char* format, unsigned long optionalFirst, unsigned long value)
{
    unsigned long first = optionalFirst != 0 ? optionalFirst : value;
    if (sDebugFile != 0)
    {
        char buffer[256];
        nlSNPrintf(buffer, sizeof(buffer), format, first, value);
        nlWriteLineDebug(sDebugFile, buffer, false);
    }
    else
    {
        fn_8004F594(10, format, first, value);
    }
}

extern "C" bool fn_8035DE28(
    const unsigned long&, AudioBankValue_8035DE28* value)
{
    const char* format = "%-25s%dk\n";
    unsigned long size = value->m_Size >> 10;
    if (sDebugFile != 0)
    {
        char buffer[256];
        nlSNPrintf(buffer, sizeof(buffer), format,
            value != 0 ? (unsigned long)value : size, size);
        nlWriteLineDebug(sDebugFile, buffer, false);
    }
    else
    {
        fn_8004F594(10, format,
            value != 0 ? (unsigned long)value : size, size);
    }
    return true;
}

extern "C" void fn_8035DEC4(const char* path)
{
    sDebugFile = nlOpenFileDebug(path, false, false);
    AudioBankPrint_8035DE28(
        "\n\n  --- AUDIO BANK MEMORY ---\n", 0, 0);

    sResidentTotal = 0;
    {
        AudioBankCallback_8035DE28 callback(fn_8035DE28);
        WalkAudioBanks_8035DE28(
            &sAudioBankTree, sAudioBankTree.m_Root, callback);
    }

    unsigned char* memoryRoot = fn_8035EA28();
    unsigned char* memory = *(unsigned char**)(memoryRoot + 4);
    unsigned long freeMemory =
        ((MemoryAllocator*)(memory + 0x434))->TotalFreeMemory() >> 10;

    memoryRoot = fn_8035EA28();
    memory = *(unsigned char**)(memoryRoot + 4);
    unsigned long totalMemory = *(unsigned long*)(memory + 0x43C) >> 10;

    AudioBankPrint_8035DE28(
        "<resident total>           %dk\n", 0, sResidentTotal);
    AudioBankPrint_8035DE28(
        "\nCurrent stream load      %dk\n", 0,
        totalMemory - sResidentTotal - freeMemory);
    AudioBankPrint_8035DE28(
        "\n<free>                   %dk\n", 0, freeMemory);
    AudioBankPrint_8035DE28(
        "-------------------------------\n", 0, 0);
    AudioBankPrint_8035DE28(
        "<total>                  %dk\n", 0, totalMemory);
    AudioBankPrint_8035DE28("\n\n", 0, 0);

    nlCloseFileDebug(sDebugFile);
    sDebugFile = 0;
}

void AudioMemoryLoader_8035E3CC::fn_8035E3CC(const char* name)
{
    unsigned int allocSize;
    AVLTreeNode* existingNode;
    unsigned long key = (unsigned long)this;
    char path[128];
    nlSNPrintf(path, sizeof(path), "%s%s.nlxwb", lbl_806E201C + 0x4A, name);

    nlFile* file;
    void* data;
    file = nlOpen(path);
    unsigned int size = nlFileSize(file, &allocSize);
    if (size == 0)
    {
        nlClose(file);
        ((AudioMemoryLoader_8035E3CC*)key)->fn_802EDA54();
        return;
    }

    data = fn_8035C0B4(lbl_806E2020, allocSize);
    AVLTreeNode* node = sAudioBankTree.AddAVLNode(
        (AVLTreeNode**)&sAudioBankTree.m_Root, (void*)&key, 0,
        &existingNode);
    AudioBankValue_8035DE28* value =
        &((AudioBankTreeEntry_8035DE28*)node)->value;
    if (existingNode != 0)
        value = 0;
    if (value != 0)
    {
        nlStrNCpy(value->m_Name, name, sizeof(value->m_Name));
        value->m_Size = size;
        sCurrentStreamBytes += size;
    }

    nlReadAsync(file, data, size, fn_8035E640, key, 0);
}

void AudioMemoryLoader_8035E3CC::fn_802EDA38_0()
{
    unsigned long key = (unsigned long)this;
    if (m_Data != 0)
    {
        fn_8035C114(lbl_806E2020, m_Data);
        RemoveAudioBank_8035DE28(&sAudioBankTree, key);
        m_Data = 0;
    }
}

void AudioMemoryLoader_8035E3CC::fn_802EF6B0(nlChunk* chunk)
{
    switch ((int)chunk->GetID())
    {
    case 0x23703:
        m_SoundTable = chunk->GetData();
        return;
    }
    AudioBankLoader_802EF6B0::fn_802EF6B0(chunk);
}

extern "C" void fn_8035E63C()
{
}

extern "C" void fn_8035E640(
    nlFile* file, void* data, unsigned int,
    unsigned long userParam)
{
    AudioMemoryLoader_8035E3CC* loader =
        (AudioMemoryLoader_8035E3CC*)userParam;
    nlClose(file);
    loader->m_Data = data;
    SPInitSoundTable(loader->m_SoundTable, data, lbl_806E21E8);
    loader->fn_802EDA54();
}

extern "C" bool fn_8035E6A0()
{
    unsigned char* storage =
        (unsigned char*)nlMalloc(0x600, 0x20, false);
    AudioBlockPool_8035E6A0* pool = new AudioBlockPool_8035E6A0;
    if (pool != 0)
    {
        pool->m_FreeList = storage;
        pool->m_Storage = storage;
        for (unsigned int i = 0; i < 15; ++i)
            *(void**)((unsigned char*)pool->m_Storage + i * 0x60) =
                (unsigned char*)pool->m_Storage + (i + 1) * 0x60;
        *(void**)((unsigned char*)pool->m_Storage + 15 * 0x60) = 0;
    }
    sBlockPool = pool;
    return true;
}

void AudioFileLoader_8035E7B4::fn_8035E3CC(const char* name)
{
    char path[128];
    nlSNPrintf(path, sizeof(path), "%s%s.nlxwb", lbl_806E201C + 0x4A, name);
    m_File = nlOpen(path);
    fn_802EDA54();
}

void AudioFileLoader_8035E7B4::fn_802EDA38_0()
{
    nlClose(m_File);
    m_File = 0;
}
