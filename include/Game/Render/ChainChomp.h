#ifndef _CHAINCHOMP_H_
#define _CHAINCHOMP_H_

#include "Game/Render/SkinAnimatedMovableNPC.h"

class cFielder;
class cSAnim;

enum eChainChompState
{
    CHAIN_STATE_HIDDEN = 0,
    CHAIN_STATE_FALL = 1,
    CHAIN_STATE_RECOVER = 2,
    CHAIN_STATE_CHASE = 3,
    CHAIN_STATE_LEAVE = 4,
    CHAIN_STATE_UNIDENTIFIED_5 = 5,
};

class ChainChomp : public SkinAnimatedMovableNPC
{
public:
    virtual ~ChainChomp();
    virtual void Move(float fDeltaT);
    virtual void DrawShadow(
        const cPoseAccumulator& pa, const nlMatrix4& worldMatrix);

    /* 0x84 */ cSAnim* mpIdleAnim;
    /* 0x88 */ cSAnim* mpRecoverAnim;
    /* 0x8C */ cSAnim* mpDropAnim;
    /* 0x90 */ cFielder* mpTarget;
    /* 0x94 */ eChainChompState meChainChompState;
};

#endif // _CHAINCHOMP_H_
