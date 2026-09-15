#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glPlat.h"
#include "NL/gl/gl.h"
#include "NL/gl/glModel.h"
#include "NL/glx/GXShadowVolumeMaterialProgram.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTarget.h"
#include "NL/gl/glView.h"
#include "NL/nlString.h"
#include "Game/GL/GLShadowBlendMeshWriter.h"
#include "Game/Render/ShadowVolume.h"

static char sShadowVolumeTargetName[] = "shadowvolume";
static char sShadowVolumeTextureName[] = "target/shadowvolume";
static GLRenderPair sShadowVolumeTarget;

void CreateShadowVolumeTarget()
{
    GLTargetInfo info;
    nlZeroMemory(&info, sizeof(info));
    info.width = glplatGetDefaultTargetWidth();
    info.height = glplatGetDefaultTargetHeight();
    info.format = GLTargetFormat_6;
    info.unknown18 = 0;
    sShadowVolumeTarget = glCreateTarget(sShadowVolumeTargetName, &info);
}

void SetShadowVolumeTarget(GLView* view, GLView*)
{
    view->SetRenderPair(sShadowVolumeTarget);
    view->m_Target = GLViewTarget_Mode8;
}

void AttachShadowVolumeModels(
    glModel* firstModel, glModel* secondModel, GLView* view, GLView*)
{
    glSetDefaultState(false);
    glSetRasterState(GLS_Culling, 1);
    glSetRasterState(GLS_DepthTest, 1);
    glSetRasterState(GLS_DepthWrite, 0);
    glSetRasterState(GLS_AlphaTest, 0);
    glSetRasterState(GLS_AlphaBlend, 2);
    glSetRasterState(GLS_ColourWrite, 2);
    glModelSetRasterState(firstModel, glHandleizeRasterState());
    view->AttachModel(firstModel, 0);

    glSetRasterState(GLS_Culling, 2);
    glSetRasterState(GLS_DepthTest, 1);
    glSetRasterState(GLS_DepthWrite, 0);
    glSetRasterState(GLS_AlphaTest, 0);
    glSetRasterState(GLS_AlphaBlend, 7);
    glSetRasterState(GLS_ColourWrite, 2);
    glModelSetRasterState(secondModel, glHandleizeRasterState());
    view->AttachModel(secondModel, 0);
}

void RenderShadowVolumeBlend(GLView* view)
{
    static u32 shadowVolumeTexture = glGetTexture(sShadowVolumeTextureName);

    GLShadowBlendMeshWriter writer;
    nlColour colour = { 0, 0, 0, 0 };
    glSetDefaultState(false);
    glSetRasterState(GLS_DepthTest, 0);
    glSetRasterState(GLS_DepthWrite, 0);
    glSetRasterState(GLS_AlphaBlend, 1);
    glSetRasterState(GLS_Culling, 0);
    glSetRasterState(GLS_ColourWrite, 3);
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTextureState(glHandleizeTextureState());

    float width = glGetOrthographicWidth();
    float height = glGetOrthographicHeight();
    if (writer.Begin(4, GLP_TriStrip, 0))
    {
        static_cast<GXShadowVolumeParameters*>(writer.GetModel()
                ->packets->materialParameters)->useFixedColour = 0;
        glTextureBinding* state
            = &static_cast<GXShadowVolumeParameters*>(writer.GetModel()
                   ->packets->materialParameters)->diffuseTexture;
        state->texture = shadowVolumeTexture;
        state->textureIndex = 0xFFFF;
        state->SetWrapS(true);
        state->SetWrapT(true);
        state->unknown07 = 0;

        float uMin = 0.0f;
        float uMax = 1.0f;
        writer.Texcoord(uMax, 0.0f);
        writer.Colour(colour);
        writer.Vertex(width, 0.0f, 0.0f);

        writer.Texcoord(uMin, 0.0f);
        writer.Colour(colour);
        writer.Vertex(0.0f, 0.0f, 0.0f);

        writer.Texcoord(uMax, 1.0f);
        writer.Colour(colour);
        writer.Vertex(width, height, 0.0f);

        writer.Texcoord(uMin, 1.0f);
        writer.Colour(colour);
        writer.Vertex(0.0f, height, 0.0f);

        if (writer.End())
        {
            glModel* model = writer.GetModel();
            glModelSetRasterState(model, glHandleizeRasterState());
            view->AttachModel(model, 0);
        }
    }
}
