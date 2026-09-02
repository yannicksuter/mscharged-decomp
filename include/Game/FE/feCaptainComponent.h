#ifndef GAME_FE_FE_CAPTAIN_COMPONENT_H
#define GAME_FE_FE_CAPTAIN_COMPONENT_H

#include "NL/nlBasicString.h"
#include "NL/nlFunction.h"

class TLComponentInstance;

struct UnidentifiedTextFader
{
    UnidentifiedTextFader(int value);

    void fn_801E423C(int value);
    void fn_801E4244(int value);
    void fn_801E424C(int value);

    /* 0x00 */ u8 mUnidentified00[0x4];
    /* 0x04 */ BasicString<char, Detail::TempStringAllocator> mText;
    /* 0x08 */ u8 mUnidentified08[0xC];
    /* 0x14 */ int mUnidentified14;
    /* 0x18 */ u8 mUnidentified18[0x4];
    /* 0x1C */ int mUnidentified1C;
    /* 0x20 */ int mUnidentified20;
    /* 0x24 */ float mUnidentified24;
    /* 0x28 */ float mUnidentified28;
    /* 0x2C */ float mUnidentified2C;
    /* 0x30 */ Function<FnVoidVoid> mUnidentified30;
    /* 0x38 */ u8 mUnidentified38[0x8];
}; // size 0x40

class TU801DA134Component
{
public:
    TU801DA134Component();
    virtual ~TU801DA134Component();

    void fn_801DA198(TLComponentInstance* component, int side);
    void fn_801DA88C();
    void fn_801DC824(bool visible0, bool visible1, bool visible2);
    int fn_801DCD74(int index);
    void fn_801DCD84(int value);
    void fn_801DCD8C(int index, int value);

    /* 0x04 */ TLComponentInstance* mComponent;
    /* 0x08 */ TLComponentInstance* mUnidentified08;
    /* 0x0C */ int mUnidentified0C;
    /* 0x10 */ int mSide;
    /* 0x14 */ int mUnidentified14;
    /* 0x18 */ int mSidekicks[3];
    /* 0x24 */ int mUnidentified24;
}; // size 0x28

#endif // GAME_FE_FE_CAPTAIN_COMPONENT_H
