#include "Game/DebugWriteCache.h"

#include <string.h>

#include "NL/nlDebug.h"
#include "NL/nlMain.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlTimer.h"

struct UnidentifiedDebugWriteHeader
{
    /* 0x0 */ u16 mType;
    /* 0x2 */ u16 mSize;
    /* 0x4 */ u16 mPaddedSize;
    /* 0x6 */ u16 mMarker;
}; // size: 0x8

static inline u16 GetUnidentifiedPaddedSize(u16 size)
{
    u16 remainder = size % 4;
    if (remainder == 0)
    {
        return size;
    }
    return size + (4 - remainder);
}

static inline UnidentifiedDebugWriteBuffer* GetUnidentifiedCurrentBuffer(
    DebugWriteCache* cache)
{
    if (cache->mCurrentBuffer >= 0
        && cache->mCurrentBuffer < cache->mBufferCount)
    {
        return &cache->mBuffers[cache->mCurrentBuffer];
    }
    return 0;
}

extern "C" void fn_80338CC4(DebugWriteCache* cache)
{
    cache->mCurrentBuffer = -1;
    for (int i = 0; i < cache->mBufferCount; ++i)
    {
        UnidentifiedDebugWriteBuffer& buffer = cache->mBuffers[i];
        buffer.mCurrent = buffer.mData;
        buffer.mFrame = -1;
    }
}

extern "C" void fn_80338D04(DebugWriteCache* cache, u16* type,
    const char* name, RunningChecksum* checksum, float value)
{
    if (*type == 0xFFFF)
    {
        if (cache->mTypeCount >= cache->mTypeCapacity)
        {
            nlBreak();
        }

        u16 newType = cache->mTypeCount++;
        *type = newType;
        UnidentifiedDebugWriteType* entry = &cache->mTypes[newType];
        entry->mType = newType;
        entry->mKind = 2;
        nlStrNCpy(entry->mName, name, sizeof(entry->mName));
        entry->mData.mScalar.mFieldType = 17;
        entry->mData.mScalar.mSize = lbl_80533C98[17].size;
        entry->mData.mScalar.mCount = 0;
    }

    checksum->ChecksumData(&value, sizeof(value));

    UnidentifiedDebugWriteBuffer* buffer
        = GetUnidentifiedCurrentBuffer(cache);
    UnidentifiedDebugWriteHeader header;
    header.mType = *type;
    header.mSize = sizeof(value);
    header.mPaddedSize = sizeof(value);
    header.mMarker = 0xDADA;

    if (buffer->mCurrent + sizeof(header) + sizeof(value)
        < buffer->mData + buffer->mSize)
    {
        memcpy(buffer->mCurrent, &header, sizeof(header));
        buffer->mCurrent += sizeof(header);
        memcpy(buffer->mCurrent, &value, sizeof(value));
        buffer->mCurrent += sizeof(value);

        for (u16 i = header.mSize; i < header.mPaddedSize; ++i)
        {
            *buffer->mCurrent++ = 0;
        }
    }
}

extern "C" u16 fn_80338EBC(DebugWriteCache* cache, const char* name)
{
    if (cache->mTypeCount >= cache->mTypeCapacity)
    {
        nlBreak();
    }

    u16 type = cache->mTypeCount++;
    cache->mCurrentType = type;

    UnidentifiedDebugWriteType* entry = &cache->mTypes[type];
    entry->mKind = 1;
    entry->mType = type;
    nlStrNCpy(entry->mName, name, sizeof(entry->mName));
    entry->mData.mComposite.mLastField = 0;
    entry->mData.mComposite.mFieldCount = 0;
    return cache->mCurrentType;
}

extern "C" void fn_80338F78(DebugWriteCache* cache)
{
    cache->mCurrentType = 0xFFFF;
}

extern "C" void fn_80338F88(DebugWriteCache* cache, int fieldType, u16 size,
    u32 offset, const char* name)
{
    UnidentifiedDebugWriteType* owner
        = &cache->mTypes[cache->mCurrentType];
    UnidentifiedDebugWriteField* field;

    if (cache->mFieldCount >= cache->mFieldCapacity)
    {
        nlBreak();
        field = 0;
    }
    else
    {
        field = &cache->mFields[cache->mFieldCount++];
    }

    field->mSize = size;
    field->mOffset = offset;
    field->mNext = 0;
    field->mOwner = owner;
    nlStrNCpy(field->mName, name, sizeof(field->mName));
    field->mFieldType = fieldType;
    field->mCount = 0;

    if (owner->mData.mComposite.mLastField == 0)
    {
        owner->mData.mComposite.mLastField = field;
        field->mNext = field;
    }
    else
    {
        field->mNext = owner->mData.mComposite.mLastField->mNext;
        owner->mData.mComposite.mLastField->mNext = field;
    }
    owner->mData.mComposite.mLastField = field;
    ++owner->mData.mComposite.mFieldCount;
}

extern "C" void fn_80339090(DebugWriteCache* cache, int fieldType, u16 size,
    u32 count, u32 offset, const char* name)
{
    UnidentifiedDebugWriteType* owner
        = &cache->mTypes[cache->mCurrentType];
    UnidentifiedDebugWriteField* field;

    if (cache->mFieldCount >= cache->mFieldCapacity)
    {
        nlBreak();
        field = 0;
    }
    else
    {
        field = &cache->mFields[cache->mFieldCount++];
    }

    field->mSize = size;
    field->mOffset = offset;
    field->mNext = 0;
    field->mOwner = owner;
    nlStrNCpy(field->mName, name, sizeof(field->mName));
    field->mFieldType = fieldType;
    field->mCount = count;

    if (owner->mData.mComposite.mLastField == 0)
    {
        owner->mData.mComposite.mLastField = field;
        field->mNext = field;
    }
    else
    {
        field->mNext = owner->mData.mComposite.mLastField->mNext;
        owner->mData.mComposite.mLastField->mNext = field;
    }
    owner->mData.mComposite.mLastField = field;
    ++owner->mData.mComposite.mFieldCount;
}

extern "C" void fn_8033919C(DebugWriteCache* cache, const char* value)
{
    UnidentifiedDebugWriteBuffer* buffer
        = GetUnidentifiedCurrentBuffer(cache);
    if (buffer == 0)
    {
        return;
    }

    u16 size = nlStrLen(value) + 1;
    UnidentifiedDebugWriteHeader header;
    header.mType = 0xFFFE;
    header.mSize = size;
    header.mPaddedSize = GetUnidentifiedPaddedSize(size);
    header.mMarker = 0xDADA;

    if (buffer->mCurrent + sizeof(header) + size
        < buffer->mData + buffer->mSize)
    {
        memcpy(buffer->mCurrent, &header, sizeof(header));
        buffer->mCurrent += sizeof(header);
        memcpy(buffer->mCurrent, value, size);
        buffer->mCurrent += size;

        for (u16 i = header.mSize; i < header.mPaddedSize; ++i)
        {
            *buffer->mCurrent++ = 0;
        }
    }
}

extern "C" void* fn_8033930C(
    DebugWriteCache* cache, u16 type, void* value, u32 size)
{
    UnidentifiedDebugWriteBuffer* buffer
        = GetUnidentifiedCurrentBuffer(cache);
    UnidentifiedDebugWriteHeader header;
    header.mType = type;
    header.mSize = size;
    header.mPaddedSize = GetUnidentifiedPaddedSize(size);
    header.mMarker = 0xDADA;

    if (buffer->mCurrent + sizeof(header) + (u16)size
        >= buffer->mData + buffer->mSize)
    {
        return 0;
    }

    memcpy(buffer->mCurrent, &header, sizeof(header));
    buffer->mCurrent += sizeof(header);
    void* result = buffer->mCurrent;
    memcpy(buffer->mCurrent, value, (u16)size);
    buffer->mCurrent += (u16)size;

    for (u16 i = header.mSize; i < header.mPaddedSize; ++i)
    {
        *buffer->mCurrent++ = 0;
    }
    return result;
}

extern "C" void fn_80339450(DebugWriteCache* cache, u16 type,
    void* value, void* context)
{
    UnidentifiedDebugWriteType* entry = &cache->mTypes[type];
    if (entry->mKind == 1)
    {
        UnidentifiedDebugWriteField* field;
        if (entry->mData.mComposite.mLastField == 0)
        {
            field = 0;
        }
        else
        {
            field = entry->mData.mComposite.mLastField->mNext;
        }

        u16 count = entry->mData.mComposite.mFieldCount;
        while (count != 0)
        {
            u16 size = field->mSize;
            if (field->mCount != 0)
            {
                size *= field->mCount;
            }
            ((RunningChecksum*)context)->ChecksumData(
                (const u8*)value + field->mOffset, size);
            field = field->mNext;
            --count;
        }
    }
    else if (entry->mKind == 2)
    {
        u16 size = entry->mData.mScalar.mSize;
        if (entry->mData.mScalar.mCount != 0)
        {
            size *= entry->mData.mScalar.mCount;
        }
        ((RunningChecksum*)context)->ChecksumData(value, size);
    }
}

extern "C" void fn_80339544(DebugWriteCache* cache, u32 frame)
{
    cache->mCurrentBuffer
        = (cache->mCurrentBuffer + 1) % cache->mBufferCount;
    UnidentifiedDebugWriteBuffer* buffer
        = &cache->mBuffers[cache->mCurrentBuffer];
    buffer->mCurrent = buffer->mData;
    buffer->mFrame = frame;
}

extern "C" void fn_8033957C(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%u", *(const u8*)value);
}

extern "C" void fn_80339594(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%u", *(const u16*)value);
}

extern "C" void fn_803395AC(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%u", *(const u32*)value);
}

extern "C" void fn_803395C4(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%lu", *(const unsigned long long*)value);
}

extern "C" void fn_803395E4(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *(const u8*)value);
}

extern "C" void fn_803395FC(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *(const s16*)value);
}

extern "C" void fn_80339614(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *(const s32*)value);
}

extern "C" void fn_8033962C(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%ld", *(const long long*)value);
}

extern "C" void fn_8033964C(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *(const s32*)value);
}

extern "C" void fn_80339664(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%u", *(const u32*)value);
}

extern "C" void fn_8033967C(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *(const s16*)value);
}

extern "C" void fn_80339694(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%u", *(const u16*)value);
}

extern "C" void fn_803396AC(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%ld", *(const s32*)value);
}

extern "C" void fn_803396C4(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%lu", *(const u32*)value);
}

extern "C" void fn_803396DC(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *(const s32*)value);
}

extern "C" void fn_803396F4(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "pointer converted to index 0x%x",
        *(const u32*)value);
}

extern "C" void fn_80339710(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *(const u8*)value);
}

extern "C" void fn_80339728(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%f (%x)", *(const float*)value,
        *(const u32*)value);
}

extern "C" void fn_80339748(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%f (%lx)", *(const double*)value,
        *(const unsigned long long*)value);
}

extern "C" void fn_80339770(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%x", *(const u16*)value);
}

extern "C" void fn_80339788(
    const void* value, void*, char* buffer, unsigned long size)
{
    float seconds = ((const Timer*)value)->GetSeconds();
    nlSNPrintf(buffer, size, "%f (%x)", seconds, *(u32*)&seconds);
}

extern "C" void fn_803397E0(
    const void* value, void*, char* buffer, unsigned long size)
{
    const float* values = (const float*)value;
    const u32* bits = (const u32*)value;
    nlSNPrintf(buffer, size, "(%f %f) (%x %x)", values[0], values[1],
        bits[0], bits[1]);
}

extern "C" void fn_8033980C(
    const void* value, void*, char* buffer, unsigned long size)
{
    const float* values = (const float*)value;
    const u32* bits = (const u32*)value;
    nlSNPrintf(buffer, size, "(%f %f %f) (%x %x %x)", values[0],
        values[1], values[2], bits[0], bits[1], bits[2]);
}

extern "C" void fn_80339840(
    const void* value, void*, char* buffer, unsigned long size)
{
    const float* values = (const float*)value;
    const u32* bits = (const u32*)value;
    nlSNPrintf(buffer, size, "(%f %f %f %f) (%x %x %x %x)", values[0],
        values[1], values[2], values[3], bits[0], bits[1], bits[2], bits[3]);
}

extern "C" void fn_8033987C(
    const void* value, void*, char* buffer, unsigned long size)
{
    const float* values = (const float*)value;
    const u32* bits = (const u32*)value;
    nlSNPrintf(buffer, size, "(%f %f %f %f) (%x %x %x %x)", values[0],
        values[1], values[2], values[3], bits[0], bits[1], bits[2], bits[3]);
}

static inline void WriteUnidentifiedFloatArray(
    const void* value, char* buffer, unsigned long size, int count)
{
    buffer[0] = '\0';
    int i = 0;
    const float* values = (const float*)value;
    for (; i < count; ++i)
    {
        char element[100];
        nlSNPrintf(element, sizeof(element), "%f (%x) ", *values,
            *(const u32*)values);
        nlStrNCat(buffer, buffer, element, size);
        ++values;
    }
}

extern "C" void fn_803398B8(
    const void* value, void*, char* buffer, unsigned long size)
{
    WriteUnidentifiedFloatArray(value, buffer, size, 9);
}

extern "C" void fn_80339940(
    const void* value, void*, char* buffer, unsigned long size)
{
    WriteUnidentifiedFloatArray(value, buffer, size, 16);
}

extern "C" void fn_803399C8(
    const void* value, void*, char* buffer, unsigned long size)
{
    const float* values = (const float*)value;
    const u32* bits = (const u32*)value;
    nlSNPrintf(buffer, size, "(%f %f %f) (%x %x %x)", values[0],
        values[1], values[2], bits[0], bits[1], bits[2]);
}

extern "C" void fn_803399FC(
    const void* value, void*, char* buffer, unsigned long size)
{
    const float* values = (const float*)value;
    const u32* bits = (const u32*)value;
    nlSNPrintf(buffer, size, "(%f %f %f %f) (%x %x %x %x)", values[0],
        values[1], values[2], values[3], bits[0], bits[1], bits[2], bits[3]);
}

extern "C" void fn_80339A38(
    const void* value, void*, char* buffer, unsigned long size)
{
    const float* values = (const float*)value;
    const u32* bits = (const u32*)value;
    nlSNPrintf(buffer, size, "(%f %f %f %f) (%x %x %x %x)", values[0],
        values[1], values[2], values[3], bits[0], bits[1], bits[2], bits[3]);
}

extern "C" void fn_80339A74(
    const void* value, void*, char* buffer, unsigned long size)
{
    WriteUnidentifiedFloatArray(value, buffer, size, 12);
}

extern "C" void fn_80339AFC(
    const void* value, void*, char* buffer, unsigned long size)
{
    WriteUnidentifiedFloatArray(value, buffer, size, 16);
}

extern "C" DebugFieldType lbl_80533C98[32] = {
    { 1, 0, fn_8033957C },
    { 2, 0, fn_80339594 },
    { 4, 0, fn_803395AC },
    { 8, 0, fn_803395C4 },
    { 1, 0, fn_803395E4 },
    { 2, 0, fn_803395FC },
    { 4, 0, fn_80339614 },
    { 8, 0, fn_8033962C },
    { 4, 0, fn_8033964C },
    { 4, 0, fn_80339664 },
    { 2, 0, fn_8033967C },
    { 2, 0, fn_80339694 },
    { 4, 0, fn_803396AC },
    { 4, 0, fn_803396C4 },
    { 4, 0, fn_803396DC },
    { 4, 0, fn_803396F4 },
    { 1, 0, fn_80339710 },
    { 4, 0, fn_80339728 },
    { 8, 0, fn_80339748 },
    { 2, 0, fn_80339770 },
    { 8, 0, fn_80339788 },
    { 8, 0, fn_803397E0 },
    { 12, 0, fn_8033980C },
    { 16, 0, fn_80339840 },
    { 16, 0, fn_8033987C },
    { 36, 0, fn_803398B8 },
    { 64, 0, fn_80339940 },
    { 12, 0, fn_803399C8 },
    { 16, 0, fn_803399FC },
    { 16, 0, fn_80339A38 },
    { 48, 0, fn_80339A74 },
    { 64, 0, fn_80339AFC },
};
