#ifndef NL_GLX_GLXFONT_H
#define NL_GLX_GLXFONT_H

#include "types.h"

class MemoryAllocator;

u32 glplatCreateFont(
    unsigned long width, unsigned long height, const unsigned short* data,
    unsigned long handle, MemoryAllocator* allocator);

#endif // NL_GLX_GLXFONT_H
