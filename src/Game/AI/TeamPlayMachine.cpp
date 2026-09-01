#include "Game/AI/TeamPlayMachine.h"

#include "Game/AI/FuzzyVariant.h"
#include "Game/DB/tu_8010A40C.h"
#include "Game/GameInfo.h"
#include "NL/nlMemory.h"

struct UnidentifiedGameState
{
    u8 mUnidentified000[0x18];
    int mUnidentified018;
};

extern UnidentifiedGameState* lbl_806E0C94;
extern UnidentifiedStateTransition lbl_806E20B8;

float lbl_806DC448 = 1.1f;
char lbl_80503FC0[] = "TeamPlayMachine";

TeamPlayMachine::~TeamPlayMachine()
{
}

void TeamPlayMachine::UnidentifiedVirtual2()
{
    UnidentifiedScriptMachine::UnidentifiedVirtual2();

    TutorialMegastrikeDesire* desire =
        new (nlMalloc(sizeof(TutorialMegastrikeDesire), 8, false))
            TutorialMegastrikeDesire(5, lbl_806E20B8);
    UnidentifiedAddState(5, desire, false);
}

void TeamPlayMachine::UnidentifiedVirtual3(float deltaTime)
{
    UnidentifiedScriptMachine::UnidentifiedVirtual3(deltaTime);
}

void TeamPlayMachine::UnidentifiedVirtual7()
{
    UnidentifiedVariantCollection values;
    int state = -1;

    if (lbl_806E0C94->mUnidentified018 == 1)
    {
        values.Set(7, FuzzyVariant(lbl_806DC448));
        state = 1;
    }
    else if (GameInfoManager::Instance()->IsInMode4()
        && lbl_806E0FA0->mCurrentChallenge == 2)
    {
        state = 5;
    }

    if (state != -1)
    {
        UnidentifiedVirtual5(state, &values, true);
    }
    else
    {
        UnidentifiedScriptMachine::UnidentifiedVirtual7();
    }
}

TeamPlayMachine::TeamPlayMachine()
    : UnidentifiedScriptMachine(7, true, false, lbl_80503FC0)
{
}

void TeamPlayMachine::UnidentifiedVirtual8()
{
}
