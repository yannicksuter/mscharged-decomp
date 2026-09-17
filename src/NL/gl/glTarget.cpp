#include "NL/gl/glTarget.h"
#include "NL/glx/glxTarget.h"

#include "NL/gl/gl.h"
#include "NL/nlAVLTree.h"
#include "NL/nlString.h"

typedef nlAVLTree<unsigned long, GLXTarget*, DefaultKeyCompare<unsigned long> > TargetTree;

static TargetTree targets;

static inline GLRenderPair GetBackBufferTarget()
{
    static const unsigned long hash = glHash("target/backbuffer");
    return GLRenderPair(hash, glplatGetBackBufferTarget());
}

static inline GLRenderPair FindTarget(unsigned long hash)
{
    GLXTarget** foundTarget = 0;
    if (targets.FindGet(hash, &foundTarget))
        return GLRenderPair(hash, *foundTarget);
    return GLRenderPair(0, 0);
}

static inline GLRenderPair CreateTarget(unsigned long& hash, const GLTargetInfo* targetInfo)
{
    GLXTarget* platformTarget = glplatCreateTarget(targetInfo);
    if (platformTarget != 0)
    {
        platformTarget->CreateTexture(hash);
        targets.Add(hash, platformTarget);
    }
    else
    {
        hash = 0;
    }
    return GLRenderPair(hash, platformTarget);
}

static inline GLRenderPair FindOrCreateTarget(unsigned long& hash, const GLTargetInfo* targetInfo)
{
    GLRenderPair result = FindTarget(hash);
    if (result)
        return result;
    return CreateTarget(hash, targetInfo);
}

void gl_TargetStartup()
{
    unsigned long hash;
    GLXTarget* platformTarget;
    GLRenderPair target = GetBackBufferTarget();
    platformTarget = target.target;
    hash = glHash("target/backbuffer");
    targets.Add(hash, platformTarget);
}

GLRenderPair glGetBackBufferTarget()
{
    return GetBackBufferTarget();
}

GLRenderPair glCreateTarget(const char* name, const GLTargetInfo* targetInfo)
{
    glBeginResource(name);

    char targetName[128];
    nlStrNCat(targetName, "target/", name, sizeof(targetName));
    unsigned long hash = glHash(targetName);

    GLRenderPair result = FindOrCreateTarget(hash, targetInfo);

    glEndResource();
    return result;
}

void glDestroyTarget(GLRenderPair* target)
{
    unsigned long hash = target->hash;
    TargetTree::Entry* entry = (TargetTree::Entry*)targets.RemoveAVLNode(
        (AVLTreeNode**)&targets.m_Root, &hash);
    if (entry != 0)
        delete entry;

    target->target->DestroyTexture(target->hash);
    delete target->target;
    target->hash = 0;
    target->target = 0;
}

unsigned long glGetTargetTexture(GLRenderPair target)
{
    return target.hash;
}
