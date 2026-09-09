#ifndef NL_GLX_GX_MATERIAL_PROGRAM_BASE_H
#define NL_GLX_GX_MATERIAL_PROGRAM_BASE_H

class GLView;
struct glModelPacket;

struct GXMaterialParameter
{
    /* 0x00 */ unsigned long hash;
    /* 0x04 */ unsigned long metadata;
    /* 0x08 */ unsigned long offset;
}; // size: 0xC

template <class Derived>
class GXMaterialProgramImpl
{
public:
    virtual void Activate(GLView* view);
    virtual void Deactivate();
    virtual void Configure() = 0;
    virtual void Prepare(const glModelPacket* packet);
    virtual void Draw(const glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters() = 0;
    virtual void Initialize() = 0;

    /* 0x04 */ unsigned long programHash;
    /* 0x08 */ unsigned long parameterDataSize;
    /* 0x0C */ unsigned long parameterCount;

protected:
    ~GXMaterialProgramImpl() { }
}; // size: 0x10

#endif // NL_GLX_GX_MATERIAL_PROGRAM_BASE_H
