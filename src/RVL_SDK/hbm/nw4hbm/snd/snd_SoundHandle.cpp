#include "revolution/hbm/nw4hbm/snd/SoundHandle.h"

#include "revolution/types.h"

#include "revolution/hbm/nw4hbm/snd/BasicSound.h"

#include "revolution/hbm/HBMAssert.hpp"

namespace nw4hbm {
namespace snd {

void SoundHandle::detail_AttachSound(detail::BasicSound* sound) {
    NW4HBMAssertPointerNonnull_Line(sound, 85);

    mSound = sound;

    if (sound->IsAttachedGeneralHandle()) {
        mSound->DetachGeneralHandle();
    }

    mSound->mGeneralHandle = this;
}

void SoundHandle::DetachSound() {
    if (IsAttachedSound()) {
        if (mSound->mGeneralHandle == this) {
            mSound->mGeneralHandle = NULL;
        }

        if (mSound->mTempGeneralHandle == this) {
            mSound->mTempGeneralHandle = NULL;
        }
    }

    if (mSound) {
        mSound = NULL;
    }
}

} // namespace snd
} // namespace nw4hbm
