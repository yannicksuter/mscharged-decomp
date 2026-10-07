#include "Game/NetworkMessages.h"

void NetMessageDraft::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
    serializer->Transfer(&mMachineCount, sizeof(mMachineCount));
    serializer->Transfer(&mChooseSides, sizeof(mChooseSides));
    serializer->Transfer(&mPlayerSides, sizeof(mPlayerSides));
    NetworkDraftMachineInfo* entry = mEntries;
    for (int i = 0; i < mMachineCount; ++entry, ++i)
    {
        serializer->Transfer(&entry->mStats, sizeof(entry->mStats));
        serializer->Transfer(&entry->mProfileId, sizeof(entry->mProfileId));
        serializer->Transfer(entry->mName, sizeof(entry->mName));
        serializer->Transfer(entry->mMiiData, sizeof(entry->mMiiData));
        serializer->Transfer(&entry->mMachineIndex, sizeof(entry->mMachineIndex));
        serializer->Transfer(&entry->mGuestEnabled, sizeof(entry->mGuestEnabled));
    }
}

void NetMessageDraftMachineInfo::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mEntry.mStats, sizeof(mEntry.mStats));
    serializer->Transfer(&mEntry.mProfileId, sizeof(mEntry.mProfileId));
    serializer->Transfer(mEntry.mName, sizeof(mEntry.mName));
    serializer->Transfer(mEntry.mMiiData, sizeof(mEntry.mMiiData));
    serializer->Transfer(&mEntry.mMachineIndex, sizeof(mEntry.mMachineIndex));
    serializer->Transfer(&mEntry.mGuestEnabled, sizeof(mEntry.mGuestEnabled));
}

void NetMessageDraftPickedCaptain::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mTeamIndex, sizeof(mTeamIndex));
    serializer->Transfer(&mCaptain, sizeof(mCaptain));
}

void NetMessageDraftPickedSidekicks::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mTeamIndex, sizeof(mTeamIndex));
    serializer->Transfer(&mSidekick0, sizeof(mSidekick0));
    serializer->Transfer(&mSidekick1, sizeof(mSidekick1));
    serializer->Transfer(&mSidekick2, sizeof(mSidekick2));
}

void NetMessageSidesChanged::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
    serializer->Transfer(&mSide, sizeof(mSide));
    serializer->Transfer(&mGuest, sizeof(mGuest));
    serializer->Transfer(&mAccepted, sizeof(mAccepted));
}

void NetMessageCheckConnection::Serialize(NetworkMessageSerializer* serializer)
{
    for (int i = 0; i < 2; ++i)
    {
        serializer->Transfer(&mProfileIds[i], sizeof(mProfileIds[i]));
    }
}

void NetMessageConnectionDecision::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mAccepted, sizeof(mAccepted));
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
}
