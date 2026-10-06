#ifndef GAME_PASS_BALL_DATA_H
#define GAME_PASS_BALL_DATA_H

class cPlayer;

struct PassBallData
{
    cPlayer* pPasser;
    cPlayer* pTarget;
    bool bVolleyPass;
    int mPasserControllerID;
}; // total size: 0x10


#endif // GAME_PASS_BALL_DATA_H
