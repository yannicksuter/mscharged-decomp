#ifndef _FILTEREDRANDOM_H_
#define _FILTEREDRANDOM_H_

#include "types.h"

class FilteredRandomRange
{
public:
    FilteredRandomRange();
    ~FilteredRandomRange() { }
    int genrand(int range);

    /* 0x00 */ u32 m_repeatingRunLength;
    /* 0x04 */ s32 m_hist[10];
}; // total size: 0x2C

#endif // _FILTEREDRANDOM_H_
