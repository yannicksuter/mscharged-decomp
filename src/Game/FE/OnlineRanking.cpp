#include "Game/FE/OnlineRanking.h"

#include "NL/nlBind.h"
#include "NL/nlFunction.h"

extern "C" int lbl_806DD830;

UnidentifiedOnlineRankingScene::UnidentifiedOnlineRankingScene()
    : BaseOverlayHandler(0xFFFFFFFF, POSITION_ALL)
    , mUnidentified188(1.0f,
          Function<FETimer*>(Bind<void>(
              MemFun(&UnidentifiedOnlineRankingScene::fn_801F048C), this, Placeholder<0>())))
{
    mUnidentified1A4 = false;
    mUnidentified1A5 = false;
    mUnidentified1A6 = false;
    mUnidentified1A8 = lbl_806DD830;
    mUnidentified188.SetEnabled(true);
}

void UnidentifiedOnlineRankingScene::fn_801F048C(FETimer* timer)
{
    mUnidentified1A4 = true;
    --mUnidentified1A8;
}
