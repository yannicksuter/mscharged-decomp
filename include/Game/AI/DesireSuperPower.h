#ifndef GAME_AI_DESIRE_SUPER_POWER_H
#define GAME_AI_DESIRE_SUPER_POWER_H

#include "Game/AI/Desire.h"
#include "Game/Character.h"


class DesireSuperPower;
extern "C" bool fn_800D0DB0(DesireSuperPower*, void*);
bool InitializeBowserJr(DesireSuperPower*, void*);
bool InitializeDiddy(DesireSuperPower*, void*);
extern "C" void fn_800C9D74(DesireSuperPower*, int);
void EmitBowserJrShriek(DesireSuperPower*);

class DesireSuperPower : public Desire
{
    friend bool fn_800D0DB0(DesireSuperPower*, void*);
    friend bool InitializeBowserJr(DesireSuperPower*, void*);
    friend bool InitializeDiddy(DesireSuperPower*, void*);
    friend void fn_800C9D74(DesireSuperPower*, int);
    friend void EmitBowserJrShriek(DesireSuperPower*);

public:
    DesireSuperPower();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void SetContext(ScriptMachine*);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);

    void EmitHeavenlyLight();

private:
    static UnidentifiedVariant_80054AB8 FollowPathTransition(const FuzzyVariant&, shdStateMachine*);
    static UnidentifiedVariant_80054AB8 ChooseDirectionTransition(const FuzzyVariant&, shdStateMachine*);
    void UpdateWario(DesireUpdate*, float);
    bool IsMuckBallReady() const;
    void UpdatePetey(DesireUpdate*, float);
    void UpdateWaluigiAI(DesireUpdate*, float);
    void UpdateBowser(DesireUpdate*, float);
    int BuildPathPoints();
    void UpdateWaluigi(DesireUpdate*, float);
    void UpdateYoshi(DesireUpdate*, float);
    void UpdateMario(DesireUpdate*, float);
    void UpdateLuigi(DesireUpdate*, float);
    void UpdateBowserJr(DesireUpdate*, float);
    void UpdateDaisy(DesireUpdate*, float);
    void UpdateDiddy(DesireUpdate*, float);
    void UpdateDK(DesireUpdate*, float);
    void UpdatePeach(DesireUpdate*, float);

    void* mpDKShockAvoidable;
    cFielder* mpTarget;
    nlVector2 mvPathPoints[8];
};


// Shared functions and data from Game/AI/DesireSuperPower.cpp.
void CopyVector2(nlVector2*, const nlVector2*);
void HandleMuckBallCollision(void*);
void HandleMuckBallWallCollision(void*);
eCharacterClass GetCharacterClass(const cCharacter*);
unsigned short GetCharacterFacing(const cCharacter*);
const nlVector3* GetCharacterPosition(const cCharacter*);
bool IsGameplayOrOvertime(const cGame*);

extern const nlVector3 gFielderDesireZeroVector;

#endif // GAME_AI_DESIRE_SUPER_POWER_H
