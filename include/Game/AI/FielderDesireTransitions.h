#ifndef GAME_AI_FIELDER_DESIRE_TRANSITIONS_H
#define GAME_AI_FIELDER_DESIRE_TRANSITIONS_H

#include "Game/AI/Desire.h"

class AIContext;

// Native transition functions bound to the fielder desires. Each returns
// DESIRE_FINISHED when its condition holds for the input's fielder and
// DESIRE_CONTINUE otherwise.
DesireUpdate TransDesireBallOwner(AIContext* input);
DesireUpdate TransDesireNotBallOwner(AIContext* input);
DesireUpdate TransDesireActionDone(AIContext* input);

#endif // GAME_AI_FIELDER_DESIRE_TRANSITIONS_H
