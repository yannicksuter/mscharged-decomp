#include "revolution/hbm/nw4hbm/snd/PlayerHeap.h"

#include "revolution/hbm/nw4hbm/snd/BasicSound.h"
#include "revolution/hbm/nw4hbm/snd/SoundPlayer.h"

namespace nw4hbm {
namespace snd {
namespace detail {

void PlayerHeap::Destroy() {
    if (mPlayer != NULL) {
        mPlayer->detail_RemovePlayerHeap(this);
        mPlayer = NULL;
    }

    if (mSound != NULL) {
        mSound->SetPlayerHeap(NULL);
        mSound = NULL;
    }

    SoundHeap::Destroy();
}

} // namespace detail
} // namespace snd
} // namespace nw4hbm
