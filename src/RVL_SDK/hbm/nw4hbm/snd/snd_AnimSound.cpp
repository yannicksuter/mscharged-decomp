#include "revolution/hbm/nw4hbm/snd/AnimSound.h"

// Trigger behavior follows the early NW4R implementation at RMCP01 0x80087340.
// R4QE01 retains only shared assertion literals in this inferred unit.

namespace nw4hbm
{
namespace snd
{
namespace detail
{

void AnimSoundImpl::UpdateTrigger(const AnimEventRef* eventRef, s32 current, PlayDirection direction)
{
    const AnimEvent* event = fileReader.GetEvent(eventRef);
    if (event == NULL)
    {
        return;
    }

    if (eventRef->frameInfo.frameFlag & 4)
    {
        return;
    }

    if (eventRef->frameInfo.frameFlag & 2)
    {
        if (eventRef->frameInfo.startFrame == current)
        {
            StopEvent(event);
            if (eventCallback != NULL)
            {
                eventCallback(EVENT_TRIGGER_START, current, event->GetSoundLabel(), event->userParam, eventCallbackArg);
            }
            StartEvent(event, true);
        }
    }
    else
    {
        if (eventRef->frameInfo.startFrame == current)
        {
            StopEvent(event);
            if (eventCallback != NULL)
            {
                eventCallback(EVENT_TRIGGER_START, current, event->GetSoundLabel(), event->userParam, eventCallbackArg);
            }
            StartEvent(event, true);
        }

        s32 endFrame;
        if (direction == PLAY_FORWARD)
        {
            endFrame = eventRef->frameInfo.endFrame;
        }
        else
        {
            endFrame = ut::Max(eventRef->frameInfo.startFrame * 2 - eventRef->frameInfo.endFrame, (s32)0);
        }

        if (endFrame == current)
        {
            if (eventCallback != NULL)
            {
                eventCallback(EVENT_TRIGGER_STOP, current, event->GetSoundLabel(), event->userParam, eventCallbackArg);
            }
            StopEvent(event);
        }
    }
}

void AnimSoundImpl::StopEvent(const AnimEvent* event)
{
    for (int i = 0; i < numEventPlayers; i++)
    {
        eventPlayer[i].StopEvent(event);
    }
}

void AnimEventPlayer::ForceStop()
{
    handle.Stop(0);
    event = NULL;
}

} // namespace detail
} // namespace snd
} // namespace nw4hbm
