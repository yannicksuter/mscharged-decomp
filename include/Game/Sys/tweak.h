#ifndef GAME_SYS_TWEAK_H
#define GAME_SYS_TWEAK_H

#include "NL/nlColour.h"

class GLView;

int nlScreenPrintf(int x, int y, bool bFromTop, int font, const char* format, ...);
void DrawTextRectangle(GLView* view, const nlColour& c,
    float left, float top, float right, float bottom, float z, int layer,
    bool bSnapToGrid, bool bVirtualCoords);

#endif // GAME_SYS_TWEAK_H
