#ifndef GAME_SH_SH_OPTIONS_H
#define GAME_SH_SH_OPTIONS_H

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;

class OptionsScene : public BaseSceneHandler
{
public:
    OptionsScene();
    virtual ~OptionsScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void OnButtonPointerPress(int index, void* context);
    void OnButtonPointerEnter(int index, void* context);
    void OnButtonPointerLeave(int index, void* context);
    void InitializePointerButtons();

    /* 0x01C */ TLComponentInstance* mUnidentified1C[3];
    /* 0x028 */ FEPointerButton mUnidentified28[3];
    /* 0x244 */ FEBackButton mUnidentified244;
    /* 0x31C */ bool mUnidentified31C;
    /* 0x31D */ u8 mPadding31D[3];
    /* 0x320 */ int mUnidentified320;
    /* 0x324 */ SceneList mUnidentified324;
}; // size 0x328

#endif // GAME_SH_SH_OPTIONS_H
