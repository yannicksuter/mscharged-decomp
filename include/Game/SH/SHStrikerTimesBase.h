#ifndef GAME_SH_SH_STRIKER_TIMES_BASE_H
#define GAME_SH_SH_STRIKER_TIMES_BASE_H

#include "Game/FE/BaseOverlayHandler.h"
#include "Game/FE/feAsyncImage.h"
#include "Game/FE/feScrollText.h"
#include "NL/nlBasicString.h"
#include "NL/nlFunction.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feScrollBar.h"

class TLComponentInstance;
class TLInstance;
class TLTextInstance;

class SHStrikerTimesBase : public BaseOverlayHandler
{
public:
    SHStrikerTimesBase();
    virtual ~SHStrikerTimesBase();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void SetDisplayMode(unsigned int transition);
    virtual void OnDoneTransitionComplete();
    virtual void OnBackTransitionComplete() { }
    virtual void SetArticleImageName(int captain, int mood, int special);

    void ShowPreviousPage();
    void ShowNextPage();
    void InitializeControls();
    void InitializeContent();
    void OnDonePointerEnter(int index, void* context);
    void OnDonePointerLeave(int index, void* context);
    void OnDonePointerPress(int index, void* context);

    /* 0x028 */ int mDisplayMode;
    /* 0x02C */ int mPage;
    /* 0x030 */ bool mDonePressed;
    /* 0x031 */ char mHeadlineStringID[0x40];
    /* 0x071 */ char mStoryStringID[0x40];
    /* 0x0B1 */ char mArticleImageName[0x43];
    /* 0x0F4 */ BasicString<unsigned short, Detail::TempStringAllocator> mHeadlineText;
    /* 0x0F8 */ BasicString<unsigned short, Detail::TempStringAllocator> mStoryText;
    /* 0x0FC */ bool mUseCustomText;
    /* 0x100 */ int mState;
    /* 0x104 */ bool mControlsInitialized;
    /* 0x105 */ bool mDoneVisible;
    /* 0x106 */ bool mCanShowDone;
    /* 0x107 */ bool mContentInitialized;
    /* 0x108 */ bool mIntroAudioPlayed;
    /* 0x109 */ bool mLogoReady;
    /* 0x10C */ FEPointerButton mDoneButton;
    /* 0x1C0 */ FEScrollText mHeadlineScroller;
    /* 0x200 */ FEScrollText mStoryHeadlineScroller;
    /* 0x240 */ AsyncImage mStoryImage;
    /* 0x2E0 */ AsyncImage mHeadlineImage;
    /* 0x380 */ AsyncImage mLogoImage;
    /* 0x420 */ FEScrollBar mScrollBar;
}; // size 0x5D4

#endif // GAME_SH_SH_STRIKER_TIMES_BASE_H
