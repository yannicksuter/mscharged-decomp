#ifndef GAME_SH_SH_CUP_HUB_H
#define GAME_SH_SH_CUP_HUB_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feScrollBar.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;
class TLTextInstance;
class FEPageControls;

class CupHubScene : public BaseSceneHandler
{
public:
    CupHubScene();
    virtual ~CupHubScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void UpdateRows();
    void fn_802017C4(int index, int value);
    void fn_80201BE0(int index);
    void fn_802025F4(TLTextInstance* roundText, int round, TLTextInstance* gameText, int game, int index);
    void fn_80202C34();
    void fn_80203320(int index, void* context);
    void fn_80203498(int index, void* context);
    void fn_80203674(int index, void* context);
    void fn_802037E8(int index, void* context);
    void fn_80203980(int index, void* context);
    void fn_80203A10(int index, void* context);
    void fn_80203A88(int index, void* context);
    void fn_80203B54();

    /* 0x01C */ TLComponentInstance* mUnidentified1C;
    /* 0x020 */ TLComponentInstance* mUnidentified20[4];
    /* 0x030 */ FEPointerButton mMatchupComponents[4];
    /* 0x300 */ bool mUnidentified300;
    /* 0x304 */ int mUnidentified304;
    /* 0x308 */ FEScrollBar mScrollWidget;
    /* 0x4BC */ u16 mTextBuffers[4][16];
    /* 0x53C */ u16 mUnidentified53C[4][16];
    /* 0x5BC */ u16 mUnidentified5BC[64];
    /* 0x63C */ u16 mUnidentified63C[4][2][4];
    /* 0x67C */ bool mUnidentified67C;
    /* 0x67D */ bool mUnidentified67D;
    /* 0x67E */ bool mUnidentified67E;
    /* 0x67F */ bool mUnidentified67F;
    /* 0x680 */ int mUnidentified680[4];
    /* 0x690 */ FEBackButton mNavigationComponent;
    /* 0x768 */ FEPageControls* mUnidentified768;
    /* 0x76C */ FEPointerButton mRulesComponent;
    /* 0x820 */ TLComponentInstance* mRulesButton;
    /* 0x824 */ s8 mMatchupStates[54][2];
    /* 0x890 */ u8 mUnidentified890;
    /* 0x894 */ int mUnidentified894;
}; // size 0x898

#endif // GAME_SH_SH_CUP_HUB_H
