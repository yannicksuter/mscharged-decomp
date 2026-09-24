#include "Game/SH/SHChooseCaptains.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/fePointer.inl"
#include "Game/FE/fePresentation.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/GameSceneManager.h"
#include "Game/NetworkDraft.h"
#include "Game/NetworkSession.h"
#include "Game/SH/SHNavigation.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

static int lbl_8051CE60[12] = { 0, 5, 3, 6, 4, 7, 1, 8, 2, 9, 10, 11 };

/**
 * Offset/Address/Size: 0x0 | 0x80222098 | size: 0x468
 */
ChooseCaptainsSceneV2::ChooseCaptainsSceneV2(SceneType sceneType, ScreenMovement movement)
    : mUnidentified1C(false)
    , mSceneType(sceneType)
    , mMovement(movement)
    , mUnidentified4A(false)
    , mUnidentified4B(false)
    , mUnidentified4C(false)
    , mUnidentified4D(false)
    , mUnidentified4E(false)
    , mUnidentified1314(0)
    , mUnidentified137C(false)
    , mUnidentified1380(0)
{
    mUnidentified28[0] = -1;
    mUnidentified28[1] = -1;
    mUnidentified132C[0] = 0;
    mUnidentified132C[1] = 0;

    mUnidentified920[0].mContext = (void*)0;
    mUnidentified920[0].mSpeakerEnabled = false;
    mUnidentified920[1].mContext = (void*)1;
    mUnidentified920[1].mSpeakerEnabled = false;

    mUnidentifiedBF0.mSpeakerEnabled = false;

    mUnidentifiedA88[0].mContext = (void*)0;
    mUnidentifiedA88[0].Disable();
    mUnidentifiedA88[1].mContext = (void*)1;
    mUnidentifiedA88[1].Disable();

    mUnidentified30[0] = -1;
    mUnidentified30[1] = -1;
    mUnidentified38[0] = false;
    if (GameInfoManager::Instance()->IsInMode3() || GameInfoManager::Instance()->mIsOnlineMode != 0)
    {
        mUnidentified38[1] = true;
    }
    else
    {
        mUnidentified38[1] = false;
    }
    mUnidentified3A[0] = false;
    mUnidentified3A[1] = false;
    mUnidentified3C[0] = false;
    mUnidentified3C[1] = false;
    mUnidentified40[0] = -1;
    mUnidentified40[1] = -1;
    mUnidentified48[0] = movement == SCREEN_BACK;
    mUnidentified48[1] = movement == SCREEN_BACK;

    mBackButton.SetPopScene(false);
    if (mSceneType == ST_STRIKER_CUP)
    {
        mBackButton.SetPushBackScene(false);
    }

    if (movement == SCREEN_BACK)
    {
        if (mSceneType == ST_STRIKER_CUP)
        {
            int team = g_pCupManager->unknown_0x8A28;
            mUnidentified38[0] = true;
            mUnidentified40[0] = team;
        }
        else
        {
            mUnidentified40[0] = GameInfoManager::Instance()->GetTeam(0);
            mUnidentified38[0] = true;
            if (GameInfoManager::Instance()->mIsOnlineMode == 0)
            {
                int team = GameInfoManager::Instance()->GetTeam(1);
                mUnidentified38[1] = true;
                mUnidentified40[1] = team;
            }
        }
    }

    for (int i = 0; i < 12; ++i)
    {
        mCaptainButtons[i].mContext = (void*)i;
        mCaptainButtons[i].mSpeakerEnabled = false;

        if (mUnidentified40[0] == lbl_8051CE60[i])
        {
            mUnidentified30[0] = i;
        }
        else if (mUnidentified40[1] == lbl_8051CE60[i])
        {
            mUnidentified30[1] = i;
        }
    }
}

/**
 * Offset/Address/Size: 0x468 | 0x80222500 | size: 0xE8
 */
ChooseCaptainsSceneV2::~ChooseCaptainsSceneV2()
{
    TLInstance* timer = GetNavigationScene()->GetTimer();
    timer->m_bVisible = false;
}

int ChooseCaptainsSceneV2::GetSide(unsigned long pad)
{
    if (mUnidentified28[0] == pad)
    {
        return 0;
    }

    if (mUnidentified28[1] == pad)
    {
        return 1;
    }

    return -1;
}

/**
 * Offset/Address/Size: 0x2EE0 | 0x80224F78 | size: 0xC8
 */
void ChooseCaptainsSceneV2::fn_80224F78(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (GetSide(index) == -1)
    {
        return;
    }

    if (!mCaptainButtons[which].HasOtherPointerState(1, index))
    {
        mUnidentified12D4[which]->SetActiveSlide("off", true, false);
    }

    mCaptainButtons[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x33EC | 0x80225484 | size: 0x118
 */
void ChooseCaptainsSceneV2::fn_80225484(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (mUnidentified28[which] != -1 || GetSide(index) != -1)
    {
        return;
    }

    if (!mUnidentified920[which].HasOtherPointerState(1, index))
    {
        mUnidentified132C[which]->SetActiveSlide("over", true, false);

        if (mUnidentified48[which])
        {
            FEAudio::PlayAnimAudioEvent(0xAA73EF35, 0, 0, 1);
        }
        else
        {
            FEAudio::PlayAnimAudioEvent(0x50204AFA, 0, 0, 1);
        }
    }

    mUnidentified920[which].SetPointerState(1, index);
    mUnidentified920[which].PlayHoverFeedback(index);
}

/**
 * Offset/Address/Size: 0x3504 | 0x8022559C | size: 0xD8
 */
void ChooseCaptainsSceneV2::fn_8022559C(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (mUnidentified28[which] != -1 || GetSide(index) != -1)
    {
        return;
    }

    if (!mUnidentified920[which].HasOtherPointerState(1, index))
    {
        mUnidentified132C[which]->SetActiveSlide("off", true, false);
    }

    mUnidentified920[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x3D10 | 0x80225DA8 | size: 0xB0
 */
void ChooseCaptainsSceneV2::fn_80225DA8(int index, void* context)
{
    if (!mUnidentified38[0] || !mUnidentified38[1])
    {
        return;
    }

    if (!mUnidentifiedBF0.HasOtherPointerState(1, index))
    {
        mUnidentified1314->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }

    mUnidentifiedBF0.SetPointerState(1, index);
    mUnidentifiedBF0.PlayHoverFeedback(index);
}

/**
 * Offset/Address/Size: 0x3DC0 | 0x80225E58 | size: 0x8C
 */
void ChooseCaptainsSceneV2::fn_80225E58(int index, void* context)
{
    if (!mUnidentified38[0] || !mUnidentified38[1])
    {
        return;
    }

    if (!mUnidentifiedBF0.HasOtherPointerState(1, index))
    {
        mUnidentified1314->SetActiveSlide("off", true, false);
    }

    mUnidentifiedBF0.SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x35DC | 0x80225674 | size: 0x158
 */
void ChooseCaptainsSceneV2::fn_80225674(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (mUnidentified28[which] != -1 || GetSide(index) != -1)
    {
        return;
    }

    if (mUnidentified920[which].GetPointerState(index) == 0 && mUnidentified28[which] == -1 && GetSide(index) == -1)
    {
        if (!mUnidentified920[which].HasOtherPointerState(1, index))
        {
            mUnidentified132C[which]->SetActiveSlide("over", true, false);

            if (mUnidentified48[which])
            {
                FEAudio::PlayAnimAudioEvent(0xAA73EF35, 0, 0, 1);
            }
            else
            {
                FEAudio::PlayAnimAudioEvent(0x50204AFA, 0, 0, 1);
            }
        }

        mUnidentified920[which].SetPointerState(1, index);
        mUnidentified920[which].PlayHoverFeedback(index);
    }
}

/**
 * Offset/Address/Size: 0x3734 | 0x802257CC | size: 0xEC
 */
void ChooseCaptainsSceneV2::fn_802257CC(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mUnidentified28[which] != -1 || side != -1 || !mUnidentified38[which] || mUnidentified3A[which])
    {
        return;
    }

    mUnidentified130C[which]->SetActiveSlide("down", true, false);

    for (int i = 0; i < 4; ++i)
    {
        mUnidentifiedA88[which].SetPointerState(0, i);
    }

    mUnidentified3A[which] = true;
    mCaptainComponents[which].fn_801E0B20(true);
}

/**
 * Offset/Address/Size: 0x3820 | 0x802258B8 | size: 0xF4
 */
void ChooseCaptainsSceneV2::fn_802258B8(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (mUnidentified28[which] != -1 || GetSide(index) != -1 || !mUnidentified38[which] || mUnidentified3A[which])
    {
        return;
    }

    if (!mUnidentifiedA88[which].HasOtherPointerState(1, index))
    {
        mUnidentified130C[which]->SetActiveSlide("over", true, false);
    }

    mUnidentifiedA88[which].SetPointerState(1, index);
}

/**
 * Offset/Address/Size: 0x3914 | 0x802259AC | size: 0xF4
 */
void ChooseCaptainsSceneV2::fn_802259AC(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (mUnidentified28[which] != -1 || GetSide(index) != -1 || !mUnidentified38[which] || mUnidentified3A[which])
    {
        return;
    }

    if (!mUnidentifiedA88[which].HasOtherPointerState(1, index))
    {
        mUnidentified130C[which]->SetActiveSlide("off", true, false);
    }

    mUnidentifiedA88[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x3A08 | 0x80225AA0 | size: 0x138
 */
void ChooseCaptainsSceneV2::fn_80225AA0(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (mUnidentified28[which] != -1 || GetSide(index) != -1 || !mUnidentified38[which] || mUnidentified3A[which])
    {
        return;
    }

    if (mUnidentifiedA88[which].GetPointerState(index) == 0)
    {
        fn_802258B8(index, context);
    }
}

/**
 * Offset/Address/Size: 0x3E4C | 0x80225EE4 | size: 0xCC
 */
void ChooseCaptainsSceneV2::fn_80225EE4(int index, void* context)
{
    if (!mUnidentified38[0] || !mUnidentified38[1])
    {
        return;
    }

    if (mUnidentifiedBF0.GetPointerState(index) == 0)
    {
        fn_80225DA8(index, context);
    }
}

/**
 * Offset/Address/Size: 0x2C4C | 0x80224CE4 | size: 0x4C
 */
void ChooseCaptainsSceneV2::fn_80224CE4(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (side == -1)
    {
        return;
    }

    if (mUnidentified30[side] == which)
    {
        return;
    }

    fn_80224D30(index, which);
}

/**
 * Offset/Address/Size: 0x2C98 | 0x80224D30 | size: 0x248
 */
void ChooseCaptainsSceneV2::fn_80224D30(int index, unsigned long which)
{
    int side = GetSide(index);

    if (side == -1)
    {
        return;
    }

    int other = !side;
    int captain = lbl_8051CE60[which];

    if ((captain == 9 && !IsBowserJrUnlocked()) || (captain == 10 && !IsDiddyKongUnlocked())
        || (captain == 11 && !IsPeteyUnlocked()))
    {
        return;
    }

    if (mUnidentified30[other] == which && mUnidentified38[other])
    {
        return;
    }

    if (NetworkDraft::Instance()->mState != NET_DRAFT_IDLE
        && NetworkDraft::Instance()->IsCaptainTaken(lbl_8051CE60[which]))
    {
        return;
    }

    mUnidentified30[side] = which;
    int selectedCaptain = lbl_8051CE60[which];

    if (mSceneType == ST_STRIKER_CUP)
    {
        mCaptainComponents[1].fn_801E0280(6);
        mCaptainComponents[1].fn_801DEDD0(selectedCaptain, index, 1);
    }

    mCaptainComponents[side].fn_801DEDD0(selectedCaptain, index, 1);

    if (!mCaptainButtons[which].HasOtherPointerState(1, index))
    {
        mUnidentified12D4[which]->SetActiveSlide("over", true, false);
    }

    mCaptainButtons[which].SetPointerState(1, index);
    mCaptainButtons[which].PlayHoverFeedback(index);

    switch (index)
    {
    case 0:
        FEAudio::PlayAnimAudioEvent(0xA183DBCD, 0, 0, 1);
        break;
    case 1:
        FEAudio::PlayAnimAudioEvent(0xA183DBCE, 0, 0, 1);
        break;
    case 2:
        FEAudio::PlayAnimAudioEvent(0xA183DBCF, 0, 0, 1);
        break;
    case 3:
        FEAudio::PlayAnimAudioEvent(0xA183DBD0, 0, 0, 1);
        break;
    }
}

/**
 * Offset/Address/Size: 0x3B40 | 0x80225BD8 | size: 0x1D0
 */
void ChooseCaptainsSceneV2::fn_80225BD8(int index, void* context)
{
    if (!mUnidentified38[0] || !mUnidentified38[1])
    {
        return;
    }

    mUnidentified1314->SetActiveSlide("down", true, false);

    for (int i = 0; i < 4; ++i)
    {
        mUnidentifiedBF0.SetPointerState(0, i);
    }

    mUnidentifiedBF0.Disable();
    mUnidentified4E = true;

    FEAudio::PlayAnimAudioEvent(0x9F9BF00F, 0, 0, 1);
    FEAudio::PlayAnimAudioEvent(0x2A10C1C3, 0, 0, 1);

    mUnidentified1380 = 2;
    GetNavigationScene()->HideButtons();

    mUnidentified1324->SetActiveSlide("out", true, false);
    mUnidentified1328->SetActiveSlide("out", true, false);
    mUnidentified1334->SetActiveSlide("out", true, false);

    if (mSceneType == ST_STRIKER_CUP)
    {
        g_pCupManager->unknown_0x8A28 = mUnidentified40[0];
    }
    else
    {
        GameInfoManager::Instance()->SetTeam(0, mUnidentified40[0]);
        if (GameInfoManager::Instance()->mIsOnlineMode == 0)
        {
            GameInfoManager::Instance()->SetTeam(1, mUnidentified40[1]);
        }
    }
}

/**
 * Offset/Address/Size: 0x1C70 | 0x80223D08 | size: 0x80
 */
bool fn_80223D08(int pad)
{
    if (IsOnlineRankedMatch())
    {
        return pad == gOnlineLocalControllerIndices[0];
    }

    if (NetworkDraft::Instance()->mCurrentDrafterIsGuest)
    {
        return pad == gOnlineLocalControllerIndices[1];
    }

    return pad == gOnlineLocalControllerIndices[0];
}

/**
 * Offset/Address/Size: 0x60A8 | 0x80228140 | size: 0x5C
 */
void ChooseCaptainsSceneV2::fn_80228140()
{
    mUnidentified1C = false;
    FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, 1);
    GameSceneManager::Instance()->Push((SceneList)0x28, SCREEN_BACK, true);
}

/**
 * Offset/Address/Size: 0x5B34 | 0x80227BCC | size: 0x1DC
 */
void ChooseCaptainsSceneV2::fn_80227BCC(int which)
{
    TLTextInstance* offText = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified1304[which],
        nlStringLowerHash("off"), nlStringLowerHash("Group"), nlStringLowerHash("select text"), 0, 0, 0);
    TLTextInstance* overText = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified1304[which],
        nlStringLowerHash("over"), nlStringLowerHash("Group"), nlStringLowerHash("select text"), 0, 0, 0);
    TLTextInstance* downText = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified1304[which],
        nlStringLowerHash("down"), nlStringLowerHash("Group"), nlStringLowerHash("select text"), 0, 0, 0);

    if (!mUnidentified3C[which])
    {
        offText->SetStringId("CHANGE_CAPTAIN");
        overText->SetStringId("CHANGE_CAPTAIN");
        downText->SetStringId("CHANGE_CAPTAIN");
        mUnidentified3C[which] = true;
    }
    else
    {
        offText->SetStringId("SELECT");
        overText->SetStringId("SELECT");
        downText->SetStringId("SELECT");
        mUnidentified3C[which] = false;
    }
}

/**
 * Offset/Address/Size: 0x1B00 | 0x80223B98 | size: 0x170
 */
void ChooseCaptainsSceneV2::fn_80223B98(int value)
{
    TLSlide* slide = mPresentation->GetActiveSlide();

    FEFinder<TLTextInstance, 3>::FindOrDefault<TLSlide>(slide, "Layer", "TimerText")->m_bVisible = false;

    TLInstance* timer = GetNavigationScene()->GetTimer();

    if (value == -1)
    {
        timer->m_bVisible = false;
    }
    else
    {
        char text[8];

        timer->m_bVisible = true;
        nlSNPrintf(text, 8, "%d", value);
        nlStrToWcs(text, mTimerText, 8);
        ((TLTextInstance*)timer)->SetString(mTimerText);
    }
}
