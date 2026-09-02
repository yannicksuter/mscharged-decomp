#ifndef GAME_SH_SH_OPTIONS_H
#define GAME_SH_SH_OPTIONS_H

#include "Game/BaseGameSceneManager.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;

class OptionsScene : public BaseSceneHandler
{
public:
    OptionsScene();
    virtual ~OptionsScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_801D2A08(int index, void* context);
    void fn_801D3098(int index, void* context);
    void fn_801D3148(int index, void* context);
    void fn_801D31E0();

    /* 0x01C */ TLComponentInstance* mUnidentified1C[3];
    /* 0x028 */ TU80219248Component mUnidentified28[3];
    /* 0x244 */ TU8022EF84Component mUnidentified244;
    /* 0x31C */ bool mUnidentified31C;
    /* 0x31D */ u8 mPadding31D[3];
    /* 0x320 */ int mUnidentified320;
    /* 0x324 */ SceneList mUnidentified324;
}; // size 0x328

#endif // GAME_SH_SH_OPTIONS_H
