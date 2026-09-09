#ifndef NL_GL_GLTEXTURE_H
#define NL_GL_GLTEXTURE_H

#include "types.h"

class PlatTexture;

bool glTextureLoad(unsigned long texture);
u32 glTextureGetWidth();
u32 glTextureGetHeight();
int glTextureGetNumBits(int component);
void glTextureAdd(unsigned long texture, const void* buffer, unsigned long length, void* resourceInterface);
void glTextureReplace(unsigned long texture, const void* buffer, unsigned long length);

bool glEndLoadTextureBundle(
    void* data, unsigned long size, void* allocator, int nParam);
unsigned long glGetTextureIndex(unsigned long texture);

void glInitTextureManager(unsigned long count);
void glRegisterTexture(unsigned long texture, PlatTexture* platformTexture, void* allocator);
void glReleaseTexture(PlatTexture* texture);

extern u32 gDiffuseTextureSemantic;
extern u32 gDetailTextureSemantic;
extern u32 gSpecularTextureSemantic;
extern u32 gBumpMapTextureSemantic;
extern u32 gNormalMapTextureSemantic;
extern u32 gShadowTextureSemantic;
extern u32 gSelfIllumTextureSemantic;
extern u32 gGlossTextureSemantic;
extern u32 gRampTextureSemantic;
extern u32 gMaskTextureSemantic;
extern u32 gWhiteTextureID;

#endif // NL_GL_GLTEXTURE_H
