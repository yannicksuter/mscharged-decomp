#include "Game/GL/GLInventory.h"

#include "Game/SAnim.h"
#include "Game/SHierarchy.h"
#include "NL/gl/glModel.h"

#include <string.h>

inline void* operator new(unsigned long, void* memory)
{
    return memory;
}

extern "C" GLSkinMesh* fn_8036A378(
    nlChunk*, GLMaterialList*, cSHierarchy*);
extern "C" void fn_802D3C1C(GLTextureAnim*, float);
extern "C" void fn_802D3F7C(glModel*, float);

GLInventory::GLInventory()
{
    m_bCreated = false;
    for (int i = 0; i < 16; i++)
    {
        m_pFileData[i] = 0;
        m_pSkinData[i] = 0;
        m_pTextureAnims[i] = 0;
        m_pModels[i] = 0;
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

void GLInventory::ReleaseLevel(int nLevel)
{
    DeleteFileEntries(m_pFileData[nLevel]->m_Head);
    m_pFileData[nLevel]->Clear();
    m_pSkinData[nLevel]->Release();
    m_pMaterialLists[nLevel]->Release();
    m_pTextureAnims[nLevel]->Release();
    m_pModels[nLevel]->Release();
    m_pVertexAnims[nLevel]->Release();
}

GLInventory::~GLInventory()
{
    m_bCreated = false;
    for (int i = 0; i < 16; i++)
    {
        ReleaseLevel(i);
        delete m_pFileData[i];
        delete m_pSkinData[i];
        delete m_pMaterialLists[i];
        delete m_pTextureAnims[i];
        delete m_pModels[i];
        delete m_pVertexAnims[i];
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

        deleting_GLInventory<GLMaterialList>* pMaterialLists =
            (deleting_GLInventory<GLMaterialList>*)nlMalloc(
                sizeof(deleting_GLInventory<GLMaterialList>), 8, false);
        if (pMaterialLists != 0)
        {
            pMaterialLists->m_pItems = new (nlMalloc(
                sizeof(deleting_GLInventory<GLMaterialList>::Tree), 8, false))
                deleting_GLInventory<GLMaterialList>::Tree();
        }
        m_pMaterialLists[i] = pMaterialLists;

        deleting_GLInventory<GLTextureAnim>* pTextureAnims =
            (deleting_GLInventory<GLTextureAnim>*)nlMalloc(
                sizeof(deleting_GLInventory<GLTextureAnim>), 8, false);
        if (pTextureAnims != 0)
        {
            pTextureAnims->m_pItems = new (nlMalloc(
                sizeof(deleting_GLInventory<GLTextureAnim>::Tree), 8, false))
                deleting_GLInventory<GLTextureAnim>::Tree();
        }
        m_pTextureAnims[i] = pTextureAnims;

        deleting_GLInventory<glModel>* pModels =
            (deleting_GLInventory<glModel>*)nlMalloc(
                sizeof(deleting_GLInventory<glModel>), 8, false);
        if (pModels != 0)
        {
            pModels->m_pItems = new (nlMalloc(
                sizeof(deleting_GLInventory<glModel>::Tree), 8, false))
                deleting_GLInventory<glModel>::Tree();
        }
        m_pModels[i] = pModels;

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
    }
}

void GLInventory::fn_802D19C4(void* value)
{
    m_Unknown000 = *(void**)value;
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

void GLInventory::AddMaterialList(
    unsigned long key, GLMaterialList* materialList)
{
    unsigned long k = key;
    GLMaterialList* value = materialList;
    deleting_GLInventory<GLMaterialList>::Tree* pItems =
        m_pMaterialLists[m_nLevel]->m_pItems;
    pItems->Add(k, value);
}

GLMaterialList* GLInventory::GetMaterialList(unsigned long id)
{
    for (int i = m_nLevel; i >= 0; i--)
    {
        unsigned long key = id;
        GLMaterialList** pResult;
        bool found = m_pMaterialLists[i]->m_pItems->FindGet(key, &pResult);
        GLMaterialList* result;
        if (found)
            result = *pResult;
        else
            result = 0;
        if (result != 0)
            return result;
    }
    return 0;
}

void GLInventory::AddVertexAnim(unsigned long key, GLVertexAnim* vertexAnim)
{
    unsigned long k = key;
    GLVertexAnim* value = vertexAnim;
    deleting_GLInventory<GLVertexAnim>::Tree* pTree =
        m_pVertexAnims[m_nLevel]->m_pItems;
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

void GLInventory::AddTextureAnim(unsigned long key, GLTextureAnim* anim)
{
    unsigned long k = key;
    GLTextureAnim* value = anim;
    deleting_GLInventory<GLTextureAnim>::Tree* pTree =
        m_pTextureAnims[m_nLevel]->m_pItems;
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
    fn_802D3C1C(*anim, m_Unknown00);
}

void GLInventory::AddModel(unsigned long key, glModel* model)
{
    unsigned long k = key;
    glModel* value = model;
    deleting_GLInventory<glModel>::Tree* pTree =
        m_pModels[m_nLevel]->m_pItems;
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

class Callback_802D2670
{
public:
    void fn_802D2670(const unsigned long&, glModel** model);
    float m_Unknown00;
};

void Callback_802D2670::fn_802D2670(
    const unsigned long&, glModel** model)
{
    fn_802D3F7C(*model, m_Unknown00);
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
    nlChunk* pChunk = 0;
    for (int i = m_nLevel; i >= 0; i--)
    {
        unsigned long key = hashID;
        nlChunk** pResult;
        bool found = m_pSkinData[i]->m_pItems->FindGet(key, &pResult);
        if (found)
            pChunk = *pResult;
        else
            pChunk = 0;
        if (pChunk != 0)
            break;
    }

    GLMaterialList* pMaterialList = 0;
    for (int i = m_nLevel; i >= 0; i--)
    {
        unsigned long key = hashID;
        GLMaterialList** pResult;
        bool found = m_pMaterialLists[i]->m_pItems->FindGet(key, &pResult);
        if (found)
            pMaterialList = *pResult;
        else
            pMaterialList = 0;
        if (pMaterialList != 0)
            break;
    }

    return fn_8036A378(pChunk, pMaterialList, hierarchy);
}

void GLInventory::Update(float deltaTime)
{
    Callback_802D254C callback_802D254C;
    callback_802D254C.m_Unknown00 = deltaTime;
    for (int i = m_nLevel; i >= 0; i--)
    {
        deleting_GLInventory<GLTextureAnim>::Tree* tree =
            m_pTextureAnims[i]->m_pItems;
        tree->InorderWalk(tree->m_Root, &callback_802D254C,
            &Callback_802D254C::fn_802D254C);
    }

    Callback_802D2670 callback_802D2670;
    callback_802D2670.m_Unknown00 = deltaTime;
    for (int i = m_nLevel; i >= 0; i--)
    {
        deleting_GLInventory<glModel>::Tree* tree =
            m_pModels[i]->m_pItems;
        tree->InorderWalk(tree->m_Root, &callback_802D2670,
            &Callback_802D2670::fn_802D2670);
    }
}
