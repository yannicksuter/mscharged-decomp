#ifndef NL_TICKER_H
#define NL_TICKER_H

#include "types.h"

void nlInitTicker();
u32 nlGetTicker();
f32 fn_802AAA28(u32 delta);
f32 nlGetTickerDifference(u32 startTick, u32 endTick);

#endif // NL_TICKER_H
