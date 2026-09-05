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
            m_pItems->m_Root, this, &freeing_GLInventory::fn_Unknown);
        m_pItems->Clear();
    }

    void fn_Unknown(const unsigned long&, ValueType** value)
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
            m_pItems->m_Root, this, &deleting_GLInventory::fn_Unknown);
        m_pItems->Clear();
    }

    void fn_Unknown(const unsigned long&, ValueType** value)
    {
        delete *value;
    }

    Tree* m_pItems;
};

template <typename ValueType>
class UnidentifiedInventory_802D3854
{
public:
    typedef nlAVLTree<unsigned long, ValueType*,
        DefaultKeyCompare<unsigned long> >
        Tree;

    struct Callback
    {
        void fn_Unknown(const unsigned long&, ValueType** value)
        {
            m_00(*value);
        }

        void (*m_00)(ValueType*);
    };

    ~UnidentifiedInventory_802D3854()
    {
        m_pItems->Clear();
        delete m_pItems;
    }

    void Release(void (*fn)(ValueType*))
    {
        if (fn != 0)
        {
            Callback callback;
            callback.m_00 = fn;
            m_pItems->InorderWalk(
                m_pItems->m_Root, &callback, &Callback::fn_Unknown);
        }
        m_pItems->Clear();
    }

    Tree* m_pItems;
};

class GLInventory
{
public:
    GLInventory();
    ~GLInventory();

    void Create();
    void fn_802D19C4(void* value);
    void ResourceMark();
    void ResourceRelease(int nLevel);
    void ReleaseLevel(int nLevel);

    void AddModel(unsigned long key, glModel* model);
    glModel* GetModel(unsigned long id);
    void fn_802D2324(unsigned long key, PlatTexture* texture);
    PlatTexture* fn_802D2370(unsigned long id);
    void AddTextureAnim(unsigned long key, GLTextureAnim* anim);
    GLTextureAnim* GetTextureAnim(unsigned long id);
    void AddVertexAnim(unsigned long key, GLVertexAnim* vertexAnim);
    GLVertexAnim* GetVertexAnim(unsigned long id);
    void AddSkinData(unsigned long key, nlChunk* skinData);
    GLSkinMesh* MakeSkinMesh(unsigned long hashID, cSHierarchy* hierarchy);
    void Update(float deltaTime);

    /* 0x000 */ void (*m_Unknown000)(glModel*);
    /* 0x004 */ nlListContainer<void*>* m_pFileData[16];
    /* 0x044 */ freeing_GLInventory<nlChunk>* m_pSkinData[16];
    /* 0x084 */ UnidentifiedInventory_802D3854<glModel>* m_pModels[16];
    /* 0x0C4 */ UnidentifiedInventory_802D3854<GLTextureAnim>* m_pTextureAnims[16];
    /* 0x104 */ deleting_GLInventory<GLVertexAnim>* m_pVertexAnims[16];
    /* 0x144 */ UnidentifiedInventory_802D3854<PlatTexture>* m_Unidentified144[16];
    /* 0x184 */ int m_nLevel;
    /* 0x188 */ unsigned char m_bCreated;
};

#endif // GAME_GL_GLINVENTORY_H
