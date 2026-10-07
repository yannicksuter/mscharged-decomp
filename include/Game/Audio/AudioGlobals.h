#ifndef GAME_AUDIO_AUDIO_GLOBALS_H
#define GAME_AUDIO_AUDIO_GLOBALS_H

class AudioSystem;
class AudioBackend;
class XSoundHandle;

extern AudioSystem* g_pAudioSystem;
extern AudioBackend* g_pAudioBackend;
extern void* g_pAudioSilenceBuffer;
extern unsigned long gAudioMemorySize;
extern void* gExclusiveAudioContext;
extern XSoundHandle* g_pLastAudioHandle;
extern unsigned int gResidentVoiceDropCount;
extern unsigned int gStreamVoiceDropCount;

void ReleaseAudioSoundOwner(void* value, void* owner);
void SetAudioEffectContext(unsigned long* hash, int index);

#endif // GAME_AUDIO_AUDIO_GLOBALS_H
