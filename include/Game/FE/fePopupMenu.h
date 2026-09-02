#ifndef GAME_FE_FE_POPUP_MENU_H
#define GAME_FE_FE_POPUP_MENU_H

#include "Game/BaseSceneHandler.h"
#include "NL/nlBasicString.h"
#include "NL/nlFunction.h"
#include "unclassified/tu_80219248.h"
#include "types.h"

class TLComponentInstance;

typedef Function<FnVoidVoid> _FEPopupMenuCB;

enum ePopupMenu
{
    INVALID_TYPE = -1,
};

struct Popup
{
    /* 0x00 */ BasicString<unsigned short, Detail::TempStringAllocator>* pMessage;
    /* 0x04 */ BasicString<unsigned short, Detail::TempStringAllocator>* pOptionLabels[3];
    /* 0x10 */ int numOptions;
}; // size 0x14

class FEPopupMenu : public BaseSceneHandler
{
public:
    FEPopupMenu();
    virtual ~FEPopupMenu();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void SetPositions();
    void CentrePopup(float totalHeight, float topOfMessageBox);
    void ResizeHighlight();
    void SetBackButtonCallback(_FEPopupMenuCB callback);

    void Create(ePopupMenu type)
    {
        Create(type, Function<FnVoidVoid>(Nothing));
    }

    void Create(ePopupMenu type, Function<FnVoidVoid> option1)
    {
        Create(type, option1, Function<FnVoidVoid>(Nothing));
    }

    void Create(ePopupMenu type, Function<FnVoidVoid> option1, Function<FnVoidVoid> option2)
    {
        Create(type, option1, option2, Function<FnVoidVoid>(Nothing));
    }

    void Create(
        ePopupMenu type,
        Function<FnVoidVoid> option1,
        Function<FnVoidVoid> option2,
        Function<FnVoidVoid> option3)
    {
        Create(type, option1, option2, option3, Function<FnVoidVoid>(Nothing));
    }

    void Create(
        ePopupMenu type,
        Function<FnVoidVoid> option1,
        Function<FnVoidVoid> option2,
        Function<FnVoidVoid> option3,
        Function<FnVoidVoid> option4);

    static void Nothing() { }

    /* 0x01C */ unsigned short mMessageBuffer[1024];
    /* 0x81C */ unsigned short mOptionBuffers[4][48];
    /* 0x99C */ bool mMenuDisplayed;
    /* 0x99D */ bool mUnidentified99D;
    /* 0x99E */ bool mMenuCreated;
    /* 0x99F */ bool mRunCallBack;
    /* 0x9A0 */ bool mUnknownA1F;
    /* 0x9A1 */ bool mUnidentified9A1;
    /* 0x9A2 */ bool mUnidentified9A2;
    /* 0x9A3 */ bool mUnidentified9A3;
    /* 0x9A4 */ int mHighlightedOption;
    /* 0x9A8 */ int mUnidentified9A8;
    /* 0x9AC */ float mAcceptDelayTime;
    /* 0x9B0 */ Popup mPopup;
    /* 0x9C4 */ TLComponentInstance* mOptionInstances[3];
    /* 0x9D0 */ TLComponentInstance* mUnidentified9D0[3];
    /* 0x9DC */ TU80219248Component mControllerComponents[3];
    /* 0xBF8 */ unsigned int mUnidentifiedBF8[4];
    /* 0xC08 */ int mUnidentifiedC08;
    /* 0xC0C */ int mUnidentifiedC0C;
    /* 0xC10 */ Function<FnVoidVoid> callBacks[3];
    /* 0xC28 */ Function<FnVoidVoid> mUnknownA64;
    /* 0xC30 */ unsigned char mUnidentifiedC30[0x10];
    /* 0xC40 */ ePopupMenu mType;
    /* 0xC44 */ bool mUnknownAA4;
    /* 0xC45 */ bool mUnknownAA5;
    /* 0xC46 */ unsigned char mUnidentifiedC46[6];
    /* 0xC4C */ bool mUnidentifiedC4C;
    /* 0xC4D */ unsigned char mUnidentifiedC4D[3];
    /* 0xC50 */ float mUnidentifiedC50;
    /* 0xC54 */ float mUnidentifiedC54;
    /* 0xC58 */ bool mUnidentifiedC58;
}; // size 0xC5C

#endif // GAME_FE_FE_POPUP_MENU_H
