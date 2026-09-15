#include "NL/nlDebugViews.h"

static GLView* sDebugFontView;
static GLView* sDebugSquareView;

void SetDebugFontView(GLView* pView)
{
    sDebugFontView = pView;
}

GLView* GetDebugFontView()
{
    return sDebugFontView;
}

void SetDebugSquareView(GLView* pView)
{
    sDebugSquareView = pView;
}
