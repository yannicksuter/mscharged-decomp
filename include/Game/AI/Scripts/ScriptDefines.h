#ifndef GAME_AI_SCRIPTS_SCRIPTDEFINES_H
#define GAME_AI_SCRIPTS_SCRIPTDEFINES_H

class cFielder;
class cTeam;

void FuzzyScriptClearGlobals();
void FuzzyScriptSetCurrentTeam(cTeam* pCurrentTeam);
void FuzzyScriptSetCurrentFielder(cFielder* pCurrentFielder);

#endif // GAME_AI_SCRIPTS_SCRIPTDEFINES_H
