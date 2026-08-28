#ifndef GAME_AI_SCRIPTS_SCRIPT_QUESTIONS_H
#define GAME_AI_SCRIPTS_SCRIPT_QUESTIONS_H

class cFielder;

float GoalieOutOfPosition(cFielder* pFielder);
float LikelyToScore(cFielder* pFielder);
float PlayerShotDistance(cFielder* pFielder);

#endif // GAME_AI_SCRIPTS_SCRIPT_QUESTIONS_H
