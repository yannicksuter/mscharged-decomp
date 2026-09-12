#include "revolution/dvd/dvd.h"
#include "revolution/os/OSInterrupt.h"
#include "revolution/os/OSThread.h"

#include "NL/nlFileGC.h"
#include "NL/nlDLRing.h"
#include "NL/nlFile.h"
#include "NL/nlFunction.h"
#include "NL/nlMemory.h"
#include <stdio.h>
#include <string.h>

class DolphinFile;
class DolphinFileAllocator_80589450;
class AsyncManager;
struct AsyncEntry;

typedef void (*CancelAsyncCallback)(nlFile*, void*, unsigned int, unsigned long, ReadAsyncCallback);

static Function<void(int)> g_HandleDVDMessageCallback;
static Function<void(int)> g_HandleDVDAllClearCallback;
static Function<void(int)> g_HandleDVDRetryCB;
static Function<FnVoidVoid> g_CheckForResetCB;

class DolphinFile : public nlFile
{
public:
    DolphinFile(int entryNum)
        : m_Position(0)
        , PendingAsync(0)
        , mEntryNum(entryNum)
    {
        DVDFileInfo info;
        DVDFastOpen(entryNum, &info);
        mLength = info.size;
        mStartAddress = info.offset;
        DVDClose(&info);
    }

    virtual ~DolphinFile();
    virtual u32 FileSize(unsigned int* size);
    virtual void Read(void* buffer, unsigned int size, unsigned long bufferSize);

    void* operator new(unsigned long);
    void operator delete(void* ptr);

    /* 0x04 */ unsigned long m_Position;
    /* 0x08 */ unsigned long PendingAsync;
    /* 0x0C */ int mEntryNum;
    /* 0x10 */ unsigned long mStartAddress;
    /* 0x14 */ unsigned long mLength;

    static DolphinFileAllocator_80589450 sAllocator;
};

class DolphinFileAllocator_80589450
{
public:
    DolphinFileAllocator_80589450()
        : m_pFree(0)
        , mMemory((DolphinFile*)mStorage)
    {
        m_pFree = mMemory;
        for (int i = 0; i < 95; ++i)
        {
            *(DolphinFile**)(mMemory + i) = mMemory + i + 1;
        }
        *(DolphinFile**)(mMemory + 95) = 0;
    }

    ~DolphinFileAllocator_80589450() { }

    DolphinFile* Allocate()
    {
        DolphinFile* entry = m_pFree;
        if (entry == 0)
        {
            return 0;
        }
        m_pFree = *(DolphinFile**)entry;
        return entry;
    }

    void DeleteEntry(DolphinFile* entry)
    {
        *(DolphinFile**)entry = m_pFree;
        m_pFree = entry;
    }

private:
    DolphinFile* m_pFree;
    DolphinFile* mMemory;
    unsigned char mStorage[sizeof(DolphinFile) * 96];
};

inline void* DolphinFile::operator new(unsigned long)
{
    return sAllocator.Allocate();
}

inline void DolphinFile::operator delete(void* ptr)
{
    sAllocator.DeleteEntry((DolphinFile*)ptr);
}

DolphinFileAllocator_80589450 DolphinFile::sAllocator;

enum AsyncReadPhase
{
    READ_HEAD = 0,
    READ_COMPLETE = 1,
    READ_TAIL = 2,
    READ_TAIL_ONLY = 3,
};

struct AsyncEntry
{
    /* 0x00 */ AsyncEntry* m_next;
    /* 0x04 */ AsyncEntry* m_prev;
    /* 0x08 */ DolphinFile* m_pFile;
    /* 0x0C */ DVDFileInfo mFileInfo;
    /* 0x48 */ void* m_pBuffer;
    /* 0x4C */ unsigned long m_uSize;
    /* 0x50 */ unsigned long m_uPosition : 30;
    /* 0x50 */ unsigned long Phase : 2;
    /* 0x54 */ void* mTailBuffer;
    /* 0x58 */ ReadAsyncCallback m_pFunc;
    /* 0x5C */ unsigned long m_uParam;
};

class AsyncManager
{
public:
    AsyncManager();

    AsyncEntry* AddEntry(DolphinFile* pFile, ReadAsyncCallback pFunc, void* pBuffer,
        unsigned long position, unsigned long uSize, unsigned long uParam, AsyncReadPhase phase);
    int Service();
    void CancelPendingReads(DolphinFile* pFile, CancelAsyncCallback callback);
    bool Contains(AsyncEntry* entry) const;
    bool Cancel(AsyncEntry* entry, CancelAsyncCallback callback);

    /* 0x0000 */ AsyncEntry* mCurrent;
    /* 0x0004 */ AsyncEntry* m_freeEntryList;
    /* 0x0008 */ AsyncEntry* m_activeEntryList;
    /* 0x000C */ AsyncEntry m_asyncEntries[64];
    /* 0x180C */ unsigned char* mTailBuffers;
};

static AsyncManager* s_pAsyncManager;
static bool sServicingReads;

bool IsAsyncReadBusy(AsyncEntry* entry);

static bool CheckDVDStatus()
{
    bool WasAProblem = false;

    for (;;)
    {
        int Status = DVDGetDriveStatus();
        switch (Status)
        {
        case DVD_STATE_FATAL:
        case DVD_STATE_NO_DISK:
        case DVD_STATE_COVER_OPENED:
        case DVD_STATE_WRONG_DISK_ID:
        case DVD_STATE_CANCELED:
            if (g_HandleDVDMessageCallback)
            {
                g_HandleDVDMessageCallback(Status);
            }
            WasAProblem = true;

            while (Status == DVDGetDriveStatus())
            {
                OSYieldThread();
                if (g_CheckForResetCB)
                {
                    g_CheckForResetCB();
                }
            }
            break;

        case DVD_STATE_BUSY:
            WasAProblem = true;
            if (g_HandleDVDRetryCB)
            {
                g_HandleDVDRetryCB(1);
            }
            while (DVDGetDriveStatus() == DVD_STATE_BUSY)
            {
                OSYieldThread();
                if (g_CheckForResetCB)
                {
                    g_CheckForResetCB();
                }
            }
            break;

        default:
            break;
        }

        if (Status == DVD_STATE_IDLE || Status == DVD_STATE_FATAL)
        {
            break;
        }
    }

    if (WasAProblem && g_HandleDVDAllClearCallback)
    {
        g_HandleDVDAllClearCallback(0);
    }
    return WasAProblem;
}

void nlRegHandleDVDMessageCB(const Function<void(int)>& cb)
{
    g_HandleDVDMessageCallback = cb;
}

void nlRegHandleDVDAllClearCB(const Function<void(int)>& cb)
{
    g_HandleDVDAllClearCallback = cb;
}

void nlRegHandleDVDRetryingCB(const Function<void(int)>& cb)
{
    g_HandleDVDRetryCB = cb;
}

void nlRegCheckForResetFromFSCB(const Function<FnVoidVoid>& cb)
{
    g_CheckForResetCB = cb;
}

nlFile* nlOpen(const char* fileName)
{
    long FileEntrynum;
    nlFile* file;

    FileEntrynum = DVDConvertPathToEntrynum(fileName);
    if (FileEntrynum == -1)
    {
        file = 0;
    }
    else
    {
        file = new DolphinFile(FileEntrynum);
    }
    return file;
}

DolphinFile::~DolphinFile()
{
}

u32 DolphinFile::FileSize(unsigned int* size)
{
    if (size != 0)
    {
        *size = AlignUp32(mLength);
    }
    return mLength;
}

void DolphinFile::Read(void* buffer, unsigned int size, unsigned long bufferSize)
{
    unsigned long alignedSize;
    unsigned long tailSize;

    if (bufferSize > size)
    {
        alignedSize = AlignUp32(size);
        tailSize = 0;
    }
    else
    {
        tailSize = size & 31;
        alignedSize = size - tailSize;
    }

    unsigned long position = m_Position;
    AsyncEntry* finalEntry;
    if (alignedSize != 0)
    {
        finalEntry = s_pAsyncManager->AddEntry(this, 0, buffer, position, alignedSize, 0, tailSize == 0 ? READ_COMPLETE : READ_HEAD);
        if (tailSize != 0)
        {
            s_pAsyncManager->AddEntry(this, 0, (unsigned char*)buffer + alignedSize, position + alignedSize, tailSize, 0, READ_TAIL);
        }
    }
    else
    {
        finalEntry = s_pAsyncManager->AddEntry(this, 0, buffer, position, tailSize, 0, READ_TAIL_ONLY);
    }

    m_Position += size;
    while (s_pAsyncManager->Contains(finalEntry))
    {
        OSYieldThread();
        s_pAsyncManager->Service();
    }
}

void AsyncManager::CancelPendingReads(DolphinFile* pFile, CancelAsyncCallback callback)
{
    if (pFile->PendingAsync == 0 || m_activeEntryList == 0)
    {
        return;
    }

    AsyncEntry* entry = m_activeEntryList->m_next;
    for (;;)
    {
        AsyncEntry* next = entry->m_next;
        BOOL interrupts = OSDisableInterrupts();
        if (entry->m_pFile == pFile && !IsAsyncReadBusy(entry))
        {
            --pFile->PendingAsync;
            mCurrent = entry;
            if (callback != 0)
            {
                callback(entry->m_pFile, entry->m_pBuffer, entry->m_uSize, entry->m_uParam, entry->m_pFunc);
            }
            nlDLRingRemove(&m_activeEntryList, entry);
            nlDLRingAddEnd(&m_freeEntryList, entry);
            DVDClose(&entry->mFileInfo);
        }
        OSRestoreInterrupts(interrupts);

        if (m_activeEntryList == 0 || entry == m_activeEntryList)
        {
            break;
        }
        entry = next;
    }
}

AsyncEntry* AsyncManager::AddEntry(DolphinFile* pFile, ReadAsyncCallback pFunc,
    void* pBuffer, unsigned long position, unsigned long uSize, unsigned long uParam,
    AsyncReadPhase phase)
{
    if (m_freeEntryList == 0)
    {
        return 0;
    }

    AsyncEntry* pEntry = nlDLRingRemoveStart(&m_freeEntryList);
    pEntry->m_pFile = pFile;
    ++pFile->PendingAsync;
    pEntry->m_pFunc = pFunc;
    pEntry->m_pBuffer = pBuffer;
    pEntry->m_uSize = uSize;
    pEntry->m_uParam = uParam;
    pEntry->m_uPosition = position;
    pEntry->Phase = phase;

    DVDFastOpen(pFile->mEntryNum, &pEntry->mFileInfo);
    nlDLRingAddEnd(&m_activeEntryList, pEntry);

    void* readBuffer = (phase & READ_TAIL) != 0 ? pEntry->mTailBuffer : pBuffer;
    DVDReadAsyncPrio(&pEntry->mFileInfo, readBuffer, AlignUp32(uSize), pEntry->m_uPosition, 0, 2);
    return pEntry;
}

int AsyncManager::Service()
{
    AsyncEntry* entry = m_activeEntryList == 0 ? 0 : m_activeEntryList->m_next;
    bool stop = false;

    while (m_activeEntryList != 0 && !stop)
    {
        int status = DVDGetCommandBlockStatus(&entry->mFileInfo.block);
        if (status == DVD_STATE_BUSY || status == DVD_STATE_WAITING)
        {
            stop = true;
            continue;
        }
        if (status != DVD_STATE_IDLE)
        {
            CheckDVDStatus();
            continue;
        }

        if (sServicingReads)
        {
            AsyncEntry* last = m_activeEntryList->m_next;
            while (last != 0 && last != entry)
            {
                last = last->m_next;
                if (last == m_activeEntryList->m_next)
                {
                    break;
                }
            }
        }

        AsyncReadPhase phase = (AsyncReadPhase)entry->Phase;
        if (phase == READ_HEAD)
        {
            entry = entry->m_next;
            continue;
        }

        if (phase == READ_TAIL || phase == READ_TAIL_ONLY)
        {
            memcpy(entry->m_pBuffer, entry->mTailBuffer, entry->m_uSize);
        }

        if (phase == READ_TAIL)
        {
            AsyncEntry* head = entry->m_prev;
            --entry->m_pFile->PendingAsync;
            head->m_uSize += entry->m_uSize;
            DVDGetCommandBlockStatus(&head->mFileInfo.block);
            nlDLRingRemove(&m_activeEntryList, entry);
            nlDLRingAddEnd(&m_freeEntryList, entry);
            DVDClose(&entry->mFileInfo);
            entry = head;
        }

        mCurrent = entry;
        --entry->m_pFile->PendingAsync;
        AsyncEntry* next = entry->m_next == entry ? 0 : entry->m_next;
        nlDLRingRemove(&m_activeEntryList, entry);
        nlDLRingAddEnd(&m_freeEntryList, entry);
        DVDClose(&entry->mFileInfo);

        if (entry->m_pFunc != 0)
        {
            entry->m_pFunc(entry->m_pFile, entry->m_pBuffer, entry->m_uSize, entry->m_uParam);
        }
        entry = next != 0 ? next : m_activeEntryList;
    }
    return stop;
}

bool AsyncManager::Contains(AsyncEntry* wanted) const
{
    if (m_activeEntryList == 0)
    {
        return false;
    }

    AsyncEntry* entry = m_activeEntryList->m_next;
    do
    {
        if (entry == wanted)
        {
            return true;
        }
        entry = entry->m_next;
    } while (entry != m_activeEntryList->m_next);
    return false;
}

bool AsyncManager::Cancel(AsyncEntry* entry, CancelAsyncCallback callback)
{
    if (!Contains(entry))
    {
        return false;
    }

    --entry->m_pFile->PendingAsync;
    mCurrent = entry;
    if (callback != 0)
    {
        callback(entry->m_pFile, entry->m_pBuffer, entry->m_uSize, entry->m_uParam, entry->m_pFunc);
    }
    nlDLRingRemove(&m_activeEntryList, entry);
    nlDLRingAddEnd(&m_freeEntryList, entry);
    DVDClose(&entry->mFileInfo);
    return true;
}

AsyncManager::AsyncManager()
    : mCurrent(0)
    , m_freeEntryList(0)
    , m_activeEntryList(0)
{
    mTailBuffers = (unsigned char*)nlMalloc(64 * 32, 32, true);
    for (int i = 0; i < 64; ++i)
    {
        AsyncEntry* entry = &m_asyncEntries[i];
        entry->mTailBuffer = mTailBuffers + i * 32;
        entry->m_uPosition = 0;
        entry->Phase = READ_HEAD;
        nlDLRingAddEnd(&m_freeEntryList, entry);
    }
}

AsyncEntry* nlGetCurrentAsyncRead()
{
    return s_pAsyncManager->mCurrent;
}

void nlInitFileSystem()
{
    DVDInit();
    if (s_pAsyncManager == 0)
    {
        AsyncManager* pManager =
            (AsyncManager*)nlMalloc(sizeof(AsyncManager), 8, false);
        s_pAsyncManager = pManager == 0 ? 0 : new (pManager) AsyncManager;
    }
}

void nlServiceFileSystem()
{
    s_pAsyncManager->Service();
}

AsyncEntry* nlReadAsync(nlFile* file, void* buffer, unsigned int size,
    ReadAsyncCallback callback, unsigned long uParam, unsigned long bufferSize)
{
    unsigned long position;
    unsigned long alignedSize;
    unsigned long tailSize;

    if (bufferSize > size)
    {
        alignedSize = AlignUp32(size);
        tailSize = 0;
    }
    else
    {
        alignedSize = size - (size & 31);
        tailSize = size - alignedSize;
    }

    position = ((DolphinFile*)file)->m_Position;
    AsyncEntry* firstEntry;
    if (alignedSize != 0)
    {
        firstEntry = s_pAsyncManager->AddEntry((DolphinFile*)file, callback, buffer, position, alignedSize, uParam, tailSize == 0 ? READ_COMPLETE : READ_HEAD);
        if (tailSize != 0)
        {
            s_pAsyncManager->AddEntry((DolphinFile*)file, 0, (unsigned char*)buffer + alignedSize, position + alignedSize, tailSize, 0, READ_TAIL);
        }
    }
    else
    {
        firstEntry = s_pAsyncManager->AddEntry((DolphinFile*)file, callback, buffer, position, tailSize, uParam, READ_TAIL_ONLY);
    }

    ((DolphinFile*)file)->m_Position += size;
    return firstEntry;
}

void nlSeek(nlFile* file, unsigned int offset, unsigned long origin)
{
    DolphinFile* gcFile = (DolphinFile*)file;
    switch (origin)
    {
    case 0:
        gcFile->m_Position = offset;
        break;
    case 1:
        gcFile->m_Position += offset;
        break;
    case 2:
        gcFile->m_Position = gcFile->FileSize(0) - offset;
        break;
    }
}

void* nlLoadEntireHostFile(const char* fileName, unsigned long* outSize,
    unsigned int alignment, eAllocType type, void* target)
{
    void* buffer = 0;
    FILE* file = fopen(fileName, "rb");
    if (file != 0)
    {
        fseek(file, 0, 2);
        *outSize = ftell(file);
        fseek(file, 0, 0);
        unsigned long size = *outSize;

        if (size != 0)
        {
            if (target != 0)
            {
                buffer = target;
            }
            else if (type == AllocateEnd)
            {
                buffer = operator new(size, alignment, true);
            }
            else
            {
                buffer = operator new(size, alignment, false);
            }
            fread(buffer, 1, size, file);
        }
        fclose(file);
        if (outSize != 0)
        {
            *outSize = size;
        }
    }
    return buffer;
}

bool IsAsyncReadBusy(AsyncEntry* entry)
{
    switch (DVDGetCommandBlockStatus(&entry->mFileInfo.block))
    {
    case DVD_STATE_BUSY:
        return true;
    case DVD_STATE_WAITING:
        return false;
    case DVD_STATE_IDLE:
        if (entry->Phase == READ_HEAD)
        {
            return IsAsyncReadBusy(entry->m_next);
        }
        return true;
    }
    return false;
}

bool nlAsyncReadsPending(nlFile* file)
{
    if (file != 0)
    {
        return ((DolphinFile*)file)->PendingAsync != 0;
    }
    return s_pAsyncManager->m_activeEntryList != 0;
}

bool nlAsyncReadBusy(AsyncEntry* entry)
{
    if (!s_pAsyncManager->Contains(entry))
    {
        return false;
    }
    return IsAsyncReadBusy(entry);
}

void nlCancelPendingAsyncReads(nlFile* pFile, CancelAsyncCallback callback)
{
    if (pFile != 0)
    {
        s_pAsyncManager->CancelPendingReads((DolphinFile*)pFile, callback);
    }
}

bool nlCancelAsyncRead(AsyncEntry* entry, CancelAsyncCallback callback)
{
    return s_pAsyncManager->Cancel(entry, callback);
}

namespace
{
struct AsyncToVirMemBufferLoad
{
    int numChunksLeft;
    unsigned long param;
    ReadAsyncCallback callback;
    char* target;
    int size;

    AsyncToVirMemBufferLoad()
        : numChunksLeft(0)
    {
    }
};

AsyncToVirMemBufferLoad asyncToVirMemBufferLoad[4];
} // namespace
