#ifndef GAME_SYS_AUDIO_H
#define GAME_SYS_AUDIO_H

#include "Game/Audio/AudioSystem.h"
#include "Game/Audio/GameStreams.h"
#include "Game/Audio/AudioResourceLoader.h"
#include "Game/Audio/XSoundHandle.h"
#include "types.h"

class cPlayer;

struct AudioHandleState
{
    unsigned long m_CueId;
    void* m_Context;
    unsigned long m_Flags;
};

class GameAudio : public AudioSystem
{
public:
    GameAudio();

    bool Initialize();
    virtual void Shutdown();
    void Update(float deltaTime);

    unsigned long m_PlayRequestCount;
};

typedef AudioResourceLoadCallback AudioPlayCallback;

void LoadSoundBank(GameAudio* audio, int slotId,
    unsigned long cueId, AudioPlayCallback callback,
    void* context);
void UnloadSoundBanks(GameAudio* audio);
bool PlaySound(int slotId, unsigned long cueId,
    const void* debugName, void* context);
bool PlayOwnedSound(int slotId, unsigned long cueId,
    XSoundOwner* owner, const void* debugName,
    void* context);
XSoundHandle* CreateSoundHandle(int slotId,
    unsigned long cueId, XSoundOwner* owner,
    const void* debugName, void* context, bool findExisting);
bool IsSoundTracked(unsigned long cueId, void* context);
XSoundHandle* FindSoundHandle(
    unsigned long cueId, void* context);
bool PlayTrackedSound(int slotId, unsigned long cueId,
    const void* debugName, void* context, bool restartable);
bool PlayTrackedOwnedSound(int slotId, unsigned long cueId,
    XSoundOwner* owner, const void* debugName,
    void* context, bool restartable);
void StopSound(unsigned long cueId, void* context);
void PauseSound(unsigned long cueId, void* context);
void ResumeSound(unsigned long cueId, void* context);
void SetLastSoundParameter(unsigned long parameter, float value);
unsigned long GetSoundState(unsigned long cueId, void* context);
void SetSoundCallbackEnabled(
    unsigned long cueId, void* context, unsigned char enabled);
bool PrepareTrackedSound(int slotId, unsigned long cueId,
    XSoundOwner* owner, const void* debugName,
    void* context, bool restartable);
bool StartTrackedSound(unsigned long cueId, void* context);
void PauseAllAudio();
void ResumeAllAudio();
int GetAudioPauseDepth();
void InvalidateSoundHandle(XSoundHandle* handle);
void InitializeGameStreams();
void StopCrowdReactions();
void PlayCaptainChant(int slotId, unsigned long cueId, void* context);
void StopCaptainChant(unsigned long cueId, void* context);
void PlayCaptainPowerupStream(int slotId, unsigned long cueId, void* context);
void StopCaptainPowerupStream(unsigned long cueId, void* context);
void PlaySuddenDeathMusic();
void PauseSuddenDeathMusic();
void ResumeSuddenDeathMusic();
void StopSuddenDeathMusic();
void SetPlayerAudioController(cPlayer* player);

#endif // GAME_SYS_AUDIO_H
