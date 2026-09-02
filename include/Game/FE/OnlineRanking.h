#ifndef GAME_FE_ONLINE_RANKING_H
#define GAME_FE_ONLINE_RANKING_H

#include "Game/FE/BaseOverlayHandler.h"

struct UnidentifiedTimer_8030616C
{
    u32 mUnidentified00[7];
}; // size 0x1C

class UnidentifiedOnlineRankingScene : public BaseOverlayHandler
{
public:
    UnidentifiedOnlineRankingScene();
    virtual ~UnidentifiedOnlineRankingScene();
    virtual void Update(float dt);
    virtual void SceneCreated();

    void fn_801F048C();

    /* 0x028 */ u32 mUnidentified028[0x58];
    /* 0x188 */ UnidentifiedTimer_8030616C mUnidentified188;
    /* 0x1A4 */ bool mUnidentified1A4;
    /* 0x1A5 */ bool mUnidentified1A5;
    /* 0x1A6 */ bool mUnidentified1A6;
    /* 0x1A7 */ u8 mPadding1A7;
    /* 0x1A8 */ int mUnidentified1A8;
    /* 0x1AC */ u8 mUnidentified1AC[0x14];
}; // size 0x1C0

#endif // GAME_FE_ONLINE_RANKING_H
