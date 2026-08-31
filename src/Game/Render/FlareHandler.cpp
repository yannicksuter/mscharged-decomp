#include "Game/Render/FlareHandler.h"

#include "NL/gl/glState.h"
#include "NL/gl/glView.h"

FlareHandler FlareHandler::instance;

void FlareHandler::Initialize(GLView* view)
{
    mpView = view;
}

void FlareHandler::Cleanup()
{
    while (halos.m_pStart != 0)
    {
        FlareStruct* obj = nlListRemoveStart(
            &halos.m_pStart, &halos.m_pEnd);
        delete obj;
    }

    while (glows.m_pStart != 0)
    {
        FlareStruct* obj = nlListRemoveStart(
            &glows.m_pStart, &glows.m_pEnd);
        delete obj;
    }
}

void FlareHandler::AddFace(
    const FlareStruct* pFlare, GLMeshWriter* pMeshWriter)
{
    float sn;
    float cs;
    nlVector3 position;
    nlVector3 viewPosition;
    nlVector3 v[4];

    position = pFlare->worldPosition;
    nlMultPosVectorMatrix(viewPosition, position, viewMatrix);
    if (viewPosition.y == 0.0f)
    {
        sn = 0.0f;
        cs = 0.5f * pFlare->size;
    }
    else
    {
        nlSinCos(&sn, &cs,
            (u16)((s32)(8192.0f * (viewPosition.x / viewPosition.y))
                + 0x2000));
        sn *= pFlare->size * 0.5f;
        cs *= pFlare->size * 0.5f;
    }

    nlVector3 a;
    a.x = cs * viewRight.x + sn * viewUp.x;
    a.y = cs * viewRight.y + sn * viewUp.y;
    a.z = cs * viewRight.z + sn * viewUp.z;

    nlVector3 b;
    b.x = -sn * viewRight.x + cs * viewUp.x;
    b.y = -sn * viewRight.y + cs * viewUp.y;
    b.z = -sn * viewRight.z + cs * viewUp.z;

    nlVec3Set(v[0], position.x + a.x + b.x,
        position.y + a.y + b.y, position.z + a.z + b.z);
    nlVec3Set(v[1], position.x - a.x + b.x,
        position.y - a.y + b.y, position.z - a.z + b.z);
    nlVec3Set(v[2], position.x - a.x - b.x,
        position.y - a.y - b.y, position.z - a.z - b.z);
    nlVec3Set(v[3], position.x + a.x - b.x,
        position.y + a.y - b.y, position.z + a.z - b.z);

    pMeshWriter->Texcoord(0, 0);
    pMeshWriter->Colour(pFlare->colour);
    pMeshWriter->Vertex(v[0]);

    pMeshWriter->Texcoord(1024, 0);
    pMeshWriter->Colour(pFlare->colour);
    pMeshWriter->Vertex(v[1]);

    pMeshWriter->Texcoord(1024, 1024);
    pMeshWriter->Colour(pFlare->colour);
    pMeshWriter->Vertex(v[2]);

    pMeshWriter->Texcoord(0, 1024);
    pMeshWriter->Colour(pFlare->colour);
    pMeshWriter->Vertex(v[3]);
}

void FlareHandler::Render()
{
    if (halos.m_pStart != 0 || glows.m_pStart != 0)
    {
        mpView->m_Interface->GetViewMatrix(viewMatrix);
        nlVec3Set(viewRight,
            viewMatrix.m11, viewMatrix.m21, viewMatrix.m31);
        nlVec3Set(viewUp,
            viewMatrix.m12, viewMatrix.m22, viewMatrix.m32);

        glSetDefaultState(true);
        glSetRasterState(GLS_DepthWrite, 0);
        glSetRasterState(GLS_Culling, 0);
        glSetRasterState(GLS_AlphaBlend, 3);
        glSetRasterState(GLS_AlphaTest, 1);
        glSetCurrentRasterState(glHandleizeRasterState());

        if (halos.m_pStart != 0)
        {
            GLMeshWriter writer;
            int count = 0;
            FlareStruct* halo = halos.m_pStart;
            while (halo != 0)
            {
                ++count;
                halo = halo->next;
            }

            if (writer.Begin(count * 4, 3, 0))
            {
                u32 texture = glGetTexture("global/flare_halo");
                UnidentifiedTextureState* state =
                    (UnidentifiedTextureState*)writer.GetModel()
                        ->packets->unknown20;
                state->texture = texture;
                state->textureIndex = 0xFFFF;
                state->SetWrapS(true);
                state->SetWrapT(true);
                state->unknown07 = 0;

                halo = halos.m_pStart;
                while (halo != 0)
                {
                    AddFace(halo, &writer);
                    halo = halo->next;
                }

                if (writer.End())
                {
                    mpView->AttachModel(writer.GetModel(), 0);
                }
            }
        }

        if (glows.m_pStart != 0)
        {
            GLMeshWriter writer;
            int count = 0;
            FlareStruct* glow = glows.m_pStart;
            while (glow != 0)
            {
                ++count;
                glow = glow->next;
            }

            if (writer.Begin(count * 4, 3, 0))
            {
                u32 texture = glGetTexture("global/flare_glow");
                UnidentifiedTextureState* state =
                    (UnidentifiedTextureState*)writer.GetModel()
                        ->packets->unknown20;
                state->texture = texture;
                state->textureIndex = 0xFFFF;
                state->SetWrapS(true);
                state->SetWrapT(true);
                state->unknown07 = 0;

                glow = glows.m_pStart;
                while (glow != 0)
                {
                    AddFace(glow, &writer);
                    glow = glow->next;
                }

                if (writer.End())
                {
                    mpView->AttachModel(writer.GetModel(), 0);
                }
            }
        }
    }
}
