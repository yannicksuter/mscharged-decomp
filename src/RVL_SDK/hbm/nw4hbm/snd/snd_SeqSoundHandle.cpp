#include "revolution/hbm/nw4hbm/snd/SeqSoundHandle.h"

namespace nw4hbm {
namespace snd {
void SeqSoundHandle::DetachSound() {
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
