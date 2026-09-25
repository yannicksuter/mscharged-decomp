#ifndef GAME_FE_FE_CHARACTER_PDA_COMPONENT_H
#define GAME_FE_FE_CHARACTER_PDA_COMPONENT_H

#include "types.h"
#include "Game/FE/feScrollBar.h"
#include "Game/FE/feScrollText.h"
#include "Game/FE/feTimer.h"

class TLComponentInstance;
class TLImageInstance;
class TLInstance;
struct UnidentifiedTLGroupInstance;

struct FEAttributeBar
{
    FEAttributeBar(TLComponentInstance* component);
    void SetValue(int value);

    /* 0x00 */ int mValue;
    /* 0x04 */ TLComponentInstance* mComponent;
}; // size 0x8

class FECharacterPDAComponent
{
public:
    FECharacterPDAComponent();
    virtual ~FECharacterPDAComponent();

    void Initialize(TLComponentInstance* component, int side, u32 value);
    void SetVisible(bool visible);
    void SetReadyPromptVisible(bool visible);
    void Update(float dt);
    void ResetAttributes();
    void StepAttributeBar(FETimer* timer, FEAttributeBar* component, int value);
    void SetCaptainInfo(int captain, int index, unsigned long flag);
    void SetSidekickInfo(int sidekick, int index, unsigned long flag);
    void SetDisplayMode(int value);
    void ShowSlideIn();
    void ApplyCaptainColours(int captain, int opponent);
    void TintInstanceForCaptain(TLInstance* instance, int captain, unsigned char alpha);
    void SetCupIconsVisible(bool fire, bool crystal, bool striker);

    /* 0x004 */ FEAttributeBar* mCaptainAttributeBars[4];
    /* 0x014 */ FEAttributeBar* mSidekickAttributeBars[4];
    /* 0x024 */ u32 mUnidentified24;
    /* 0x028 */ UnidentifiedTimerList_80306294 mTimers;
    /* 0x034 */ FEScrollText mScrollText;
    /* 0x074 */ TLComponentInstance* mRootComponent;
    /* 0x078 */ TLComponentInstance* mPDAScreens;
    /* 0x07C */ TLComponentInstance* mCaptainAttributes;
    /* 0x080 */ TLComponentInstance* mPositions;
    /* 0x084 */ TLComponentInstance* mJoinPrompt;
    /* 0x088 */ TLComponentInstance* mReadyPrompt;
    /* 0x08C */ UnidentifiedTLGroupInstance* mPositionsGroup;
    /* 0x090 */ TLComponentInstance* mSidekickAttributes;
    /* 0x094 */ TLComponentInstance* mTeamLogos;
    /* 0x098 */ TLImageInstance* mSidekickAttributeImages[7];
    /* 0x0B4 */ TLImageInstance* mCaptainAttributeImages[7];
    /* 0x0D0 */ TLComponentInstance* mDescriptions;
    /* 0x0D4 */ TLComponentInstance* mScrollArrows;
    /* 0x0D8 */ UnidentifiedTLGroupInstance* mContinueGroup;
    /* 0x0DC */ FEScrollBar mScrollBar;
    /* 0x290 */ int mUnidentified290;
    /* 0x294 */ int mSelectedCaptain;
    /* 0x298 */ int mSelectedSidekick;
    /* 0x29C */ int mDisplayMode;
    /* 0x2A0 */ int mSide;
    /* 0x2A4 */ bool mCaptainNeedsRefresh;
    /* 0x2A5 */ bool mSidekickNeedsRefresh;
    /* 0x2A6 */ bool mSlideAnimating;
    /* 0x2A7 */ bool mScrollBarNeedsReset;
    /* 0x2A8 */ bool mUnidentified2A8;
}; // size 0x2AC

#endif // GAME_FE_FE_CHARACTER_PDA_COMPONENT_H
