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


void SeqSound::SetChannelPriority(int priority) {
    NW4HBMAssertHeaderClampedLRValue_Line(priority, 0, 127, 233);
    mSeqPlayer.SetChannelPriority(priority);
}

void SeqSound::SetPlayerPriority(int priority) {
    BasicSound::SetPlayerPriority(priority);
    mManager->UpdatePriority(this, CalcCurrentPlayerPriority());
}

bool SeqSound::IsAttachedTempSpecialHandle() { return mTempSpecialHandle != NULL; }

void SeqSound::DetachTempSpecialHandle() { mTempSpecialHandle->DetachSound(); }


} // namespace detail
} // namespace snd
} // namespace nw4hbm
