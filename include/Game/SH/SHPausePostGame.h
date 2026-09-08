#ifndef _SHPAUSEPOSTGAME_H_
#define _SHPAUSEPOSTGAME_H_

#include "Game/FE/feInput.h"
#include "Game/SH/SHSceneBase.h"
#include "unclassified/tu_80209584.h"

class PausePostGameScene : public UnidentifiedSHSceneBase
{
public:
    PausePostGameScene(int);
    virtual ~PausePostGameScene();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void SHSceneVirtual30();

    void OnSelectRematch();
    void OnSelectQuit();
    void OnSelectChangeTeams();

    /* 0x5D4 */ int mUnidentified5D4;
    /* 0x5D8 */ eFEINPUT_PAD mUnidentified5D8;
    /* 0x5DC */ u8 mUnidentified5DC[0x30];
    /* 0x60C */ bool mUnidentified60C;
    /* 0x60D */ u8 mPadding60D[3];
    /* 0x610 */ int mUnidentified610;
    /* 0x614 */ TU80209584Summary mSummary;
}; // size 0xA48

#endif // _SHPAUSEPOSTGAME_H_
