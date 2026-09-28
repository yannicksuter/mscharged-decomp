#include "NL/gl/glMaterialProgram.h"
#define NL_AVL_TREE_DEFER_DELETE_ENTRY
#include "NL/nlAVLTree.h"
#undef NL_AVL_TREE_DEFER_DELETE_ENTRY

typedef nlAVLTree<unsigned long, void*, DefaultKeyCompare<unsigned long> > MaterialProgramTree;

static MaterialProgramTree sMaterialPrograms;

template <typename KeyType, typename ValueType, typename AllocatorType, typename CompareType>
inline void AVLTreeBase<KeyType, ValueType, AllocatorType, CompareType>::DeleteEntry(
    AVLTreeUntemplated* tree, AVLTreeNode* entry)
{
    Entry* e = (Entry*)entry;
    ((AVLTreeBase*)tree)->m_Allocator.Delete(e);
}

void glRegisterMaterialProgram(void* program, unsigned long hash)
{
    sMaterialPrograms.Add(hash, program);
}

void* glGetMaterialProgram(unsigned long hash)
{
    void** program = 0;
    if (sMaterialPrograms.FindGet(hash, &program))
        return *program;
    return 0;
}

void glForEachMaterialProgram(MaterialProgramCallback* callback)
{
    sMaterialPrograms.Walk(callback);
}
