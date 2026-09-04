#ifndef UNCLASSIFIED_TU_80332770_H
#define UNCLASSIFIED_TU_80332770_H

#include "Game/NetworkMessages.h"
#include "Game/Task/FixedUpdateTaskBase.h"
#include "NL/nlSlotPool.h"
#include "types.h"

extern SlotPoolBase lbl_805848E8;

// Variable-length deterministic data packet. The first byte is the payload
// size; the remaining 32 bytes are copied to and from network messages.
struct DetermDataEvent
{
    DetermDataEvent();
    DetermDataEvent(const void* data, u32 size);
    ~DetermDataEvent();
    void operator delete(void* data)
    {
        SlotPoolEntry* entry = (SlotPoolEntry*)data;
        entry->next = lbl_805848E8.m_FreeList;
        lbl_805848E8.m_FreeList = entry;
    }

    /* 0x00 */ u8 mSize;
    /* 0x01 */ u8 mPadding01[3];
    /* 0x04 */ u8 mData[0x20];
}; // size: 0x24

struct NetworkMessageInputRecord
{
    NetworkMessageInputRecord()
    {
        memset(mData, 0, sizeof(mData));
    }

    u8 mData[0x10];
};

class NetworkMessageType0_80533B7C : public UnidentifiedNetworkMessage
{
public:
    NetworkMessageType0_80533B7C();

    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType0_80533B7C();
    virtual int GetType();

    /* 0x008 */ u8 mUnidentified008;
    /* 0x009 */ u8 mUnidentified009;
    /* 0x00A */ u8 mUnidentified00A;
    /* 0x00B */ u8 mPadding00B;
    /* 0x00C */ u32 mUnidentified00C;
    /* 0x010 */ u32 mUnidentified010;
    /* 0x014 */ u32 mUnidentified014;
    /* 0x018 */ u8 mUnidentified018[4];
    /* 0x01C */ NetworkMessageInputRecord mUnidentified01C[4];
    /* 0x05C */ u8 mUnidentified05C;
    /* 0x05D */ DetermDataEvent mDetermData[4];
    /* 0x0ED */ u8 mPadding0ED[3];
}; // size: 0xF0

class NetworkMessageType1_80533B68 : public UnidentifiedNetworkMessage
{
public:
    NetworkMessageType1_80533B68() { }

    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType1_80533B68();
    virtual int GetType();

    /* 0x008 */ NetworkMessageType0_80533B7C mMessage0;
    /* 0x0F8 */ NetworkMessageType0_80533B7C mMessage1;
}; // size: 0x1E8

class NetworkMessageType8_80533BA4 : public UnidentifiedNetworkMessage
{
public:
    NetworkMessageType8_80533BA4();

    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType8_80533BA4();
    virtual int GetType();

    /* 0x008 */ u8 mUnidentified008[4];
    /* 0x00C */ NetworkMessageType0_80533B7C mMessages[4];
    /* 0x3CC */ u8 mPadding3CC[0xC];
}; // size: 0x3D8

class NetworkMessageType9_80533B90 : public UnidentifiedNetworkMessage
{
public:
    NetworkMessageType9_80533B90() { }

    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType9_80533B90();
    virtual int GetType();

    /* 0x008 */ NetworkMessageType8_80533BA4 mMessage0;
    /* 0x3E0 */ NetworkMessageType8_80533BA4 mMessage1;
}; // size: 0x7B8

class UnidentifiedNetworkManager
{
public:
    void fn_8033288C();
    void fn_803328AC(UnidentifiedFixedUpdateTaskBase* frameProvider);
    int fn_803328B4();
    void fn_803328FC();
    bool fn_80332A00();

    /* 0x00 */ UnidentifiedFixedUpdateTaskBase* mFrameProvider;
    /* 0x04 */ bool mEnabled;
    /* 0x05 */ u8 mPadding05[3];
    /* 0x08 */ u32 mUnidentified08;
    /* 0x0C */ float mUnidentified0C;
    /* 0x10 */ int mUnidentified10;
    /* 0x14 */ float mUnidentified14;
    /* 0x18 */ bool mUnidentified18;
    /* 0x19 */ u8 mPadding19[3];
}; // size: 0x1C

extern UnidentifiedNetworkManager* lbl_806E2138;

extern "C" bool fn_80332770();
extern "C" void fn_803327DC();

#endif // UNCLASSIFIED_TU_80332770_H
