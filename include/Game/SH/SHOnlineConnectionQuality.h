#ifndef UNCLASSIFIED_TU_8026F444_H
#define UNCLASSIFIED_TU_8026F444_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feTimer.h"

class NetMessageCheckConnection;
class NetMessageConnectionDecision;
class TLComponentInstance;

class OnlineConnectionQualityScene : public BaseSceneHandler
{
public:
    OnlineConnectionQualityScene();
    virtual ~OnlineConnectionQualityScene();
    virtual void Update(float dt);
    virtual void SceneCreated();

    void OnCheckConnection(NetMessageCheckConnection* message);
    void OnCountdownTick(FETimer* timer);
    void OnReturnTimer(FETimer* timer);
    void InitializeInput();
    void OnDecisionPointerPress(unsigned int index, void* context);
    void OnConnectionDecision(NetMessageConnectionDecision* message);
    void UpdateConnectionQuality();
    void OnDecisionPointerEnter(unsigned int index, void* context);
    void OnDecisionPointerLeave(unsigned int index, void* context);
    void CloseConnectionsAndReturn();
    void ShowError(int error);

    /* 0x01C */ unsigned int mUnidentified01C;
    /* 0x020 */ int mHoverCounts[4];
    /* 0x030 */ bool mInitialized;
    /* 0x031 */ bool mDecisionMade;
    /* 0x032 */ unsigned char mPadding032[2];
    /* 0x034 */ int mDecisionOutcome;
    /* 0x038 */ int mDecision;
    /* 0x03C */ int mMachineDecisions[2];
    /* 0x044 */ unsigned short mTimerText[128];
    /* 0x144 */ FETimer mCountdownTimer;
    /* 0x160 */ FETimer mReturnTimer;
    /* 0x17C */ bool mTimerTextDirty;
    /* 0x17D */ unsigned char mPadding17D[3];
    /* 0x180 */ int mCountdownSeconds;
    /* 0x184 */ unsigned int mProfileIds[2];
    /* 0x18C */ FEPointerButton mDecisionButtons[2];
    /* 0x2F4 */ TLComponentInstance* mDecisionButtonInstances[2];
    /* 0x2FC */ TLComponentInstance* mStarsComponent;
    /* 0x300 */ bool mPopupActive;
}; // size 0x304

#endif // UNCLASSIFIED_TU_8026F444_H
