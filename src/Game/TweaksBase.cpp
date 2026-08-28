#include "Game/TweaksBase.h"

#include "NL/nlString.h"

TweaksBase::TweaksBase(const char* param_1)
{
    mszFileName[0] = 0;
    nlStrNCpy<char>(mszFileName, param_1, 0x3F);
}

TweaksBase::~TweaksBase()
{
}
