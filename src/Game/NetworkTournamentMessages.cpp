#include "Game/NetworkMessages.h"
#include "Game/NetworkMessageSerializer.h"

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
