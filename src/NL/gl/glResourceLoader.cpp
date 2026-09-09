#include "NL/gl/glResourceLoader.h"

#include "Game/GL/GLInventory.h"
#include "Game/GL/GLTextureAnim.h"
#include "Game/SAnim.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glTexture.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlDLRing.h"

GLResourceChunkLoader::GLResourceChunkLoader(
    GLResourcePool* pResource, int textureBundleMode)
{
    m_pResource = pResource;
    m_pModels = 0;
    mTextureBundleMode = textureBundleMode;
}

bool GLResourceChunkLoader::LoadChunk(nlChunk* pChunk)
{
    switch (pChunk->GetID())
    {
    case 0x00024100:
        glEndLoadTextureBundle(pChunk->GetData(),
            pChunk->GetSize()
                - ((u8*)pChunk->GetAlignedData()
                    - (u8*)pChunk->GetUnalignedData()),
            m_pResource,
            mTextureBundleMode);
        break;
    case 0x8001B000:
    case 0x8001B100:
        unsigned long numModels;
        m_pModels = (glModel*)glEndLoadModel(pChunk,
            pChunk->GetSize()
                - ((u8*)pChunk->GetAlignedData()
                    - (u8*)pChunk->GetUnalignedData()),
            &numModels,
            m_pResource);
        break;
    default:
        return false;
    }
    return true;
}

PlatTexture* glx_GetTex(unsigned long handle)
{
    GLResourcePool* pResource = nlDLRingGetStart(glGetResourcePools());
    for (;; pResource = pResource->m_next)
    {
        PlatTexture* pTex = pResource->m_inventory->GetTexture(handle);
        if (pTex != 0)
            return pTex;
        if (nlDLRingIsEnd(glGetResourcePools(), pResource))
            break;
    }
    return 0;
}

GLTextureAnim* glGetTextureAnim(unsigned long texture)
{
    GLResourcePool* pResource = nlDLRingGetStart(glGetResourcePools());
    for (;; pResource = pResource->m_next)
    {
        GLTextureAnim* anim = pResource->m_inventory->GetTextureAnim(texture);
        if (anim != 0)
            return anim;
        if (nlDLRingIsEnd(glGetResourcePools(), pResource))
            break;
    }
    return 0;
}
