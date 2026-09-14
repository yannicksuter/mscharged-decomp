#include "Game/Render/TimedObject.h"

template <>
TimedObjectManager*
    nlSingleton<TimedObjectManager>::s_pInstance = 0;

TimedObject::TimedObject(float lifetime)
    : mEnabled(true)
    , mLifetime(lifetime)
    , mElapsedTime(0.0f)
{
    TimedObjectManager::Instance()->mObjects.AddEnd(this);
}

TimedObjectManager::TimedObjectManager()
{
}

TimedObjectManager::~TimedObjectManager()
{
    mObjects.Clear();
}

void TimedObjectManager::Update(float dt)
{
    nlListIterator<TimedObject*> iterator
        = mObjects.Begin();
    while (iterator.IsValid())
    {
        TimedObject* object = iterator.Current();
        bool remove = false;

        if (object->mEnabled
            && object->mElapsedTime < object->mLifetime)
        {
            object->mElapsedTime += dt;
            object->Update(dt);
        }

        if (object->HasExpired())
        {
            remove = true;
        }

        iterator.Next();
        if (remove)
        {
            mObjects.RemoveEntry(object);
            delete object;
        }
    }
}
