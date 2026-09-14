#ifndef GAME_RENDER_TIMED_OBJECT_H
#define GAME_RENDER_TIMED_OBJECT_H

#include "NL/nlList.h"
#include "NL/nlSingleton.h"

class TimedObjectManager;

class TimedObject
{
public:
    TimedObject(float lifetime);
    virtual ~TimedObject() { }
    virtual void Update(float deltaTime) = 0;

    bool HasExpired() const
    {
        return mElapsedTime >= mLifetime;
    }

    /* 0x04 */ bool mEnabled;
    /* 0x08 */ float mLifetime;
    /* 0x0C */ float mElapsedTime;
}; // total size: 0x10

class TimedObjectList
    : public ListContainerBase<TimedObject*,
          NewAdapter<ListEntry<TimedObject*> > >
{
public:
    typedef void (TimedObjectList::*EntryCallback)(
        ListEntry<TimedObject*>*);

    void Clear()
    {
        EntryCallback callback = &TimedObjectList::DeleteEntry;
        nlWalkList(m_Head, this, callback);
        m_Head = 0;
        m_Tail = 0;
    }

    void DeleteEntry(ListEntry<TimedObject*>* entry);
};

class TimedObjectManager
    : public nlSingleton<TimedObjectManager>
{
public:
    TimedObjectManager();
    virtual ~TimedObjectManager();

    void Update(float dt);

    /* 0x04 */ TimedObjectList mObjects;
}; // total size: 0x10

inline void TimedObjectList::DeleteEntry(
    ListEntry<TimedObject*>* entry)
{
    delete entry->entry;
    delete entry;
}

#endif // GAME_RENDER_TIMED_OBJECT_H
