#include "NL/gl/glTexture.h"

#include "Game/GL/GLInventory.h"
#include "Game/GL/GLTextureAnim.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

TextureManager_802CDF0C* lbl_806E1F08;
u32 lbl_806E1F0C = nlStringLowerHash("NLG_DIFFUSE");
u32 lbl_806E1F10 = nlStringLowerHash("NLG_DETAIL");
u32 lbl_806E1F14 = nlStringLowerHash("NLG_SPECULAR");
u32 lbl_806E1F18 = nlStringLowerHash("NLG_BUMPMAP");
u32 lbl_806E1F1C = nlStringLowerHash("NLG_NORMALMAP");
u32 lbl_806E1F20 = nlStringLowerHash("NLG_SHADOW");
u32 lbl_806E1F24 = nlStringLowerHash("NLG_SELFILLUM");
u32 lbl_806E1F28 = nlStringLowerHash("NLG_GLOSS");
u32 lbl_806E1F2C = nlStringLowerHash("NLG_RAMP");
u32 lbl_806E1F30 = nlStringLowerHash("NLG_MASK");
u32 lbl_806E1F34 = nlStringLowerHash("global/white");

bool glTextureLoad(unsigned long texture)
{
    unsigned long result = lbl_806E1F08->fn_802CE1B8(texture);
    if (result == 0xFFFF)
        return false;

    unsigned long key = result;
    return glplatTextureLoad((PlatTexture*)lbl_806E1F08->fn_802CE294(&key));
}

u32 glTextureGetWidth()
{
    return glplatTextureGetWidth();
}

u32 glTextureGetHeight()
{
    return glplatTextureGetHeight();
}

int glTextureGetNumBits(int component)
{
    return glplatTextureGetNumBits(component);
}

void glTextureAdd(unsigned long texture, const void* buffer,
    unsigned long length, void* resourceInterface)
{
    ResourceInterface_802CC094* resource = (ResourceInterface_802CC094*)resourceInterface;
    if (fn_802D3A08(buffer, length))
    {
        fn_802D3A34(buffer, length, resource);
    }
    else
    {
        PlatTexture* pTex = fn_8036BBDC(
            texture, buffer, length, resourceInterface);
        resource->m_inventory->fn_802D2324(texture, pTex);
        lbl_806E1F08->fn_802CE360(pTex);
    }
}

void glTextureReplace(unsigned long texture, const void* buffer,
    unsigned long length)
{
    glplatTextureReplace(glx_GetTex(texture), buffer, length);
}

extern "C" bool fn_802CDD78(
    void* data, unsigned long size, void* allocator, int nParam)
{
    fn_8036BBD4(data, allocator);
    BundleHeader* header = (BundleHeader*)data;
    int numTextures = header->numTextures;
    glTexBundleDict* dict = (glTexBundleDict*)(header + 1);
    unsigned char* textureData = (unsigned char*)(dict + numTextures);
    ResourceInterface_802CC094* resource = (ResourceInterface_802CC094*)allocator;
    GLInventory* inventory = resource->m_inventory;

    glTexBundleDict* entry = dict;
    for (int i = 0; i < numTextures; ++i, ++entry)
    {
        GXTextureHeader* texture = (GXTextureHeader*)(textureData + entry->offset);
        if (!fn_802D3A08(texture, entry->fileSize))
        {
            PlatTexture* pTex = inventory->fn_802D2370(entry->hash);
            if (pTex != 0)
            {
                switch (nParam)
                {
                case 2:
                    glplatTextureReplace(pTex, texture, entry->fileSize);
                    break;
                }
            }
            else
            {
                unsigned long handle = entry->hash;
                fn_802C8284(handle);
                pTex = fn_8036BBC0(entry, texture, allocator);
                resource->m_inventory->fn_802D2324(handle, pTex);
                lbl_806E1F08->fn_802CE360(pTex);
                fn_802C8288();
            }
        }
    }

    entry = dict;
    for (int i = 0; i < numTextures; ++i, ++entry)
    {
        const void* texture = textureData + entry->offset;
        if (fn_802D3A08(texture, entry->fileSize))
        {
            fn_802D3A34(texture, entry->fileSize, resource);
        }
    }

    fn_8036BBD8(data);
    return true;
}

extern "C" void fn_802CDEC0(unsigned long count)
{
    lbl_806E1F08 = new (8, false) TextureManager_802CDF0C(count);
}

TextureManager_802CDF0C* fn_802CDF0C()
{
    return lbl_806E1F08;
}

void fn_802CDF14(unsigned long texture, PlatTexture* platformTexture,
    void* allocator)
{
    ResourceInterface_802CC094* resource = (ResourceInterface_802CC094*)allocator;
    resource->m_inventory->fn_802D2324(texture, platformTexture);
    lbl_806E1F08->fn_802CE360(platformTexture);
}

extern "C" void fn_802CDF5C(PlatTexture* texture)
{
    TextureManager_802CDF0C* manager = lbl_806E1F08;
    u32 index = texture->unknown0E;
    manager->m_08->UnidentifiedAddEnd((u16)index);
    manager->m_04[index] = 0;
    texture->unknown0E = 0xFFFF;
    texture->ClearData();
}

extern "C" unsigned long fn_802CDFCC(unsigned long texture)
{
    return lbl_806E1F08->fn_802CE1B8(texture);
}
