#ifndef _FEMANAGER_H_
#define _FEMANAGER_H_

#include "Game/FE/feInput.h"

class cAnimCamera;

enum eFEState
{
    eFE_INVALID = -1,
    eFE_START_SCREEN = 0,
    eFE_PAUSE = 1,
    eFE_SHOW_WINNER = 2,
    eFE_PRE_GAME_START = 3,
    eFE_INGAME = 4,
    eFE_END_GAME = 5,
    eFE_WAIT_FOR_LOAD = 6,
    eFE_WAIT_USER_END_GAME_INPUT = 7,
    eFE_PROCESS_MENU_INPUT = 8,
};

class FrontEnd
{
public:
    enum MenuEnterType
    {
        MET_INVALID = -1,
        MET_PAUSE = 0,
        MET_CHOOSESIDES = 1,
        MET_CONNECTIONLOST = 2,
        MET_SYNCERROR = 3,
        MET_QUEUEOVERFLOW = 4,
        MET_END = 5,
    };

    static bool Initialize();
    static void Destroy();
    static void Reset();
    static void SetControllerState();
    static void EnterStartScreen(bool bStraightToKickoff);
    static void ExitWinnerScreen();
    static void EnterMenuState(MenuEnterType menuType);
    static void ExitMenuState();
    static void Update(float fTimeDelta);
    static void UpdateForGame(float fDeltaT);
    static void UpdateForDemoMode(float fDeltaT);
    static void ReturnToFE();
    static void OnGetReadyForKickoff();
    static void OnPresentationBypass();
    static void OnGameOver();
    static void PopupNetworkErrorOverlayCallback();

    static eFEState m_feStateCurrent;
    static eFEState m_feStatePending;
    static eFEState m_feStatePrevious;
    static unsigned int m_lastTaskState;
    static cAnimCamera* m_pPauseMenuCamera;
    static eFEINPUT_PAD m_hitStartPad;
    static MenuEnterType m_menuType;
    static bool m_bGameOver;
    static bool m_bInPauseMenuState;
    static float m_fDemoTimeElapsed;
    static float m_pauseDelay;
    static unsigned char m_ctrlConnectedState[4];
};

#endif // _FEMANAGER_H_
