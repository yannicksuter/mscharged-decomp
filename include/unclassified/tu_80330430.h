#ifndef UNCLASSIFIED_TU_80330430_H
#define UNCLASSIFIED_TU_80330430_H

#include "Game/NetworkMessages.h"

class NetworkMessageType2_805333DC : public UnidentifiedNetworkMessage
{
public:
    NetworkMessageType2_805333DC();
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType2_805333DC();
    virtual int GetType();

    /* 0x08 */ u8 mUnidentified08[8];
}; // size: 0x10

class NetworkMessageType3_805333C8 : public UnidentifiedNetworkMessage
{
public:
    NetworkMessageType3_805333C8();
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType3_805333C8();
    virtual int GetType();

    /* 0x08 */ u8 mUnidentified08[0x20];
}; // size: 0x28

class NetworkMessageType4_80533468 : public UnidentifiedNetworkMessage
{
public:
    NetworkMessageType4_80533468();
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType4_80533468();
    virtual int GetType();

    /* 0x08 */ u8 mUnidentified08[0x1C];
}; // size: 0x24

class NetworkMessageType5_80533454 : public UnidentifiedNetworkMessage
{
public:
    NetworkMessageType5_80533454();
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType5_80533454();
    virtual int GetType();

    /* 0x08 */ u8 mUnidentified08[0xE0];
}; // size: 0xE8

class NetworkMessageType6_8053342C : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType6_8053342C();
    virtual int GetType();
};

class NetworkMessageType7_80533440 : public UnidentifiedNetworkMessage
{
public:
    NetworkMessageType7_80533440();
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType7_80533440();
    virtual int GetType();

    /* 0x08 */ u8 mUnidentified08[0x1C];
}; // size: 0x24

class NetworkMessageType10_80533418 : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType10_80533418();
    virtual int GetType();
};

class NetworkMessageType11_80533404 : public UnidentifiedNetworkMessage
{
public:
    NetworkMessageType11_80533404()
        : mUnidentified08(0)
    {
    }

    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType11_80533404();
    virtual int GetType();

    /* 0x08 */ u8 mUnidentified08;
};

class NetworkMessageType12_805333F0 : public UnidentifiedNetworkMessage
{
public:
    NetworkMessageType12_805333F0()
        : mUnidentified08(-1)
    {
    }

    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType12_805333F0();
    virtual int GetType();

    /* 0x08 */ s8 mUnidentified08;
};

#endif // UNCLASSIFIED_TU_80330430_H
