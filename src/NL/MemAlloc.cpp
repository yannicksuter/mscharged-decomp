#include "NL/MemAlloc.h"
#include "NL/nlDebugFile.h"
#include "NL/nlDLRing.h"
#include "NL/nlPrint.h"

extern void nlBreak();

bool g_bPrintMemoryNewLowWaterMarks;
bool g_bActivateMemoryLowWaterMarkChecking;

char sTotalFreeMemoryFormat[] = "Total Free Memory: %d\n";
char sLargestFreeBlockFormat[] = "Largest Free Block: %d\n";
char sFreePanicDumpFilename[] = "FreePanicDump.txt";
extern char sFreeMemoryDumpHeader[];
extern char sFreeMemoryDumpTotalFormat[];

struct MemoryStats_802AF2E4
{
    u32 total;
    u32 largest;
    u32 count;
};

class MemoryStatsCallback_802AF2E4
{
public:
    void Callback(FreeBlockList* block);

    MemoryStats_802AF2E4* stats;
};

class FreePanicDumpCallback_802AF470
{
public:
    void Callback(FreeBlockList* block);

    void* file;
    u32 total;
    u32 count;
};

static inline unsigned int GetTotalFreeMemory(MemoryAllocator* allocator)
{
    MemoryStats_802AF2E4 stats;
    MemoryStatsCallback_802AF2E4 callback;
    callback.stats = &stats;
    stats.total = 0;
    stats.largest = 0;
    stats.count = 0;
    nlWalkDLRing(allocator->m_free_block_list, &callback, &MemoryStatsCallback_802AF2E4::Callback);
    return stats.total;
}

static inline unsigned int GetLargestFreeBlock(MemoryAllocator* allocator)
{
    MemoryStats_802AF2E4 stats;
    MemoryStatsCallback_802AF2E4 callback;
    callback.stats = &stats;
    stats.total = 0;
    stats.largest = 0;
    stats.count = 0;
    nlWalkDLRing(allocator->m_free_block_list, &callback, &MemoryStatsCallback_802AF2E4::Callback);
    return stats.largest;
}

void* MemoryAllocator::AllocateFromStart(unsigned long size, unsigned int alignment)
{
    FreeBlockList* start = nlDLRingGetStart(m_free_block_list);
    const char* filename;
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
            u32 address = (u32)cur + 4;
            u32 remainder = address % alignment;
            address += (alignment - remainder) * (remainder != 0);
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
            nlPrintf(sTotalFreeMemoryFormat, GetTotalFreeMemory(this));
            nlPrintf(sLargestFreeBlockFormat, GetLargestFreeBlock(this));

            filename = sFreePanicDumpFilename;
            FreePanicDumpCallback_802AF470 dump;
            if (filename == 0)
            {
                dump.file = 0;
            }
            else
            {
                dump.file = nlOpenFileDebug(filename, false, false);
            }
            dump.total = 0;
            dump.count = 0;
            void* file = dump.file;
            if (nlDebugFileIsValid(dump.file))
            {
                nlWriteLineDebug(dump.file, sFreeMemoryDumpHeader, false);
            }
            else
            {
                nlPrintf(sFreeMemoryDumpHeader);
            }

            nlWalkDLRing(m_free_block_list, &dump, &FreePanicDumpCallback_802AF470::Callback);

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
            nlBreak();
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

    u32 currentBlockSize = cur->m_size;
    u32 header = size;
    void* result = (u8*)cur + prefix;
    u32 suffixSize = currentBlockSize - usedSize;
    if (prefix > 4)
    {
        header = size | 0x80000000;
        *(u32*)((u8*)result - 8) = prefix - 4;
    }
    u8* blockEnd = (u8*)result + size;
    if (suffixSize != 0)
    {
        header |= 0x40000000;
        *(u32*)(((u32)blockEnd + 3) & ~3u) = suffixSize;
    }
    *(u32*)((u8*)result - 4) = header;
    m_04++;
    return result;
}

void* MemoryAllocator::AllocateFromEnd(unsigned long size, unsigned int alignment)
{
    FreeBlockList* end = nlDLRingGetEnd(m_free_block_list);
    const char* filename;
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
            nlPrintf(sTotalFreeMemoryFormat, GetTotalFreeMemory(this));
            nlPrintf(sLargestFreeBlockFormat, GetLargestFreeBlock(this));

            filename = sFreePanicDumpFilename;
            FreePanicDumpCallback_802AF470 dump;
            if (filename == 0)
            {
                dump.file = 0;
            }
            else
            {
                dump.file = nlOpenFileDebug(filename, false, false);
            }
            dump.total = 0;
            dump.count = 0;
            void* file = dump.file;
            if (nlDebugFileIsValid(dump.file))
            {
                nlWriteLineDebug(dump.file, sFreeMemoryDumpHeader, false);
            }
            else
            {
                nlPrintf(sFreeMemoryDumpHeader);
            }

            nlWalkDLRing(m_free_block_list, &dump, &FreePanicDumpCallback_802AF470::Callback);

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
            nlBreak();
        }
    }

    u32 remaining = blockSize - requestSize;
    alignment = 4;
    if (remaining > 0xC)
    {
        cur->m_size = remaining;
    }
    else
    {
        alignment = remaining + 4;
        nlDLRingRemove(&m_free_block_list, cur);
    }

    void* result = (u8*)(offset - alignment) + alignment;
    u32 header = size;
    if (alignment > 4)
    {
        header = size | 0x80000000;
        *(u32*)((u8*)result - 8) = alignment - 4;
    }
    u32 suffixGap = requestSize - alignedSize - 4;
    u8* blockEnd = (u8*)result + size;
    if (suffixGap != 0)
    {
        header |= 0x40000000;
        *(u32*)(((u32)blockEnd + 3) & ~3u) = suffixGap;
    }
    *(u32*)((u8*)result - 4) = header;
    m_04++;
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
    m_04 = 0;
    m_0C = memory;
    m_08 = size;
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

void MemoryStatsCallback_802AF2E4::Callback(FreeBlockList* block)
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
    MemoryStats_802AF2E4 stats;
    MemoryStatsCallback_802AF2E4 callback;
    callback.stats = &stats;
    stats.total = 0;
    stats.largest = 0;
    stats.count = 0;
    FreeBlockList* head = m_free_block_list;
    nlWalkDLRing(head, &callback, &MemoryStatsCallback_802AF2E4::Callback);
    return stats.total;
}

unsigned int MemoryAllocator::LargestFreeBlock()
{
    MemoryStats_802AF2E4 stats;
    MemoryStatsCallback_802AF2E4 callback;
    callback.stats = &stats;
    stats.total = 0;
    stats.largest = 0;
    stats.count = 0;
    FreeBlockList* head = m_free_block_list;
    nlWalkDLRing(head, &callback, &MemoryStatsCallback_802AF2E4::Callback);
    return stats.largest;
}

char sFreeMemoryDumpHeader[] = "count   address     size        allocNum    file(line)  description \n";
char sFreeMemoryDumpTotalFormat[] = "Total free memory: %d\n";

void FreePanicDumpCallback_802AF470::Callback(FreeBlockList* block)
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
