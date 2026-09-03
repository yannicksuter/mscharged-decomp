class GLView;

static GLView* lbl_806E1DB8;
static GLView* lbl_806E1DBC;

extern "C" void fn_802B7798(GLView* pView)
{
    lbl_806E1DB8 = pView;
}

extern "C" GLView* fn_802B77A0()
{
    return lbl_806E1DB8;
}

extern "C" void fn_802B77A8(GLView* pView)
{
    lbl_806E1DBC = pView;
}
