#include "unclassified/tu_802F2C3C.h"

#include "NL/nlSlotPool.h"
#include "types.h"

// One playing sound: it owns the chain of playback objects the sequence asks
// for and forwards every transport call to each of them, then to the next
// owner of the chain. The playback objects keep their virtual tables as plain
// arrays, matching the unit that defines them.
SlotPool<PlaybackOwner_802F2C3C> sPlaybackOwnerPool_802F3E20(32, 16);

extern "C" PlaybackObject_802F2C3C* fn_802F2CAC(
    PlaybackOwner_802F2C3C* owner, PlaybackRequest_802F2C3C* request);

/**
 * Address/Size: 0x802F3E20 | size: 0xAC
 */
extern "C" PlaybackOwner_802F2C3C* fn_802F3E20(PlaybackOwner_802F2C3C* owner,
    SoundInstance_802F2C3C* instance, PlaybackSequence_802F2C3C* sequence)
{
    owner->next = 0;
    owner->instance = instance;
    owner->sequence = sequence;
    owner->objects = 0;
    owner->volumeOffset = *(float*)sequence;
    owner->pitchOffset = 0.0f;
    owner->started = false;

    PlaybackObject_802F2C3C* previous = 0;
    for (u32 index = 0; index < sequence->count; index++)
    {
        PlaybackObject_802F2C3C* object
            = fn_802F2CAC(owner, sequence->requests + index);
        if (previous == 0)
            owner->objects = object;
        else
            previous->next = object;
        previous = object;
    }
    return owner;
}

/**
 * Address/Size: 0x802F3ECC | size: 0xA0
 */
extern "C" PlaybackOwner_802F2C3C* fn_802F3ECC(
    PlaybackOwner_802F2C3C* owner, int destroy)
{
    if (owner != 0)
    {
        PlaybackObject_802F2C3C* object = owner->objects;
        while (object != 0)
        {
            PlaybackObject_802F2C3C* following = object->next;
            delete object;
            object = following;
        }
        owner->objects = 0;
        if (destroy > 0)
            sPlaybackOwnerPool_802F3E20.Free(owner);
    }
    return owner;
}

/**
 * Address/Size: 0x802F3F6C | size: 0x104
 */
extern "C" void fn_802F3F6C(PlaybackOwner_802F2C3C* owner)
{
    for (PlaybackObject_802F2C3C* object = owner->objects; object != 0;
         object = object->next)
    {
        object->UnidentifiedVirtual0C();
    }
    if (owner->next != 0)
        fn_802F3F6C(owner->next);
}

/**
 * Address/Size: 0x802F4070 | size: 0x104
 */
extern "C" void fn_802F4070(PlaybackOwner_802F2C3C* owner)
{
    for (PlaybackObject_802F2C3C* object = owner->objects; object != 0;
         object = object->next)
    {
        object->UnidentifiedVirtual10();
    }
    if (owner->next != 0)
        fn_802F4070(owner->next);
}

/**
 * Address/Size: 0x802F4174 | size: 0x104
 */
extern "C" void fn_802F4174(PlaybackOwner_802F2C3C* owner)
{
    for (PlaybackObject_802F2C3C* object = owner->objects; object != 0;
         object = object->next)
    {
        object->UnidentifiedVirtual18();
    }
    if (owner->next != 0)
        fn_802F4174(owner->next);
}

/**
 * Address/Size: 0x802F4278 | size: 0x104
 */
extern "C" void fn_802F4278(PlaybackOwner_802F2C3C* owner)
{
    for (PlaybackObject_802F2C3C* object = owner->objects; object != 0;
         object = object->next)
    {
        object->UnidentifiedVirtual1C();
    }
    if (owner->next != 0)
        fn_802F4278(owner->next);
}

/**
 * Address/Size: 0x802F437C | size: 0x19C
 */
extern "C" int fn_802F437C(PlaybackOwner_802F2C3C* owner, float time)
{
    int result = 9;
    for (PlaybackObject_802F2C3C* object = owner->objects; object != 0;
         object = object->next)
    {
        if (!owner->started || object->state != 0)
        {
            int state = object->UnidentifiedVirtual20(time);
            result = result > state ? state : result;
        }
    }
    if (owner->next != 0)
    {
        int state = fn_802F437C(owner->next, time);
        result = result > state ? state : result;
    }
    return result;
}

/**
 * Address/Size: 0x802F4518 | size: 0x118
 */
extern "C" void fn_802F4518(PlaybackOwner_802F2C3C* owner)
{
    owner->started = true;
    for (PlaybackObject_802F2C3C* object = owner->objects; object != 0;
         object = object->next)
    {
        if ((u32)(object->state - 2) <= 3)
            object->UnidentifiedVirtual14();
    }
    if (owner->next != 0)
        fn_802F4518(owner->next);
}

/**
 * Address/Size: 0x802F4630 | size: 0x8
 */
extern "C" void fn_802F4630(PlaybackOwner_802F2C3C* owner, float value)
{
    owner->pitchOffset = value;
}

/**
 * Address/Size: 0x802F4638 | size: 0x8
 */
extern "C" void fn_802F4638(PlaybackOwner_802F2C3C* owner, float value)
{
    owner->volumeOffset = value;
}

/**
 * Address/Size: 0x802F4640 | size: 0x7C
 */
extern "C" void fn_802F4640(
    PlaybackOwner_802F2C3C* owner, u32* results, u32* count)
{
    for (PlaybackObject_802F2C3C* object = owner->objects; object != 0;
         object = object->next)
    {
        *count += object->UnidentifiedVirtual2C(results + *count);
    }
}
