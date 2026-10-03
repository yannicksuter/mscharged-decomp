#include "NL/MemAlloc.h"
#include "NL/nlMath.h"
#include "NL/nlDebug.h"
#include "NL/nlDebugFile.h"
#include "NL/nlDLRing.h"
#include "NL/nlPrint.h"
#include <stdio.h>


bool g_bPrintMemoryNewLowWaterMarks;
bool g_bActivateMemoryLowWaterMarkChecking;

char sTotalFreeMemoryFormat[] = "Total Free Memory: %d\n";
char sLargestFreeBlockFormat[] = "Largest Free Block: %d\n";
char sFreePanicDumpFilename[] = "FreePanicDump.txt";
extern char sFreeMemoryDumpHeader[];
extern char sFreeMemoryDumpTotalFormat[];

struct MemoryStats
{
    MemoryStats(MemoryAllocator* allocator);
    u32 total;
    u32 largest;
    u32 count;
};

class MemoryStatsCallback
{
public:
    void Callback(FreeBlockList* block);

    MemoryStats* stats;
};

class FreeBlockDumpCallback
{
public:
    void Callback(FreeBlockList* block);

    void* file;
    u32 total;
    u32 count;
};

inline MemoryStats::MemoryStats(MemoryAllocator* allocator)
{
    MemoryStatsCallback callback;
    callback.stats = this;
    total = 0;
    largest = 0;
    count = 0;
    nlWalkDLRing(allocator->m_free_block_list, &callback, &MemoryStatsCallback::Callback);
}

static inline unsigned int GetTotalFreeMemory(MemoryAllocator* allocator)
{
    MemoryStats stats(allocator);
    return stats.total;
}

static inline unsigned int GetLargestFreeBlock(MemoryAllocator* allocator)
{
    MemoryStats stats(allocator);
    return stats.largest;
}

static inline void WriteFreeMemoryDump(MemoryAllocator* allocator, FILE* file)
{
    FreeBlockDumpCallback dump;
    dump.file = file;
    dump.total = 0;
    dump.count = 0;
    if (nlDebugFileIsValid(dump.file))
    {
        nlWriteLineDebug(dump.file, sFreeMemoryDumpHeader, false);
    }
    else
    {
        nlPrintf(sFreeMemoryDumpHeader);
    }

    nlWalkDLRing(allocator->m_free_block_list, &dump, &FreeBlockDumpCallback::Callback);

    char buffer[512];
    nlSNPrintf(buffer, sizeof(buffer), sFreeMemoryDumpTotalFormat, dump.total);
    buffer[511] = 0;
    if (nlDebugFileIsValid(dump.file))
    {
        nlWriteLineDebug(dump.file, buffer, false);
    }
    else
    {
        nlPrintf(buffer);
    }

    if (nlDebugFileIsValid(file))
    {
        nlCloseFileDebug(file);
    }
}

static inline void DumpFreeBlocks(MemoryAllocator* allocator, const char* filename)
{
    FILE* file;
    if (filename == 0)
    {
        file = 0;
    }
    else
    {
        file = (FILE*)nlOpenFileDebug(filename, false, false);
    }
    WriteFreeMemoryDump(allocator, file);
}

static inline void ReportAllocationFailure(MemoryAllocator* allocator)
{
    const char* filename;
    nlPrintf(sTotalFreeMemoryFormat, GetTotalFreeMemory(allocator));
    nlPrintf(sLargestFreeBlockFormat, GetLargestFreeBlock(allocator));

    filename = sFreePanicDumpFilename;
    DumpFreeBlocks(allocator, filename);
    nlBreak();
}

static inline void* InitializeAllocation(void* block, unsigned long size, u32 prefixSize, u32 suffixSize)
{
    void* result = (u8*)block + prefixSize;
    u32 header = size;
    if (prefixSize > 4)
    {
        header = size | 0x80000000;
        *(u32*)((u8*)result - 8) = prefixSize - 4;
    }
    u8* blockEnd = (u8*)result + size;
    if (suffixSize != 0)
    {
        header |= 0x40000000;
        *(u32*)(((u32)blockEnd + 3) & ~3u) = suffixSize;
    }
    *(u32*)((u8*)result - 4) = header;
    return result;
}

void* MemoryAllocator::AllocateFromStart(unsigned long size, unsigned int alignment)
{
    FreeBlockList* start = nlDLRingGetStart(m_free_block_list);
    FreeBlockList* cur = start;
    u32 alignedSize = (size + 3) & ~3u;
    u32 prefix;
    u32 usedSize;
    u32 blockSize;

    for (;;)
    {
        blockSize = cur->m_size;
        if (blockSize > alignedSize)
        {
            u32 address = nlAlignUp((u32)cur + 4, alignment);
            prefix = address - (u32)cur;
            usedSize = prefix + alignedSize;
            if (usedSize <= blockSize)
            {
                break;
            }
        }

        cur = cur->m_next;
        if (cur == start)
        {
            ReportAllocationFailure(this);
        }
    }

    FreeBlockList* prev = cur->m_prev;
    if (cur->m_next == cur)
    {
        m_free_block_list = 0;
    }
    else
    {
        cur->m_prev->m_next = cur->m_next;
        cur->m_next->m_prev = cur->m_prev;
        if (m_free_block_list == cur)
        {
            m_free_block_list = cur->m_prev;
        }
    }

    u32 remaining = cur->m_size - usedSize;
    if (remaining > 0xC)
    {
        FreeBlockList* newFree = (FreeBlockList*)((u8*)cur + usedSize);
        newFree->m_size = remaining;
        if (m_free_block_list == 0 || cur == start)
        {
            FreeBlockList* head = m_free_block_list;
            if (head == 0)
            {
                m_free_block_list = newFree;
                newFree->m_next = newFree;
                newFree->m_prev = newFree;
            }
            else
            {
                head->m_next->m_prev = newFree;
                newFree->m_next = head->m_next;
                newFree->m_prev = head;
                head->m_next = newFree;
            }
        }
        else
        {
            prev->m_next->m_prev = newFree;
            newFree->m_next = prev->m_next;
            newFree->m_prev = prev;
            prev->m_next = newFree;
            if (m_free_block_list == prev)
            {
                m_free_block_list = newFree;
            }
        }
        cur->m_size = usedSize;
    }

    void* result = InitializeAllocation(cur, size, prefix, cur->m_size - usedSize);
    m_allocation_count++;
    return result;
}

static inline void* AllocateFromBlockEnd(FreeBlockList** freeBlocks, FreeBlockList* block, unsigned long size,
    u32 address, u32 requestSize)
{
    u32 remaining = block->m_size - requestSize;
    u32 prefixSize = 4;
    if (remaining > sizeof(FreeBlockList))
    {
        block->m_size = remaining;
    }
    else
    {
        prefixSize = remaining + 4;
        nlDLRingRemove(freeBlocks, block);
    }
    return InitializeAllocation((void*)(address - prefixSize), size, prefixSize, requestSize - ((size + 3) & ~3u) - 4);
}

void* MemoryAllocator::AllocateFromEnd(unsigned long size, unsigned int alignment)
{
    FreeBlockList* end = nlDLRingGetEnd(m_free_block_list);
    u32 alignedSize = (size + 3) & ~3u;
    u32 alignMask = ~(alignment - 1);
    FreeBlockList* cur = end;
    u32 blockSize;
    u32 offset;
    u32 requestSize;

    for (;;)
    {
        blockSize = cur->m_size;
        if (blockSize > alignedSize)
        {
            u32 endAddress = (u32)cur + blockSize;
            u32 delta = endAddress - alignedSize;
            offset = delta & alignMask;
            requestSize = (endAddress - offset) + 4;
            if (requestSize <= blockSize)
            {
                break;
            }
        }

        cur = cur->m_next;
        if (cur == end)
        {
            ReportAllocationFailure(this);
        }
    }

    void* result = AllocateFromBlockEnd(&m_free_block_list, cur, size, offset, requestSize);
    m_allocation_count++;
    return result;
}

void MemoryAllocator::Free(void* p)
{
    if (p == 0)
    {
        return;
    }

    FreeBlockList* block = (FreeBlockList*)((u8*)p - 4);
    s32 size;
    s32 header;
    header = *(u32*)block;
    size = header & 0x3FFFFFFF;
    size = (size + 3) & 0xFFFFFFFC;
    if (header & 0x40000000)
    {
        size += *(u32*)((u8*)p + size);
    }
    size += 4;
    if (header & 0x80000000)
    {
        u32 offset = *(u32*)((u8*)block - 4);
        block = (FreeBlockList*)((u8*)block - offset);
        size += offset;
    }
    AddBlock(block, size);
}

void* MemoryAllocator::Allocate(unsigned long size, unsigned int alignment, bool fromEnd)
{
    if (alignment < 4)
    {
        alignment = 4;
    }
    if (size < 0xC)
    {
        size = 0xC;
    }
    if (fromEnd)
    {
        return AllocateFromEnd(size, alignment);
    }
    return AllocateFromStart(size, alignment);
}

void MemoryAllocator::Initialize(void* memory, unsigned int size)
{
    m_free_block_list = 0;
    AddBlock(memory, size);
    m_allocation_count = 0;
    m_memory = memory;
    m_memory_size = size;
    m_10 = 0x40000000;
    m_14 = 0x40000000;
}

void MemoryAllocator::AddBlock(void* memory, unsigned int size)
{
    FreeBlockList* block = (FreeBlockList*)memory;
    block->m_size = size;
    FreeBlockList* start = m_free_block_list == 0 ? 0 : m_free_block_list->m_next;
    if (start > block || start == 0)
    {
        FreeBlockList* head = m_free_block_list;
        if (head == 0)
        {
            m_free_block_list = block;
            block->m_next = block;
            block->m_prev = block;
        }
        else
        {
            head->m_next->m_prev = block;
            block->m_next = head->m_next;
            block->m_prev = head;
            head->m_next = block;
        }
    }
    else
    {
        FreeBlockList* iter = start->m_next;
        while (iter != start)
        {
            if (iter > block)
            {
                break;
            }
            iter = iter->m_next;
        }
        FreeBlockList* after = iter->m_prev;
        after->m_next->m_prev = block;
        block->m_next = after->m_next;
        block->m_prev = after;
        after->m_next = block;
        if (m_free_block_list == after)
        {
            m_free_block_list = block;
        }
    }

    FreeBlockList* next = block->m_next;
    if (next > block && (u8*)block + block->m_size == (u8*)next)
    {
        block->m_size += next->m_size;
        if (next->m_next == next)
        {
            m_free_block_list = 0;
        }
        else
        {
            next->m_prev->m_next = next->m_next;
            next->m_next->m_prev = next->m_prev;
            if (m_free_block_list == next)
            {
                m_free_block_list = next->m_prev;
            }
        }
    }

    FreeBlockList* prev = block->m_prev;
    if (prev < block && (u8*)prev + prev->m_size == (u8*)block)
    {
        prev->m_size += block->m_size;
        if (block->m_next == block)
        {
            m_free_block_list = 0;
        }
        else
        {
            block->m_prev->m_next = block->m_next;
            block->m_next->m_prev = block->m_prev;
            if (m_free_block_list == block)
            {
                m_free_block_list = block->m_prev;
            }
        }
    }
}

void MemoryStatsCallback::Callback(FreeBlockList* block)
{
    stats->total += block->m_size;
    u32 largest = stats->largest;
    if (block->m_size >= largest)
    {
        largest = block->m_size;
    }
    stats->largest = largest;
    stats->count++;
}

unsigned int MemoryAllocator::TotalFreeMemory()
{
    MemoryStats stats(this);
    return stats.total;
}

unsigned int MemoryAllocator::LargestFreeBlock()
{
    MemoryStats stats(this);
    return stats.largest;
}

char sFreeMemoryDumpHeader[] = "count   address     size        allocNum    file(line)  description \n";
char sFreeMemoryDumpTotalFormat[] = "Total free memory: %d\n";

void FreeBlockDumpCallback::Callback(FreeBlockList* block)
{
    char buffer[512];
    nlSNPrintf(buffer, sizeof(buffer), "%4d| 0x%08x| %10d| \n", count, block, block->m_size);
    buffer[511] = 0;
    if (nlDebugFileIsValid(file))
    {
        nlWriteLineDebug(file, buffer, false);
    }
    else
    {
        nlPrintf(buffer);
    }
    u32 newCount = count + 1;
    u32 newTotal = total + block->m_size;
    count = newCount;
    total = newTotal;
}
