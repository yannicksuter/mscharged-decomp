#ifndef NL_GLX_DISPLAY_LIST_H
#define NL_GLX_DISPLAY_LIST_H

#include "types.h"

struct DisplayList
{
    /* 0x00 */ u32 magic;
    /* 0x04 */ void* list;
    /* 0x08 */ u32 size;
    /* 0x0C */ u16 numStreams;
    /* 0x0E */ u16 hasColorStream;
}; // size: 0x10

#endif // NL_GLX_DISPLAY_LIST_H
