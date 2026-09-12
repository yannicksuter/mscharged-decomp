#include "Game/Task/TransitionTask.h"

#include "Game/Ball.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/BasicStadium.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Character.h"
#include "Game/CharacterTemplate.h"
#include "Game/Drawable/DrawableCharacter.h"
#include "Game/OverlayManager.h"
#include "Game/Game.h"
#include "Game/NisPlayer.h"
#include "Game/PadActions.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "Game/Render/Wiper.h"
#include "Game/ReplayManager.h"
#include "Game/Team.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlTask.h"
#include "types.h"
#include "unclassified/tu_80284A58.h"

// Charged keeps the predecessor's transition-manager surface but replaces
// several of its subsystem calls. Everything still address-named below lives
// in a translation unit that has not been reconstructed yet.

extern unsigned char g_JaapAndJacksNastyHackBecauseWeDoNotKnowDifferenceBetweenPausePauseAndPostGamePause;

void TransitionTask::Initialize()
{
}

static inline void ClearCharacterEffectsTexturing()
{
    int i;
    cGame* pGame;

    for (i = 0; i < 10; i++)
    {
        if (g_pCharacters[i] != NULL)
        {
            g_pCharacters[i]->ResetEffects();
        }
    }

    pGame = g_pGame;
    if (pGame != NULL)
    {
        pGame->mUnidentified49C.mEvent10.Deliver();
        DrawableCharacter::RenderAllCharacters();
    }
}

void TransitionTask::StateTransition(u32 from, u32 to)
{
    int i;

    nlTaskManager::m_pInstance->mLocked = true;

    bool bNISLighting;
    if (to & 0x10)
    {
        bNISLighting = true;
    }
    else if (to == 1 && (from & 0x10))
    {
        bNISLighting = true;
    }
    else
    {
        bNISLighting = false;
    }
    DrawableCharacter::sCameraRelativeLighting = bNISLighting;

    if (to == 4)
    {
        UpdateMonkeyState(1);
    }

    if (to == 0x10)
    {
        NisPlayer::Instance()->fn_8027D11C();
    }

    if (BasicStadium::GetCurrentStadium() != NULL)
    {
        if (to == 0x10 && GetPresentation()->mUnidentified164)
        {
            BasicStadium::GetCurrentStadium()->SetEffectsActive(0x37, 1);
        }
        else
        {
            BasicStadium::GetCurrentStadium()->SetEffectsActive(0x37, 0);
        }
    }

    fn_80285714(GetPresentation(), from, to);

    if (to == 2)
    {
        UpdateMonkeyState(0);

        if (from != 1 && from != 0x20)
        {
            ReplayManager::Instance()->PrepareForRecording();
        }

        if (!g_JaapAndJacksNastyHackBecauseWeDoNotKnowDifferenceBetweenPausePauseAndPostGamePause)
        {
            if ((from & 0x18) || (from == 1 && (nlTaskManager::m_pInstance->mPreviousState & 0x18)))
            {
                NisPlayer::Instance()->Reset();
                Wiper::Instance().Reset();
                WorldDarkening::Instance().fn_801AF550();
            }
        }
        else
        {
            g_JaapAndJacksNastyHackBecauseWeDoNotKnowDifferenceBetweenPausePauseAndPostGamePause = false;
        }
    }

    if ((from == 2 && to != 1) || (from == 1 && to != 2))
    {
        if (g_pBall != NULL)
        {
            g_pBall->KillBlurHandler();
        }
    }

    if (g_pOverlayManager != NULL)
    {
        static_cast<OverlayManager*>(g_pOverlayManager)->HandleStateTransition(from, to);
    }

    if ((to & 0x18) || to == 0x20000)
    {
        if (from != 1 && to != 0x20000)
        {
            {
                UnidentifiedPresentationState* presentation = GetPresentation();
                presentation->mLetterBoxEnabled = true;
            }

            for (i = 0; i < 2; i++)
            {
                g_pTeams[i]->fn_800A7998();
            }

            ClearCharacterEffectsTexturing();

            g_pGame->ResetPowerups(false);
            gNPCManager->fn_801ABF8C();
            lbl_806E12C8->ResetEffects();
        }
    }
    else if ((from & 0x18) || (from == 1 && (nlTaskManager::m_pInstance->mPreviousState & 0x18)))
    {
        if (to != 1 && to != 4)
        {
            UnidentifiedPresentationState* presentation = GetPresentation();
            presentation->mLetterBoxEnabled = false;
            presentation->mLetterBoxDuration = 0.0f;

            ClearCharacterEffectsTexturing();

            g_pGame->ResetPowerups(false);
            gNPCManager->fn_801ABF8C();
            lbl_806E12C8->ResetEffects();
        }
        else if (to == 1)
        {
            if (g_pGame != NULL && g_pGame->m_eGameState == 3)
            {
                UnidentifiedPresentationState* presentation = GetPresentation();
                presentation->mLetterBoxEnabled = false;
                presentation->mLetterBoxDuration = 0.0f;
            }
        }
    }

    if (to == 8)
    {
        cCameraManager::PushWorldUpVector();
    }

    if (from == 8)
    {
        cCameraManager::PopWorldUpVector();
    }

    nlTaskManager::m_pInstance->mLocked = false;
}

TransitionTask gTransitionTask;

