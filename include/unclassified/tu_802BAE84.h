#ifndef UNCLASSIFIED_TU_802BAE84_H
#define UNCLASSIFIED_TU_802BAE84_H

#include "NL/nlColour.h"

class GLView;

extern "C"
{
    int fn_802BB048(
        int x, int y, bool flipY, int font, const char* format, ...);
    void fn_802BB11C(GLView* view, const nlColour& colour,
        float x0, float y0, float x1, float y1, float depth, int layer,
        bool expand, bool convertCoordinates);
}

#endif // UNCLASSIFIED_TU_802BAE84_H
