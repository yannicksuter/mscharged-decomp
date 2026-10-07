#include "Game/NetworkMessages.h"

void NetMessageGameStart::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mRandomSeed, sizeof(mRandomSeed));
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
    serializer->Transfer(&mMachineCount, sizeof(mMachineCount));
    serializer->Transfer(&mHomeCharacters[0], sizeof(mHomeCharacters[0]));
    serializer->Transfer(&mHomeCharacters[1], sizeof(mHomeCharacters[1]));
    serializer->Transfer(&mHomeCharacters[2], sizeof(mHomeCharacters[2]));
    serializer->Transfer(&mHomeCharacters[3], sizeof(mHomeCharacters[3]));
    serializer->Transfer(&mAwayCharacters[0], sizeof(mAwayCharacters[0]));
    serializer->Transfer(&mAwayCharacters[1], sizeof(mAwayCharacters[1]));
    serializer->Transfer(&mAwayCharacters[2], sizeof(mAwayCharacters[2]));
    serializer->Transfer(&mAwayCharacters[3], sizeof(mAwayCharacters[3]));
    serializer->Transfer(&mStadium, sizeof(mStadium));
    serializer->Transfer(mMachinePlayerCounts, sizeof(mMachinePlayerCounts));
    serializer->Transfer(&mTournamentGame, sizeof(mTournamentGame));
    serializer->Transfer(&mTournamentSetup[0], sizeof(mTournamentSetup[0]));
    serializer->Transfer(&mTournamentSetup[1], sizeof(mTournamentSetup) - 1);
    serializer->Transfer(&mSecondGameRandomSeed, sizeof(mSecondGameRandomSeed));
    serializer->Transfer(&mThirdGameRandomSeed, sizeof(mThirdGameRandomSeed));
}

int NetMessageGameStart::GetType()
{
    return 13;
}
