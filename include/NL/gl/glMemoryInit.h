#ifndef NL_GL_GLMEMORYINIT_H
#define NL_GL_GLMEMORYINIT_H

struct GLMemoryRequirement;

struct GLMemoryConfig
{
    unsigned long mFrameMemSize1;
    unsigned long mFrameMemSize2;
    const GLMemoryRequirement* mResourceRequirements;
    int mNumResourceRequirements;
    unsigned long mMaxTextures;
}; // size 0x14

bool glInitMemory(const GLMemoryConfig* config);

#endif // NL_GL_GLMEMORYINIT_H
