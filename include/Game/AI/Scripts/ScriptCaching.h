#ifndef GAME_AI_SCRIPTS_SCRIPT_CACHING_H
#define GAME_AI_SCRIPTS_SCRIPT_CACHING_H

#include "Game/AI/FuzzyVariant.h"
#include "NL/nlAVLTree.h"

extern unsigned char lbl_806DF568;

class ScriptQuestionCache
{
public:
    ScriptQuestionCache()
        : mQuestionCacheMap(16, 16)
    {
    }

    ~ScriptQuestionCache()
    {
        FreeBlocks();
    }

    void FreeBlocks()
    {
        Clear();
        mQuestionCacheMap.GetAllocator()->FreeBlocks();
    }

    void Clear()
    {
        mQuestionCacheMap.Clear();
        mCacheHits = 0;
        mTotalLookups = 0;
    }

    unsigned char Lookup(
        unsigned long hash, UnidentifiedVariant_80054AB8& returnVal,
        const char* name)
    {
        UnidentifiedVariant_80054AB8* pValue;

        if (!lbl_806DF568)
        {
            return 0;
        }

        mTotalLookups++;
        if (mQuestionCacheMap.FindGet(hash, &pValue))
        {
            mCacheHits++;
            returnVal = *pValue;
            return 1;
        }
        return 0;
    }

    nlAVLTreeSlotPool<unsigned long, UnidentifiedVariant_80054AB8,
        DefaultKeyCompare<unsigned long> > mQuestionCacheMap;
    int mTotalLookups;
    int mCacheHits;
};

extern ScriptQuestionCache lbl_805842EC;

#endif // GAME_AI_SCRIPTS_SCRIPT_CACHING_H
