#ifndef GAME_SH_HALL_OF_FAME_PLAYER_CARD_H
#define GAME_SH_HALL_OF_FAME_PLAYER_CARD_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feAsyncImage.h"
#include "Game/FE/feBackButton.h"

class SHHallOfFamePlayerCard : public BaseSceneHandler
{
public:
    SHHallOfFamePlayerCard(int mode);
    virtual ~SHHallOfFamePlayerCard();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void UpdateText();
    void UpdateImages();

    /* 0x01C */ int mMode;
    /* 0x020 */ int mCardIndex;
    /* 0x024 */ FEBackButton mNavigation;
    /* 0x0FC */ int mUnidentified0FC[4];
    /* 0x10C */ bool mSlideFinished;
    /* 0x10D */ bool mIsUnlocked;
    /* 0x10E */ unsigned short mTitleText[0x20];
    /* 0x14E */ unsigned short mDescriptionText[0x100];
    /* 0x34E */ unsigned short mNameText[0x20];
    /* 0x38E */ unsigned char mPadding38E[2];
    /* 0x390 */ AsyncImage mFrontImage;
    /* 0x430 */ AsyncImage mBackImage;
    /* 0x4D0 */ bool mFrontImageReady;
    /* 0x4D1 */ bool mBackImageReady;
    /* 0x4D2 */ unsigned char mPadding4D2[2];
}; // size 0x4D4

#endif // GAME_SH_HALL_OF_FAME_PLAYER_CARD_H
