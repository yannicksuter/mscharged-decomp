#ifndef GAME_AI_SCRIPTS_SCRIPTDEFINES_H
#define GAME_AI_SCRIPTS_SCRIPTDEFINES_H

class cBall;
class cFielder;
class cTeam;

extern cFielder* g_pScriptCurrentFielder;
extern cFielder* g_pScriptCurrentMark;
extern cFielder* g_pScriptBallOwner;
extern cTeam* g_pScriptCurrentTeam;
extern cTeam* g_pScriptOtherTeam;
extern cBall* g_pScriptBall;

void FuzzyScriptClearGlobals();
void FuzzyScriptSetCurrentTeam(cTeam* pCurrentTeam);
void FuzzyScriptSetCurrentFielder(cFielder* pCurrentFielder);

#endif // GAME_AI_SCRIPTS_SCRIPTDEFINES_H
