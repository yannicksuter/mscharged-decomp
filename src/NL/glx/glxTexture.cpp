#include "NL/glx/glxTexture.h"

#include "NL/gc/gcSwizzler.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/nlFile.h"
#include "NL/nlList.h"
#include "NL/nlMemory.h"

#include <string.h>

class TextureManager_802CDF0C;

extern "C"
{
    MemoryAllocator* fn_802CC094();
    void* fn_802CC0A4(
        unsigned long size, int memoryType, MemoryAllocator* allocator);
    bool fn_802CDD78(
        void* data, unsigned long size, MemoryAllocator* allocator, int);
    TextureManager_802CDF0C* fn_802CDF0C();
    unsigned long fn_802CE1B8(
        TextureManager_802CDF0C* manager, unsigned long texture);
    PlatTexture* fn_802CE294(
        TextureManager_802CDF0C* manager, const unsigned long* texture);
    PlatTexture* fn_802CE2B8(
        TextureManager_802CDF0C* manager,
        UnidentifiedTextureState* texture);
    int fn_80383478(const char* format, ...);
    void fn_8004F594(int category, const char* format, ...);

    void DCStoreRange(void* address, unsigned long length);
    void GXInitTlutObj(
        void* object, void* palette, int format, unsigned short entries);
    void GXInitTexObjCI(void* object, void* image, unsigned short width,
        unsigned short height, int format, int wrapS, int wrapT,
        unsigned char mipmap, unsigned long tlutName);
    void GXInitTexObj(void* object, void* image, unsigned short width,
        unsigned short height, int format, int wrapS, int wrapT,
        unsigned char mipmap);
    void GXInitTexObjLOD(void* object, int minFilter, int magFilter,
        float minLod, float maxLod, float lodBias, unsigned char biasClamp,
        unsigned char edgeLod, int maxAnisotropy);
    void GXInitTexObjWrapMode(void* object, int wrapS, int wrapT);
    void GXInitTexObjTlut(void* object, unsigned long tlutName);
    void GXLoadTlut(void* object, unsigned long tlutName);
    void GXLoadTexObj(void* object, unsigned long textureMap);
    void GXInvalidateTexAll();
}

struct TweakValueBool_804F4578
{
    /* 0x00 */ void* mVTable;
    /* 0x04 */ const char* mName;
    /* 0x08 */ unsigned char mUnidentified08;
    /* 0x09 */ unsigned char mUnidentified09;
    /* 0x0A */ bool mValue;
};

extern TweakValueBool_804F4578 gbReduceTextures;

static glxTextureLoadCallback_t glxTextureLoad_cb;
static unsigned long nGridMemory;
static bool glx_bGridMode;

static nlListContainer<PlatTexture*> gridTextures;
static PlatTexture texobj;

glxTextureLoadCallback_t glx_SetLoadCallback(
    glxTextureLoadCallback_t callback)
{
    glxTextureLoadCallback_t previous = glxTextureLoad_cb;
    glxTextureLoad_cb = callback;
    return previous;
}

static PlatTexture* glx_MakeGridTexture(int width, int height)
{
    fn_80383478("Creating grid texture %d x %d\n", width, height);

    unsigned char bits[4] = { 5, 6, 5, 0 };
    PlatTexture* texture = new (nlMalloc(sizeof(PlatTexture), 8, false)) PlatTexture();
    texture->Create(width, height, GXTex_RGB565, fn_802CC094(), 1, true, true);
    memcpy(texture->m_Bits, bits, sizeof(texture->m_Bits));

    unsigned short gridColor = 0xFFFF;
    int x;
    int y;
    unsigned short* data = (unsigned short*)texture->m_LinearData;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            int gridX = x / 8;
            if ((y / 8) & 1)
            {
                data[y * width + x] = (gridX & 1) == 0 ? 0 : gridColor;
            }
            else
            {
                data[y * width + x] = (gridX & 1) ? 0 : gridColor;
            }
        }
    }

    nGridMemory += height * (width << 1) + sizeof(PlatTexture);
    fn_8004F594(1, "grid [%d %d] now using %uKB\n", width, height, nGridMemory / 1024);

    texture->Swizzle(true);
    texture->Prepare();
    return texture;
}

static PlatTexture* glx_GetGridTexture(int width, int height)
{
    width /= 2;
    height /= 2;

    if ((width & (width - 1)) != 0 || (height & (height - 1)) != 0)
    {
        return 0;
    }
    if (width < 16 || height < 16)
    {
        return 0;
    }

    ListEntry<PlatTexture*>* current = gridTextures.m_Head;
    while (current != 0)
    {
        PlatTexture* texture = current->entry;
        if (width == texture->m_Width && height == texture->m_Height)
        {
            return texture;
        }
        current = current->next;
    }

    PlatTexture* texture = glx_MakeGridTexture(width, height);
    ListEntry<PlatTexture*>* entry = new (nlMalloc(sizeof(ListEntry<PlatTexture*>), 8, false))
        ListEntry<PlatTexture*>(texture);
    nlListAddStart(&gridTextures.m_Head, entry, &gridTextures.m_Tail);
    return gridTextures.m_Head->entry;
}

bool glplatTextureLoad(PlatTexture* texture)
{
    if (texture == 0)
    {
        memset(&texobj, 0, sizeof(texobj));
        return false;
    }

    memcpy(&texobj, texture, sizeof(texobj));
    return true;
}

u32 glplatTextureGetWidth()
{
    return texobj.m_Width;
}

u32 glplatTextureGetHeight()
{
    return texobj.m_Height;
}

int glplatTextureGetNumBits(int component)
{
    if (texobj.m_Bits[component] == 0xFF)
    {
        unsigned long format = texobj.m_Format;
        unsigned char bits[4] = { 0, 0, 0, 0 };
        switch (format)
        {
        case GXTex_RGB565:
            bits[0] = 5;
            bits[1] = 6;
            bits[2] = 5;
            bits[3] = 0;
            break;
        case GXTex_RGB5A3:
            bits[0] = 5;
            bits[1] = 5;
            bits[2] = 5;
            bits[3] = 3;
            break;
        case GXTex_CMPR:
            break;
        case GXTex_RGBA8:
            bits[0] = 8;
            bits[1] = 8;
            bits[2] = 8;
            bits[3] = 8;
            break;
        case GXTex_I8:
            bits[0] = 8;
            bits[1] = 0;
            bits[2] = 0;
            bits[3] = 0;
            break;
        case GXTex_I4:
            bits[0] = 4;
            bits[1] = 0;
            bits[2] = 0;
            bits[3] = 0;
            break;
        case GXTex_A8:
            break;
        case GXTex_IA8:
            bits[0] = 8;
            bits[1] = 0;
            bits[2] = 0;
            bits[3] = 8;
            break;
        }
        return bits[component];
    }

    return texobj.m_Bits[component];
}

PlatTexture* glx_CreatePlatTexture(MemoryAllocator* allocator)
{
    return new (fn_802CC0A4(sizeof(PlatTexture), 0, allocator)) PlatTexture();
}

void PlatTexture::ClearData()
{
    if (this != 0 && m_LinearData != 0)
    {
        nlFree(m_LinearData);
        m_LinearData = 0;
    }
}

PlatTexture::~PlatTexture()
{
    if (m_LinearData != 0)
    {
        nlFree(m_LinearData);
        m_LinearData = 0;
    }
}

void PlatTexture::CreateWithMemory(int width, int height,
    eGXTextureFormat format, int numLevels, const void* textureData)
{
    m_Width = width;
    m_Height = height;
    m_Levels = numLevels;
    m_MaxLevel = numLevels;
    m_Format = format;
    m_SwizzledData = (void*)textureData;
    m_LinearData = 0;
}

void PlatTexture::Create(int width, int height, eGXTextureFormat format,
    MemoryAllocator* allocator, int numLevels, bool linearData,
    bool newResourceMemory)
{
    if (m_LinearData != 0)
    {
        nlFree(m_LinearData);
        m_LinearData = 0;
    }

    m_Width = width;
    m_Height = height;
    m_Levels = numLevels;
    m_MaxLevel = numLevels;
    m_Format = format;

    unsigned long textureSize = GCTextureSize(format, width, height, numLevels, -1);
    if (newResourceMemory)
    {
        m_SwizzledData = nlMalloc(textureSize, 32, false);
    }
    else
    {
        m_SwizzledData = fn_802CC0A4(textureSize, 4, allocator);
    }

    if (linearData)
    {
        m_LinearData = nlMalloc(textureSize, 32, false);
    }
    else
    {
        m_LinearData = 0;
    }
}

void PlatTexture::Swizzle(bool deleteLinear)
{
    GCSwizzle(m_SwizzledData, m_LinearData, m_Width, m_Height, m_Format, false);
    if (deleteLinear)
    {
        nlFree(m_LinearData);
        m_LinearData = 0;
    }
}

static inline int* glx_GetGXFormatTable()
{
    static int gxFormat[9] = { 4, 5, 14, 6, 1, 0, 1, 3, 9 };
    return gxFormat;
}

void PlatTexture::Prepare()
{
    DCStoreRange(m_SwizzledData,
        GCTextureSize(m_Format, m_Width, m_Height, m_Levels, -1));
    if (m_nPaletteEntries > 0)
    {
        DCStoreRange(m_PaletteData, m_nPaletteEntries * 2);
        GXInitTlutObj(
            m_TlutObj, m_PaletteData, 2, m_nPaletteEntries);
    }

    if (m_Format == GXTex_CI8)
    {
        GXInitTexObjCI(m_TexObj, m_SwizzledData, m_Width, m_Height, glx_GetGXFormatTable()[m_Format], 0, 0, m_Levels > 1, 0);
        GXInitTexObjLOD(m_TexObj, m_Levels == 1 ? 1 : 3, 1, 0.0f, (float)(m_MaxLevel - 1), 0.0f, false, false, 0);
        return;
    }

    GXInitTexObj(m_TexObj, m_SwizzledData, m_Width, m_Height, glx_GetGXFormatTable()[m_Format], 0, 0, m_Levels > 1);
    GXInitTexObjLOD(m_TexObj, m_Levels == 1 ? 1 : 5, 1, 0.0f, (float)(m_MaxLevel - 1), 0.0f, false, false, 0);
}

PlatTexture* glx_MakeTexture(GXTextureHeader* header,
    MemoryAllocator* allocator, unsigned long texhandle)
{
    PlatTexture* pTex;
    unsigned char* textureData;
    int width;
    int height;
    eGXTextureFormat format;
    int numLevels;
    unsigned long numEntries;
    int textureSize;

    width = header->width;
    height = header->height;
    numLevels = header->numLevels;
    textureData = (unsigned char*)(header + 1);

    if (gbReduceTextures.mValue)
    {
        if (numLevels > 2)
        {
            textureData += GCTextureSize(
                header->format, width, height, 2, -1);
            width /= 4;
            height /= 4;
            numLevels -= 2;
        }
        else if (numLevels > 1)
        {
            textureData += GCTextureSize(
                header->format, width, height, 1, -1);
            width /= 2;
            height /= 2;
            numLevels -= 1;
        }
    }

    textureSize = GCTextureSize(
        header->format, width, height, numLevels, texhandle);
    pTex = new (fn_802CC0A4(sizeof(PlatTexture), 0, allocator)) PlatTexture();

    format = header->format;
    pTex->Create(width, height, format, allocator, numLevels, false, false);
    memcpy(pTex->m_Bits, header->numBits, sizeof(pTex->m_Bits));
    pTex->m_bMissingTexture = header->missingTexture ? true : false;

    numEntries = header->numEntries;
    if (numEntries != 0)
    {
        pTex->m_PaletteData =
            (u16*)fn_802CC0A4(numEntries * 2, 4, allocator);
        pTex->m_nPaletteEntries = numEntries;
        memcpy(pTex->m_PaletteData, textureData + textureSize,
            header->numEntries * 2);
    }

    memcpy(pTex->m_SwizzledData, textureData, textureSize);
    pTex->Prepare();
    return pTex;
}

bool glplatBeginLoadTextureBundle(const char* filename,
    void (*callback)(void*, unsigned long, void*), void* param)
{
    return nlLoadEntireFileAsync(filename, callback, param, 32, AllocateEnd, 0, 0, 0)
        != 0;
}

bool glplatLoadTextureBundle(
    const char* filename, MemoryAllocator* allocator)
{
    bool result;
    void* data;
    unsigned long size;
    data = nlLoadEntireFile(filename, &size, 32, AllocateStart, 0, 0, 0);
    result = fn_802CDD78(data, size, allocator, 0);
    delete[] (unsigned char*)data;
    return result;
}

extern "C" PlatTexture* fn_8036BBC0(glTexBundleDict* entry,
    GXTextureHeader* header, MemoryAllocator* allocator)
{
    return glx_MakeTexture(header, allocator, entry->hash);
}

extern "C" void fn_8036BBD8()
{
    GXInvalidateTexAll();
}

extern "C" PlatTexture* fn_8036BBDC(unsigned long handle,
    GXTextureHeader* header, unsigned long, MemoryAllocator* allocator)
{
    return glx_MakeTexture(header, allocator, handle);
}

void glplatTextureReplace(PlatTexture* pTex, const void* textureData,
    unsigned long size)
{
    const GXTextureHeader* pHeader = (GXTextureHeader*)textureData;
    const unsigned char* src =
        (const unsigned char*)textureData + sizeof(GXTextureHeader);
    int width = pHeader->width;
    int height = pHeader->height;
    int numLevels = pHeader->numLevels;

    if (gbReduceTextures.mValue)
    {
        if (numLevels > 2)
        {
            src += GCTextureSize(
                pHeader->format, width, height, 2, -1);
        }
        else if (numLevels > 1)
        {
            src += GCTextureSize(
                pHeader->format, width, height, 1, -1);
        }
    }

    memcpy(pTex->m_SwizzledData, src,
        GCTextureSize(pTex->m_Format, pTex->m_Width, pTex->m_Height,
            pTex->m_Levels, -1));

    if (pHeader->numEntries != 0)
    {
        int paletteOffset = GCTextureSize(pTex->m_Format, pTex->m_Width,
            pTex->m_Height, pTex->m_Levels, -1);
        memcpy(pTex->m_PaletteData, src + paletteOffset,
            pHeader->numEntries * 2);
    }

    pTex->Prepare();
}

void glxInitTex()
{
}

extern "C" void fn_8036BE88(int textureMap, void* textureData)
{
    UnidentifiedTextureState* textureState =
        (UnidentifiedTextureState*)textureData;
    TextureManager_802CDF0C* textureManager;
    PlatTexture* pTex;
    static unsigned long missingTexture =
        glGetTexture("font/fixedWidthMedium");

    textureManager = fn_802CDF0C();
    pTex = fn_802CE2B8(textureManager, textureState);
    if (pTex == 0)
    {
        unsigned long texture =
            fn_802CE1B8(textureManager, missingTexture);
        pTex = fn_802CE294(textureManager, &texture);
    }

    if (glx_bGridMode)
    {
        PlatTexture* gridTexture =
            glx_GetGridTexture(pTex->m_Width, pTex->m_Height);
        if (gridTexture != 0)
        {
            pTex = gridTexture;
        }
    }

    unsigned char flags = textureState->flags;
    bool wrapS = !(flags & 1);
    bool wrapT = !(flags & 2);

    unsigned long textureObject[8];
    memcpy(textureObject, pTex->m_TexObj, sizeof(textureObject));
    GXInitTexObjWrapMode(textureObject, wrapS, wrapT);

    if (pTex->m_nPaletteEntries != 0)
    {
        unsigned long tlutObject[3];
        memcpy(tlutObject, pTex->m_TlutObj, sizeof(tlutObject));
        GXInitTexObjTlut(textureObject, textureMap);
        GXLoadTlut(tlutObject, textureMap);
    }

    GXLoadTexObj(textureObject, textureMap);
}
