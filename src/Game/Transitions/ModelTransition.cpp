#include "Game/Transitions/ModelTransition.h"

#include "Game/Effects/EffectsGroup.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/SAnim/pnSAnimController.h"
#include "NL/gl/gl.h"
#include "NL/gl/glLoadModel.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlVector.h"
#include "unclassified/tu_802A87F8.h"

#include <stddef.h>
#include <stdlib.h>

extern "C"
{
    extern unsigned long lbl_806E1F0C;

    void* fn_802CDF0C();
    unsigned long fn_802CE1B8(void*, unsigned long);
    void fn_802CC458(glModelPacket*, unsigned long, unsigned long);
    void fn_802CC4FC(glModelPacket*, unsigned long, const unsigned long*);
    ResourceInterface_802CC094* fn_802CC094();
    glModel* fn_802C8208(
        const char*, unsigned long*, ResourceInterface_802CC094*);
    EffectsGroup* fn_802E7CDC(EmissionManager*, const char*);
    EmissionController* fn_802E7FE4(
        EmissionManager*, EffectsGroup*, int, bool, bool);
}

GLView* ModeledScreenTransition::s_3DView;
void (*lbl_806E217C)(glModel*);
nlAVLTree<unsigned long, TransitionModelStore,
    DefaultKeyCompare<unsigned long> > ModeledScreenTransition::g_ModelInventory;

static inline int GetNumLeafNodesInHierarchy(
    cSHierarchy& h, int node, int ret)
{
    if (h.GetNumChildren(node) == 0)
    {
        return ret + 1;
    }

    for (int i = 0; i < h.GetNumChildren(node); i++)
    {
        ret = GetNumLeafNodesInHierarchy(h, h.GetChild(node, i), ret);
    }

    return ret;
}

static void ShuffleIntoOutline(
    Vector<nlVector3, DefaultAllocator>& polygon)
{
    float min = 9999.0f;

    for (int i = 1; i < polygon.mSize; i++)
    {
        float dist = nlGetLengthSquared3D(
            polygon.mData[0].x - polygon.mData[i].x,
            polygon.mData[0].y - polygon.mData[i].y,
            polygon.mData[0].z - polygon.mData[i].z);

        if (dist < min)
        {
            nlVector3 tmp = polygon.mData[i];
            min = dist;
            polygon.mData[i] = polygon.mData[1];
            polygon.mData[1] = tmp;
        }
    }

    nlVector3 dir;

    for (int i = 1; i < polygon.mSize - 1; i++)
    {
        float max = 1.0f;
        nlRecipSqrt(
            dir.x * dir.x + dir.y * dir.y + dir.z * dir.z, true);

        int prev = i;
        prev -= 1;
        nlVec3Set(dir,
            polygon.mData[i].x - polygon.mData[prev].x,
            polygon.mData[i].y - polygon.mData[prev].y,
            polygon.mData[i].z - polygon.mData[prev].z);

        for (int j = i + 1; j < polygon.mSize; j++)
        {
            nlVector3 delta;
            nlVec3Sub(delta, polygon[i], polygon[j]);
            float recip = nlRecipSqrt(delta.GetLengthSq3D(), true);
            nlVec3Scale(delta, recip);
            float dot = nlVec3DotProduct(dir, delta);

            if (dot <= max)
            {
                max = dot;
                nlVector3 tmp = polygon[i + 1];
                polygon[i + 1] = polygon[j];
                polygon[j] = tmp;
            }
        }
    }
}

static inline int UpdateEffectsFromLeafNodes(cPoseAccumulator& pa,
    EmissionController** ecs, cSHierarchy& skeleton, int leaf, int node)
{
    if (skeleton.GetNumChildren(node) == 0)
    {
        EmissionController* ec = ecs[leaf];
        if (ec != NULL)
        {
            if (EmissionManager::Instance()->IsStillAlive(ec))
            {
                ecs[leaf]->SetPosition(
                    pa.GetNodeMatrix(node).GetTranslation());
            }
            else
            {
                ecs[leaf] = NULL;
            }
        }
        leaf++;
    }
    else
    {
        for (int i = 0; i < skeleton.GetNumChildren(node); i++)
        {
            leaf = UpdateEffectsFromLeafNodes(pa, ecs, skeleton, leaf,
                skeleton.GetChild(node, i));
        }
    }

    return leaf;
}

ModeledScreenTransition::ModeledScreenTransition()
{
    m_pModels = NULL;
    m_nModels = 0;
    m_pSkeleton = NULL;
    m_pAnim = NULL;
    m_pSkelFile = NULL;
    m_pAnimFile = NULL;
    m_nTexture = 0xFFFFFFFF;
    m_pPoseAccumulator = NULL;
    m_pPoseTree = NULL;
    m_bScreenGrab = false;
    m_bEnableGrab = false;
    m_pModelMap = NULL;
    m_Effects = NULL;
    m_RenderOutline = false;
    m_mWorldMatrix.SetIdentity();
    m_EffectName[0] = '\0';
}

ModeledScreenTransition::~ModeledScreenTransition()
{
    if (m_pPoseAccumulator != NULL)
    {
        delete m_pPoseAccumulator;
        m_pPoseAccumulator = NULL;
    }

    if (m_pPoseTree != NULL)
    {
        delete m_pPoseTree;
        m_pPoseTree = NULL;
    }

    if (m_pSkeleton != NULL)
    {
        m_pSkeleton = NULL;
        delete[] m_pSkelFile;
        m_pSkelFile = NULL;
    }

    if (m_pAnim != NULL)
    {
        m_pAnim->Destroy();
        m_pAnim = NULL;
        delete[] m_pAnimFile;
        m_pAnimFile = NULL;
    }

    if (m_pModelMap != NULL)
    {
        delete[] m_pModelMap;
        m_pModelMap = NULL;
    }

    if (m_Effects != NULL)
    {
        delete[] m_Effects;
    }

    m_pModels = NULL;
    m_nModels = 0;
}

void ModeledScreenTransition::DoSanityCheck()
{
}

void ModeledScreenTransition::Update(float deltaTime)
{
    if (m_pPoseTree != NULL)
    {
        bool skipUpdate = false;
        if (m_pPoseTree->m_ePlayMode == PM_HOLD
            && m_pPoseTree->m_fTime == 1.0f)
        {
            skipUpdate = true;
        }

        if (!skipUpdate)
        {
            m_pPoseTree->Update(deltaTime);
            m_pPoseAccumulator->Pose(*m_pPoseTree, m_mWorldMatrix);
        }
    }

    if (m_EffectName[0] != '\0')
    {
        UpdateEffectsFromLeafNodes(*m_pPoseAccumulator, m_Effects,
            *m_pSkeleton, 0, 0);
    }
}

bool ModeledScreenTransition::UnidentifiedVirtual30()
{
    if (m_bEnableGrab)
    {
        m_bEnableGrab = false;
        return true;
    }
    return false;
}

static inline u32 glAllocSetMatrix(const nlMatrix4& matrix)
{
    u32 handle = glAllocMatrix();
    if (handle != 0xFFFFFFFF)
    {
        glSetMatrix(handle, matrix);
    }
    return handle;
}

void ModeledScreenTransition::Render(GLView* view)
{
    for (unsigned long i = 0; i < m_nModels; i++)
    {
        for (unsigned long j = 0; j < m_pModels[i].numPackets; j++)
        {
            m_pModels[i].packets[j].matrix = glAllocSetMatrix(
                m_pPoseAccumulator->GetNodeMatrix(m_pModelMap[i]));
        }

        s_3DView->AttachModel(&m_pModels[i], 0);
        if (lbl_806E217C != NULL)
        {
            lbl_806E217C(&m_pModels[i]);
        }
    }

    if (m_RenderOutline)
    {
        RenderOutline();
    }
}

static inline void ClearOutline(
    Vector<nlVector3, DefaultAllocator>& outline);

static inline void ReserveOutline(
    Vector<nlVector3, DefaultAllocator>& outline, int capacity)
{
    if (outline.mCapacity < capacity)
    {
        Vector<nlVector3, DefaultAllocator> other(capacity, 0);
        for (int i = 0; i < outline.mSize; i++)
        {
            other.mData[i] = outline.mData[i];
        }
        other.mSize = outline.mSize;
        outline.Swap(other);
    }
}

static inline void InsertOutline(
    Vector<nlVector3, DefaultAllocator>& outline, nlVector3* at,
    const nlVector3* begin, const nlVector3* end)
{
    int size = end - begin;
    int offset = at - outline.mData;
    ReserveOutline(outline, outline.mSize + size);
    at = outline.mData + offset;
    nlVector3* t = outline.mData + outline.mSize - 1;
    while (t >= at)
    {
        *(t + size) = *t;
        t--;
    }
    while (begin != end)
    {
        *at = *begin;
        begin++;
        at++;
    }
    outline.mSize += size;
}

void ModeledScreenTransition::RenderOutline() const
{
    Vector<nlVector3, DefaultAllocator> outline;
    nlVector3 current;
    outline.mData = NULL;
    outline.mSize = 0;
    outline.mCapacity = 0;
    ReserveOutline(outline, 8);

    for (int i = 0; (u32)i < m_nModels; i++)
    {
        for (int iPacket = 0;
            (u32)iPacket < m_pModels[i].numPackets; iPacket++)
        {
            const glModelPacket& packet = m_pModels[i].packets[iPacket];
            const glModelStream* positionStream = NULL;
            for (u32 stream = 0; stream < packet.numStreams; stream++)
            {
                if (packet.streams[stream].id == 1)
                {
                    positionStream = &packet.streams[stream];
                    break;
                }
            }

            const nlVector3* positions
                = (const nlVector3*)positionStream->address;
            const u16* indices = packet.indexBuffer;
            for (int iVertex = 0;
                iVertex < (int)packet.numVertices; iVertex++)
            {
                current = positions[*indices++];
                nlMultPosVectorMatrix(current, current,
                    m_pPoseAccumulator->GetNodeMatrix(m_pModelMap[i]));
                InsertOutline(outline, outline.mData + outline.mSize,
                    &current, &current + 1);
            }

            ShuffleIntoOutline(outline);

            State_802A87F8 mesh;
            fn_802A87F8(&mesh);
            glSetDefaultState(true);
            glSetCurrentMatrix(glGetIdentityMatrix());
            glSetRasterState(GLS_AlphaBlend, 1);
            glSetCurrentRasterState(glHandleizeRasterState());
            fn_802A8854(&mesh, outline.mSize + 1, GLP_LineStrip, NULL);

            for (int k = 0; k < outline.mSize; k++)
            {
                *mesh.colour++ = *(const u32*)&m_OutlineColour;
                float z = outline.mData[k].z;
                float y = outline.mData[k].y;
                float x = outline.mData[k].x;
                *mesh.position++ = x;
                *mesh.position++ = y;
                *mesh.position++ = z;
            }

            *mesh.colour++ = *(const u32*)&m_OutlineColour;
            float z = outline.mData[0].z;
            float y = outline.mData[0].y;
            float x = outline.mData[0].x;
            *mesh.position++ = x;
            *mesh.position++ = y;
            *mesh.position++ = z;

            if (fn_802A89AC(&mesh))
            {
                s_3DView->AttachModel(mesh.model, 2);
            }

            ClearOutline(outline);
            fn_802A8814(&mesh, -1);
        }
    }
}

static inline void ClearOutline(
    Vector<nlVector3, DefaultAllocator>& outline)
{
    for (int i = 0; i < outline.mSize; i++)
    {
        outline.mData[i] = nlVector3();
    }
    outline.mSize = 0;
}

bool ModeledScreenTransition::IsFinished()
{
    if (!m_pPoseTree)
    {
        return true;
    }

    return m_pPoseTree->m_ePlayMode == PM_HOLD
        && m_pPoseTree->m_fTime == 1.0f;
}

float ModeledScreenTransition::Time() const
{
    if (m_pPoseTree != NULL)
    {
        return m_pPoseTree->m_fTime;
    }
    return 0.0f;
}

void ModeledScreenTransition::Reset()
{
    cPN_SAnimController* controller;

    if (m_pPoseTree == NULL && m_pAnim != NULL)
    {
        controller = new cPN_SAnimController(
            m_pAnim, NULL, PM_HOLD, NULL, 0, false);
        m_pPoseTree = controller;
    }

    if (m_pPoseTree != NULL)
    {
        m_pPoseTree->SetTime(0.0f);
    }

    m_bEnableGrab = m_bScreenGrab;

    if (m_EffectName[0] != '\0')
    {
        const int numLeafNodes
            = GetNumLeafNodesInHierarchy(*m_pSkeleton, 0, 0);
        if (m_Effects == NULL)
        {
            m_Effects = (EmissionController**)nlMalloc(
                numLeafNodes * sizeof(EmissionController*), 8, false);
        }

        for (int i = 0; i < numLeafNodes; i++)
        {
            EmissionManager* manager = EmissionManager::Instance();
            EffectsGroup* group = fn_802E7CDC(manager, m_EffectName);
            m_Effects[i]
                = fn_802E7FE4(EmissionManager::Instance(), group,
                    0, true, false);
            m_Effects[i]->m_pContext = s_3DView;
        }
    }
}

void ModeledScreenTransition::Cancel()
{
    delete m_pPoseTree;
    m_pPoseTree = NULL;

    delete[] m_Effects;
    m_Effects = NULL;
}

ModeledScreenTransition* ModeledScreenTransition::LoadFromParser(
    SimpleParser* parser)
{
    char* pToken = parser->NextToken(true);

    while (pToken != NULL)
    {
        if (nlStrCmp(pToken, "texture") == 0)
        {
            m_nTexture = glHash(parser->NextTokenOnLine(true));
            m_Unknown20
                = fn_802CE1B8(fn_802CDF0C(), m_nTexture);
        }
        else if (nlStrCmp(pToken, "name") == 0)
        {
            pToken = parser->NextTokenOnLine(true);
            Load(pToken);
        }
        else if (nlStrCmp(pToken, "screengrab") == 0)
        {
            m_bScreenGrab = true;
        }
        else if (nlStrCmp(pToken, "effect") == 0)
        {
            char* effect = parser->NextTokenOnLine(true);
            nlStrNCpy(m_EffectName, effect, 64);
        }
        else if (nlStrCmp(pToken, "outline") == 0)
        {
            m_RenderOutline = true;
        }
        else if (nlStrCmp(pToken, "outline_colour") == 0)
        {
            m_OutlineColour.c[0]
                = atoi(parser->NextTokenOnLine(true));
            m_OutlineColour.c[1]
                = atoi(parser->NextTokenOnLine(true));
            m_OutlineColour.c[2]
                = atoi(parser->NextTokenOnLine(true));
            m_OutlineColour.c[3]
                = atoi(parser->NextTokenOnLine(true));
        }
        else if (nlStrCmp(pToken, "end") == 0)
        {
            break;
        }

        pToken = parser->NextToken(true);
    }

    m_pPoseAccumulator = new (nlMalloc(
        sizeof(cPoseAccumulator), 8, false))
        cPoseAccumulator(m_pSkeleton, false);

    FixupModel();
}

void ModeledScreenTransition::CreateInstance(
    TransitionModelStore& modelInfo)
{
    m_nModels = modelInfo.nModels;
    m_pModels = glModelDupArrayNoStreams(modelInfo.pModels,
        modelInfo.nModels, true, fn_802CC094());
}

void ModeledScreenTransition::Load(const char* szName)
{
    unsigned long fileSize = 0;
    TransitionModelStore* pModelStore;
    unsigned long hash;
    TransitionModelStore newStore;
    char buf[128];
    hash = glHash(szName);

    if (ModeledScreenTransition::g_ModelInventory.FindGet(
            hash, &pModelStore))
    {
        CreateInstance(*pModelStore);
    }
    else
    {
        glSetIgnoreDuplicateModels(true);

        nlSNPrintf(buf, 128, "art/transitions/%s.rlg", szName);
        m_pModels = fn_802C8208(buf, &m_nModels, fn_802CC094());

        glSetIgnoreDuplicateModels(false);

        newStore.pModels = m_pModels;
        newStore.nModels = m_nModels;
        ModeledScreenTransition::g_ModelInventory.Add(hash, newStore);
    }

    nlSNPrintf(buf, 128, "art/transitions/%s.sanim", szName);
    m_pAnimFile = (char*)nlLoadEntireFile(
        buf, &fileSize, 0x20, AllocateStart, NULL, 0, NULL);
    m_pAnim = cSAnim::Initialize((nlChunk*)m_pAnimFile);

    nlSNPrintf(buf, 128, "art/transitions/%s.shier", szName);
    m_pSkelFile = (char*)nlLoadEntireFile(
        buf, &fileSize, 0x20, AllocateStart, NULL, 0, NULL);
    m_pSkeleton = cSHierarchy::Initialize((nlChunk*)m_pSkelFile);

    m_pModelMap = (int*)nlMalloc(m_nModels * 4, 8, false);
    for (unsigned long i = 0; i < m_nModels; i++)
    {
        m_pModelMap[i]
            = m_pSkeleton->GetNodeIndexByID(m_pModels[i].unknown00);
    }
}

void ModeledScreenTransition::FixupModel()
{
    glStateBundle savedState;
    glStateSave(savedState);

    for (unsigned long i = 0; i < m_nModels; i++)
    {
        for (unsigned long j = 0; j < m_pModels[i].numPackets; j++)
        {
            glSetCurrentRasterState(
                m_pModels[i].packets[j].rasterState);
            glSetRasterState(GLS_AlphaBlend, 0);
            glSetRasterState(GLS_Culling, 0);
            glSetRasterState(GLS_DepthTest, 1);
            glSetRasterState(GLS_DepthWrite, 1);

            if (m_nTexture != 0xFFFFFFFF)
            {
                fn_802CC458(&m_pModels[i].packets[j],
                    lbl_806E1F0C, m_nTexture);
                unsigned long unknown20 = m_Unknown20;
                fn_802CC4FC(&m_pModels[i].packets[j],
                    lbl_806E1F0C, &unknown20);
            }

            m_pModels[i].packets[j].rasterState
                = glGetCurrentRasterState();
        }
    }

    glStateRestore(savedState);
}

float ModeledScreenTransition::GetTransitionLength()
{
    if (m_pAnim == NULL)
    {
        return 0.0f;
    }
    return m_pAnim->m_nNumKeys / 30.0f;
}
