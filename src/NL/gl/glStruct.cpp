#include "NL/gl/glStruct.h"

gl_ScreenInfo gScreenInfo;

gl_ScreenInfo* glGetScreenInfo()
{
    return &gScreenInfo;
}

u32 glGetScreenWidth()
{
    return gScreenInfo.ScreenWidth;
}

u32 glGetScreenHeight()
{
    return gScreenInfo.ScreenHeight;
}
