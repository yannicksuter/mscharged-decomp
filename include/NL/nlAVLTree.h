#ifndef NL_AVL_TREE_H
#define NL_AVL_TREE_H

#include "NL/nlList.h"
#include "NL/nlSlotPool.h"
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

template <typename KeyType, typename ValueType, typename CompareType>
class nlAVLTreeIterator;

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

    void Find(const KeyType& key, ValueType** foundValue,
        KeyType** foundKey) const
    {
        Entry* node = (Entry*)FindAVLNode(
            (AVLTreeNode*)m_Root, (void*)&key);
        if (node == 0)
            return;

        if (foundValue != 0)
            *foundValue = &node->value;
        if (foundKey != 0)
            *foundKey = &node->key;
    }

    ValueType* Add(const KeyType& key, const ValueType& value)
    {
        AVLTreeNode* existingNode;
        AddAVLNode((AVLTreeNode**)&m_Root, (void*)&key, (void*)&value, &existingNode);
        if (existingNode == 0)
            return 0;
        return &((Entry*)existingNode)->value;
    }

    template <typename CallbackType>
    void Walk(CallbackType* cbClass,
        void (CallbackType::*cb)(const KeyType&, ValueType*))
    {
        InorderWalk(m_Root, cbClass, cb);
    }

    template <typename CallbackType>
    void InorderWalk(Entry* curr, CallbackType* cbClass,
        void (CallbackType::*cb)(const KeyType&, ValueType*))
    {
        while (curr != 0)
        {
            InorderWalk((Entry*)curr->node.left, cbClass, cb);
            (cbClass->*cb)(curr->key, &curr->value);
            curr = (Entry*)curr->node.right;
        }
    }

    nlAVLTreeIterator<KeyType, ValueType, CompareType>* GetIterator();

    static void DeleteEntry(AVLTreeUntemplated* tree, AVLTreeNode* entry)
    {
        AVLTreeBase* self = (AVLTreeBase*)tree;
        self->m_Allocator.DeleteEntry((Entry*)entry);
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
        Entry* newNode;
        m_Allocator.Allocate(newNode);
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

template <typename KeyType, typename ValueType, typename CompareType>
class nlAVLTreeSlotPool
    : public AVLTreeBase<KeyType, ValueType,
          BasicSlotPool<AVLTreeEntry<KeyType, ValueType> >, CompareType>
{
public:
    nlAVLTreeSlotPool()
    {
    }

    nlAVLTreeSlotPool(int initial, int delta)
    {
        this->m_Allocator.Initialize(initial, delta);
    }

    ~nlAVLTreeSlotPool()
    {
        this->Clear();
        this->m_Allocator.FreeBlocks();
    }
};

template <typename KeyType, typename ValueType, typename CompareType>
class nlAVLTreeIterator
{
public:
    typedef AVLTreeEntry<KeyType, ValueType> Entry;

    nlAVLTreeIterator()
        : m_NumStackEntries(0)
    {
    }

    void Initialize(Entry* entry)
    {
        m_NumStackEntries = 0;
        if (entry != 0)
            PushLeft(entry);
    }

    void PushLeft(Entry* entry)
    {
        while (entry->node.left != 0)
        {
            m_Stack[m_NumStackEntries] = entry;
            ++m_NumStackEntries;
            entry = (Entry*)entry->node.left;
        }
        m_Stack[m_NumStackEntries] = entry;
        ++m_NumStackEntries;
    }

    void Next()
    {
        --m_NumStackEntries;
        Entry* entry = m_Stack[m_NumStackEntries];
        Entry* right = (Entry*)entry->node.right;
        if (right != 0)
            PushLeft(right);
    }

    bool IsValid() const
    {
        return m_NumStackEntries != 0;
    }

    Entry* Current() const
    {
        return m_Stack[m_NumStackEntries - 1];
    }

    Entry* m_Stack[32];
    unsigned int m_NumStackEntries;
};

template <typename KeyType, typename ValueType, typename AllocatorType,
    typename CompareType>
inline nlAVLTreeIterator<KeyType, ValueType, CompareType>*
AVLTreeBase<KeyType, ValueType, AllocatorType, CompareType>::GetIterator()
{
    typedef nlAVLTreeIterator<KeyType, ValueType, CompareType> Iterator;
    Iterator* iterator = (Iterator*)nlMalloc(sizeof(Iterator), 8, false);
    if (iterator != 0)
        iterator->m_NumStackEntries = 0;
    iterator->Initialize(m_Root);
    return iterator;
}

#endif // NL_AVL_TREE_H
