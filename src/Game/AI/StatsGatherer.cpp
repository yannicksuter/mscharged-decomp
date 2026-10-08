#include "Game/AI/StatsGatherer.h"

StatsGatherer::StatsGatherer()
    : InterpreterCore(10)
{
}

StatsGatherer::~StatsGatherer()
{
}

void StatsGatherer::SetTestName(const char* name)
{
    m_testName = name;
}
