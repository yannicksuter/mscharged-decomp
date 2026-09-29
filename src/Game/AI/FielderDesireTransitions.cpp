#include "Game/AI/FielderDesireTransitions.h"

#include "Game/AI/AIContext.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/Scripts/ScriptQuestions.h"

int gTransDesireBallOwnerFinished = DESIRE_FINISHED;
#pragma explicit_zero_data on
int gTransDesireBallOwnerContinue = DESIRE_CONTINUE;
#pragma explicit_zero_data off
int gTransDesireNotBallOwnerFinished = DESIRE_FINISHED;
#pragma explicit_zero_data on
int gTransDesireNotBallOwnerContinue = DESIRE_CONTINUE;
#pragma explicit_zero_data off
int gTransDesireActionDoneFinished = DESIRE_FINISHED;
#pragma explicit_zero_data on
int gTransDesireActionDoneContinue = DESIRE_CONTINUE;
#pragma explicit_zero_data off

/**
 * Offset/Address/Size: 0x0 | 0x800D6298 | size: 0x98
 */
DesireUpdate TransDesireBallOwner(AIContext* input)
{
    if (BallOwner(input->mData.pPlayer))
    {
        return DesireUpdate(FT_INT, gTransDesireBallOwnerFinished);
    }
    return DesireUpdate(FT_INT, gTransDesireBallOwnerContinue);
}

/**
 * Offset/Address/Size: 0x98 | 0x800D6330 | size: 0x98
 */
DesireUpdate TransDesireNotBallOwner(AIContext* input)
{
    if (!BallOwner(input->mData.pPlayer))
    {
        return DesireUpdate(FT_INT, gTransDesireNotBallOwnerFinished);
    }
    return DesireUpdate(FT_INT, gTransDesireNotBallOwnerContinue);
}

/**
 * Offset/Address/Size: 0x130 | 0x800D63C8 | size: 0x94
 */
DesireUpdate TransDesireActionDone(AIContext* input)
{
    if (((cFielder*)input->mData.pPlayer)->IsActionDone())
    {
        return DesireUpdate(FT_INT, gTransDesireActionDoneFinished);
    }
    return DesireUpdate(FT_INT, gTransDesireActionDoneContinue);
}
