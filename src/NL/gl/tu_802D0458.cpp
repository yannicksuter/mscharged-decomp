#include "NL/gl/UnidentifiedChunkLoader_802D0458.h"

#include "Game/GL/GLInventory.h"
#include "Game/GL/GLTextureAnim.h"
#include "Game/SAnim.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glTexture.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlDLRing.h"

UnidentifiedChunkLoader_802D0458::UnidentifiedChunkLoader_802D0458(
    ResourceInterface_802CC094* pResource, int nParam)
{
    m_pResource = pResource;
    m_pModels = 0;
    m_nUnidentified008 = nParam;
}

bool UnidentifiedChunkLoader_802D0458::fn_802D046C(nlChunk* pChunk)
{
    switch (pChunk->GetID())
    {
    case 0x00024100:
        fn_802CDD78(pChunk->GetData(),
            pChunk->GetSize()
                - ((u8*)pChunk->GetAlignedData()
                    - (u8*)pChunk->GetUnalignedData()),
            m_pResource,
            m_nUnidentified008);
        break;
    case 0x8001B000:
    case 0x8001B100:
        unsigned long numModels;
        m_pModels = (glModel*)fn_802C81FC(pChunk,
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
    ResourceInterface_802CC094* pResource = nlDLRingGetStart(fn_802CC09C());
    for (;; pResource = pResource->m_next)
    {
        PlatTexture* pTex = pResource->m_inventory->fn_802D2370(handle);
        if (pTex != 0)
            return pTex;
        if (nlDLRingIsEnd(fn_802CC09C(), pResource))
            break;
    }
    return 0;
}

extern "C" GLTextureAnim* fn_802D0758(unsigned long texture)
{
    ResourceInterface_802CC094* pResource = nlDLRingGetStart(fn_802CC09C());
    for (;; pResource = pResource->m_next)
    {
        GLTextureAnim* anim = pResource->m_inventory->GetTextureAnim(texture);
        if (anim != 0)
            return anim;
        if (nlDLRingIsEnd(fn_802CC09C(), pResource))
            break;
    }
    return 0;
}
