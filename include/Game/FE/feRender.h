#ifndef GAME_FE_RENDER_H
#define GAME_FE_RENDER_H

#include "NL/nlColour.h"
#include "NL/nlMath.h"

class FEScene;
class GLView;
class TLImageInstance;
class TLInstance;
struct UnidentifiedTextureState;

class FERender
{
public:
    static void Initialize();
    static void RenderScene(FEScene* scene);
    static void RenderTimeLineAsset(TLInstance* pTLInstance, float fCurrentTime, const nlMatrix4& parentMatrix);
    static unsigned char RenderImageInstance(const TLImageInstance* pTLImageInstance, const nlMatrix4& matrix);
    static void PushTransformMatrix(const TLInstance* instance, const nlMatrix4& parentMatrix, nlMatrix4& combinedMatrix);

    static FEScene* m_pRenderScene;
};


// Image-instance model callback installed by GameRenderTask and invoked from
// FERender::RenderImageInstance.
typedef void (*RenderImageCallback)(GLView* view, unsigned long texture,
    const UnidentifiedTextureState* pExtraTextureStates,
    const nlVector2* positions, const nlVector2* texcoords);
extern RenderImageCallback g_pfnRenderImage;

#endif // GAME_FE_RENDER_H
