#ifndef NL_GL_GLMATERIALPROGRAM_H
#define NL_GL_GLMATERIALPROGRAM_H

class GLView;
struct glModelPacket;
struct GXMaterialParameter
{
    /* 0x00 */ unsigned long hash;
    /* 0x04 */ unsigned long metadata;
    /* 0x08 */ unsigned long offset;
}; // size: 0xC

class GLMaterialProgram
{
public:
    virtual void Activate(GLView* view) = 0;
    virtual void Deactivate() = 0;
    virtual void Configure(glModelPacket* packet) = 0;
    virtual void Prepare(const glModelPacket* packet) = 0;
    virtual void Draw(const glModelPacket* packet) = 0;
    virtual const GXMaterialParameter* GetParameters() = 0;
    virtual void Initialize() = 0;

    /* 0x04 */ unsigned long programHash;
    /* 0x08 */ unsigned long parameterDataSize;
    /* 0x0C */ unsigned long parameterCount;
}; // size: 0x10

template <typename R, typename A0, typename A1>
class Function2;

typedef Function2<bool, unsigned long&, void*&> MaterialProgramCallback;

void glRegisterMaterialProgram(void* program, unsigned long hash);
void* glGetMaterialProgram(unsigned long hash);
void glForEachMaterialProgram(MaterialProgramCallback* callback);

#endif // NL_GL_GLMATERIALPROGRAM_H
