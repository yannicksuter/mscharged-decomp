#include "revolution/hbm/nw4hbm/snd/StrmSoundHandle.h"

#include "revolution/types.h"

#include "revolution/hbm/nw4hbm/snd/StrmSound.h"

namespace nw4hbm {
namespace snd {

void StrmSoundHandle::DetachSound() {
    if (IsAttachedSound()) {
        if (mSound->mTempSpecialHandle == this) {
            mSound->mTempSpecialHandle = NULL;
        }
    }

    if (mSound) {
        mSound = NULL;
    }
}

} // namespace snd
} // namespace nw4hbm
