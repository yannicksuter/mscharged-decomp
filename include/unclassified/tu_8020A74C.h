#ifndef UNCLASSIFIED_TU_8020A74C_H
#define UNCLASSIFIED_TU_8020A74C_H

#include "Game/BaseSceneHandler.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;

class TU8020A74CScene : public BaseSceneHandler
{
public:
    TU8020A74CScene();
    virtual ~TU8020A74CScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_8020D5C8();
    void fn_8020DBC4(int index, void* context);
    void fn_8020DC68(int index, void* context);
    void fn_8020DCF4(int index, void* context);
    void fn_8020DDC0(int index, void* context);
    void fn_8020DE60(int index, void* context);
    void fn_8020DEE8(int index, void* context);

    /* 0x01C */ unsigned char mUnidentified1C[0x2C0];
    /* 0x2DC */ bool mUnidentified2DC;
    /* 0x2DD */ unsigned char mPadding2DD[3];
    /* 0x2E0 */ void* mTournamentData;
    /* 0x2E4 */ bool mNetworkTournament;
    /* 0x2E5 */ unsigned char mPadding2E5[3];
    /* 0x2E8 */ float mUnidentified2E8;
    /* 0x2EC */ int mUnidentified2EC;
    /* 0x2F0 */ bool mUnidentified2F0;
    /* 0x2F1 */ bool mUnidentified2F1;
    /* 0x2F2 */ bool mUnidentified2F2;
    /* 0x2F3 */ bool mUnidentified2F3;
    /* 0x2F4 */ bool mUnidentified2F4;
    /* 0x2F5 */ bool mUnidentified2F5;
    /* 0x2F6 */ bool mUnidentified2F6;
    /* 0x2F7 */ bool mUnidentified2F7;
    /* 0x2F8 */ bool mUnidentified2F8;
    /* 0x2F9 */ bool mUnidentified2F9;
    /* 0x2FA */ bool mUnidentified2FA;
    /* 0x2FB */ bool mUnidentified2FB;
    /* 0x2FC */ int mSelectionCounts[4];
    /* 0x30C */ TU8022EF84Component mNavigationComponent;
    /* 0x3E4 */ TU80219248Component mMatchupComponents[7];
    /* 0x8D0 */ TU80219248Component mBracketComponent;
    /* 0x984 */ TLComponentInstance* mUnidentified984;
    /* 0x988 */ TLComponentInstance* mMatchupInstances[7];
    /* 0x9A4 */ TLComponentInstance* mUnidentified9A4[7];
    /* 0x9C0 */ TLComponentInstance* mBracketInstance;
    /* 0x9C4 */ int mState;
}; // size 0x9C8

#endif // UNCLASSIFIED_TU_8020A74C_H
