#ifndef GAME_DEBUGWRITECACHE_H
#define GAME_DEBUGWRITECACHE_H

#include "types.h"

class RunningChecksum;

typedef void (*DebugFieldWriter)(
    const void* value, void* context, char* buffer, unsigned long size);

// Indices into gDebugFieldTypes, named after each entry's writer.
enum DebugFieldTypeId
{
    DEBUG_FIELD_U8 = 0,
    DEBUG_FIELD_U16 = 1,
    DEBUG_FIELD_U32 = 2,
    DEBUG_FIELD_U64 = 3,
    DEBUG_FIELD_CHAR = 4,
    DEBUG_FIELD_S16 = 5,
    DEBUG_FIELD_S32 = 6,
    DEBUG_FIELD_S64 = 7,
    DEBUG_FIELD_INT = 8,
    DEBUG_FIELD_UNSIGNED_INT = 9,
    DEBUG_FIELD_SHORT = 10,
    DEBUG_FIELD_UNSIGNED_SHORT = 11,
    DEBUG_FIELD_LONG = 12,
    DEBUG_FIELD_UNSIGNED_LONG = 13,
    DEBUG_FIELD_ENUM = 14,
    DEBUG_FIELD_POINTER = 15,
    DEBUG_FIELD_BOOL = 16,
    DEBUG_FIELD_FLOAT = 17,
    DEBUG_FIELD_DOUBLE = 18,
    DEBUG_FIELD_ANGLE = 19,
    DEBUG_FIELD_TIMER = 20,
    DEBUG_FIELD_VECTOR2 = 21,
    DEBUG_FIELD_VECTOR3 = 22,
    DEBUG_FIELD_VECTOR4 = 23,
    DEBUG_FIELD_QUATERNION = 24,
    DEBUG_FIELD_MATRIX3 = 25,
    DEBUG_FIELD_MATRIX4 = 26,
    DEBUG_FIELD_ODE_VECTOR3 = 27,
    DEBUG_FIELD_ODE_VECTOR4 = 28,
    DEBUG_FIELD_ODE_QUATERNION = 29,
    DEBUG_FIELD_ODE_MATRIX3 = 30,
    DEBUG_FIELD_ODE_MATRIX4 = 31,
    DEBUG_FIELD_TYPE_COUNT = 32
};

// DebugWriteType::mKind values.
enum DebugWriteTypeKind
{
    DEBUG_KIND_COMPOSITE = 1,
    DEBUG_KIND_SCALAR = 2
};

// Reserved type ids.
enum DebugReservedTypeId
{
    DEBUG_TYPE_TEXT = 0xFFFE,
    DEBUG_TYPE_NONE = 0xFFFF
};

struct DebugFieldType
{
    /* 0x0 */ u16 size;
    /* 0x4 */ DebugFieldWriter writer;
}; // size: 0x8

struct DebugWriteField;

struct DebugWriteType
{
    void InitializeComposite(u16 type, const char* name);

    /* 0x00 */ u16 mType;
    /* 0x02 */ u16 mKind;
    /* 0x04 */ char mName[16];
    union
    {
        struct
        {
            /* 0x14 */ DebugWriteField* mLastField;
            /* 0x18 */ u16 mFieldCount;
            /* 0x1A */ u16 mPadding1A;
        } mComposite;
        struct
        {
            /* 0x14 */ u16 mSize;
            /* 0x16 */ u16 mCount;
            /* 0x18 */ u8 mFieldType;
            /* 0x19 */ u8 mPadding19[3];
        } mScalar;
    } mData;
}; // size: 0x1C

struct DebugWriteField
{
    /* 0x00 */ u16 mSize;
    /* 0x02 */ u16 mOffset;
    /* 0x04 */ DebugWriteField* mNext;
    /* 0x08 */ DebugWriteType* mOwner;
    /* 0x0C */ char mName[16];
    /* 0x1C */ u16 mCount;
    /* 0x1E */ u8 mFieldType;
    /* 0x1F */ u8 mPadding1F;
}; // size: 0x20

struct DebugWriteBuffer
{
    /* 0x00 */ int mFrame;
    /* 0x04 */ u32 mSize;
    /* 0x08 */ u8* mData;
    /* 0x0C */ u8* mCurrent;
}; // size: 0x10

class DebugWriteCache
{
public:
    void Reset();
    void WriteFloat(u16* type, const char* name, RunningChecksum* checksum, float value);
    u16 BeginType(const char* name);
    void EndType();
    void AddField(const char* name, int fieldType, unsigned int offset);
    void AddField(int fieldType, u16 size, unsigned int offset, const char* name);
    void AddArrayField(int fieldType, u16 size, unsigned int count, unsigned int offset, const char* name);
    void WriteText(const char* value);
    void* WriteData(u16 type, void* value, unsigned int size);
    void ChecksumData(u16 type, void* value, void* context);
    void BeginFrame(unsigned int frame);

    /* 0x00 */ u16 mTypeCount;
    /* 0x02 */ u16 mCurrentType;
    /* 0x04 */ int mTypeCapacity;
    /* 0x08 */ DebugWriteType* mTypes;
    /* 0x0C */ int mFieldCapacity;
    /* 0x10 */ int mFieldCount;
    /* 0x14 */ DebugWriteField* mFields;
    /* 0x18 */ int mBufferCount;
    /* 0x1C */ int mCurrentBuffer;
    /* 0x20 */ DebugWriteBuffer* mBuffers;
}; // size: 0x24

extern DebugFieldType gDebugFieldTypes[DEBUG_FIELD_TYPE_COUNT];

inline void DebugWriteCache::AddField(const char* name, int fieldType, unsigned int offset)
{
    AddField(fieldType, gDebugFieldTypes[fieldType].size, offset, name);
}

#endif // GAME_DEBUGWRITECACHE_H
