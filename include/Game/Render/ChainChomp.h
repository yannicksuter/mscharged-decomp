#ifndef _CHAINCHOMP_H_
#define _CHAINCHOMP_H_

#include "Game/Render/SkinAnimatedMovableNPC.h"
#include "NL/nlTimer.h"

class cFielder;
class cSAnim;
class SFXEmitter;

enum eChainChompState
{
    CHAIN_STATE_HIDDEN = 0,
    CHAIN_STATE_FALL = 1,
    CHAIN_STATE_RECOVER = 2,
    CHAIN_STATE_CHASE = 3,
    CHAIN_STATE_UNIDENTIFIED_4 = 4,
    CHAIN_STATE_UNIDENTIFIED_5 = 5,
    CHAIN_STATE_LEAVE = 6,
};

class ChainChomp : public SkinAnimatedMovableNPC
{
public:
    virtual ~ChainChomp();
    virtual void Move(float fDeltaT);
    virtual void DrawShadow(
        const cPoseAccumulator& pa, const nlMatrix4& worldMatrix);

    bool IsHidden() const;
    void Leave();
    void Hide();

    /* 0x84 */ cSAnim* mpIdleAnim;
    /* 0x88 */ cSAnim* mpRecoverAnim;
    /* 0x8C */ cSAnim* mpDropAnim;
    /* 0x90 */ cFielder* mpTarget;
    /* 0x94 */ eChainChompState meChainChompState;
    /* 0x98 */ Timer mtStateTimer;
    /* 0xA0 */ float mUnidentifiedA0;
    /* 0xA4 */ SFXEmitter* mpInEffectSFX;
    /* 0xA8 */ cFielder* mpThrower;
    /* 0xAC */ int mnThrowerPadID;
    /* 0xB0 */ void* mUnidentifiedB0;
}; // total size: 0xB4

#endif // _CHAINCHOMP_H_
