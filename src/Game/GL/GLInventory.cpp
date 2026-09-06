#include "Game/GL/GLInventory.h"

#include "Game/GL/GLTextureAnim.h"
#include "Game/GL/GLVertexAnim.h"
#include "Game/SAnim.h"
#include "Game/SHierarchy.h"
#include "NL/gl/glModel.h"
#include "NL/glx/glxLoadModel.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlMemory.h"

#include <string.h>

GLInventory::GLInventory()
{
    m_bCreated = false;
    for (int i = 0; i < 16; i++)
    {
        m_pFileData[i] = 0;
        m_pSkinData[i] = 0;
        m_pTextureAnims[i] = 0;
        m_pVertexAnims[i] = 0;
    }
    m_nLevel = 0;
    memset(&m_Unknown000, 0, sizeof(m_Unknown000));
}

static inline void DeleteFileEntries(ListEntry<void*>* current)
{
    while (current != 0)
    {
        operator delete(current->entry);
        current = current->next;
    }
}

inline void GLInventory::ReleaseLevel(int nLevel)
{
    DeleteFileEntries(m_pFileData[nLevel]->m_Head);
    m_pFileData[nLevel]->Clear();
    m_pSkinData[nLevel]->Release();
    m_pModels[nLevel]->Release(m_Unknown000);
    m_pTextureAnims[nLevel]->Release(fn_802D3B68);
    m_pVertexAnims[nLevel]->Release();
    m_Unidentified144[nLevel]->Release(fn_802CDF5C);
}

GLInventory::~GLInventory()
{
    m_bCreated = false;
    for (int i = 0; i < 16; i++)
    {
        ReleaseLevel(i);
        delete m_pFileData[i];
        delete m_pSkinData[i];
        delete m_pTextureAnims[i];
        delete m_pVertexAnims[i];
        delete m_pModels[i];
        delete m_Unidentified144[i];
    }
}

void GLInventory::Create()
{
    m_bCreated = true;

    int i = 0;

    for (; i < 16; i++)
    {
        nlListContainer<void*>* fileData = new (
            nlMalloc(sizeof(nlListContainer<void*>), 8, false))
            nlListContainer<void*>();
        m_pFileData[i] = fileData;

        freeing_GLInventory<nlChunk>* pSkinData =
            (freeing_GLInventory<nlChunk>*)nlMalloc(
                sizeof(freeing_GLInventory<nlChunk>), 8, false);
        if (pSkinData != 0)
        {
            pSkinData->m_pItems = new (nlMalloc(
                sizeof(freeing_GLInventory<nlChunk>::Tree), 8, false))
                freeing_GLInventory<nlChunk>::Tree();
        }
        m_pSkinData[i] = pSkinData;

        UnidentifiedInventory_802D3854<glModel>* pModels = (UnidentifiedInventory_802D3854<glModel>*)nlMalloc(
            sizeof(UnidentifiedInventory_802D3854<glModel>), 8, false);
        if (pModels != 0)
        {
            pModels->m_pItems = new (nlMalloc(
                sizeof(UnidentifiedInventory_802D3854<glModel>::Tree), 8, false))
                UnidentifiedInventory_802D3854<glModel>::Tree();
        }
        m_pModels[i] = pModels;

        UnidentifiedInventory_802D3854<GLTextureAnim>* pTextureAnims = (UnidentifiedInventory_802D3854<GLTextureAnim>*)nlMalloc(
            sizeof(UnidentifiedInventory_802D3854<GLTextureAnim>), 8, false);
        if (pTextureAnims != 0)
        {
            pTextureAnims->m_pItems = new (nlMalloc(
                sizeof(UnidentifiedInventory_802D3854<GLTextureAnim>::Tree), 8, false))
                UnidentifiedInventory_802D3854<GLTextureAnim>::Tree();
        }
        m_pTextureAnims[i] = pTextureAnims;

        deleting_GLInventory<GLVertexAnim>* pVertexAnims =
            (deleting_GLInventory<GLVertexAnim>*)nlMalloc(
                sizeof(deleting_GLInventory<GLVertexAnim>), 8, false);
        if (pVertexAnims != 0)
        {
            pVertexAnims->m_pItems = new (nlMalloc(
                sizeof(deleting_GLInventory<GLVertexAnim>::Tree), 8, false))
                deleting_GLInventory<GLVertexAnim>::Tree();
        }
        m_pVertexAnims[i] = pVertexAnims;

        UnidentifiedInventory_802D3854<PlatTexture>* pUnidentified144 = (UnidentifiedInventory_802D3854<PlatTexture>*)nlMalloc(
            sizeof(UnidentifiedInventory_802D3854<PlatTexture>), 8, false);
        if (pUnidentified144 != 0)
        {
            pUnidentified144->m_pItems = new (nlMalloc(
                sizeof(UnidentifiedInventory_802D3854<PlatTexture>::Tree), 8, false))
                UnidentifiedInventory_802D3854<PlatTexture>::Tree();
        }
        m_Unidentified144[i] = pUnidentified144;
    }
}

void GLInventory::fn_802D19C4(void* value)
{
    m_Unknown000 = *(void (**)(glModel*))value;
}

void GLInventory::ResourceMark()
{
    m_nLevel++;
}

void GLInventory::ResourceRelease(int nLevel)
{
    while (m_nLevel != nLevel)
    {
        ReleaseLevel(m_nLevel);
        m_nLevel--;
    }
}

void GLInventory::AddModel(
    unsigned long key, glModel* model)
{
    unsigned long k = key;
    glModel* value = model;
    UnidentifiedInventory_802D3854<glModel>::Tree* pTree = m_pModels[m_nLevel]->m_pItems;
    pTree->Add(k, value);
}

glModel* GLInventory::GetModel(unsigned long id)
{
    for (int i = m_nLevel; i >= 0; i--)
    {
        unsigned long key = id;
        glModel** pResult;
        bool found = m_pModels[i]->m_pItems->FindGet(key, &pResult);
        glModel* result;
        if (found)
            result = *pResult;
        else
            result = 0;
        if (result != 0)
            return result;
    }
    return 0;
}

void GLInventory::fn_802D2324(unsigned long key, PlatTexture* texture)
{
    unsigned long k = key;
    PlatTexture* value = texture;
    UnidentifiedInventory_802D3854<PlatTexture>::Tree* pTree = m_Unidentified144[m_nLevel]->m_pItems;
    pTree->Add(k, value);
}

PlatTexture* GLInventory::fn_802D2370(unsigned long id)
{
    for (int i = m_nLevel; i >= 0; i--)
    {
        unsigned long key = id;
        PlatTexture** pResult;
        bool found = m_Unidentified144[i]->m_pItems->FindGet(key, &pResult);
        PlatTexture* result;
        if (found)
            result = *pResult;
        else
            result = 0;
        if (result != 0)
            return result;
    }
    return 0;
}

void GLInventory::AddTextureAnim(unsigned long key, GLTextureAnim* anim)
{
    unsigned long k = key;
    GLTextureAnim* value = anim;
    UnidentifiedInventory_802D3854<GLTextureAnim>::Tree* pTree = m_pTextureAnims[m_nLevel]->m_pItems;
    pTree->Add(k, value);
}

GLTextureAnim* GLInventory::GetTextureAnim(unsigned long id)
{
    for (int i = m_nLevel; i >= 0; i--)
    {
        unsigned long key = id;
        GLTextureAnim** foundValue;
        bool found =
            m_pTextureAnims[i]->m_pItems->FindGet(key, &foundValue);
        GLTextureAnim* result;
        if (found)
            result = *foundValue;
        else
            result = 0;
        if (result != 0)
            return result;
    }
    return 0;
}

class Callback_802D254C
{
public:
    void fn_802D254C(const unsigned long&, GLTextureAnim** anim);
    float m_Unknown00;
};

void Callback_802D254C::fn_802D254C(
    const unsigned long&, GLTextureAnim** anim)
{
    (*anim)->Update(m_Unknown00);
}

void GLInventory::AddVertexAnim(unsigned long key, GLVertexAnim* vertexAnim)
{
    unsigned long k = key;
    GLVertexAnim* value = vertexAnim;
    deleting_GLInventory<GLVertexAnim>::Tree* pTree = m_pVertexAnims[m_nLevel]->m_pItems;
    pTree->Add(k, value);
}

GLVertexAnim* GLInventory::GetVertexAnim(unsigned long id)
{
    for (int i = m_nLevel; i >= 0; i--)
    {
        unsigned long key = id;
        GLVertexAnim** pResult;
        bool found = m_pVertexAnims[i]->m_pItems->FindGet(key, &pResult);
        GLVertexAnim* result;
        if (found)
            result = *pResult;
        else
            result = 0;
        if (result != 0)
            return result;
    }
    return 0;
}

class Callback_802D2670
{
public:
    void fn_802D2670(const unsigned long&, GLVertexAnim** vertexAnim);
    float m_Unknown00;
};

void Callback_802D2670::fn_802D2670(
    const unsigned long&, GLVertexAnim** vertexAnim)
{
    (*vertexAnim)->Update(m_Unknown00);
}

void GLInventory::AddSkinData(unsigned long key, nlChunk* skinData)
{
    unsigned long key2 = key;
    nlChunk* skinData2 = skinData;
    freeing_GLInventory<nlChunk>* pSkinData = m_pSkinData[m_nLevel];
    freeing_GLInventory<nlChunk>::Tree* tree = pSkinData->m_pItems;
    tree->Add(key2, skinData2);
}

GLSkinMesh* GLInventory::MakeSkinMesh(
    unsigned long hashID, cSHierarchy* hierarchy)
{
    struct SkinDataHelper
    {
        static inline nlChunk* Get(GLInventory* self, unsigned long id)
        {
            nlChunk* result;
            int i;
            for (i = self->m_nLevel; i >= 0; i--)
            {
                unsigned long key = id;
                nlChunk** pResult;
                bool found =
                    self->m_pSkinData[i]->m_pItems->FindGet(key, &pResult);
                if (found)
                    result = *pResult;
                else
                    result = 0;
                if (result != 0)
                    return result;
            }
            return 0;
        }
    };

    nlChunk* foundChunk = SkinDataHelper::Get(this, hashID);
    nlChunk* pChunk = foundChunk;
    glModel* pModel = GetModel(hashID);

    return glx_MakeSkinMesh(pChunk, pModel, hierarchy);
}

void GLInventory::Update(float deltaTime)
{
    Callback_802D254C callback_802D254C;
    callback_802D254C.m_Unknown00 = deltaTime;
    for (int i = m_nLevel; i >= 0; i--)
    {
        UnidentifiedInventory_802D3854<GLTextureAnim>::Tree* tree = m_pTextureAnims[i]->m_pItems;
        tree->InorderWalk(tree->m_Root, &callback_802D254C,
            &Callback_802D254C::fn_802D254C);
    }

    Callback_802D2670 callback_802D2670;
    callback_802D2670.m_Unknown00 = deltaTime;
    for (int i = m_nLevel; i >= 0; i--)
    {
        deleting_GLInventory<GLVertexAnim>::Tree* tree = m_pVertexAnims[i]->m_pItems;
        tree->InorderWalk(tree->m_Root, &callback_802D2670,
            &Callback_802D2670::fn_802D2670);
    }
}
