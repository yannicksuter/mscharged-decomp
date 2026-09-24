#ifndef GAME_FE_FE_CAPTAIN_COMPONENT_H
#define GAME_FE_FE_CAPTAIN_COMPONENT_H

#include "types.h"
#include "Game/FE/feScrollBar.h"
#include "Game/FE/feScrollText.h"
#include "Game/FE/feTimer.h"

class TLComponentInstance;
class TLImageInstance;
class TLInstance;
struct UnidentifiedTLGroupInstance;
struct CharacterInfo;

class TU801DA134Component
{
public:
    TU801DA134Component();
    virtual ~TU801DA134Component();

    void fn_801DA198(TLComponentInstance* component, int side);
    void fn_801DA718(TLInstance* instance, int captain, unsigned char alpha);
    void fn_801DA88C();
    TLImageInstance* fn_801DA924(int index, const char* name);
    void fn_801DABAC(int index, int state);
    void fn_801DAFC8();
    void fn_801DB69C(float dt);
    void fn_801DCB28();
    void fn_801DCC28();
    void fn_801DCCEC();
    void fn_801DC824(bool visible0, bool visible1, bool visible2);
    int fn_801DCD74(int index);
    void fn_801DCD84(int value);
    void fn_801DCD8C(int index, int value);
    static TLImageInstance* FindSidekickImage(int sidekick, int captain);
    static TLImageInstance* FindCaptainImage(int captain, bool left);
    static void SetOverallSlide(TLComponentInstance* overall, const CharacterInfo& info);

    /* 0x04 */ TLComponentInstance* mComponent;
    /* 0x08 */ TLComponentInstance* mUnidentified08;
    /* 0x0C */ int mUnidentified0C;
    /* 0x10 */ int mSide;
    /* 0x14 */ int mUnidentified14;
    /* 0x18 */ int mSidekicks[3];
    /* 0x24 */ int mUnidentified24;
}; // size 0x28


#endif // GAME_FE_FE_CAPTAIN_COMPONENT_H
