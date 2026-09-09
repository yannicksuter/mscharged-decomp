#include "NL/gl/glMemoryInit.h"

#include "NL/gl/glMemory.h"
#include "NL/gl/glTexture.h"

bool glInitMemory(const GLMemoryConfig* config)
{
    if (!glxInitMemory(config->mFrameMemSize1, config->mFrameMemSize2))
    {
        return false;
    }

    glSetCurrentResourcePool(glCreateResourcePool(config->mResourceRequirements, config->mNumResourceRequirements, "Global"));
    glInitTextureManager(config->mMaxTextures);
    return true;
}
