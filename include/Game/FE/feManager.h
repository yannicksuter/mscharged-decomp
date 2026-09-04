#ifndef _FEMANAGER_H_
#define _FEMANAGER_H_

enum eFEState
{
    eFE_INVALID = -1,
};

class FrontEnd
{
public:
    static bool Initialize();
    static void Destroy();
    static void Reset();
    static void Update(float fTimeDelta);
    static void ReturnToFE();

    static eFEState m_feStateCurrent;
    static eFEState m_feStatePending;
    static bool m_bGameOver;
};

#endif // _FEMANAGER_H_
