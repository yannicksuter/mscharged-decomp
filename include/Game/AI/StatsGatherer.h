#ifndef GAME_AI_STATS_GATHERER_H
#define GAME_AI_STATS_GATHERER_H

#include "Game/InterpreterCore.h"
#include "NL/nlBasicString.h"
#include "NL/nlTask.h"

// Partial interface for the test harness; only its shared task and string
// implementations survive linking.
class StatsGatherer : private InterpreterCore, public nlTask
{
public:
    StatsGatherer();
    virtual ~StatsGatherer();

    void SetTestName(const char* name);
    virtual void Run(float);
    virtual const char* GetName();
    virtual void DoFunctionCall(unsigned int);

private:
    NLString m_testName;
};

#endif // GAME_AI_STATS_GATHERER_H
