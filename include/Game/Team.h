#ifndef GAME_TEAM_H
#define GAME_TEAM_H

#include "types.h"

class cNet;
class cFielder;

class cTeam
{
public:
    cFielder* GetFielder(int nIndex);
    cTeam* GetOtherTeam();

private:
    /* 0x00 */ u8 mUnidentified000[0xE8];

public:
    /* 0xE8 */ cNet* m_pNet;
};

#endif // GAME_TEAM_H
