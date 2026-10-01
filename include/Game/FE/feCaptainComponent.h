#ifndef GAME_FE_FE_CAPTAIN_COMPONENT_H
#define GAME_FE_FE_CAPTAIN_COMPONENT_H

#include "types.h"
#include "Game/FE/feScrollBar.h"
#include "Game/FE/feScrollText.h"
#include "Game/FE/feTimer.h"

class TLComponentInstance;
class TLImageInstance;
class TLInstance;
struct TLGroupInstance;
struct CharacterInfo;

class FECaptainComponent
{
public:
    FECaptainComponent();
    virtual ~FECaptainComponent();

    void Initialize(TLComponentInstance* component, int side);
    void ApplyTeamColour(TLInstance* instance, int captain, unsigned char alpha);
    void Show();
    TLImageInstance* FindPositionImage(int index, const char* name);
    void SetRecycleState(int index, int state);
    void UpdateOverallSlides();
    void LoadSlotImages(float dt);
    void RandomizeSidekicks();
    void ResetSidekicks();
    void ReloadSidekicks();
    void SetSlotVisibility(bool visible0, bool visible1, bool visible2);
    int GetSidekick(int index);
    void SetCaptain(int value);
    void SetSidekick(int index, int value);
    static TLImageInstance* FindSidekickImage(int sidekick, int captain);
    static TLImageInstance* FindCaptainImage(int captain, bool left);
    static void SetOverallSlide(TLComponentInstance* overall, const CharacterInfo& info);

    /* 0x04 */ TLComponentInstance* mComponent;
    /* 0x08 */ TLComponentInstance* mPositions;
    /* 0x0C */ int mUnidentified0C;
    /* 0x10 */ int mSide;
    /* 0x14 */ int mCaptain;
    /* 0x18 */ int mSidekicks[3];
    /* 0x24 */ int mUnidentified24;
}; // size 0x28


#endif // GAME_FE_FE_CAPTAIN_COMPONENT_H
