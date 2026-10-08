#include "Game/Render/AttackSideIndicators.h"

#include "Game/Render/StadiumWorldObjects.h"
#include "Game/Net.h"
#include "Game/Team.h"
#include "NL/nlList.h"
#include "NL/nlTask.h"
#include "Game/UnidentifiedStaticStorage.h"

// Stadium drawables that are revealed in index order on the half of the field
// a team attacks. The world loader registers every such object through
// RegisterAttackSideIndicator, and GameRenderTask drives the two sets from
// each team's ball-carrier shot value once the match is running.

float gAttackSideIndicatorFullValue = 0.2f;

class AttackSideIndicatorSet
{
public:
    AttackSideIndicatorSet(bool positiveX)
        : mProgress(0.0f)
        , mMaxIndex(-1)
        , mPositiveX(positiveX)
    {
    }

    virtual ~AttackSideIndicatorSet();

    void Update(float value)
    {
        float progress = value / gAttackSideIndicatorFullValue;
        if (progress > 1.0f)
        {
            progress = 1.0f;
        }
        mProgress = progress;

        nlListIterator<StadiumAttackSideIndicator*> iterator = mObjects.Begin();
        while (iterator.IsValid())
        {
            StadiumAttackSideIndicator* object = iterator.Current();
            float fraction = (float)(int)object->m_nIndex
                           / (float)(mMaxIndex + 1);
            if (fraction <= mProgress)
            {
                object->m_nVisible = 1;
            }
            else
            {
                object->m_nVisible = 0;
            }
            iterator.Next();
        }
    }

    void UpdateMaxIndex()
    {
        nlListIterator<StadiumAttackSideIndicator*> iterator = mObjects.Begin();
        while (iterator.IsValid())
        {
            int index = (int)iterator.Current()->m_nIndex;
            if (index > mMaxIndex)
            {
                mMaxIndex = index;
            }
            iterator.Next();
        }
    }

    /* 0x04 */ nlListContainer<StadiumAttackSideIndicator*> mObjects;
    /* 0x10 */ float mProgress;
    /* 0x14 */ int mMaxIndex;
    /* 0x18 */ bool mPositiveX;
}; // total size: 0x1C

AttackSideIndicatorSet* gAttackSideIndicatorSets[2];

void CreateAttackSideIndicators()
{
    for (int i = 0; i < 2; ++i)
    {
        bool positiveX
            = g_pTeams[i]->GetOtherNet()->GetGoalLineX() > 0.0f;
        gAttackSideIndicatorSets[i]
            = new (nlMalloc(sizeof(AttackSideIndicatorSet), 8, false))
                AttackSideIndicatorSet(positiveX);
    }
}

void DestroyAttackSideIndicators()
{
    for (int i = 0; i < 2; ++i)
    {
        if (gAttackSideIndicatorSets[i] != 0)
        {
            delete gAttackSideIndicatorSets[i];
            gAttackSideIndicatorSets[i] = 0;
        }
    }
}

void UpdateAttackSideIndicators()
{
    bool active = nlTaskManager::m_pInstance->mCurrentState == 2;
    for (int i = 0; i < 2; ++i)
    {
        gAttackSideIndicatorSets[i]->Update(
            active ? g_pTeams[i]->mfAttackIndicatorProgress : 0.0f);
    }
}

AttackSideIndicatorSet::~AttackSideIndicatorSet()
{
    mObjects.Clear();
}

void RegisterAttackSideIndicator(StadiumAttackSideIndicator* object)
{
    AttackSideIndicatorSet* set;
    for (int i = 0; i < 2; ++i)
    {
        float x = object->GetWorldMatrix()->m41;
        set = gAttackSideIndicatorSets[i];
        if (set->mPositiveX == (x > 0.0f))
        {
            set->mObjects.AddEnd(object);
            gAttackSideIndicatorSets[i]->UpdateMaxIndex();
        }
    }
}
