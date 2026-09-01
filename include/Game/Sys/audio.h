#ifndef GAME_SYS_AUDIO_H
#define GAME_SYS_AUDIO_H

#include "Game/Audio/AudioLoadMode_806E201C.h"
#include "Game/Audio/XSoundHandle_802ED74C.h"
#include "types.h"

class cPlayer;

struct AudioHandleState_800EBF78
{
    unsigned long m_CueId;
    void* m_Context;
    unsigned long m_Flags;
};

class GameAudio_800EB6AC : public AudioLoadMode_806E201C
{
public:
    GameAudio_800EB6AC();

    bool Initialize();
    virtual void Shutdown();
    void Update(float deltaTime);

    unsigned long m_PlayRequestCount;
};

typedef void (*AudioPlayCallback_800EBB04)(
    XSoundHandle_802ED74C*, void* context);

extern "C"
{
    void fn_800EBB04(GameAudio_800EB6AC* audio, int slotId,
        unsigned long cueId, AudioPlayCallback_800EBB04 callback,
        void* context);
    void fn_800EBBD8(GameAudio_800EB6AC* audio, unsigned long value);
    bool fn_800EBBFC(int slotId, unsigned long cueId,
        const void* debugName, void* context);
    bool fn_800EBC84(int slotId, unsigned long cueId,
        XSoundOwner_802ED74C* owner, const void* debugName,
        void* context);
    XSoundHandle_802ED74C* fn_800EBD00(int slotId,
        unsigned long cueId, XSoundOwner_802ED74C* owner,
        const void* debugName, void* context, bool findExisting);
    bool fn_800EBE90(unsigned long cueId, void* context);
    XSoundHandle_802ED74C* fn_800EBEF4(
        unsigned long cueId, void* context);
    bool fn_800EBF78(int slotId, unsigned long cueId,
        const void* debugName, void* context, bool restartable);
    bool fn_800EC058(int slotId, unsigned long cueId,
        XSoundOwner_802ED74C* owner, const void* debugName,
        void* context, bool restartable);
    void fn_800EC12C(unsigned long cueId, void* context);
    void fn_800EC2A4(unsigned long cueId, void* context);
    void fn_800EC400(unsigned long cueId, void* context);
    void fn_800EC548(unsigned long parameter, float value);
    unsigned long fn_800EC5C4(unsigned long cueId, void* context);
    void fn_800EC65C(
        unsigned long cueId, void* context, unsigned char enabled);
    bool fn_800EC708(int slotId, unsigned long cueId,
        XSoundOwner_802ED74C* owner, const void* debugName,
        void* context, bool restartable);
    bool fn_800EC7BC(unsigned long cueId, void* context);
    void fn_800EC868();
    void fn_800ECB50();
    unsigned int fn_800ECCCC();
    void fn_800ED8C8(XSoundHandle_802ED74C* handle);
    void fn_800ED8D4();
    void fn_800ED8D8();
    void fn_800ED92C(unsigned long cueId);
    void fn_800EDA84(int slotId, unsigned long cueId, void* context);
    void fn_800EDB04(unsigned long cueId, void* context);
    void fn_800EDB10(int slotId, unsigned long cueId, void* context);
    void fn_800EDB90(unsigned long cueId, void* context);
    void fn_800EDB9C();
    void fn_800EDBF8();
    void fn_800EDC2C();
    void fn_800EDCAC();
    void fn_800EDCE8(cPlayer* player);
}

#endif // GAME_SYS_AUDIO_H
