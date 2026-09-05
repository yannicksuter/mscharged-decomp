#include "Game/FE/OnlineRanking.h"

#include "NL/nlFunction.h"

extern "C" void fn_80306208(UnidentifiedTimer_8030616C* timer, bool enabled);
extern "C" int lbl_806DD830;

typedef void (UnidentifiedOnlineRankingScene::*UnidentifiedOnlineRankingCallback)();

struct UnidentifiedOnlineRankingCallbackRef
{
    UnidentifiedOnlineRankingCallbackRef(UnidentifiedOnlineRankingCallback callback)
        : mCallback(callback)
    {
    }

    UnidentifiedOnlineRankingCallback mCallback;
};

struct UnidentifiedOnlineRankingBinding
{
    UnidentifiedOnlineRankingCallback mCallback;
    UnidentifiedOnlineRankingScene* mTarget;
    bool mUnidentified10;

    UnidentifiedOnlineRankingBinding(
        UnidentifiedOnlineRankingCallbackRef callback, UnidentifiedOnlineRankingScene* target)
        : mCallback(callback.mCallback)
        , mTarget(target)
    {
    }

    void operator()()
    {
        (mTarget->*mCallback)();
    }
};

static inline UnidentifiedOnlineRankingBinding BindOnlineRankingCallback(
    UnidentifiedOnlineRankingCallbackRef callback, UnidentifiedOnlineRankingScene* target)
{
    return UnidentifiedOnlineRankingBinding(callback, target);
}

UnidentifiedOnlineRankingScene::UnidentifiedOnlineRankingScene()
    : BaseOverlayHandler(0xFFFFFFFF, POSITION_ALL)
    , mUnidentified188(1.0f,
          Function<FnVoidVoid>(BindOnlineRankingCallback(
              &UnidentifiedOnlineRankingScene::fn_801F048C, this)))
{
    mUnidentified1A4 = false;
    mUnidentified1A5 = false;
    mUnidentified1A6 = false;
    mUnidentified1A8 = lbl_806DD830;
    fn_80306208(&mUnidentified188, true);
}

void UnidentifiedOnlineRankingScene::fn_801F048C()
{
    mUnidentified1A4 = true;
    --mUnidentified1A8;
}
