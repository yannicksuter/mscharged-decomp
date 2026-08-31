#include "NL/nlList.h"
#include "NL/nlSingleton.h"

class UnidentifiedManager_80188928;

class UnidentifiedObject_80188884
{
public:
    UnidentifiedObject_80188884(float param1);
    virtual ~UnidentifiedObject_80188884();
    virtual void UnidentifiedVirtual0C(float param1) = 0;

    bool UnidentifiedCondition() const
    {
        return mUnidentified00C >= mUnidentified008;
    }

    /* 0x04 */ bool mUnidentified004;
    /* 0x08 */ float mUnidentified008;
    /* 0x0C */ float mUnidentified00C;
}; // total size: 0x10

class UnidentifiedList_80188884
    : public ListContainerBase<UnidentifiedObject_80188884*,
          NewAdapter<ListEntry<UnidentifiedObject_80188884*> > >
{
public:
    typedef void (UnidentifiedList_80188884::*EntryCallback)(
        ListEntry<UnidentifiedObject_80188884*>*);

    void Clear()
    {
        EntryCallback callback = &UnidentifiedList_80188884::DeleteEntry;
        nlWalkList(m_Head, this, callback);
        m_Head = 0;
        m_Tail = 0;
    }

    void DeleteEntry(ListEntry<UnidentifiedObject_80188884*>* entry);
};

class UnidentifiedManager_80188928
    : public nlSingleton<UnidentifiedManager_80188928>
{
public:
    UnidentifiedManager_80188928();
    virtual ~UnidentifiedManager_80188928();

    void fn_80188A54(float param1);

    /* 0x04 */ UnidentifiedList_80188884 mUnidentified004;
}; // total size: 0x10

UnidentifiedManager_80188928*
    nlSingleton<UnidentifiedManager_80188928>::s_pInstance = 0;

UnidentifiedObject_80188884::UnidentifiedObject_80188884(float param1)
    : mUnidentified004(true)
    , mUnidentified008(param1)
    , mUnidentified00C(0.0f)
{
    UnidentifiedManager_80188928::Instance()->mUnidentified004.AddEnd(this);
}

UnidentifiedManager_80188928::UnidentifiedManager_80188928()
{
}

UnidentifiedManager_80188928::~UnidentifiedManager_80188928()
{
    mUnidentified004.Clear();
}

void UnidentifiedManager_80188928::fn_80188A54(float param1)
{
    nlListIterator<UnidentifiedObject_80188884*> iterator
        = mUnidentified004.Begin();
    while (iterator.IsValid())
    {
        UnidentifiedObject_80188884* object = iterator.Current();
        bool remove = false;

        if (object->mUnidentified004
            && object->mUnidentified00C < object->mUnidentified008)
        {
            object->mUnidentified00C += param1;
            object->UnidentifiedVirtual0C(param1);
        }

        if (object->UnidentifiedCondition())
        {
            remove = true;
        }

        iterator.Next();
        if (remove)
        {
            mUnidentified004.RemoveEntry(object);
            delete object;
        }
    }
}

inline void UnidentifiedList_80188884::DeleteEntry(
    ListEntry<UnidentifiedObject_80188884*>* entry)
{
    delete entry->entry;
    delete entry;
}

UnidentifiedObject_80188884::~UnidentifiedObject_80188884()
{
}
