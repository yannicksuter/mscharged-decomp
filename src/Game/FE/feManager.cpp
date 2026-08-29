#include "Game/FE/feManager.h"

bool FrontEnd::Initialize()
{
    Reset();
    return true;
}

void FrontEnd::Destroy()
{
    m_feStateCurrent = eFE_INVALID;
    m_feStatePending = eFE_INVALID;
}
