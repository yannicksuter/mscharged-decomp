#ifndef NL_GLX_GX_MOVIE_MATERIAL_PROGRAM_H
#define NL_GLX_GX_MOVIE_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"
#include "NL/nlColour.h"

struct GXMovieParameters
{
    /* 0x00 */ glTextureBinding texture;
    /* 0x08 */ nlFloatColour tint;
}; // size: 0x18

class GXMovieMaterialProgram : public GXMaterialProgramImpl<GXMovieMaterialProgram>
{
public:
    GXMovieMaterialProgram();
    virtual ~GXMovieMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);

    static GXMovieMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

#endif // NL_GLX_GX_MOVIE_MATERIAL_PROGRAM_H
