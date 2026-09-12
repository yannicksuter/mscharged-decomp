#ifndef NL_GLX_GX_VERTEX_COLOUR_MATERIAL_PROGRAM_H
#define NL_GLX_GX_VERTEX_COLOUR_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

class GXVertexColourMaterialProgram : public GXMaterialProgramImpl<GXVertexColourMaterialProgram>
{
public:
    GXVertexColourMaterialProgram();
    virtual ~GXVertexColourMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXVertexColourMaterialProgram* Instance;
    static bool Initialized;
};

#endif // NL_GLX_GX_VERTEX_COLOUR_MATERIAL_PROGRAM_H
