#include "NL/gl/glTexture.h"

#include "Game/GL/GLInventory.h"
#include "Game/GL/GLTextureAnim.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

glTextureManager* gTextureManager;
u32 gDiffuseTextureSemantic = nlStringLowerHash("NLG_DIFFUSE");
u32 gDetailTextureSemantic = nlStringLowerHash("NLG_DETAIL");
u32 gSpecularTextureSemantic = nlStringLowerHash("NLG_SPECULAR");
u32 gBumpMapTextureSemantic = nlStringLowerHash("NLG_BUMPMAP");
u32 gNormalMapTextureSemantic = nlStringLowerHash("NLG_NORMALMAP");
u32 gShadowTextureSemantic = nlStringLowerHash("NLG_SHADOW");
u32 gSelfIllumTextureSemantic = nlStringLowerHash("NLG_SELFILLUM");
u32 gGlossTextureSemantic = nlStringLowerHash("NLG_GLOSS");
u32 gRampTextureSemantic = nlStringLowerHash("NLG_RAMP");
u32 gMaskTextureSemantic = nlStringLowerHash("NLG_MASK");
u32 gWhiteTextureID = nlStringLowerHash("global/white");

bool glTextureLoad(unsigned long texture)
{
    unsigned long result = gTextureManager->GetTextureIndex(texture);
    if (result == 0xFFFF)
        return false;

    unsigned long key = result;
    return glplatTextureLoad((PlatTexture*)gTextureManager->GetTextureAtIndex(&key));
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
    GLResourcePool* resource = (GLResourcePool*)resourceInterface;
    if (glIsTextureAnim(buffer, length))
    {
        glAddTextureAnim(buffer, length, resource);
    }
    else
    {
        PlatTexture* pTex = glplatTextureAdd(
            texture, buffer, length, resourceInterface);
        resource->m_inventory->AddTexture(texture, pTex);
        gTextureManager->RegisterTexture(pTex);
    }
}

void glTextureReplace(unsigned long texture, const void* buffer,
    unsigned long length)
{
    glplatTextureReplace(glx_GetTex(texture), buffer, length);
}

bool glEndLoadTextureBundle(
    void* data, unsigned long size, void* allocator, int existingTextureMode)
{
    GLResourcePool* resource = (GLResourcePool*)allocator;
    BundleHeader* header = (BundleHeader*)data;
    glplatBeginTextureBundle(header, resource);
    int numTextures = header->numTextures;
    glTexBundleDict* dict = (glTexBundleDict*)(header + 1);
    unsigned char* textureData = (unsigned char*)(dict + numTextures);
    GLInventory* inventory = resource->m_inventory;

    int i;
    for (i = 0; i < numTextures; ++i)
    {
        glTexBundleDict* entry = dict + i;
        GXTextureHeader* texture = (GXTextureHeader*)(textureData + entry->offset);
        if (!glIsTextureAnim(texture, entry->fileSize))
        {
            PlatTexture* pTex = inventory->GetTexture(entry->hash);
            if (pTex != 0)
            {
                switch (existingTextureMode)
                {
                case 0:
                case 1:
                    break;
                case 2:
                    glplatTextureReplace(pTex, texture, entry->fileSize);
                    break;
                }
            }
            else
            {
                unsigned long handle = entry->hash;
                glBeginResource(handle);
                pTex = glplatTextureAddFromBundle(entry, texture, allocator);
                resource->m_inventory->AddTexture(handle, pTex);
                gTextureManager->RegisterTexture(pTex);
                glEndResource();
            }
        }
    }

    for (i = 0; i < numTextures; ++dict, ++i)
    {
        const void* texture = textureData + dict->offset;
        if (glIsTextureAnim(texture, dict->fileSize))
        {
            glAddTextureAnim(texture, dict->fileSize, resource);
        }
    }

    glplatEndTextureBundle(header);
    return true;
}

void glInitTextureManager(unsigned long count)
{
    gTextureManager = new (8, false) glTextureManager(count);
}

glTextureManager* glGetTextureManager()
{
    return gTextureManager;
}

void glRegisterTexture(unsigned long texture, PlatTexture* platformTexture,
    void* allocator)
{
    GLResourcePool* resource = (GLResourcePool*)allocator;
    resource->m_inventory->AddTexture(texture, platformTexture);
    gTextureManager->RegisterTexture(platformTexture);
}

void glReleaseTexture(PlatTexture* texture)
{
    glTextureManager* manager = gTextureManager;
    u32 index = texture->m_TextureIndex;
    manager->mFreeIndices->AddEnd((u16)index);
    manager->mTextures[index] = 0;
    texture->m_TextureIndex = 0xFFFF;
    texture->ClearData();
}

unsigned long glGetTextureIndex(unsigned long texture)
{
    return gTextureManager->GetTextureIndex(texture);
}
