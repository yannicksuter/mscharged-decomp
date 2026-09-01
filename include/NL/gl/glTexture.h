#ifndef NL_GL_GLTEXTURE_H
#define NL_GL_GLTEXTURE_H

#include "types.h"

bool glTextureLoad(unsigned long texture);
u32 glTextureGetWidth();
u32 glTextureGetHeight();
int glTextureGetNumBits(int component);
void* glTextureAdd(unsigned long texture, const void* buffer, unsigned long length, void* resourceInterface);
void glTextureReplace(unsigned long texture, const void* buffer, unsigned long length);

#endif // NL_GL_GLTEXTURE_H
