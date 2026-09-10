#include "Game/SH/SHChooseSidekicks.h"

#include "Game/DB/GameProgress.h"
#include "Game/FE/feAsyncImage.h"
#include "Game/FE/tlInstance.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/GameInfo.h"
#include "Game/SH/SHNavigation.h"

/**
 * Offset/Address/Size: 0x0 | 0x802284B8 | size: 0x3F0
 */
ChooseSidekicksSceneV2::ChooseSidekicksSceneV2(ChooseCaptainsSceneV2::SceneType sceneType, ScreenMovement movement)
    : mUnidentified1C(false)
    , mMovement(movement)
    , mSceneType(sceneType)
    , mUnidentified4A(false)
    , mUnidentified4B(false)
    , mUnidentified4C(false)
    , mUnidentified4D(false)
    , mUnidentified1918(0)
    , mUnidentified1950(false)
    , mUnidentified1954(0)
    , mUnidentified19F8(false)
{
    int i;

    mUnidentified20[0] = -1;
    mUnidentified20[1] = -1;

    for (i = 0; i < 8; ++i)
    {
        mUnidentifiedE8[i].mContext = (void*)i;
        mUnidentifiedE8[i].mSpeakerEnabled = false;
    }

    for (i = 0; i < 6; ++i)
    {
        mUnidentified688[0][i].mContext = (void*)i;
        mUnidentified688[0][i].mSpeakerEnabled = false;
        mUnidentifiedAC0[0][i].mContext = (void*)i;
        mUnidentifiedAC0[0][i].mSpeakerEnabled = false;
        mUnidentifiedA0[0][i] = false;
    }

    for (i = 0; i < 2; ++i)
    {
        mUnidentifiedEF8[i].mContext = (void*)i;
        mUnidentifiedEF8[i].mSpeakerEnabled = false;
    }

    for (i = 0; i < 2; ++i)
    {
        mUnidentified1060[i].mContext = (void*)i;
        mUnidentified1060[i].mSpeakerEnabled = false;
    }

    mUnidentified28[0] = -1;
    mUnidentified28[1] = -1;
    mUnidentified30[0] = -1;
    mUnidentified30[1] = -1;
    mUnidentified48[0] = false;
    mUnidentified48[1] = false;
    mUnidentified11C8.mSpeakerEnabled = false;

    mBackButton.SetPopScene(false);

    if (GameInfoManager::Instance()->IsInMode3())
    {
        mUnidentified40[0] = g_pCupManager->unknown_0x8A28;
        mUnidentified40[1] = -1;
    }
    else if (GameInfoManager::Instance()->mIsOnlineMode != 0)
    {
        mUnidentified40[0] = GameInfoManager::Instance()->GetTeam(0);
        mUnidentified40[1] = -1;
    }
    else
    {
        mUnidentified40[0] = GameInfoManager::Instance()->GetTeam(0);
        mUnidentified40[1] = GameInfoManager::Instance()->GetTeam(1);
    }

    for (int side = 0; side < 2; ++side)
    {
        for (i = 0; i < 8; ++i)
        {
            mUnidentified1958[side][i] = 0;
            mUnidentified1998[side][i] = 0;
            mUnidentified19D8[side][i] = true;
            mUnidentified19E8[side][i] = true;
        }
    }
}

/**
 * Offset/Address/Size: 0x3F0 | 0x802288A8 | size: 0x198
 */
ChooseSidekicksSceneV2::~ChooseSidekicksSceneV2()
{
    GetNavigationScene()->mTimer->m_bVisible = false;

    for (int i = 0; i < 8; ++i)
    {
        for (int side = 0; side < 2; ++side)
        {
            if (mUnidentified1958[side][i] != 0)
            {
                delete mUnidentified1958[side][i];
                mUnidentified1958[side][i] = 0;
            }

            if (mUnidentified1998[side][i] != 0)
            {
                delete mUnidentified1998[side][i];
                mUnidentified1998[side][i] = 0;
            }
        }
    }
}

int ChooseSidekicksSceneV2::GetSide(unsigned long pad)
{
    if (mUnidentified20[0] == pad)
    {
        return 0;
    }

    if (mUnidentified20[1] == pad)
    {
        return 1;
    }

    return -1;
}

/**
 * Offset/Address/Size: 0x3074 | 0x8022B52C | size: 0xD4
 */
void ChooseSidekicksSceneV2::fn_8022B52C(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mUnidentified20[which] != -1 || side != -1)
    {
        return;
    }

    mUnidentified18FC[which]->SetActiveSlide("over", true, false);
    FEAudio::PlayAnimAudioEvent(0x970D6164, 0, 0, 1);

    for (int i = 0; i < 4; ++i)
    {
        mUnidentifiedEF8[which].SetPointerState(2, i);
    }

    mUnidentified50[which].fn_801DCB28();
}

/**
 * Offset/Address/Size: 0x3148 | 0x8022B600 | size: 0xE8
 */
void ChooseSidekicksSceneV2::fn_8022B600(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mUnidentified20[which] != -1 || side != -1)
    {
        return;
    }

    if (!mUnidentifiedEF8[which].HasOtherPointerState(1, index))
    {
        mUnidentified18FC[which]->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF35, 0, 0, 1);
    }

    mUnidentifiedEF8[which].PlayHoverFeedback(index);
    mUnidentifiedEF8[which].SetPointerState(1, index);
}

/**
 * Offset/Address/Size: 0x2F48 | 0x8022B400 | size: 0x94
 */
void ChooseSidekicksSceneV2::fn_8022B400(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    bool group = which >= 3;
    int slot = !group ? which : which - 3;
    int side = GetSide(index);

    if (mUnidentified20[group] != -1 || side != -1)
    {
        return;
    }

    mUnidentified688[group][slot].SetPointerState(0, index);
    mUnidentifiedAC0[group][slot].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x2D7C | 0x8022B234 | size: 0x1CC
 */
void ChooseSidekicksSceneV2::fn_8022B234(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int group = which >= 3;
    int slot = group ? which - 3 : which;
    int side = GetSide(index);

    if (mUnidentified20[group] != -1 || side != -1)
    {
        return;
    }

    if (!mUnidentified688[group][slot].HasOtherPointerState(1, index))
    {
        if (mUnidentifiedA0[group][slot])
        {
            mUnidentified18E4[group][slot]->SetActiveSlide("over", true, false);
        }
        else
        {
            mUnidentified18CC[group][slot]->SetActiveSlide("over", true, false);
            FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified18CC[group][slot],
                nlStringLowerHash("over"), nlStringLowerHash("CHANGE"), 0, 0, 0, 0)
                ->SetActiveSlide("Slide1", true, false);
        }

        FEAudio::PlayAnimAudioEvent(0x23628A1D, 0, 0, 1);
    }

    mUnidentified688[group][slot].SetPointerState(1, index);
    mUnidentifiedAC0[group][slot].SetPointerState(1, index);
    mUnidentified688[group][slot].PlayHoverFeedback(index);
}

/**
 * Offset/Address/Size: 0x2FDC | 0x8022B494 | size: 0x98
 */
void ChooseSidekicksSceneV2::fn_8022B494(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    bool group = which >= 3;
    int slot = !group ? which : which - 3;
    int side = GetSide(index);

    if (mUnidentified20[group] != -1 || side != -1)
    {
        return;
    }

    if (mUnidentified688[group][slot].GetPointerState(index) != 0)
    {
        return;
    }

    fn_8022B234(index, context);
}

/**
 * Offset/Address/Size: 0x3230 | 0x8022B6E8 | size: 0xD8
 */
void ChooseSidekicksSceneV2::fn_8022B6E8(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mUnidentified20[which] != -1 || side != -1)
    {
        return;
    }

    if (!mUnidentifiedEF8[which].HasOtherPointerState(1, index))
    {
        mUnidentified18FC[which]->SetActiveSlide("off", true, false);
    }

    mUnidentifiedEF8[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x3308 | 0x8022B7C0 | size: 0xF4
 */
void ChooseSidekicksSceneV2::fn_8022B7C0(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mUnidentified20[which] != -1 || side != -1)
    {
        return;
    }

    mUnidentified1904[which]->SetActiveSlide("over", true, false);
    FEAudio::PlayAnimAudioEvent(0x970D6164, 0, 0, 1);

    for (int i = 0; i < 4; ++i)
    {
        mUnidentified1060[which].SetPointerState(2, i);
    }

    mCaptainComponents[which].fn_801E0B20(false);
    mUnidentified48[which] = false;
    mUnidentified50[which].fn_801DCC28();
}

/**
 * Offset/Address/Size: 0x34E4 | 0x8022B99C | size: 0xD8
 */
void ChooseSidekicksSceneV2::fn_8022B99C(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mUnidentified20[which] != -1 || side != -1)
    {
        return;
    }

    if (!mUnidentified1060[which].HasOtherPointerState(1, index))
    {
        mUnidentified1904[which]->SetActiveSlide("off", true, false);
    }

    mUnidentified1060[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x392C | 0x8022BDE4 | size: 0xB0
 */
void ChooseSidekicksSceneV2::fn_8022BDE4(int index, void* context)
{
    if (mUnidentified20[0] != -1 || mUnidentified20[1] != -1)
    {
        return;
    }

    if (!mUnidentified11C8.HasOtherPointerState(1, index))
    {
        mUnidentified1918->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }

    mUnidentified11C8.SetPointerState(1, index);
    mUnidentified11C8.PlayHoverFeedback(index);
}

/**
 * Offset/Address/Size: 0x39DC | 0x8022BE94 | size: 0x8C
 */
void ChooseSidekicksSceneV2::fn_8022BE94(int index, void* context)
{
    if (mUnidentified20[0] != -1 || mUnidentified20[1] != -1)
    {
        return;
    }

    if (!mUnidentified11C8.HasOtherPointerState(1, index))
    {
        mUnidentified1918->SetActiveSlide("off", true, false);
    }

    mUnidentified11C8.SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x33FC | 0x8022B8B4 | size: 0xE8
 */
void ChooseSidekicksSceneV2::fn_8022B8B4(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mUnidentified20[which] != -1 || side != -1)
    {
        return;
    }

    if (!mUnidentified1060[which].HasOtherPointerState(1, index))
    {
        mUnidentified1904[which]->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF35, 0, 0, 1);
    }

    mUnidentified1060[which].PlayHoverFeedback(index);
    mUnidentified1060[which].SetPointerState(1, index);
}

/**
 * Offset/Address/Size: 0x3A68 | 0x8022BF20 | size: 0xCC
 */
void ChooseSidekicksSceneV2::fn_8022BF20(int index, void* context)
{
    if (mUnidentified20[0] != -1 || mUnidentified20[1] != -1)
    {
        return;
    }

    if (mUnidentified11C8.GetPointerState(index) == 0)
    {
        fn_8022BDE4(index, context);
    }
}
