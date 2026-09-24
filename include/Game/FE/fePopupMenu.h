#ifndef GAME_FE_FE_POPUP_MENU_H
#define GAME_FE_FE_POPUP_MENU_H

#include "Game/BaseSceneHandler.h"
#include "NL/nlBasicString.h"
#include "NL/nlFunction.h"
#include "Game/FE/fePointerButton.h"
#include "types.h"
#include "NL/nlColour.h"

class TLComponentInstance;
class TLTextInstance;

typedef Function<FnVoidVoid> _FEPopupMenuCB;

enum ePopupMenu
{
    INVALID_TYPE = -1,
    POPUP_LOW_BATTERY = 0x8D,
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
    void fn_801C83AC(bool visible);
    void fn_801C8494();
    void fn_801C87E0(unsigned int index, void* context);
    void fn_801C88B4(unsigned int index, void* context);
    void fn_801C8960(unsigned int index, void* context);
    ePopupMenu fn_801CA644() const { return mType; }

    // Never called. R4QE01 keeps the implicit nlColour copy-assignment that
    // SceneCreated calls out of line in the position of an already-synthesized
    // member: at its call slot in SceneCreated's header drain, right behind
    // TLInstance::SetVisible (0x801CA618). Under GC/3.0a5 -sym on an implicit
    // member drains there only when some body parsed before SceneCreated
    // already assigned an nlColour; no header this unit includes does, and the
    // linker kept no trace of the body that did. This inline reproduces that
    // parse-time synthesis and emits nothing itself. Still unidentified, hence
    // the name: the original setter's spelling, signature and callers.
    // Registered as an accepted exception in the workspace final review.
    void UnidentifiedSetHighlightedOptionColour(const nlColour& colour)
    {
        mHighlightedOptionColour = colour;
    }
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
    /* 0x9D0 */ TLTextInstance* mUnidentified9D0[3];
    /* 0x9DC */ FEPointerButton mControllerComponents[3];
    /* 0xBF8 */ unsigned int mUnidentifiedBF8[4];
    /* 0xC08 */ int mUnidentifiedC08;
    /* 0xC0C */ int mUnidentifiedC0C;
    /* 0xC10 */ Function<FnVoidVoid> callBacks[3];
    /* 0xC28 */ Function<FnVoidVoid> mUnknownA64;
    /* 0xC30 */ nlColour mHighlightedOptionColour;
    /* 0xC34 */ unsigned char mUnidentifiedC34[0x0C];
    /* 0xC40 */ ePopupMenu mType;
    /* 0xC44 */ bool mUnknownAA4;
    /* 0xC45 */ bool mUnknownAA5;
    /* 0xC46 */ unsigned char mUnidentifiedC46[2];
    /* 0xC48 */ TLComponentInstance* mUnidentifiedC48;
    /* 0xC4C */ bool mUnidentifiedC4C;
    /* 0xC4D */ unsigned char mUnidentifiedC4D[3];
    /* 0xC50 */ float mUnidentifiedC50;
    /* 0xC54 */ float mUnidentifiedC54;
    /* 0xC58 */ bool mUnidentifiedC58;

private:
    void UpdateBackgroundScale(float fDeltaT);
}; // size 0xC5C

#endif // GAME_FE_FE_POPUP_MENU_H
