#include "Game/NetworkMessages.h"
#include "Game/NetworkMessageSerializer.h"

#include <string.h>

void NetMessageMegaBallPointer::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mPointerX, sizeof(mPointerX));
    serializer->Transfer(&mPointerY, sizeof(mPointerY));
    serializer->Transfer(&mAngleHighByte, sizeof(mAngleHighByte));
    serializer->Transfer(&mTextureIndex, sizeof(mTextureIndex));
    serializer->Transfer(&mStatus, sizeof(mStatus));
}

void NetworkMessageType35::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mCount, sizeof(mCount));

    if (serializer->mDirection == 0)
    {
        u8 values = 0;
        memcpy(&values, serializer->mPosition, sizeof(values));
        serializer->mPosition += sizeof(values);
        for (int i = 0; i < mCount; ++i)
        {
            if ((values & (1 << i)) != 0)
            {
                mValues[i] = true;
            }
            else
            {
                mValues[i] = false;
            }
        }
    }
    else
    {
        u8 values = 0;
        for (int i = 0; i < mCount; ++i)
        {
            if (mValues[i])
            {
                values |= 1 << i;
            }
        }
        memcpy(serializer->mPosition, &values, sizeof(values));
        serializer->mPosition += sizeof(values);
    }
}

int NetworkMessageType35::GetType()
{
    return 35;
}

int NetMessageMegaBallPointer::GetType()
{
    return 34;
}
