#include "Game/DebugWriteCache.h"

#include <string.h>

#include "NL/nlDebug.h"
#include "NL/nlMain.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlTimer.h"
#include "NL/nlstring_tmpl.h"

struct DebugWriteRecordHeader
{
    /* 0x0 */ u16 mType;
    /* 0x2 */ u16 mSize;
    /* 0x4 */ u16 mPaddedSize;
    /* 0x6 */ u16 mMarker;
}; // size: 0x8

enum DebugRecordMarker
{
    DEBUG_RECORD_MARKER = 0xDADA
};

static inline DebugWriteBuffer* GetCurrentDebugBuffer(
    DebugWriteCache* cache)
{
    if (cache->mCurrentBuffer >= 0
        && cache->mCurrentBuffer < cache->mBufferCount)
    {
        return &cache->mBuffers[cache->mCurrentBuffer];
    }
    return 0;
}

static inline DebugWriteField* AllocateDebugField(
    DebugWriteCache* cache)
{
    if (cache->mFieldCount >= cache->mFieldCapacity)
    {
        nlBreak();
        return 0;
    }
    DebugWriteField* field = &cache->mFields[cache->mFieldCount];
    cache->mFieldCount++;
    return field;
}

static inline void InitializeDebugField(
    DebugWriteField* field, DebugWriteType* owner, int fieldType,
    u16 size, unsigned int offset, const char* name, unsigned int count)
{
    field->mSize = size;
    field->mOffset = offset;
    field->mNext = 0;
    field->mOwner = owner;
    nlStrNCpy(field->mName, name, sizeof(field->mName));
    field->mFieldType = fieldType;
    field->mCount = count;
}

static inline void ResetDebugBuffer(
    DebugWriteBuffer* buffer, int frame)
{
    buffer->mCurrent = buffer->mData;
    buffer->mFrame = frame;
}

void DebugWriteCache::Reset()
{
    mCurrentBuffer = -1;
    for (int i = 0; i < mBufferCount; ++i)
    {
        ResetDebugBuffer(&mBuffers[i], -1);
    }
}

static inline void WriteFloatRecord(DebugWriteCache* cache,
    u16 recordType, const float* value)
{
    DebugWriteBuffer* buffer
        = GetCurrentDebugBuffer(cache);
    DebugWriteRecordHeader header;
    header.mType = recordType;
    header.mSize = sizeof(*value);
    header.mMarker = DEBUG_RECORD_MARKER;
    u16 dataSize = header.mSize;
    u16 remainder = dataSize % 4;
    if (remainder == 0)
    {
        header.mPaddedSize = dataSize;
    }
    else
    {
        header.mPaddedSize = dataSize + (4 - remainder);
    }

    if (buffer->mCurrent + sizeof(header) + dataSize
        < buffer->mData + buffer->mSize)
    {
        memcpy(buffer->mCurrent, &header, sizeof(header));
        buffer->mCurrent += sizeof(header);
        memcpy(buffer->mCurrent, value, dataSize);
        buffer->mCurrent += dataSize;

        for (int i = header.mSize; i < header.mPaddedSize; ++i)
        {
            *buffer->mCurrent++ = 0;
        }
    }
}

void DebugWriteCache::WriteFloat(u16* type,
    const char* name, RunningChecksum* checksum, float value)
{
    if (*type == DEBUG_TYPE_NONE)
    {
        u16 fieldSize = gDebugFieldTypes[DEBUG_FIELD_FLOAT].size;
        if (mTypeCount >= mTypeCapacity)
        {
            nlBreak();
        }

        u16 newType = mTypeCount++;
        *type = newType;
        DebugWriteType* entry = &mTypes[newType];
        entry->mKind = DEBUG_KIND_SCALAR;
        entry->mType = newType;
        nlStrNCpy(entry->mName, name, sizeof(entry->mName));
        entry->mData.mScalar.mFieldType = DEBUG_FIELD_FLOAT;
        entry->mData.mScalar.mSize = fieldSize;
        entry->mData.mScalar.mCount = 0;
    }

    checksum->ChecksumData(&value, sizeof(value));

    WriteFloatRecord(this, *type, &value);
}

inline void DebugWriteType::InitializeComposite(u16 type, const char* name)
{
    mKind = DEBUG_KIND_COMPOSITE;
    mType = type;
    nlStrNCpy(mName, name, sizeof(mName));
    mData.mComposite.mLastField = 0;
    mData.mComposite.mFieldCount = 0;
}

u16 DebugWriteCache::BeginType(const char* name)
{
    if (mTypeCount >= mTypeCapacity)
    {
        nlBreak();
    }

    u16 type = mTypeCount++;
    mCurrentType = type;

    DebugWriteType* entry = &mTypes[type];
    entry->InitializeComposite(type, name);
    return mCurrentType;
}

void DebugWriteCache::EndType()
{
    mCurrentType = DEBUG_TYPE_NONE;
}

void DebugWriteCache::AddField(int fieldType, u16 size,
    unsigned int offset, const char* name)
{
    DebugWriteType* owner
        = &mTypes[mCurrentType];
    DebugWriteField* field = AllocateDebugField(this);

    InitializeDebugField(field, owner, fieldType, size, offset, name, 0);

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

void DebugWriteCache::AddArrayField(int fieldType, u16 size,
    unsigned int count, unsigned int offset, const char* name)
{
    DebugWriteType* owner
        = &mTypes[mCurrentType];
    DebugWriteField* field = AllocateDebugField(this);

    InitializeDebugField(field, owner, fieldType, size, offset, name, count);

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

void DebugWriteCache::WriteText(const char* value)
{
    DebugWriteBuffer* buffer
        = GetCurrentDebugBuffer(this);
    if (buffer == 0)
    {
        return;
    }

    unsigned long length = nlStrLen(value);
    DebugWriteRecordHeader header;
    header.mType = DEBUG_TYPE_TEXT;
    header.mSize = length + 1;
    header.mMarker = DEBUG_RECORD_MARKER;
    u16 dataSize = length + 1;
    u16 remainder = dataSize % 4;
    if (remainder == 0)
    {
        header.mPaddedSize = dataSize;
    }
    else
    {
        header.mPaddedSize = dataSize + (4 - remainder);
    }

    if (buffer->mCurrent + sizeof(header) + (u16)(length + 1)
        < buffer->mData + buffer->mSize)
    {
        memcpy(buffer->mCurrent, &header, sizeof(header));
        buffer->mCurrent += sizeof(header);
        memcpy(buffer->mCurrent, value, (u16)(length + 1));
        buffer->mCurrent += (u16)(length + 1);

        for (int i = header.mSize; i < header.mPaddedSize; ++i)
        {
            *buffer->mCurrent++ = 0;
        }
    }
}

void* DebugWriteCache::WriteData(u16 type, void* value, unsigned int size)
{
    DebugWriteBuffer* buffer
        = GetCurrentDebugBuffer(this);
    DebugWriteRecordHeader header;
    header.mType = type;
    header.mSize = size;
    header.mMarker = DEBUG_RECORD_MARKER;
    u16 dataSize = size;
    u16 remainder = dataSize % 4;
    if (remainder == 0)
    {
        header.mPaddedSize = dataSize;
    }
    else
    {
        header.mPaddedSize = dataSize + (4 - remainder);
    }

    void* result;
    if (buffer->mCurrent + sizeof(header) + (u16)size
        >= buffer->mData + buffer->mSize)
    {
        result = 0;
    }
    else
    {
        memcpy(buffer->mCurrent, &header, sizeof(header));
        buffer->mCurrent += sizeof(header);
        result = buffer->mCurrent;
        memcpy(buffer->mCurrent, value, (u16)size);
        buffer->mCurrent += (u16)size;

        for (int i = header.mSize; i < header.mPaddedSize; ++i)
        {
            *buffer->mCurrent++ = 0;
        }
    }
    return result;
}

void DebugWriteCache::ChecksumData(u16 type,
    void* value, void* context)
{
    DebugWriteType* entry = &mTypes[type];
    if (entry->mKind == DEBUG_KIND_COMPOSITE)
    {
        u16 count = entry->mData.mComposite.mFieldCount;
        DebugWriteField* field;
        if (entry->mData.mComposite.mLastField == 0)
        {
            field = 0;
        }
        else
        {
            field = entry->mData.mComposite.mLastField->mNext;
        }

        while (count != 0)
        {
            u16 size = field->mCount == 0
                ? field->mSize
                : field->mSize * field->mCount;
            ((RunningChecksum*)context)->ChecksumData(
                (const u8*)value + field->mOffset, size);
            field = field->mNext;
            --count;
        }
    }
    else if (entry->mKind == DEBUG_KIND_SCALAR)
    {
        u16 size = entry->mData.mScalar.mCount == 0
            ? entry->mData.mScalar.mSize
            : entry->mData.mScalar.mSize * entry->mData.mScalar.mCount;
        ((RunningChecksum*)context)->ChecksumData(value, size);
    }
}

void DebugWriteCache::BeginFrame(unsigned int frame)
{
    mCurrentBuffer
        = (mCurrentBuffer + 1) % mBufferCount;
    ResetDebugBuffer(&mBuffers[mCurrentBuffer], frame);
}

void WriteDebugU8(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%u", *(const u8*)value);
}

void WriteDebugU16(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%u", *(const u16*)value);
}

void WriteDebugU32(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%u", *(const u32*)value);
}

void WriteDebugU64(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%lu", *(const unsigned long long*)value);
}

void WriteDebugChar(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *(const u8*)value);
}

void WriteDebugS16(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *(const s16*)value);
}

void WriteDebugS32(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *(const s32*)value);
}

void WriteDebugS64(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%ld", *(const long long*)value);
}

void WriteDebugInt(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *(const s32*)value);
}

void WriteDebugUnsignedInt(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%u", *(const u32*)value);
}

void WriteDebugShort(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *(const s16*)value);
}

void WriteDebugUnsignedShort(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%u", *(const u16*)value);
}

void WriteDebugLong(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%ld", *(const s32*)value);
}

void WriteDebugUnsignedLong(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%lu", *(const u32*)value);
}

void WriteDebugEnum(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *(const s32*)value);
}

void WriteDebugPointer(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "pointer converted to index 0x%x",
        *(const u32*)value);
}

void WriteDebugBool(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *(const u8*)value);
}

void WriteDebugFloat(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%f (%x)", *(const float*)value,
        *(const u32*)value);
}

void WriteDebugDouble(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%f (%lx)", *(const double*)value,
        *(const unsigned long long*)value);
}

void WriteDebugAngle(
    const void* value, void*, char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%x", *(const u16*)value);
}

void WriteDebugTimer(
    const void* value, void*, char* buffer, unsigned long size)
{
    float seconds = ((const Timer*)value)->GetSeconds();
    nlSNPrintf(buffer, size, "%f (%x)", seconds, *(u32*)&seconds);
}

void WriteDebugVector2(
    const void* value, void*, char* buffer, unsigned long size)
{
    const float* values = (const float*)value;
    const u32* bits = (const u32*)value;
    nlSNPrintf(buffer, size, "(%f %f) (%x %x)", values[0], values[1],
        bits[0], bits[1]);
}

void WriteDebugVector3(
    const void* value, void*, char* buffer, unsigned long size)
{
    const float* values = (const float*)value;
    const u32* bits = (const u32*)value;
    nlSNPrintf(buffer, size, "(%f %f %f) (%x %x %x)", values[0],
        values[1], values[2], bits[0], bits[1], bits[2]);
}

void WriteDebugVector4(
    const void* value, void*, char* buffer, unsigned long size)
{
    const float* values = (const float*)value;
    const u32* bits = (const u32*)value;
    nlSNPrintf(buffer, size, "(%f %f %f %f) (%x %x %x %x)", values[0],
        values[1], values[2], values[3], bits[0], bits[1], bits[2], bits[3]);
}

void WriteDebugQuaternion(
    const void* value, void*, char* buffer, unsigned long size)
{
    const float* values = (const float*)value;
    const u32* bits = (const u32*)value;
    nlSNPrintf(buffer, size, "(%f %f %f %f) (%x %x %x %x)", values[0],
        values[1], values[2], values[3], bits[0], bits[1], bits[2], bits[3]);
}

static inline void WriteDebugFloatArray(
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

void WriteDebugMatrix3(
    const void* value, void*, char* buffer, unsigned long size)
{
    WriteDebugFloatArray(value, buffer, size, 9);
}

void WriteDebugMatrix4(
    const void* value, void*, char* buffer, unsigned long size)
{
    WriteDebugFloatArray(value, buffer, size, 16);
}

void WriteDebugODEVector3(
    const void* value, void*, char* buffer, unsigned long size)
{
    const float* values = (const float*)value;
    const u32* bits = (const u32*)value;
    nlSNPrintf(buffer, size, "(%f %f %f) (%x %x %x)", values[0],
        values[1], values[2], bits[0], bits[1], bits[2]);
}

void WriteDebugODEVector4(
    const void* value, void*, char* buffer, unsigned long size)
{
    const float* values = (const float*)value;
    const u32* bits = (const u32*)value;
    nlSNPrintf(buffer, size, "(%f %f %f %f) (%x %x %x %x)", values[0],
        values[1], values[2], values[3], bits[0], bits[1], bits[2], bits[3]);
}

void WriteDebugODEQuaternion(
    const void* value, void*, char* buffer, unsigned long size)
{
    const float* values = (const float*)value;
    const u32* bits = (const u32*)value;
    nlSNPrintf(buffer, size, "(%f %f %f %f) (%x %x %x %x)", values[0],
        values[1], values[2], values[3], bits[0], bits[1], bits[2], bits[3]);
}

void WriteDebugODEMatrix3(
    const void* value, void*, char* buffer, unsigned long size)
{
    WriteDebugFloatArray(value, buffer, size, 12);
}

void WriteDebugODEMatrix4(
    const void* value, void*, char* buffer, unsigned long size)
{
    WriteDebugFloatArray(value, buffer, size, 16);
}

DebugFieldType gDebugFieldTypes[DEBUG_FIELD_TYPE_COUNT] = {
    { 1, WriteDebugU8 },
    { 2, WriteDebugU16 },
    { 4, WriteDebugU32 },
    { 8, WriteDebugU64 },
    { 1, WriteDebugChar },
    { 2, WriteDebugS16 },
    { 4, WriteDebugS32 },
    { 8, WriteDebugS64 },
    { 4, WriteDebugInt },
    { 4, WriteDebugUnsignedInt },
    { 2, WriteDebugShort },
    { 2, WriteDebugUnsignedShort },
    { 4, WriteDebugLong },
    { 4, WriteDebugUnsignedLong },
    { 4, WriteDebugEnum },
    { 4, WriteDebugPointer },
    { 1, WriteDebugBool },
    { 4, WriteDebugFloat },
    { 8, WriteDebugDouble },
    { 2, WriteDebugAngle },
    { 8, WriteDebugTimer },
    { 8, WriteDebugVector2 },
    { 12, WriteDebugVector3 },
    { 16, WriteDebugVector4 },
    { 16, WriteDebugQuaternion },
    { 36, WriteDebugMatrix3 },
    { 64, WriteDebugMatrix4 },
    { 12, WriteDebugODEVector3 },
    { 16, WriteDebugODEVector4 },
    { 16, WriteDebugODEQuaternion },
    { 48, WriteDebugODEMatrix3 },
    { 64, WriteDebugODEMatrix4 },
};
