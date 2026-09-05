#ifndef _GLXTEXTURE_H_
#define _GLXTEXTURE_H_

#include "types.h"
#include "NL/gl/TextureManager_802CDF0C.h"

#include <string.h>

class GLView;

typedef unsigned long (*glxTextureLoadCallback_t)(unsigned long);

enum eGXTextureFormat
{
    GXTex_RGB565 = 0,
    GXTex_RGB5A3 = 1,
    GXTex_CMPR = 2,
    GXTex_RGBA8 = 3,
    GXTex_I8 = 4,
    GXTex_I4 = 5,
    GXTex_A8 = 6,
    GXTex_IA8 = 7,
    GXTex_CI8 = 8,
    GXTex_Num = 9,
};

struct GXTextureHeader
{
    /* 0x00 */ unsigned long numLevels;
    /* 0x04 */ eGXTextureFormat format;
    /* 0x08 */ unsigned char numBits[4];
    /* 0x0C */ unsigned char missingTexture;
    /* 0x0D */ unsigned char _pad0D;
    /* 0x0E */ unsigned short width;
    /* 0x10 */ unsigned short height;
    /* 0x12 */ unsigned char _pad12[2];
    /* 0x14 */ unsigned long numEntries;
    /* 0x18 */ unsigned long _pad18[2];
};

struct BundleEntry
{
    /* 0x00 */ unsigned long hash;
    /* 0x04 */ unsigned long offset;
    /* 0x08 */ unsigned long fileSize;
    /* 0x0C */ unsigned long pad;
};

struct glTexBundleDict : public BundleEntry
{
};

struct BundleHeader
{
    unsigned long magic;
    unsigned long numTextures;
    unsigned long pad1;
    unsigned long pad2;
};

class PlatTexture
{
public:
    PlatTexture()
    {
        m_Width = 0;
        m_Height = 0;
        m_Levels = 0;
        m_MaxLevel = 0;
        m_Format = GXTex_Num;
        m_nPaletteEntries = 0;
        m_bMissingTexture = false;
        m_SwizzledData = 0;
        m_LinearData = 0;
        m_PaletteData = 0;
        memset(m_TexObj, 0, sizeof(m_TexObj));
        memset(m_TlutObj, 0, sizeof(m_TlutObj));
        memset(m_Bits, 0xFF, sizeof(m_Bits));
        unknown0E = 0xFFFF;
    }

    ~PlatTexture();

    void ClearData();
    void Prepare();
    void Swizzle(bool bDeleteLinear);
    void Create(int width, int height, eGXTextureFormat format,
        void* allocator, int numLevels, bool bLinearData,
        bool bNewResourceMemory);
    void CreateWithMemory(int width, int height, eGXTextureFormat format,
        int numLevels, const void* pTextureData);

    /* 0x00 */ u16 m_Width;
    /* 0x02 */ u16 m_Height;
    /* 0x04 */ u8 m_Levels;
    /* 0x05 */ u8 m_MaxLevel;
    /* 0x06 */ u8 _pad06[2];
    /* 0x08 */ eGXTextureFormat m_Format;
    /* 0x0C */ s16 m_nPaletteEntries;
    /* 0x0E */ u16 unknown0E;
    /* 0x10 */ bool m_bMissingTexture;
    /* 0x11 */ u8 _pad11[3];
    /* 0x14 */ void* m_SwizzledData;
    /* 0x18 */ void* m_LinearData;
    /* 0x1C */ u16* m_PaletteData;
    /* 0x20 */ u8 m_Bits[4];
    /* 0x24 */ u32 m_TexObj[8];
    /* 0x44 */ u32 m_TlutObj[3];
};

PlatTexture* glx_GetTex(unsigned long handle);
PlatTexture* glx_CreatePlatTexture(void* allocator);
PlatTexture* glx_MakeTexture(GXTextureHeader* header,
    void* allocator, unsigned long handle);
void glplatTextureReplace(PlatTexture* pTex, const void* textureData,
    unsigned long size);
bool glplatBeginLoadTextureBundle(const char* filename,
    void (*callback)(void*, unsigned long, void*), void* param);
bool glplatLoadTextureBundle(
    const char* filename, void* allocator);
bool glplatTextureLoad(PlatTexture* texture);
int glplatTextureGetNumBits(int component);
u32 glplatTextureGetHeight();
u32 glplatTextureGetWidth();
void glxInitTex();
glxTextureLoadCallback_t glx_SetLoadCallback(
    glxTextureLoadCallback_t callback);

extern TextureManager_802CDF0C* lbl_806E1F08;

extern "C" void fn_802CDEC0(unsigned long);
TextureManager_802CDF0C* fn_802CDF0C();
void fn_802CDF14(unsigned long texture, PlatTexture* platformTexture,
    void* allocator);
extern "C" void fn_802CDF5C(PlatTexture* texture);
extern "C" PlatTexture* fn_8036BBC0(glTexBundleDict* entry,
    GXTextureHeader* header, void* allocator);
extern "C" void fn_8036BBD4(void* data, void* allocator);
extern "C" void fn_8036BBD8(void* data);
extern "C" PlatTexture* fn_8036BBDC(unsigned long handle,
    const void* textureData, unsigned long size, void* allocator);

#endif // _GLXTEXTURE_H_
