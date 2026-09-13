#ifndef _OVERLAYHANDLERSTRIKERTIMES_H_
#define _OVERLAYHANDLERSTRIKERTIMES_H_

#include "Game/FE/BaseOverlayHandler.h"
#include "Game/FE/feScrollText.h"
#include "Game/FE/feTimer.h"

class AsyncImage;

class StrikerTimesOverlay : public BaseOverlayHandler
{
public:
    StrikerTimesOverlay();
    virtual ~StrikerTimesOverlay();
    virtual void Update(float dt);
    virtual void SceneCreated();

    void OnCountdownTick(FETimer* timer);
    void SetArticleImage(int captain, int variant);

    /* 0x028 */ FEScrollText* mHeadlineScroller;
    /* 0x02C */ FEScrollText mStoryScroller;
    /* 0x06C */ unsigned short mTimerText[8];
    /* 0x07C */ bool mIsNetworkGame;
    /* 0x080 */ FETimer mCountdownTimer;
    /* 0x09C */ bool mCountdownTicked;
    /* 0x0A0 */ int mCountdownSeconds;
    /* 0x0A4 */ AsyncImage* mArticleImage;
    /* 0x0A8 */ bool mButtonsHidden;
    /* 0x0AC */ float mInputDelay;
}; // size 0xB0

#endif // _OVERLAYHANDLERSTRIKERTIMES_H_
