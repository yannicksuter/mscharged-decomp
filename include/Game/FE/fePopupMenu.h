#ifndef GAME_FE_FE_POPUP_MENU_H
#define GAME_FE_FE_POPUP_MENU_H

#include "NL/nlFunction.h"
#include "types.h"

typedef Function<FnVoidVoid> _FEPopupMenuCB;

enum ePopupMenu
{
    INVALID_TYPE = -1,
};

class FEPopupMenu
{
public:
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

    /* 0x000 */ u8 mUnidentified000[0x9A1];
    /* 0x9A1 */ bool mMenuCreated;
};

#endif // GAME_FE_FE_POPUP_MENU_H
