#ifndef GAME_FE_RENDER_H
#define GAME_FE_RENDER_H

#include "NL/nlColour.h"
#include "NL/nlMath.h"

class FEScene;
class TLImageInstance;
class TLInstance;

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

#endif // GAME_FE_RENDER_H
