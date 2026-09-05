#ifndef GAME_EFFECTS_EMITTER_CALLBACKS_H
#define GAME_EFFECTS_EMITTER_CALLBACKS_H

class cCharacter;
class DrawableCharacter;
class EmissionController;
class ImpostorModel_802DAEE0;

DrawableCharacter* GetReplayDrawableCharacter(cCharacter* character);
void UpdateEmitterFromCharacterUnculled(EmissionController& controller);
void UpdateEmitterFromCharacterWithoutAnimController(
    EmissionController& controller, cCharacter* character);
void UpdateEmitterFromCharacter(EmissionController& ec);
void UpdateEmitterPoseFromCharacter(EmissionController& emitter);
void UpdateEmitterFromBall(EmissionController& emitter);
void UpdateEmitterFromBallTrail(EmissionController& controller);
void UpdateEmitterFromBallLandingSpot(EmissionController& controller);
void UpdateEmitterFromCharacterHead(EmissionController& controller);
void UpdateEmitterFromCharacterBackward(EmissionController& controller);
void UpdateEmitterFromCharacterForward(EmissionController& controller);
void UpdateEmitterFromImpostorModel(
    EmissionController& controller, ImpostorModel_802DAEE0* model);

#endif // GAME_EFFECTS_EMITTER_CALLBACKS_H
