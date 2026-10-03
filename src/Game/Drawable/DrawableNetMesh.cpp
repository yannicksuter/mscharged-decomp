#include "Game/Drawable/DrawableNetMesh.h"
#include "Game/GL/MeshWriter.h"
#include "NL/glx/GXConstantColourMaterialProgram.h"
#include "Game/Replay.h"
#include "Game/Field.h"
#include "Game/Net.h"
#include "Game/Render/NetMesh.h"
#include "Game/Render/RLView.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "Game/Task/GameRenderTask.h"
#include "NL/gl/glDraw3.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlDebug.h"
#include "NL/nlMemory.h"
#include "NL/nlMath.h"

#include <string.h>
#include "Game/UnidentifiedStaticStorage.h"

__declspec(weak) char LightTextureName[] = "global/lightramp";
__declspec(weak) char BlackTextureName[] = "global/black";
__declspec(weak) char WhiteTextureName[] = "global/white";
__declspec(weak) char NetMeshTextureName[] = "global/netmesh";
__declspec(weak) char CheckerTextureName[] = "global/checkers";

static u32 LightTexture = glGetTexture(LightTextureName);
static u32 BlackTexture = glGetTexture(BlackTextureName);
static u32 WhiteTexture = glGetTexture(WhiteTextureName);

GLResourcePool* gNetMeshResourcePools[2][2];
GLResourceMarker* gNetMeshResourceMarkers[2][2];

bool sbResourcePoolSelected[2] = { true, true };
u8 sbRenderAnimatedNetMesh = 1;
char gNetMeshResourcePoolName[8] = "NetMesh";

GLView* g_pNetMeshView;
u32 g_NetMeshInvisiblePlaneView;
shortVector2* spTexcoord[2];
u32* spColour[2];
u16* spTriIndices[2];
bool sbStaticInitialized[2];
int sNumVertices[2];
glModel* spNetModel[2];
bool sbNetModelValid[2];
int siResourcePoolIndex[2];
u8 sbUseCheckerTexture;
int siInvisiblePlaneAlpha;

static u32 NetMeshTexture = glGetTexture(NetMeshTextureName);
static u32 CheckerTexture = glGetTexture(CheckerTextureName);

static inline void MarkMeshUploaded(
    glModel* model, bool uploaded, const DrawableNetMesh* mesh)
{
    ((glModel* volatile*)spNetModel)
        [((const volatile DrawableNetMesh*)mesh)->mNetIndex] = model;
    sbNetModelValid[((const volatile DrawableNetMesh*)mesh)->mNetIndex] = uploaded;
}

DrawableNetMesh::DrawableNetMesh(bool isPositiveXNet)
    : mNetIndex(isPositiveXNet ? 0 : 1)
    , mNetMesh(0)
    , mInitialized(false)
    , mVisible(false)
{
    g_pNetMeshView = GetLayerView(eCLV_UnsortedPerspective);
    g_NetMeshInvisiblePlaneView = (u32)GetLayerView(eCLV_InvisiblePlane);
}

DrawableNetMesh::~DrawableNetMesh()
{
    Destroy();
}

void DrawableNetMesh::RenderInvisiblePlanes() const
{
    float goalLineX = cField::GetGoalLineX(1U);
    float netHeight = cNet::m_fNetHeight;
    float netWidth = cNet::m_fNetWidth;

    glSetDefaultState(true);
    glSetRasterState((eGLState)1, 1);
    glSetRasterState((eGLState)5, 1);
    glSetRasterState((eGLState)6, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTexture(WhiteTexture, (eGLTextureType)0);
    glSetTextureState((eGLTextureState)0, 0);
    glSetCurrentTextureState(glHandleizeTextureState());

    nlMatrix4 matrix;
    nlMakeRotationMatrixY(matrix, 1.5707964f);

    nlColour colour = { 0xFF, 0xFF, 0x00, 0x00 };
    colour.c[3] = (u8)siInvisiblePlaneAlpha;
    glQuad3 quad;

    matrix.e2[3][0] = goalLineX - 0.05f;
    matrix.e2[3][1] = 0.0f;
    matrix.e2[3][2] = 0.5f * netHeight;
    matrix.e2[3][3] = 1.0f;
    quad.SetupRotatedRectangle(netHeight, netWidth, matrix, false, false);
    quad.SetColour(colour);
    glAttachQuad3((eGLView)g_NetMeshInvisiblePlaneView, 1, &quad);

    matrix.e2[3][0] = goalLineX + 0.05f;
    matrix.e2[3][1] = 0.0f;
    matrix.e2[3][2] = 0.5f * netHeight;
    matrix.e2[3][3] = 1.0f;
    quad.SetupRotatedRectangle(netHeight, netWidth, matrix, false, false);
    quad.SetColour(colour);
    glAttachQuad3((eGLView)g_NetMeshInvisiblePlaneView, 1, &quad);

    matrix.e2[3][0] = -goalLineX - 0.05f;
    matrix.e2[3][1] = 0.0f;
    matrix.e2[3][2] = 0.5f * netHeight;
    matrix.e2[3][3] = 1.0f;
    quad.SetupRotatedRectangle(netHeight, netWidth, matrix, false, false);
    quad.SetColour(colour);
    glAttachQuad3((eGLView)g_NetMeshInvisiblePlaneView, 1, &quad);

    matrix.e2[3][0] = 0.05f + -goalLineX;
    matrix.e2[3][1] = 0.0f;
    matrix.e2[3][2] = 0.5f * netHeight;
    matrix.e2[3][3] = 1.0f;
    quad.SetupRotatedRectangle(netHeight, netWidth, matrix, false, false);
    quad.SetColour(colour);
    glAttachQuad3((eGLView)g_NetMeshInvisiblePlaneView, 1, &quad);
}

void DrawableNetMesh::Render() const
{
    if (!sbRenderAnimatedNetMesh || !mInitialized || !NetMesh::s_bAnimatedNetMeshEnabled)
    {
        return;
    }

    if (!g_bRenderWorldEffects)
    {
        return;
    }

    MeshWriter writer;
    nlVector3* sourcePositions = mPositions;
    shortVector2* sourceTexcoords = spTexcoord[mNetIndex];

    glSetDefaultState(true);
    glSetRasterState((eGLState)6, 0);
    glSetRasterState((eGLState)5, 1);
    glSetRasterState((eGLState)3, 1);
    glSetRasterState((eGLState)0, 1);
    glSetTextureState((eGLTextureState)0, 0);
    glSetRasterState((eGLState)1, 1);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentMatrix(glGetIdentityMatrix());

    u32 texture = NetMesh::sNetTextureHandle;
    if (sbUseCheckerTexture)
    {
        texture = CheckerTexture;
    }
    glSetCurrentTexture(texture, (eGLTextureType)0);

    u16* indices = spTriIndices[mNetIndex];
    if ((!sbNetModelValid[mNetIndex] || mVisible == true)
        && writer.Begin(mNumTriIndices, 1, gNetMeshResourcePools[mNetIndex][siResourcePoolIndex[mNetIndex]]))
    {
        if (!sbResourcePoolSelected[mNetIndex])
        {
            volatile int& bufferIndex = siResourcePoolIndex[mNetIndex];
            bufferIndex = (bufferIndex + 1) % 2;
            const volatile DrawableNetMesh* volatileThis = this;
            {
                int index = volatileThis->mNetIndex;
                gNetMeshResourcePools[index][siResourcePoolIndex[index]]->ReleaseResource(
                    (unsigned long)gNetMeshResourceMarkers[index][siResourcePoolIndex[index]]);
            }
            ((volatile u8*)sbResourcePoolSelected)[volatileThis->mNetIndex] = true;
            {
                int index = volatileThis->mNetIndex;
                GLResourceMarker* handle = (GLResourceMarker*)gNetMeshResourcePools[index][siResourcePoolIndex[index]]->MarkResource();
                ((GLResourceMarker* volatile*)gNetMeshResourceMarkers[(unsigned int)index])
                    [siResourcePoolIndex[(unsigned int)index]] = handle;
            }
            {
                int index = volatileThis->mNetIndex;
                if (gNetMeshResourceMarkers[index][siResourcePoolIndex[index]]->mUsedMemory[0]
                    || gNetMeshResourceMarkers[index][siResourcePoolIndex[index]]->mUsedMemory[1])
                {
                    nlBreak();
                }
            }
        }
        else
        {
            gNetMeshResourcePools[mNetIndex][siResourcePoolIndex[mNetIndex]]->ReleaseResource(
                (unsigned long)gNetMeshResourceMarkers[mNetIndex][siResourcePoolIndex[mNetIndex]]);
            const volatile DrawableNetMesh* volatileThis = this;
            int index = volatileThis->mNetIndex;
            GLResourceMarker* handle = (GLResourceMarker*)gNetMeshResourcePools[index][siResourcePoolIndex[index]]->MarkResource();
            gNetMeshResourceMarkers[(unsigned int)index][siResourcePoolIndex[(unsigned int)index]] = handle;
        }

        float darkness = 1.0f - WorldDarkening::Instance().mPos;
        float colour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        colour[0] = darkness;
        colour[1] = darkness;
        colour[2] = darkness;

        int i;
        for (i = 0; i < mNumTriIndices; indices++, i++)
        {
            u16 index = *indices;
            writer.Texcoord(
                0.0009765625f * sourceTexcoords[index].x,
                0.0009765625f * sourceTexcoords[index].y);
            writer.Vertex(sourcePositions[index]);
        }

        memcpy(&((GXConstantColourParameters*)writer.model->packets->materialParameters)->constantColour,
            colour, sizeof(colour));
        u32 texture = glGetCurrentTexture((eGLTextureType)0);
        glTextureBinding* binding
            = &((GXConstantColourParameters*)writer.model->packets->materialParameters)->diffuseTexture;
        binding->texture = texture;
        binding->textureIndex = 0xFFFF;
        binding->SetWrapS(0);
        binding->SetWrapT(0);
        binding->unknown07 = 0;

        if (!writer.End())
        {
            return;
        }

        MarkMeshUploaded(writer.GetModel(), true, this);
    }

    if (sbNetModelValid[mNetIndex] && spNetModel[mNetIndex])
    {
        g_pNetMeshView->AttachModel(spNetModel[mNetIndex], false);
    }

    RenderInvisiblePlanes();
}

void DrawableNetMesh::Reset()
{
    sbResourcePoolSelected[0] = false;
    sbResourcePoolSelected[1] = false;
}

void DrawableNetMesh::Initialize(int numVertices, int numTriIndices)
{
    mPositions = (nlVector3*)nlMalloc(numVertices * sizeof(nlVector3), 8, false);

    if (!sbStaticInitialized[mNetIndex])
    {
        spTriIndices[mNetIndex] = (u16*)nlMalloc(numTriIndices * sizeof(u16), 8, false);

        int allocationSize = numVertices * 4;
        spTexcoord[mNetIndex] = (shortVector2*)nlMalloc(allocationSize, 8, false);
        spColour[mNetIndex] = (u32*)nlMalloc(allocationSize, 8, false);
        memset(spColour[mNetIndex], 0xFF, allocationSize);

        sbStaticInitialized[mNetIndex] = true;
        sNumVertices[mNetIndex] = numVertices;

        GLMemoryRequirement requirements[2] = {
            { GLM_Header, 0x100 },
            { GLM_VertexData, 0x2EF8 },
        };
        siResourcePoolIndex[mNetIndex] = 0;
        sbResourcePoolSelected[mNetIndex] = true;
        sbNetModelValid[mNetIndex] = false;

        for (int i = 0; i < 2; ++i)
        {
            gNetMeshResourcePools[mNetIndex][i] =
                glCreateResourcePool(requirements, 2, gNetMeshResourcePoolName);
            gNetMeshResourceMarkers[mNetIndex][i] = (GLResourceMarker*)gNetMeshResourcePools[mNetIndex][i]->MarkResource();
        }
    }
}

void DrawableNetMesh::Destroy()
{
    if (mInitialized)
    {
        operator delete[](mPositions);
    }

    if (sbStaticInitialized[mNetIndex])
    {
        operator delete[](spTexcoord[mNetIndex]);
        operator delete[](spTriIndices[mNetIndex]);
        operator delete[](spColour[mNetIndex]);
        sbStaticInitialized[mNetIndex] = false;

        for (int i = 0; i < 2; ++i)
        {
            glDestroyResourcePool(
                gNetMeshResourcePools[mNetIndex][i]);
            gNetMeshResourcePools[mNetIndex][i] = 0;
        }

        sbNetModelValid[mNetIndex] = false;
        sbResourcePoolSelected[mNetIndex] = false;
    }

    mInitialized = false;
}

void DrawableNetMesh::Grab(NetMesh& netMesh)
{
    mNetMesh = &netMesh;
    mVisible = false;

    if (!netMesh.mbInitialized)
    {
        return;
    }

    if (!mInitialized)
    {
        int numTriIndices = netMesh.m_NumTriStripIndices;
        int numVertices = netMesh.m_NumParticles;
        mNumVertices = numVertices;
        mNumTriIndices = numTriIndices;
        Initialize(numVertices, numTriIndices);
        mInitialized = true;
        mJoltCache = 0.0f;
    }

    shortVector2* texcoords = spTexcoord[mNetIndex];
    u16* triIndices = spTriIndices[mNetIndex];
    for (int i = 0; i < netMesh.m_NumTriStripIndices; ++i)
    {
        *triIndices++ = netMesh.m_TriStripIndices[i];
    }

    for (int i = 0; i < netMesh.m_NumParticles; ++i)
    {
        mPositions[i] = netMesh.m_v3Position[i];
        *texcoords++ = netMesh.m_v2TextureCoords[i];
    }

    mVisible = netMesh.mbIsActive;
}

void DrawableNetMesh::Blend(
    float blendFactor, const DrawableNetMesh& lhs, const DrawableNetMesh& rhs)
{
    if (!lhs.mInitialized || !rhs.mInitialized)
    {
        return;
    }

    if (!mInitialized)
    {
        int numTriIndices = lhs.mNumTriIndices;
        int numVertices = lhs.mNumVertices;
        mNumTriIndices = numTriIndices;
        mNumVertices = numVertices;
        Initialize(numVertices, numTriIndices);
        mInitialized = true;
        mJoltCache = 0.0f;
    }

    nlVector3* destination;
    nlVector3* source;
    float oneMinusBlend = 1.0f - blendFactor;

    int offset;
    int i;
    for (i = 0, offset = 0; i < mNumVertices; ++i, offset += sizeof(nlVector3))
    {
        source = (nlVector3*)((char*)((const volatile DrawableNetMesh*)&lhs)->mPositions + offset);
        destination = (nlVector3*)((char*)((volatile DrawableNetMesh*)this)->mPositions + offset);
        nlVec3Scale(*destination, *source, oneMinusBlend);
    }

    for (int i = 0; i < mNumVertices; ++i)
    {
        destination = &((volatile DrawableNetMesh*)this)->mPositions[i];
        source = (nlVector3*)&((const volatile DrawableNetMesh*)&rhs)->mPositions[i];
        nlVec3ScaleAdd(*destination, blendFactor, *source, *destination);
    }

    mVisible = lhs.mVisible;
}

void DrawableNetMesh::Replay(LoadFrame& frame)
{
    float joltValue = 0.0f;
    Replayable<0>(frame, joltValue);

    if (joltValue != mJoltCache)
    {
        mJoltCache = joltValue;
        if (mNetMesh != 0 && mJoltCache > 0.0f)
        {
            mNetMesh->JoltNet();
        }
    }

    bool visible = true;
    Replayable<0>(frame, visible);
    if (mVisible != visible)
    {
        sbNetModelValid[mNetIndex] = false;
        mVisible = visible;
    }
}

void DrawableNetMesh::Replay(SaveFrame& frame)
{
    mJoltCache = mNetMesh->mJolt;
    Replayable<0>(frame, mJoltCache);
    Replayable<0>(frame, mVisible);
}
