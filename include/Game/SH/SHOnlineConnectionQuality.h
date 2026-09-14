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

    /* 0x01C */ unsigned int mUnidentified01C;
    /* 0x020 */ int mUnidentified020[4];
    /* 0x030 */ bool mUnidentified030;
    /* 0x031 */ bool mUnidentified031;
    /* 0x032 */ unsigned char mPadding032[2];
    /* 0x034 */ int mUnidentified034;
    /* 0x038 */ int mUnidentified038;
    /* 0x03C */ int mUnidentified03C[2];
    /* 0x044 */ unsigned short mUnidentified044[128];
    /* 0x144 */ FETimer mUnidentified144;
    /* 0x160 */ FETimer mUnidentified160;
    /* 0x17C */ bool mUnidentified17C;
    /* 0x17D */ unsigned char mPadding17D[3];
    /* 0x180 */ int mUnidentified180;
    /* 0x184 */ unsigned int mUnidentified184[2];
    /* 0x18C */ FEPointerButton mUnidentified18C[2];
    /* 0x2F4 */ TLComponentInstance* mUnidentified2F4[2];
    /* 0x2FC */ TLComponentInstance* mUnidentified2FC;
    /* 0x300 */ bool mUnidentified300;
}; // size 0x304

#endif // UNCLASSIFIED_TU_8026F444_H
