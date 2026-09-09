#include "NL/gl/glPlat.h"
#include "NL/gl/gl.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTarget.h"
#include "NL/gl/glView.h"
#include "NL/nlString.h"
#include "Game/GL/GLShadowBlendMeshWriter.h"
#include "unclassified/tu_8037091C.h"


static char sString_805356E0[] = "shadowvolume";
static char sString_805356F0[] = "target/shadowvolume";
static GLRenderPair sRenderPair_806E2408;

extern "C" void fn_8037091C()
{
    GLTargetInfo info;
    nlZeroMemory(&info, sizeof(info));
    info.width = glplatGetDefaultTargetWidth();
    info.height = glplatGetDefaultTargetHeight();
    info.format = 6;
    info.unknown18 = 0;
    sRenderPair_806E2408 = glCreateTarget(sString_805356E0, &info);
}

extern "C" void fn_80370998(GLView* view, GLView*)
{
    view->SetRenderPair(sRenderPair_806E2408);
    view->m_Target = GLViewTarget_Mode8;
}

extern "C" void fn_803709C4(
    glModel* firstModel, glModel* secondModel, GLView* view)
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
    static u32 texture_806E2410 = glGetTexture(sString_805356F0);

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
        ((glTextureBinding*)writer.GetModel()
                ->packets->unknown20)[1]
            .texture = 0;
        glTextureBinding* state
            = (glTextureBinding*)writer.GetModel()
                  ->packets->unknown20;
        state->texture = texture_806E2410;
        state->textureIndex = 0xFFFF;
        state->SetWrapS(true);
        state->SetWrapT(true);
        state->unknown07 = 0;

        writer.Texcoord(1.0f, 0.0f);
        writer.Colour(colour);
        writer.Vertex(width, 0.0f, 0.0f);

        writer.Texcoord(0.0f, 0.0f);
        writer.Colour(colour);
        writer.Vertex(0.0f, 0.0f, 0.0f);

        writer.Texcoord(1.0f, 1.0f);
        writer.Colour(colour);
        writer.Vertex(width, height, 0.0f);

        writer.Texcoord(0.0f, 1.0f);
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
