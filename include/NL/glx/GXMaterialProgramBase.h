#ifndef NL_GLX_GX_MATERIAL_PROGRAM_BASE_H
#define NL_GLX_GX_MATERIAL_PROGRAM_BASE_H

#include "NL/gl/glMaterialProgram.h"

template <class Derived>
class GXMaterialProgramImpl : public GLMaterialProgram
{
public:
    virtual void Activate(GLView* view);
    virtual void Deactivate();
    virtual void Configure(glModelPacket* packet) = 0;
    virtual void Prepare(glModelPacket* packet);
    virtual void Draw(const glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters() = 0;
    virtual void Initialize() = 0;

protected:
    ~GXMaterialProgramImpl() { }
}; // size: 0x10

#endif // NL_GLX_GX_MATERIAL_PROGRAM_BASE_H
