#ifndef NL_GLX_GX_CAMERA_SCROLLED_OVERLAY_MATERIAL_PROGRAM_H
#define NL_GLX_GX_CAMERA_SCROLLED_OVERLAY_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXCameraScrolledOverlayParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding overlayTexture;
    /* 0x10 */ glTextureBinding overlayMaskTexture;
    /* 0x18 */ float overlayScale;
    /* 0x1C */ float cameraScroll;
    /* 0x20 */ float overlayAmount;
    /* 0x24 */ int diffuseWrapEnabled;
    /* 0x28 */ int lightingEnabled;
    /* 0x2C */ int shadowEnabled;
}; // size: 0x30

class GXCameraScrolledOverlayMaterialProgram : public GXMaterialProgramImpl<GXCameraScrolledOverlayMaterialProgram>
{
public:
    GXCameraScrolledOverlayMaterialProgram();
    virtual ~GXCameraScrolledOverlayMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXCameraScrolledOverlayMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[9];
};

#endif // NL_GLX_GX_CAMERA_SCROLLED_OVERLAY_MATERIAL_PROGRAM_H
