#include "unclassified/tu_80330430.h"

NetworkMessageType4_80533468::NetworkMessageType4_80533468()
    : mUnidentified19(0)
{
}

void NetworkMessageType4_80533468::Serialize(UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(mUnidentified08, sizeof(mUnidentified08));
    serializer->Transfer(&mUnidentified0C, sizeof(mUnidentified0C));
    serializer->Transfer(mUnidentified0E, sizeof(mUnidentified0E));
    serializer->Transfer(&mUnidentified19, sizeof(mUnidentified19));
    serializer->Transfer(mUnidentified1A, mUnidentified19);
}

NetworkMessageType5_80533454::NetworkMessageType5_80533454()
    : mUnidentified1A(0)
{
}

void NetworkMessageType5_80533454::Serialize(UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(mUnidentified08, sizeof(mUnidentified08));
    serializer->Transfer(&mUnidentified0C, sizeof(mUnidentified0C));
    serializer->Transfer(&mUnidentified0E, sizeof(mUnidentified0E));
    serializer->Transfer(mUnidentified0F, sizeof(mUnidentified0F));
    serializer->Transfer(&mUnidentified1A, sizeof(mUnidentified1A));
    serializer->Transfer(mUnidentified1B, mUnidentified1A);
    serializer->Transfer(&mUnidentified23, sizeof(mUnidentified23));
    for (u8 index = 0; index < mUnidentified23; ++index)
    {
        UnidentifiedNetworkPeerInfo_80330430& info = mUnidentified24[index];
        serializer->Transfer(info.mUnidentified00, sizeof(info.mUnidentified00));
        serializer->Transfer(&info.mUnidentified04, sizeof(info.mUnidentified04));
        serializer->Transfer(info.mUnidentified06, sizeof(info.mUnidentified06));
        serializer->Transfer(&info.mUnidentified11, sizeof(info.mUnidentified11));
        serializer->Transfer(&info.mUnidentified12, sizeof(info.mUnidentified12));
        serializer->Transfer(info.mUnidentified13, info.mUnidentified12);
    }
}

NetworkMessageType7_80533440::NetworkMessageType7_80533440()
{
}

void NetworkMessageType7_80533440::Serialize(UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(mUnidentified08.mUnidentified00, sizeof(mUnidentified08.mUnidentified00));
    serializer->Transfer(&mUnidentified08.mUnidentified04, sizeof(mUnidentified08.mUnidentified04));
    serializer->Transfer(mUnidentified08.mUnidentified06, sizeof(mUnidentified08.mUnidentified06));
    serializer->Transfer(&mUnidentified08.mUnidentified11, sizeof(mUnidentified08.mUnidentified11));
    serializer->Transfer(&mUnidentified08.mUnidentified12, sizeof(mUnidentified08.mUnidentified12));
    serializer->Transfer(mUnidentified08.mUnidentified13, mUnidentified08.mUnidentified12);
}

void NetworkMessageType6_8053342C::Serialize(UnidentifiedMessageSerializer* serializer)
{
}

void NetworkMessageType10_80533418::Serialize(UnidentifiedMessageSerializer* serializer)
{
}

void NetworkMessageType11_80533404::Serialize(UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
}

void NetworkMessageType12_805333F0::Serialize(UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
}

NetworkMessageType6_8053342C::~NetworkMessageType6_8053342C()
{
}

int NetworkMessageType12_805333F0::GetType()
{
    return 12;
}

int NetworkMessageType11_80533404::GetType()
{
    return 11;
}

int NetworkMessageType10_80533418::GetType()
{
    return 10;
}

int NetworkMessageType6_8053342C::GetType()
{
    return 6;
}

int NetworkMessageType7_80533440::GetType()
{
    return 7;
}

int NetworkMessageType5_80533454::GetType()
{
    return 5;
}

int NetworkMessageType4_80533468::GetType()
{
    return 4;
}
