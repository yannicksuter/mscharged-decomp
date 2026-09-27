#ifndef GAME_EFFECTS_EMITTER_CALLBACKS_H
#define GAME_EFFECTS_EMITTER_CALLBACKS_H

class cCharacter;
class DrawableCharacter;
class EmissionController;
class ImpostorModel;

DrawableCharacter* GetReplayDrawableCharacter(cCharacter* character);
void UpdateEmitterFromCharacterUnculled(EmissionController& controller);
void UpdateEmitterFromCharacterWithoutAnimController(
    EmissionController& controller, void* context);
void UpdateEmitterFromCharacter(EmissionController& ec);
void UpdateEmitterPoseFromCharacter(EmissionController& emitter);
void UpdateEmitterFromBall(EmissionController& emitter);
void UpdateEmitterFromBallTrail(EmissionController& controller);
void UpdateEmitterFromBallLandingSpot(EmissionController& controller);
void UpdateEmitterFromCharacterHead(EmissionController& controller);
void UpdateEmitterFromCharacterBackward(EmissionController& controller);
void UpdateEmitterFromCharacterForward(EmissionController& controller);
void UpdateEmitterFromImpostorModel(
    EmissionController& controller, void* context);

#endif // GAME_EFFECTS_EMITTER_CALLBACKS_H
