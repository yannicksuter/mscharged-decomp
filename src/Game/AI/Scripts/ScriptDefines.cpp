#include "Game/AI/Scripts/ScriptDefines.h"

#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/Team.h"
#include "types.h"

struct Unidentified800A636CResult
{
    u8 mUnidentified000[0x240];
    cFielder* m_pCurrentFielder;
};

extern "C" Unidentified800A636CResult* fn_800A636C(cTeam* pTeam);

cFielder* g_pScriptCurrentFielder;
cFielder* g_pScriptCurrentMark;
cFielder* g_pScriptBallOwner;
cTeam* g_pScriptCurrentTeam;
cTeam* g_pScriptOtherTeam;
cBall* g_pScriptBall;

void FuzzyScriptClearGlobals()
{
    if (g_pScriptCurrentTeam != 0)
    {
        Unidentified800A636CResult* pTeamContext =
            fn_800A636C(g_pScriptCurrentTeam);
        pTeamContext->m_pCurrentFielder = 0;
    }

    g_pScriptCurrentFielder = 0;
    g_pScriptCurrentMark = 0;
    g_pScriptBallOwner = 0;
    g_pScriptCurrentTeam = 0;
    g_pScriptOtherTeam = 0;
    g_pScriptBall = 0;
}

void FuzzyScriptSetCurrentTeam(cTeam* pCurrentTeam)
{
    if (pCurrentTeam == 0)
    {
        FuzzyScriptClearGlobals();
        return;
    }

    g_pScriptCurrentFielder = 0;
    g_pScriptBall = g_pBall;
    g_pScriptCurrentMark = 0;
    g_pScriptBallOwner = g_pBall->GetOwnerFielder();
    g_pScriptCurrentTeam = pCurrentTeam;
    g_pScriptOtherTeam =
        (g_pTeams[0] == pCurrentTeam) ? g_pTeams[1] : g_pTeams[0];
}

void FuzzyScriptSetCurrentFielder(cFielder* pCurrentFielder)
{
    if (pCurrentFielder == 0)
    {
        FuzzyScriptClearGlobals();
        return;
    }

    g_pScriptCurrentFielder = pCurrentFielder;
    g_pScriptBall = g_pBall;
    g_pScriptCurrentMark = pCurrentFielder->GetMark();
    g_pScriptBallOwner = g_pBall->GetOwnerFielder();
    g_pScriptCurrentTeam = pCurrentFielder->m_pTeam;
    g_pScriptOtherTeam =
        (g_pTeams[0] == g_pScriptCurrentTeam) ? g_pTeams[1] : g_pTeams[0];
    fn_800A636C(g_pScriptCurrentTeam)->m_pCurrentFielder = pCurrentFielder;
}
