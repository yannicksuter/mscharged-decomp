#ifndef GAME_AI_SCRIPTS_SCRIPT_CACHING_H
#define GAME_AI_SCRIPTS_SCRIPT_CACHING_H

#include "Game/AI/FuzzyVariant.h"
#include "NL/nlAVLTree.h"

class ScriptQuestionCache
{
public:
    void Clear()
    {
        mQuestionCacheMap.Clear();
        mCacheHits = 0;
        mTotalLookups = 0;
    }

    nlAVLTreeSlotPool<unsigned long, UnidentifiedVariant_80054AB8,
        DefaultKeyCompare<unsigned long> > mQuestionCacheMap;
    int mTotalLookups;
    int mCacheHits;
};

extern ScriptQuestionCache lbl_805842EC;

#endif // GAME_AI_SCRIPTS_SCRIPT_CACHING_H
