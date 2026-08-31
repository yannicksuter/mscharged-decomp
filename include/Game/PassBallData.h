#ifndef GAME_PASS_BALL_DATA_H
#define GAME_PASS_BALL_DATA_H

class cPlayer;

struct UnidentifiedEventData_800663A8
{
    cPlayer* pPasser;
    cPlayer* pTarget;
    bool bVolleyPass;
    int mPasserControllerID;
}; // total size: 0x10

typedef UnidentifiedEventData_800663A8 PassBallData;

#endif // GAME_PASS_BALL_DATA_H
