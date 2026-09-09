#include "Game/GL/GLInventory.h"

#include "Game/GL/GLTextureAnim.h"
#include "Game/GL/GLVertexAnim.h"
#include "Game/SAnim.h"
#include "Game/SHierarchy.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glTexture.h"
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
    memset(&mModelReleaseCallback, 0, sizeof(mModelReleaseCallback));
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
    m_pModels[nLevel]->Release(mModelReleaseCallback);
    m_pTextureAnims[nLevel]->Release(glReleaseTextureAnim);
    m_pVertexAnims[nLevel]->Release();
    m_pTextures[nLevel]->Release(glReleaseTexture);
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
        delete m_pTextures[i];
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

        clearing_GLInventory<glModel>* pModels = (clearing_GLInventory<glModel>*)nlMalloc(
            sizeof(clearing_GLInventory<glModel>), 8, false);
        if (pModels != 0)
        {
            pModels->m_pItems = new (nlMalloc(
                sizeof(clearing_GLInventory<glModel>::Tree), 8, false))
                clearing_GLInventory<glModel>::Tree();
        }
        m_pModels[i] = pModels;

        clearing_GLInventory<GLTextureAnim>* pTextureAnims = (clearing_GLInventory<GLTextureAnim>*)nlMalloc(
            sizeof(clearing_GLInventory<GLTextureAnim>), 8, false);
        if (pTextureAnims != 0)
        {
            pTextureAnims->m_pItems = new (nlMalloc(
                sizeof(clearing_GLInventory<GLTextureAnim>::Tree), 8, false))
                clearing_GLInventory<GLTextureAnim>::Tree();
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

        clearing_GLInventory<PlatTexture>* pTextures = (clearing_GLInventory<PlatTexture>*)nlMalloc(
            sizeof(clearing_GLInventory<PlatTexture>), 8, false);
        if (pTextures != 0)
        {
            pTextures->m_pItems = new (nlMalloc(
                sizeof(clearing_GLInventory<PlatTexture>::Tree), 8, false))
                clearing_GLInventory<PlatTexture>::Tree();
        }
        m_pTextures[i] = pTextures;
    }
}

void GLInventory::SetModelReleaseCallback(const ModelReleaseCallback& callback)
{
    mModelReleaseCallback = callback;
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
    clearing_GLInventory<glModel>::Tree* pTree = m_pModels[m_nLevel]->m_pItems;
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

void GLInventory::AddTexture(unsigned long key, PlatTexture* texture)
{
    unsigned long k = key;
    PlatTexture* value = texture;
    clearing_GLInventory<PlatTexture>::Tree* pTree = m_pTextures[m_nLevel]->m_pItems;
    pTree->Add(k, value);
}

PlatTexture* GLInventory::GetTexture(unsigned long id)
{
    for (int i = m_nLevel; i >= 0; i--)
    {
        unsigned long key = id;
        PlatTexture** pResult;
        bool found = m_pTextures[i]->m_pItems->FindGet(key, &pResult);
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
    clearing_GLInventory<GLTextureAnim>::Tree* pTree = m_pTextureAnims[m_nLevel]->m_pItems;
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

class UpdateTextureAnimCallback
{
public:
    void Update(const unsigned long&, GLTextureAnim** anim);
    float mDeltaTime;
};

void UpdateTextureAnimCallback::Update(
    const unsigned long&, GLTextureAnim** anim)
{
    (*anim)->Update(mDeltaTime);
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

class UpdateVertexAnimCallback
{
public:
    void Update(const unsigned long&, GLVertexAnim** vertexAnim);
    float mDeltaTime;
};

void UpdateVertexAnimCallback::Update(
    const unsigned long&, GLVertexAnim** vertexAnim)
{
    (*vertexAnim)->Update(mDeltaTime);
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
    UpdateTextureAnimCallback textureCallback;
    textureCallback.mDeltaTime = deltaTime;
    for (int i = m_nLevel; i >= 0; i--)
    {
        clearing_GLInventory<GLTextureAnim>::Tree* tree = m_pTextureAnims[i]->m_pItems;
        tree->InorderWalk(tree->m_Root, &textureCallback,
            &UpdateTextureAnimCallback::Update);
    }

    UpdateVertexAnimCallback vertexCallback;
    vertexCallback.mDeltaTime = deltaTime;
    for (int i = m_nLevel; i >= 0; i--)
    {
        deleting_GLInventory<GLVertexAnim>::Tree* tree = m_pVertexAnims[i]->m_pItems;
        tree->InorderWalk(tree->m_Root, &vertexCallback,
            &UpdateVertexAnimCallback::Update);
    }
}
