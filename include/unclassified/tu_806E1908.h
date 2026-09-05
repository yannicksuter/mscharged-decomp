#ifndef UNCLASSIFIED_TU_806E1908_H
#define UNCLASSIFIED_TU_806E1908_H

struct TU806E1908
{
    /* 0x00 */ unsigned char mPadding00[0x3C];
    /* 0x3C */ unsigned long mUnidentified3C[10];
};

extern TU806E1908* lbl_806E1908;
extern "C" bool fn_8026F280(TU806E1908* object, int index, int slot, bool value);

#endif // UNCLASSIFIED_TU_806E1908_H
