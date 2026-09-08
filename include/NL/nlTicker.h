#ifndef NL_TICKER_H
#define NL_TICKER_H

#include "types.h"

void nlInitTicker();
u32 nlGetTicker();
f32 fn_802AAA28(u32 delta);
f32 nlGetTickerDifference(unsigned int startTick, unsigned int endTick);

#endif // NL_TICKER_H
