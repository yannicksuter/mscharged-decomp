#include "NL/gl/glTarget.h"

#include "NL/gl/gl.h"
#include "NL/nlAVLTree.h"
#include "NL/nlString.h"

class TargetPlatform_8036DE50
{
public:
    virtual ~TargetPlatform_8036DE50();
    virtual void fn_8036D9A0() = 0;
    virtual void fn_8036D9A8() = 0;
    virtual void fn_8036D9AC(unsigned long hash) = 0;
    virtual void fn_8036DBAC(unsigned long hash) = 0;
};

extern "C" void fn_802C8280(const char* name);
extern "C" void fn_802C8288();
extern "C" TargetPlatform_8036DE50* fn_8036D894();
extern "C" TargetPlatform_8036DE50* fn_8036DE50(void* targetInfo);

typedef nlAVLTree<unsigned long, TargetPlatform_8036DE50*, DefaultKeyCompare<unsigned long> > TargetTree;

static TargetTree targets;

static inline GLRenderPair GetBackBufferTarget()
{
    static const unsigned long hash = glHash("target/backbuffer");
    return GLRenderPair(hash, fn_8036D894());
}

static inline GLRenderPair FindTarget(unsigned long hash)
{
    TargetPlatform_8036DE50** foundTarget = 0;
    if (targets.FindGet(hash, &foundTarget))
        return GLRenderPair(hash, *foundTarget);
    return GLRenderPair(0, 0);
}

static inline GLRenderPair CreateTarget(unsigned long& hash, void* targetInfo)
{
    TargetPlatform_8036DE50* platformTarget = fn_8036DE50(targetInfo);
    if (platformTarget != 0)
    {
        platformTarget->fn_8036D9AC(hash);
        targets.Add(hash, platformTarget);
    }
    else
    {
        hash = 0;
    }
    return GLRenderPair(hash, platformTarget);
}

void gl_TargetStartup()
{
    GLRenderPair target = GetBackBufferTarget();
    TargetPlatform_8036DE50* platformTarget = target.target;
    unsigned long hash = glHash("target/backbuffer");
    targets.Add(hash, platformTarget);
}

extern "C" GLRenderPair fn_802CD82C()
{
    return GetBackBufferTarget();
}

extern "C" GLRenderPair fn_802CD884(const char* name, void* targetInfo)
{
    fn_802C8280(name);

    char targetName[128];
    nlStrNCat(targetName, "target/", name, sizeof(targetName));
    unsigned long hash = glHash(targetName);

    GLRenderPair target = FindTarget(hash);
    GLRenderPair result;
    if (target)
        result = target;
    else
        result = CreateTarget(hash, targetInfo);

    fn_802C8288();
    return result;
}

extern "C" void fn_802CDA14(GLRenderPair* target)
{
    unsigned long hash = target->hash;
    TargetTree::Entry* entry = (TargetTree::Entry*)targets.RemoveAVLNode(
        (AVLTreeNode**)&targets.m_Root, &hash);
    if (entry != 0)
        delete entry;

    target->target->fn_8036DBAC(target->hash);
    delete target->target;
    target->hash = 0;
    target->target = 0;
}

extern "C" unsigned long fn_802CDAA8(const GLRenderPair* target)
{
    return target->hash;
}
