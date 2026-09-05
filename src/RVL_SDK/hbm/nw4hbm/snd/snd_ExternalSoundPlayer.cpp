#include "revolution/hbm/nw4hbm/snd/ExternalSoundPlayer.h"

namespace nw4hbm {
namespace snd {
namespace detail {

ExternalSoundPlayer::ExternalSoundPlayer() : mPlayableCount(1), mVolume(1.0f) {}

ExternalSoundPlayer::~ExternalSoundPlayer() {
    BasicSoundExtPlayList::Iterator it = mSoundList.GetBeginIter();

    while (it != mSoundList.GetEndIter()) {
        BasicSoundExtPlayList::Iterator curr = it++;
        curr->SetExternalSoundPlayer(NULL);
    }
}

void ExternalSoundPlayer::SetPlayableSoundCount(int count) {
    mPlayableCount = count;

    while (GetPlayingSoundCount() > GetPlayableSoundCount()) {
        GetLowestPrioritySound()->Shutdown();
    }
}

void ExternalSoundPlayer::InsertSoundList(BasicSound* sound) {
    mSoundList.PushBack(sound);
    sound->SetExternalSoundPlayer(this);
}

void ExternalSoundPlayer::RemoveSoundList(BasicSound* sound) {
    mSoundList.Erase(sound);
    sound->SetExternalSoundPlayer(NULL);
}

BasicSound* ExternalSoundPlayer::GetLowestPrioritySound() {
    int lowestPrio = BasicSound::PRIORITY_MAX + 1;
    BasicSound* pLowest = NULL;

    for (BasicSoundExtPlayList::Iterator it = mSoundList.GetBeginIter(); it != mSoundList.GetEndIter(); it++) {

        int priority = it->CalcCurrentPlayerPriority();

        if (lowestPrio > priority) {
            pLowest = &*it;
            lowestPrio = priority;
        }
    }

    return pLowest;
}
} // namespace detail
} // namespace snd
} // namespace nw4hbm
