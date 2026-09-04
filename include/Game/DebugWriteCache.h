#ifndef GAME_DEBUGWRITECACHE_H
#define GAME_DEBUGWRITECACHE_H

#include "types.h"

class RunningChecksum;

typedef void (*DebugFieldWriter)(
    const void* value, void* context, char* buffer, unsigned long size);

struct DebugFieldType
{
    /* 0x0 */ u16 size;
    /* 0x2 */ u16 unknown;
    /* 0x4 */ DebugFieldWriter writer;
}; // size: 0x8

struct UnidentifiedDebugWriteField;

struct UnidentifiedDebugWriteType
{
    /* 0x00 */ u16 mType;
    /* 0x02 */ u16 mKind;
    /* 0x04 */ char mName[16];
    union
    {
        struct
        {
            /* 0x14 */ UnidentifiedDebugWriteField* mLastField;
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

struct UnidentifiedDebugWriteField
{
    /* 0x00 */ u16 mSize;
    /* 0x02 */ u16 mOffset;
    /* 0x04 */ UnidentifiedDebugWriteField* mNext;
    /* 0x08 */ UnidentifiedDebugWriteType* mOwner;
    /* 0x0C */ char mName[16];
    /* 0x1C */ u16 mCount;
    /* 0x1E */ u8 mFieldType;
    /* 0x1F */ u8 mPadding1F;
}; // size: 0x20

struct UnidentifiedDebugWriteBuffer
{
    /* 0x00 */ int mFrame;
    /* 0x04 */ u32 mSize;
    /* 0x08 */ u8* mData;
    /* 0x0C */ u8* mCurrent;
}; // size: 0x10

class DebugWriteCache
{
public:
    /* 0x00 */ u16 mTypeCount;
    /* 0x02 */ u16 mCurrentType;
    /* 0x04 */ int mTypeCapacity;
    /* 0x08 */ UnidentifiedDebugWriteType* mTypes;
    /* 0x0C */ int mFieldCapacity;
    /* 0x10 */ int mFieldCount;
    /* 0x14 */ UnidentifiedDebugWriteField* mFields;
    /* 0x18 */ int mBufferCount;
    /* 0x1C */ int mCurrentBuffer;
    /* 0x20 */ UnidentifiedDebugWriteBuffer* mBuffers;
}; // size: 0x24

extern "C" DebugFieldType lbl_80533C98[32];

extern "C" void fn_80338CC4(DebugWriteCache* cache);
extern "C" void fn_80338D04(DebugWriteCache* cache, u16* type,
    const char* name, RunningChecksum* checksum, float value);
extern "C" u16 fn_80338EBC(DebugWriteCache* cache, const char* name);
extern "C" void fn_80338F78(DebugWriteCache* cache);
extern "C" void fn_80338F88(DebugWriteCache* cache, int fieldType, u16 size,
    u32 offset, const char* name);
extern "C" void fn_80339090(DebugWriteCache* cache, int fieldType, u16 size,
    u32 count, u32 offset, const char* name);
extern "C" void fn_8033919C(DebugWriteCache* cache, const char* value);
extern "C" void* fn_8033930C(
    DebugWriteCache* cache, u16 type, void* value, u32 size);
extern "C" void fn_80339450(DebugWriteCache* cache, u16 type,
    void* value, void* context);
extern "C" void fn_80339544(DebugWriteCache* cache, u32 frame);

#endif // GAME_DEBUGWRITECACHE_H
