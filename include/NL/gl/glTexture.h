#ifndef NL_GL_GLTEXTURE_H
#define NL_GL_GLTEXTURE_H

#include "types.h"

bool glTextureLoad(unsigned long texture);
u32 glTextureGetWidth();
u32 glTextureGetHeight();
int glTextureGetNumBits(int component);

#endif // NL_GL_GLTEXTURE_H
