#ifndef _GCSWIZZLER_H_
#define _GCSWIZZLER_H_

#include "types.h"
#include "NL/glx/glxTexture.h"

u32 GCTextureSize(eGXTextureFormat format, int width, int height, int numLevels,
    unsigned long texhandle);
void GCSwizzle(void* swizzledData, const void* linearData, unsigned short width,
    unsigned short height, eGXTextureFormat format, bool endianSwap);

#endif // _GCSWIZZLER_H_
