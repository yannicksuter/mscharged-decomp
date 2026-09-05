#include "unclassified/tu_80330430.h"

NetworkMessageType2_805333DC::NetworkMessageType2_805333DC()
{
}

void NetworkMessageType2_805333DC::Serialize(UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(mUnidentified08, sizeof(mUnidentified08));
}

NetworkMessageType3_805333C8::NetworkMessageType3_805333C8()
{
}

void NetworkMessageType3_805333C8::Serialize(UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(mUnidentified08, sizeof(mUnidentified08));
    serializer->Transfer(&mUnidentified10, sizeof(mUnidentified10));
    serializer->Transfer(mUnidentified14, sizeof(mUnidentified14));
    serializer->Transfer(&mUnidentified18, sizeof(mUnidentified18));
    serializer->Transfer(mUnidentified1A, 11);
}

int NetworkMessageType3_805333C8::GetType()
{
    return 3;
}

int NetworkMessageType2_805333DC::GetType()
{
    return 2;
}
