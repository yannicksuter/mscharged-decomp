#include "NL/gl/glMaterialProgram.h"
#include "NL/nlAVLTree.h"

typedef nlAVLTree<unsigned long, void*, DefaultKeyCompare<unsigned long> > MaterialProgramTree;

static MaterialProgramTree sMaterialPrograms;

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
