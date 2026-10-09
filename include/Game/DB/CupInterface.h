#ifndef GAME_DB_CUP_INTERFACE_H
#define GAME_DB_CUP_INTERFACE_H

#include "types.h"

struct BasicGameInfo;
struct NetworkTournamentGame;

enum eCupPersona
{
    CUP_PERSONA_MUSHROOM = 0,
    CUP_PERSONA_FLOWER = 1,
    CUP_PERSONA_STAR = 2,
    CUP_PERSONA_SUNSHINE = 3,
    CUP_PERSONA_BANANA = 4,
    CUP_PERSONA_NEXT_LEVEL = 5,
    CUP_PERSONA_KONGA = 6,
    CUP_PERSONA_SAND = 7,
    CUP_PERSONA_LAVA = 8,
    CUP_PERSONA_NINTENDO = 9,
};

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
