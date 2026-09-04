#ifndef UNCLASSIFIED_TU_80188884_H
#define UNCLASSIFIED_TU_80188884_H

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

    void Update(float dt);

    /* 0x04 */ UnidentifiedList_80188884 mUnidentified004;
}; // total size: 0x10

#endif // UNCLASSIFIED_TU_80188884_H
