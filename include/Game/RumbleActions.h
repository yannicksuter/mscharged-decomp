#ifndef GAME_RUMBLE_ACTIONS_H
#define GAME_RUMBLE_ACTIONS_H

class DetInput;

void UpdateRumbleActions(float dt);
void PlayRumbleAction(int preset, DetInput* pad);

#endif // GAME_RUMBLE_ACTIONS_H
