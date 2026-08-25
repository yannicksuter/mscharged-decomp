#include "revolution/hbm/nw4hbm/snd/WaveSoundHandle.h"

namespace nw4hbm {
namespace snd {

void WaveSoundHandle::DetachSound() {
    if (IsAttachedSound()) {
        if (mSound->mTempSpecialHandle == this) {
            mSound->mTempSpecialHandle = NULL;
        }
    }

    if (mSound != NULL) {
        mSound = NULL;
    }
}

} // namespace snd
} // namespace nw4hbm
