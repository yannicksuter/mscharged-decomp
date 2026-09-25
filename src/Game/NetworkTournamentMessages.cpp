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

void NetMessageTournamentStart::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
    serializer->Transfer(&mMachineCount, sizeof(mMachineCount));
    serializer->Transfer(&mCupPersona, sizeof(mCupPersona));
    serializer->Transfer(&mFirstStadium, sizeof(mFirstStadium));
    serializer->Transfer(&mSecondStadium, sizeof(mSecondStadium));
    serializer->Transfer(mSeedings, sizeof(mSeedings));
}

void NetMessageTournamentGameUpdate::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUpdateType, sizeof(mUpdateType));
    serializer->Transfer(&mGameIndex, sizeof(mGameIndex));
    serializer->Transfer(&mIsHomeMachine, sizeof(mIsHomeMachine));
    serializer->Transfer(&mGameStatus, sizeof(mGameStatus));
    serializer->Transfer(&mGameTimeDelta, sizeof(mGameTimeDelta));
    serializer->Transfer(&mHasGameInfo, sizeof(mHasGameInfo));
    if (mHasGameInfo != 0)
    {
        serializer->Transfer(&mGameInfo, sizeof(mGameInfo));
    }
}

void NetMessageTournamentLoadingState::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
    serializer->Transfer(
        &mFinishedLoadingToKnockout, sizeof(mFinishedLoadingToKnockout));
}

int NetMessageTournamentLoadingState::GetType()
{
    return 33;
}

int NetMessageTournamentGameUpdate::GetType()
{
    return 32;
}

int NetMessageTournamentStart::GetType()
{
    return 20;
}
