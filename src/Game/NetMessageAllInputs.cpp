#include "Game/NetworkInputMessages.h"

#include <string.h>

NetMessageAllInputs::NetMessageAllInputs()
{
    mFlags = 0;
    memset(mControl80Data, 0, sizeof(mControl80Data));
}

void NetMessageAllInputs::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mFlags, 1);
    for (int i = 0; i < 4; ++i)
    {
        if (mFlags & (1 << i))
            mMessages[i].Serialize(serializer);
    }
    if (mFlags & 0x80)
    {
        serializer->Transfer(&mControl80Data[0], 4);
        serializer->Transfer(&mControl80Data[1], 4);
        serializer->Transfer(&mControl80Data[2], 4);
    }
}

void NetMessageAllInputsBundle::Serialize(
    NetworkMessageSerializer* serializer)
{
    mMessage0.Serialize(serializer);
    mMessage1.Serialize(serializer);
}

int NetMessageAllInputsBundle::GetType()
{
    return NETMSG_ALL_INPUTS_BUNDLE;
}

int NetMessageAllInputs::GetType()
{
    return NETMSG_ALL_INPUTS;
}
