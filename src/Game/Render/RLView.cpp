#include "Game/Render/RLView.h"

#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/glx/glxGX.h"

extern "C" void fn_8036EB90(bool enabled);

static bool sZWriteState;
static bool sZWriteChanged;

RLView::RLView(GLViewInterface* viewInterface, const GLRenderPair& renderPair, GLViewSortMode sortMode)
    : GLView(viewInterface, renderPair, sortMode)
{
    mZWriteEnabled = true;
    mCoPlanarEnabled = false;
}

RLView::~RLView()
{
}

void RLView::BeginRender()
{
    sZWriteState = mZWriteEnabled;
    fn_8036EB90(mZWriteEnabled);
}

void RLView::EndRender()
{
    if (mCoPlanarEnabled)
    {
        gxSetCoPlanar(false);
    }
}

void RLView::BeginPacket(const glModelPacket* packet)
{
    unsigned long state = glGetRasterState(packet->rasterState, GLS_AlphaBlend);
    bool zwrite = false;
    if (mZWriteEnabled && state != 2 && state != 3)
    {
        zwrite = true;
    }
    sZWriteChanged = zwrite != sZWriteState;
    if (sZWriteChanged)
    {
        fn_8036EB90(zwrite);
    }
    if (mCoPlanarEnabled && packet->indexBuffer != 0)
    {
        gxSetCoPlanar(true);
    }
}

void RLView::EndPacket(const glModelPacket* packet)
{
    if (mCoPlanarEnabled && packet->indexBuffer != 0)
    {
        gxSetCoPlanar(false);
    }
}
