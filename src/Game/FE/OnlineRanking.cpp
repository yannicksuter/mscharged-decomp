#include "Game/FE/OnlineRanking.h"

#include "NL/nlBind.h"
#include "NL/nlFunction.h"

extern "C" void fn_80306208(UnidentifiedTimer_8030616C* timer, bool enabled);
extern "C" int lbl_806DD830;

UnidentifiedOnlineRankingScene::UnidentifiedOnlineRankingScene()
    : BaseOverlayHandler(0xFFFFFFFF, POSITION_ALL)
    , mUnidentified188(1.0f,
          Function<UnidentifiedTimer_8030616C*>(Bind<void>(
              MemFun(&UnidentifiedOnlineRankingScene::fn_801F048C), this, Placeholder<0>())))
{
    mUnidentified1A4 = false;
    mUnidentified1A5 = false;
    mUnidentified1A6 = false;
    mUnidentified1A8 = lbl_806DD830;
    fn_80306208(&mUnidentified188, true);
}

void UnidentifiedOnlineRankingScene::fn_801F048C(UnidentifiedTimer_8030616C* timer)
{
    mUnidentified1A4 = true;
    --mUnidentified1A8;
}
