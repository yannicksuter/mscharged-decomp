#include "decomp.h"
#include "revolution/hbm/nw4hbm/snd.h"
#include "revolution/hbm/nw4hbm/ut.h"

namespace nw4hbm {
namespace snd {
namespace detail {
NW4HBM_UT_GET_DERIVED_RUNTIME_TYPEINFO(SeqSound, BasicSound);

SeqSound::SeqSound(SoundInstanceManager<SeqSound>* manager) :
    mTempSpecialHandle(NULL),
    mManager(manager),
    mCallback(NULL),
    mLoadingFlag(false) {}

void SeqSound::InitParam() {
    BasicSound::InitParam();
    mCallback = NULL;
    mCallbackData = 0;
}

SeqPlayer::SetupResult SeqSound::Setup(SeqTrackAllocator* allocator, u32 allocTrackFlags, int voices,
                                       NoteOnCallback* callback) {
    NW4HBMAssertPointerNonnull_Line(callback, 87);
    NW4HBMAssertPointerNonnull_Line(GetSoundPlayer(), 88);
    InitParam();
    return mSeqPlayer.Setup(allocator, allocTrackFlags, voices, callback);
}

void SeqSound::Prepare(const void* seqBase, s32 seqOffset) {
    NW4HBMAssertPointerNonnull_Line(seqBase, 112);
    mSeqPlayer.SetSeqData(seqBase, seqOffset);
}

void SeqSound::Prepare(const SeqLoadCallback* callback, u32 callbackData) {
    NW4HBMAssertPointerNonnull_Line(callback, 128);

    if (callback == NULL) {
        return;
    }

    callback->LoadData(NotifyLoadAsyncEndSeqData, this, callbackData);

    mCallback = callback;
    mCallbackData = callbackData;
    mLoadingFlag = true;
}

void SeqSound::NotifyLoadAsyncEndSeqData(bool result, const void* seqBase, s32 seqOffset, void* userData) {
    detail::SeqSound* sound = static_cast<detail::SeqSound*>(userData);
    NW4HBMAssertPointerNonnull_Line(sound, 154);

    sound->mLoadingFlag = false;
    if (!result) {
        sound->Stop();
        return;
    }

    sound->Prepare(seqBase, seqOffset);
}

void SeqSound::Shutdown() {
    if (mLoadingFlag) {
        if (mCallback != NULL) {
            mCallback->CancelLoading(mCallbackData);
        }
    }
    BasicSound::Shutdown();
    mManager->Free(this);
}

void SeqSound::SetTempoRatio(f32 tempoRatio) {
    NW4HBMAssert(tempoRatio >= 0.0f);
    mSeqPlayer.SetTempoRatio(tempoRatio);
}

void SeqSound::SetChannelPriority(int priority) {
    NW4HBMAssertHeaderClampedLRValue_Line(priority, 0, 127, 233);
    mSeqPlayer.SetChannelPriority(priority);
}

void SeqSound::SetReleasePriorityFix(bool flag) { mSeqPlayer.SetReleasePriorityFix(flag); }

void SeqSound::SetPlayerPriority(int priority) {
    BasicSound::SetPlayerPriority(priority);
    mManager->UpdatePriority(this, CalcCurrentPlayerPriority());
}

void SeqSound::SetTrackVolume(u32 trackFlags, f32 volume) {
    NW4HBMAssert(volume >= 0.0f);
    mSeqPlayer.SetTrackVolume(trackFlags, volume);
}

void SeqSound::SetTrackPitch(u32 trackFlags, f32 pitch) {
    NW4HBMAssert(pitch >= 0.0f);
    mSeqPlayer.SetTrackPitch(trackFlags, pitch);
}

bool SeqSound::ReadVariable(int varNo, s16* var) {
    NW4HBMAssertPointerNonnull(var);
    NW4HBMAssertHeaderClampedLValue(varNo, 0, SeqPlayer::LOCAL_VARIABLE_NUM);

    vs16* varPtr = mSeqPlayer.GetVariablePtr(varNo);
    if (varPtr == NULL) {
        return false;
    }

    *var = *varPtr;
    return true;
}

bool SeqSound::WriteVariable(int varNo, s16 value) {
    NW4HBMAssertHeaderClampedLValue(varNo, 0, SeqPlayer::LOCAL_VARIABLE_NUM);
    mSeqPlayer.SetLocalVariable(varNo, value);
    return true;
}

bool SeqSound::WriteGlobalVariable(int varNo, s16 value) {
    NW4HBMAssertHeaderClampedLValue(varNo, 0, SeqPlayer::GLOBAL_VARIABLE_NUM);
    SeqPlayer::SetGlobalVariable(varNo, value);
    return true;
}

bool SeqSound::WriteTrackVariable(int trackNo, int varNo, s16 value) {
    NW4HBMAssertHeaderClampedLValue(trackNo, 0, SeqPlayer::TRACK_NUM);
    NW4HBMAssertHeaderClampedLValue(varNo, 0, SeqTrack::VARIABLE_NUM);

    SeqTrack* track = mSeqPlayer.GetPlayerTrack(trackNo);
    if (track == NULL) {
        return false;
    }

    track->SetTrackVariable(varNo, value);
    return true;
}

bool SeqSound::IsAttachedTempSpecialHandle() { return mTempSpecialHandle != NULL; }

void SeqSound::DetachTempSpecialHandle() { mTempSpecialHandle->DetachSound(); }


} // namespace detail
} // namespace snd
} // namespace nw4hbm
