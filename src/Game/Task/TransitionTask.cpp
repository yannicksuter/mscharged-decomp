#include "Game/Task/TransitionTask.h"

#include "Game/Ball.h"
#include "Game/BasicStadium.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Character.h"
#include "Game/CharacterTemplate.h"
#include "Game/Drawable/DrawableCharacter.h"
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

class BaseGameSceneManager;

// Charged keeps the predecessor's transition-manager surface but replaces
// several of its subsystem calls. Everything still address-named below lives
// in a translation unit that has not been reconstructed yet.

// Singleton returned by fn_80284A58. Only the fields this unit reads are
// modelled; the letter-box names come from the predecessor's Presentation.
struct UnidentifiedPresentationState
{
    char mUnidentified000[0xC0];
    /* 0xC0 */ float mLetterBoxDuration;
    /* 0xC4 */ bool mLetterBoxEnabled;
    char mUnidentified0C5[0x9F];
    /* 0x164 */ bool mUnidentified164;
};

extern "C" {
void fn_80278A00(BasicStadium*, int, int);
UnidentifiedPresentationState* fn_80284A58();
void fn_80285714(UnidentifiedPresentationState*, u32, u32);
void fn_800A7998(cTeam*);
void fn_801E23A4(BaseGameSceneManager*, u32, u32);
}

extern unsigned char g_JaapAndJacksNastyHackBecauseWeDoNotKnowDifferenceBetweenPausePauseAndPostGamePause;
extern BaseGameSceneManager* lbl_806E1860;

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
        pGame->mUnidentified49C.mEvent10.UnidentifiedDeliver();
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
        if (to == 0x10 && fn_80284A58()->mUnidentified164)
        {
            fn_80278A00(BasicStadium::GetCurrentStadium(), 0x37, 1);
        }
        else
        {
            fn_80278A00(BasicStadium::GetCurrentStadium(), 0x37, 0);
        }
    }

    fn_80285714(fn_80284A58(), from, to);

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

    if (lbl_806E1860 != NULL)
    {
        fn_801E23A4(lbl_806E1860, from, to);
    }

    if ((to & 0x18) || to == 0x20000)
    {
        if (from != 1 && to != 0x20000)
        {
            {
                UnidentifiedPresentationState* presentation = fn_80284A58();
                presentation->mLetterBoxEnabled = true;
            }

            for (i = 0; i < 2; i++)
            {
                fn_800A7998(g_pTeams[i]);
            }

            ClearCharacterEffectsTexturing();

            g_pGame->ResetPowerups(false);
            lbl_806E1608->fn_801ABF8C();
            lbl_806E12C8->ResetEffects();
        }
    }
    else if ((from & 0x18) || (from == 1 && (nlTaskManager::m_pInstance->mPreviousState & 0x18)))
    {
        if (to != 1 && to != 4)
        {
            UnidentifiedPresentationState* presentation = fn_80284A58();
            presentation->mLetterBoxEnabled = false;
            presentation->mLetterBoxDuration = 0.0f;

            ClearCharacterEffectsTexturing();

            g_pGame->ResetPowerups(false);
            lbl_806E1608->fn_801ABF8C();
            lbl_806E12C8->ResetEffects();
        }
        else if (to == 1)
        {
            if (g_pGame != NULL && g_pGame->m_eGameState == 3)
            {
                UnidentifiedPresentationState* presentation = fn_80284A58();
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

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
