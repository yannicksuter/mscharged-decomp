#ifndef GAME_SH_SH_CUP_HUB_H
#define GAME_SH_SH_CUP_HUB_H

#include "Game/BaseSceneHandler.h"
#include "Game/SH/SHSceneBase.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;

class CupHubScene : public BaseSceneHandler
{
public:
    CupHubScene();
    virtual ~CupHubScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_802017C4(int index, int value);
    void fn_80201BE0(int index);
    void fn_802025F4(int value0, int value1, int value2, int value3, int value4);
    void fn_80202C34();
    void fn_80203320(int index, void* context);
    void fn_80203498(int index, void* context);
    void fn_80203674(int index, void* context);
    void fn_802037E8(int index, void* context);
    void fn_80203980(int index, void* context);
    void fn_80203A10(int index, void* context);
    void fn_80203A88(int index, void* context);
    void fn_80203B54();

    /* 0x01C */ TLComponentInstance* mUnidentified1C[5];
    /* 0x030 */ TU80219248Component mMatchupComponents[4];
    /* 0x300 */ bool mUnidentified300;
    /* 0x304 */ int mUnidentified304;
    /* 0x308 */ UnidentifiedScrollWidget mScrollWidget;
    /* 0x4BC */ u16 mTextBuffers[7][32];
    /* 0x67C */ bool mUnidentified67C;
    /* 0x67D */ bool mUnidentified67D;
    /* 0x67E */ bool mUnidentified67E;
    /* 0x67F */ bool mUnidentified67F;
    /* 0x680 */ int mUnidentified680[4];
    /* 0x690 */ TU8022EF84Component mNavigationComponent;
    /* 0x768 */ int mUnidentified768;
    /* 0x76C */ TU80219248Component mRulesComponent;
    /* 0x820 */ TLComponentInstance* mRulesButton;
    /* 0x824 */ s8 mMatchupStates[9][12];
    /* 0x890 */ bool mUnidentified890;
    /* 0x894 */ int mUnidentified894;
}; // size 0x898

#endif // GAME_SH_SH_CUP_HUB_H
