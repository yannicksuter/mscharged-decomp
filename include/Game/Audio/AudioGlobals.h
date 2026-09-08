#ifndef GAME_AUDIO_AUDIO_GLOBALS_H
#define GAME_AUDIO_AUDIO_GLOBALS_H

class AudioSystem;
class AudioBackend;

extern AudioSystem* g_pAudioSystem;
extern AudioBackend* g_pAudioBackend;
extern void* g_pAudioSilenceBuffer;

void SetControllerSpeakerEnabled(bool enabled);
void SetAudioEffectContext(unsigned long* hash, int index);

#endif // GAME_AUDIO_AUDIO_GLOBALS_H
