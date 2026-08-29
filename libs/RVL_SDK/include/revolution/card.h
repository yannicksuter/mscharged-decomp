#ifndef _REVOLUTION_CARD_H_
#define _REVOLUTION_CARD_H_

#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct CARDFileInfo
    {
        /* 0x00 */ s32 chan;
        /* 0x04 */ s32 fileNo;
        /* 0x08 */ s32 offset;
        /* 0x0C */ s32 length;
        /* 0x10 */ u16 iBlock;
        /* 0x12 */ u16 padding;
    } CARDFileInfo;

#ifdef __cplusplus
}
#endif

#endif
