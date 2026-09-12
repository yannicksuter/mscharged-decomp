#include "Game/TweakCallback.h"
#include "NL/nlList.h"

#include "Game/UnidentifiedStaticStorage.h"

static bool sTweakCallbacksUninitialized = true;
static TweakCallback* sTweakCallbackHead;
static TweakCallback* sTweakCallbackTail;

TweakCallback::~TweakCallback()
{
    nlListRemoveElement(&sTweakCallbackHead, this, &sTweakCallbackTail);
}

void TweakCallback::Register()
{
    if (sTweakCallbacksUninitialized)
    {
        sTweakCallbackHead = 0;
        sTweakCallbackTail = 0;
        sTweakCallbacksUninitialized = false;
    }

    nlListAddEnd(&sTweakCallbackHead, &sTweakCallbackTail, this);
}

int TweakCallback::GetValueType()
{
    int result = 6;
    if (mUnidentified010)
    {
        result = 7;
    }
    return result;
}
