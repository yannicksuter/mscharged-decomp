#ifndef GAME_AI_DESIRE_INL
#define GAME_AI_DESIRE_INL

/**
 * Offset/Address/Size: 0x9638 | 0x800D1D34 | size: 0x8
 */
extern "C" inline int fn_800D1D34(const shdStateMachine* machine)
{
    return machine->UnidentifiedGetState();
}

/**
 * Offset/Address/Size: 0x9640 | 0x800D1D3C | size: 0x8
 */
extern "C" inline UnidentifiedVariantCollection* fn_800D1D3C(
    shdStateMachine* stateMachine)
{
    return &stateMachine->mUnidentified01C;
}

/**
 * Offset/Address/Size: 0x9648 | 0x800D1D44 | size: 0x8
 */
extern "C" inline float fn_800D1D44(const DesireRunInDirection* desire)
{
    return desire->GetMaxDistance();
}

/**
 * Offset/Address/Size: 0x9650 | 0x800D1D4C | size: 0x8
 */
extern "C" inline float fn_800D1D4C(const DesireRunInDirection* desire)
{
    return desire->GetDistanceTravelled();
}

#endif // GAME_AI_DESIRE_INL
