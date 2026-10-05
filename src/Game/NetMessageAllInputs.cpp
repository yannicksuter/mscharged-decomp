#include "Game/NetworkInputMessages.h"

#include <string.h>

NetMessageAllInputs::NetMessageAllInputs()
{
    mFlags = 0;
    memset(mUnidentified3CC, 0, sizeof(mUnidentified3CC));
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
        serializer->Transfer(&mUnidentified3CC[0], 4);
        serializer->Transfer(&mUnidentified3CC[1], 4);
        serializer->Transfer(&mUnidentified3CC[2], 4);
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
    return 9;
}

int NetMessageAllInputs::GetType()
{
    return 8;
}
