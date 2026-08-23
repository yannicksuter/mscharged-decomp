#ifndef _GLSTRUCT_H_
#define _GLSTRUCT_H_

#include "types.h"

struct gl_ScreenInfo
{
    int ScreenWidth;
    int ScreenHeight;
    int ColourDepth[4];
    int ZDepth;
    int StencilDepth;
    float PixelCentre;
    bool FSAA;
};

gl_ScreenInfo* glGetScreenInfo();
u32 glGetScreenWidth();
u32 glGetScreenHeight();

#endif // _GLSTRUCT_H_
