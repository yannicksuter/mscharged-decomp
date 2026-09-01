#ifndef GAME_NET_TOURN_MANAGER_H
#define GAME_NET_TOURN_MANAGER_H

#include "Game/DB/BasicGameInfo.h"
#include "Game/NetworkMessages.h"
#include "types.h"

enum NetworkTournamentGameState
{
    NET_TOURN_GAME_EMPTY = 0,
    NET_TOURN_GAME_READY = 1,
    NET_TOURN_GAME_IN_PROGRESS = 2,
    NET_TOURN_GAME_STATE_3 = 3,
    NET_TOURN_GAME_STATE_4 = 4,
    NET_TOURN_GAME_OVER = 5,
    NET_TOURN_GAME_NO_CONTEST = 6,
    NET_TOURN_GAME_NO_PLAYERS = 7,
    NET_TOURN_GAME_HOME_ADVANCES = 8,
    NET_TOURN_GAME_AWAY_ADVANCES = 9,
    NET_TOURN_GAME_STATE_10 = 10,
    NET_TOURN_GAME_STATE_11 = 11,
};

struct NetworkTournamentGame
{
    NetworkTournamentGame()
        : mState(NET_TOURN_GAME_EMPTY)
        , mHomeMachine(-1)
        , mAwayMachine(-1)
        , mBracketIndex(-1)
        , mHomeUpdate(0)
        , mAwayUpdate(0)
    {
        mGameInfo.Reset(true);
    }

    bool IsFinished() const;
    bool GetWinnerAndLoser(int* winnerSide, int* winningMachine) const;

    /* 0x000 */ int mState;
    /* 0x004 */ int mHomeMachine;
    /* 0x008 */ int mAwayMachine;
    /* 0x00C */ int mBracketIndex;
    /* 0x010 */ BasicGameInfo mGameInfo;
    /* 0x138 */ int mHomeUpdate;
    /* 0x13C */ int mAwayUpdate;
}; // size: 0x140

class NetworkTournamentCupInterface
{
public:
    virtual BasicGameInfo* GetGameInfo(int phase, int matchup) = 0;
    virtual bool HasGameBeenPlayed(int phase, int matchup) = 0;
    virtual NetworkTournamentGame* GetTournamentGame(int phase, int matchup) = 0;
    virtual BasicGameInfo* GetCurrentGameInfo() = 0;
    virtual int GetRoundMask(int phase, int round) const = 0;
    virtual int GetNumTournamentGames() const = 0;
    virtual int GetCurrentMode() const = 0;
    virtual int GetStadium() const = 0;
    virtual bool IsFinalRound() const = 0;
    virtual s16 GetCurrentRoundNumber() const = 0;
    virtual int IsTournamentMode() const = 0;
    virtual int GetNumRounds() const = 0;
};

class NetTournManager : public NetworkTournamentCupInterface,
                        public UnidentifiedNetworkMessageReceiver
{
public:
    NetTournManager()
    {
        Reset(true);
        mTrophyPresentation = 0;
    }

    static void CreateInstance();
    static NetTournManager* Instance();
    static void GenerateFirstRoundSeedings(int machineCount, u8* seedings);

    void Reset(bool clearTeams);
    void TransitionOnlineMenuToTournament(NetMessageTournamentStart* message);
    void BuildInitialBracket();
    void AdvanceBracket();
    void OnTournamentGameStart(NetMessageGameStart* message);
    int MachineIdxToTournamentIdx(int machine) const;
    int TournamentIdxToMachineIdx(int machine) const;
    bool SendTournamentGameStart(NetworkTournamentGame* game);
    void SendToAllTournamentMachines(void* data, int size);
    void UpdateRoundGameRange();
    void StartReadyGames();
    void MarkDisconnectedMachine(int machine);
    bool AreRoundGamesFinished();
    void Update(float dt);
    void NotifyGameStarted();
    void NotifyFinishedLoadingToKnockout();
    void NotifyOverlayPopped(int overlay);
    void NotifyGameOver();
    void ResetGameProgressUpdateTimer(int reason);
    virtual int ReceiverVirtual00(UnidentifiedNetworkMessage* message);
    void HandleTournamentGameUpdate(NetMessageTournamentGameUpdate* message);

    virtual BasicGameInfo* GetGameInfo(int phase, int matchup);
    virtual bool HasGameBeenPlayed(int phase, int matchup);
    virtual NetworkTournamentGame* GetTournamentGame(int phase, int matchup);
    virtual BasicGameInfo* GetCurrentGameInfo();
    virtual int GetRoundMask(int phase, int round) const;
    virtual int GetNumTournamentGames() const;
    virtual int GetCurrentMode() const;
    virtual int GetStadium() const { return mStadium; }
    virtual bool IsFinalRound() const;
    virtual s16 GetCurrentRoundNumber() const;
    virtual int IsTournamentMode() const;
    virtual int GetNumRounds() const;

    void AttachTournamentTrophy(void* presentation);
    void DetachTournamentTrophy();
    void DestroyTournamentTrophy();
    const char* GetTournamentTrophyResource() const;

    /* 0x008 */ int mState;
    /* 0x00C */ int mMachineCount;
    /* 0x010 */ int mLocalMachineIndex;
    /* 0x014 */ bool mLargeBracket;
    /* 0x015 */ u8 mPadding015[3];
    /* 0x018 */ int mStadium;
    /* 0x01C */ int mHomeTeam;
    /* 0x020 */ int mAwayTeam;
    /* 0x024 */ int mSeedings[8];
    /* 0x044 */ s16 mCurrentRound;
    /* 0x046 */ u8 mPadding046[2];
    /* 0x048 */ float mTimeToStartGames;
    /* 0x04C */ bool mWaitingToStartGames;
    /* 0x04D */ u8 mPadding04D[3];
    /* 0x050 */ NetworkTournamentGame mGames[7];
    /* 0x910 */ int mWinningMachine;
    /* 0x914 */ bool mLoadedToKnockout[8];
    /* 0x91C */ bool mLoadedToGame[8];
    /* 0x924 */ bool mLocalMachineEliminated;
    /* 0x925 */ bool mTournamentMachineMappingActive;
    /* 0x926 */ u8 mPadding926[2];
    /* 0x928 */ int mCurrentGameIndex;
    /* 0x92C */ int mTournamentToMachine[2];
    /* 0x934 */ int mFirstGameInRound;
    /* 0x938 */ int mLastGameInRound;
    /* 0x93C */ int mLastGameProgressUpdate;
    /* 0x940 */ int mGameProgressUpdateCount;
    /* 0x944 */ void* mTrophyPresentation;
    /* 0x948 */ void* mTrophyResource;
}; // size: 0x94C

#endif // GAME_NET_TOURN_MANAGER_H
