#include "unclassified/tu_802B7798.h"

static GLView* lbl_806E1DB8;
static GLView* lbl_806E1DBC;

void SetDebugFontView(GLView* pView)
{
    lbl_806E1DB8 = pView;
}

GLView* GetDebugFontView()
{
    return lbl_806E1DB8;
}

extern "C" void fn_802B77A8(GLView* pView)
{
    lbl_806E1DBC = pView;
}
