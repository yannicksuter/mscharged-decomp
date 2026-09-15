#ifndef GAME_RUMBLE_ACTIONS_H
#define GAME_RUMBLE_ACTIONS_H

class DetInput;
class cGlobalPad;

enum eRumbleActionPreset
{
    RUMBLE_SMALL_CONTACT = 1,
    RUMBLE_MEDIUM_CONTACT = 2,
    RUMBLE_SOLID_CONTACT = 3,
    RUMBLE_SHOT_CONTACT = 4,
    RUMBLE_POINTER_HOVER = 5,
};

bool IsRumbleActionActive(cGlobalPad* pad);
void BeginRumbleAction(eRumbleActionPreset preset, cGlobalPad* pad);
void StopRumbleAction(cGlobalPad* pad);
void UpdateRumbleActions(float dt);
void PlayRumbleAction(int preset, DetInput* pad);

#endif // GAME_RUMBLE_ACTIONS_H
