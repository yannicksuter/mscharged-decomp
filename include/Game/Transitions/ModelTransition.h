#ifndef GAME_TRANSITIONS_MODEL_TRANSITION_H
#define GAME_TRANSITIONS_MODEL_TRANSITION_H

#include "Game/Effects/EmissionController.h"
#include "Game/PoseAccumulator.h"
#include "Game/SAnim.h"
#include "Game/SHierarchy.h"
#include "Game/Sys/simpleparser.h"
#include "Game/Transitions/ScreenTransitionManager.h"
#include "NL/nlAVLTree.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

class cPN_SAnimController;

struct TransitionModelStore
{
    glModel* pModels;
    unsigned long nModels;
};

class ModeledScreenTransition : public ScreenTransition
{
public:
    ModeledScreenTransition();

    virtual ~ModeledScreenTransition();
    virtual void Update(float deltaTime);
    virtual void Render(GLView* view);
    virtual bool IsFinished();
    virtual float Time() const;
    virtual void Reset();
    virtual void Cancel();
    virtual float GetTransitionLength();
    virtual void DoSanityCheck();
    virtual bool UnidentifiedVirtual30();

    void RenderOutline() const;
    ModeledScreenTransition* LoadFromParser(SimpleParser* parser);
    void CreateInstance(TransitionModelStore& modelInfo);
    void Load(const char* szName);
    void FixupModel();

    /* 0x04 */ glModel* m_pModels;
    /* 0x08 */ unsigned long m_nModels;
    /* 0x0C */ cSHierarchy* m_pSkeleton;
    /* 0x10 */ cSAnim* m_pAnim;
    /* 0x14 */ char* m_pSkelFile;
    /* 0x18 */ char* m_pAnimFile;
    /* 0x1C */ unsigned long m_nTexture;
    /* 0x20 */ unsigned long m_Unknown20;
    /* 0x24 */ cPoseAccumulator* m_pPoseAccumulator;
    /* 0x28 */ cPN_SAnimController* m_pPoseTree;
    /* 0x2C */ nlMatrix4 m_mWorldMatrix;
    /* 0x6C */ bool m_bScreenGrab;
    /* 0x6D */ bool m_bEnableGrab;
    /* 0x70 */ int* m_pModelMap;
    /* 0x74 */ char m_EffectName[64];
    /* 0xB4 */ EmissionController** m_Effects;
    /* 0xB8 */ bool m_RenderOutline;
    /* 0xB9 */ nlColour m_OutlineColour;

    static GLView* s_3DView;
    static nlAVLTree<unsigned long, TransitionModelStore,
        DefaultKeyCompare<unsigned long> > g_ModelInventory;
}; // total size: 0xC0

#endif // GAME_TRANSITIONS_MODEL_TRANSITION_H
