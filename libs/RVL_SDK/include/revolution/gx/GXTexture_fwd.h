#ifndef RVL_SDK_GX_TEXTURE_FWD_H
#define RVL_SDK_GX_TEXTURE_FWD_H

#include <revolution/gx/GXPublicTypes.h>
#include <revolution/gx/GXTextureTypes.h>

typedef struct _GXTexObj GXTexObj;
typedef struct _GXTlutObj GXTlutObj;

#ifdef __cplusplus
extern "C" {
#endif

void GXInitTexObj(const GXTexObj* object, void* image,
                  unsigned short width, unsigned short height, GXTexFmt format,
                  GXTexWrapMode wrapS, GXTexWrapMode wrapT, GXBool mipmap);
void GXInitTexObjCI(GXTexObj* object, void* image,
                    unsigned short width, unsigned short height, GXCITexFmt format,
                    GXTexWrapMode wrapS, GXTexWrapMode wrapT, GXBool mipmap,
                    unsigned long tlutName);
void GXInitTexObjLOD(GXTexObj* object, GXTexFilter minFilter, GXTexFilter magFilter,
                     float minLod, float maxLod, float lodBias, GXBool biasClamp,
                     GXBool edgeLod, GXAnisotropy maxAnisotropy);
void GXInitTexObjWrapMode(const GXTexObj* object,
                          GXTexWrapMode wrapS, GXTexWrapMode wrapT);
void GXInitTexObjTlut(const GXTexObj* object, unsigned long tlutName);
void GXLoadTexObj(const GXTexObj* object, GXTexMapID textureMap);
void GXInitTlutObj(const GXTlutObj* object, void* palette,
                   GXTlutFmt format, unsigned short entries);
void GXLoadTlut(const GXTlutObj* object, unsigned long tlutName);
void GXInvalidateTexAll(void);

#ifdef __cplusplus
}
#endif

#endif // RVL_SDK_GX_TEXTURE_FWD_H
