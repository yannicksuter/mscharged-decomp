#include "Game/Render/RLView.h"

#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxSend.h"

static bool sFogState;
static bool sFogChanged;

RLView::RLView(GLViewInterface* viewInterface, const GLRenderPair& renderPair, GLViewSortMode sortMode)
    : GLView(viewInterface, renderPair, sortMode)
{
    mFogEnabled = true;
    mCoPlanarEnabled = false;
}

RLView::~RLView()
{
}

void RLView::BeginRender()
{
    sFogState = mFogEnabled;
    glx_Fog(mFogEnabled);
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
    bool fog = false;
    if (mFogEnabled && state != 2 && state != 3)
    {
        fog = true;
    }
    sFogChanged = fog != sFogState;
    if (sFogChanged)
    {
        glx_Fog(fog);
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
