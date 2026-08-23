#include "NL/gl/glStruct.h"

gl_ScreenInfo _ScreenInfo;

gl_ScreenInfo* glGetScreenInfo()
{
    return &_ScreenInfo;
}

u32 glGetScreenWidth()
{
    return _ScreenInfo.ScreenWidth;
}

u32 glGetScreenHeight()
{
    return _ScreenInfo.ScreenHeight;
}
