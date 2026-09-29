#ifndef GAME_AI_DESIRE_SUPER_POWER_H
#define GAME_AI_DESIRE_SUPER_POWER_H

#include "Game/AI/Desire.h"
#include "Game/Character.h"


class DesireSuperPower;
extern "C" bool fn_800D0DB0(DesireSuperPower*, void*);
extern "C" bool fn_800C9F4C(DesireSuperPower*, void*);
extern "C" bool fn_800CAC8C(DesireSuperPower*, void*);
extern "C" void fn_800C9D74(DesireSuperPower*, int);
void EmitBowserJrShriek(DesireSuperPower*);

class DesireSuperPower : public Desire
{
    friend bool fn_800D0DB0(DesireSuperPower*, void*);
    friend bool fn_800C9F4C(DesireSuperPower*, void*);
    friend bool fn_800CAC8C(DesireSuperPower*, void*);
    friend void fn_800C9D74(DesireSuperPower*, int);
    friend void EmitBowserJrShriek(DesireSuperPower*);

public:
    DesireSuperPower();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedSetContext(UnidentifiedScriptMachine*);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);

    void fn_800CAB18();

private:
    static UnidentifiedVariant_80054AB8 fn_800CE588(const FuzzyVariant&, shdStateMachine*);
    static UnidentifiedVariant_80054AB8 fn_800CEA20(const FuzzyVariant&, shdStateMachine*);
    void fn_800D01A0(DesireUpdate*, float);
    bool fn_800CCF90() const;
    void fn_800CCC0C(DesireUpdate*, float);
    void fn_800CDBF0(DesireUpdate*, float);
    void fn_800C93A4(DesireUpdate*, float);
    int fn_800D0004();
    void fn_800CD61C(DesireUpdate*, float);
    void fn_800D0EAC(DesireUpdate*, float);
    void fn_800CBF64(DesireUpdate*, float);
    void fn_800CB7A8(DesireUpdate*, float);
    void fn_800CA07C(DesireUpdate*, float);
    void fn_800CA57C(DesireUpdate*, float);
    void fn_800CADBC(DesireUpdate*, float);
    void fn_800CB2BC(DesireUpdate*, float);
    void fn_800CC720(DesireUpdate*, float);

    void* mpDKShockAvoidable;
    cFielder* mpTarget;
    nlVector2 mUnidentifiedPositions[8];
};


// Shared functions and data from Game/AI/DesireSuperPower.cpp.
extern "C" void fn_800CD8E4(nlVector2*, const nlVector2*);
extern "C" void fn_800D1140(void*);
extern "C" void fn_800D12E8(void*);
extern "C" eCharacterClass fn_800D1440(const cCharacter*);
extern "C" unsigned short fn_800D1448(const cCharacter*);
const nlVector3* GetCharacterPosition(const cCharacter*);
extern "C" bool fn_800D1458(const cGame*);

extern const nlVector3 lbl_804DC388;

#endif // GAME_AI_DESIRE_SUPER_POWER_H
