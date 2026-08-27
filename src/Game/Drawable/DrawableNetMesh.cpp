#include "Game/Field.h"
#include "Game/Net.h"
#include "Game/Render/NetMesh.h"
#include "NL/nlMath.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/nlMemory.h"

struct nlColour
{
    u8 c[4];
};

struct glQuad3
{
    char data[0x60];

    void SetupRotatedRectangle(float, float, const nlMatrix4&, bool, bool);
    void SetColour(const nlColour&);
};

struct StreamDefinition
{
    u32 values[4];
};

struct RenderHeader
{
    u32 state;
    u16 field4;
    u8 field6;
    u8 field7;
    float colour[4];
};

struct WriterModelData
{
    char pad[0x20];
    RenderHeader* header;
};

struct WriterModel
{
    char pad[8];
    WriterModelData* data;
};

class MeshWriter
{
public:
    int count;
    WriterModel* model;
    void* resource;
    float* position;
    short* texcoord;

    MeshWriter();
    ~MeshWriter();
    bool Begin(int, int, void*);
    bool End();

    WriterModel* GetModel() const
    {
        return model;
    }

    void Texcoord(float x, float y)
    {
        *texcoord++ = (short)(x * 4096.0f);
        *texcoord++ = (short)(y * 4096.0f);
    }

    void Vertex(const nlVector3& value)
    {
        float x;
        float y;
        float z;
        z = value.z;
        y = value.y;
        x = value.x;
        *position++ = x;
        *position++ = y;
        *position++ = z;
    }
};

struct ModelHandle
{
    u32 first;
    u32 second;
};

class MeshResource
{
public:
    virtual void Unused();
    virtual ModelHandle* Acquire();
    virtual void Release(ModelHandle*);
};

struct WorldDarkening
{
    u32 unused;
    float position;
};

struct LoadFrame
{
    char _000[8];
    u8* position;
};

struct SaveFrame
{
    char _000[8];
    u8* position;
};

class DrawableNetMesh
{
public:
    DrawableNetMesh(bool);
    ~DrawableNetMesh();

    static void Reset();
    void Initialize(int, int);
    void Destroy();
    void Grab(NetMesh&);
    void Blend(float, const DrawableNetMesh&, const DrawableNetMesh&);
    void Replay(LoadFrame&);
    void Replay(SaveFrame&);
    void Render() const;
    void RenderInvisiblePlanes() const;

    nlVector3* mPositions;
    int mNetIndex;
    int mDisplayList;
    int mNumVertices;
    int mNumTriIndices;
    float mJoltCache;
    NetMesh* mNetMesh;
    bool mInitialized;
    bool mVisible;
    char _01E[2];
};

struct UnidentifiedStaticState
{
    UnidentifiedStaticState()
        : value(0)
    {
    }

    void* value;
};

template <typename T>
struct UnidentifiedStaticStorage
{
    static UnidentifiedStaticState state;
};

struct UnidentifiedStaticTag;

__declspec(weak) char LightTextureName[] = "global/lightramp";
__declspec(weak) char BlackTextureName[] = "global/black";
__declspec(weak) char WhiteTextureName[] = "global/white";
__declspec(weak) char NetMeshTextureName[] = "global/netmesh";
__declspec(weak) char CheckerTextureName[] = "global/checkers";


u32 lbl_806E1320 = glGetTexture(LightTextureName);
u32 lbl_806E1324 = glGetTexture(BlackTextureName);
u32 lbl_806E1328 = glGetTexture(WhiteTextureName);

MeshResource* lbl_80570938[2][2];
ModelHandle* lbl_80570948[2][2];

bool lbl_806DCB38[2] = { true, true };
u8 lbl_806DCB3A = 1;
char lbl_806DCB40[8] = "NetMesh";

u32 lbl_806E132C;
u32 lbl_806E1330;
shortVector2* lbl_806E1338[2];
u32* lbl_806E1340[2];
u16* lbl_806E1348[2];
bool lbl_806E1350[2];
int lbl_806E1358[2];
WriterModel* lbl_806E1360[2];
bool lbl_806E1368[2];
int lbl_806E1370[2];
u8 lbl_806E1378;
int lbl_806E137C;

u32 lbl_806E1380 = glGetTexture(NetMeshTextureName);
u32 lbl_806E1384 = glGetTexture(CheckerTextureName);

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

extern u8 lbl_806DC7D8;
extern const StreamDefinition lbl_804DCCE0;

extern "C" void* fn_8027267C(int);
extern "C" void __dla__FPv(void*);
extern "C" void fn_802CC02C(MeshResource*);
extern "C" void fn_802C9254(u32, int, glQuad3*);
extern "C" WorldDarkening* fn_801AF510();
extern "C" void nlBreak__Fv();
extern "C" void* memcpy(void*, const void*, u32);
extern "C" void fn_802CF510(u32, WriterModel*, bool);
extern "C" MeshResource* fn_802CBFD8(const StreamDefinition*, int, const char*);
extern "C" void* memset(void*, int, u32);

static inline u8 KeepPacketFlagBit1(u8 value)
{
    return value & 2;
}

static inline u8 KeepPacketFlagBit0(u8 value)
{
    return value & 1;
}

static inline void MarkMeshUploaded(
    WriterModel* model, bool uploaded, const DrawableNetMesh* mesh)
{
    ((WriterModel* volatile*)lbl_806E1360)
        [((const volatile DrawableNetMesh*)mesh)->mNetIndex] = model;
    lbl_806E1368[((const volatile DrawableNetMesh*)mesh)->mNetIndex] = uploaded;
}

DrawableNetMesh::DrawableNetMesh(bool isPositiveXNet)
    : mNetIndex(isPositiveXNet ? 0 : 1)
    , mNetMesh(0)
    , mInitialized(false)
    , mVisible(false)
{
    lbl_806E132C = (u32)fn_8027267C(0x16);
    lbl_806E1330 = (u32)fn_8027267C(0x1C);
}

DrawableNetMesh::~DrawableNetMesh()
{
    Destroy();
}

void DrawableNetMesh::RenderInvisiblePlanes() const
{
    float goalLineX = cField::GetGoalLineX(1);
    float netHeight = cNet::m_fNetHeight;
    float netWidth = cNet::m_fNetWidth;

    glSetDefaultState(true);
    glSetRasterState((eGLState)1, 1);
    glSetRasterState((eGLState)5, 1);
    glSetRasterState((eGLState)6, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTexture(lbl_806E1328, (eGLTextureType)0);
    glSetTextureState((eGLTextureState)0, 0);
    glSetCurrentTextureState(glHandleizeTextureState());

    nlMatrix4 matrix;
    nlMakeRotationMatrixY(matrix, 1.5707964f);

    nlColour colour = { 0xFF, 0xFF, 0x00, 0x00 };
    colour.c[3] = (u8)lbl_806E137C;
    glQuad3 quad;

    matrix.e2[3][0] = goalLineX - 0.05f;
    matrix.e2[3][1] = 0.0f;
    matrix.e2[3][2] = 0.5f * netHeight;
    matrix.e2[3][3] = 1.0f;
    quad.SetupRotatedRectangle(netHeight, netWidth, matrix, false, false);
    quad.SetColour(colour);
    fn_802C9254(lbl_806E1330, 1, &quad);

    matrix.e2[3][0] = goalLineX + 0.05f;
    matrix.e2[3][1] = 0.0f;
    matrix.e2[3][2] = 0.5f * netHeight;
    matrix.e2[3][3] = 1.0f;
    quad.SetupRotatedRectangle(netHeight, netWidth, matrix, false, false);
    quad.SetColour(colour);
    fn_802C9254(lbl_806E1330, 1, &quad);

    matrix.e2[3][0] = -goalLineX - 0.05f;
    matrix.e2[3][1] = 0.0f;
    matrix.e2[3][2] = 0.5f * netHeight;
    matrix.e2[3][3] = 1.0f;
    quad.SetupRotatedRectangle(netHeight, netWidth, matrix, false, false);
    quad.SetColour(colour);
    fn_802C9254(lbl_806E1330, 1, &quad);

    matrix.e2[3][0] = 0.05f + -goalLineX;
    matrix.e2[3][1] = 0.0f;
    matrix.e2[3][2] = 0.5f * netHeight;
    matrix.e2[3][3] = 1.0f;
    quad.SetupRotatedRectangle(netHeight, netWidth, matrix, false, false);
    quad.SetColour(colour);
    fn_802C9254(lbl_806E1330, 1, &quad);
}

void DrawableNetMesh::Render() const
{
    if (!lbl_806DCB3A || !mInitialized || !NetMesh::s_bAnimatedNetMeshEnabled)
    {
        return;
    }

    if (!lbl_806DC7D8)
    {
        return;
    }

    MeshWriter writer;
    nlVector3* sourcePositions = mPositions;
    shortVector2* sourceTexcoords = lbl_806E1338[mNetIndex];

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
    if (lbl_806E1378)
    {
        texture = lbl_806E1384;
    }
    glSetCurrentTexture(texture, (eGLTextureType)0);

    u16* indices = lbl_806E1348[mNetIndex];
    if ((!lbl_806E1368[mNetIndex] || mVisible == true)
        && writer.Begin(mNumTriIndices, 1, lbl_80570938[mNetIndex][lbl_806E1370[mNetIndex]]))
    {
        if (!lbl_806DCB38[mNetIndex])
        {
            volatile int& bufferIndex = lbl_806E1370[mNetIndex];
            bufferIndex = (bufferIndex + 1) % 2;
            const volatile DrawableNetMesh* volatileThis = this;
            {
                int index = volatileThis->mNetIndex;
                lbl_80570938[index][lbl_806E1370[index]]->Release(
                    lbl_80570948[index][lbl_806E1370[index]]);
            }
            ((volatile u8*)lbl_806DCB38)[volatileThis->mNetIndex] = true;
            {
                int index = volatileThis->mNetIndex;
                ModelHandle* handle =
                    lbl_80570938[index][lbl_806E1370[index]]->Acquire();
                ((ModelHandle* volatile*)lbl_80570948[(unsigned int)index])
                    [lbl_806E1370[(unsigned int)index]] = handle;
            }
            {
                int index = volatileThis->mNetIndex;
                if (lbl_80570948[index][lbl_806E1370[index]]->first
                    || lbl_80570948[index][lbl_806E1370[index]]->second)
                {
                    nlBreak__Fv();
                }
            }
        }
        else
        {
            lbl_80570938[mNetIndex][lbl_806E1370[mNetIndex]]->Release(
                lbl_80570948[mNetIndex][lbl_806E1370[mNetIndex]]);
            const volatile DrawableNetMesh* volatileThis = this;
            int index = volatileThis->mNetIndex;
            ModelHandle* handle =
                lbl_80570938[index][lbl_806E1370[index]]->Acquire();
            lbl_80570948[(unsigned int)index][lbl_806E1370[(unsigned int)index]] = handle;
        }

        float darkness = 1.0f - fn_801AF510()->position;
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

        memcpy(writer.model->data->header->colour, colour, sizeof(colour));
        u32 state = glGetCurrentTexture((eGLTextureType)0);
        RenderHeader* header = writer.model->data->header;
        header->state = state;
        header->field4 = 0xFFFF;
        header->field6 = KeepPacketFlagBit1(header->field6);
        header->field6 = KeepPacketFlagBit0(header->field6);
        header->field7 = 0;

        if (!writer.End())
        {
            return;
        }

        MarkMeshUploaded(writer.GetModel(), true, this);
    }

    if (lbl_806E1368[mNetIndex] && lbl_806E1360[mNetIndex])
    {
        fn_802CF510(lbl_806E132C, lbl_806E1360[mNetIndex], false);
    }

    RenderInvisiblePlanes();
}

void DrawableNetMesh::Reset()
{
    lbl_806DCB38[0] = false;
    lbl_806DCB38[1] = false;
}

void DrawableNetMesh::Initialize(int numVertices, int numTriIndices)
{
    mPositions = (nlVector3*)nlMalloc(numVertices * sizeof(nlVector3), 8, false);

    if (!lbl_806E1350[mNetIndex])
    {
        lbl_806E1348[mNetIndex] = (u16*)nlMalloc(numTriIndices * sizeof(u16), 8, false);

        int allocationSize = numVertices * 4;
        lbl_806E1338[mNetIndex] = (shortVector2*)nlMalloc(allocationSize, 8, false);
        lbl_806E1340[mNetIndex] = (u32*)nlMalloc(allocationSize, 8, false);
        memset(lbl_806E1340[mNetIndex], 0xFF, allocationSize);

        lbl_806E1350[mNetIndex] = true;
        lbl_806E1358[mNetIndex] = numVertices;

        StreamDefinition streams = lbl_804DCCE0;
        lbl_806E1370[mNetIndex] = 0;
        lbl_806DCB38[mNetIndex] = true;
        lbl_806E1368[mNetIndex] = false;

        for (int i = 0; i < 2; ++i)
        {
            lbl_80570938[mNetIndex][i] = fn_802CBFD8(&streams, 2, lbl_806DCB40);
            lbl_80570948[mNetIndex][i] = lbl_80570938[mNetIndex][i]->Acquire();
        }
    }
}

void DrawableNetMesh::Destroy()
{
    if (mInitialized)
    {
        __dla__FPv(mPositions);
    }

    if (lbl_806E1350[mNetIndex])
    {
        __dla__FPv(lbl_806E1338[mNetIndex]);
        __dla__FPv(lbl_806E1348[mNetIndex]);
        __dla__FPv(lbl_806E1340[mNetIndex]);
        lbl_806E1350[mNetIndex] = false;

        for (int i = 0; i < 2; ++i)
        {
            fn_802CC02C(lbl_80570938[mNetIndex][i]);
            lbl_80570938[mNetIndex][i] = 0;
        }

        lbl_806E1368[mNetIndex] = false;
        lbl_806DCB38[mNetIndex] = false;
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

    shortVector2* texcoords = lbl_806E1338[mNetIndex];
    u16* triIndices = lbl_806E1348[mNetIndex];
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
    memcpy(&joltValue, frame.position, sizeof(joltValue));
    frame.position += sizeof(joltValue);

    if (joltValue != mJoltCache)
    {
        mJoltCache = joltValue;
        if (mNetMesh != 0 && mJoltCache > 0.0f)
        {
            mNetMesh->JoltNet();
        }
    }

    bool visible = true;
    memcpy(&visible, frame.position, sizeof(visible));
    frame.position += sizeof(visible);
    if (mVisible != visible)
    {
        lbl_806E1368[mNetIndex] = false;
        mVisible = visible;
    }
}

void DrawableNetMesh::Replay(SaveFrame& frame)
{
    mJoltCache = mNetMesh->mJolt;
    memcpy(frame.position, &mJoltCache, sizeof(mJoltCache));
    frame.position += sizeof(mJoltCache);
    memcpy(frame.position, &mVisible, sizeof(mVisible));
    frame.position += sizeof(mVisible);
}
