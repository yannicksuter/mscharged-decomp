#ifndef NL_GLX_FRESNEL_TEXTURE_H
#define NL_GLX_FRESNEL_TEXTURE_H

#include "NL/gl/glModel.h"
#include "NL/glx/glxTexture.h"

inline void glxBindFresnelTexture(int textureMap, unsigned long texture,
    int ramp, unsigned long (&indices)[5], bool& initialized)
{
    if (!initialized)
    {
        indices[0] = 0xFFFF;
        indices[1] = 0xFFFF;
        indices[2] = 0xFFFF;
        indices[3] = 0xFFFF;
        indices[4] = 0xFFFF;
        initialized = true;
    }
    if (indices[ramp] == 0xFFFF || indices[ramp] == 0)
        indices[ramp] = glGetTextureManager()->GetTextureIndex(texture);
    glTextureBinding binding;
    binding.texture = texture;
    binding.flags = 0;
    binding.SetWrapS(true);
    binding.SetWrapT(true);
    binding.unknown07 = 0;
    binding.textureIndex = indices[ramp];
    glx_BindTexture(textureMap, &binding);
}

#endif // NL_GLX_FRESNEL_TEXTURE_H
