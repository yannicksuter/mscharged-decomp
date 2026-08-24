#ifndef NL_AVL_TREE_H
#define NL_AVL_TREE_H

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
    virtual int CompareNodes(AVLTreeNode* node1, AVLTreeNode* node2) = 0;
    virtual int CompareKey(void* key, AVLTreeNode* node) = 0;
    virtual AVLTreeNode* AllocateEntry(void* key, void* value) = 0;

    AVLTreeNode* AddAVLNode(AVLTreeNode** rootNode, void* key, void* value, AVLTreeNode** existingNode);
    AVLTreeNode* RemoveAVLNode(AVLTreeNode** root, void* key);
};

#endif // NL_AVL_TREE_H
