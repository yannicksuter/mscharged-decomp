#include "Game/Drawable/DrawableObj.h"
#include "Game/Net.h"
#include "Game/Team.h"
#include "NL/nlList.h"
#include "NL/nlTask.h"

class UnidentifiedManager_801AE0C4
{
public:
    UnidentifiedManager_801AE0C4(bool positiveX)
        : mUnidentified010(0.0f)
        , mUnidentified014(-1)
        , mPositiveX(positiveX)
    {
    }

    virtual ~UnidentifiedManager_801AE0C4();

    /* 0x04 */ nlListContainer<DrawableObject*> mObjects;
    /* 0x10 */ float mUnidentified010;
    /* 0x14 */ int mUnidentified014;
    /* 0x18 */ bool mPositiveX;
}; // total size: 0x1C

extern "C"
{
    float lbl_806DD038 = 0.2f;
    UnidentifiedManager_801AE0C4* lbl_806E1620[2];
}

extern "C" void fn_801AE0C4()
{
    for (int i = 0; i < 2; ++i)
    {
        bool positiveX
            = g_pTeams[i]->GetOtherNet()->GetGoalLineX() > 0.0f;
        lbl_806E1620[i]
            = new (nlMalloc(sizeof(UnidentifiedManager_801AE0C4), 8, false))
                UnidentifiedManager_801AE0C4(positiveX);
    }
}

extern "C" void fn_801AE184()
{
    for (int i = 0; i < 2; ++i)
    {
        if (lbl_806E1620[i] != 0)
        {
            delete lbl_806E1620[i];
            lbl_806E1620[i] = 0;
        }
    }
}

extern "C" void fn_801AE1FC()
{
    bool active = nlTaskManager::m_pInstance->mCurrentState == 2;
    for (int i = 0; i < 2; ++i)
    {
        float value = active ? g_pTeams[i]->mUnidentified00C : 0.0f;
        UnidentifiedManager_801AE0C4* manager = lbl_806E1620[i];
        float progress = value / lbl_806DD038;
        if (progress > 1.0f)
        {
            progress = 1.0f;
        }
        manager->mUnidentified010 = progress;

        nlListIterator<DrawableObject*> iterator = manager->mObjects.Begin();
        while (iterator.IsValid())
        {
            DrawableObject* object = iterator.Current();
            float fraction = (float)(int)object->m_uObjectFlags
                           / (float)(manager->mUnidentified014 + 1);
            if (fraction <= manager->mUnidentified010)
            {
                object->mUnidentified074 = 1;
            }
            else
            {
                object->mUnidentified074 = 0;
            }
            iterator.Next();
        }
    }
}

UnidentifiedManager_801AE0C4::~UnidentifiedManager_801AE0C4()
{
    mObjects.Clear();
}

extern "C" void fn_801AE400(DrawableObject* object)
{
    for (int i = 0; i < 2; ++i)
    {
        float x = object->GetWorldMatrix().m41;
        UnidentifiedManager_801AE0C4* manager = lbl_806E1620[i];
        bool positiveX = x > 0.0f;
        if (manager->mPositiveX == positiveX)
        {
            manager->mObjects.AddEnd(object);

            nlListIterator<DrawableObject*> iterator
                = manager->mObjects.Begin();
            while (iterator.IsValid())
            {
                int objectFlags = (int)iterator.Current()->m_uObjectFlags;
                if (objectFlags > manager->mUnidentified014)
                {
                    manager->mUnidentified014 = objectFlags;
                }
                iterator.Next();
            }
        }
    }
}
