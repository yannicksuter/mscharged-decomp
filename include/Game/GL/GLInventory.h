#ifndef GAME_GL_GLINVENTORY_H
#define GAME_GL_GLINVENTORY_H

#include "NL/nlAVLTree.h"
#include "NL/nlList.h"

class cSHierarchy;
class GLSkinMesh;
class glModel;
class GLTextureAnim;
class PlatTexture;
class GLVertexAnim;
class nlChunk;

template <typename ValueType>
class freeing_GLInventory
{
public:
    typedef nlAVLTree<unsigned long, ValueType*,
        DefaultKeyCompare<unsigned long> > Tree;

    ~freeing_GLInventory()
    {
        Release();
        delete m_pItems;
    }

    void Release()
    {
        m_pItems->InorderWalk(
            m_pItems->m_Root, this, &freeing_GLInventory::FreeItem);
        m_pItems->Clear();
    }

    void FreeItem(const unsigned long&, ValueType** value)
    {
        nlFree(*value);
    }

    Tree* m_pItems;
};

template <typename ValueType>
class deleting_GLInventory
{
public:
    typedef nlAVLTree<unsigned long, ValueType*,
        DefaultKeyCompare<unsigned long> > Tree;

    ~deleting_GLInventory()
    {
        Release();
        delete m_pItems;
    }

    void Release()
    {
        m_pItems->InorderWalk(
            m_pItems->m_Root, this, &deleting_GLInventory::DeleteItem);
        m_pItems->Clear();
    }

    void DeleteItem(const unsigned long&, ValueType** value)
    {
        delete *value;
    }

    Tree* m_pItems;
};

template <typename ValueType>
class clearing_GLInventory
{
public:
    typedef nlAVLTree<unsigned long, ValueType*,
        DefaultKeyCompare<unsigned long> >
        Tree;

    struct Callback
    {
        void Invoke(const unsigned long&, ValueType** value)
        {
            mCallback(*value);
        }

        void (*mCallback)(ValueType*);
    };

    ~clearing_GLInventory()
    {
        m_pItems->Clear();
        delete m_pItems;
    }

    void Release(void (*fn)(ValueType*))
    {
        if (fn != 0)
        {
            Callback callback;
            callback.mCallback = fn;
            m_pItems->InorderWalk(
                m_pItems->m_Root, &callback, &Callback::Invoke);
        }
        m_pItems->Clear();
    }

    Tree* m_pItems;
};

class GLInventory
{
public:
    typedef void (*ModelReleaseCallback)(glModel*);

    GLInventory();
    ~GLInventory();

    void Create();
    void SetModelReleaseCallback(const ModelReleaseCallback& callback);
    void ResourceMark();
    void ResourceRelease(int nLevel);
    void ReleaseLevel(int nLevel);

    void AddModel(unsigned long key, glModel* model);
    glModel* GetModel(unsigned long id);
    void AddTexture(unsigned long key, PlatTexture* texture);
    PlatTexture* GetTexture(unsigned long id);
    void AddTextureAnim(unsigned long key, GLTextureAnim* anim);
    GLTextureAnim* GetTextureAnim(unsigned long id);
    void AddVertexAnim(unsigned long key, GLVertexAnim* vertexAnim);
    GLVertexAnim* GetVertexAnim(unsigned long id);
    void AddSkinData(unsigned long key, nlChunk* skinData);
    GLSkinMesh* MakeSkinMesh(unsigned long hashID, cSHierarchy* hierarchy);
    void Update(float deltaTime);

    /* 0x000 */ ModelReleaseCallback mModelReleaseCallback;
    /* 0x004 */ nlListContainer<void*>* m_pFileData[16];
    /* 0x044 */ freeing_GLInventory<nlChunk>* m_pSkinData[16];
    /* 0x084 */ clearing_GLInventory<glModel>* m_pModels[16];
    /* 0x0C4 */ clearing_GLInventory<GLTextureAnim>* m_pTextureAnims[16];
    /* 0x104 */ deleting_GLInventory<GLVertexAnim>* m_pVertexAnims[16];
    /* 0x144 */ clearing_GLInventory<PlatTexture>* m_pTextures[16];
    /* 0x184 */ int m_nLevel;
    /* 0x188 */ unsigned char m_bCreated;
};

#endif // GAME_GL_GLINVENTORY_H
