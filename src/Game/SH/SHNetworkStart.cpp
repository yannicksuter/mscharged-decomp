#include "Game/SH/SHNetworkStart.h"

#include "Game/FE/feFinder.h"
#include "Game/NetworkSession.h"
#include "Game/FE/tlComponentInstance.h"
#include "NL/nlString.h"
#include "Game/UnidentifiedStaticStorage.h"

void NetworkStartScene::DeselectMenuItem(TLComponentInstance* component)
{
    component->SetActiveSlide("off", true, false);
    component->Update(0.0f);
}

void NetworkStartScene::SelectMenuItem(TLComponentInstance* component)
{
    component->SetActiveSlide("on", true, false);
    component->Update(0.0f);
}

void NetworkStartScene::SetActionButtons(int state)
{
    TLSlide* activeSlide = mPresentation->m_currentSlide;
    unsigned long buttonsHash = nlStringLowerHash("BUTTONS");
    unsigned long layerHash = nlStringLowerHash("Layer");
    TLComponentInstance* buttons = FEFinder<TLComponentInstance, 2>::Find(
        activeSlide,
        layerHash,
        buttonsHash,
        0,
        0,
        0,
        0);

    bool visible = true;
    if (state == 0)
    {
        buttons->SetActiveSlide("A AND B", true, false);
    }
    else if (state == 1)
    {
        buttons->SetActiveSlide("A", true, false);
    }
    else if (state == 2)
    {
        buttons->SetActiveSlide("B", true, false);
    }
    else if (state == -1)
    {
        visible = false;
    }
    buttons->m_bVisible = visible;
}

NetworkStartScene::~NetworkStartScene()
{
    LANLobby* lobby = g_pNetworkSessionBase->GetTransport();
    if (lobby != 0)
    {
        lobby->SetLobbyListener(0);
    }
}

NetworkStartScene::NetworkStartScene()
    : mState(0)
    , mUnidentified238(false)
{
    gNetworkStartWaitingForDialog = false;
    gNetworkStartResetRequested = false;
    for (int i = 0; i < 7; ++i)
    {
        mPlayerText[i] = 0;
        mPlayerNames[i][0] = 0;
    }
}

void ResumeNetworkStart()
{
    gNetworkStartWaitingForDialog = false;
}

void ResetNetworkStart()
{
    gNetworkStartWaitingForDialog = false;
    gNetworkStartResetRequested = true;
}
