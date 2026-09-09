#ifndef GAME_DB_CUP_INTERFACE_H
#define GAME_DB_CUP_INTERFACE_H

#include "types.h"

struct BasicGameInfo;
struct NetworkTournamentGame;

class CupInterface
{
public:
    virtual BasicGameInfo* GetGameInfo(int phase, int matchup) = 0;
    virtual bool HasGameBeenPlayed(int phase, int matchup) = 0;
    virtual NetworkTournamentGame* GetTournamentGame(int phase, int matchup) = 0;
    virtual BasicGameInfo* GetCurrentGameInfo() = 0;
    virtual u16 GetNumGamesPerRound(int phase, int round) const = 0;
    virtual u16 GetNumGames(int phase) const = 0;
    virtual int GetCurrentMode() const = 0;
    virtual int GetCupPersona() const = 0;
    virtual bool IsCupWinningGame(int team) const = 0;
    virtual s16 GetCurrentRoundNumber() const = 0;
    virtual int GetCurrentRoundType() const = 0;
    virtual u16 GetNumPlayoffRounds() const = 0;
};

#endif // GAME_DB_CUP_INTERFACE_H
