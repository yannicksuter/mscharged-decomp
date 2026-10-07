#ifndef GAME_AUDIO_GAME_STREAMS_H
#define GAME_AUDIO_GAME_STREAMS_H

class cPlayer;
class XSoundHandle;

extern void* gExclusiveAudioContext;
extern XSoundHandle* g_pLastAudioHandle;

void InitializeGameStreams();
void StopCrowdReactions();
void PlayCrowdReaction(unsigned long cueId);
void PlayCaptainChant(int slotId, unsigned long cueId, void* context);
void StopCaptainChant(unsigned long cueId, void* context);
void PlayCaptainPowerupStream(int slotId, unsigned long cueId, void* context);
void StopCaptainPowerupStream(unsigned long cueId, void* context);
void PlaySuddenDeathMusic();
void PauseSuddenDeathMusic();
void ResumeSuddenDeathMusic();
void StopSuddenDeathMusic();
void SetPlayerAudioController(cPlayer* player);

#endif // GAME_AUDIO_GAME_STREAMS_H
