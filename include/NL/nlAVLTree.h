#ifndef NL_AVL_TREE_H
#define NL_AVL_TREE_H

#include "NL/nlList.h"
#include "types.h"

struct AVLTreeNode
{
    AVLTreeNode* left;
    AVLTreeNode* right;
    s8 heavy;
};

class AVLTreeUntemplated
{
public:
    typedef void (*DeleteCallback)(AVLTreeUntemplated*, AVLTreeNode*);

    virtual int CompareNodes(AVLTreeNode* node1, AVLTreeNode* node2) = 0;
    virtual int CompareKey(void* key, AVLTreeNode* node) = 0;
    virtual AVLTreeNode* AllocateEntry(void* key, void* value) = 0;

    AVLTreeNode* AddAVLNode(AVLTreeNode** rootNode, void* key, void* value, AVLTreeNode** existingNode);
    AVLTreeNode* RemoveAVLNode(AVLTreeNode** root, void* key);
    AVLTreeNode* FindAVLNode(AVLTreeNode* root, void* key) const;
    void DestroyTree(AVLTreeNode* root, DeleteCallback callback);
};

template <typename KeyType, typename ValueType>
class AVLTreeEntry
{
public:
    AVLTreeNode node;
    KeyType key;
    ValueType value;
};

template <typename KeyType>
class DefaultKeyCompare
{
public:
    int operator()(const KeyType& key1, const KeyType& key2) const
    {
        if (key1 == key2)
            return 0;
        if (key1 < key2)
            return -1;
        return 1;
    }
};

template <typename KeyType, typename ValueType, typename AllocatorType, typename CompareType>
class AVLTreeBase : public AVLTreeUntemplated
{
public:
    typedef AVLTreeEntry<KeyType, ValueType> Entry;

    AVLTreeBase()
        : m_Root(0)
        , m_Compare(0)
    {
    }

    ~AVLTreeBase()
    {
        Clear();
    }

    void Clear()
    {
        if (m_Root != 0)
        {
            DestroyTree((AVLTreeNode*)m_Root, DeleteEntry);
            m_Root = 0;
        }
    }

    bool FindGet(const KeyType& key, ValueType** foundValue) const
    {
        Entry* node = (Entry*)FindAVLNode((AVLTreeNode*)m_Root, (void*)&key);
        if (node == 0)
            return false;

        if (foundValue != 0)
            *foundValue = &node->value;
        return true;
    }

    static void DeleteEntry(AVLTreeUntemplated*, AVLTreeNode* entry)
    {
        delete entry;
    }

    virtual int CompareNodes(AVLTreeNode* node1, AVLTreeNode* node2)
    {
        CompareType compare;
        return compare(((Entry*)node1)->key, ((Entry*)node2)->key);
    }

    virtual int CompareKey(void* key, AVLTreeNode* node)
    {
        CompareType compare;
        return compare(*(KeyType*)key, ((Entry*)node)->key);
    }

    virtual AVLTreeNode* AllocateEntry(void* key, void* value)
    {
        Entry* newNode = (Entry*)nlMalloc(sizeof(Entry), 8, false);
        newNode->node.left = 0;
        newNode->node.right = 0;
        newNode->node.heavy = 0;
        newNode->key = *(KeyType*)key;
        if (value != 0)
            newNode->value = *(ValueType*)value;
        return (AVLTreeNode*)newNode;
    }

    AllocatorType m_Allocator;
    Entry* m_Root;
    CompareType* m_Compare;
};

template <typename KeyType, typename ValueType, typename CompareType>
class nlAVLTree
    : public AVLTreeBase<KeyType, ValueType,
          NewAdapter<AVLTreeEntry<KeyType, ValueType> >, CompareType>
{
};

#endif // NL_AVL_TREE_H
