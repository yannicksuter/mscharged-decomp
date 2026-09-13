#ifndef _GLXGXCOLOUR_H_
#define _GLXGXCOLOUR_H_

#include <revolution/gx/GXTev.h>

#include "NL/nlColour.h"

inline void gxSetTevKColour(int stage, const nlColour& colour)
{
    GXColor gxColour = { colour.c[0], colour.c[1], colour.c[2], colour.c[3] };
    GXSetTevKColor((GXTevKColorID)stage, gxColour);
}

inline void gxSetTevKColour(int stage, const nlFloatColour& colour)
{
    nlColour colour8;
    ConvertColour(colour8, colour);
    gxSetTevKColour(stage, colour8);
}

#endif // _GLXGXCOLOUR_H_
