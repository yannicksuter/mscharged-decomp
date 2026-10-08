#ifndef GAME_PAD_ACTIONS_H
#define GAME_PAD_ACTIONS_H

extern bool gEnablePadMonkeys;
extern int gGameCubeButtonRemap[51];

void UseDefaultFreestyleButtonRemap(bool useDefaultRemap);
void CreatePadBackends();
void DestroyPadBackends();
void InitPads();
void InitPlatPad();
void UpdateMonkeyState(int monkeySet);

#endif // GAME_PAD_ACTIONS_H
