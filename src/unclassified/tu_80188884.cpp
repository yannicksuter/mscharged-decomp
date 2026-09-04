#include "unclassified/tu_80188884.h"

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

void UnidentifiedManager_80188928::Update(float dt)
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
            object->mUnidentified00C += dt;
            object->UnidentifiedVirtual0C(dt);
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
