#ifndef GAME_AI_DESIRE_INL
#define GAME_AI_DESIRE_INL

/**
 * Offset/Address/Size: 0x9638 | 0x800D1D34 | size: 0x8
 */
inline int GetStateMachineState(const shdStateMachine* machine)
{
    return machine->GetState();
}

/**
 * Offset/Address/Size: 0x9640 | 0x800D1D3C | size: 0x8
 */
inline UnidentifiedVariantCollection* GetStateMachineParameters(
    shdStateMachine* stateMachine)
{
    return &stateMachine->mParameters;
}

/**
 * Offset/Address/Size: 0x9648 | 0x800D1D44 | size: 0x8
 */
inline float GetRunInDirectionMaxDistance(const DesireRunInDirection* desire)
{
    return desire->GetMaxDistance();
}

/**
 * Offset/Address/Size: 0x9650 | 0x800D1D4C | size: 0x8
 */
inline float GetRunInDirectionDistanceTravelled(const DesireRunInDirection* desire)
{
    return desire->GetDistanceTravelled();
}

#endif // GAME_AI_DESIRE_INL
