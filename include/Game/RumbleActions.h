#ifndef GAME_RUMBLE_ACTIONS_H
#define GAME_RUMBLE_ACTIONS_H

class DetInput;
class cGlobalPad;

enum eRumbleActionPreset
{
    RUMBLE_POINTER_HOVER = 5,
};

bool IsRumbleActionActive(cGlobalPad* pad);
void BeginRumbleAction(eRumbleActionPreset preset, cGlobalPad* pad);

void StopRumbleAction(cGlobalPad* pad);
void UpdateRumbleActions(float dt);
void BeginRumbleAction(int preset, cGlobalPad* pad);
void StopRumbleAction(cGlobalPad* pad);
void PlayRumbleAction(int preset, DetInput* pad);

#endif // GAME_RUMBLE_ACTIONS_H
